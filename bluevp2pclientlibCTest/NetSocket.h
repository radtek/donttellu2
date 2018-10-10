#ifndef NETSOCKET_H_INCLUDED
#define NETSOCKET_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define _NETSOCKET_DEBUG_
//**** Enum ****
typedef enum SHCMD_TYPE_e
{
    SHCMD_CTRL_TD,  //
    SHCMD_SCAN_TD_STATE

} SHCMD_TYPE_e;
typedef enum CTRLTD_CMD_e
{
    CTRLTDCMD_SHUTOFF,  //
    CTRLTDCMD_SHUTON

} CTRLTD_CMD_e;
typedef enum TDSTATE_e
{
    TDSTATE_SHUTOFF = 0,
    TDSTATE_SHUTON  //

} TDSTATE_e;
typedef enum SHRESULT_e
{
    SHRESULT_OK,
    SHRESULT_ERROR  //

} SHRESULT_e;
typedef struct _CONTROLER_NETWORKINFO_s
{
    char szifName[12];
    char szIP[36];
    char szMac[16];
    char szSubmask[36];
    char szGateway[36];
    int nLocalPort;
    //Remote Server
    char szRSIP[36];
    int nRSPort;
    char szRSType[10]; //UDP,TCP
} CONTROLER_NETWORKINFO_s;
//---- Info
CONTROLER_NETWORKINFO_s g_NetworkInfo;
//**** Public Function ****
typedef void (*pfuncDoNetSocket)(enum SHCMD_TYPE_e eCmd,const struct sockaddr_in* pAddr,CTRLTD_CMD_e eCtrlCmd);
int InitUdpServer(void);
int StartUdpServer(pfuncDoNetSocket pfunc);
void StopUdpServer(void);
//**** Send Cmd ****
int ReplyCtrlTD(const struct sockaddr_in* pAddr,SHRESULT_e eRet);
int ReplyTDState(const struct sockaddr_in* pAddr,TDSTATE_e eState);
#endif // NETSOCKET_H_INCLUDED
