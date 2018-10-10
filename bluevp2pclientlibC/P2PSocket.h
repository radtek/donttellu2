#ifndef P2PSOCKET_H_INCLUDED
#define P2PSOCKET_H_INCLUDED

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
#include <linux/rtnetlink.h>    //for rtnetlink
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

//心跳超时重发
#define KEEPALIVE_TIMEOUT	30
#define LOGIN_TIMEOUT	20

//**** JSON ****
#define JSON_LANIP		"lanip"
#define JSON_LANPORT	"lanport"
#define JSON_MASK		"mask"
#define JSON_MAC		"mac"
#define JSON_DEVID		"devid"
#define JSON_DEVTYPE	"devtype"
#define JSON_REMARK		"remark"
#define JSON_ISMAINCTRL	"ismainctrl"

#define JSON_SESSION	"session"
#define JSON_DEVINFO	"devinfo"
#define JSON_PROXYDATA	"proxydata"

//ans
#define JSON_SESSIONID	"sessionid"
#define JSON_GUID		"guid"
#define JSON_IP			"ip"
#define	JSON_PORT		"port"
#define JSON_ISSAMELAN	"issamelan"

#define JSON_DEVS		"devs"
#define JSON_MAINCTRL	"mainctrl"

#define ERR_EXIT()	\
{	\
	goto Finish_Exit;	\
}

//**** Enum ****
typedef enum tagOPCLASS
{
	OP_CLASS_P2P = 1,		//请求
	OP_CLASS_P2PANS,		//应答
	OP_CLASS_P2PCLIENT,		//客户端之间请求
	OP_CLASS_P2PCLIENTANS,	//客户端之间应答
	OP_CLASS_P2PPROXY,		//服务器代理中转请求
	OP_CLASS_P2PPROXYANS,	//服务器代理中转应答
	OP_CLASS_USER,
} OPCLASS;

typedef enum tagOPCODE
{
	OP_P2P_LOGIN = 1,
	OP_P2P_LOGOUT = 2,
	OP_P2P_GETDEVINFO = 3,
	OP_P2P_CONNDEV = 4,
	OP_P2P_DIG = 5,
	OP_P2P_DOWNLOAD = 6,
	OP_P2P_KEEPALIVE = 7,
	OP_P2P_UPLOAD = 8,
	OP_P2P_PROXY = 9,
	OP_P2P_PROXYANS = 10,
} OPCODE;

typedef enum tagErrCode
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
	} ErrCode;

//**** Public UTIL Function ****
unsigned short Char2To_uint2(const unsigned char *ch);
void uint2ToChar2(unsigned short w, unsigned char *ch);

//封包定义——NETSOCKET
#define NETSOCKET_HEAD	"####Json="
#define NETSOCKET_TAIL	"****"

//封包定义——P2PSOCKET
#define	BVNRHO_MAGICSTR	"$bluev$"
#define BV_MAJORVER		1
#define BV_MINORVER		0

// 此结构的大小为32个字节。
typedef struct tagBLUEV_UDPReqHead
{
	unsigned char	cbSize[2];			// size of this structure, 当前为32个字节。
	unsigned char	nMajorVer;			// major version, must be BV_MAJORVER
	unsigned char	nMinorVer;			// minor version, must be BV_MINORVER
	char	szMagicStr[8];				// must be $bluev$
	unsigned char	nOpClass[2];		// operation class, 2 bytes int
	unsigned char	nOpCode[2];			// operation code
	unsigned char	nDataLen[2];		// data length followed max 2^16 = 64k
	// 到这里是18个字节。
	unsigned char	bnRes[14];
	// 到这里是32个字节。
} BLUEV_UDPReqHead;

// 此结构的大小为32个字节。
typedef struct tagBLUEV_UDPAnsHead
{
	unsigned char	cbSize[2];			// size of this structure, 当前为32个字节。
	unsigned char	nMajorVer;			// major version, must be BV_MAJORVER
	unsigned char	nMinorVer;			// minor version, must be BV_MINORVER
	char	szMagicStr[8];				// must be $bluev$
	unsigned char	nOpClass[2];		// operation class, 2 bytes int
	unsigned char	nOpCode[2];			// operation code
	unsigned char	nDataLen[2];		// data length followed max 2^16 = 64k
	unsigned char	nRetVal[2];			// 返回值
	//20 bytes here
	unsigned char	bnRes[12];
} BLUEV_UDPAnsHead;

int IsReqHeadValid(BLUEV_UDPReqHead *pReq);
int InitReqHead(BLUEV_UDPReqHead *pReq);

int IsAnsHeadValid(BLUEV_UDPAnsHead *pAns);

int InitAnsHead(BLUEV_UDPAnsHead *pAns);

typedef struct tagUDPServiceParam
{
	unsigned char	bnReqBuf[1024];
	unsigned char	bnAnsBuf[1024];
	int		nReqLen;
	int		nAnsLen;
} UDPServiceParam;

typedef struct tagUserId
{
	long long	llSessionId;
	char		szGuid[40];
} UserId;

typedef struct tagUDPConnection
{
	int			nSockfd;
	struct	sockaddr_in	fromaddr;
} UDPConnection;

typedef struct tagP2PProxyInfo
{
	long long	llSessionId;	//代理请求者session id
	int					bProxy;			//是否是代理模式
} P2PProxyInfo;

typedef struct tagP2PConnection
{
	UDPConnection	stConn;		//当前连接信息，可能是服务端，也可能是设备
	volatile int			bLogIn;		//已登录到服务端
	UserId					stUserId;	//P2P登陆信息
	P2PProxyInfo		stProxyInfo;//代理模式时信息
} P2PConnection;

typedef struct tagLogInContext
{
	char	*pszSvrIP;	//123.57.72.97
	int		nSvrPort;	//7899
	char	*pszLanIp;	//LANIP
	int		nLanPort;	//LANPORT
	char	*pszMask;	//mask
	char	*pszDevId;	//dev id. can use MAC addr
} LogInContext;

typedef	struct tagEvent
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
} Event;

typedef struct tagAppInfo
{
	long long	llSessionId;
    const char	*pszIp;
	int					nPort;
	char				bConnected;
	char				bnRes[3];
} AppInfo;

typedef struct tagAppInfoNode
{
	struct tagAppInfoNode	*pPre;
	struct tagAppInfoNode	*pNext;
	AppInfo				*pData;
} AppInfoNode;

typedef struct tagAppInfoList
{
	AppInfoNode	*pFirst;
	AppInfoNode	*pLast;
    int							bInit;
} AppInfoLinkList;

typedef int (* AppInfoLinkList_Func)(AppInfoNode *pNode);
typedef int (* AppInfoLinkList_CompFunc)(AppInfoNode *pNode, AppInfoNode *pNodeComp);

int InitAppInfoList(AppInfoLinkList *pList);
int ReleaseAppInfoList(AppInfoLinkList *pList);
int AddAppInfoList(AppInfoLinkList *pList, AppInfoNode *pNode);
int IsValidAppInfoList(AppInfoLinkList *pList);
int TravelAppInfoList(AppInfoLinkList *pList, AppInfoLinkList_Func func);
int ReleaseAppInfoListNode(AppInfoNode *pNode);
int QueryAppInfoList(AppInfoLinkList *pList, AppInfoNode **ppNode, AppInfoLinkList_CompFunc func, AppInfoNode *pNodeComp);
int CompAppInfoListNode(AppInfoNode *pNode, AppInfoNode *pNodeComp);
int CompAppInfoListNode2(AppInfoNode *pNode, AppInfoNode *pNodeComp);

//**** JSON PARSE Function ****
int Json2Session(const char *pszJson, long long *pllSessionId, char **ppszGuid);
int Json2ProxyData(const char *pszJson, long long *pllSessionId, char **ppData, int *pnDataLen);
int Json2MainCtrl(const char *pszJson, long long *pllSessionId, char **ppszIP, int *pnPort);


//**** JSON BUILD Function ****
int ProxyData2Json(P2PConnection *pConn, char *pszBase64Data, char **ppszJson);
int UserData2Json(LogInContext *pContext, char **ppszJson);
int Session2Json(P2PConnection *pConn, char **ppszJson);

//**** Service Function ****
//只处理P2P服务器发来请求
int P2P_Service(UDPConnection *pConn, UDPServiceParam *pCtx);
int LogInAns(UDPConnection *pConn, UDPServiceParam *pCtx);
int LogOutAns(UDPConnection *pConn, UDPServiceParam *pCtx);
int KeepAliveAns(UDPConnection *pConn, UDPServiceParam *pCtx);

int ProxyReq(UDPConnection *pConn, UDPServiceParam *pCtx);
int DigClient(UDPConnection *pConn, UDPServiceParam *pCtx);

//只处理设备NetSocket发来请求或应答
int NetSocket_Service(UDPConnection *pConn, UDPServiceParam *pCtx);
int ProxyAns(UDPConnection *pConn, UDPServiceParam *pCtx);

//只处理主控端发来请求或应答
int DigClientAns(UDPConnection *pConn, UDPServiceParam *pCtx);
int KeepAliveByClient(UDPConnection *pConn, UDPServiceParam *pCtx);
int ConnByClient(UDPConnection *pConn, UDPServiceParam *pCtx);


//**** P2PServer Function ****
int OpenP2PServer(int nPort);
int StartP2PServer(void);
int CloseP2PServer(void);
int IsLogin();

//**** Operation Function ****
int LogIn(LogInContext	*pContext);
int LogOut();
int KeepAlive(int nSecond);

#endif // NETSOCKET_H_INCLUDED
