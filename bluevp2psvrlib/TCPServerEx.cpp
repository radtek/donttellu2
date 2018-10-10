#include "bluevp2psvrdef.h"
#include <process.h>

namespace BLUEV {
	namespace BLUEVNET{

		int	BLUEV_ServerEx(void *pParam)
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

			//非长连接，关闭socket
			if(!ms._bLongConn)
				pCtx->nSocket = INVALID_SOCKET;

			return 0;
		}

		unsigned WINAPI	BLUEV_PreForkServerEx_Proc(void *pParam)
		{
			TCPServerEx	*pServer = (TCPServerEx *)pParam;
			if(pServer == NULL)
				return -1;

			int		retval = -1;
			char	*pszThreadName = "server preforkEX thread";
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

				retval = BLUEV_ServerEx(pContext);

				//长连接需要再次放回
				if(pContext->nSocket != INVALID_SOCKET)
				{
					pServer->_context._rwLock.lockWrite();
					FD_SET(pContext->nSocket, &pServer->_context._fdClientSets);
					pServer->_context._rwLock.unlockWrite();
				}

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


		int TCPServerThreadEx::run()
		{
			if(!_stParam.preForkService)
				return -1;

			if(_bMajor)
				return runMajor();
			else
				return runMinor();
		}

		int TCPServerThreadEx::runMajor()
		{
			SOCKET	nsockfd;
			struct	sockaddr_in	addrin;
			int		naddrlen;
			char	szBuf[512] = {0};
			int		retval = -1;
			BLUEV_NetReqHead	reqObj;
			BLUEV_NetAnsHead	ansObj;

			if(!_bMajor)
				return -1;

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

				_context._rwLock.lockRead();
				int	nSocketCount = _context._fdClientSets.fd_count;
				_context._rwLock.unlockRead();

				sprintf(szBuf, "Client %lld sock=%d Port=%d Addr=%s S=%ld\r\n",
					_stCounter._llTotalClient, nsockfd, ntohs(addrin.sin_port),
					inet_ntoa(addrin.sin_addr), nSocketCount);
				
				std::cout << szBuf;

				if(nSocketCount >= _stParam.socketLimit)
				{
					memset(szBuf, 0, sizeof(szBuf));
					sprintf(szBuf, "tow many sockets %d %d\r\n", nSocketCount, _stParam.socketLimit);
					std::cout << szBuf;
					//fflush(stdout);
					int	nSecond = 1000;	//ms
					setsockopt(nsockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&nSecond, sizeof(int));
					setsockopt(nsockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&nSecond, sizeof(int));

					SocketHelper::BLUEV_SOCK_Recv(nsockfd, &reqObj, sizeof(reqObj));
					UTIL_CONV::uint2ToChar2(-1, ansObj.nRetVal);
					sprintf(szBuf, "Cur=%d Max=%d\r\n", nSocketCount, _stParam.socketLimit);
					std::cout << szBuf;;
					SocketHelper::BLUEV_SOCK_Send(nsockfd, &ansObj, sizeof(ansObj));
					closesocket(nsockfd);

					continue;
				}

				_context._rwLock.lockWrite();
				//linux
				//_stCounter._nfds = nsockfd++;

				FD_SET(nsockfd, &_context._fdClientSets);
				_context._mapClientNum[nsockfd] = _stCounter._llTotalClient;
				
				_context._rwLock.unlockWrite();
			}

			std::cout << "TCP线程结束\r\n";
			_svcStatus.decThreadCnt();

			return	0;
		}

		int TCPServerThreadEx::runMinor()
		{
			BLUEV_NetReqHead	reqObj;
			BLUEV_NetAnsHead	ansObj;
			int		retval = -1;
			fd_set	fdRead;
			int		nfds;

			FD_ZERO(&fdRead);

			if(_bMajor)
				return -1;

			_svcStatus.incThreadCnt();
			while(!_svcStatus.isWillStop())
			{
				_context._rwLock.lockRead();

				if(_context._fdClientSets.fd_count == 0)
				{
					_context._rwLock.unlockRead();

					Sleep(1000);
					continue;
				}

				fdRead = _context._fdClientSets;
				nfds = _context._nfds;

				_context._rwLock.unlockRead();

				if(0 > (retval = SocketHelper::BLUEV_SOCK_Select((int)nfds, &fdRead, NULL, NULL, 3, 0))) 
					continue;

				if(retval == 0)// time limit reached.
					continue;

				std::map<SOCKET, __int64>::const_iterator	itr;
				
				for(unsigned int i = 0; i < fdRead.fd_count; ++i)
				{
					__int64	llClientNo = 0;
					SOCKET	&nsockfd = fdRead.fd_array[i];

					_context._rwLock.lockRead();
					if(!FD_ISSET(nsockfd, &_context._fdClientSets))
					{
						_context._rwLock.unlockRead();
						continue;
					}

					itr = _context._mapClientNum.find(nsockfd);
					if(itr != _context._mapClientNum.end())
						llClientNo = itr->second;

					_context._rwLock.unlockRead();


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

						break;
					}

					struct	sockaddr_in	addrin;
					int		naddrlen = sizeof(addrin);

					getpeername(nsockfd, (struct sockaddr *)&addrin, &naddrlen);

					pCtx->llClientNo = llClientNo;
					pCtx->logger = _pLogger;
					pCtx->sin_addr = addrin.sin_addr;
					pCtx->sin_port = addrin.sin_port;
					pCtx->param = &_stParam;
					pCtx->nSocket = nsockfd;
					pCtx->pCounter = (void *)&_stCounter;

					//fflush(stdout);
					
					//提前删除，以免后面多次recv导致select唤醒。
					_context._rwLock.lockWrite();
					FD_CLR(nsockfd, &_context._fdClientSets);
					_context._rwLock.unlockWrite();

					// prefork service.
					if(0 > _queContext.addData(pCtx))
					{
						retval = 0;
						break;
					}

					continue;
				}
				
			}

			std::cout << "TCP线程结束\r\n";
			_svcStatus.decThreadCnt();
			
			return	0;
		}

/*
		unsigned WINAPI BLUEV_Main_Proc_MAJOR(void *pParam)
		{
			TCPServerThreadEx	*pThread = (TCPServerThreadEx *)pParam;
			if(pThread == NULL)
				return -1;

			int		retval = pThread->runMajor();

			delete	pThread;

			return 0;
		}

		unsigned WINAPI BLUEV_Main_Proc_MINOR(void *pParam)
		{
			TCPServerThreadEx	*pThread = (TCPServerThreadEx *)pParam;
			if(pThread == NULL)
				return -1;

			int		retval = pThread->runMinor();

			delete	pThread;

			return 0;
		}
		*/

		extern unsigned WINAPI BLUEV_Main_Proc(void *pParam);

		int TCPServerEx::start()
		{
			//线程池
			if(_param.preForkService)
			{
				for(int i = 0; i < _param.threadLimit; ++i)
				{
					HANDLE		hHandle;
					unsigned	tid;

					hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_PreForkServerEx_Proc, this, 0, &tid);
					if(hHandle == INVALID_HANDLE_VALUE)
						return -1;

					CloseHandle(hHandle);
				}
			}

			HANDLE		hHandle;
			unsigned	tid;

			TCPServerThreadEx	*pThread = new TCPServerThreadEx(_svrSocket, _svcStatus, _logger, _param, _svrMgr, _queContext, _context);
			if(pThread == NULL)
				return -1;

			hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_Main_Proc, pThread, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			CloseHandle(hHandle);

			pThread = new TCPServerThreadEx(_svrSocket, _svcStatus, _logger, _param, _svrMgr, _queContext, _context);
			if(pThread == NULL)
				return -1;

			pThread->_bMajor = false;
			hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_Main_Proc, pThread, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			CloseHandle(hHandle);

			return 0;
		}

	}
}