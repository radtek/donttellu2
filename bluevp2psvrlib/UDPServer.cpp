
#include "bluevp2psvrdef.h"
#include <process.h>
#include <iostream>

namespace	BLUEV	{ namespace BLUEVNET {

	using BLUEVNET::SocketHelper;
	using BLUEVNET::BLUEV_UDPReqHead;

	// pParam 是UDPServer结构
	unsigned WINAPI	UDPServer_Thread_Proc(void *pParam)
	{
		int		retval;
		SOCKET	sockfd;
		struct	sockaddr_in	fromaddr;
		int		nfromaddrlen, bHasData, nReqLen;
		
		BLUEV_UDPReqHead		*pReq;
		char					*pszThreadName = "UDP 线程";
		UDPServer				*pServer = (UDPServer *)pParam;
		IServiceStatus			&svcStatus = pServer->svcStatus;
		UDPServiceParamContext	paramCtx;
		UDPConnection			conn;
		//ILogger					*logger = pServer->logger;

		pServer->svcStatus.incThreadCnt();
		retval = 1;

		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);	// high priority

		//GA7PRINT_INFO(L"%s %d started.", pszThreadName, GAFIS_THREAD_GetCurID()); 
		char	szBuf[100] = {0};
		sprintf(szBuf, "%s %d started. \r\n", pszThreadName, GetCurrentThreadId());
		std::cout << szBuf /*<< std::endl*/;

		sockfd = pServer->svrSocket;
		conn.nSockfd = sockfd;
		pServer->bPrepared = true;

		do {
			
			nfromaddrlen = sizeof(fromaddr);

			do {
				SocketHelper::BLUEV_SOCK_WaitDataToRead(&sockfd, 1, 3, 0, &bHasData);
			} while(!svcStatus.isWillStop() && !bHasData );

			if ( svcStatus.isWillStop() ) break;

			//if ( (i=recvfrom(sockfd, buf, nbuflen, flags, fromaddr, fromaddrlen)) == SOCKET_ERROR ) i = -1;
			nReqLen = recvfrom(sockfd, (char *)paramCtx.bnReqBuf, sizeof(paramCtx.bnReqBuf), 0, (struct sockaddr *)&fromaddr, &nfromaddrlen);
			if ( nReqLen< (int)sizeof(BLUEV_UDPReqHead) ) continue;

			pReq = (BLUEV_UDPReqHead *)(paramCtx.bnReqBuf);
			if ( !pReq->isValid() ) continue;

			if ( UTIL_CONV::Char2To_uint2(pReq->nDataLen) + UTIL_CONV::Char2To_uint2(pReq->cbSize) != nReqLen ) continue;

			int		opClass;
			opClass = UTIL_CONV::Char2To_uint2(pReq->nOpClass);
			
			IUDPService	*psvc = pServer->svrMgr.lookup(opClass);
			if ( psvc==NULL )
			{
				sprintf(szBuf, "service %d not found\r\n", opClass);
				std::cout << szBuf;
				continue;
			}

			paramCtx.nReqLen = nReqLen;
			conn.fromaddr = fromaddr;

			psvc->execute(conn, paramCtx);

			if ( paramCtx.nAnsLen>0 )
			{
				//if ( (i=sendto(sockfd, buf, nbuflen, flags, toaddr, toaddrlen))==SOCKET_ERROR ) i = -1;
				if(0 > sendto(sockfd, (char *)paramCtx.bnAnsBuf, paramCtx.nAnsLen, 0, (struct sockaddr *)&fromaddr, nfromaddrlen))
					std::cout << "ans err send to " << inet_ntoa(fromaddr.sin_addr) << ntohs(fromaddr.sin_port) << std::endl;	//log send err
			}
		} while(!svcStatus.isWillStop());

		memset(szBuf, 0, sizeof(szBuf));
		sprintf(szBuf, "%s %d 结束.\r\n", pszThreadName, GetCurrentThreadId());
		std::cout << szBuf /*<< std::endl*/;
		//GA7PRINT_INFO(L"%s %d 结束.", pszThreadName, GAFIS_THREAD_GetCurID());
		pServer->svcStatus.decThreadCnt();
		
		return	unsigned(retval);
	}

	int		UDPServer::open()
	{
		int		retval = -1;

		svrSocket = SocketHelper::BLUEV_SOCKUDP_ServerOpen(port, 0);
		if ( svrSocket==INVALID_SOCKET ) return -1;

		return	1;
	}

	int		UDPServer::start()
	{
		HANDLE		hHandle;
		unsigned	tid;

		hHandle = (HANDLE)_beginthreadex(0, 0, UDPServer_Thread_Proc, this, 0, &tid);
		if(hHandle == INVALID_HANDLE_VALUE)
			return -1;

		CloseHandle(hHandle);

		return	0;
	}
				
	int		UDPServer::close()
	{
		if ( svrSocket != INVALID_SOCKET )
		{
			closesocket(svrSocket);
			svrSocket = INVALID_SOCKET;
		}
		return	0;
	}

}}

