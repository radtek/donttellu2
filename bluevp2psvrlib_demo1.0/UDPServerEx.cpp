#include "bluevp2psvrdef.h"
#include <typeinfo>
#include <process.h>

namespace BLUEV{
	namespace BLUEVNET{

		using BLUEVBASE::BluevError;
		
		unsigned WINAPI	UDPServerEx_ThreadRecv_Proc(void *pParam)
		{
			int		retval = -1;
			SOCKET	sockfd = INVALID_SOCKET;
			struct	sockaddr_in	fromaddr;
			int		nfromaddrlen = 0, bHasData = 0, nReqLen = 0;

			BLUEV_UDPReqHead		*pReq = NULL;
			char					*pszThreadName = "UDPEx recv线程";
			UDPServerEx				*pServer = (UDPServerEx *)pParam;
			IServiceStatus			&svcStatus = pServer->svcStatus;
			UDPContext				*pUDPCtx = NULL;

			pServer->svcStatus.incThreadCnt();

			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);	// high priority

			char	szBuf[100] = {0};
			sprintf(szBuf, "%s %d started. \r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			sockfd = pServer->svrSocket;
			nfromaddrlen = sizeof(fromaddr);
			
			pServer->bPrepared = true;

			do 
			{
				do
				{
					SocketHelper::BLUEV_SOCK_WaitDataToRead(&sockfd, 1, 3, 0, &bHasData);
				}
				while(!svcStatus.isWillStop() && !bHasData);

				if(svcStatus.isWillStop())
					break;

				//内存池已满，放弃此消息
				if(0 > pServer->_memPool.getObj(pUDPCtx) || pUDPCtx == NULL)
				{
					std::cout << "内存池已满，放弃此消息.\r\n";
					continue;
				}

				try
				{
					UDPServiceParamContext	&paramCtx = pUDPCtx->paramCtx;
					UDPConnection			&conn = pUDPCtx->conn;
					
					conn.nSockfd = sockfd;

					nReqLen = recvfrom(sockfd, (char *)paramCtx.bnReqBuf, sizeof(paramCtx.bnReqBuf), 0, (struct sockaddr *)&fromaddr, &nfromaddrlen);
					if(nReqLen < sizeof(BLUEV_UDPReqHead))
						throw BluevError(__FILE__, __LINE__);

					pReq = (BLUEV_UDPReqHead *)(paramCtx.bnReqBuf);
					if(!pReq->isValid())
						throw BluevError(__FILE__, __LINE__);

					if(UTIL_CONV::Char2To_uint2(pReq->nDataLen) + UTIL_CONV::Char2To_uint2(pReq->cbSize) != nReqLen)
					{
						sprintf(szBuf, "errlen, recv:%d, headlen:%d, datalen:%d", nReqLen,
							UTIL_CONV::Char2To_uint2(pReq->cbSize),
							UTIL_CONV::Char2To_uint2(pReq->nDataLen));
						
						throw BluevError(__FILE__, __LINE__, szBuf);
					}

					int		opClass = UTIL_CONV::Char2To_uint2(pReq->nOpClass);

					IUDPService	*psvc = pServer->svrMgr.lookup(opClass);
					if(psvc == NULL)
					{
						sprintf(szBuf, "service %d not found\r\n", opClass);
						std::cout << szBuf;
						throw BluevError(__FILE__, __LINE__);
					}

					paramCtx.nReqLen = nReqLen;
					conn.fromaddr = fromaddr;

					//加入请求消息队列
					pServer->_queReq.push_back(pUDPCtx);
					pUDPCtx = NULL;
					
				}
				catch(BluevError &e)
				{
					char	szBuf[100] = {0};
					sprintf(szBuf, "err from:%s %d\r\n", inet_ntoa(fromaddr.sin_addr), ntohs(fromaddr.sin_port));
				
					e.print(szBuf);
#ifdef _DEBUG
					if(pUDPCtx && nReqLen > 0)
					{
						std::string strErr = szBuf;
						strErr = "\r\n" + strErr;

						FILE * fp = fopen(ERRLOG_PATH, "a+");
						if(fp)
						{
							fwrite(strErr.c_str(), strErr.size(), 1, fp);

							UDPServiceParamContext	&paramCtx = pUDPCtx->paramCtx;

							fwrite(paramCtx.bnReqBuf, nReqLen, 1, fp);
							fclose(fp);
						}
					}
#endif
					if(pUDPCtx)
					{
						pServer->_memPool.releaseObj(pUDPCtx);
						pUDPCtx = NULL;
					}

					continue;
				}
				
			}
			while(!svcStatus.isWillStop());

			memset(szBuf, 0, sizeof(szBuf));
			sprintf(szBuf, "%s %d 结束.\r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			pServer->svcStatus.decThreadCnt();

			retval = 0;

			return	unsigned(retval);
		}
		
		unsigned WINAPI	UDPServerEx_ThreadSend_Proc(void *pParam)
		{
			int		retval = -1;
			SOCKET	sockfd = INVALID_SOCKET;
			struct	sockaddr_in	fromaddr;
			int		nfromaddrlen = 0, bHasData = 0, nAnsLen = 0;

			BLUEV_UDPReqHead		*pAns = NULL;
			char					*pszThreadName = "UDPEx send线程";
			UDPServerEx				*pServer = (UDPServerEx *)pParam;
			IServiceStatus			&svcStatus = pServer->svcStatus;
			UDPContext				*pUDPCtx = NULL;

			pServer->svcStatus.incThreadCnt();

			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);	// high priority

			char	szBuf[100] = {0};
			sprintf(szBuf, "%s %d started. \r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			sockfd = pServer->svrSocket;
			nfromaddrlen = sizeof(fromaddr);

			//pServer->bPrepared = true;

			do 
			{
				if(svcStatus.isWillStop())
					break;

				//从应答消息队列里获取
				if(0 > pServer->_queAns.pop_front(pUDPCtx) || pUDPCtx == NULL)
				{
					//应答队列为空
					if(pServer->_queAns.bNoWait)
						Sleep(100);
					continue;
				}

				try
				{
					UDPServiceParamContext	&paramCtx = pUDPCtx->paramCtx;
					UDPConnection			&conn = pUDPCtx->conn;

					sockfd = conn.nSockfd;
					fromaddr = conn.fromaddr;

					if(paramCtx.nAnsLen > 0)
					{
						if(0 > sendto(sockfd, (char *)paramCtx.bnAnsBuf, paramCtx.nAnsLen, 0, (struct sockaddr *)&fromaddr, nfromaddrlen))
						{
							//log send err
							std::cout << "ans err send to ";
							//std::cout << "ans err send to " << inet_ntoa(fromaddr.sin_addr) << ntohs(fromaddr.sin_port) << std::endl;	
						}
					}
					else
					{
						//非法反馈
						throw BluevError(__FILE__, __LINE__);
					}
				}
				catch(BluevError &e)
				{
					e.print();

					if(pUDPCtx)
					{
						pServer->_memPool.releaseObj(pUDPCtx);
						pUDPCtx = NULL;
					}

					continue;
				}

				//释放
				if(pUDPCtx)
				{
					pServer->_memPool.releaseObj(pUDPCtx);
					pUDPCtx = NULL;
				}

			}
			while(!svcStatus.isWillStop());

			memset(szBuf, 0, sizeof(szBuf));
			sprintf(szBuf, "%s %d 结束.\r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			pServer->svcStatus.decThreadCnt();

			retval = 0;

			return	unsigned(retval);
		}

		unsigned WINAPI	UDPServerEx_ThreadWork_Proc(void *pParam)
		{
			int		retval = -1;
			int		nfromaddrlen = 0, nAnsLen = 0;

			BLUEV_UDPReqHead		*pReq = NULL;
			char					*pszThreadName = "UDPEx work线程";
			UDPServerEx				*pServer = (UDPServerEx *)pParam;
			IServiceStatus			&svcStatus = pServer->svcStatus;
			UDPContext				*pUDPCtx = NULL;

			pServer->svcStatus.incThreadCnt();

			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);	// high priority

			char	szBuf[100] = {0};
			sprintf(szBuf, "%s %d started. \r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			//pServer->bPrepared = true;

			do
			{
				if(svcStatus.isWillStop())
					break;

				//从请求消息队列里获取
				if(0 > pServer->_queReq.pop_front(pUDPCtx) || pUDPCtx == NULL)
				{
					//请求队列为空
					if(pServer->_queReq.bNoWait)
						Sleep(100);
					continue;
				}

				try
				{
					UDPServiceParamContext	&paramCtx = pUDPCtx->paramCtx;
					UDPConnection			&conn = pUDPCtx->conn;

					pReq = (BLUEV_UDPReqHead *)(paramCtx.bnReqBuf);
					/*if(!pReq->isValid())
						throw BluevError(__FILE__, __LINE__);

					if(UTIL_CONV::Char2To_uint2(pReq->nDataLen) + UTIL_CONV::Char2To_uint2(pReq->cbSize) != paramCtx.nReqLen)
						throw BluevError(__FILE__, __LINE__);*/

					int		opClass = UTIL_CONV::Char2To_uint2(pReq->nOpClass);

					IUDPService	*psvc = pServer->svrMgr.lookup(opClass);
					if(psvc == NULL)
					{
						sprintf(szBuf, "service %d not found\r\n", opClass);
						std::cout << szBuf;
						throw BluevError(__FILE__, __LINE__);
					}

					psvc->execute(conn, paramCtx);

					//加入应答消息队列
					if(paramCtx.nAnsLen > 0)
					{
						pServer->_queAns.push_back(pUDPCtx);
						pUDPCtx = NULL;
					}
					else
					{
						//不反馈
						if(pUDPCtx)
						{
							pServer->_memPool.releaseObj(pUDPCtx);
							pUDPCtx = NULL;
						}
					}
				}
				catch(BluevError &e)
				{
					e.print();

					if(pUDPCtx)
					{
						pServer->_memPool.releaseObj(pUDPCtx);
						pUDPCtx = NULL;
					}

					continue;
				}

			}
			while(!svcStatus.isWillStop());

			memset(szBuf, 0, sizeof(szBuf));
			sprintf(szBuf, "%s %d 结束.\r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			pServer->svcStatus.decThreadCnt();

			retval = 0;

			return	unsigned(retval);
		}

		int		UDPServerEx::start()
		{
			//准备内存池   1024 * 2K = 2M
			if(0 > _memPool.alloc(MAX_CLIENTS))
				return -1;

			HANDLE		hHandle;
			unsigned	tid;

			hHandle = (HANDLE)_beginthreadex(0, 0, UDPServerEx_ThreadRecv_Proc, this, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			CloseHandle(hHandle);

			hHandle = (HANDLE)_beginthreadex(0, 0, UDPServerEx_ThreadSend_Proc, this, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			CloseHandle(hHandle);

			//工作线程
			hHandle = (HANDLE)_beginthreadex(0, 0, UDPServerEx_ThreadWork_Proc, this, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			CloseHandle(hHandle);

			return	0;
		}
	}
}