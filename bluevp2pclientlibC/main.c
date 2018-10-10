#include "NetSocket.h"
#include <time.h>
#include "P2PSocket.h"
#include "DeviceBridge.h"
#include "DataPool.h"
#include "Configuration.h"

static int g_nRun = 1;
static char g_CommName[12] = "";
//static int g_nCheckLoop = 0;
//**************
void PrintfData(LPTASKDATANODE_s pNode)
{
    if(!pNode)
    {
        printf("<%s:%d>Node is NULL\n", __FILE__,__LINE__);
        return;
    }
    printf("<%s:%d>Recv IP=[%s],Port=[%d]\n",
           __FILE__,__LINE__,
           inet_ntoa(pNode->clientaddr.sin_addr),
           ntohs(pNode->clientaddr.sin_port));
    printf("<%s:%d>id=[%s],SHCMD_TYPE_e=[%d],CTRLTD_CMD_e=[%d]\n",
           __FILE__,__LINE__,pNode->td[0].id,pNode->eCmd,pNode->td[0].ctrlID);
    if(strncmp(g_TD[0].id,pNode->td[0].id,strlen(g_TD[0].id)))
    {
        return;
    }
    int nRet = 0;
    if(SHCMD_SCAN_TD_STATE == pNode->eCmd)
    {
        nRet = GetLightState();
        printf("<%s:%d>GetLightState=[%d]\n", __FILE__,__LINE__,nRet);
    }
    else if(SHCMD_CTRL_TD == pNode->eCmd)
    {
        if(CTRLTDCMD_SHUTOFF == pNode->td[0].ctrlID)
        {
            nRet = ShutOffLight();
            printf("<%s:%d>GetLightState=[%d]\n", __FILE__,__LINE__,nRet);
        }
        else if(CTRLTDCMD_SHUTON == pNode->td[0].ctrlID)
        {
            nRet = ShutOnLight();
            printf("<%s:%d>GetLightState=[%d]\n", __FILE__,__LINE__,nRet);
        }
    }
    //int nRet =AddTaskData(pNode);
    //printf("<%s:%d>AddTaskData=[%d]\n", __FILE__,__LINE__,nRet);
}

static int Initialze(void)
{
    if(InitDataPool())
    {
        return -9;
    }
    bzero(&g_NetworkInfo,sizeof(g_NetworkInfo));
    memset(g_TD,0x00,sizeof(g_TD));
    g_TD[0].eState = TDSTATE_NULL;
    strncpy(g_TD[0].id,"00001001",sizeof(char) * 8);
    //----/dev/ttyS
    int nRet = GetPrivateProfileString("commid","/dev/ttyS0",g_CommName,sizeof(g_CommName) - 1,"./configuration.conf");
    printf("<%s[%d]:%s>ret=[%d],commid=[%s]\n",__FILE__,__LINE__,__FUNCTION__,nRet,g_CommName);
    if(nRet)
    {
        return nRet;
    }
    //----Local Port
    g_NetworkInfo.nLocalPort = GetPrivateProfileInt("localport",10701,"./configuration.conf");
    printf("<%s[%d]:%s>localport=[%d]\n",__FILE__,__LINE__,__FUNCTION__,g_NetworkInfo.nLocalPort);

    //----Local IP
    nRet = GetPrivateProfileString("localip","",g_NetworkInfo.szIP,sizeof(g_NetworkInfo.szIP) - 1,"./configuration.conf");
    printf("<%s[%d]:%s>ret=[%d],localip=[%s]\n",__FILE__,__LINE__,__FUNCTION__,nRet,g_NetworkInfo.szIP);

    //****Remote Server
    //----Port
    g_NetworkInfo.nRSPort = GetPrivateProfileInt("rsport",7899,"./configuration.conf");
    printf("<%s[%d]:%s>rsport=[%d]\n",__FILE__,__LINE__,__FUNCTION__,g_NetworkInfo.nRSPort);
    //----IP
	//nRet = GetPrivateProfileString("rsip","10.171.89.196",g_NetworkInfo.szRSIP,sizeof(g_NetworkInfo.szRSIP) - 1,"./configuration.conf");
    nRet = GetPrivateProfileString("rsip","123.57.72.97",g_NetworkInfo.szRSIP,sizeof(g_NetworkInfo.szRSIP) - 1,"./configuration.conf");
    printf("<%s[%d]:%s>ret=[%d],rsip=[%s]\n",__FILE__,__LINE__,__FUNCTION__,nRet,g_NetworkInfo.szRSIP);
    if(nRet || 0 >= strlen(g_NetworkInfo.szRSIP))
    {
        return -1;
    }
    //----Type
    nRet = GetPrivateProfileString("rstype","UDP",g_NetworkInfo.szRSType,sizeof(g_NetworkInfo.szRSType) - 1,"./configuration.conf");
    printf("<%s[%d]:%s>ret=[%d],rstype=[%s]\n",__FILE__,__LINE__,__FUNCTION__,nRet,g_NetworkInfo.szRSType);
    if(nRet || 0 >= strlen(g_NetworkInfo.szRSType))
    {
        return -2;
    }
    //Open /dev/ttyS
    while(g_nRun && (nRet = OpenDevice(g_CommName)))
    {
        printf("<%s:%d>OpenDevice[%s]=[%d]\n",__FILE__,__LINE__,g_CommName,nRet);
        sleep(2);
    }
    if(!g_nRun)
    {
        return -3;
    }
    //Get State
    nRet = GetLightState();
    printf("<%s:%d>GetLightState=[%d]\n",__FILE__,__LINE__,nRet);
    while((nRet = AutoRecall(NULL)))
    {
        printf("<%s:%d>AutoRecall=[%d],Light State=[%d]\n",__FILE__,__LINE__,nRet,g_TD[0].eState);
    }
    //Init UdpServer
    nRet = InitUdpServer();
    printf("<%s:%d>InitUdpServer=[%d]\n",__FILE__,__LINE__,nRet);
    if(nRet)
    {
        return -4;
    }
    // Start Udp Server
    if(0 != StartUdpServer(&PrintfData))
    {
        printf("<%s:%d>StartUdpServer Error\n",__FILE__,__LINE__);
        return -5;
    }
    return 0;
}
static void Cleanup(void)
{
    g_nRun = 0;
    CloseP2PServer();
    StopUdpServer();
    CloseDevice();
    ReleaseDataPool();
    printf("<%s:%d>Cleanup\n",__FILE__,__LINE__);
}
//********************
int main()
{
    printf("****Begin****\n");
    atexit(Cleanup);
    int nRet = Initialze();
    printf("<%s[%d]:%s>Initialze=[%d]\n",__FILE__,__LINE__,__FUNCTION__,nRet);
    if(nRet)
    {
        return -1;
    }


    OpenP2PServer(8888);
    StartP2PServer();
    LogInContext    context;
    context.pszSvrIP = g_NetworkInfo.szRSIP;//"123.57.72.97";
    context.nSvrPort = g_NetworkInfo.nRSPort;//7899;
    context.pszLanIp = g_NetworkInfo.szIP;//"192.168.0.5";
    context.nLanPort = 8888;
    context.pszMask = g_NetworkInfo.szSubmask;//"255.255.255.0";
    context.pszDevId = g_NetworkInfo.szMac;//"111111";
    LogIn(&context);

    sleep(2);
    KeepAlive(20);

    do
    {
        WatchComm(&g_nRun);
    }
    while(0);
    StopUdpServer();
    printf("****End****\n");
    return 0;
}
