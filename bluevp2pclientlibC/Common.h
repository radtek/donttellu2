#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
//********
#ifndef SAFE_FREE
#define SAFE_FREE(x) if((x)){ \
                     free(x);x = NULL;}
#endif
//********
typedef enum SHCMD_TYPE_e
{
    SHCMD_CTRL_TD,  //
    SHCMD_SCAN_TD_STATE

} SHCMD_TYPE_e;
typedef enum CTRLTD_CMD_e
{
    CTRLTDCMD_SHUTOFF,  //
    CTRLTDCMD_SHUTON,
    CTRLTDCMD_NULL

} CTRLTD_CMD_e;
typedef enum TDSTATE_e
{
    TDSTATE_SHUTOFF = 0,
    TDSTATE_SHUTON,  //
    TDSTATE_NULL
} TDSTATE_e;

typedef enum SHRESULT_e
{
    SHRESULT_OK,
    SHRESULT_ERROR  //

} SHRESULT_e;

typedef struct _TDDATA_s{
    char id[9];
    CTRLTD_CMD_e ctrlID;
    int nValue;
    TDSTATE_e eState;
    SHRESULT_e eResult;
} TDDATA_s,*LPTDDATA_s;

typedef struct _TASKDATANODE_s
{
    TDDATA_s td[1];
    SHCMD_TYPE_e eCmd;
    struct sockaddr_in clientaddr;
    struct _TASKDATANODE_s* next;
} TASKDATANODE_s,*LPTASKDATANODE_s;
typedef struct _CONTROLER_NETWORKINFO_s
{
    char szifName[12];
    char szIP[36];
    char szMac[16];
    char szSubmask[36];
    //char szGateway[36];
    char szBroadCast[36];
    int nLocalPort;
    //Remote Server
    char szRSIP[36];
    int nRSPort;
    char szRSType[10]; //UDP,TCP
} CONTROLER_NETWORKINFO_s;
//
//****************
CONTROLER_NETWORKINFO_s g_NetworkInfo;
TDDATA_s g_TD[1];
#endif // COMMON_H_INCLUDED
