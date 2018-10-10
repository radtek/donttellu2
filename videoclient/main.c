#include "netcli.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "show.h"

#define LIGHT_ID	((DEV_TYPE_LIGHT << 16) + 1)
#define LIGHT_ID2	((DEV_TYPE_LIGHT << 16) + 2)
#define LIGHT_ID3	((DEV_TYPE_LIGHT << 16) + 3)
#define SCENE_ID	((DEV_TYPE_SCENE << 16) + 1)
#define CAMERA_ID	((DEV_TYPE_CAMERA << 16) + 1)
#define CURTAIN_ID	((DEV_TYPE_CURTAIN << 16) + 1)
#define WINDOW_ID	((DEV_TYPE_WINDOW << 16) + 1)
#define GLASS_ID	((DEV_TYPE_GLASS << 16) + 1)
#define PROJECTOR_ID	((DEV_TYPE_PROJECTOR << 16) + 1)
#define ROBOT_ID	((DEV_TYPE_ROBOT << 16) + 1)
#define DVD_ID		((DEV_TYPE_DVD << 16) + 1)
#define STB_ID		((DEV_TYPE_STB << 16) + 1)
#define THEATRE_ID	((DEV_TYPE_THEATRE << 16) + 1)
void show_menu(void)
{
	printf("========== TEST COMMAND ==========\n");
	printf("a: light_on\n");
	printf("b: light_off\n");
	printf("c: light_bright 100\n");
	printf("d: light_bright 50\n");
	printf("e: scene_leave\n");
	printf("f: scene_back\n");
	printf("g: camera_play\n");
	printf("h: camera_stop\n");
	printf("i: get status\n");

	return;
}

void *control_thread(void *arg)
{
	int i;
	char buf[128];
	printf("Thread start...\n");

	while (1) {
		fgets(buf, sizeof(buf), stdin);
		buf[127] = '\0';

		for (i = 0; i < 128 && buf[i] != '\0'; i++) {
			switch (buf[i]) {
			case 'a':
				if (light_on(LIGHT_ID, 200) == 0) {
					printf("LIGHT[%08x] ON - OK!\n", LIGHT_ID);
				} else {
					printf("LIGHT[%08x] ON - TIMEOUT!\n", LIGHT_ID);
				}
				break;
			case 'b':
				if (light_off(LIGHT_ID2, 200) == 0) {
					printf("LIGHT[%08x] OFF - OK!\n", LIGHT_ID2);
				} else {
					printf("LIGHT[%08x] OFF - TIMEOUT!\n", LIGHT_ID2);
				}
				break;
			case 'c':
				if (light_bright(LIGHT_ID3, 100, 200) == 0) {
					printf("LIGHT[%08x] BRIGHT 100 - OK!\n", LIGHT_ID3);
				} else {
					printf("LIGHT[%08x] BRIGHT 100 - TIMEOUT!\n", LIGHT_ID3);
				}
				break;
			case 'd':
				if (light_bright(LIGHT_ID3, 50, 200) == 0) {
					printf("LIGHT[%08x] BRIGHT 50 - OK!\n", LIGHT_ID3);
				} else {
					printf("LIGHT[%08x] BRIGHT 50 - TIMEOUT!\n", LIGHT_ID3);
				}
				break;
			case 'e':
				if (scene_leave(800) == 0) {
					printf("SCENE LEAVE - OK!\n");
				} else {
					printf("SCENE LEAVE - TIMEOUT!\n");
				}
				break;
			case 'f':
				if (scene_back(800) == 0) {
					printf("SCENE BACK - OK!\n");
				} else {
					printf("SCENE BACK - TIMEOUT!\n");
				}
				break;
			case 'g':
				if (camera_play(CAMERA_ID, 200) == 0) {
					printf("CAMERA [%d] PLAY - OK!\n", CAMERA_ID);
				} else {
					printf("CAMERA [%d] PLAY - TIMEOUT!\n", CAMERA_ID);
				}
				break;
			case 'h':
				if (camera_stop(CAMERA_ID, 200) == 0) {
					printf("CAMERA [%d] STOP - OK!\n", CAMERA_ID);
				} else {
					printf("CAMERA [%d] STOP - TIMEOUT!\n", CAMERA_ID);
				}
				break;
			case 'i':
				get_status(0);
				break;

			case 'm':
				show_menu();
				break;
			default:
				break;
			}
		}
	}

	return NULL;
}


#include "bluevtcpclientlib.h"
#define	SVR_IP	"123.57.72.97"
#define	SVR_PORT	8878
#define	USERNAME	"admin1"
#define	PASSWORD	"admin1"
#define	KEEPALIVE_SECOND	15
#define	LAN_PORT	8856

	char	*pszJson =	\
	"{"
	"	\"svrip\":\"123.57.72.97\",	"
	"	\"svrport\":8878,	"
	"	\"lanport\":8856,	"
	"	\"ismainctrl\":1,	"
	"	\"username\":\"admin1\",	"
	"	\"password\":\"admin1\"	"
	"}";

static int Server_LogIn()
{
	LogInContext	ctx;
	memset(&ctx, 0, sizeof(ctx));

	//if(0 > TCPCLIENT_GetLocalInfo(&ctx))
	//	return -1;

	ctx.bMainCtrl = 1;
	ctx.nLanPort = LAN_PORT;
	strncpy(ctx.szUserName, USERNAME, sizeof(ctx.szUserName));
	strncpy(ctx.szPassWord, PASSWORD, sizeof(ctx.szPassWord));

	TCPCLIENT_InitEx(&ctx, SVR_IP, SVR_PORT);

	TCPCLIENT_Start();

	TCPCLIENT_Login();

	TCPCLIENT_Keepalive(KEEPALIVE_SECOND);

	return 0;
}

static int Server_GetDevice(char szIP[16], int *pnPort)
{
	if(szIP == NULL || pnPort == NULL)
		return -1;

	while(!TCPCLIENT_IsLogin())
	{
		printf("wait login..\n");
		sleep(1);
		continue;
	}

	DeviceInfo	dev;
	memset(&dev, 0, sizeof(dev));
	if(0 > TCPCLIENT_GetDeviceEx(&dev) )
	{
		printf("null device\n");
		TCPCLIENT_Logout();
		TCPCLIENT_End();

		return 0;
	}

	if(dev.bSameLan)
	{
		strcpy(szIP, dev.szLanIp);
		*pnPort = dev.nLanPort;
	}
	else if(dev.bUPNP)
	{
		strcpy(szIP, dev.szUPNPIP);
		*pnPort = dev.nUPNPPort;
	}
	else
	{
		strcpy(szIP, dev.szIp);
		*pnPort = dev.nPort;
	}

	return 0;
}

int main()
{
	if(0 > Server_LogIn())
		return -1;

	char	szDevIP[16] = {0};
	int		nDevPort = 0;
    if(0 > Server_GetDevice(szDevIP, &nDevPort) || nDevPort <= 0)
		return -1;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
	pthread_t tid;
	int ret;
	int devid, dlen;
	short cmd, arg;
	char *p;

	net_init();
	if (net_connect(szDevIP, nDevPort) != 0) {
		fprintf(stderr, "Can't connect to server!\n");
		return -1;
	}

	ret = pthread_create(&tid, NULL, (void*)control_thread, NULL);
	if(ret != 0) {
		printf("Thread Create Error\n");
		exit(0);
	}
	do {
		ret =  msg_recv(&devid, &cmd, &arg, &dlen, (void **)&p);
		printf("========================\nmessage received:\n");
		printf("Device ID:\t%08x[type:%s,id:%d]\n", (unsigned int)devid,
			showtype(get_dev_type(devid)), get_dev_subid(devid));
		printf("Command:\t%s[%d]\n", showcmd(cmd), cmd);
		printf("Arg:\t%d\n", arg);
		printf("Data Length:\t%d\n", dlen);

	} while (ret == 0);
	fprintf(stderr, "Net disconnected!\n");

	pthread_join(tid, NULL);

	return 0;
}

