#include "P2PSocket.h"
#include "NetSocket.h"
#include "base64.h"
#include "cJSON.h"

static int g_P2PServer = -1;
static int g_P2PServerPort = 6214;
static volatile int g_bStop = 0;

static P2PConnection g_ConnSvr ;
static Event	*g_phKeepAlive;
static Event	*g_phLogIn;
static LogInContext	g_ctxLogin;

static AppInfoLinkList		g_listAppInfo;

static int	g_snRange = 0;

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

static int BLUEV_SOCK_NameToIP(char *cServerName, int *nIP)
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


static int RebuildConn(const char *pszIp, int nPort, struct sockaddr_in *psin, int *pnAddrLen)
{
	int nIP = 0;

	if(pszIp == NULL|| nPort <= 0 || psin == NULL || pnAddrLen == NULL)
		return -1;

	*pnAddrLen = sizeof(*psin);

	if(0 > BLUEV_SOCK_NameToIP((char *)pszIp, &nIP))
		return -1;

	BLUEV_SOCK_SetAddrN(psin, nIP, nPort);

	return 0;
}


///////////////////////////////////////////////////
//工作线程
void * P2PServerProc(void *pParam)
{
    int				nRetVal, bHasData, nSockfd;
    socklen_t  nAddrLen;
	int				nReqLen, bFromSvr;
	UDPServiceParam	ctx;
	UDPConnection	conn;
	struct			sockaddr_in addr;

	BLUEV_UDPReqHead	*pReq;

	if(pParam == NULL)
		return (void *)-1;

	nAddrLen = sizeof(addr);
	memset(&conn, 0, sizeof(conn));
	nSockfd = *(int *)pParam;

	printf("%s %lx started. \n", "P2P work thread", pthread_self());

	do
	{
		memset(&ctx, 0, sizeof(ctx));

		do
		{
			//select
			BLUEV_SOCK_WaitDataToRead(&nSockfd, 1, 3, 0, &bHasData);
		}
		while(!g_bStop && !bHasData);

		if(g_bStop)
			break;

		//recv from
		nReqLen = recvfrom(nSockfd, ctx.bnReqBuf, sizeof(ctx.bnReqBuf), 0, (struct sockaddr*)&addr, &nAddrLen);

		conn.nSockfd = nSockfd;
		conn.fromaddr = addr;

		//check req head
		//一种是服务器发来，一种是设备反馈。两种包头
		//新增一种是P2P客户端发来，同设备反馈包头，需要验证IP
		pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;
		if(0 == IsReqHeadValid(pReq))
		{
			bFromSvr = 0;

			//也可能来自设备
			if(strlen((char*)ctx.bnReqBuf) != nReqLen ||
				0 != strncmp(NETSOCKET_HEAD, (char*)ctx.bnReqBuf, strlen(NETSOCKET_HEAD)) ||
				0 != strncmp(NETSOCKET_TAIL, (char*)(ctx.bnReqBuf + nReqLen - 4), strlen(NETSOCKET_TAIL)))
			{
				continue;
			}
		}
		else
		{
			bFromSvr = 1;
		}

		ctx.nReqLen = nReqLen;
		//execute service
		if(bFromSvr)
		{
			P2P_Service(&conn, &ctx);
		}
		else
		{
			//根据IP区分
			int	nIP = 0;
			BLUEV_SOCK_NameToIP(NETSOCKET_LOCIP, &nIP);
			if(conn.fromaddr.sin_addr.s_addr == nIP && conn.fromaddr.sin_port == g_NetworkInfo.nLocalPort)
			{
				//来自设备反馈
				NetSocket_Service(&conn, &ctx);
			}
			else
			{
				//来自P2P客户端请求
				//转发给NETSOCKET
				//构造设备连接信息
				struct	sockaddr_in	addr;
				int			nAddrLen, nPort;

				nPort = g_NetworkInfo.nLocalPort;

				if(0 > RebuildConn(NETSOCKET_LOCIP, nPort, &addr, &nAddrLen))
					continue;

				sendto(nSockfd, ctx.bnReqBuf, ctx.nReqLen, 0, (struct sockaddr *)&addr, nAddrLen);

				//不回复
				ctx.nAnsLen = 0;
			}
		}

		//send to
		if(ctx.nAnsLen > 0)
		{
			sendto(conn.nSockfd, ctx.bnAnsBuf, ctx.nAnsLen, 0, (struct sockaddr*)&conn.fromaddr, sizeof(conn.fromaddr));
		}
	}
	while(!g_bStop);

	//log
	printf("%s %lx end.\n", "P2P work thread", pthread_self());
	nRetVal = 0;

	return (void *)nRetVal;
}

//心跳线程
void * P2PKeepAliveProc(void *pParam)
{
    int				nRetVal, nSecond, nszJsonLen, nReqLen;
    char			*pszJson;
    BLUEV_UDPReqHead	*pReq;
    char			bnReqBuf[1024] = {0};

    if(pParam == NULL)
        return (void *)-1;

    printf("%s %lx started. \n", "P2P keepalive thread", pthread_self());

    nRetVal = -1;
    nSecond = (int)pParam;
    //nSecond = 20;

    pReq = (BLUEV_UDPReqHead *)bnReqBuf;
	InitReqHead(pReq);

    uint2ToChar2(OP_CLASS_P2P, pReq->nOpClass);
    uint2ToChar2(OP_P2P_KEEPALIVE, pReq->nOpCode);

    //init event
    if(g_phKeepAlive == NULL)
    {
    	if(0 > BLUEV_CreateEvent(&g_phKeepAlive))
    		return (void *)-1;
    }

    do
    {
    	nRetVal = -1;

    	//未登录
		if(!g_ConnSvr.bLogIn)
		{
			//中途失去连接，等待重连
			sleep(1);
			continue;
		}

    	pszJson = NULL;
		if(0 > SessionId2Json(&g_ConnSvr, &pszJson) ||
			pszJson == NULL)
			break;//return (void *)nRetVal;

		nszJsonLen = strlen(pszJson);
		if(nszJsonLen <= 0)
			break;//return (void *)nRetVal;

		memcpy((char *)bnReqBuf + sizeof(BLUEV_UDPReqHead), pszJson, nszJsonLen);
		uint2ToChar2(nszJsonLen, pReq->nDataLen);
		nReqLen = sizeof(BLUEV_UDPReqHead) + nszJsonLen;

		{
			int			nSecondReSend;	//未收到反馈确认，则指数退避重发。
			int			nReslut;
			for(nSecondReSend = 3; nSecondReSend < KEEPALIVE_TIMEOUT; nSecondReSend *= 2)
			{
				printf("req keepalive json : %s\n", pszJson);
				sendto(g_ConnSvr.stConn.nSockfd, bnReqBuf, nReqLen, 0, (struct sockaddr *)&g_ConnSvr.stConn.fromaddr, sizeof(g_ConnSvr.stConn.fromaddr));

				//WAIT EVENT
				nReslut = BLUEV_Event_TimeWait(g_phKeepAlive, nSecondReSend);
				if(nReslut == ETIMEDOUT)
					continue;
				else if(nReslut == 0)
				{
					//收到心跳确认，被唤醒
					break;
				}
				else
				{
					//出错
					break;
				}
			}

		}

		if(pszJson)
		{
			free(pszJson);
			pszJson = NULL;
		}

		sleep(nSecond);

		nRetVal = 0;
    }
    while(!g_bStop);

    printf("%s %lx end.\n", "P2P keepalive thread", pthread_self());
	if(pszJson)
		free(pszJson);

	//release event
	BLUEV_ReleaseEvent(g_phKeepAlive);
	g_phKeepAlive = NULL;

	return (void *)nRetVal;
}

//登录线程
void * P2PLogInProc(void *pParam)
{
	BLUEV_UDPReqHead	*pReq;
	char				*pszJson;
	int					nszJsonLen, nReqLen, nRetVal;
	char				bnReqBuf[1024] = {0};

	if(g_P2PServer < 0)
		return (void *)-1;

	//init event
	if(g_phLogIn == NULL)
	{
		if(0 > BLUEV_CreateEvent(&g_phLogIn))
			return (void *)-1;
	}

	do
	{
		nRetVal = -1;

		if(IsLogin())
		{
			sleep(1);
			continue;
		}

		pReq = (BLUEV_UDPReqHead *)bnReqBuf;
		InitReqHead(pReq);

		uint2ToChar2(OP_CLASS_P2P, pReq->nOpClass);
		uint2ToChar2(OP_P2P_LOGIN, pReq->nOpCode);

		pszJson = NULL;
		if(0 > UserData2Json(&g_ctxLogin, &pszJson) ||
			pszJson == NULL)
			break;

		nszJsonLen = strlen(pszJson);
		if(nszJsonLen <= 0)
			break;

		memcpy((char *)bnReqBuf + sizeof(BLUEV_UDPReqHead), pszJson, nszJsonLen);
		uint2ToChar2(nszJsonLen, pReq->nDataLen);
		nReqLen = sizeof(BLUEV_UDPReqHead) + nszJsonLen;

		//服务器连接信息
		{
			struct	sockaddr_in	addr;
			int			nIP, nPort;

			nPort = g_ctxLogin.nSvrPort;
			nIP = 0;
			if(0 > BLUEV_SOCK_NameToIP(g_ctxLogin.pszSvrIP, &nIP) ||
				nIP == 0)
				break;

			BLUEV_SOCK_SetAddrN(&addr, nIP, nPort);

			//sendto(g_P2PServer, bnReqBuf, nReqLen, 0, (struct sockaddr *)&addr, sizeof(addr));
			{
				int			nSecondReSend;	//未收到反馈确认，则指数退避重发。
				int			nReslut;
				for(nSecondReSend = 2; nSecondReSend < LOGIN_TIMEOUT; nSecondReSend *= 2)
				{
					if(g_ConnSvr.bLogIn)
						break;

					printf("req login json : %s\n", pszJson);
					sendto(g_P2PServer, bnReqBuf, nReqLen, 0, (struct sockaddr *)&addr, sizeof(addr));

					//WAIT EVENT
					nReslut = BLUEV_Event_TimeWait(g_phLogIn, nSecondReSend);
					if(nReslut == ETIMEDOUT)
						continue;
					else if(nReslut == 0)
					{
						//收到登录确认，被唤醒
						break;
					}
					else
					{
						//出错
						break;
					}
				}

			}
		}

		if(pszJson)
		{
			free(pszJson);
			pszJson = NULL;
		}

		nRetVal = 0;
	}
	while(!g_bStop);

	if(pszJson)
		free(pszJson);

	//release event
	BLUEV_ReleaseEvent(g_phLogIn);
	g_phLogIn = NULL;

	return (void *)nRetVal;
}

//#################################################
//**** P2PSERVER Function
int OpenP2PServer(int nPort)
{
	int		sockfd, nRetVal, nTemp;
	struct	sockaddr_in	serveraddr;

	if(nPort <= 0 || g_P2PServer != -1)
		return -1;

	g_bStop = 0;
	nRetVal = -1;

	//初始化socket
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0 ) {
		return nRetVal;
	}

	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port	= htons(/*(in_port_t)*/nPort);

	nTemp = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,  &nTemp, sizeof(nTemp));

	if ( bind(sockfd,(struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)) != 0) {
		close(sockfd);
		return nRetVal;
	}

	g_P2PServer = sockfd;
	g_P2PServerPort = nPort;

	//初始化链表
	InitAppInfoList(&g_listAppInfo);
	nRetVal = 0;

    return nRetVal;
}

int StartP2PServer(void)
{
    int			nRetVal;
	pthread_t	hHandle;
	int			tid;

	//发起线程
	nRetVal = BLUEV_CreateThread(&hHandle, 0, P2PServerProc, &g_P2PServer, 0, &tid);
	if(nRetVal < 0)
		return nRetVal;

	pthread_detach(hHandle);
	nRetVal = 0;

	return nRetVal;
}

int CloseP2PServer(void)
{
    g_bStop = 1;

	//等待线程退出
	sleep(2);
	if(g_P2PServer > 0)
	{
		close(g_P2PServer);
		g_P2PServer = -1;
	}

	ReleaseAppInfoList(&g_listAppInfo);

	return 0;
}

int IsLogin()
{
	return g_ConnSvr.bLogIn;
}
////////////////////////////////////////////////////

//operation
/*
int LogIn(LogInContext	*pContext)
{
	BLUEV_UDPReqHead	*pReq;
	char				*pszJson;
	int					nszJsonLen, nReqLen;
	char				bnReqBuf[1024] = {0};

	if(g_P2PServer < 0)
		return -1;

	if(!pContext || !pContext->pszLanIp || pContext->nLanPort <= 0 ||
		!pContext->pszMask || !pContext->pszDevId ||
		!pContext->pszSvrIP || pContext->nSvrPort <= 0)
		return -1;

	if(&g_ctxLogin != pContext)
		g_ctxLogin = *pContext;

	pReq = (BLUEV_UDPReqHead *)bnReqBuf;
	InitReqHead(pReq);

	uint2ToChar2(OP_CLASS_P2P, pReq->nOpClass);
	uint2ToChar2(OP_P2P_LOGIN, pReq->nOpCode);

	pszJson = NULL;
	if(0 > UserData2Json(pContext, &pszJson) ||
		pszJson == NULL)
		return -1;

	nszJsonLen = strlen(pszJson);
	if(nszJsonLen <= 0)
		return -1;

	printf("req login json : %s\n", pszJson);

	memcpy((char *)bnReqBuf + sizeof(BLUEV_UDPReqHead), pszJson, nszJsonLen);
	uint2ToChar2(nszJsonLen, pReq->nDataLen);
	nReqLen = sizeof(BLUEV_UDPReqHead) + nszJsonLen;

	//服务器连接信息
	{
		struct	sockaddr_in	addr;
		int		nIP, nPort;

		nPort = pContext->nSvrPort;
		nIP = 0;
		if(0 > BLUEV_SOCK_NameToIP(pContext->pszSvrIP, &nIP) ||
			nIP == 0)
		{
			free(pszJson);
			return -1;
		}

		BLUEV_SOCK_SetAddrN(&addr, nIP, nPort);
		sendto(g_P2PServer, bnReqBuf, nReqLen, 0, (struct sockaddr *)&addr, sizeof(addr));

	}

	free(pszJson);
	return 0;
}*/

int LogIn(LogInContext	*pContext)
{
	int				nRetVal;
	pthread_t	hHandle;
	int				tid;

	if(g_P2PServer < 0)
		return -1;

	if(!pContext || !pContext->pszLanIp || pContext->nLanPort <= 0 ||
		!pContext->pszMask || !pContext->pszDevId || !pContext->pszGateWay ||
		!pContext->pszSvrIP || pContext->nSvrPort <= 0)
		return -1;

	if(&g_ctxLogin != pContext)
		g_ctxLogin = *pContext;

	//发起线程
	nRetVal = BLUEV_CreateThread(&hHandle, 0, P2PLogInProc, (void *)0, 0, &tid);
	if(nRetVal < 0)
		return nRetVal;

	pthread_detach(hHandle);
	nRetVal = 0;

	return nRetVal;
}


int LogOut()
{
	BLUEV_UDPReqHead	*pReq;
	char				*pszJson;
	int					nszJsonLen, nReqLen;
	char				bnReqBuf[1024] = {0};

	if(g_P2PServer < 0 || !g_ConnSvr.bLogIn)
		return -1;

	pReq = (BLUEV_UDPReqHead *)bnReqBuf;
	InitReqHead(pReq);

	uint2ToChar2(OP_CLASS_P2P, pReq->nOpClass);
	uint2ToChar2(OP_P2P_LOGOUT, pReq->nOpCode);

	pszJson = NULL;
	if(0 > Session2Json(&g_ConnSvr, &pszJson) ||
		pszJson == NULL)
		return -1;

	nszJsonLen = strlen(pszJson);
	if(nszJsonLen <= 0)
		return -1;

	printf("req logout json : %s\n", pszJson);

	memcpy((char *)bnReqBuf + sizeof(BLUEV_UDPReqHead), pszJson, nszJsonLen);
	uint2ToChar2(nszJsonLen, pReq->nDataLen);
	nReqLen = sizeof(BLUEV_UDPReqHead) + nszJsonLen;

	//服务器连接信息
	sendto(g_ConnSvr.stConn.nSockfd, bnReqBuf, nReqLen, 0, (struct sockaddr *)&g_ConnSvr.stConn.fromaddr, sizeof(g_ConnSvr.stConn.fromaddr));

	free(pszJson);
	return 0;
}

int KeepAlive(int nSecond)
{
    int			nRetVal;
	pthread_t	hHandle;
	int			tid;

   /* if(nSecond <= 0 || !g_ConnSvr.bLogIn)
        return -1;*/
	//现在登录会自动重连，这里判断登录不对。
	if(nSecond <= 0)
	        return -1;

	//发起线程
	nRetVal = BLUEV_CreateThread(&hHandle, 0, P2PKeepAliveProc, (void *)nSecond, 0, &tid);
	if(nRetVal < 0)
		return nRetVal;

	pthread_detach(hHandle);
	nRetVal = 0;

	return nRetVal;
}
//service

//只处理P2P服务器发来请求
int P2P_Service(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPReqHead	*pReq;
	BLUEV_UDPAnsHead	*pAns;
	int					nOpClass, nOpCode, nRetVal;
	int					bReq, bProxy, bWithSvr;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	nRetVal = -1;
	pReq = (BLUEV_UDPReqHead *)pCtx->bnReqBuf;

    nOpClass = Char2To_uint2(pReq->nOpClass);

	nOpCode = Char2To_uint2(pReq->nOpCode);

	pCtx->nAnsLen = sizeof(BLUEV_UDPAnsHead);

	switch(nOpClass)
	{
	case OP_CLASS_P2P:
		//来自服务器的请求
		bProxy = 0;
		bReq = 1;
		bWithSvr = 1;

		break;
	case OP_CLASS_P2PANS:
		//来自服务器的应答
		bProxy = 0;
		bReq = 0;
		bWithSvr = 1;

		break;
	case OP_CLASS_P2PPROXY:
		//来自服务器代理请求
		bProxy = 1;
		bReq = 1;
		bWithSvr = 1;

		break;
	case OP_CLASS_P2PPROXYANS:
		//来自服务器代理应答
		bProxy = 1;
		bReq = 0;
		bWithSvr = 1;

		break;
	case OP_CLASS_P2PCLIENT:
		//来自主控端的请求
		bProxy = 0;
		bReq = 1;
		bWithSvr = 0;

		break;
	case OP_CLASS_P2PCLIENTANS:
		//来自主控端的应答
		bProxy = 0;
		bReq = 0;
		bWithSvr = 0;

		break;
	default:

		return -1;
	}

	if(!bReq && !bProxy && bWithSvr)
	{
		//不反馈
		pCtx->nAnsLen = 0;

		switch(nOpCode)
		{
		case OP_P2P_LOGIN:
			nRetVal = LogInAns(pConn, pCtx);

			break;
		case OP_P2P_LOGOUT:
			nRetVal = LogOutAns(pConn, pCtx);

			break;
		case OP_P2P_KEEPALIVE:
			nRetVal = KeepAliveAns(pConn, pCtx);

			break;
		default:

			ERR_EXIT();
		}
	}
	else if(bReq && !bProxy && bWithSvr)
	{
		switch(nOpCode)
		{
		case OP_P2P_DIG:
			nRetVal = DigClient(pConn, pCtx);

			break;
		default:

			ERR_EXIT();
		}
	}
	else if(bProxy)
	{
		switch(nOpCode)
		{
		case OP_P2P_PROXY:
			nRetVal = ProxyReq(pConn, pCtx);

			break;
		default:

			ERR_EXIT();
		}
	}
	else if(bReq && !bWithSvr)
	{
		switch(nOpCode)
		{
		case OP_P2P_CONNDEV:
			nRetVal = ConnByClient(pConn, pCtx);

			break;
		case OP_P2P_KEEPALIVE:
			nRetVal = KeepAliveByClient(pConn, pCtx);

			break;
		default:

			ERR_EXIT();
		}
	}
	else if(!bReq && !bWithSvr)
	{
		switch(nOpCode)
		{
		case OP_P2P_CONNDEV:
			nRetVal = DigClientAns(pConn, pCtx);

			break;
		default:

			ERR_EXIT();
		}
	}

Finish_Exit:
	if(pCtx->nAnsLen > 0)
	{
		pAns = (BLUEV_UDPAnsHead *)pCtx->bnAnsBuf;
		uint2ToChar2(nRetVal, pAns->nRetVal);
		uint2ToChar2(OP_CLASS_P2PCLIENTANS, pAns->nOpClass);
		uint2ToChar2(nOpCode, pAns->nOpCode);
	}

	return nRetVal;
}

int LogInAns(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPAnsHead	*pAns;
	char				*pszJson, *pszData;
	int					nJsonLen, nszDataLen, nResult;
	long long			llSessionId;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	//登录成功
	pAns = (BLUEV_UDPAnsHead *)pCtx->bnReqBuf;
	nResult = (short)Char2To_uint2(pAns->nRetVal);
	printf("login ans result:%d\n", nResult);

	if(nResult < 0)
	{
		//服务端内存错误
		printf("fail to login\n");
		//线程会自动重新登录

		return -1;
	}

	//parse json
	pszJson = (char *)pCtx->bnReqBuf + sizeof(*pAns);
	nJsonLen = strlen(pszJson);
	if(nJsonLen <= 0 || nJsonLen != Char2To_uint2(pAns->nDataLen))
		return -1;

	printf("receive login json:%s\n", pszJson);

	pszData = NULL;
	nszDataLen = 0;
	if(0 > Json2Session(pszJson, &llSessionId, &pszData) ||
		pszData == NULL)
		return -1;

	nszDataLen = strlen(pszData);
	if(nszDataLen <= 0 || nszDataLen > sizeof(g_ConnSvr.stUserId.szGuid))
	{
		free(pszData);
		return -1;
	}

	g_ConnSvr.stUserId.llSessionId = llSessionId;
	memcpy(g_ConnSvr.stUserId.szGuid, pszData, nszDataLen);
	g_ConnSvr.stConn = *pConn;
	g_ConnSvr.bLogIn = 1;

	free(pszData);
	pszData = NULL;

	//不回复
	pCtx->nAnsLen = 0;

	printf("登陆成功，session id:%lld\n", g_ConnSvr.stUserId.llSessionId);

	//唤醒登录线程，使其退出指数退避
	if(0 > BLUEV_SetEvent(g_phLogIn))
		return -1;

	return 0;
}

int LogOutAns(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPAnsHead	*pAns;
	int										nResult;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	//不回复
	pCtx->nAnsLen = 0;

	pAns = (BLUEV_UDPAnsHead *)pCtx->bnReqBuf;
	nResult = (short)Char2To_uint2(pAns->nRetVal);

	printf("logout ans result:%d\n", nResult);

	if(nResult < 0)
	{
		switch(nResult)
		{
		case ERR_LOGOUT:
			break;	//重复登出
		default:
			return -1;
		}
	}

	//登出成功
	g_ConnSvr.bLogIn = 0;
	memset(&g_ConnSvr.stUserId, 0, sizeof(g_ConnSvr.stUserId));
	memset(&g_ConnSvr.stProxyInfo, 0, sizeof(g_ConnSvr.stProxyInfo));

	printf("登出成功\n");

	return 0;
}

int KeepAliveAns(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPAnsHead	*pAns;
	int										nResult;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	//不回复
	pCtx->nAnsLen = 0;

	pAns = (BLUEV_UDPAnsHead *)pCtx->bnReqBuf;
	nResult = (short)Char2To_uint2(pAns->nRetVal);
	printf("keepalive ans result:%d\n", nResult);

	if(nResult < 0)
	{
		switch(nResult)
		{
		case ERR_KEEPALIVE:
			//登录已失效
			printf("session is already invalid\n");
			//通知线程重新登录,先唤醒
			BLUEV_SetEvent(g_phLogIn);
			g_ConnSvr.bLogIn = 0;
			//return nResult;
		default:
			//唤醒
			BLUEV_SetEvent(g_phKeepAlive);
			return nResult;
		}
	}

	printf("keepalive ok\n");

	//唤醒心跳线程，使其退出指数退避
	if(0 > BLUEV_SetEvent(g_phKeepAlive))
		return -1;

	return 0;
}

int DigClient(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	long long						llSessionId;
	BLUEV_UDPReqHead	*pReq;
	char									*pszJson, *pszIp;
	int										nJsonLen, nPort, nRetVal;
	AppInfo							*pInfo;
	AppInfoNode				*pNode, *pNodeNew;

	nRetVal = -1;
	pszIp = NULL;
	nPort = 0;
	llSessionId = 0;
	pInfo = NULL;
	pNode = NULL;
	pNodeNew = NULL;

	pReq = (BLUEV_UDPReqHead *)pCtx->bnReqBuf;

	//parse json
	pszJson = (char *)pCtx->bnReqBuf + sizeof(*pReq);
	nJsonLen = strlen(pszJson);
	if(nJsonLen <= 0 || nJsonLen != Char2To_uint2(pReq->nDataLen))
		return -1;

	printf("receive digClient json:%s\n", pszJson);

	if(0 > Json2MainCtrl(pszJson, &llSessionId, &pszIp, &nPort) ||
		llSessionId <= 0 || pszIp == NULL || nPort <= 0)
		ERR_EXIT();

	//说明设备在服务器上，服务器无法获取外网IP
	if(0 == memcmp(pszIp, "127.0.0.1", strlen(pszIp)))
	{
		free(pszIp);
		pszIp = malloc(strlen(g_NetworkInfo.szRSIP) + 1);
		if(pszIp == NULL)
			ERR_EXIT();
		memset(pszIp, 0, strlen(g_NetworkInfo.szRSIP) + 1);
		memcpy(pszIp, g_NetworkInfo.szRSIP, strlen(g_NetworkInfo.szRSIP) );
	}

	//加入打洞任务列表
	pInfo = (AppInfo *)malloc(sizeof(AppInfo));
	if(pInfo == NULL)
		ERR_EXIT();
	memset(pInfo, 0, sizeof(AppInfo));

	pInfo->bConnected = 0;
	pInfo->llSessionId = llSessionId;
	memcpy(pInfo->szIp, pszIp, strlen(pszIp));
	pInfo->nPort = nPort;

	{
		struct tm	*t;
		time_t			tt;

		time(&tt);
		t = localtime(&tt);

		pInfo->tmCreate = *t;
		pInfo->tmUpdate = *t;
	}

	//初始化任务列表
	if(!g_listAppInfo.bInit)
	{
		InitAppInfoList(&g_listAppInfo);
	}
	//加入列表尾部
	pNodeNew = (AppInfoNode *)malloc(sizeof(AppInfoNode));
	if(pNodeNew == NULL)
		ERR_EXIT();
	memset(pNodeNew, 0, sizeof(*pNodeNew));

	pNodeNew->pData = pInfo;
	if(0 > QueryAppInfoList(&g_listAppInfo, &pNode, CompAppInfoListNode, pNodeNew) ||
		pNode == NULL)
	{
		//未找到则加入新任务
		AddAppInfoList(&g_listAppInfo, pNodeNew);
		pNode = pNodeNew;
		pInfo = NULL;
		pNodeNew = NULL;
	}
	else
	{
		//任务已存在
		ReleaseAppInfoListNode(pNodeNew);
		pNodeNew = NULL;
		pInfo = NULL;
	}

	if(pNode->pData->bConnected)
	{
		//打洞任务到来之前已连通
		pCtx->nAnsLen = 0;

		if(pNode->pData->nPort != nPort)
		{
			printf("打洞任务到来之前已连通，ip：%s,且端口已从%d更新为%d\n",
				pszIp, nPort, pNode->pData->nPort);
		}
		else
		{
			printf("打洞任务到来之前已连通，ip：%s, port:%d\n",
				pszIp, nPort);
		}

		nRetVal = 0;
		ERR_EXIT();
	}

	//未连通
	{
		BLUEV_UDPReqHead	req;
		struct	sockaddr_in sin;
		int			nAddrLen, nPortScn;

		InitReqHead(&req);
		uint2ToChar2(OP_CLASS_P2PCLIENT, req.nOpClass);
		uint2ToChar2(OP_P2P_DIG, req.nOpCode);

		//端口扫描，对称型路由需要猜测端口（效果并不好，范围可能会需要几w，当本机是Cone，不要扫描)
		//同一内网时不用，且不会调用此函数
		for(nPortScn = nPort - g_snRange; nPortScn <= nPort + g_snRange; nPortScn++)
		{
			if(nPortScn <= 0)
				continue;

			if(pNode->pData->bConnected)
				break;

			//连接
			if(0 > RebuildConn(pszIp, nPortScn, &sin, &nAddrLen))
				continue;

			// 发送一个数据包，进行打洞
			if(0 > sendto(g_ConnSvr.stConn.nSockfd, (char *)&req, sizeof(BLUEV_UDPReqHead), 0, (struct sockaddr *)&sin, nAddrLen))
				continue;

			//身份证明，发送自己的sessionid或对方的sessionid
		}
	}

	// 可不必回复。主控端若收不到，会持续请求。
	// 直到这里打洞成功。
	pCtx->nAnsLen = 0;

	printf("发送打洞数据包，ip：%s, port：%d\n", pszIp, nPort);

	nRetVal = 0;

Finish_Exit:
    if(pNodeNew)
        free(pNodeNew);

    if(pInfo)
		free(pInfo);

    if(pszIp)
		free(pszIp);

	return nRetVal;
}

int ProxyReq(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPReqHead	*pReq;
	char				*pszJson, *pData;
	int					nJsonLen, nDataLen;
	long long			llSessionId;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	pReq = (BLUEV_UDPReqHead *)pCtx->bnReqBuf;

	//parse json
	pszJson = (char *)pCtx->bnReqBuf + sizeof(*pReq);
	nJsonLen = strlen(pszJson);
	if(nJsonLen <= 0 || nJsonLen != Char2To_uint2(pReq->nDataLen))
		return -1;

	printf("receive proxyreq json:%s\n", pszJson);

	pData = NULL;
	nDataLen = 0;
	llSessionId = 0;
	if(0 > Json2ProxyData(pszJson, &llSessionId, &pData, &nDataLen) ||
		pData == NULL || nDataLen <= 0 || llSessionId <= 0)
	{
		if(pData)
			free(pData);

		return -1;
	}

	//填充代理模式信息
	//多个主控端同时代理请求时，此处sessionid会变。
	//我们只向最后一个反馈，由服务端保证同步。
	//考虑极端情况：此主控端发完请求立刻下线，此处sessionid更新为无效的，
	//向服务端反馈时，会因sessionid无效而报错。因此需要在json里加一个标记，
	//告诉服务端：即使sessionid无效，亦群发通知。
	g_ConnSvr.stProxyInfo.llSessionId = llSessionId;
	if(g_ConnSvr.stProxyInfo.bProxy == 0)
	{
		//服务端只有一个，不必每次更新
		g_ConnSvr.stProxyInfo.bProxy = 1;
	}

	//向环回接口发送
	//一种是使用pConn->stConn.nSockfd, 则recvfrom时需要区分两种包头
	//另一种是新开一个socket，另起线程专门接收设备应答，不能这里死等。
	//再包装成代理包发往服务器端，且还得是用pConn->stConn.nSockfd，否则端口会变。
	//因此还是采用第一种。。

	//构造设备连接信息
	{
		struct	sockaddr_in	addr;
		int			nAddrLen, nPort;

		nPort = g_NetworkInfo.nLocalPort;

		if(0 > RebuildConn(NETSOCKET_LOCIP, nPort, &addr, &nAddrLen))
		{
			if(pData)
				free(pData);

			return -1;
		}

		sendto(pConn->nSockfd, pData, nDataLen, 0, (struct sockaddr *)&addr, nAddrLen);

	}

	//不回复
	pCtx->nAnsLen = 0;

	if(pData)
		free(pData);

	return 0;
}


//只处理设备NetSocket发来请求或应答
int NetSocket_Service(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPAnsHead	*pAns;
	int					nRetVal = -1;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	//是否已登录
	if(g_ConnSvr.stUserId.llSessionId <= 0 || strlen(g_ConnSvr.stUserId.szGuid) <= 0)
		return -1;

	//当前是否为代理模式
	if(g_ConnSvr.stProxyInfo.bProxy && g_ConnSvr.stProxyInfo.llSessionId > 0)
	{
		//重建与服务端连接
		*pConn = g_ConnSvr.stConn;

		//代理模式
		nRetVal = ProxyAns(pConn, pCtx);

	}

	if(pCtx->nAnsLen > 0)
	{
		pAns = (BLUEV_UDPAnsHead *)pCtx->bnAnsBuf;
		uint2ToChar2(nRetVal, pAns->nRetVal);

		//P2P反馈
		//travel list
		{
			AppInfoNode	*pCur;

			pCur = g_listAppInfo.pFirst;

			while(pCur)
			{
				//
				{
					AppInfo	*pInfo;
					struct 		sockaddr_in	addr;
					int				nAddrLen;
					pInfo = (AppInfo *)pCur->pData;
					if(pInfo == NULL)
					{
						pCur = pCur->pNext;
						continue;
					}

					if(pInfo->bConnected == 0)
					{
						pCur = pCur->pNext;
						continue;
					}

					RebuildConn(pInfo->szIp, pInfo->nPort, &addr, &nAddrLen);

					sendto(g_ConnSvr.stConn.nSockfd, pAns, pCtx->nAnsLen, 0, (struct sockaddr *)&addr, (socklen_t)nAddrLen);

					printf("p2p ans to client:%lld\n", pInfo->llSessionId);
				}

				pCur = pCur->pNext;
			}
		}
	}

	return nRetVal;
}

int ProxyAns(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPAnsHead	*pAns;
	char				*pszJson, *pBase64Data;
	int					nJsonLen, nBaseLen;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	//构造应答头
	pAns = (BLUEV_UDPAnsHead *)pCtx->bnAnsBuf;
	InitAnsHead(pAns);

    //uint2ToChar2(OP_CLASS_P2PPROXYANS, pAns->nOpClass);
	uint2ToChar2(OP_CLASS_P2P, pAns->nOpClass);
	uint2ToChar2(OP_P2P_PROXYANS, pAns->nOpCode);

	//binary to base64
	pBase64Data = NULL;
	nBaseLen = 0;
	if(0 > BASE64_Encode((char *)pCtx->bnReqBuf, pCtx->nReqLen, &pBase64Data, &nBaseLen, 0) ||
		pBase64Data == NULL || nBaseLen <= 0)
		return -1;

	//build json
	pszJson = NULL;
	if(0 > ProxyData2Json(&g_ConnSvr, pBase64Data, &pszJson) ||
		pszJson == NULL)
	{
		free(pBase64Data);
		if(pszJson)
			free(pszJson);
	}

	nJsonLen = strlen(pszJson);
	printf("send proxyans json:%s\n", pszJson);

	memcpy(pCtx->bnAnsBuf + sizeof(*pAns), pszJson, nJsonLen);
	uint2ToChar2(nJsonLen, pAns->nDataLen);
	pCtx->nAnsLen = nJsonLen + sizeof(*pAns);

	if(pBase64Data)
		free(pBase64Data);
	if(pszJson)
		free(pszJson);

	return 0;
}

//只处理主控端发来请求或应答
int DigClientAns(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	//主控端反馈收到打洞包。

	//验证回复者身份 sessionid

	const char		*pszIP;
	int							nPort, nRetVal;
	AppInfoNode	*pNode;
	AppInfoNode	nodeQuery;
	AppInfo				info;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	nRetVal = -1;
	pNode = NULL;
	pszIP = inet_ntoa(pConn->fromaddr.sin_addr);
	nPort = ntohs(pConn->fromaddr.sin_port);
	///////////////////////////////////////

	memset(&nodeQuery, 0, sizeof(AppInfoNode));
	nodeQuery.pData = &info;

	memset(&info, 0, sizeof(info));
	memcpy(info.szIp, pszIP, strlen(pszIP));
	info.nPort = nPort;

	if(0 > QueryAppInfoList(&g_listAppInfo, &pNode, CompAppInfoListNode2, &nodeQuery) ||
		pNode == NULL)
	{
		//未找到，无效的反馈。
		printf("无效的主控端反馈收到打洞数据包！，ip：%s port:%d\n", pszIP, nPort);
		ERR_EXIT();
	}

	//修改端口号
	if(nPort != pNode->pData->nPort)
	{
		pNode->pData->nPort = nPort;
		printf("端口已更新\n");
	}

	pNode->pData->bConnected = 1;
	AppInfo_KeepAlive(pNode->pData);

	printf("主控端%lld反馈收到打洞数据包，连接成功！，ip：%s port:%d\n", pNode->pData->llSessionId, pszIP, nPort);

	//通知服务器

	//与主控端维持心跳

	//不回复
	pCtx->nAnsLen = 0;

	nRetVal = 0;
Finish_Exit:

	return nRetVal;
}

int KeepAliveByClient(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPReqHead	*pReq;
	int					nJsonLen, nRetVal;
	char				*pszJson;
	long long	llSessionId;

	AppInfoNode	*pNode;
	AppInfoNode	nodeQuery;
	AppInfo				info;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	nRetVal = -1;
	pNode = NULL;
	memset(&info, 0, sizeof(info));
	memset(&nodeQuery, 0, sizeof(nodeQuery));

   //parse json
	pReq = (BLUEV_UDPReqHead *)pCtx->bnReqBuf;
	pszJson = (char *)pCtx->bnReqBuf + sizeof(BLUEV_UDPReqHead);
	nJsonLen = strlen(pszJson);
	if(nJsonLen <= 0 || nJsonLen != Char2To_uint2(pReq->nDataLen))
		return -1;

	llSessionId = 0;
	if(0 > Json2SessionID(pszJson, &llSessionId) ||
		llSessionId <= 0)
		ERR_EXIT();

	nodeQuery.pData = &info;
	info.llSessionId = llSessionId;

	if(0 > QueryAppInfoList(&g_listAppInfo, &pNode, CompAppInfoListNode, &nodeQuery) ||
		pNode == NULL)
	{
		//未找到，无效的心跳。
		printf("无效的心跳数据包%lld！\n", llSessionId);
		ERR_EXIT();
	}
	else
	{
		AppInfo_KeepAlive(pNode->pData);
	}

	nRetVal = 0;
Finish_Exit:

	return nRetVal;
}

int ConnByClient(UDPConnection *pConn, UDPServiceParam *pCtx)
{
	BLUEV_UDPReqHead	*pReq;
	char						*pszIp, *pszJson;
	int							nPort, nJsonLen, nRetVal;
	long long			llSessionId;

	AppInfoNode	*pNode ;
	AppInfoNode	*pNodeQuery;
	AppInfo				*pInfo;

	if(pCtx == NULL || pConn == NULL)
		return -1;

	nRetVal = -1;
	pInfo = NULL;
	pNodeQuery = NULL;
	pNode = NULL;

	//收到连接消息，成功。

	//验证连接者身份 sessionid
	pszIp = inet_ntoa(pConn->fromaddr.sin_addr);
	nPort = ntohs(pConn->fromaddr.sin_port);

	//parse json
	pReq = (BLUEV_UDPReqHead *)pCtx->bnReqBuf;
	pszJson = (char *)pCtx->bnReqBuf + sizeof(BLUEV_UDPReqHead);
	nJsonLen = strlen(pszJson);
	if(nJsonLen <= 0 || nJsonLen != Char2To_uint2(pReq->nDataLen))
		return -1;

	llSessionId = 0;
	if(0 > Json2SessionID(pszJson, &llSessionId) ||
		llSessionId <= 0)
		ERR_EXIT();

	pInfo = (AppInfo *)malloc(sizeof(AppInfo));
	if(pInfo == NULL)
		ERR_EXIT();
	memset(pInfo, 0, sizeof(AppInfo));

	pNodeQuery = (AppInfoNode *)malloc(sizeof(AppInfoNode));
	if(pNodeQuery == NULL)
		ERR_EXIT();
	memset(pNodeQuery, 0, sizeof(AppInfoNode));

	pInfo->llSessionId = llSessionId;
	memcpy(pInfo->szIp, pszIp, strlen(pszIp));
	pInfo->nPort = nPort;
	pInfo->bConnected = 1;

	{
		struct tm	*t;
		time_t			tt;

		time(&tt);
		t = localtime(&tt);

		pInfo->tmCreate = *t;
		pInfo->tmUpdate = *t;
	}

	pNodeQuery->pData = pInfo;

	if(0 > QueryAppInfoList(&g_listAppInfo, &pNode, CompAppInfoListNode, pNodeQuery) ||
		pNode == NULL)
	{
		//没有打洞任务，可能是连接请求先于服务端到达
		printf("connByClient 先于服务端digClient到达\n");

		AddAppInfoList(&g_listAppInfo, pNodeQuery);
		pNode = pNodeQuery;
		pInfo = NULL;
		pNodeQuery = NULL;
	}
	else
	{
		//已有打洞任务，只更新

		//修改端口号
		if(nPort != pNode->pData->nPort)
		{
			pNode->pData->nPort = nPort;
			printf("端口已更新\n");
		}

		pNode->pData->bConnected = 1;
		AppInfo_KeepAlive(pNode->pData);
	}

	printf("收到打洞数据包,连接成功！，ip：%s port:%d\n", pszIp, nPort);

	//验证是否同一内网
	//自己判断IP
	//或者由主控端提供json信息

	//通知服务器

	//与主控端维持心跳

	//回复一个包头
	pCtx->nAnsLen = sizeof(BLUEV_UDPAnsHead);

	nRetVal = 0;
Finish_Exit:
	if(pInfo)
		free(pInfo);
	if(pNodeQuery)
		free(pNodeQuery);

	return nRetVal;
}

//json parse
int Json2Session(const char *pszJson, long long *pllSessionId, char **ppszGuid)
{
	cJSON	*root, *sessionid, *guid;

	long long	llSessionId;
	int			nszDataLen, nRetVal;
	char		*pszData;

	nRetVal = -1;

	if(pszJson == NULL || pllSessionId == NULL || ppszGuid == NULL)
		return -1;

	if(strlen(pszJson) <= 0)
		return -1;

	root = cJSON_Parse(pszJson);
	if(root == NULL)
		return -1;

	sessionid = cJSON_GetObjectItem(root, JSON_SESSIONID);
	guid = cJSON_GetObjectItem(root, JSON_GUID);

	if(sessionid == NULL || guid == NULL)
		ERR_EXIT();

	if(sessionid->type != cJSON_String || sessionid->valuestring == NULL)
		ERR_EXIT();

	if(guid->type != cJSON_String || guid->valuestring == NULL)
		ERR_EXIT();

	llSessionId = atoll(sessionid->valuestring);
	if(llSessionId <= 0)
		ERR_EXIT();

	nszDataLen = strlen(guid->valuestring);
	if(nszDataLen <= 0)
		ERR_EXIT();

	pszData = (char *)malloc(nszDataLen + 1);
	if(pszData == NULL)
		ERR_EXIT();

	memset(pszData, 0, nszDataLen + 1);

	memcpy(pszData, guid->valuestring, nszDataLen);

	*ppszGuid = pszData;
	*pllSessionId = llSessionId;

	nRetVal = 0;

Finish_Exit:
	if(root)
		cJSON_Delete(root);

	return nRetVal;
}

int Json2SessionID(const char *pszJson, long long *pllSessionId)
{
	cJSON	*root, *sessionid;

	long long	llSessionId;
	int					nRetVal;

	nRetVal = -1;

	if(pszJson == NULL || pllSessionId == NULL)
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

	llSessionId = atoll(sessionid->valuestring);
	if(llSessionId <= 0)
		ERR_EXIT();

	*pllSessionId = llSessionId;

	nRetVal = 0;

Finish_Exit:
	if(root)
		cJSON_Delete(root);

	return nRetVal;
}

int Json2ProxyData(const char *pszJson, long long *pllSessionId, char **ppData, int *pnDataLen)
{
	cJSON	*root, *sessionid, *proxydata;

	long long	llSessionId;
	int			nszDataLen, nDataLen, nRetVal;
	char		*pData;

	nRetVal = -1;
	if(pszJson == NULL || pllSessionId == NULL || ppData == NULL || pnDataLen == NULL)
		return -1;

	if(strlen(pszJson) <= 0)
		return -1;

	root = cJSON_Parse(pszJson);
	if(root == NULL)
		return -1;

	sessionid = cJSON_GetObjectItem(root, JSON_SESSIONID);
	proxydata = cJSON_GetObjectItem(root, JSON_PROXYDATA);

	if(sessionid == NULL || proxydata == NULL)
		ERR_EXIT();

	if(sessionid->type != cJSON_String || sessionid->valuestring == NULL)
		ERR_EXIT();

	if(proxydata->type != cJSON_String || proxydata->valuestring == NULL)
		ERR_EXIT();

	llSessionId = atoll(sessionid->valuestring);
	if(llSessionId <= 0)
		ERR_EXIT();

	nszDataLen = strlen(proxydata->valuestring);
	if(nszDataLen <= 0)
		ERR_EXIT();

	pData = NULL;
	nDataLen = 0;
	if(0 > BASE64_DecodeEx(proxydata->valuestring, nszDataLen, &pData, &nDataLen, 0) ||
		pData == NULL || nDataLen <= 0)
		ERR_EXIT();

	*ppData = pData;
	*pnDataLen = nDataLen;
	*pllSessionId = llSessionId;

	nRetVal = 0;

Finish_Exit:
	if(root)
		cJSON_Delete(root);

	return nRetVal;
}

int Json2MainCtrl(const char *pszJson, long long *pllSessionId, char **ppszIP, int *pnPort)
{
	cJSON	*root,  *mainctrl, *sessionid, *ip, *port;

	long long	llSessionId;
	int			nszDataLen, nRetVal, nPort;
	char		*pszData;

	nRetVal = -1;
	pszData = NULL;
	nszDataLen = 0;

	if(pszJson == NULL || pllSessionId == NULL || ppszIP == NULL || pnPort == NULL)
		return -1;

	if(strlen(pszJson) <= 0)
		return -1;

	root = cJSON_Parse(pszJson);
	if(root == NULL)
		return -1;

	mainctrl = cJSON_GetObjectItem(root, JSON_MAINCTRL);
	if(mainctrl == NULL)
		ERR_EXIT();

	sessionid = cJSON_GetObjectItem(mainctrl, JSON_SESSIONID);
	ip = cJSON_GetObjectItem(mainctrl, JSON_IP);
	port = cJSON_GetObjectItem(mainctrl, JSON_PORT);

	if(sessionid ==  NULL || ip == NULL || port == NULL)
		ERR_EXIT();

	if(sessionid->type != cJSON_String || sessionid->valuestring == NULL)
		ERR_EXIT();

	if(ip->type != cJSON_String || ip->valuestring == NULL)
		ERR_EXIT();

	if(port->type != cJSON_Number || port->valueint <= 0)
		ERR_EXIT();

	llSessionId = atoll(sessionid->valuestring);
	if(llSessionId <= 0)
		ERR_EXIT();

	nszDataLen = strlen(ip->valuestring);
	if(nszDataLen <= 0)
		ERR_EXIT();

	pszData = (char *)malloc(nszDataLen + 1);
	if(pszData == NULL)
		ERR_EXIT();

	memset(pszData, 0, nszDataLen + 1);

	memcpy(pszData, ip->valuestring, nszDataLen);

	nPort = port->valueint;
	if(nPort <= 0)
		ERR_EXIT();

	*ppszIP = pszData;
	*pllSessionId = llSessionId;
	*pnPort = nPort;

	nRetVal = 0;

Finish_Exit:
	if(root)
		cJSON_Delete(root);
	if(nRetVal < 0)
	{
		if(pszData)
			free(pszData);
	}
	return nRetVal;
}

//json build
int ProxyData2Json(P2PConnection *pConn, char *pszBase64Data, char **ppszJson)
{
	cJSON	*root, *session, *mainctrl;
	char	*pszJson;
	char	szBuf[64] = {0};

	if(pConn == NULL || pszBase64Data == NULL || ppszJson == NULL)
		return -1;

	root = cJSON_CreateObject();
	if(!root)
		return -1;
	session = cJSON_CreateObject();
	if(!session)
		return -1;
	mainctrl = cJSON_CreateObject();
	if(!mainctrl)
		return -1;

	sprintf(szBuf, "%lld", pConn->stUserId.llSessionId);
	cJSON_AddStringToObject(session, JSON_SESSIONID, szBuf);

	cJSON_AddStringToObject(session, JSON_GUID, pConn->stUserId.szGuid);

	sprintf(szBuf, "%lld", pConn->stProxyInfo.llSessionId);
	cJSON_AddStringToObject(mainctrl, JSON_SESSIONID, szBuf);
	cJSON_AddNumberToObject(mainctrl, JSON_BROADCAST, 1);

	cJSON_AddStringToObject(root, JSON_PROXYDATA, pszBase64Data);
	cJSON_AddItemToObject(root, JSON_MAINCTRL, mainctrl);
	cJSON_AddItemToObject(root, JSON_SESSION, session);

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

int UserData2Json(LogInContext *pContext, char **ppszJson)
{
	cJSON	*root;
	char	*pszJson;

	if(pContext == NULL || ppszJson == NULL)
		return -1;

	root = cJSON_CreateObject();
	if(!root)
		return -1;

	cJSON_AddStringToObject(root, JSON_LANIP, pContext->pszLanIp);
	cJSON_AddNumberToObject(root, JSON_LANPORT, pContext->nLanPort);
	cJSON_AddStringToObject(root, JSON_MASK, pContext->pszMask);
	cJSON_AddNumberToObject(root, JSON_ISMAINCTRL, 0);
	cJSON_AddStringToObject(root, JSON_DEVID, pContext->pszDevId);
	cJSON_AddStringToObject(root, JSON_GATEWAY, pContext->pszGateWay);

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
	cJSON	*root;
	char	*pszJson;
	char	szBuf[64] = {0};

	if(pConn == NULL || ppszJson == NULL)
		return -1;

	root = cJSON_CreateObject();
	if(!root)
		return -1;

	sprintf(szBuf, "%lld", pConn->stUserId.llSessionId);
	cJSON_AddStringToObject(root, JSON_SESSIONID, (char *)szBuf);
	cJSON_AddStringToObject(root, JSON_GUID, (char *)pConn->stUserId.szGuid);

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

int SessionId2Json(P2PConnection *pConn, char **ppszJson)
{
	cJSON	*root;
	char	*pszJson;
	char	szBuf[64] = {0};

	if(pConn == NULL || ppszJson == NULL)
		return -1;

	root = cJSON_CreateObject();
	if(!root)
		return -1;

	sprintf(szBuf, "%lld", pConn->stUserId.llSessionId);
	cJSON_AddStringToObject(root, JSON_SESSIONID, (char *)szBuf);
	//cJSON_AddStringToObject(root, JSON_GUID, (char *)pConn->stUserId.szGuid);

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

int IsReqHeadValid(BLUEV_UDPReqHead *pReq)
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

int InitReqHead(BLUEV_UDPReqHead *pReq)
{
	if(pReq == NULL)
		return -1;

	memset(pReq, 0, sizeof(*pReq));
	uint2ToChar2(sizeof(*pReq), pReq->cbSize);
	memcpy(pReq->szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
	pReq->nMajorVer = BV_MAJORVER;

	return 0;
}

int IsAnsHeadValid(BLUEV_UDPAnsHead *pAns)
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

int InitAnsHead(BLUEV_UDPAnsHead *pAns)
{
	if(pAns == NULL)
		return -1;

	memset(pAns, 0, sizeof(*pAns));
	uint2ToChar2(sizeof(*pAns), pAns->cbSize);
	memcpy(pAns->szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
	pAns->nMajorVer = BV_MAJORVER;

	return 0;
}

//////////////////链表////////////////////////////////
int InitAppInfoList(AppInfoLinkList *pList)
{
	if(pList == NULL)
		return -1;

	pList->pFirst = pList->pLast = NULL;
	pList->bInit = 1;

	return 0;
}

int ReleaseAppInfoList(AppInfoLinkList *pList)
{
	if(0 > IsValidAppInfoList(pList))
		return -1;

	TravelAppInfoList(pList, ReleaseAppInfoListNode);
	free(pList->pLast);

	pList->pFirst = pList->pLast = NULL;
	pList->bInit = 0;

	return 0;
}

int AddAppInfoList(AppInfoLinkList *pList, AppInfoNode *pNode)
{
	if(0 > IsValidAppInfoList(pList) || pNode == NULL)
		return -1;

	pNode->pNext = pNode->pPre = NULL;

	if(pList->pLast == NULL)
	{
		pList->pLast = pList->pFirst = pNode;
		return 0;
	}

	pList->pLast->pNext = pNode;
	pNode->pPre = pList->pLast;

	pList->pLast = pNode;

	return 0;
}

int IsValidAppInfoList(AppInfoLinkList *pList)
{
	if(pList == NULL || pList->pFirst == NULL ||
		pList->pLast == NULL || !pList->bInit)
		return -1;

	return 0;
}

int TravelAppInfoList(AppInfoLinkList *pList, AppInfoLinkList_Func func)
{
	AppInfoNode	*pCur;

	if(0 > IsValidAppInfoList(pList))
		return -1;

	pCur = pList->pFirst;

	while(pCur)
	{
		if(func)
			func(pCur);

		pCur = pCur->pNext;
	}

	return 0;
}

int ReleaseAppInfoListNode(AppInfoNode *pNode)
{
	if(pNode == NULL)
		return -1;

	if(pNode->pPre)
	{
		free(pNode->pPre);
	}

	if(pNode->pData)
	{
		free(pNode->pData);
	}

	return 0;
}

int QueryAppInfoList(AppInfoLinkList *pList, AppInfoNode **ppNode, AppInfoLinkList_CompFunc func, AppInfoNode *pNodeComp)
{
	AppInfoNode	*pCur;

	if(0 > IsValidAppInfoList(pList) || ppNode == NULL || func == NULL || pNodeComp == NULL)
		return -1;

	pCur = pList->pFirst;

	while(pCur)
	{
		//查询条件
		if(func(pCur, pNodeComp))
		{
			*ppNode = pCur;
			return 0;
		}

		pCur = pCur->pNext;
	}

	return 0;
}

int CompAppInfoListNode(AppInfoNode *pNode, AppInfoNode *pNodeComp)
{
	if(pNode == NULL || pNodeComp == NULL)
		return 0;

    if(pNode->pData->llSessionId == pNodeComp->pData->llSessionId)
		return 1;

	return 0;
}

int CompAppInfoListNode2(AppInfoNode *pNode, AppInfoNode *pNodeComp)
{
	if(pNode == NULL || pNodeComp == NULL)
		return 0;

	if(0 == memcmp(pNode->pData->szIp, pNodeComp->pData->szIp, sizeof(pNodeComp->pData->szIp)))
		return 1;

	return 0;
}

int AppInfo_KeepAlive(AppInfo *pInfo)	//刷新更新时间
{
	struct tm	*t;
	time_t			tt;

	if(pInfo == NULL)
		return -1;

	//lock
	time(&tt);
	t = localtime(&tt);

	pInfo->tmUpdate = *t;
	//unlock
	return 0;
}



