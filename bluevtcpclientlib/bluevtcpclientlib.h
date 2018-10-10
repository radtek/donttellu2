#ifndef BLUEVTCPCLIENTLIB_H_INCLUDED
#define BLUEVTCPCLIENTLIB_H_INCLUDED

/*
JSON FOR APP
	"{"
	"	\"svrip\":\"123.57.72.97\",	"
	"	\"svrport\":8878,	"
	"	\"lanport\":8856,	"
	"	\"ismainctrl\":1,	"
	"	\"username\":\"admin1\",	"
	"	\"password\":\"admin1\"	"
	"}";

JSON FOR DEVICE
	"{"
	"	\"svrip\":\"123.57.72.97\",	"
	"	\"svrport\":8878,	"
	"	\"lanport\":8856,	"
	"	\"ismainctrl\":0,	"
	"	\"devid\":\"deviceid1\",	"
	"	\"isupnp\":1,	"
	"	\"upnpip\":\"192.168.1.109\",	"
	"	\"upnpport\":8856	"
	"}";
*/
extern int TCPCLIENT_Init(const char *pszJson);
extern int TCPCLIENT_Start(void);
extern int TCPCLIENT_End(void);


//**** Operation Function ****
extern int TCPCLIENT_Login(void);
extern int TCPCLIENT_IsLogin(void);
extern int TCPCLIENT_IsConnect(void);
extern int TCPCLIENT_Logout(void);
extern int TCPCLIENT_Keepalive(int nSecond);
/*
out param: ppszInfo  内部分配内存，需外部释放
example
{
	"devs":
	[
	{
		"ip":"125.35.66.234",
		"issamelan":1,
		"isupnp":0,
		"lanip":"192.168.179.154",
		"lanport":8856,
		"port":57651,
		"sessionid":"38C99E01-BA70-463B-B75C-6041F9DB7E89"
	}
	]
}
*/
extern int TCPCLIENT_GetDevice(char **ppszInfo);

/**********************供C使用**********************************/
typedef struct tagLogInContext
{
	char	szLanIp[16];	//LANIP
	int		nLanPort;		//LANPORT
	char	szMask[16];	//mask
	char	szGateWay[16];	//gateway

	char	szMac[20];
	char	szUserName[20];
	char	szPassWord[32];

	char	szDevId[20];	//dev id. can use MAC addr
	char	szUPNPIP[16];
	int		nUPNPPort;
	char	bMainCtrl;
	char	bUPNP;
	char	szRes2[2];
} LogInContext;

/* in param: pCtx 外部分配内存 */
extern int TCPCLIENT_InitEx(LogInContext *pCtx, char szServerIP[16], int nServerPort);

/* in param: pCtx 外部分配内存 */
extern int TCPCLIENT_GetLocalInfo(LogInContext *pCtx);
extern LogInContext TCPCLIENT_QueryLogInCtx();
/* in param: pCtx 外部分配内存 */
extern int TCPCLIENT_SetLogInCtx(LogInContext *pCtx);

typedef struct tagDeviceInfo
{
	char	szIp[16];
	int		nPort;
	char	szLanIp[16];
	int		nLanPort;
	char	szUPNPIP[16];
	int		nUPNPPort;
	char	bUPNP;
	char	bSameLan;
	char	szRes2[2];
} DeviceInfo;

/* in param: pDevInfo 外部分配内存 */
extern int TCPCLIENT_GetDeviceEx(DeviceInfo *pDevInfo);



#endif // BLUEVTCPCLIENTLIB_H_INCLUDED
