#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "upnpctrlpoint.h"

#define   ETH_NAME   "eth0"

int GetLocalIP(char szIP[])
{
   int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(0 > sockfd)
        return -1;

	struct	ifreq ifr;
	memset(&ifr, 0, sizeof ifr);
	strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ);

	if(ioctl(sockfd, SIOCGIFADDR, &ifr) == 0)
	{
		struct sockaddr_in	*host = (struct sockaddr_in *)&ifr.ifr_addr;
		if(!host)	return -1;
		strcpy(szIP, inet_ntoa(host->sin_addr));
	}

	return 0;
}

int main()
{
	int						nErrCode = 0;
	PortMapping	pmObj;
	memset(&pmObj, 0, sizeof(pmObj));

	CTRLPOINT_Start();
	printf("\nstart..\n");

	do
	{
		printf("\nwait..\n");
		sleep(1);
	}
	while(1 > CTRLPOINT_TravelDeviceList());

	CTRLPOINT_ACTION_GetExternalIP(1);

	while(0 > CTRLPOINT_GetResult(GETEXTERNALIP, &nErrCode))
	{
		printf("\nwait result..\n");
		sleep(1);
	}

	printf("\ngetexternip result:%d\n", nErrCode);
	getchar();

	char	szIP[16] = {0};
	CTRLPOINT_RESULT_GetExternalIP(szIP, &nErrCode);

	printf("\ngetexternip:%s, errcode:%d\n", szIP, nErrCode);
	getchar();

	char	szClient[16] = {0};
	GetLocalIP(szClient);
	//pmObj.pszRemoteHost = "host";
	pmObj.nExternPort = 7899;
	pmObj.nInterPort = 7899;
	pmObj.pszClient = szClient;
	//pmObj.pszClient = "172.16.0.110";
	//pmObj.pszClient = "192.168.3.101";
	pmObj.bUDP = 1;
	pmObj.bEnable = 1;
	pmObj.pszDesc = "bluevision";

	while(1)
	{
		printf("\nadd port mapping..\n");
		CTRLPOINT_ACTION_AddPortMapping(1, &pmObj);

		while(0 > CTRLPOINT_GetResult(ADDPORTMAPPING, &nErrCode))
		{
			//可能断网导致action失败
			if(nErrCode != 0)
				break;

			printf("\nwait result..\n");
			sleep(1);
		}

		printf("\naddportmapping result:%d\n", nErrCode);
		if(nErrCode == 0)
			break;
	}

	getchar();

	printf("\nquery port mapping..\n");
	CTRLPOINT_ACTION_QueryPortMapping(1, &pmObj);

	while(0 > CTRLPOINT_GetResult(QUERYPORTMAPPING, &nErrCode))
	{
		printf("\nwait result..\n");
		sleep(1);
	}

	printf("\nqueryportmapping result:%d\n", nErrCode);
	getchar();

	CTRLPOINT_RESULT_QueryPortMapping(&pmObj, &nErrCode);

	printf("\nqueryportmapping client:%s, desc:%s, enable:%d, interport:%d, leaseduration:%d, errcode:%d\n",
		pmObj.pszClient, pmObj.pszDesc, pmObj.bEnable,
		pmObj.nInterPort, pmObj.nLeaseDuration, nErrCode);
	getchar();

	printf("\ndel port mapping..\n");
	CTRLPOINT_ACTION_DelPortMapping(1, &pmObj);

	while(0 > CTRLPOINT_GetResult(DELPORTMAPPING, &nErrCode))
	{
		printf("\nwait result..\n");
		sleep(1);
	}

	printf("\ndelportmapping result:%d\n", nErrCode);
	getchar();

	CTRLPOINT_Stop();
	printf("\nend..\n");

    return 0;
}
