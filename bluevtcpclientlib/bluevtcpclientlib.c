#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#ifndef	SYSTEM_MAC
#include <linux/rtnetlink.h>    //for rtnetlink
#endif

#include <netinet/in.h>
#include <netdb.h>

#include "cJSON.h"
#include "bluevtcpclientlib.h"
#include "TCPSocket.h"

static LogInContext	g_ctxLogin;
static char					g_bInit = 0;
static char					g_szSvrIP[16] = {0};
static int						g_nSvrPort = 0;

static char					g_calledKeepAlive = 0;  //限定只能调用一次
static char					g_calledLogIn = 0;

 static char 					g_szIFName[12] = {0};

#define	ETH_NAME   "eth0"
#define	BUFSIZE	8192

#ifndef	SYSTEM_MAC
struct route_info
{
    u_int dstAddr;
    u_int srcAddr;
    u_int gateWay;
    char ifName[IF_NAMESIZE];
};

static int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
    struct	nlmsghdr *nlHdr;
    int 		readLen = 0,  msgLen = 0;

    do{

        if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
        {
            perror("SOCK READ: ");
            return -1;
        }

        nlHdr = (struct nlmsghdr *)bufPtr;
        if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
        {
            perror("Error in recieved packet");
            return -1;
        }

        if(nlHdr->nlmsg_type == NLMSG_DONE)
        {
            break;
        }
        else
        {
            bufPtr += readLen;
            msgLen += readLen;
        }

        if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
            break;

    } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));

    return msgLen;
}

static void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway, char *ifName)
{
    struct rtmsg	*rtMsg;
    struct rtattr	*rtAttr;
    int 					rtLen;
    char 					*tempBuf = NULL;
    struct in_addr	dst;
    struct in_addr	gate;

    tempBuf = (char *)malloc(100);
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

    // If the route is not for AF_INET or does not belong to main routing table
    //then return.
    if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
        return;

    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);

    for(; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
    {
        switch(rtAttr->rta_type)
		{
        case RTA_OIF:
            if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
            break;
        case RTA_GATEWAY:
            rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_PREFSRC:
            rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_DST:
            rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
            break;
        }
    }

    dst.s_addr = rtInfo->dstAddr;
    if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))
    {
        sprintf(ifName, "%s", rtInfo->ifName);
        //printf("oif:%s",rtInfo->ifName);
        gate.s_addr = rtInfo->gateWay;
        sprintf(gateway, "%s", (char *)inet_ntoa(gate));
        //printf("%s\n",gateway);
        gate.s_addr = rtInfo->srcAddr;
        //printf("src:%s\n",(char *)inet_ntoa(gate));
        gate.s_addr = rtInfo->dstAddr;
        //printf("dst:%s\n",(char *)inet_ntoa(gate));
    }

    free(tempBuf);
    return;
}

static int getGateWay(char *gateway, char *ifName)
{
    struct nlmsghdr	*nlMsg;
//    struct rtmsg *rtMsg;
    struct route_info	*rtInfo;
    char							msgBuf[BUFSIZE];

    int	sock, len, msgSeq = 0;

    if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
        perror("Socket Creation: ");
        return -1;
    }

    memset(msgBuf, 0, BUFSIZE);
    nlMsg = (struct nlmsghdr *)msgBuf;
//    rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

    nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
    nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
    nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.

    if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0)
    {
        printf("Write To Socket Failed\n");
        return -1;
    }

    if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0)
    {
        printf("Read From Socket Failed\n");
        return -1;
    }

    rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
    for(; NLMSG_OK(nlMsg, len); nlMsg = NLMSG_NEXT(nlMsg, len))
    {
        memset(rtInfo, 0, sizeof(struct route_info));
        parseRoutes(nlMsg, rtInfo, gateway, ifName);
    }

    free(rtInfo);
    close(sock);
    return 0;
}

static int getLocalInfo(LogInContext *pCtx)
{
	if(pCtx == NULL)
		return -1;

	if(0 > getGateWay(pCtx->szGateWay, g_szIFName))
		return -1;

	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(0 > sockfd)
        return -1;

	struct	ifreq ifr;
	memset(&ifr, 0, sizeof ifr);
	//strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ);
	strncpy(ifr.ifr_name, g_szIFName, strlen(g_szIFName));

	if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
		return -1;

	struct sockaddr_in	*host = (struct sockaddr_in *)&ifr.ifr_addr;
	if(!host)	return -1;
	strcpy(pCtx->szLanIp, inet_ntoa(host->sin_addr));


	 if(0 > ioctl(sockfd,SIOCGIFHWADDR,&ifr))
		return -1;

	sprintf(pCtx->szMac,"%02x-%02x-%02x-%02x-%02x-%02x",
								(unsigned char)ifr.ifr_hwaddr.sa_data[0],
								(unsigned char)ifr.ifr_hwaddr.sa_data[1],
								(unsigned char)ifr.ifr_hwaddr.sa_data[2],
								(unsigned char)ifr.ifr_hwaddr.sa_data[3],
								(unsigned char)ifr.ifr_hwaddr.sa_data[4],
								(unsigned char)ifr.ifr_hwaddr.sa_data[5]);

	if(0 > ioctl(sockfd,SIOCGIFNETMASK,&ifr))
		return -1;

	strcpy(pCtx->szMask, inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_netmask))->sin_addr));

	return 0;
}

#else
static int getLocalInfo(LogInContext *pCtx)
{
	//MAC 不支持linux方式获取本地信息
	return 0;
}

#endif

/*
//主控端JSON
{
	"svrip":"",
	"svrport":,
	“lanport”:内网端口,
	“ismainctrl”:是否主控端， //1 主控端  0 被控端
	“username”:”账号”,
	“password”:”密码MD5 的16进制表示”
}

//被控端JSON
{
	"svrip":"",
	"svrport":,
	“lanport”:内网端口,	//注意，被控端时，此端口表示设备另起的server port，即UPNPPORT
	“ismainctrl”:是否主控端， //1 主控端  0 被控端
	“devid”:”设备id”,
	"isupnp":是否UPNP,
	"upnpip":"UPNPIP",
	"upnpport":UPNPPORT
}
*/

//json parse
static int Json2Param(const char *pszJson, LogInContext **ppCtx)
{
	cJSON	*root, *svrip, *svrport, *lanport, *ismainctrl;
	cJSON	*username, *password;
	cJSON	*devid, *isupnp, *upnpip, *upnpport;

	int			nRetVal, nItemLen;
	LogInContext	*pCtx = NULL;

	nRetVal = -1;

	if(pszJson == NULL || ppCtx == NULL)
		return -1;

	if(strlen(pszJson) == 0)
		return -1;

	root = cJSON_Parse(pszJson);
	if(root == NULL)
		return -1;

	pCtx = (LogInContext *)malloc(sizeof(LogInContext));
	if(pCtx == NULL)
		return -1;

	memset(pCtx, 0, sizeof(*pCtx));

	svrip = cJSON_GetObjectItem(root, JSON_SVRIP);
	svrport = cJSON_GetObjectItem(root, JSON_SVRPORT);

	lanport = cJSON_GetObjectItem(root, JSON_LANPORT);
	ismainctrl = cJSON_GetObjectItem(root, JSON_ISMAINCTRL);

	if(svrip == NULL || svrport == NULL || lanport == NULL || ismainctrl == NULL)
		ERR_EXIT();

	if(svrip->type != cJSON_String || svrip->valuestring == NULL)
		ERR_EXIT();
	if(svrport->type != cJSON_Number || svrport->valueint <= 0)
		ERR_EXIT();

	if(lanport->type != cJSON_Number || lanport->valueint <= 0)
			ERR_EXIT();
	if(ismainctrl->type != cJSON_Number || ismainctrl->valueint < 0)
		ERR_EXIT();

	if((nItemLen = strlen(svrip->valuestring)) >= sizeof(g_szSvrIP))
		ERR_EXIT();
	memcpy(g_szSvrIP, svrip->valuestring, nItemLen);

	g_nSvrPort = svrport->valueint;

	pCtx->nLanPort = lanport->valueint;
	pCtx->bMainCtrl = ismainctrl->valueint ? 1 : 0;

	if(pCtx->bMainCtrl)
	{
		username = cJSON_GetObjectItem(root, JSON_USERNAME);
		password = cJSON_GetObjectItem(root, JSON_PASSWORD);

		if(username == NULL || password == NULL)
			ERR_EXIT();
		if(username->type != cJSON_String || username->valuestring == NULL)
			ERR_EXIT();
		if(password->type != cJSON_String || password->valuestring == NULL)
			ERR_EXIT();

		if((nItemLen = strlen(username->valuestring)) >= sizeof(pCtx->szUserName))
			ERR_EXIT();
		memcpy(pCtx->szUserName, username->valuestring, nItemLen);
		if((nItemLen = strlen(password->valuestring)) >= sizeof(pCtx->szPassWord))
			ERR_EXIT();
		memcpy(pCtx->szPassWord, password->valuestring, nItemLen);
	}
	else
	{
		devid = cJSON_GetObjectItem(root, JSON_DEVID);
		if(devid == NULL)
			ERR_EXIT();
		if(devid->type != cJSON_String || devid->valuestring == NULL)
			ERR_EXIT();

		if((nItemLen = strlen(devid->valuestring)) >= sizeof(pCtx->szDevId))
			ERR_EXIT();
		memcpy(pCtx->szDevId, devid->valuestring, nItemLen);

		isupnp = cJSON_GetObjectItem(root, JSON_ISUPNP);

		//可以没有此节点。
		if(isupnp)
		{
			if(isupnp->type != cJSON_Number || isupnp->valueint < 0)
				ERR_EXIT();

			pCtx->bUPNP = (isupnp->valueint) ? 1 : 0;

			if(pCtx->bUPNP)
			{
				upnpip = cJSON_GetObjectItem(root, JSON_UPNPIP);
				upnpport = cJSON_GetObjectItem(root, JSON_UPNPPORT);

				if(upnpip == NULL || upnpport == NULL)
					ERR_EXIT();

				if(upnpip->type != cJSON_String || upnpip->valuestring == NULL)
					ERR_EXIT();

				if((nItemLen = strlen(upnpip->valuestring)) >= sizeof(pCtx->szUPNPIP))
					ERR_EXIT();
				memcpy(pCtx->szUPNPIP, upnpip->valuestring, nItemLen);

				if(upnpport->type != cJSON_Number || upnpport->valueint <= 0)
					ERR_EXIT();

				pCtx->nUPNPPort = upnpport->valueint;
			}
		}

	}

	*ppCtx = pCtx;

	nRetVal = 0;

Finish_Exit:
	if(root)
		cJSON_Delete(root);

	if(nRetVal != 0)
	{
		if(pCtx)
			free(pCtx);
	}

	return nRetVal;
}


int TCPCLIENT_Init(const char *pszJson)
{
	if(pszJson == NULL)
		return -1;

	if(g_bInit)
		return 0;

	//parse json
	if(0)
	{
		int		nSvrPort = 8878;
		char	*pszSvrIP = "123.57.72.97";
		char	*pszDevId = "deviceid1";
		char	*pszUserName = "admin1";
		char	*pszPassWord = "admin1";

		LogInContext	ctx;
		memset(&ctx, 0, sizeof(ctx));

		if(0 > getLocalInfo(&ctx))
			return -1;

		ctx.nLanPort = 9978;
		ctx.bMainCtrl = 1;

		if(ctx.bMainCtrl )
		{
				memcpy(ctx.szUserName, pszUserName, strlen(pszUserName));
				memcpy(ctx.szPassWord, pszPassWord, strlen(pszPassWord));
		}
		else
		{
			//被控端
			memcpy(ctx.szDevId, pszDevId, strlen(pszDevId));

			ctx.bUPNP = 1;
			memcpy(ctx.szUPNPIP, ctx.szLanIp, strlen(ctx.szLanIp));
			ctx.nUPNPPort = 9991;
		}

		g_ctxLogin = ctx;

		memcpy(g_szSvrIP, pszSvrIP, strlen(pszSvrIP));
		g_nSvrPort = nSvrPort;
	}
	else
	{
		LogInContext	*pCtx = NULL;

		if(0 > Json2Param(pszJson, &pCtx) || pCtx == NULL)
			return -1;

		if(0 > getLocalInfo(pCtx))
		{
			free(pCtx);
			return -1;
		}

		g_ctxLogin = *pCtx;

		free(pCtx);
	}

	if(0 > InitServer())
		return -1;

	g_bInit = 1;

	return 0;
}

int TCPCLIENT_Start(void)
{
	if(!g_bInit)
		return -1;

	OpenTCPServer(g_ctxLogin.nLanPort);
	StartTCPServer(g_szSvrIP, g_nSvrPort);

	return 0;
}

int TCPCLIENT_End(void)
{
	if(!g_bInit)
		return -1;

	CloseTCPServer();

	g_bInit = 0;
	g_calledLogIn = 0;
	g_calledKeepAlive = 0;

	return 0;
}

int TCPCLIENT_Login(void)
{
	if(!g_bInit)
		return -1;

	if(g_calledLogIn == 1)
		return 0;

	int	nRet = LogIn(&g_ctxLogin);
	if(nRet == 0)
	{
		g_calledLogIn = 1;
	}

	return nRet;
}

int TCPCLIENT_IsLogin(void)
{
	return IsLogin();
}

int TCPCLIENT_IsConnect(void)
{
	return IsConnect();
}

int TCPCLIENT_Logout(void)
{
	if(!g_bInit)
		return -1;

	return LogOut();
}

int TCPCLIENT_Keepalive(int nSecond)
{
	if(!g_bInit)
		return -1;

	if(g_calledKeepAlive == 1)
		return 0;

	int	nRet = KeepAlive(nSecond);
	if(nRet == 0)
	{
		g_calledKeepAlive = 1;
	}

	return nRet;
}

int TCPCLIENT_GetDevice(char **ppszInfo)
{
	if(!g_bInit || ppszInfo ==  NULL)
		return -1;

	return GetDevice(ppszInfo);
}


/***********************************************************************/
int TCPCLIENT_InitEx(LogInContext *pCtx, char szServerIP[16], int nServerPort)
{
	if(pCtx == NULL || szServerIP == NULL || nServerPort <= 0 || strlen(szServerIP) >= sizeof(g_szSvrIP))
		return -1;

	if(g_bInit)
		return 0;

	strcpy(g_szSvrIP, szServerIP);

	g_nSvrPort = nServerPort;

	{
		if(0 > getLocalInfo(pCtx))
			return -1;

		g_ctxLogin = *pCtx;

	}

	if(0 > InitServer())
		return -1;

	g_bInit = 1;

	return 0;
}

int TCPCLIENT_GetLocalInfo(LogInContext *pCtx)
{
	if(pCtx == NULL)
		return -1;

	return getLocalInfo(pCtx);
}

LogInContext TCPCLIENT_QueryLogInCtx()
{
    return g_ctxLogin;
}

int TCPCLIENT_SetLogInCtx(LogInContext *pCtx)
{
	if(pCtx == NULL)
		return -1;

    if(pCtx == &g_ctxLogin)
		return 0;

	g_ctxLogin = *pCtx;

	SetLoginContext(pCtx);

	return 0;
}

int TCPCLIENT_GetDeviceEx(DeviceInfo *pDevInfo)
{
	if(!g_bInit || pDevInfo == NULL)
		return -1;

	char	*pszInfo = NULL;
	if(0 > GetDevice(&pszInfo) || pszInfo == NULL)
		return -1;

	printf("GetDevice ok\n");
	if(0 > Json2DeviceInfo(pszInfo, pDevInfo))
	{
		free(pszInfo);
		return -1;
	}

	free(pszInfo);
	return 0;
}
