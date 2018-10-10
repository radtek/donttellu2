#pragma once

namespace BLUEV
{
	namespace BLUEVNET
	{
		
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::UTIL_TICK;

		static	int	gs_nBLUEVSOCKINITED = 0;
	
		class BVNETLIB_API	SocketHelper
		{
		public:

#define	SOCKMAJORVERSION	2
#define	SOCKMINORVERSION	2

#define	SERVEROPEN_OPT_USESOCKET		0x1
#define	FRAMESIZE	4096

			static int	BLUEV_SOCK_Init()
			{
				WORD wVersionRequested; 
				WSADATA wsaData; 
				int		err; 
				char	s1[] = "Network subsystem is not ready for network commnuication";
				char	s2[] = "Sock version requested has not been implemnted";
				char	s3[] = "A blocking socket is in progress";
				char	s4[] = "System has reached task limit";
				char	s5[] = "Parameter is invalid";
				char	s6[] = "Version is not what we want";

				if ( gs_nBLUEVSOCKINITED ) return 1;

				wVersionRequested = MAKEWORD(SOCKMAJORVERSION, SOCKMINORVERSION); 

				err = WSAStartup(wVersionRequested, &wsaData); 

				if (err != 0)  {
					/* set error */
					/*switch(err) {
					case	WSASYSNOTREADY:	BLUEV_SOCKERR_SET(GAFISERR_SOCK_INIT, (unsigned char *)s1, sizeof(s1));	break;
					case	WSAVERNOTSUPPORTED:	BLUEV_SOCKERR_SET(GAFISERR_SOCK_INIT, (unsigned char *)s2, sizeof(s2));	break;
					case	WSAEINPROGRESS:	BLUEV_SOCKERR_SET(GAFISERR_SOCK_INIT, (unsigned char *)s3, sizeof(s3));	break;
					case	WSAEPROCLIM:	BLUEV_SOCKERR_SET(GAFISERR_SOCK_INIT, (unsigned char *)s4, sizeof(s4));	break;
					default:		BLUEV_SOCKERR_SET(GAFISERR_SOCK_INIT, (unsigned char *)s5, sizeof(s5));	break;
					}*/
					/* Tell the user that we couldn't find a useable */ 
					/* winsock.dll.     */ 
					return -1; 
				}

				/* Confirm that the Windows Sockets DLL supports 1.1.*/ 
				/* Note that if the DLL supports versions greater */ 
				/* than 1.1 in addition to 1.1, it will still return */ 
				/* 1.1 in wVersion since that is the version we */ 
				/* requested. */ 

				if ( LOBYTE( wsaData.wVersion ) != SOCKMAJORVERSION || 
					HIBYTE( wsaData.wVersion ) != SOCKMINORVERSION ) { 
						/* Tell the user that we couldn't find a useable */ 
						/* winsock.dll. */ 
						WSACleanup();
						//BLUEV_SOCKERR_SET(GAFISERR_SOCK_INIT, (unsigned char *)s6, sizeof(s6));
						return -1; 
				} 

				gs_nBLUEVSOCKINITED = 1;
				return 1;
			}

			static void BLUEV_SOCK_SetAddrN(struct sockaddr_in *addr, int nIPInNetWorkByteOrder, int nPort)
			{
				memset(addr, 0, sizeof(struct sockaddr_in));
				addr->sin_family = AF_INET;
				addr->sin_addr.s_addr = nIPInNetWorkByteOrder;
				addr->sin_port	= htons((u_short)nPort);
			}
		
			static int BLUEV_SOCK_SetOption(SOCKET sockfd, int nLevel, int optname, void *optval, int optlen)
			{
				int	i = 1;

				if ( (i=setsockopt(sockfd, nLevel, optname, (const char*)optval, optlen))==SOCKET_ERROR ) i = -1;
				return	i;
			}

			static int BLUEV_SOCK_ReuseSocket(SOCKET sockfd)
			{
				int		b = 1;

				return	BLUEV_SOCK_SetOption(sockfd, SOL_SOCKET, SO_REUSEADDR, &b, sizeof(b));
			}

			static SOCKET BLUEV_SOCKUDP_ServerOpen(int nPort, int nNetIP)
			{
				SOCKET	sockfd, retval;
				struct sockaddr_in      serveraddr;
				/*struct linger			solig = {1,0}; */

				retval = INVALID_SOCKET;
				if ( !gs_nBLUEVSOCKINITED ) {
					if ( BLUEV_SOCK_Init()<0 ) return retval;
				}

				/* Create a socket for the communications */
				if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0))==INVALID_SOCKET ) {
					return retval;
				}

				if ( nNetIP==0 ) nNetIP = INADDR_ANY;
				BLUEV_SOCK_SetAddrN(&serveraddr, nNetIP, nPort);
				BLUEV_SOCK_ReuseSocket(sockfd);

				/* Bind the address to this socket to make it 'known' */
				if ( bind(sockfd,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)) != 0) {
					closesocket(sockfd);
					return retval;
				}

				return sockfd;
			}
		
			// on windows platform the nfds is ignored
			// on unix platform is the max value of fd+1
			static int BLUEV_SOCK_Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, int seconds, int microseconds)
			{
				struct	timeval	tv, *ptv;

				if ( seconds<0 ) {
					ptv = NULL;
				} else {
					tv.tv_sec = seconds;
					tv.tv_usec = microseconds;
					ptv = &tv;
				}
				seconds = select(nfds, readfds, writefds, exceptfds, ptv);

				if ( seconds==SOCKET_ERROR ) 
				{
					return -1;
				}
				return	seconds;
			}

			// given an array of sockets, wait data to be ready to read, if nSeconds<0 then wait for
			// ever.
			static int BLUEV_SOCK_WaitDataToRead(SOCKET *pfd, int nfdCount, int nSeconds, int nMicroSeconds,
				int *pbHasData)
			{
				int		retval, i;
				SOCKET	msockfd;
				fd_set	fds;

				if ( pbHasData ) {
					memset(pbHasData, 0, sizeof(int)*nfdCount);
				}
				if ( pfd==NULL ) {
					return	-1;
				}
				FD_ZERO(&fds);
				retval = -1;
				msockfd = pfd[0];

				FD_SET(pfd[0], &fds);
				for(i=1; i<nfdCount; i++) {
					FD_SET(pfd[i], &fds);
					if ( msockfd<pfd[i] ) msockfd = pfd[i];
				}

				//win
				msockfd = nfdCount;
				//linux
				//msockfd += 1;

				if ( (retval = BLUEV_SOCK_Select((int)msockfd, &fds, NULL, NULL, nSeconds, nMicroSeconds))<0 ) 
					return -1;

				if ( retval==0 ) return	retval;	// time limit reached.

				if ( pbHasData ) {
					memset(pbHasData, 0, sizeof(int)*nfdCount);
				}
				for(i=0; i<nfdCount; i++) {
					if ( FD_ISSET(pfd[i], &fds) ) {
						if ( pbHasData ) pbHasData[i] = 1;
					}
				}
				return	retval;
			}
		
			static int BLUEV_SOCK_EnsureInited(void)
			{
				if ( gs_nBLUEVSOCKINITED ) return 1;	// ok.
				// not initialized.
				return	BLUEV_SOCK_Init();
			}
			
			//127.0.0.1 => 0100007f  反过来存成int
			// on success return 1, else return -1, ip is in network byte order
			static int BLUEV_SOCK_NameToIP(const char *cServerName, int *nIP)
			{
				int	retval;
				//struct hostent	*hostp;

				retval = -1;
				if ( cServerName==NULL || nIP==NULL || strlen(cServerName)<=0 ) {
					return retval;
				}

				if ( BLUEV_SOCK_EnsureInited()<0 ) 
					return retval;

				if ( (*nIP=inet_addr(cServerName))==INADDR_NONE ) 
					return retval;
				//gethostbyname not safe,use getnameinfo instead
				/*if ( (*nIP=inet_addr(cServerName))!=INADDR_NONE ) return 1;
				else {
					if ((hostp = gethostbyname(cServerName)) != (struct hostent *)NULL && hostp->h_length == sizeof(int) ) {
						memcpy(nIP, hostp->h_addr_list[0], sizeof(int));
					} else {
				
						return retval;
					}
				}*/
				return	1;
			}
		
			static SOCKET BLUEV_SOCKUDP_ClientOpen()
			{
				SOCKET		sockfd;
				SOCKET		retval;
				/*struct linger			solig = {1,0}; */

				retval = INVALID_SOCKET;
				if ( !gs_nBLUEVSOCKINITED ) {
					if ( BLUEV_SOCK_Init()<0 )	return retval;
				}

				if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0))==INVALID_SOCKET ) {
					return retval;
				}

				return sockfd;
			}

			static SOCKET BLUEV_SOCK_ServerOpenEx(int nPort, int nOption)
			{
				SOCKET	sockfd, retval;
				struct sockaddr_in      serveraddr;
				/*struct linger			solig = {1,0}; */

				retval = INVALID_SOCKET;
				//if ( (sockfd=GAFIS_SOCK_OpenSocket())==INVALID_SOCKET ) return retval;
				if ( !gs_nBLUEVSOCKINITED ) {
					if ( BLUEV_SOCK_Init()<0 ) return retval;
				}

				if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET ) {
					return retval;
				}

				/*
				if(setsockopt(sockfd,SOL_SOCKET,SO_LINGER, (char *)&solig,sizeof(struct linger)) != 0 ){
					closesocket(sockfd);
					return -1;
				}
				*/

				BLUEV_SOCK_SetAddrN(&serveraddr, INADDR_ANY, nPort);
				if ( nOption & SERVEROPEN_OPT_USESOCKET ) 
					BLUEV_SOCK_ReuseSocket(sockfd);

				/* Bind the address to this socket to make it 'known' */
				if ( bind(sockfd,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)) != 0) {
					closesocket(sockfd);
					return retval;
				}
			//	SOMAXCONN listen backlog count. //g_stBv.nListenBacklogCnt
				if ( listen(sockfd, 64)!=0 ) {
					closesocket(sockfd);
					return retval;
				}

				//if ( g_glbsockopt.nSvrRecvTimeOut>0 ) BLUEV_SOCK_SetRecvTimeOut(sockfd, g_glbsockopt.nSvrRecvTimeOut);
				//if ( g_glbsockopt.nSvrSendTimeOut>0 ) BLUEV_SOCK_SetSendTimeOut(sockfd, g_glbsockopt.nSvrSendTimeOut);
				return sockfd;
			}
		
			static SOCKET BLUEV_SOCK_AcceptPeer(SOCKET sockfd, struct sockaddr *peer, int *peerlen)
			{
				SOCKET	nsockfd = INVALID_SOCKET;
				int		nSockLen, *pSockLen;

				if ( peerlen==NULL ) {
					pSockLen = NULL;
				} else {
					pSockLen = &nSockLen;
					nSockLen = *peerlen;
				}

				if ( (nsockfd = accept(sockfd, peer, pSockLen))==INVALID_SOCKET )
				{
					if ( peerlen ) *peerlen = (int)*pSockLen;
					
					return nsockfd;
				}
				if ( peerlen ) *peerlen = (int)*pSockLen;
				return nsockfd;
			}

			static int BLUEV_SOCK_SendWithOption(SOCKET sockfd, void *data, int datalen, int flags)
			{
				int     bytes = 0, actbs = 0;
				int     offset = 0;
				char	*cdata = (char *)data;
				char	szBuf[128] = {0}, szInfo[32] = {0};
				int		nErrCode = 0;
				__int64	llTimeUsed = 0;

				if(datalen <= 0)
					return 0;

				if(datalen > 0 && data == NULL)
				{
					sprintf(szBuf, "send data error: Len=%d Pt=%p\r\n", datalen, data);
					std::cout << szBuf;
					return -1;
				}

				offset = 0;
				
				while(offset < datalen)
				{
					bytes = (datalen - offset > FRAMESIZE) ? FRAMESIZE : (datalen - offset);

					llTimeUsed = UTIL_TICK::getTickCount();

					actbs = send(sockfd, cdata + offset, bytes, flags);

					llTimeUsed = UTIL_TICK::getTickCount() - llTimeUsed;
					
					if(actbs <= 0)
					{
						nErrCode = WSAGetLastError();

						SocketHelper::BLUEV_SOCK_GetPeerIPName(sockfd, szInfo);
						sprintf(szBuf, "Send error retval=%d sent=%d total=%d. ip=%s time=%g\r\n",
							actbs, offset, datalen, szInfo, UTIL_TICK::tickToSecond(llTimeUsed));
						std::cout << szBuf;

						WSASetLastError(nErrCode);
						return -1;
					}

					offset += actbs;
				}

				return(datalen);
			}

			static int BLUEV_SOCK_Send(SOCKET sockfd, void *data, int datalen)
			{
				return BLUEV_SOCK_SendWithOption(sockfd, data, datalen, 0);
			}

			// flags may be MSG_PEEK, or MSG_OOB
			static int BLUEV_SOCK_RecvWithOption(SOCKET sockfd, void *data, int datalen, int flags)
			{
				int     bytes = 0, actbs = 0;
				int     offset = 0;
				char	*cdata = (char *)data;
				char	szBuf[256] = {0}, szInfo[32] = {0};
				int		nErrCode = 0;
				__int64	llTimeUsed = 0;

				if(datalen <= 0)
					return 0;
				if(datalen > 0 && data == NULL)
					return -1;

				offset = 0;
				while(offset < datalen)
				{
					bytes = datalen - offset > FRAMESIZE ?  FRAMESIZE : (datalen-offset);

					llTimeUsed = UTIL_TICK::getTickCount();
					actbs = recv(sockfd, cdata + offset, bytes, flags);
					llTimeUsed = UTIL_TICK::getTickCount() - llTimeUsed;
					if(actbs <= 0)
					{
						nErrCode = WSAGetLastError();
						
						SocketHelper::BLUEV_SOCK_GetPeerIPName(sockfd, szInfo);
						sprintf(szBuf, "recvdata error. retval=%d read=%d total=%d. ip=%s time=%g\r\n",
							actbs, offset, datalen, szInfo, UTIL_TICK::tickToSecond(llTimeUsed));
						std::cout << szBuf;

						WSASetLastError(nErrCode);
						return -1;
					}

					offset += actbs;
				}
				
				return(datalen);
			}

			static int BLUEV_SOCK_Recv(SOCKET sockfd, void *data, int datalen)
			{
				return BLUEV_SOCK_RecvWithOption(sockfd, data, datalen, 0);
			}

			// ip in network byte order.
			static int BLUEV_SOCK_GetPeerIP(SOCKET sockfd, int *pnIP)
			{
				struct	sockaddr_in	st_addr;
				int		naddr_len;

				naddr_len = sizeof(st_addr);
				if(getpeername(sockfd, (struct sockaddr *)&st_addr, &naddr_len) == 0)
				{
					*pnIP = st_addr.sin_addr.s_addr;
				}
				else
					return -1;

				return 0;
			}
			
			static int BLUEV_SOCK_GetPeerIPName(SOCKET sockfd, char *pszPeerIPName)
			{
				struct	in_addr		sin_addr;
				int		nIP;

				if(BLUEV_SOCK_GetPeerIP(sockfd, &nIP) < 0)
					return -1;
				
				sin_addr.s_addr = nIP;
				sprintf(pszPeerIPName, "%s", inet_ntoa(sin_addr));
				
				return 0;
			}

			static SOCKET BLUEV_SOCK_OpenSocket()
			{
				SOCKET		sockfd;
				SOCKET		retval;

				retval = INVALID_SOCKET;
				if(!gs_nBLUEVSOCKINITED)
				{
					if(BLUEV_SOCK_Init() < 0)
						return retval;
				}

				if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
					return retval;

				return	sockfd;
			}
		
		};
	
	}
}