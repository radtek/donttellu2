#include "bluevp2psvrdef.h"
#include <process.h>

namespace BLUEV {
	namespace BLUEVNET{

		int	BLUEV_Server(void *pParam)
		{
			if(pParam == NULL)
				return -1;

			TCPServerContext	*pCtx = (TCPServerContext *)pParam;
			ConnectionCounter	*pCounter = (ConnectionCounter *)pCtx->pCounter;

			MainService	ms(pCtx->svcMgr, *pCtx->param);
			ms._logger = pCtx->logger;
			ms._llClientNo = pCtx->llClientNo;
			ms._sin_addr = pCtx->sin_addr;
			ms._sin_port = pCtx->sin_port;
			ms._nSocket = pCtx->nSocket;

			ms.doService();

			return 0;
		}

		unsigned WINAPI	BLUEV_PreForkServer_Proc(void *pParam)
		{
			TCPServer	*pServer = (TCPServer *)pParam;
			if(pServer == NULL)
				return -1;

			int		retval = -1;
			char	*pszThreadName = "server prefork thread";
			char	szBuf[512] = {0};
			TCPServerContext	*pContext = NULL;

			pServer->_svcStatus.incThreadCnt();

			sprintf(szBuf, "%s %d started.\r\n", pszThreadName, GetCurrentThreadId());
			std::cout << (szBuf);

			do
			{
				if(0 > pServer->_queContext.getData(pContext) || pContext == NULL)
				{
					if(pServer->_queContext.bNoWait)
						Sleep(100);

					continue;
				}

				retval = BLUEV_Server(pContext);

				if(pContext)
				{
					delete pContext;
					pContext = NULL;
				}

			} while(!pServer->_svcStatus.isWillStop());

			sprintf(szBuf, "%s %d terminated.\r\n", pszThreadName, GetCurrentThreadId());
			std::cout << (szBuf);

			pServer->_svcStatus.decThreadCnt();

			return	retval;
		}

		unsigned WINAPI BLUEV_Server_Proc(void *pParam)
		{
			if(pParam == NULL)
				return -1;

			TCPServerContext *pCtx = (TCPServerContext *)pParam;

			pCtx->svcStatus.incThreadCnt();

			int retval = BLUEV_Server(pParam);

			ConnectionCounter	*pCounter = (ConnectionCounter *)pCtx->pCounter;
			if(pCounter)
				pCounter->threadTerminate();
			
			pCtx->svcStatus.decThreadCnt();

			if(pCtx)
				delete pCtx;

			return	retval;
		}

		int TCPServerThread::run()
		{
			SOCKET	nsockfd;
			struct	sockaddr_in	addrin;
			int		naddrlen;
			char	szBuf[512] = {0};
			int		retval = -1;
			BLUEV_NetReqHead	reqObj;
			BLUEV_NetAnsHead	ansObj;
			int		tisok;

			_svcStatus.incThreadCnt();
			while(!_svcStatus.isWillStop())
			{
				naddrlen = sizeof(addrin);
				nsockfd = SocketHelper::BLUEV_SOCK_AcceptPeer(_nSocket, (struct sockaddr *)&addrin, &naddrlen);
				if(nsockfd == INVALID_SOCKET)
				{
					if(_svcStatus.isWillStop())
					{
						break;
					}
					else
					{
						std::cout << ("Two many sockets or two many threads, just wait...\r\n");
						Sleep(1000);
						continue;
					}
				}

				_stCounter.clientComing();
				sprintf(szBuf, "Client %lld sock=%d Port=%d Addr=%s T=%ld\r\n",
					_stCounter._llTotalClient, nsockfd, ntohs(addrin.sin_port),
					inet_ntoa(addrin.sin_addr), _stCounter._nCurThreadNum);

				std::cout << szBuf;

				if(_stCounter._nCurThreadNum >= _stParam.threadLimit)
				{
					memset(szBuf, 0, sizeof(szBuf));
					sprintf(szBuf, "tow many threads %d %d\r\n", _stCounter._nCurThreadNum, _stParam.threadLimit);
					std::cout << szBuf;
					//fflush(stdout);
					int	nSecond = 1000;	//ms
					setsockopt(nsockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nSecond, sizeof(int));
					setsockopt(nsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&nSecond, sizeof(int));
					
					SocketHelper::BLUEV_SOCK_Recv(nsockfd, &reqObj, sizeof(reqObj));
					UTIL_CONV::uint2ToChar2(-1, ansObj.nRetVal);
					sprintf(szBuf, "Cur=%d Max=%d\r\n", _stCounter._nCurThreadNum, _stParam.threadLimit);
					std::cout << szBuf;;
					SocketHelper::BLUEV_SOCK_Send(nsockfd, &ansObj, sizeof(ansObj));
					closesocket(nsockfd);
					naddrlen = sizeof(addrin);
					continue;
				}

				TCPServerContext	*pCtx;

				try
				{
					pCtx = new TCPServerContext(_svcStatus, _svcMgr);
				}
				catch (std::bad_alloc * e)
				{
					std::cout << ("allocate memory error\r\n");
					int	nSecond = 1000;	//ms
					setsockopt(nsockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nSecond, sizeof(int));
					setsockopt(nsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&nSecond, sizeof(int));

					SocketHelper::BLUEV_SOCK_Recv(nsockfd, &reqObj, sizeof(reqObj));
					UTIL_CONV::uint2ToChar2(-1, ansObj.nRetVal);
					SocketHelper::BLUEV_SOCK_Send(nsockfd, &ansObj, sizeof(ansObj));
					closesocket(nsockfd);
					naddrlen = sizeof(addrin);
					continue;

				}

				pCtx->logger = _pLogger;
				pCtx->llClientNo = _stCounter._llTotalClient;
				pCtx->sin_addr = addrin.sin_addr;
				pCtx->sin_port = addrin.sin_port;
				pCtx->param = &_stParam;
				pCtx->nSocket = nsockfd;
				pCtx->pCounter = (void *)&_stCounter;

				//fflush(stdout);
				if ( _stParam.preForkService )
				{
					// prefork service.
					if(0 > _queContext.addData(pCtx))
					{
						retval = 0;
						break;
					}
					
					naddrlen = sizeof(addrin);
					continue;
				}
				/* allocate memory for thread */
				//fflush(stdout);
				_stCounter.threadStart();
				tisok = 1;
				unsigned	tid;
				HANDLE		hThread;
				//fflush(stdout);
				hThread = (HANDLE)_beginthreadex(0, 0, BLUEV_Server_Proc, pCtx, 0, &tid);

				if(hThread == INVALID_HANDLE_VALUE)
				{
					std::cout << "create thread failed\r\n";
					
					int	nSecond = 1000;	//ms
					setsockopt(nsockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nSecond, sizeof(int));
					setsockopt(nsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&nSecond, sizeof(int));

					SocketHelper::BLUEV_SOCK_Recv(nsockfd, &reqObj, sizeof(reqObj));
					UTIL_CONV::uint2ToChar2(-1, ansObj.nRetVal);
					
					SocketHelper::BLUEV_SOCK_Send(nsockfd, &ansObj, sizeof(ansObj));
					//NETSCR_SendErrStruct(&pCtx->nSocket);
					closesocket(nsockfd);
					_stCounter.threadTerminate();
					delete	pCtx;
					tisok = 0;
				}
				//fflush(stdout);
				
				if(tisok)
					CloseHandle(hThread);

				naddrlen = sizeof(addrin);
			}

			std::cout << "TCP线程结束\r\n";
			_svcStatus.decThreadCnt();
			return	0;
		}

		unsigned WINAPI BLUEV_Main_Proc(void *pParam)
		{
			IRunnable	*pThread = (IRunnable *)pParam;
			if(pThread == NULL)
				return -1;

			int		retval = pThread->run();

			delete	pThread;

			return 0;
		}

		int TCPServer::open()
		{
			_svrSocket = SocketHelper::BLUEV_SOCK_ServerOpenEx(_nPort, 1);
			if(_svrSocket == INVALID_SOCKET)
				return -1;
			return 0;
		}

		int TCPServer::start()
		{
			//线程池
			if(_param.preForkService)
			{
				for(int i = 0; i < _param.threadLimit; ++i)
				{
					HANDLE		hHandle;
					unsigned	tid;

					hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_PreForkServer_Proc, this, 0, &tid);
					if(hHandle == INVALID_HANDLE_VALUE)
						return -1;

					CloseHandle(hHandle);
				}
			}

			HANDLE		hHandle;
			unsigned	tid;

			TCPServerThread		*pThread = new TCPServerThread(_svrSocket, _svcStatus, _logger, _param, _svrMgr, _queContext);
			if(pThread == NULL)
				return -1;

			hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_Main_Proc, pThread, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			CloseHandle(hHandle);

			return 0;
		}

		int TCPServer::close()
		{
			if(_svrSocket != INVALID_SOCKET)
			{
				closesocket(_svrSocket);
				_svrSocket = INVALID_SOCKET;
			}

			_queContext.stop();

			return 0;
		}
	}
}