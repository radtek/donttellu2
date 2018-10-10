#include "NetSocket.h"
#include <time.h>
#include "P2PSocket.h"

static int g_nState = 0;
//
static void Cleanup(void)
{
    StopUdpServer();
    printf("<%s:%d>Cleanup\n",__FILE__,__LINE__);
}
static int GetRandState(void)
{
    srand((unsigned)time(0));
    return rand() % 2;
}
static int g_nCheckLoop = 0;
void PrintfData(SHCMD_TYPE_e eCmd,const struct sockaddr_in* pAddr,CTRLTD_CMD_e eCtrlCmd)
{
    printf("<%s:%d>Recv IP=[%s],Port=[%d]\n",__FILE__,__LINE__,inet_ntoa(pAddr->sin_addr),ntohs(pAddr->sin_port));
    printf("<%s:%d>SHCMD_TYPE_e=[%d],CTRLTD_CMD_e=[%d]\n",__FILE__,__LINE__,eCmd,eCtrlCmd);
    if(SHCMD_CTRL_TD == eCmd){
        g_nCheckLoop = (g_nCheckLoop + 1) % 7;
        //if(GetRandState() == g_nState){
        if(g_nCheckLoop){
            if(CTRLTDCMD_SHUTON == eCtrlCmd){
                g_nState = 1;
                printf("<%s:%d>CTRLTDCMD_SHUTON\n",__FILE__,__LINE__);
            }else if(CTRLTDCMD_SHUTOFF == eCtrlCmd){
                g_nState = 0;
                printf("<%s:%d>CTRLTDCMD_SHUTOFF\n",__FILE__,__LINE__);
            }
            printf("<%s:%d>ReplyCtrlTD SHRESULT_OK\n",__FILE__,__LINE__);
            ReplyCtrlTD(pAddr,SHRESULT_OK);
        }else{
            printf("<%s:%d>ReplyCtrlTD SHRESULT_ERROR\n",__FILE__,__LINE__);
            ReplyCtrlTD(pAddr,SHRESULT_ERROR);
        }
    }else if(SHCMD_SCAN_TD_STATE == eCmd){
        printf("<%s:%d>State=[%d]\n",__FILE__,__LINE__,g_nState);
        ReplyTDState(pAddr,(TDSTATE_e)g_nState);
    }
}

int main()
{
    printf("****Begin****\n");
    atexit(Cleanup);
    g_nState = GetRandState();
    printf("<%s:%d>g_nState=[%d]\n",__FILE__,__LINE__,g_nState);
    InitUdpServer();

    OpenP2PServer(8888);
    StartP2PServer();

    {
    	LogInContext    context;
		context.pszSvrIP = g_NetworkInfo.szRSIP;//"123.57.72.97";
		context.nSvrPort = g_NetworkInfo.nRSPort;//7899;
		context.pszLanIp = g_NetworkInfo.szIP;//"192.168.0.5";
		context.nLanPort = 8888;
		context.pszMask = g_NetworkInfo.szSubmask;//"255.255.255.0";
		context.pszDevId = g_NetworkInfo.szMac;//"111111";
		context.pszGateWay = g_NetworkInfo.szGateway;

		LogIn(&context);
    }

    sleep(2);
    KeepAlive(20);
    //LogOut();
    //CloseP2PServer();

    do
    {
        if(0 != StartUdpServer(&PrintfData)){
            printf("<%s:%d>StartUdpServer Error\n",__FILE__,__LINE__);
            break;
        }

        while(1){
            sleep(5);
        }

    }
    while(0);
    StopUdpServer();
    printf("****End****\n");
    return 0;
}
