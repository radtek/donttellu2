#ifndef UPNPCTRLPOINT_H_INCLUDED
#define UPNPCTRLPOINT_H_INCLUDED

typedef struct PortMapping
{
	int 	nExternPort;
	int 	nInterPort;
	int		nLeaseDuration;		//持续时间
	char	*pszRemoteHost;	//Can be null
	char	*pszClient;					//IP or Computer name, suggest to use IP
	char	*pszDesc;					//Can be null
	char	bUDP;
	char	bEnable;
	char	szRes[6];						//补齐32字节
} PortMapping;	//32 bytes


//must be same order to TV_CONTROL_VARCOUNT
typedef enum EnumAction
{
	ADDPORTMAPPING = 0,
	DELPORTMAPPING,
	QUERYPORTMAPPING,
	GETEXTERNALIP,

	ENUM_MAX
} EnumAction;



extern int CTRLPOINT_Start();

extern int CTRLPOINT_Stop();

/*	action function, async	*/
//nDevNum为1
extern int CTRLPOINT_ACTION_AddPortMapping(int nDevNum, const PortMapping *pObjPM);

//PortMapping 仅需要pszRemoteHost, nExternPort, bUDP
extern int CTRLPOINT_ACTION_DelPortMapping(int nDevNum, const PortMapping *pObjPM);

//PortMapping 仅需要pszRemoteHost, nExternPort, bUDP
extern int CTRLPOINT_ACTION_QueryPortMapping(int nDevNum, const PortMapping *pObjPM);

extern int CTRLPOINT_ACTION_GetExternalIP(int nDevNum);

/*	action result function	*/
//用于判断是否发现设备
//返回设备总数
extern int CTRLPOINT_TravelDeviceList();

//返回0													说明查询到结果，ACTION已经完成
//返回-1，且*pnErrCode为0			说明未查询到结果，可以继续查询
//返回-1，且*pnErrCode不为0		说明之前的ACTION可能已经失败，需要重新进行ACTION
extern int CTRLPOINT_GetResult(EnumAction enAction, int *pnErrCode);

//szIP由调用方提供足够内存(至少16字节)
extern int CTRLPOINT_RESULT_GetExternalIP(char szIP[], int *pnErrCode);

//pObjPM只修改nInterPort, nLeaseDuration, pszClient, pszDesc, bEnable;
//其中pszClient, pszDesc由函数内部提供内存。需要释放。
extern int CTRLPOINT_RESULT_QueryPortMapping(PortMapping *pObjPM, int *pnErrCode);

#endif // UPNPCTRLPOINT_H_INCLUDED
