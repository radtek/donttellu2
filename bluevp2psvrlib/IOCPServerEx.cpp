#include "bluevp2psvrdef.h"
#include <process.h>
#include <sstream>

namespace BLUEV {	namespace BLUEVNET{

	unsigned WINAPI	BLUEV_IOCPServer_WorkProc(void *pParam)
	{
		while(1)
		{
			//从请求队列取


		}
		return 0;
	}

	unsigned WINAPI	BLUEV_IOCPServerEx_Proc(void *pParam)
	{
		IOCPServerEx	*pServer = (IOCPServerEx *)pParam;
		if(pServer == NULL)
			return -1;

		char	*pszThreadName = "iocpserverex thread";
		char	szBuf[512] = {0};

		pServer->_svcStatus.incThreadCnt();

		sprintf(szBuf, "%s %d started.\r\n", pszThreadName, GetCurrentThreadId());
		std::cout << (szBuf);

		while(!pServer->_svcStatus.isWillStop())
		{
			DWORD					dwBytesTransferred = 0;
			LPPER_IO_OPERATION_DATA	lpPerIOData = NULL;
			LPPER_HANDLE_DATA		pPerHandle = NULL;

			//在关联到此完成端口的所有套接字上等待I/O完成
			BOOL	bRet = GetQueuedCompletionStatus(pServer->_hIocp, &dwBytesTransferred, (PULONG_PTR)&pPerHandle, (LPOVERLAPPED *)&lpPerIOData, WSA_INFINITE);
			if(!bRet || dwBytesTransferred == 0xffffffff)
			{
				DWORD	dwError = GetLastError();
				if(dwError == WAIT_TIMEOUT)
					continue;

				if(pPerHandle)
				{
					pServer->_lockRW.lockWrite();

					map_iocpbuffer_t::const_iterator	itr;
					itr = pServer->_mapIOBuffer.find(pPerHandle->_nsockfd);
					if(itr != pServer->_mapIOBuffer.end())
						pServer->_mapIOBuffer.erase(itr);

					pServer->_lockRW.unlockWrite();

					//不该在此关闭，与check线程重复，这里不关闭，描述符就不会复用
					//if(pPerHandle->_nsockfd != INVALID_SOCKET)
					//	closesocket(pPerHandle->_nsockfd);

					pServer->_memPool.releaseObj((IOCPContext *)pPerHandle);
				}

				if(!bRet)
				{
					std::cout << "client closed\r\n";
					//可能超时断开
					//登出

				}
				else
					std::cout << "server closed\r\n";

				continue;
			}
			
			//投递0字节
			if(dwBytesTransferred == 0)
			{
				//取任务
				IOCP_TASK	task;
				if(0 > pServer->_taskMgr.getTask(pPerHandle->_nsockfd, task))
					continue;

				IOCP_TASKWorker worker(task, pServer->_queTaskRHead, pServer->_queTaskRData);
				worker.doTask();

				continue;
			}

			switch(lpPerIOData->_enumOperationType)
			{
			case RECV_POSTED:	//完成一个接收请求
				{
					IOCP_TASK	task;
					if(0 > pServer->_taskMgr.getTask(pPerHandle->_nsockfd, task))
						continue;

					IOCP_TASKWorker worker(task, pServer->_queTaskRHead, pServer->_queTaskRData);
					worker.finishTask();
					
				}

				break;
			case SEND_POSTED:
				{
					//获取任务状态
					IOCP_TASK	&task;
					IOCP_TASKConvertor	convertor;
					convertor.convert(task);

					//投递读0字节
					WSABUF	buffer;
					if(0 > lpPerIOData->getBuffer(buffer, 0))
						throw BluevError(__FILE__, __LINE__);

					lpPerIOData->_enumOperationType = RECV_POSTED;
					WSARecv(pPerHandle->_nsockfd, &lpPerIOData->_buffer, 1,
						&lpPerIOData->_dwBytesRecved,
						&lpPerIOData->_nFlags,
						&lpPerIOData->_overlap,
						NULL);
				}

				break;
				//case OP_ACCEPT: 
				//	break;
			}
		}

		sprintf(szBuf, "%s %d terminated.\r\n", pszThreadName, GetCurrentThreadId());
		std::cout << (szBuf);

		pServer->_svcStatus.decThreadCnt();

		return 0;
	}
#endif
	extern unsigned WINAPI BLUEV_Main_Proc(void *pParam);

	int IOCPServerEx::open()
	{
		_svrSocket = SocketHelper::BLUEV_SOCK_ServerOpenEx(_nPort, 1);
		if(_svrSocket == INVALID_SOCKET)
			return -1;

		_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if(_hIocp == NULL)
			return -1;

		return 0;
	}

	int IOCPServerEx::start()
	{
		//准备内存池   1024 * 10 * 1K = 10M
		if(0 > _memPool.alloc(IOCP_MAX_CLIENTS))
			return -1;

		//线程池
		if(_param.preForkService)
		{
			_phIocpThread = new HANDLE[_param.threadLimit];
			if(_phIocpThread == NULL)
				return -1;

			memset(_phIocpThread, (int)INVALID_HANDLE_VALUE, _param.threadLimit * sizeof(HANDLE));

			for(int i = 0; i < _param.threadLimit; ++i)
			{
				HANDLE		hHandle;
				unsigned	tid;

				hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_IOCPServer_Proc, this, 0, &tid);
				if(hHandle == INVALID_HANDLE_VALUE)
					return -1;

				_phIocpThread[i] = hHandle;
			}
		}

		HANDLE		hHandle;
		unsigned	tid;

		IOCPServerThreadEx	*pThread = new IOCPServerThreadEx(_svrSocket, _svcStatus, _logger, _param, _svrMgr, _hIocp, _memPool, this);
		if(pThread == NULL)
			return -1;

		hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_Main_Proc, pThread, 0, &tid);
		if(hHandle == INVALID_HANDLE_VALUE)
			return -1;

		CloseHandle(hHandle);

		return 0;
	}

	int IOCPServerEx::close()
	{
		//唤醒IOCP线程
		for(int i = 0; i < _param.threadLimit; ++i)
		{
			PostQueuedCompletionStatus(_hIocp, 0xffffffff, 0, NULL);
		}

		//等待IOCP线程退出
		WaitForMultipleObjects(_param.threadLimit, _phIocpThread, TRUE, INFINITE);

		if(_svrSocket != INVALID_SOCKET)
		{
			closesocket(_svrSocket);
			_svrSocket = INVALID_SOCKET;
		}

		CloseHandle(_hIocp);

		return 0;
	}


	int IOCPServerThreadEx::run()
	{
		SOCKET	nsockfd;
		struct	sockaddr_in	addrin;
		int		naddrlen;
		char	szBuf[512] = {0};
		int		retval = -1;

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
			sprintf(szBuf, "Client %lld sock=%d Port=%d Addr=%s\r\n",
				_stCounter._llTotalClient, nsockfd, ntohs(addrin.sin_port),
				inet_ntoa(addrin.sin_addr));

			std::cout << szBuf;

			IOCPContext		*pIOCPCtx = NULL;
			if(0 > _memPool.getObj(pIOCPCtx) || pIOCPCtx == NULL)
			{
				std::cout << "IOCP Server内存池已满，放弃此连接.\r\n";
				closesocket(nsockfd);
				continue;
			}

			PER_HANDLE_DATA	&stHandle = pIOCPCtx->_stHandle;

			stHandle._nsockfd = nsockfd;
			stHandle._addr = addrin;

			//关联iocp和接收socket
			CreateIoCompletionPort((HANDLE)stHandle._nsockfd, _hIocp, (ULONG_PTR)&stHandle, 0);

			//增加任务TASKREADHEAD
			_server._taskMgr.addTask(IOCP_TASK::IOCPTASK_READHEAD, nsockfd, *pIOCPCtx);

			//投递读0字节
			PER_IO_OPERATION_DATA	&stIoOpt = pIOCPCtx->_stIoOpt;
			WSABUF	buffer;
			stIoOpt.getBuffer(buffer, 0);
			stIoOpt._enumOperationType = RECV_POSTED;

			WSARecv(nsockfd, &stIoOpt._buffer, 1,
				&stIoOpt._dwBytesRecved,
				&stIoOpt._nFlags,
				&stIoOpt._overlap,
				NULL);
		}

		std::cout << "TCP线程结束\r\n";
		_svcStatus.decThreadCnt();
		return	0;
	}
}}