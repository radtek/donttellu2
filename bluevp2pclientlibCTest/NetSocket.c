#include "NetSocket.h"
#include "cJSON.h"
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

#define BUFSIZE 8192
//**** type ****
struct route_info
{
    u_int dstAddr;
    u_int srcAddr;
    u_int gateWay;
    char ifName[IF_NAMESIZE];
};

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
static const char TDID[9] = "00001001";

static struct sockaddr_in g_addrRemoteServer;
//static struct sockaddr_in g_addrPC;
//---- Socket
static int g_udpServer = -1;
static int g_nRun = 0;
//---- Thread
static pthread_t g_ptServer = 0;
//---- callback function
static pfuncDoNetSocket funcDoNetSocket;
//#################################################
//**** Public Reply Function ****
int ReplyCtrlTD(const struct sockaddr_in* pAddr,SHRESULT_e eRet)
{
    if(!pAddr){
        return -1;
    }
    int nRet = -2;
    cJSON* root = cJSON_CreateObject();
    do
    {
        if(!root){
            break;
        }
        cJSON_AddStringToObject(root,"Cmd",CMDRET_CTRLTD);
        cJSON_AddStringToObject(root,"GW",g_NetworkInfo.szMac);
        cJSON* sdArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root,"SD",sdArray);
        cJSON* td = cJSON_CreateObject();
        cJSON_AddItemToArray(sdArray,td);
        cJSON_AddStringToObject(td,"ID",TDID);
        if(SHRESULT_OK == eRet){
            cJSON_AddStringToObject(td,"Ret","0");
        }else{
            cJSON_AddStringToObject(td,"Ret","1");
        }
        char* out = cJSON_Print(root);
        if(!out){
            nRet = -3;
            break;
        }
        char cmd[4096];
        memset(cmd,0x00,sizeof(cmd));
        sprintf(cmd,"%s%s%s",DATAPACK_HEAD,out,DATAPACK_TAIL);
        free(out);
        nRet = sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)pAddr,sizeof(struct sockaddr));
    }
    while(0);
    cJSON_Delete(root);
    return nRet;
}
int ReplyTDState(const struct sockaddr_in* pAddr,TDSTATE_e eState)
{
    if(!pAddr){
        return -1;
    }
    int nRet = -2;
    cJSON* root = cJSON_CreateObject();
    do
    {
        if(!root){
            break;
        }
        cJSON_AddStringToObject(root,"Cmd",CMDRET_SCANTDSTATE);
        cJSON_AddStringToObject(root,"GW",g_NetworkInfo.szMac);
        cJSON* sdArray = cJSON_CreateArray();
        cJSON_AddItemToObject(root,"SD",sdArray);
        cJSON* td = cJSON_CreateObject();
        cJSON_AddItemToArray(sdArray,td);
        cJSON_AddStringToObject(td,"ID",TDID);
        if(TDSTATE_SHUTON == eState){
            cJSON_AddStringToObject(td,"State","1");
            cJSON_AddStringToObject(td,"Value","1");
        }else{
            cJSON_AddStringToObject(td,"State","0");
            cJSON_AddStringToObject(td,"Value","0");
        }
        char* out = cJSON_Print(root);
        if(!out){
            nRet = -3;
            break;
        }
        char cmd[4096];
        memset(cmd,0x00,sizeof(cmd));
        sprintf(cmd,"%s%s%s",DATAPACK_HEAD,out,DATAPACK_TAIL);
        free(out);
        nRet = sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)pAddr,sizeof(struct sockaddr));
    }
    while(0);
    cJSON_Delete(root);

    return nRet;
}
static int ReplyScanControler(const struct sockaddr_in* pAddr)
{
    if(!pAddr){
        return -1;
    }
    int nRet = -2;
    cJSON* root = cJSON_CreateObject();
    do
    {
        if(!root){
            break;
        }
        cJSON_AddStringToObject(root,"Cmd",CMDRET_SCANCONTROLER);
        cJSON_AddStringToObject(root,"GW",g_NetworkInfo.szMac);
        cJSON* param = cJSON_CreateObject();
        cJSON_AddItemToObject(root,"Param",param);
        cJSON_AddStringToObject(param,"IP",g_NetworkInfo.szIP);
        cJSON_AddStringToObject(param,"Mac",g_NetworkInfo.szMac);
        cJSON_AddStringToObject(param,"Submask",g_NetworkInfo.szSubmask);
        cJSON_AddStringToObject(param,"Gateway",g_NetworkInfo.szGateway);
        cJSON_AddStringToObject(param,"RSIP",g_NetworkInfo.szRSIP);
        //char szTemp[8];
        //bzero(szTemp,sizeof(szTemp));
        //sprintf(szTemp,"%d",g_NetworkInfo.nRSPort);
        cJSON_AddNumberToObject(param,"RSPort",g_NetworkInfo.nRSPort);
        cJSON_AddStringToObject(param,"RSType",g_NetworkInfo.szRSType);


        char* out = cJSON_Print(root);
        if(!out){
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
    if(SHRESULT_OK == eRet){
        sprintf(cmd,"%s{\"Cmd\":\"%s\",\"GW\":\"%s\",\"Ret\":\"0\"}%s",
                    DATAPACK_HEAD,
                    CMDRET_SETCONTROLER,
                    g_NetworkInfo.szMac,
                    DATAPACK_TAIL);
    }else{
        sprintf(cmd,"%s{\"Cmd\":\"%s\",\"GW\":\"%s\",\"Ret\":\"1\"}%s",
                    DATAPACK_HEAD,
                    CMDRET_SETCONTROLER,
                    g_NetworkInfo.szMac,
                    DATAPACK_TAIL);
    }
    return sendto(g_udpServer,cmd,strlen(cmd),0,(const struct sockaddr*)pAddr,sizeof(struct sockaddr));
}
//**** Private Function ****
static int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;
    do{
        //鏀跺埌鍐呮牳鐨勫簲绛�
        if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
        {
            perror("SOCK READ: ");
            return -1;
        }
        nlHdr = (struct nlmsghdr *)bufPtr;
        //妫�鏌eader鏄惁鏈夋晥
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
        {
            break;
        }
    } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
    return msgLen;
}
//---- 鍒嗘瀽杩斿洖鐨勮矾鐢变俊鎭�
static void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway, char *ifName)
{
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;
    char *tempBuf = NULL;
    struct in_addr dst;
    struct in_addr gate;

    tempBuf = (char *)malloc(100);
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
    // If the route is not for AF_INET or does not belong to main routing table
    //then return.
    if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
        return;

    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);
    for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
        switch(rtAttr->rta_type) {
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

static int get_gateway(char *gateway, char *ifName)
{
    struct nlmsghdr *nlMsg;
//    struct rtmsg *rtMsg;
    struct route_info *rtInfo;
    char msgBuf[BUFSIZE];

    int sock, len, msgSeq = 0;

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


    if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
        printf("Write To Socket Failed鈥n");
        return -1;
    }


    if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
        printf("Read From Socket Failed鈥n");
        return -1;
    }
    rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
    for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
        memset(rtInfo, 0, sizeof(struct route_info));
        parseRoutes(nlMsg, rtInfo, gateway, ifName);
    }
    free(rtInfo);
    close(sock);
    return 0;
}
static int SetIPAddr(const char* szIP,const char* szSubmask,const char* szGateway)
{
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(0 > fd){
        return -1;
    }
    int nRet = -2;
    do
    {
        struct ifreq req;
        bzero(&req,sizeof(req));
        struct sockaddr_in* addr = (struct sockaddr_in*)&(req.ifr_addr);
        addr->sin_family = AF_INET;
        if(szIP && strcmp(g_NetworkInfo.szIP,szIP)){
            if(0 > inet_aton(szIP,&(addr->sin_addr))){
                break;
            }
            if(0 > ioctl(fd,SIOCSIFADDR,&req)){
                break;
            }
        }
        if(szSubmask && strcmp(g_NetworkInfo.szSubmask,szSubmask)){
            if(0 > inet_aton(szSubmask,&(addr->sin_addr))){
                break;
            }
            if(0 > ioctl(fd,SIOCSIFNETMASK,&req)){
                break;
            }
        }
        if(szGateway && strcmp(g_NetworkInfo.szGateway,szGateway)){
            struct rtentry rt;
            bzero(&rt,sizeof(rt));
            //bzero(addr,sizeof(struct sockaddr_in));
            addr = (struct sockaddr_in*)&(rt.rt_gateway);
            addr->sin_family = AF_INET;
            addr->sin_port = 0;
            if(0 > inet_aton(szGateway,&(addr->sin_addr))){
                break;
            }
            ((struct sockaddr_in*)&(rt.rt_dst))->sin_family = AF_INET;
            ((struct sockaddr_in*)&(rt.rt_genmask))->sin_family = AF_INET;
            rt.rt_flags = RTF_GATEWAY;
            if(0 > ioctl(fd,SIOCADDRT,&rt)){
                break;
            }
        }
        nRet = 0;
    }
    while(0);
    close(fd);
    return nRet;
}
static int GetLocalInfo(void)
{
    //assert(pIP);
    bzero(&g_addrRemoteServer,sizeof(g_addrRemoteServer));
    bzero(&g_NetworkInfo,sizeof(g_NetworkInfo));
    int nRet = get_gateway(g_NetworkInfo.szGateway, g_NetworkInfo.szifName);
    if(0 != nRet){
        printf("<%s:%d>get_gateway nRet=[%d]\n",__FILE__,__LINE__,nRet);
        return nRet;
    }
    printf("<%s:%d>interface=[%s],Gateway=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szifName,g_NetworkInfo.szGateway);

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(0 > sockfd){
        return -1;
    }
    do
    {
        struct ifreq req;
        bzero(&req,sizeof(req));
        strncpy(req.ifr_name,g_NetworkInfo.szifName,strlen(g_NetworkInfo.szifName));
        if(0 > ioctl(sockfd,SIOCGIFADDR,&req)){
            nRet = -2;
            break;
        }
        struct sockaddr_in* host = (struct sockaddr_in*)&(req.ifr_addr);
        if(!host){
            nRet = -3;
            break;
        }
        strcpy(g_NetworkInfo.szIP,inet_ntoa(host->sin_addr));
        printf("<%s:%d>IP=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szIP);
        //------
        //bzero(&req,sizeof(req));
        //strncpy(req.ifr_name,"eth0",strlen("eth0"));
        if(0 > ioctl(sockfd,SIOCGIFHWADDR,&req)){
            nRet = -4;
            break;
        }
        sprintf(g_NetworkInfo.szMac,"%02x%02x%02x%02x%02x%02x",
                                    (unsigned char)req.ifr_hwaddr.sa_data[0],
                                    (unsigned char)req.ifr_hwaddr.sa_data[1],
                                    (unsigned char)req.ifr_hwaddr.sa_data[2],
                                    (unsigned char)req.ifr_hwaddr.sa_data[3],
                                    (unsigned char)req.ifr_hwaddr.sa_data[4],
                                    (unsigned char)req.ifr_hwaddr.sa_data[5]);
        printf("<%s:%d>Mac=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szMac);
        //-----
        if(0 > ioctl(sockfd,SIOCGIFNETMASK,&req)){
            nRet = -5;
            break;
        }
        strcpy(g_NetworkInfo.szSubmask,inet_ntoa(((struct sockaddr_in*)&(req.ifr_netmask))->sin_addr));
        printf("<%s:%d>NetMask=[%s]\n",__FILE__,__LINE__,g_NetworkInfo.szSubmask);
        g_NetworkInfo.nLocalPort = 17101;
        //Remote Server
        //strncpy(g_NetworkInfo.szRSIP,"10.171.89.196",sizeof(g_NetworkInfo.szRSIP));
        strncpy(g_NetworkInfo.szRSIP,"123.57.72.97",sizeof(g_NetworkInfo.szRSIP));
        g_NetworkInfo.nRSPort = 7899;
        strncpy(g_NetworkInfo.szRSType,"UDP",sizeof(g_NetworkInfo.szRSType));
        g_addrRemoteServer.sin_addr.s_addr = inet_addr(g_NetworkInfo.szRSIP);
        g_addrRemoteServer.sin_port = htons(g_NetworkInfo.nRSPort);
    }
    while(0);
    close(sockfd);
    //char ifName[12];
    //bzero(ifName,sizeof(ifName));
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
       strncmp(g_NetworkInfo.szMac,gw->valuestring,strlen(g_NetworkInfo.szMac))){
        return;
    }
    cJSON* sd = sdArray->child;
    while(sd){
        cJSON* tdid = cJSON_GetObjectItem(sd,"ID");
        cJSON* tdctrl = cJSON_GetObjectItem(sd,"Ctrl");
        if(tdid && tdid->valuestring && tdctrl && tdctrl->valuestring &&
           0 == strncmp(TDID,tdid->valuestring,strlen(TDID))){
            if(0 == strncmp(tdctrl->valuestring,"1",sizeof(char))){
                funcDoNetSocket(SHCMD_CTRL_TD,pAddr,CTRLTDCMD_SHUTON);
            }else if(0 == strncmp(tdctrl->valuestring,"0",sizeof(char))){
                funcDoNetSocket(SHCMD_CTRL_TD,pAddr,CTRLTDCMD_SHUTOFF);
            }else{
                ReplyCtrlTD(pAddr,SHRESULT_ERROR);
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
       strncmp(g_NetworkInfo.szMac,gw->valuestring,strlen(g_NetworkInfo.szMac))){
        return;
    }
    cJSON* sd = sdArray->child;
    while(sd){
        if(sd->valuestring && 0 == strncmp(TDID,sd->valuestring,strlen(TDID))){
            funcDoNetSocket(SHCMD_SCAN_TD_STATE,pAddr,CTRLTDCMD_SHUTON);
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
       strncmp(g_NetworkInfo.szMac,gw->valuestring,strlen(g_NetworkInfo.szMac))){
        return;
    }
    cJSON* ip = cJSON_GetObjectItem(param,"IP");
    cJSON* submask = cJSON_GetObjectItem(param,"Submask");
    cJSON* gateway = cJSON_GetObjectItem(param,"Gateway");
    cJSON* rsip = cJSON_GetObjectItem(param,"RSIP");
    cJSON* rsport = cJSON_GetObjectItem(param,"RSPort");
    cJSON* rstype = cJSON_GetObjectItem(param,"RSType");
    printf("<%s:%d>ip=[%s],Submask=[%s],Gateway=[%s],RSIP=[%s],RSPort=[%s],RSType=[%s]\n",
           __FILE__,__LINE__,
           ip->valuestring,
           submask->valuestring,
           gateway->valuestring,
           rsip->valuestring,
           rsport->valuestring,
           rstype->valuestring);

    if(rsip && rsip->valuestring && 0 < strlen(rsip->valuestring)){
        strncpy(g_NetworkInfo.szRSIP,rsip->valuestring,sizeof(g_NetworkInfo.szRSIP));
        g_addrRemoteServer.sin_addr.s_addr = inet_addr(g_NetworkInfo.szRSIP);
    }
    if(rstype && rstype->valuestring && 0 < strlen(rstype->valuestring)){
        strncpy(g_NetworkInfo.szRSType,rstype->valuestring,sizeof(g_NetworkInfo.szRSType));
    }
    if(rsport && rsport->valuestring && 0 < strlen(rsport->valuestring)){
        g_NetworkInfo.nRSPort = atoi(rsip->valuestring);
        g_addrRemoteServer.sin_port = htons(g_NetworkInfo.nRSPort);
    }
    if(SetIPAddr(ip->valuestring,submask->valuestring,gateway->valuestring)){
        ReplySetControler(pAddr,SHRESULT_ERROR);
    }else{
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
        if(!root){
            break;
        }
        cJSON* cmd = cJSON_GetObjectItem(root,"Cmd");
        if(!cmd){
            break;
        }
        if(0 == strncmp(cmd->valuestring,CMDASK_SCANCONTROLER,strlen(CMDASK_SCANCONTROLER))){
            DoScanControler(pAddr);
        }else if(0 == strncmp(cmd->valuestring,CMDASK_CTRLTD,strlen(CMDASK_CTRLTD))){
            DoCtrlTD(pAddr,root);
        }else if(0 == strncmp(cmd->valuestring,CMDASK_SCANTDSTATE,strlen(CMDASK_SCANTDSTATE))){
            DoScanTDState(pAddr,root);
        }else if(0 == strncmp(cmd->valuestring,CMDASK_SETCONTROLER,strlen(CMDASK_SETCONTROLER))){
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
        //memset(&clientAddr,0x00,addrLen);
        //memset(szBuf,0x00,nLen);
        ret = recvfrom(g_udpServer,szBuf,nLen,0,(struct sockaddr*)&clientAddr,&addrLen);
        printf("recvfrom=[%d]\n",ret);
        if(ret <= 0 || 0 >= strlen(szBuf)){
            continue;
        }
		if(strlen(szBuf) != ret ||
			0 != strncmp(DATAPACK_HEAD,szBuf,strlen(DATAPACK_HEAD)) ||
			0 != strncmp(DATAPACK_TAIL,(szBuf + ret - 4),strlen(DATAPACK_TAIL))){
			continue;
		}
		szBuf[ret - strlen(DATAPACK_TAIL)] = '\0';
        #ifdef _NETSOCKET_DEBUG_
        printf("data=[%s]\n",szBuf + strlen(DATAPACK_HEAD));
        #endif
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
        if(0 != GetLocalInfo()){
            break;
        }
        nRet = 0;
    }
    while(0);
    return nRet;
}

int StartUdpServer(pfuncDoNetSocket pfunc)
{
    int ret = -1;
    do
    {
        if(!pfunc){
            break;
        }
        StopUdpServer();
        funcDoNetSocket = pfunc;
        ret = -1;
        g_udpServer = socket(AF_INET,SOCK_DGRAM,0);
        if(0 > g_udpServer){
            break;
        }
        struct sockaddr_in serverAddr;
        memset(&serverAddr,0x00,sizeof(serverAddr));
        serverAddr.sin_port = htons(g_NetworkInfo.nLocalPort);
        printf("<%s:%d>LocalPort=[%d]\n",__FILE__,__LINE__,g_NetworkInfo.nLocalPort);
        serverAddr.sin_family = AF_INET;
        //if(0 < strlen(g_NetworkInfo.szIP)){
        //    serverAddr.sin_addr.s_addr = inet_addr(g_NetworkInfo.szIP);
       // }else{
            serverAddr.sin_addr.s_addr = INADDR_ANY;//inet_aton(INADDR_ANY);
        //}
        int addrLen = sizeof(serverAddr);
        ret = bind(g_udpServer,(const struct sockaddr*)&serverAddr,addrLen);
        printf("<%s:%d>bind=[%d]\n",__FILE__,__LINE__,ret);
        if(0 > ret){
            break;
        }
        g_nRun = 1;
        ret = pthread_create(&g_ptServer,NULL,(void*)(&UdpServerProc),NULL);
        printf("<%s:%d>pthread_create=[%d]\n",__FILE__,__LINE__,ret);
        if(ret){
            break;
        }
        ret = 0;
    }
    while(0);
    if(0 != ret){
        StopUdpServer();
    }
    return ret;
}

void StopUdpServer(void)
{
    g_nRun = -1;
    if(0 < g_udpServer){
        shutdown(g_udpServer,2);
        close(g_udpServer);
        g_udpServer = -1;
    }
    if(0 != g_ptServer){
        void* retVal;
        pthread_join(g_ptServer,&retVal);
        g_ptServer = 0;
    }
    funcDoNetSocket = NULL;
}

//**** Send Cmd ****
