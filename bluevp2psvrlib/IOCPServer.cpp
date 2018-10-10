#include "bluevp2psvrdef.h"
#include <process.h>
#include <sstream>

namespace BLUEV {	namespace BLUEVNET{

	unsigned WINAPI	BLUEV_IOCPServer_Proc(void *pParam)
	{
		IOCPServer	*pServer = (IOCPServer *)pParam;
		if(pServer == NULL)
			return -1;

		char	*pszThreadName = "server iocp thread";
		char	szBuf[512] = {0};

		pServer->_svcStatus.incThreadCnt();

		sprintf(szBuf, "%s %d started.\r\n", pszThreadName, GetCurrentThreadId());
		std::cout << (szBuf);

		while(!pServer->_svcStatus.isWillStop())
		{
			DWORD					dwBytesTransferred = 0;
			LPPER_IO_OPERATION_DATA	lpPerIOData = NULL;
			LPPER_HANDLE_DATA		pPerHandle = NULL;
			
			//�ڹ���������ɶ˿ڵ������׽����ϵȴ�I/O���
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

					//�����ڴ˹رգ���check�߳��ظ������ﲻ�رգ��������Ͳ��Ḵ��
					//if(pPerHandle->_nsockfd != INVALID_SOCKET)
					//	closesocket(pPerHandle->_nsockfd);

					pServer->_memPool.releaseObj((IOCPContext *)pPerHandle);
				}

				if(!bRet)
				{
					std::cout << "client closed\r\n";
					//���ܳ�ʱ�Ͽ�
					//�ǳ�

				}
				else
					std::cout << "server closed\r\n";

				continue;
			}

			if(dwBytesTransferred == 0)
			{
				//Ͷ��0�ֽ�
				continue;
			}

			try
			{
				switch(lpPerIOData->_enumOperationType)
				{
				case RECV_POSTED:	//���һ����������
					{
						pServer->_lockRW.lockRead();

						map_iocpbuffer_t::const_iterator	itr;
						itr = pServer->_mapIOBuffer.find(pPerHandle->_nsockfd);
						if(itr != pServer->_mapIOBuffer.end())
						{
							//find reqhead, so this is the data after reqhead
							ServiceParamContext	svcParam;
							BLUEVConnection		conn;
							BLUEV_NetReqHead	&req = conn.req;
							BLUEV_NetAnsHead	&ans = conn.ans;

							conn.nSockfd = pPerHandle->_nsockfd;

							const IOCP_Request	&reqIOCP = itr->second;
							req = reqIOCP.req;

							pServer->_lockRW.unlockRead();

							if(!req.isValid())
								throw BluevError(__FILE__, __LINE__);

							int			opClass = UTIL_CONV::Char2To_uint2(req.nOpClass);
							int			opCode = UTIL_CONV::Char2To_int2(req.nOpCode);
							IService	*service = pServer->_svrMgr.lookup(opClass);
							if(service == NULL)
								throw BluevError(__FILE__, __LINE__);

							int dataLen = UTIL_CONV::Char4To_uint4(req.nDataLen);
							if(dataLen != lpPerIOData->_buffer.len)
								throw BluevError(__FILE__, __LINE__);

							svcParam.nAnsLen = 0;
							svcParam.nReqLen = dataLen;
							svcParam.nPort = ntohs(pPerHandle->_addr.sin_port);

							char	*pIp = inet_ntoa(pPerHandle->_addr.sin_addr);
							memcpy(svcParam.bnIP, pIp, strlen(pIp));

							if(dataLen > 0)
							{
								// ��ȡʣ������ݡ�
								svcParam.pReq = reinterpret_cast<unsigned char *>(lpPerIOData->_buffer.buf);
								if(svcParam.pReq == NULL)
									throw BluevError(__FILE__, __LINE__);
							}

							// ִ���û�����
							int retval = service->execute(conn, svcParam);

							// �ѽ�����͸��ͻ��ˡ�
							UTIL_CONV::uint2ToChar2(retval, ans.nRetVal);
							UTIL_CONV::uint4ToChar4(svcParam.nAnsLen, ans.nDataLen);

							//�ǳ���Ҫ���⴦��
							if(opCode == BLUEVOpClass::OP_TCP_LOGOUT && retval == 0)
							{
								if(pPerHandle)
								{
									pServer->_lockRW.lockWrite();

									map_iocpbuffer_t::const_iterator	itr;
									itr = pServer->_mapIOBuffer.find(pPerHandle->_nsockfd);
									if(itr != pServer->_mapIOBuffer.end())
										pServer->_mapIOBuffer.erase(itr);

									pServer->_lockRW.unlockWrite();

									if(pPerHandle->_nsockfd != INVALID_SOCKET)
									{
										//����ͬ��send���ã������첽Ͷ��ʱ��ǣ��첽send���ʱ����close��
										SocketHelper::BLUEV_SOCK_Send(pPerHandle->_nsockfd, &req, sizeof(req));

										closesocket(pPerHandle->_nsockfd);
									}

									pServer->_memPool.releaseObj((IOCPContext *)pPerHandle);
								}

								continue;
							}

							// ������IOCP BUFFER
							int		nAnsLen = sizeof(ans) + svcParam.nAnsLen;
							WSABUF	buffer;
							if(0 > lpPerIOData->getBuffer(buffer, nAnsLen))
								throw BluevError(__FILE__, __LINE__);

							memcpy(buffer.buf, &ans, sizeof(ans));
							if(svcParam.pAns != NULL && svcParam.nAnsLen > 0)
							{
								memcpy(buffer.buf + sizeof(ans), svcParam.pAns, svcParam.nAnsLen);
							}

							// �ͷ��ڴ�
							svcParam.pReq = NULL;

							if(svcParam.pAns)
							{
								delete[] svcParam.pAns;
								svcParam.pAns = NULL;
							}

							//�����������ɾ������ͷ
							pServer->_lockRW.lockWrite();

							map_iocpbuffer_t::const_iterator	itr;
							itr = pServer->_mapIOBuffer.find(pPerHandle->_nsockfd);
							if(itr != pServer->_mapIOBuffer.end())
								pServer->_mapIOBuffer.erase(itr);

							pServer->_lockRW.unlockWrite();

							//Ͷ��д����
							//memset(&lpPerIOData->overlap, 0, sizeof(OVERLAPPED));

							lpPerIOData->_enumOperationType = SEND_POSTED;

							WSASend(pPerHandle->_nsockfd, &lpPerIOData->_buffer, 1,
								&lpPerIOData->_dwBytesSent,
								lpPerIOData->_nFlags,
								&lpPerIOData->_overlap,
								NULL);
						}
						else
						{
							pServer->_lockRW.unlockRead();

							//not find reqhead, so this is req head
							std::ostringstream	oss;
							BLUEV_NetReqHead	&req = *(BLUEV_NetReqHead *)lpPerIOData->_buffer.buf;

							if(sizeof(req) != lpPerIOData->_buffer.len)
								throw BluevError(__FILE__, __LINE__);

							// У���û�����
							if(!req.isValid())
								throw BluevError(__FILE__, __LINE__, "�Ƿ�ͷ����Ϣ");

							// Ȼ����ò�ͬ�Ĵ��������д���
							int			opClass = UTIL_CONV::Char2To_uint2(req.nOpClass);
							IService	*service = pServer->_svrMgr.lookup(opClass);
							if(service == NULL)
							{
								oss.str("");
								oss << "��Ч���û�����opClass=" << opClass;
								throw BluevError(__FILE__, __LINE__, oss.str().c_str());
							}

							int dataLen = UTIL_CONV::Char4To_uint4(req.nDataLen);
							if(dataLen < 0 || dataLen > 10*1024*1024) //10M
							{
								oss.str("");
								oss << "��Ч�����ݳ��ȣ�length=" << dataLen;
								throw BluevError(__FILE__, __LINE__, oss.str().c_str());
							}

							IOCP_Request	*pReqIOCP = (IOCP_Request *)lpPerIOData->_buffer.buf;

							pServer->_lockRW.lockWrite();

							pServer->_mapIOBuffer[pPerHandle->_nsockfd] = *pReqIOCP;

							pServer->_lockRW.unlockWrite();

							WSABUF	buffer;
							if(0 > lpPerIOData->getBuffer(buffer, dataLen))
								throw BluevError(__FILE__, __LINE__);

							lpPerIOData->_enumOperationType = RECV_POSTED;

							// ����Ͷ�ݽ��ܲ���
							WSARecv(pPerHandle->_nsockfd, &lpPerIOData->_buffer, 1,
								&lpPerIOData->_dwBytesRecved,
								&lpPerIOData->_nFlags,
								&lpPerIOData->_overlap, 
								NULL);
						}
					}

					break;
				case SEND_POSTED:
					{
						WSABUF	buffer;
						if(0 > lpPerIOData->getBuffer(buffer, sizeof(BLUEV_NetReqHead)))
							throw BluevError(__FILE__, __LINE__);

						lpPerIOData->_enumOperationType = RECV_POSTED;

						//	����Ͷ�ݽ��ܲ���
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
			catch(BluevError &e)
			{
				e.print();
				continue;
			}
			
		}

		sprintf(szBuf, "%s %d terminated.\r\n", pszThreadName, GetCurrentThreadId());
		std::cout << (szBuf);

		pServer->_svcStatus.decThreadCnt();

		return 0;
	}

	extern unsigned WINAPI BLUEV_Main_Proc(void *pParam);

	int IOCPServer::open()
	{
		_svrSocket = SocketHelper::BLUEV_SOCK_ServerOpenEx(_nPort, 1);
		if(_svrSocket == INVALID_SOCKET)
			return -1;

		_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
		if(_hIocp == NULL)
			return -1;

		return 0;
	}

	int IOCPServer::start()
	{
		//׼���ڴ��   1024 * 10 * 1K = 10M
		if(0 > _memPool.alloc(IOCP_MAX_CLIENTS))
			return -1;

		//�̳߳�
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

		IOCPServerThread	*pThread = new IOCPServerThread(_svrSocket, _svcStatus, _logger, _param, _svrMgr, _hIocp, _memPool);
		if(pThread == NULL)
			return -1;

		hHandle = (HANDLE)_beginthreadex(0, 0, BLUEV_Main_Proc, pThread, 0, &tid);
		if(hHandle == INVALID_HANDLE_VALUE)
			return -1;

		CloseHandle(hHandle);

		return 0;
	}

	int IOCPServer::close()
	{
		//����IOCP�߳�
		for(int i = 0; i < _param.threadLimit; ++i)
		{
			PostQueuedCompletionStatus(_hIocp, 0xffffffff, 0, NULL);
		}

		//�ȴ�IOCP�߳��˳�
		WaitForMultipleObjects(_param.threadLimit, _phIocpThread, TRUE, INFINITE);

		if(_svrSocket != INVALID_SOCKET)
		{
			closesocket(_svrSocket);
			_svrSocket = INVALID_SOCKET;
		}
		
		CloseHandle(_hIocp);

		return 0;
	}


	int IOCPServerThread::run()
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
				std::cout << "IOCP Server�ڴ������������������.\r\n";
				closesocket(nsockfd);
				continue;
			}

			PER_HANDLE_DATA	&stHandle = pIOCPCtx->_stHandle;

			stHandle._nsockfd = nsockfd;
			stHandle._addr = addrin;

			//����iocp�ͽ���socket
			CreateIoCompletionPort((HANDLE)stHandle._nsockfd, _hIocp, (ULONG_PTR)&stHandle, 0);

			PER_IO_OPERATION_DATA	&stIoOpt = pIOCPCtx->_stIoOpt;

			WSABUF	buffer;
			if(0 > stIoOpt.getBuffer(buffer, sizeof(BLUEV_NetReqHead)))
				continue;

			stIoOpt._enumOperationType = RECV_POSTED;

			WSARecv(nsockfd, &stIoOpt._buffer, 1,
				&stIoOpt._dwBytesRecved,
				&stIoOpt._nFlags,
				&stIoOpt._overlap,
				NULL);
			
		}

		std::cout << "TCP�߳̽���\r\n";
		_svcStatus.decThreadCnt();
		return	0;
	}
}}