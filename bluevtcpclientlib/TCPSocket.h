#ifndef TCPSOCKET_H_INCLUDED
#define TCPSOCKET_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include "bluevtcpclientlib.h"

//代理中转，或P2P打通后，转发协议2给本地环回
#define NETSOCKET_LOCIP	"127.0.0.1"

//心跳超时重发
#define KEEPALIVE_TIMEOUT	30
#define LOGIN_TIMEOUT	20

//**** JSON ****
#define JSON_SVRIP		"svrip"
#define JSON_SVRPORT	"svrport"
#define JSON_LANIP		"lanip"
#define JSON_LANPORT	"lanport"
#define JSON_MASK		"mask"
#define JSON_GATEWAY		"gateway"
#define JSON_MAC		"mac"
#define JSON_DEVID		"devid"
#define JSON_DEVTYPE	"devtype"
#define JSON_REMARK		"remark"
#define JSON_ISMAINCTRL	"ismainctrl"
#define JSON_USERNAME	"username"
#define JSON_PASSWORD	"password"

#define JSON_SESSION	"session"
#define JSON_DEVINFO	"devinfo"
#define JSON_PROXYDATA	"proxydata"
#define JSON_STAMP		"stamp"
#define JSON_ISUPNP		"isupnp"
#define JSON_UPNPPORT	"upnpport"
#define JSON_UPNPIP		"upnpip"

//ans
#define JSON_SESSIONID	"sessionid"
#define JSON_GUID		"guid"
#define JSON_IP			"ip"
#define	JSON_PORT		"port"
#define JSON_ISSAMELAN	"issamelan"

#define JSON_DEVS		"devs"
#define JSON_MAINCTRL	"mainctrl"
#define JSON_BROADCAST	"broadcast"

#define ERR_EXIT()	\
{	\
	goto Finish_Exit;	\
}


//**** Enum ****
typedef enum tagOPTCPCLASS
{
	OP_TCPCLASS_P2P = 1,	//请求
	OP_TCPCLASS_P2PCLIENT,	//客户端之间请求
	OP_TCPCLASS_P2PPROXY,	//服务器代理中转请求
} OPTCPCLASS;

typedef enum tagOPTCPCODE
{
	OP_TCP_LOGIN = 1,
	OP_TCP_LOGOUT = 2,
	OP_TCP_GETDEVINFO = 3,

	OP_TCP_KEEPALIVE = 7,

	OP_TCP_PROXY = 9,
	OP_TCP_PROXYANS = 10,
	OP_TCP_PROXYCONN,
} OPTCPCODE;

typedef enum tagErrTCPCode
{
	ERR_PARSEREQ = -1,	//解析请求信息错误
	ERR_BUILDANS = -2,	//构造返回信息错误
	ERR_LOGIN = -3,	//登陆错误（内存不足）
	ERR_LOGOUT = -4,	//登出错误（未登录，重复登出，guid不符）
	ERR_MCTRLID = -5,	//主控端身份验证失败（未登录，查询登陆信息失败，非主控端）
	ERR_GETDEVINFO = -6,	//获取设备列表失败
	ERR_KEEPALIVE = -7,	//保持心跳失败（未登录）
	ERR_UCTRLID = -8,	//被控端身份验证失败（未登录，查询登陆信息失败，非被控端）
	ERR_CONNDEV = -9,	//协助打洞失败
	ERR_PROXYREQ = -10,	//代理请求失败
	ERR_PROXYANS = -11,	//代理应答失败
	ERR_BUILDREQ = -12,	//构造请求信息错误
	ERR_PARSEANS = -13,	//构造请求信息错误
} ErrTCPCode;

//**** Public UTIL Function ****
unsigned short Char2To_uint2(const unsigned char *ch);
void uint2ToChar2(unsigned short w, unsigned char *ch);
unsigned int Char4To_uint4(const unsigned char *ch);
void uint4ToChar4(unsigned int dw, unsigned char *ch);

//UTIL FUNC
long long getTickCount();
double tickToSecond(long long llTimeUsed);

//封包定义——TCPSOCKET
#define	BVNRHO_MAGICSTR	"$bluev$"
#define BV_MAJORVER		1
#define BV_MINORVER		1

// 此结构的大小为32个字节。
typedef struct tagBLUEV_NetReqHead
{
	unsigned char cbSize[2];		// size of this structure
	unsigned char nMajorVer;		// major version, must be BV_MAJORVER
	unsigned char nMinorVer;		// minor version, must be BV_MINORVER
	char	szMagicStr[8];		// must be $bluev$
	unsigned char nOpClass[2];		// operation class, 2 bytes int
	unsigned char nOpCode[2];			// operation code
	unsigned char nDataLen[4];		// data length followed
	unsigned char reuseConnection;	// temp long conn，not suggest to use(iocp server no support)
	unsigned char longConnection;	// true long conn(only serverex suport, iocp server default long conn, needn't set this)
	//22 bytes here
	unsigned char bnRes[10];
} BLUEV_NetReqHead;

// 此结构的大小为32个字节。
typedef struct tagBLUEV_NetAnsHead
{
	unsigned char cbSize[2];		// size of this structure
	unsigned char nMajorVer;		// major version, must be BV_MAJORVER
	unsigned char nMinorVer;		// minor version, must be BV_MINORVER
	char	szMagicStr[8];		// must be $bluev$
	unsigned char nDataLen[4];		// data length followed
	unsigned char nRetVal[2];
	unsigned char hasException;		// 是否发生了错误。
	// 19 bytes here
	unsigned char bnRes[13];		// reserved
} BLUEV_NetAnsHead;

int IsReqHeadValid(BLUEV_NetReqHead *pReq);
int InitReqHead(BLUEV_NetReqHead *pReq);

int IsAnsHeadValid(BLUEV_NetAnsHead *pAns);
int InitAnsHead(BLUEV_NetAnsHead *pAns);

typedef struct tagTCPServiceParam
{
	unsigned char	*pReq;
	int								nReqLen;
	unsigned char	*pAns;
	int								nAnsLen;
	//unsigned char	bnIP[16];	// IP地址
	//int								nPort;		// 端口
} TCPServiceParam;

typedef struct tagUserId
{
	char	szSessionId[40];	//36 bytes uuid + \0
} UserId;

typedef struct tagTCPConnection
{
	BLUEV_NetReqHead	req;
	BLUEV_NetAnsHead	ans;
	int			nSockfd;
} TCPConnection;

typedef struct tagP2PProxyInfo
{
	char		szSessionId[40];	//代理请求者session id
	int			bProxy;					//是否是代理模式
} P2PProxyInfo;

typedef struct tagP2PConnection
{
	volatile int		bConnected;
	volatile int		bLogIn;		//已登录到服务端
	UserId					stUserId;	//P2P登陆信息
	P2PProxyInfo	stProxyInfo;//代理模式时信息
} P2PConnection;


typedef	struct tagEvent
{
	pthread_mutex_t	mutex;
	pthread_cond_t		cond;
} Event;


//**** JSON PARSE Function ****
int Json2Session(const char *pszJson, char **ppszSessionId);
int Json2SessionID(const char *pszJson, char **ppszSessionId);
int Json2DeviceInfo(const char *pszJson, DeviceInfo *pDev);


//**** JSON BUILD Function ****
int UserData2Json(LogInContext *pContext, char **ppszJson);
int Session2Json(P2PConnection *pConn, char **ppszJson);
int SessionId2Json(P2PConnection *pConn, char **ppszJson);

//**** Service Function ****

//**** TCPServer Function ****
int InitServer();
int OpenTCPServer(int nPort);
int StartTCPServer(const char *pszSvrIP, int nSvrPort);
int CloseTCPServer(void);
int IsLogin();
int IsConnect();

//**** Operation Function ****
int LogIn(LogInContext	*pContext);
int LogOut();
int KeepAlive(int nSecond);
int GetDevice(char **ppszInfo);

int SetLoginContext(LogInContext	*pContext);
#endif // TCPSOCKET_H_INCLUDED
