#include "NetSocket.h"
#include "cJSON.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <pthread.h>

#define BUFSIZE 8192
#define MAXINTERFACE 32
//**** type ****
//**** Common ****
static const char DATAPACK_HEAD[10] = "####Json=";
static const char DATAPACK_TAIL[5] = "****";
//**** Cmd ****
//----
static const char CMDASK_SCANTD[5] = "100A";
static const char CMDRET_SCANTD[5] = "100R";
//----
static const char CMDASK_SCANTDSTATE[5] = "101A";
static const char CMDRET_SCANTDSTATE[5] = "101R";
//----
static const char CMDASK_CTRLTD[5] = "102A";
static const char CMDRET_CTRLTD[5] = "102R";
//----
static const char CMDASK_SENDSCENE[5] = "103A";
static const char CMDRET_SENDSCENE[5] = "103R";
//----
static const char CMDASK_UPLOADSCENE[5] = "104A";
static const char CMDRET_UPLOADSCENE[5] = "104R";
//----
static const char CMDASK_RUNSCENE[5] = "105A";
static const char CMDRET_RUNSCENE[5] = "105R";
//----
static const char CMDASK_STOPSCENE[5] = "106A";
static const char CMDRET_STOPSCENE[5] = "106R";
//----
static const char CMDASK_ADDTD[5] = "107A";
static const char CMDRET_ADDTD[5] = "107R";
//----
static const char CMDASK_DELTD[5] = "108A";
static const char CMDRET_DELTD[5] = "108R";
//----
static const char CMDASK_SCANCONTROLER[5] = "109A";
static const char CMDRET_SCANCONTROLER[5] = "109R";
//----
static const char CMDASK_SETCONTROLER[5] = "10aA";
static const char CMDRET_SETCONTROLER[5] = "10aR";
//**** Define Var
//----Template TDID Const
//static const char TDID[9] = "00001001";
//---- Info
static struct sockaddr_in g_addrRemoteServer;
static struct sockaddr_in g_addrPC;
//---- Socket
static int g_udpServer = -1;
static int g_nRun = 0;
//---- Thread
static pthread_t g_ptServer = 0;
//---- callback function
static pfuncDoNetSocket funcDoNetSocket;
//#################################################
//**** Public Reply Function ****
static int ReplyCtrlTD(LPTASKDATANODE_s pNode)
{
    if(!pNode)
    {
        return -1;
    }
    int nRet = -2;
    cJSON* root = cJSON_CreateObject();
    do
    {
        if(!root)
        {
            break;
        }
        cJSON_AddStringToObject(root,"Cmd",CMDRET_CTRLTD);
        cJSON_AddStringToObject(root,"GW",g_NetworkInfo.szMac);
        cJSON* sdArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root,"SD",sdArray);
        cJSON* td = cJSON_CreateObject();
        cJSON_AddItemToArray(sdArray,td);
        cJSON_AddStringToObject(td,"ID",pNode->td[0].id);
        if(SHRESULT_OK == pNode->td[0].eResult)
        {
            cJSON_AddStringToObject(td,"Ret","0");
        }
        else
        {
            cJSON_AddStringToObject(td,"Ret","1");
        }
        char* out = cJSON_Print(root);
        if(!out)
        {
            nRet = -3;
            break;
        }
        char cmd[4096];
        memset(cmd,0x00,sizeof(cmd));
        sprintf(cmd,"%s%s%s",DATAPACK_HEAD,out,DATAPACK_TAIL);
        free(out);
        nRet = sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)&(pNode->clientaddr),sizeof(struct sockaddr));
    }
    while(0);
    cJSON_Delete(root);
    return nRet;
}
static int ReplyTDState(LPTASKDATANODE_s pNode)
{
    if(!pNode)
    {
        return -1;
    }
    int nRet = -2;
    cJSON* root = cJSON_CreateObject();
    do
    {
        if(!root)
        {
            break;
        }
        cJSON_AddStringToObject(root,"Cmd",CMDRET_SCANTDSTATE);
        cJSON_AddStringToObject(root,"GW",g_NetworkInfo.szMac);
        cJSON* sdArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root,"SD",sdArray);
        cJSON* td = cJSON_CreateObject();
        cJSON_AddItemToArray(sdArray,td);
        cJSON_AddStringToObject(td,"ID",pNode->td[0].id);
        if(TDSTATE_SHUTON == pNode->td[0].eState)
        {
            cJSON_AddStringToObject(td,"State","1");
            cJSON_AddStringToObject(td,"Value","1");
        }
        else
        {
            cJSON_AddStringToObject(td,"State","0");
            cJSON_AddStringToObject(td,"Value","0");
        }
        char* out = cJSON_Print(root);
        if(!out)
        {
            nRet = -3;
            break;
        }
        char cmd[4096];
        memset(cmd,0x00,sizeof(cmd));
        sprintf(cmd,"%s%s%s",DATAPACK_HEAD,out,DATAPACK_TAIL);
        free(out);
        nRet = sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)&(pNode->clientaddr),sizeof(struct sockaddr));
    }
    while(0);
    cJSON_Delete(root);

    return nRet;
}
static int ReplyScanControler(const struct sockaddr_in* pAddr)
{
    if(!pAddr)
    {
        return -1;
    }
    int nRet = -2;
    cJSON* root = cJSON_CreateObject();
    do
    {
        if(!root)
        {
            break;
        }
        cJSON_AddStringToObject(root,"Cmd",CMDRET_SCANCONTROLER);
        cJSON_AddStringToObject(root,"GW",g_NetworkInfo.szMac);
        cJSON* param = cJSON_CreateObject();
        cJSON_AddItemToObject(root,"Param",param);
        cJSON_AddStringToObject(param,"IP",g_NetworkInfo.szIP);
        cJSON_AddStringToObject(param,"Mac",g_NetworkInfo.szMac);
        cJSON_AddStringToObject(param,"Submask",g_NetworkInfo.szSubmask);
        cJSON_AddStringToObject(param,"Gateway",g_NetworkInfo.szBroadCast);
        cJSON_AddStringToObject(param,"RSIP",g_NetworkInfo.szRSIP);
        //char szTemp[8];
        //bzero(szTemp,sizeof(szTemp));
        //sprintf(szTemp,"%d",g_NetworkInfo.nRSPort);
        cJSON_AddNumberToObject(param,"RSPort",g_NetworkInfo.nRSPort);
        cJSON_AddStringToObject(param,"RSType",g_NetworkInfo.szRSType);


        char* out = cJSON_Print(root);
        if(!out)
        {
            nRet = -3;
            break;
        }
        char cmd[4096];
        memset(cmd,0x00,sizeof(cmd));
        sprintf(cmd,"%s%s%s",DATAPACK_HEAD,out,DATAPACK_TAIL);
        free(out);
        //printf("%s\n",cmd);
        nRet = sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)pAddr,sizeof(struct sockaddr));
    }
    while(0);
    cJSON_Delete(root);

    return nRet;
}
static int ReplySetControler(const struct sockaddr_in* pAddr,SHRESULT_e eRet)
{
    char cmd[4096];
    memset(cmd,0x00,sizeof(cmd));
    if(SHRESULT_OK == eRet)
    {
        sprintf(cmd,"%s{\"Cmd\":\"%s\",\"GW\":\"%s\",\"Ret\":\"0\"}%s",
                DATAPACK_HEAD,
                CMDRET_SETCONTROLER,
                g_NetworkInfo.szMac,
                DATAPACK_TAIL);
    }
    else
    {
        sprintf(cmd,"%s{\"Cmd\":\"%s\",\"GW\":\"%s\",\"Ret\":\"1\"}%s",
                DATAPACK_HEAD,
                CMDRET_SETCONTROLER,
                g_NetworkInfo.szMac,
                DATAPACK_TAIL);
    }
    return sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)pAddr,sizeof(struct sockaddr));
}
//**** Private Function ****
static int SetIPAddr(const char* szIP,const char* szSubmask,const char* szBroadcast)
{
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(0 > fd)
    {
        return -1;
    }
    int nRet = -2;
    do
    {
        //----Set IP
        struct ifreq req;
        bzero(&req,sizeof(req));
        struct sockaddr_in* addr = (struct sockaddr_in*)&(req.ifr_addr);
        addr->sin_family = AF_INET;
        if(szIP && strcmp(g_NetworkInfo.szIP,szIP))
        {
            if(0 > inet_aton(szIP,&(addr->sin_addr)))
            {
                break;
            }
            if(0 > ioctl(fd,SIOCSIFADDR,&req))
            {
                break;
            }
        }
        //----Set Net mask
        if(szSubmask && strcmp(g_NetworkInfo.szSubmask,szSubmask))
        {
            if(0 > inet_aton(szSubmask,&(addr->sin_addr)))
            {
                break;
            }
            if(0 > ioctl(fd,SIOCSIFNETMASK,&req))
            {
                break;
            }
        }
        //---- Set Broad Cast
        if(szBroadcast && strcmp(g_NetworkInfo.szBroadCast,szBroadcast))
        {
            if(0 > inet_aton(szBroadcast,&(addr->sin_addr)))
            {
                break;
            }
            if(0 > ioctl(fd,SIOCSIFBRDADDR,&req))
            {
                break;
            }
        }
        nRet = 0;
    }
    while(0);
    close(fd);
    return nRet;
}
static int GetNetworkInfo(void)
{
    int nRet = 1;
    bzero(&g_addrRemoteServer,sizeof(g_addrRemoteServer));
    bzero(&g_addrPC,sizeof(g_addrPC));
    int sockfd = socket(PF_INET,SOCK_DGRAM,0);
    if(0 > sockfd)
    {
        return -1;
    }
    do
    {
        struct ifreq buf[MAXINTERFACE];
        struct ifconf ifc;
        memset(&ifc,0x00,sizeof(ifc));
        memset(buf,0x00,sizeof(buf));
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if(ioctl(sockfd,SIOCGIFCONF,&ifc))
        {
            nRet = -9;
            break;
        }
        int nInterfaceNum = ifc.ifc_len / sizeof(struct ifreq);
        printf("<%s:%d>Interface Num=[%d]\n",__FILE__,__LINE__,nInterfaceNum);
        if(0 >= nInterfaceNum)
        {
            nRet = -8;
            break;
        }
        int i = -1;
        while(++i < nInterfaceNum)
        {
            printf("<%s[%d]:%s>ifr_name=[%s]\n",__FILE__,__LINE__,__FUNCTION__,buf[i].ifr_name);
            if(ioctl(sockfd,SIOCGIFFLAGS,&(buf[i])))
            {
                continue;
            }
//            if(buf[i].ifr_flags & IFF_PROMISC)
//            {
//                printf("<%s:%d>[%d] IFF_PROMISC\n",__FILE__,__LINE__,i);
//            }
//            else
//            {
//                printf("<%s:%d>[%d]ifr_name=[%s]\n",__FILE__,__LINE__,i,buf[i].ifr_name);
//            }
//            if(buf[i].ifr_flags & IFF_UP)
//            {
//                printf("<%s:%d>[%d] is up\n",__FILE__,__LINE__,i);
//            }
//            else
//            {
//                printf("<%s:%d>[%d] is down\n",__FILE__,__LINE__,i);
//            }
            //break;
            if(buf[i].ifr_flags & IFF_LOOPBACK)
            {
                printf("<%s:%d>[%d] is IFF_LOOPBACK\n",__FILE__,__LINE__,i);
                continue;
            }
//            if(buf[i].ifr_flags & IFF_POINTOPOINT)
//            {
//                printf("<%s:%d>[%d] is IFF_POINTOPOINT\n",__FILE__,__LINE__,i);
//            }
//            if(buf[i].ifr_flags & IFF_RUNNING)
//            {
//                printf("<%s:%d>[%d] is IFF_RUNNING\n",__FILE__,__LINE__,i);
//            }
//            if(buf[i].ifr_flags & IFF_MULTICAST)
//            {
//                printf("<%s:%d>[%d] is IFF_MULTICAST\n",__FILE__,__LINE__,i);
//            }
//            if(buf[i].ifr_flags & IFF_MULTICAST)
//            {
//                printf("<%s:%d>[%d] is IFF_MULTICAST\n",__FILE__,__LINE__,i);
//            }
            //----Local IP
            if(0 > ioctl(sockfd,SIOCGIFADDR,&(buf[i])))
            {
                nRet = -2;
                break;
            }
            if(0 < strlen(g_NetworkInfo.szIP) &&
                    strncmp(g_NetworkInfo.szIP,
                            inet_ntoa(((struct sockaddr_in*)&(buf[i].ifr_addr))->sin_addr),
                            strlen(g_NetworkInfo.szIP))
              )
            {
                continue;
            }
            strncpy(g_NetworkInfo.szIP,
                    inet_ntoa(((struct sockaddr_in*)&(buf[i].ifr_addr))->sin_addr),
                    sizeof(g_NetworkInfo.szIP) - 1);
            strncpy(g_NetworkInfo.szifName,buf[i].ifr_name,sizeof(g_NetworkInfo.szifName) - 1);
            //----Mac
            if(0 > ioctl(sockfd,SIOCGIFHWADDR,&(buf[i])))
            {
                nRet = -4;
                break;
            }
            sprintf(g_NetworkInfo.szMac,"%02x%02x%02x%02x%02x%02x",
                    (unsigned char)buf[i].ifr_hwaddr.sa_data[0],
                    (unsigned char)buf[i].ifr_hwaddr.sa_data[1],
                    (unsigned char)buf[i].ifr_hwaddr.sa_data[2],
                    (unsigned char)buf[i].ifr_hwaddr.sa_data[3],
                    (unsigned char)buf[i].ifr_hwaddr.sa_data[4],
                    (unsigned char)buf[i].ifr_hwaddr.sa_data[5]);
            //----Broadcast IP
            if(0 > ioctl(sockfd,SIOCGIFBRDADDR,&(buf[i])))
            {
                nRet = -5;
                break;
            }
            strcpy(g_NetworkInfo.szBroadCast,inet_ntoa(((struct sockaddr_in*)&(buf[i].ifr_broadaddr))->sin_addr));
            //----Submask
            if(0 > ioctl(sockfd,SIOCGIFNETMASK,&(buf[i])))
            {
                nRet = -5;
                break;
            }
            strcpy(g_NetworkInfo.szSubmask,inet_ntoa(((struct sockaddr_in*)&(buf[i].ifr_netmask))->sin_addr));
            //----
            //g_NetworkInfo.nLocalPort = 10701;
            //Remote Server
            //strncpy(g_NetworkInfo.szRSIP,"123.57.72.97",sizeof(g_NetworkInfo.szRSIP));
            //g_NetworkInfo.nRSPort = 7899;
            //strncpy(g_NetworkInfo.szRSType,"UDP",sizeof(g_NetworkInfo.szRSType));
            g_addrRemoteServer.sin_addr.s_addr = inet_addr(g_NetworkInfo.szRSIP);
            g_addrRemoteServer.sin_port = htons(g_NetworkInfo.nRSPort);
            g_addrRemoteServer.sin_family = AF_INET;
            //----Print
            printf("<%s:%d>ifr_name=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szifName);
            printf("<%s:%d>IP=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szIP);
            printf("<%s:%d>Mac=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szMac);
            printf("<%s:%d>Broadcast=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szBroadCast);
            printf("<%s:%d>LocalPort=[%d]\n",__FILE__,__LINE__,g_NetworkInfo.nLocalPort);
            printf("<%s:%d>NetMask=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szSubmask);
            printf("<%s:%d>Remote Server IP=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szRSIP);
            printf("<%s:%d>Remote Server Port=[%d]\n",__FILE__,__LINE__,g_NetworkInfo.nRSPort);
            printf("<%s:%d>Remote Server Type=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szRSType);

            nRet = 0;
            break;
        }

    }
    while(0);

    close(sockfd);
    return nRet;
    return nRet;
}
//---- Recv Do Function
static void DoScanControler(const struct sockaddr_in* pAddr)
{
    ReplyScanControler(pAddr);
}
static void DoCtrlTD(const struct sockaddr_in* pAddr,cJSON* root)
{
    cJSON* gw = cJSON_GetObjectItem(root,"GW");
    cJSON* sdArray = cJSON_GetObjectItem(root,"SD");
    if(!gw || !(gw->valuestring) || !sdArray ||
            strncmp(g_NetworkInfo.szMac,gw->valuestring,strlen(g_NetworkInfo.szMac)))
    {
        return;
    }
    cJSON* sd = sdArray->child;
    while(sd)
    {
        cJSON* tdid = cJSON_GetObjectItem(sd,"ID");
        cJSON* tdctrl = cJSON_GetObjectItem(sd,"Ctrl");
        if(tdid && tdid->valuestring && tdctrl && tdctrl->valuestring)
        {
            TASKDATANODE_s node;
            memset(&node,0x00,sizeof(node));
            node.next = NULL;
            memcpy(&(node.clientaddr),pAddr,sizeof(struct sockaddr_in));
            node.eCmd = SHCMD_CTRL_TD;
            strncpy(node.td[0].id,tdid->valuestring,sizeof(node.td[0].id));
            node.td[0].eResult = SHRESULT_ERROR;
            if(0 == strncmp(tdctrl->valuestring,"1",sizeof(char)))
            {
                node.td[0].ctrlID = CTRLTDCMD_SHUTON;
                funcDoNetSocket(&node);
            }
            else if(0 == strncmp(tdctrl->valuestring,"0",sizeof(char)))
            {
                node.td[0].ctrlID = CTRLTDCMD_SHUTOFF;
                funcDoNetSocket(&node);
            }
            else
            {
                ReplyCtrlTD(&node);
            }
            break;
        }
        sd = sd->next;
    }
}
static void DoScanTDState(const struct sockaddr_in* pAddr,cJSON* root)
{
    cJSON* gw = cJSON_GetObjectItem(root,"GW");
    cJSON* sdArray = cJSON_GetObjectItem(root,"SD");
    if(!gw || !(gw->valuestring) || !sdArray ||
            strncmp(g_NetworkInfo.szMac,gw->valuestring,strlen(g_NetworkInfo.szMac)))
    {
        return;
    }
    cJSON* sd = sdArray->child;
    while(sd)
    {
        if(sd->valuestring && 0 == strncmp(g_TD[0].id,sd->valuestring,strlen(g_TD[0].id)))
        {
            TASKDATANODE_s node;
            memset(&node,0x00,sizeof(node));
            node.next = NULL;
            memcpy(&(node.clientaddr),pAddr,sizeof(struct sockaddr_in));
            node.eCmd = SHCMD_SCAN_TD_STATE;
            strncpy(node.td[0].id,g_TD[0].id,sizeof(node.td[0].id));
            node.td[0].ctrlID = CTRLTDCMD_NULL;
            node.td[0].eState = g_TD[0].eState;
            ReplyTDState(&node);
            break;
        }
        sd = sd->next;
    }
}
static void DoSetControler(const struct sockaddr_in* pAddr,cJSON* root)
{
    cJSON* gw = cJSON_GetObjectItem(root,"GW");
    cJSON* param = cJSON_GetObjectItem(root,"Param");
    if(!gw || !(gw->valuestring) || !param ||
            strncmp(g_NetworkInfo.szMac,gw->valuestring,strlen(g_NetworkInfo.szMac)))
    {
        return;
    }
    cJSON* ip = cJSON_GetObjectItem(param,"IP");
    cJSON* submask = cJSON_GetObjectItem(param,"Submask");
    cJSON* broadcast = cJSON_GetObjectItem(param,"Gateway");
    cJSON* rsip = cJSON_GetObjectItem(param,"RSIP");
    cJSON* rsport = cJSON_GetObjectItem(param,"RSPort");
    cJSON* rstype = cJSON_GetObjectItem(param,"RSType");
    printf("<%s:%d>ip=[%s],Submask=[%s],BroadCast=[%s],RSIP=[%s],RSPort=[%s],RSType=[%s]\n",
           __FILE__,__LINE__,
           ip->valuestring,
           submask->valuestring,
           broadcast->valuestring,
           rsip->valuestring,
           rsport->valuestring,
           rstype->valuestring);

    if(rsip && rsip->valuestring && 0 < strlen(rsip->valuestring))
    {
        strncpy(g_NetworkInfo.szRSIP,rsip->valuestring,sizeof(g_NetworkInfo.szRSIP));
        g_addrRemoteServer.sin_addr.s_addr = inet_addr(g_NetworkInfo.szRSIP);
    }
    if(rstype && rstype->valuestring && 0 < strlen(rstype->valuestring))
    {
        strncpy(g_NetworkInfo.szRSType,rstype->valuestring,sizeof(g_NetworkInfo.szRSType));
    }
    if(rsport && rsport->valuestring && 0 < strlen(rsport->valuestring))
    {
        g_NetworkInfo.nRSPort = atoi(rsip->valuestring);
        g_addrRemoteServer.sin_port = htons(g_NetworkInfo.nRSPort);
    }
    if(SetIPAddr(ip->valuestring,submask->valuestring,broadcast->valuestring))
    {
        ReplySetControler(pAddr,SHRESULT_ERROR);
    }
    else
    {
        ReplySetControler(pAddr,SHRESULT_OK);
    }
}
//---- Analyse
static int Analyse(const struct sockaddr_in* pAddr,const char* pData,int nLen)
{
    int ret = -1;
    cJSON* root = NULL;
    do
    {
        root = cJSON_Parse(pData);
        if(!root)
        {
            break;
        }
        cJSON* cmd = cJSON_GetObjectItem(root,"Cmd");
        if(!cmd)
        {
            break;
        }
        if(0 == strncmp(cmd->valuestring,CMDASK_SCANCONTROLER,strlen(CMDASK_SCANCONTROLER)))
        {
            DoScanControler(pAddr);
        }
        else if(0 == strncmp(cmd->valuestring,CMDASK_CTRLTD,strlen(CMDASK_CTRLTD)))
        {
            DoCtrlTD(pAddr,root);
        }
        else if(0 == strncmp(cmd->valuestring,CMDASK_SCANTDSTATE,strlen(CMDASK_SCANTDSTATE)))
        {
            DoScanTDState(pAddr,root);
        }
        else if(0 == strncmp(cmd->valuestring,CMDASK_SETCONTROLER,strlen(CMDASK_SETCONTROLER)))
        {
            DoSetControler(pAddr,root);
        }
        ret = 0;
    }
    while(0);
    cJSON_Delete(root);
    return ret;
}
static void UdpServerProc(void* lPvoid)
{
    int retVal = 0;
    printf("****pthread_t=[%lx] Begin****\n",pthread_self());
    char szBuf[4096];
    int ret = 0;
    int nLen = sizeof(szBuf);
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    while(g_nRun)
    {
        addrLen = sizeof(clientAddr);
        bzero(&clientAddr,addrLen);
        bzero(szBuf,nLen);
        ret = recvfrom(g_udpServer,szBuf,nLen,0,(struct sockaddr*)&clientAddr,&addrLen);
        printf("<%s:%d>recvfrom=[%d],len=[%d]\n",__FILE__,__LINE__,ret,strlen(szBuf));
        if(ret <= 0 || 0 >= strlen(szBuf))
        {
            continue;
        }
        if(strlen(szBuf) != ret ||
                0 != strncmp(DATAPACK_HEAD,szBuf,strlen(DATAPACK_HEAD)) ||
                0 != strncmp(DATAPACK_TAIL,(szBuf + ret - 4),strlen(DATAPACK_TAIL)))
        {
            continue;
        }
        szBuf[ret - strlen(DATAPACK_TAIL)] = '\0';
#ifdef _NETSOCKET_DEBUG_
        printf("data=[%s]\n",szBuf + strlen(DATAPACK_HEAD));
#endif
        if(g_addrRemoteServer.sin_addr.s_addr != clientAddr.sin_addr.s_addr &&
                g_addrPC.sin_addr.s_addr != clientAddr.sin_addr.s_addr)
        {
            memcpy(&g_addrPC,&clientAddr,sizeof(g_addrPC));
        }
        Analyse(&clientAddr,szBuf + strlen(DATAPACK_HEAD),ret - strlen(DATAPACK_HEAD) - strlen(DATAPACK_TAIL));
    }
    printf("****pthread_t=[%lx] End****\n",pthread_self());
    pthread_exit(&retVal);
}
//#################################################
//**** Public Function
int InitUdpServer(void)
{
    int nRet = -1;
    do
    {
        nRet = GetNetworkInfo();
    }
    while(0);
    return nRet;
}
int StartUdpServer(pfuncDoNetSocket pfunc)
{
    int ret = -1;
    do
    {
        if(!pfunc)
        {
            break;
        }
        StopUdpServer();
        funcDoNetSocket = pfunc;
        ret = -1;
        g_udpServer = socket(AF_INET,SOCK_DGRAM,0);
        if(0 > g_udpServer)
        {
            break;
        }
        struct sockaddr_in serverAddr;
        memset(&serverAddr,0x00,sizeof(serverAddr));
        serverAddr.sin_port = htons(g_NetworkInfo.nLocalPort);
        printf("<%s:%d>LocalPort=[%d]\n",__FILE__,__LINE__,g_NetworkInfo.nLocalPort);
        serverAddr.sin_family = AF_INET;
//        if(0 < strlen(g_NetworkInfo.szIP))
//        {
//            serverAddr.sin_addr.s_addr = inet_addr(g_NetworkInfo.szIP);
//        }
//        else
//        {
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);//
//        }
        int addrLen = sizeof(serverAddr);
        ret = bind(g_udpServer,(const struct sockaddr*)&serverAddr,addrLen);
        printf("<%s:%d>bind=[%d]\n",__FILE__,__LINE__,ret);
        if(0 > ret)
        {
            break;
        }
        g_nRun = 1;
        ret = pthread_create(&g_ptServer,NULL,(void*)(&UdpServerProc),NULL);
        printf("<%s:%d>pthread_create=[%d]\n",__FILE__,__LINE__,ret);
        if(ret)
        {
            break;
        }
        ret = 0;
    }
    while(0);
    if(0 != ret)
    {
        StopUdpServer();
    }
    return ret;
}
void StopUdpServer(void)
{
    g_nRun = 0;
    if(0 < g_udpServer)
    {
        shutdown(g_udpServer,2);
        close(g_udpServer);
        g_udpServer = -1;
    }
    if(0 != g_ptServer)
    {
        void* retVal;
        pthread_join(g_ptServer,&retVal);
        g_ptServer = 0;
    }
    funcDoNetSocket = NULL;
}
void ReplyClient(const LPTASKDATANODE_s pNode)
{
    switch(pNode->eCmd)
    {
    case SHCMD_CTRL_TD:
        ReplyCtrlTD(pNode);
        break;
    case SHCMD_SCAN_TD_STATE:
        ReplyTDState(pNode);
        break;
    default:
        break;
    }
}
void ReplyAllClient(void)
{
    cJSON* root = cJSON_CreateObject();
    do
    {
        if(!root)
        {
            break;
        }
        cJSON_AddStringToObject(root,"Cmd",CMDRET_SCANTDSTATE);
        cJSON_AddStringToObject(root,"GW",g_NetworkInfo.szMac);
        cJSON* sdArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root,"SD",sdArray);
        cJSON* td = cJSON_CreateObject();
        cJSON_AddItemToArray(sdArray,td);
        cJSON_AddStringToObject(td,"ID",g_TD[0].id);
        if(TDSTATE_SHUTON == g_TD[0].eState)
        {
            cJSON_AddStringToObject(td,"State","1");
            cJSON_AddStringToObject(td,"Value","1");
        }
        else
        {
            cJSON_AddStringToObject(td,"State","0");
            cJSON_AddStringToObject(td,"Value","0");
        }
        char* out = cJSON_Print(root);
        if(!out)
        {
            break;
        }
        char cmd[4096];
        memset(cmd,0x00,sizeof(cmd));
        sprintf(cmd,"%s%s%s",DATAPACK_HEAD,out,DATAPACK_TAIL);
        free(out);
        printf("<%s(%d):%s>cmd=[%s]\n",__FILE__,__LINE__,__FUNCTION__,cmd);
        sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)&g_addrRemoteServer,sizeof(struct sockaddr));
        sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)&g_addrPC,sizeof(struct sockaddr));
    }
    while(0);
    cJSON_Delete(root);
}
//**** Send Cmd ****
