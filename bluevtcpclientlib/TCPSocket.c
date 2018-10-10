#include "TCPSocket.h"
#include "base64.h"
#include "cJSON.h"

static int g_TCPServer = -1;
static int g_TCPServerPort = 6214;

static char	g_szSvrIP[16] = {0};
static int		g_nSvrPort = 0;

static volatile int g_bStop = 1;

static P2PConnection		g_ConnSvr ;

static Event	*g_phKeepAlive = NULL;

static LogInContext			g_ctxLogin;

//lock order
pthread_mutex_t	g_mtxLoginCtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	*g_pmtxSocket = NULL;

int SetLoginContext(LogInContext	*pContext)
{
	if(pContext ==  NULL)
		return -1;

	if(pContext == &g_ctxLogin)
		return 0;

	pthread_mutex_lock(&g_mtxLoginCtx);

	g_ctxLogin = *pContext;

	pthread_mutex_unlock(&g_mtxLoginCtx);

	return 0;
}

#define	FRAMESIZE	4096

//#################################################
//socket func
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

	if ( seconds<0 )
	{
		return -1;
	}
	return	seconds;
}

static int BLUEV_SOCK_WaitDataToRead(int *pfd, int nfdCount, int nSeconds, int nMicroSeconds,
	int *pbHasData)
{
	int		retval, i;
	int	msockfd;
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

	msockfd += 1;

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

static int BLUEV_SOCK_NameToIP(const char *cServerName, int *nIP)
{
	int				retval;
	//struct hostent	*hostp;

	retval = -1;
	if ( cServerName==NULL || nIP==NULL || strlen(cServerName)<=0 ) {
		return retval;
	}

	if ( (*nIP=inet_addr(cServerName))==INADDR_NONE )
		return retval;
	//gethostbyname not safe,use getnameinfo instead
	/*
	if ( (*nIP=inet_addr(cServerName))!=INADDR_NONE ) return 1;
	else {
		if ((hostp = gethostbyname(cServerName)) != (struct hostent *)NULL && hostp->h_length == sizeof(int) ) {
			memcpy(nIP, hostp->h_addr_list[0], sizeof(int));
		} else {
			return retval;
		}
	}*/

	return 0;
}

static void BLUEV_SOCK_SetAddrN(struct sockaddr_in *addr, int nIPInNetWorkByteOrder, int nPort)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = nIPInNetWorkByteOrder;
	addr->sin_port	= htons(/*(in_port_t)*/nPort);
}

// ip in network byte order.
static int BLUEV_SOCK_GetPeerIP(int sockfd, int *pnIP)
{
	struct	sockaddr_in	st_addr;
	socklen_t		naddr_len;

	naddr_len = sizeof(st_addr);
	if(getpeername(sockfd, (struct sockaddr *)&st_addr, &naddr_len) == 0)
	{
		*pnIP = st_addr.sin_addr.s_addr;
	}
	else
		return -1;

	return 0;
}

static int BLUEV_SOCK_GetPeerIPName(int sockfd, char *pszPeerIPName)
{
	struct	in_addr		sin_addr;
	int		nIP;

	if(BLUEV_SOCK_GetPeerIP(sockfd, &nIP) < 0)
		return -1;

	sin_addr.s_addr = nIP;
	sprintf(pszPeerIPName, "%s", inet_ntoa(sin_addr));

	return 0;
}

static int BLUEV_SOCK_SetOption(int sockfd, int nLevel, int optname, void *optval, int optlen)
{
	int	i = 1;

	i = setsockopt(sockfd, nLevel, optname, optval, optlen);
	return	i;
}

static int BLUEV_SOCK_ReuseSocket(int sockfd)
{
	int		b = 1;

	return	BLUEV_SOCK_SetOption(sockfd, SOL_SOCKET, SO_REUSEADDR, &b, sizeof(b));
}

static int BLUEV_SOCK_SendWithOption(int sockfd, void *data, int datalen, int flags)
{
	int     bytes = 0, actbs = 0;
	int     offset = 0;
	char	*cdata = (char *)data;
	char	szInfo[32] = {0};
	long long	llTimeUsed = 0;

	if(datalen <= 0)
		return 0;

	if(datalen > 0 && data == NULL)
	{
		printf("send data error: Len=%d Pt=%p\n", datalen, data);

		return -1;
	}

	offset = 0;

	while(offset < datalen)
	{
		bytes = (datalen - offset > FRAMESIZE) ? FRAMESIZE : (datalen - offset);

		llTimeUsed = getTickCount();

		actbs = send(sockfd, cdata + offset, bytes, flags);

		llTimeUsed = getTickCount() - llTimeUsed;

		if(actbs <= 0)
		{
			BLUEV_SOCK_GetPeerIPName(sockfd, szInfo);
			printf("Send error retval=%d sent=%d total=%d. ip=%s time=%g\n",
				actbs, offset, datalen, szInfo, tickToSecond(llTimeUsed));

			return -1;
		}

		offset += actbs;
	}

	return(datalen);
}

static int BLUEV_SOCK_Send(int sockfd, void *data, int datalen)
{
	return BLUEV_SOCK_SendWithOption(sockfd, data, datalen, 0);
}

// flags may be MSG_PEEK, or MSG_OOB
static int BLUEV_SOCK_RecvWithOption(int sockfd, void *data, int datalen, int flags)
{
	int     bytes = 0, actbs = 0;
	int     offset = 0;
	char	*cdata = (char *)data;
	char	szInfo[32] = {0};
	long long	llTimeUsed = 0;

	if(datalen <= 0)
		return 0;
	if(datalen > 0 && data == NULL)
		return -1;

	offset = 0;
	while(offset < datalen)
	{
		bytes = datalen - offset > FRAMESIZE ?  FRAMESIZE : (datalen-offset);

		llTimeUsed = getTickCount();
		actbs = recv(sockfd, cdata + offset, bytes, flags);
		llTimeUsed = getTickCount() - llTimeUsed;
		if(actbs <= 0)
		{
			BLUEV_SOCK_GetPeerIPName(sockfd, szInfo);
			printf("recvdata error. retval=%d read=%d total=%d. ip=%s time=%g\r\n",
				actbs, offset, datalen, szInfo, tickToSecond(llTimeUsed));

			return -1;
		}

		offset += actbs;
	}

	return(datalen);
}

static int BLUEV_SOCK_Recv(int sockfd, void *data, int datalen)
{
	return BLUEV_SOCK_RecvWithOption(sockfd, data, datalen, 0);
}


//thread func
typedef	void *(* BLUEV_THREADPROC_TYPE)(void *);
static int BLUEV_CreateThread(pthread_t *handle,
	int stacksize,
	BLUEV_THREADPROC_TYPE proc,
	void *arg,
	unsigned int nCreationFlag,
	int	*tid
	)
{
	int		retval;
	pthread_attr_t	th_attr;
	size_t	st;

	if ( handle==NULL || tid==NULL ) {
		return -1;
	}

	retval = -1;
	if  ( stacksize <= 0 ) st = 1024*1024;
	else st = stacksize;

	if ( pthread_attr_init(&th_attr)!=0 ) {
		return -1;
	}

	if ( pthread_attr_setstacksize(&th_attr, st)!=0 ) {
		pthread_attr_destroy(&th_attr);
		return -1;
	}
	if ( tid!=NULL ) *tid = 0;
	if ( pthread_create(handle, &th_attr, proc, arg)!=0 ) {
		pthread_attr_destroy(&th_attr);
		return -1;
	}

	retval = 0;

	pthread_attr_destroy(&th_attr);

	return retval;
}

static int BLUEV_CreateEvent(Event **ppEvent)
{
	Event		*pEvent;

	if(ppEvent == NULL)
		return -1;

	pEvent = (Event *)malloc(sizeof(Event));
	if(pEvent == NULL)
	{
		return -1;
	}

	pthread_cond_init(&pEvent->cond, NULL);
	pthread_mutex_init(&pEvent->mutex, NULL);

	*ppEvent = pEvent;

	return 0;
}

static int BLUEV_SetEvent(Event *pEvent)
{
	if(pEvent == NULL)
		return -1;

	pthread_cond_broadcast(&pEvent->cond);	// awaken up all waiting threads.
	if(pthread_cond_signal(&pEvent->cond) == 0)
		return 0;

	return -1;
}

static int BLUEV_ReleaseEvent(Event *pEvent)
{
	if(pEvent == NULL)
		return -1;

	while(pthread_cond_destroy(&pEvent->cond) == EBUSY)
	{
	// awaken up all thres.
		pthread_cond_broadcast(&pEvent->cond);
		sleep(0);
	}
	pthread_mutex_destroy(&pEvent->mutex);

	free(pEvent);
	return 0;
}

static int BLUEV_Event_TimeWait(Event *pEvent, int nSecond)
{
	struct	timespec ts;
	int			retval;

	if(pEvent == NULL)
		return -1;

	if(pthread_mutex_lock(&pEvent->mutex) != 0)
		return -1;

	retval = -1;
	if(nSecond <= 0)
	{
		if(pthread_cond_wait(&pEvent->cond, &pEvent->mutex) == 0)
			return 0;
	}
	else
	{
		memset(&ts, 0, sizeof(ts));
		ts.tv_sec = ((int)time(NULL)) + nSecond;
		retval = pthread_cond_timedwait(&pEvent->cond, &pEvent->mutex, &ts);

		pthread_mutex_unlock(&pEvent->mutex);
	}

	return retval;
}

static int _LogIn()
{
	int					nRetVal = -1;
	char				*pszSessionId = NULL;
	int 				nszSessionIdLen = 0;
	char				*pszJson = NULL;
	int					nJsonSize = 0;
	char				*pAnsData = NULL;
	int					nAnsDataLen = 0;

	BLUEV_NetReqHead	req;
	BLUEV_NetAnsHead	ans;

	if(g_TCPServer < 0 || g_pmtxSocket == NULL || !IsConnect())
		return -1;

	if(IsLogin())
		return 0;

	LogInContext	*pContext = &g_ctxLogin;

	//lock context
	pthread_mutex_lock(&g_mtxLoginCtx);

	//lock socket
	pthread_mutex_lock(g_pmtxSocket);

	InitReqHead(&req);

	uint2ToChar2(OP_TCPCLASS_P2P, req.nOpClass);
	uint2ToChar2(OP_TCP_LOGIN, req.nOpCode);

	req.longConnection = 1;

	if(0 > UserData2Json(pContext, &pszJson) || pszJson == NULL)
		ERR_EXIT();

	nJsonSize = strlen(pszJson);
	if(nJsonSize == 0)
		ERR_EXIT();

	printf("req login json : %s\n", pszJson);

	uint4ToChar4(nJsonSize, req.nDataLen);

	if(0 > BLUEV_SOCK_Send(g_TCPServer, &req, sizeof(req)))
		ERR_EXIT();

	if(0 > BLUEV_SOCK_Send(g_TCPServer, (void *)pszJson, nJsonSize))
		ERR_EXIT();

	if(0 > BLUEV_SOCK_Recv(g_TCPServer, &ans, sizeof(ans)))
		ERR_EXIT();

	if(!IsAnsHeadValid(&ans))
		ERR_EXIT();

	nRetVal = Char2To_uint2(ans.nRetVal);
	if(nRetVal < 0)
		ERR_EXIT();

	nAnsDataLen = Char4To_uint4(ans.nDataLen);
	if(nAnsDataLen <= 0 || nAnsDataLen >= 10*1024*1024)	//10M
		ERR_EXIT();

	pAnsData = (char *)malloc(nAnsDataLen);
	if(pAnsData == NULL)
		ERR_EXIT();

	if(0 > BLUEV_SOCK_Recv(g_TCPServer, pAnsData, nAnsDataLen))
		ERR_EXIT();

	printf("receive tcp login json:%s\n", pAnsData);

	if(0 > Json2Session(pAnsData, &pszSessionId) ||
		pszSessionId == NULL)
		ERR_EXIT();

	nszSessionIdLen = strlen(pszSessionId);
	if(nszSessionIdLen == 0 || nszSessionIdLen > sizeof(g_ConnSvr.stUserId.szSessionId))
		ERR_EXIT();

	memcpy(g_ConnSvr.stUserId.szSessionId, pszSessionId, nszSessionIdLen);
	g_ConnSvr.bLogIn = 1;

	printf("login success, session id:%s\n", g_ConnSvr.stUserId.szSessionId);

	nRetVal = 0;

Finish_Exit:
	if(pAnsData)
		free(pAnsData);

	if(pszSessionId)
		free(pszSessionId);

	if(pszJson)
		free(pszJson);

	pthread_mutex_unlock(g_pmtxSocket);
	pthread_mutex_unlock(&g_mtxLoginCtx);

	return nRetVal;
}

///////////////////////////////////////////////////
//工作线程
void * TCPServerProc(void *pParam)
{
    int								nRetVal, bHasData, nSockfd;
	int								nReqLen;
	TCPServiceParam	ctx;
	TCPConnection		conn;
	//struct						sockaddr_in addr;
	//socklen_t  				nAddrLen;

	BLUEV_NetReqHead	*pReq = &conn.req;
	BLUEV_NetAnsHead	*pAns = &conn.ans;

	if(pParam == NULL)
		return (void *)-1;

	memset(&conn, 0, sizeof(conn));
	nSockfd = *(int *)pParam;
	conn.nSockfd = nSockfd;

	printf("%s %lx started. \n", "TCP work thread", pthread_self());

	do
	{
		memset(&ctx, 0, sizeof(ctx));

		//when not first
			//UNLOCK

		do
		{
			//LOCK

			//select
			nRetVal = BLUEV_SOCK_WaitDataToRead(&nSockfd, 1, 3, 0, &bHasData);
			if(nRetVal < 0)	//close
				continue;

			if(nRetVal == 0)	//time out
				continue;
		}
		while(!g_bStop && !bHasData);

		if(g_bStop)
			break;

		//读取一个头
		if(0 > BLUEV_SOCK_Recv(nSockfd, pReq, sizeof(*pReq)))
			continue;

		//check req head
		if(0 == IsReqHeadValid(pReq))
			continue;

		nReqLen = Char4To_uint4(pReq->nDataLen);
		if(nReqLen == 0)
			continue;

		ctx.nReqLen = nReqLen;
		ctx.pReq = (unsigned char *)malloc(nReqLen);
		if(ctx.pReq == NULL)
			continue;

		memset(ctx.pReq, 0, nReqLen);

		if(0 > BLUEV_SOCK_Recv(nSockfd, ctx.pReq, nReqLen))
		{
			if(ctx.pReq)
				free(ctx.pReq);

			continue;
		}
		//execute service
		//PROXY_Service(&conn, &ctx);

		//send to
		if(ctx.nAnsLen > 0)
		{
			if(0 > BLUEV_SOCK_Send(conn.nSockfd, ctx.pAns, ctx.nAnsLen))
			{
				continue;
			}
		}

		if(ctx.pReq)
		{
			free(ctx.pReq);
			ctx.pReq = NULL;
		}
		if(ctx.pAns)
		{
			free(ctx.pAns);
			ctx.pAns = NULL;
		}

	}
	while(!g_bStop);

	//log
	printf("%s %lx end.\n", "TCP work thread", pthread_self());
	nRetVal = 0;

	return (void *)nRetVal;
}

//心跳线程
void * TCPKeepAliveProc(void *pParam)
{
    int				nRetVal, nSecond, nszJsonLen;
    char			*pszJson = NULL;
    BLUEV_NetReqHead	req;
    BLUEV_NetAnsHead	ans;

    if(pParam == NULL)
        return (void *)-1;

	if(g_pmtxSocket == NULL)
			return (void *)-1;

    printf("%s %lx started. \n", "TCP keepalive thread", pthread_self());

    nRetVal = -1;
    nSecond = (int)pParam;
    //nSecond = 20;

	InitReqHead(&req);

    uint2ToChar2(OP_TCPCLASS_P2P, req.nOpClass);
    uint2ToChar2(OP_TCP_KEEPALIVE, req.nOpCode);

    do
    {
    	nRetVal = -1;

    	//未登录
		if(!g_ConnSvr.bLogIn || !g_ConnSvr.bConnected)
		{
			//中途失去连接，等待重连
			sleep(1);
			continue;
		}

    	if(pszJson)
		{
			free(pszJson);
			pszJson = NULL;
		}

		if(0 > SessionId2Json(&g_ConnSvr, &pszJson) ||
			pszJson == NULL)
			break;//return (void *)nRetVal;

		nszJsonLen = strlen(pszJson);
		if(nszJsonLen == 0)
			break;//return (void *)nRetVal;

		uint4ToChar4(nszJsonLen, req.nDataLen);

		//lock socket
		pthread_mutex_lock(g_pmtxSocket);

		int		bSocketOk = 0;
		printf("req keepalive json : %s\n", pszJson);
		if(0 > BLUEV_SOCK_Send(g_TCPServer, &req, sizeof(req)))
			ERR_EXIT();
		if(0 > BLUEV_SOCK_Send(g_TCPServer, pszJson, nszJsonLen))
			ERR_EXIT();

		if(0 > BLUEV_SOCK_Recv(g_TCPServer, &ans, sizeof(ans)))
			ERR_EXIT();

		bSocketOk = 1;

Finish_Exit:
		//unlock
		pthread_mutex_unlock(g_pmtxSocket);

		if(!bSocketOk)
		{
			if(g_TCPServer > 0)
			{
				close(g_TCPServer);
				g_TCPServer = -1;
			}
			OpenTCPServer(g_nSvrPort);

			g_ConnSvr.bLogIn = 0;
			g_ConnSvr.bConnected = 0;

			continue;
		}

		if(!IsAnsHeadValid(&ans))
			continue;

		nRetVal = Char2To_uint2(ans.nRetVal);
		if(nRetVal < 0)
			continue;

		BLUEV_Event_TimeWait(g_phKeepAlive, nSecond);

		nRetVal = 0;
    }
    while(!g_bStop);

    printf("%s %lx end.\n", "TCP keepalive thread", pthread_self());
	if(pszJson)
		free(pszJson);

	return (void *)nRetVal;
}

//断线重连线程
void * TCPReConnectProc(void *pParam)
{
	int		nRetVal = -1;
	//int		nSocket = -1;

	//if(pParam == NULL)
	//	return (void *)nRetVal;

	//nSocket = *(int *)pParam;

	//if(nSocket < 0)
	//	return (void *)nRetVal;

	//connect svr
	int 				nIP;
	struct			sockaddr_in	serveraddr;
	struct			timeval	tv;

	while(!g_bStop)
	{
		if(IsConnect())
		{
			sleep(1);
			continue;
		}

		if(0 > BLUEV_SOCK_NameToIP(g_szSvrIP, &nIP))
			break;

		BLUEV_SOCK_SetAddrN(&serveraddr, nIP, g_nSvrPort);

#ifdef SYSTEM_MAC
		//IOS HOME切换会回收socket资源导致报异常，这里要屏蔽此异常
		int set = 1;
		setsockopt(g_TCPServer, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif

		//设置超时
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		setsockopt(g_TCPServer, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
		setsockopt(g_TCPServer, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));

		//阻塞
		nRetVal = connect(g_TCPServer, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
		if(nRetVal < 0)
		{
			sleep(1);
			continue;
		}

		g_ConnSvr.bConnected = 1;
	}

	return (void *)nRetVal;
}

//登入线程
void * TCPLogInProc(void *pParam)
{
	int		nRetVal = -1;
	LogInContext	*pContext = NULL;

	if(pParam == NULL)
		return (void *)nRetVal;

	pContext = (LogInContext *)pParam;

	if(&g_ctxLogin != pContext)
		g_ctxLogin = *pContext;

	//connect svr
	int 				nIP;
	struct			sockaddr_in	serveraddr;
	struct			timeval	tv;

	while(!g_bStop)
	{
		if(IsLogin() || !IsConnect())
		{
			sleep(1);
			continue;
		}

		if(0 > _LogIn())
		{
			sleep(1);
			continue;
		}
	}

	return (void *)nRetVal;
}


//#################################################
//**** TCPSERVER Function
int InitServer()
{
	g_TCPServer = -1;
	memset(g_szSvrIP, 0, sizeof(g_szSvrIP));
	g_nSvrPort = 0;
	g_bStop = 1;

	memset(&g_ctxLogin, 0, sizeof(g_ctxLogin));
	memset(&g_ConnSvr, 0, sizeof(g_ConnSvr));

	if(g_pmtxSocket == NULL)
	{
		g_pmtxSocket = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
		if(g_pmtxSocket == NULL)
			return -1;

        pthread_mutex_init(g_pmtxSocket, NULL);
	}

	if(g_phKeepAlive == NULL)
	{
		if(0 > BLUEV_CreateEvent(&g_phKeepAlive))
			return -1;
	}

	return 0;
}

int OpenTCPServer(int nPort)
{
	int		sockfd, nRetVal, nTemp;
	struct	sockaddr_in	serveraddr;

	if(nPort <= 0 || g_TCPServer != -1)
		return -1;

	g_bStop = 0;
	nRetVal = -1;

	//初始化socket
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0))<0 ) {
		return nRetVal;
	}

//	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_addr.s_addr = INADDR_ANY;
//	serveraddr.sin_port	= htons(/*(in_port_t)*/nPort);
//
//	nTemp = 1;
//	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,  &nTemp, sizeof(nTemp));
//
//	if ( bind(sockfd,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)) != 0) {
//		close(sockfd);
//		return nRetVal;
//	}

	g_TCPServer = sockfd;
	g_TCPServerPort = nPort;

	nRetVal = 0;

    return nRetVal;
}

int StartTCPServer(const char *pszSvrIP, int nSvrPort)
{
    if(pszSvrIP == NULL || nSvrPort <= 0 || g_TCPServer == -1)
		return -1;

	if(pszSvrIP != (char *)&g_szSvrIP)
	{
		strncpy(g_szSvrIP, pszSvrIP, sizeof(g_szSvrIP));
	}

	g_nSvrPort = nSvrPort;

    int					nRetVal;
	pthread_t	hHandle;
	int					tid;

	//发起线程
	nRetVal = BLUEV_CreateThread(&hHandle, 0, TCPReConnectProc, &g_TCPServer, 0, &tid);
	if(nRetVal < 0)
		return nRetVal;

	pthread_detach(hHandle);
	nRetVal = 0;

	return nRetVal;
}

int CloseTCPServer(void)
{
    g_bStop = 1;

	//等待线程退出
	sleep(2);
	if(g_TCPServer > 0)
	{
		close(g_TCPServer);
		g_TCPServer = -1;
	}

	if(g_phKeepAlive)
	{
		BLUEV_SetEvent(g_phKeepAlive);
		BLUEV_ReleaseEvent(g_phKeepAlive);
		g_phKeepAlive = NULL;
	}

	if(g_pmtxSocket)
	{
		pthread_mutex_destroy(g_pmtxSocket);
		free(g_pmtxSocket);

		g_pmtxSocket = NULL;
	}

	return 0;
}

int IsLogin()
{
	return g_ConnSvr.bLogIn;
}

int IsConnect()
{
	return g_ConnSvr.bConnected;
}
////////////////////////////////////////////////////

//operation
int LogIn(LogInContext *pContext)
{
    int			nRetVal;
	pthread_t	hHandle;
	int			tid;

	if(!pContext || !pContext->szLanIp[0] || pContext->nLanPort <= 0 ||
		!pContext->szMask[0]  || !pContext->szGateWay[0])
		return -1;

	//发起线程
	nRetVal = BLUEV_CreateThread(&hHandle, 0, TCPLogInProc, (void *)pContext, 0, &tid);
	if(nRetVal < 0)
		return nRetVal;

	pthread_detach(hHandle);
	nRetVal = 0;

	return nRetVal;
}

int LogOut()
{
	char				*pszJson = NULL;
	int					nRetVal = -1, nszJsonLen = 0;
	BLUEV_NetReqHead	req;
	BLUEV_NetAnsHead	ans;

	if(g_TCPServer < 0 || !IsLogin())
		return -1;

	//lock socket
	if(g_pmtxSocket == NULL)
		return -1;

	pthread_mutex_lock(g_pmtxSocket);

	InitReqHead(&req);

	uint2ToChar2(OP_TCPCLASS_P2P, req.nOpClass);
	uint2ToChar2(OP_TCP_LOGOUT, req.nOpCode);

	if(0 > Session2Json(&g_ConnSvr, &pszJson) ||
		pszJson == NULL)
		ERR_EXIT();

	nszJsonLen = strlen(pszJson);
	if(nszJsonLen == 0)
		ERR_EXIT();

	printf("req logout json : %s\n", pszJson);

	uint4ToChar4(nszJsonLen, req.nDataLen);

	if(0 > BLUEV_SOCK_Send(g_TCPServer, &req, sizeof(req)))
		ERR_EXIT();

	if(0 > BLUEV_SOCK_Send(g_TCPServer, (void *)pszJson, nszJsonLen))
		ERR_EXIT();

	if(0 > BLUEV_SOCK_Recv(g_TCPServer, &ans, sizeof(ans)))
		ERR_EXIT();

	if(!IsAnsHeadValid(&ans))
		ERR_EXIT();

	nRetVal = Char2To_uint2(ans.nRetVal);
	if(nRetVal < 0)
		ERR_EXIT();

	//登出成功
	g_ConnSvr.bLogIn = 0;
	memset(g_ConnSvr.stUserId.szSessionId, 0, sizeof(g_ConnSvr.stUserId.szSessionId));

	printf("logout success\n");

Finish_Exit:
	if(pszJson)
		free(pszJson);

	//unlock socket
	pthread_mutex_unlock(g_pmtxSocket);

	return nRetVal;
}

int KeepAlive(int nSecond)
{
    int			nRetVal;
	pthread_t	hHandle;
	int			tid;

	if(nSecond <= 0)
	        return -1;

	//发起线程
	nRetVal = BLUEV_CreateThread(&hHandle, 0, TCPKeepAliveProc, (void *)nSecond, 0, &tid);
	if(nRetVal < 0)
		return nRetVal;

	pthread_detach(hHandle);
	nRetVal = 0;

	return nRetVal;
}

int GetDevice(char **ppszInfo)
{
	char				*pszJson = NULL, *pAnsData = NULL;
	int					nRetVal = -1, nszJsonLen = 0, nAnsDataLen = 0;
	BLUEV_NetReqHead	req;
	BLUEV_NetAnsHead	ans;

	if(g_TCPServer < 0 || !IsLogin() || ppszInfo == NULL || !g_ctxLogin.bMainCtrl)
		return -1;

	//lock socket
	if(g_pmtxSocket == NULL)
		return -1;

	pthread_mutex_lock(g_pmtxSocket);

	InitReqHead(&req);

	uint2ToChar2(OP_TCPCLASS_P2P, req.nOpClass);
	uint2ToChar2(OP_TCP_GETDEVINFO, req.nOpCode);

	if(0 > Session2Json(&g_ConnSvr, &pszJson) ||
		pszJson == NULL)
		ERR_EXIT();

	nszJsonLen = strlen(pszJson);
	if(nszJsonLen == 0)
		ERR_EXIT();

	printf("req getdevice json : %s\n", pszJson);

	uint4ToChar4(nszJsonLen, req.nDataLen);

	if(0 > BLUEV_SOCK_Send(g_TCPServer, &req, sizeof(req)))
		ERR_EXIT();

	if(0 > BLUEV_SOCK_Send(g_TCPServer, (void *)pszJson, nszJsonLen))
		ERR_EXIT();

	if(0 > BLUEV_SOCK_Recv(g_TCPServer, &ans, sizeof(ans)))
		ERR_EXIT();

	if(!IsAnsHeadValid(&ans))
		ERR_EXIT();

	nRetVal = Char2To_uint2(ans.nRetVal);
	if(nRetVal < 0)
		ERR_EXIT();

	nAnsDataLen = Char4To_uint4(ans.nDataLen);
	if(nAnsDataLen <= 0 || nAnsDataLen >= 10*1024*1024)	//10M
		ERR_EXIT();

	pAnsData = (char *)malloc(nAnsDataLen + 1);
	if(pAnsData == NULL)
		ERR_EXIT();
	pAnsData[nAnsDataLen] = 0;

	if(0 > BLUEV_SOCK_Recv(g_TCPServer, pAnsData, nAnsDataLen))
		ERR_EXIT();

	printf("receive tcp getdevice json:%s\n", pAnsData);

	*ppszInfo = pAnsData;
	pAnsData = NULL;

	nRetVal = 0;

Finish_Exit:
	if(pAnsData)
		free(pAnsData);

	if(pszJson)
		free(pszJson);

	//unlock socket
	pthread_mutex_unlock(g_pmtxSocket);

	return nRetVal;
}

//json parse
int Json2Session(const char *pszJson, char **ppszSessionId)
{
	return Json2SessionID(pszJson, ppszSessionId);
}

int Json2SessionID(const char *pszJson, char **ppszSessionId)
{
	cJSON	*root, *sessionid;

	int			nszDataLen, nRetVal;
	char		*pszData;

	nRetVal = -1;

	if(pszJson == NULL || ppszSessionId == NULL)
		return -1;

	if(strlen(pszJson) <= 0)
		return -1;

	root = cJSON_Parse(pszJson);
	if(root == NULL)
		return -1;

	sessionid = cJSON_GetObjectItem(root, JSON_SESSIONID);

	if(sessionid == NULL)
		ERR_EXIT();

	if(sessionid->type != cJSON_String || sessionid->valuestring == NULL)
		ERR_EXIT();

	nszDataLen = strlen(sessionid->valuestring);
	if(nszDataLen <= 0)
		ERR_EXIT();

	pszData = (char *)malloc(nszDataLen + 1);
	if(pszData == NULL)
		ERR_EXIT();

	memset(pszData, 0, nszDataLen + 1);

	memcpy(pszData, sessionid->valuestring, nszDataLen);

	*ppszSessionId = pszData;

	nRetVal = 0;

Finish_Exit:
	if(root)
		cJSON_Delete(root);

	return nRetVal;
}

int Json2DeviceInfo(const char *pszJson, DeviceInfo *pDev)
{
	cJSON	*root, *devs, *dev;
	cJSON	*sessionid, *ip, *port, *issamelan, *lanip, *lanport;
	cJSON	*isupnp, *upnpip, *upnpport;

	int			nRetVal;

	nRetVal = -1;

	if(pszJson == NULL || pDev == NULL)
		return -1;

	if(strlen(pszJson) <= 0)
		return -1;

	root = cJSON_Parse(pszJson);
	if(root == NULL)
		return -1;

	devs = cJSON_GetObjectItem(root, JSON_DEVS);
	if(devs == NULL)
		ERR_EXIT();

	dev = cJSON_GetArrayItem(devs, 0);
	if(dev == NULL)
		ERR_EXIT();

	ip = cJSON_GetObjectItem(dev, JSON_IP);
	port = cJSON_GetObjectItem(dev, JSON_PORT);
	issamelan = cJSON_GetObjectItem(dev, JSON_ISSAMELAN);
	isupnp = cJSON_GetObjectItem(dev, JSON_ISUPNP);

	if(ip == NULL || port == NULL || issamelan == NULL || isupnp == NULL)
		ERR_EXIT();

    //省略其他检查，麻烦。。
    strncpy(pDev->szIp, ip->valuestring, sizeof(pDev->szIp));
	pDev->nPort = port->valueint;
	pDev->bSameLan = issamelan->valueint ? 1 : 0;
	pDev->bUPNP = isupnp->valueint ? 1 : 0;

	if(pDev->bSameLan)
	{
		lanip = cJSON_GetObjectItem(dev, JSON_LANIP);
		lanport = cJSON_GetObjectItem(dev, JSON_LANPORT);

		if(lanip == NULL || lanport == NULL)
			ERR_EXIT();

		strncpy(pDev->szLanIp, lanip->valuestring, sizeof(pDev->szLanIp));
		pDev->nLanPort = lanport->valueint;
	}

	if(pDev->bUPNP)
	{
		upnpip = cJSON_GetObjectItem(dev, JSON_UPNPIP);
		upnpport = cJSON_GetObjectItem(dev, JSON_UPNPPORT);

		if(upnpip == NULL || upnpport == NULL)
			ERR_EXIT();

		strncpy(pDev->szUPNPIP, upnpip->valuestring, sizeof(pDev->szUPNPIP));
		pDev->nUPNPPort = upnpport->valueint;
	}

	nRetVal = 0;

Finish_Exit:
	if(root)
		cJSON_Delete(root);

	return nRetVal;
}

//json build
int UserData2Json(LogInContext *pContext, char **ppszJson)
{
	cJSON	*root;
	char	*pszJson;

	if(pContext == NULL || ppszJson == NULL)
		return -1;

	root = cJSON_CreateObject();
	if(!root)
		return -1;

	cJSON_AddStringToObject(root, JSON_LANIP, pContext->szLanIp);
	cJSON_AddNumberToObject(root, JSON_LANPORT, pContext->nLanPort);
	cJSON_AddStringToObject(root, JSON_MASK, pContext->szMask);
	cJSON_AddStringToObject(root, JSON_GATEWAY, pContext->szGateWay);

	cJSON_AddNumberToObject(root, JSON_ISMAINCTRL, pContext->bMainCtrl);

	//主控端
	if(pContext->bMainCtrl)
	{
		cJSON_AddStringToObject(root, JSON_MAC, pContext->szMac);
		cJSON_AddStringToObject(root, JSON_USERNAME, pContext->szUserName);
		cJSON_AddStringToObject(root, JSON_PASSWORD, pContext->szPassWord);
	}
	else
	{
		cJSON_AddStringToObject(root, JSON_DEVID, pContext->szDevId);

		//被控端提供UPNP信息
		cJSON_AddNumberToObject(root, JSON_ISUPNP, pContext->bUPNP);
		if(pContext->bUPNP)
		{
			cJSON_AddStringToObject(root, JSON_UPNPIP, pContext->szUPNPIP);
			cJSON_AddNumberToObject(root, JSON_UPNPPORT, pContext->nUPNPPort);
		}
	}

	pszJson = cJSON_Print(root);
	if(pszJson == NULL)
	{
		cJSON_Delete(root);
		return -1;
	}

	*ppszJson = pszJson;

	cJSON_Delete(root);
	return 0;
}

int Session2Json(P2PConnection *pConn, char **ppszJson)
{
	return SessionId2Json(pConn, ppszJson);
}

int SessionId2Json(P2PConnection *pConn, char **ppszJson)
{
	cJSON	*root;
	char	*pszJson;

	if(pConn == NULL || ppszJson == NULL)
		return -1;

	root = cJSON_CreateObject();
	if(!root)
		return -1;

	cJSON_AddStringToObject(root, JSON_SESSIONID, (char *)pConn->stUserId.szSessionId);

	pszJson = cJSON_Print(root);
	if(pszJson == NULL)
	{
		cJSON_Delete(root);
		return -1;
	}

	*ppszJson = pszJson;

	cJSON_Delete(root);
	return 0;
}


//UTIL FUNC
unsigned short Char2To_uint2(const unsigned char *ch)
{
	return	(unsigned short)(ch[0]*256+ch[1]);
}

void uint2ToChar2(unsigned short w, unsigned char *ch)
{
	ch[0] = (unsigned char)(w/256);
	ch[1] = (unsigned char)(w & (0xFF));
}

unsigned int Char4To_uint4(const unsigned char *ch)
{
	return	(unsigned int)(ch[0]*256*256*256+ch[1]*256*256+ch[2]*256+ch[3]);
}

void uint4ToChar4(unsigned int dw, unsigned char *ch)
{
	ch[0] = (unsigned char)(dw/(256*256*256));
	ch[1] = (unsigned char)((dw/(256*256)) & (0xFF));
	ch[2] = (unsigned char)((dw/256) & (0xFF));
	ch[3] = (unsigned char)(dw & (0xFF));
}

//UTIL FUNC
long long getTickCount()
{
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	gettimeofday(&tv, NULL);
	return (long long)(tv.tv_sec * 1000000 + tv.tv_usec);
}

double tickToSecond(long long llTimeUsed)
{
	double	fTime;

	fTime = (double)1000000;
	return	llTimeUsed/fTime;
}


/////////////////////////////////
int IsReqHeadValid(BLUEV_NetReqHead *pReq)
{
	if(pReq == NULL)
		return 0;

	if(Char2To_uint2(pReq->cbSize) != sizeof(*pReq))
		return 0;

	if(memcmp(pReq->szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR)) != 0)
		return 0;

	if(pReq->nMajorVer != BV_MAJORVER)
		return 0;

	return 1;
}

int InitReqHead(BLUEV_NetReqHead *pReq)
{
	if(pReq == NULL)
		return -1;

	memset(pReq, 0, sizeof(*pReq));
	uint2ToChar2(sizeof(*pReq), pReq->cbSize);
	memcpy(pReq->szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
	pReq->nMajorVer = BV_MAJORVER;

	return 0;
}

int IsAnsHeadValid(BLUEV_NetAnsHead *pAns)
{
	if(pAns == NULL)
		return 0;

	if(Char2To_uint2(pAns->cbSize) != sizeof(*pAns))
		return 0;

	if(memcmp(pAns->szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR)) != 0)
		return 0;

	if(pAns->nMajorVer != BV_MAJORVER)
		return 0;

	return 1;
}

int InitAnsHead(BLUEV_NetAnsHead *pAns)
{
	if(pAns == NULL)
		return -1;

	memset(pAns, 0, sizeof(*pAns));
	uint2ToChar2(sizeof(*pAns), pAns->cbSize);
	memcpy(pAns->szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
	pAns->nMajorVer = BV_MAJORVER;

	return 0;
}




