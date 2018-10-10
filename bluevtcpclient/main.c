#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "bluevtcpclientlib.h"

#define   ETH_NAME   "eth0"




int main(int argc, char **argv)
{
	printf("%s\n", argv[0]);

	char	bMainCtrl = 0;
	if(argc > 1)
	{
		printf("MAINCTRL:%s\n", argv[1]);
		bMainCtrl = argv[1][0] == '1' ? 1 : 0;
	}

	char	*pszJson =	\
	"{"
	"	\"svrip\":\"123.57.72.97\",	"
	"	\"svrport\":8878,	"
	"	\"lanport\":8856,	"
	"	\"ismainctrl\":1,	"
	"	\"username\":\"admin1\",	"
	"	\"password\":\"admin1\"	"
	"}";

	char	*pszJson2 =	\
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

	if(bMainCtrl)
		TCPCLIENT_Init(pszJson);
	else
		TCPCLIENT_Init(pszJson2);

	TCPCLIENT_Start();

	TCPCLIENT_Login();

	TCPCLIENT_Keepalive(5);

	if(bMainCtrl)
	{
		while(!TCPCLIENT_IsLogin())
		{
			printf("wait login..\n");
			sleep(1);
			continue;
		}

		char	*pszInfo = NULL;
		if(0 > TCPCLIENT_GetDevice(&pszInfo) || pszInfo == NULL)
		{
			TCPCLIENT_Logout();
			TCPCLIENT_End();

			return 0;
		}

		printf("%s", pszInfo);
	}



	while(1)
		sleep(1);

	TCPCLIENT_Logout();

	TCPCLIENT_End();
//	OpenTCPServer(8897);
//	StartTCPServer("192.168.1.224", 8878);
//
//	char	szIP[16] = {0};
//	GetLocalIP(szIP);
//
//	LogInContext	ctx;
//	ctx.pszLanIp = szIP;
//	ctx.nLanPort = 9978;
//	ctx.pszMask = "255.255.255.0";
//	ctx.pszGateWay = "192.168.1.1";
//	ctx.pszDevId = "deviceid1";
//
//	ctx.bUPNP = 1;
//	ctx.pszUPNPIP = szIP;
//	ctx.nUPNPPort = 9991;
//
//	LogIn(&ctx);
//	LogOut();
//
//	CloseTCPServer();

    return 0;
}
