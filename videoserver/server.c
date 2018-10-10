#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "enet.h"
#include "ev.h"
#include "flags.h"
#define SRV_TCP_PORT	5011
#define MAX_CLIENTS	10
#define MAX_BUFSIZE	128

#define MKDEVID(DEVTYPE, SUBID)	(((DEVTYPE) << 16) + (SUBID))

#define LIGHT_ID(SUBID)		MKDEVID(DEV_TYPE_LIGHT, SUBID)
#define SCENE_ID(SUBID)		MKDEVID(DEV_TYPE_SCENE, SUBID)
#define CAMERA_ID(SUBID)	MKDEVID(DEV_TYPE_CAMERA, SUBID)
#define CURTAIN_ID(SUBID)	MKDEVID(DEV_TYPE_CURTAIN, SUBID)
#define WINDOW_ID(SUBID)	MKDEVID(DEV_TYPE_WINDOW, SUBID)
#define GLASS_ID(SUBID)		MKDEVID(DEV_TYPE_GLASS, SUBID)
#define PROJECTOR_ID(SUBID)	MKDEVID(DEV_TYPE_PROJECTOR, SUBID)
#define ROBOT_ID(SUBID)		MKDEVID(DEV_TYPE_ROBOT, SUBID)
#define PROJECTOR_ID(SUBID)	MKDEVID(DEV_TYPE_PROJECTOR, SUBID)
#define DVD_ID(SUBID)		MKDEVID(DEV_TYPE_DVD, SUBID)
#define STB_ID(SUBID)		MKDEVID(DEV_TYPE_STB, SUBID)
#define THEATRE_ID(SUBID)	MKDEVID(DEV_TYPE_THEATRE, SUBID)

struct pack {
	uint16_t header;
	uint8_t version;
	uint8_t reserve;
	uint32_t devid;
	uint16_t cmd;
	int16_t arg;
	uint32_t len;
	uint8_t data[1];
};

int get_frame(char *pbuf);
void pack_hton(struct pack *pack)
{
	pack->header = htons(pack->header);
	pack->devid = htonl(pack->devid);
	pack->cmd = htons(pack->cmd);
	pack->arg = htons(pack->arg);
	pack->len = htonl(pack->len);
	return;
}

void pack_ntoh(struct pack *pack)
{
	pack->header = ntohs(pack->header);
	pack->devid = ntohl(pack->devid);
	pack->cmd = ntohs(pack->cmd);
	pack->arg = ntohs(pack->arg);
	pack->len = ntohl(pack->len);
	return;
}

struct client {
	int sockfd;
	int buf[MAX_BUFSIZE];
	int pos;
	int video[3];
};

struct client clients[MAX_CLIENTS];


struct status {
	int id;
	short cmd;
	short arg;
};

struct status stats[] = {
	{LIGHT_ID(1), CMD_LIGHT_ON, 0},
	{SCENE_ID(1), CMD_SCENE_LEAVE, 0},
	{CAMERA_ID(0), CMD_CAMERA_STOP, 0},
	{CAMERA_ID(1), CMD_CAMERA_STOP, 0},
	{CAMERA_ID(2), CMD_CAMERA_STOP, 0},
	{CURTAIN_ID(1), CMD_CURTAIN_CLOSE, 0},
	{WINDOW_ID(1), CMD_WINDOW_OPEN, 0},
	{GLASS_ID(1), CMD_GLASS_OPACITY, 50},
	{PROJECTOR_ID(1), CMD_PROJECTOR_ON, 0},
	{ROBOT_ID(1), CMD_ROBOT_FORWARD, 0},
	{THEATRE_ID(1), CMD_THEATRE_DVD, 0},
	{MKDEVID(DEV_TYPE_SENSOR_TEMP, 1), CMD_SENSOR_TEMP, 20},
	{MKDEVID(DEV_TYPE_SENSOR_HUMI, 1), CMD_SENSOR_HUMI, 20},
	{MKDEVID(DEV_TYPE_SENSOR_PM25, 1), CMD_SENSOR_PM25, 10},
	{MKDEVID(DEV_TYPE_SENSOR_CO, 1), CMD_SENSOR_CO, 10},
	{MKDEVID(DEV_TYPE_SENSOR_DOOR, 1), CMD_SENSOR_DOOR, 10},
	{MKDEVID(DEV_TYPE_SENSOR_WINDOW, 1), CMD_SENSOR_WIN, 10},
	{MKDEVID(DEV_TYPE_SENSOR_SMOKE, 1), CMD_SENSOR_SMOKE, 10},
	{MKDEVID(DEV_TYPE_SENSOR_HUMAN, 1), CMD_SENSOR_HUMAN, 10},
	{MKDEVID(DEV_TYPE_SENSOR_BRIGHT, 1), CMD_SENSOR_BRIGHT, 10},
};


int get_status(int idx, struct pack *pack)
{
	int i, id = pack->devid;

	for (i = 0; i < (int)(sizeof(stats)/sizeof(struct status)); i++) {
		if (id == 0) {
			pack->header = 0xAACC;
			pack->version = 0;
			pack->devid = stats[i].id;
			pack->cmd = stats[i].cmd;
			pack->arg = stats[i].arg;
			pack->len = 0;
			pack_hton(pack);
			write(clients[idx].sockfd, pack, 16);
		} else if (id == stats[i].id) {
			pack->header = 0xAACC;
			pack->version = 0;
			pack->devid = stats[i].id;
			pack->cmd = stats[i].cmd;
			pack->arg = stats[i].arg;
			pack->len = 0;
			pack_hton(pack);
			write(clients[idx].sockfd, pack, 16);
			break;
		}
	}

	return 0;
}

int set_status(int id, short cmd, short arg)
{
	int i;

	for (i = 0; i < (int)(sizeof(stats)/sizeof(struct status)); i++) {
		if (stats[i].id == id) {
			stats[i].cmd = cmd;
			stats[i].arg = arg;
			break;
		}
	}

	return 0;
}

int message_process(int idx)
{
	int len, i;
	struct pack *pack;
	if (clients[idx].pos < 16)
		return 0;
	pack = (struct pack *)clients[idx].buf;
	len = ntohl(pack->len);
	if (len + 16 > clients[idx].pos)
		return 0;
	pack_ntoh(pack);
	if (pack->devid == LIGHT_ID(2))
		goto out;

	switch (pack->cmd) {
	case CMD_CURTAIN_OPEN:
	case CMD_CURTAIN_CLOSE:
	case CMD_WINDOW_OPEN:
	case CMD_WINDOW_CLOSE:
	case CMD_ROBOT_FORWARD:
	case CMD_ROBOT_BACKWARD:
	case CMD_ROBOT_TURNLEFT:
	case CMD_ROBOT_TURNRIGHT:
		break;
	case CMD_CAMERA_PLAY:
		clients[idx].video[pack->devid & 0xFF] = 1;

		pack->header = 0xAACC;
		pack_hton(pack);
		write(clients[idx].sockfd, pack, 16);
		break;
	case CMD_CAMERA_STOP:
		clients[idx].video[pack->devid & 0xFF] = 0;

		pack->header = 0xAACC;
		pack_hton(pack);
		write(clients[idx].sockfd, pack, 16);
		break;
	case CMD_GET_STATUS:
		get_status(idx, pack);
		break;
	default:
		pack->header = 0xAACC;
		set_status(pack->devid, pack->cmd, pack->arg);
		pack_hton(pack);
		for (i = 0; i < MAX_CLIENTS; i++) {
			if (clients[i].sockfd > 0)
				write(clients[i].sockfd, pack, len + 16);
		}
		break;
	}
out:
	clients[idx].pos -= len + 16;

	return 1;
}

void read_data_handle(ev_event_loop * el, int fd, void *privdata, int mask)
{
	int i, ret;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].sockfd == fd) {
			break;
		}
	}
	if (i < MAX_CLIENTS) {
		do {

			ret = read(fd, &(clients[i].buf[clients[i].pos]), MAX_BUFSIZE - clients[i].pos);

		} while (ret == -1 && errno == EAGAIN);

		if (ret <= 0) {
			ev_delete_file_event(el, fd, EV_READABLE);
			clients[i].sockfd = 0;
			clients[i].pos = 0;
			memset(clients[i].video, 0, sizeof(int) * 3);
			close(fd);
			return;
		}
		clients[i].pos += ret;

		while (message_process(i) == 1);
	}
	return;
}


void accept_tcp_handle(ev_event_loop * el, int fd, void *privdata, int mask)
{
	int cfd, cport, i;
	char ip_addr[128] = { 0 };

	cfd = enet_tcp_accept(NULL, fd, ip_addr, 128, &cport);
	if (cfd == ENET_ERR) {
		fprintf(stderr, "Accept error!\n");
		return;
	}

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].sockfd == 0) {
			if (ev_create_file_event(el, cfd, EV_READABLE,
						read_data_handle, NULL) == EV_ERR) {
			}
			clients[i].sockfd = cfd;
			return;
		}
	}

	close(cfd);
	return;
}

void *video_thread(void *arg)
{
	int i, j, len;
	unsigned count = 0;
	char buf[1024 * 20];
	struct pack *pack = (struct pack *)buf;

	do {
		sleep(2);

		len = get_frame(&buf[16]);
		for (i = 0; i < MAX_CLIENTS; i++) {
			if (clients[i].sockfd > 0) {
				for (j = 0; j < 3; j++) {
					if (clients[i].video[j] == 1) {
						//send frame
						pack->header = 0xAACC;
						pack->version = 0;
						pack->devid = MKDEVID(DEV_TYPE_CAMERA, j);
						pack->cmd = CMD_SEND_FRAME;
						pack->arg = count;
						pack->len = len;
						pack_hton(pack);
						write(clients[i].sockfd, pack, 16 + len);
					}
				}
			}
		}
		count++;
	} while (1);


	return NULL;
}


#include "bluevtcpclientlib.h"
#include "upnpctrlpoint.h"

#define	SVR_IP	"123.57.72.97"
#define	SVR_PORT	8878
#define	DEVICEID	"deviceid1"
#define	KEEPALIVE_SECOND	15

#define	UPNP_TIMEOUT	5
#define	UPNP_DESC	"bluevision"
static LogInContext			g_ctxLogin;

static int Upnp_Init(char **ppszExternIP, char szLanIP[16], int nPort, char bUDP)
{
	if(ppszExternIP == NULL || szLanIP == NULL || nPort <= 0)
		return -1;

	int						nErrCode = 0, nRetVal = -1;
	PortMapping	pmObj;
	memset(&pmObj, 0, sizeof(pmObj));

	do
	{
		//启动upnp
		if(0 > CTRLPOINT_Start())
			break;
		printf("\nstart..\n");

		//等待发现设备
		int	nSecond = 0;
		do
		{
			printf("\nwait..\n");
			sleep(1);
			if(++nSecond == UPNP_TIMEOUT)
				break;
		}
		while(1 > CTRLPOINT_TravelDeviceList());

		if(nSecond == UPNP_TIMEOUT)
		{
			printf("timeout\n");
			break;
		}

		//获取路由wanip
		CTRLPOINT_ACTION_GetExternalIP(1);

		nSecond = 0;
		while(0 > CTRLPOINT_GetResult(GETEXTERNALIP, &nErrCode))
		{
			printf("\nwait result..\n");
			sleep(1);
			if(++nSecond == UPNP_TIMEOUT)
				break;
		}

		if(nSecond == UPNP_TIMEOUT)
		{
			printf("timeout\n");
			break;
		}

		printf("\ngetexternip result:%d\n", nErrCode);

		char	*pszExternIP = (char *)malloc(16);
		if(pszExternIP == NULL)
			break;
		memset(pszExternIP, 0, 16);

		CTRLPOINT_RESULT_GetExternalIP(pszExternIP, &nErrCode);

		printf("\ngetexternip:%s, errcode:%d\n", pszExternIP, nErrCode);
		*ppszExternIP = pszExternIP;

		//pmObj.pszRemoteHost = "host";
		pmObj.nExternPort = nPort;
		pmObj.nInterPort = nPort;
		pmObj.pszClient = szLanIP;
		//pmObj.pszClient = "172.16.0.110";
		//pmObj.pszClient = "192.168.3.101";
		pmObj.bUDP = bUDP;
		pmObj.bEnable = 1;
		pmObj.pszDesc = UPNP_DESC;

		//可能已存在，先删除
		printf("\ndel port mapping..\n");
		CTRLPOINT_ACTION_DelPortMapping(1, &pmObj);

		nSecond = 0;
		while(0 > CTRLPOINT_GetResult(DELPORTMAPPING, &nErrCode))
		{
			printf("\nwait result..\n");
			sleep(1);
			if(++nSecond == UPNP_TIMEOUT)
				break;
		}

		if(nSecond == UPNP_TIMEOUT)
		{
			printf("timeout\n");
			break;
		}

		printf("\ndelportmapping result:%d\n", nErrCode);

		//增加UPNP信息
		printf("\nadd port mapping..\n");
		CTRLPOINT_ACTION_AddPortMapping(1, &pmObj);

		nSecond = 0;
		while(0 > CTRLPOINT_GetResult(ADDPORTMAPPING, &nErrCode))
		{
			//可能断网导致action失败
			if(nErrCode != 0)
				break;

			printf("\nwait result..\n");
			sleep(1);
			if(++nSecond == UPNP_TIMEOUT)
				break;
		}

		if(nSecond == UPNP_TIMEOUT)
		{
			printf("timeout\n");
			break;
		}

		printf("\naddportmapping result:%d\n", nErrCode);
		if(nErrCode != 0)
			break;

		nRetVal = 0;
	}
	while(0);

	CTRLPOINT_Stop();
	printf("\nend..\n");

	return nRetVal;
}

static int Server_LogIn()
{
	LogInContext	ctx;
	memset(&ctx, 0, sizeof(ctx));

	if(0 > TCPCLIENT_GetLocalInfo(&ctx))
		return -1;

	char	bUpnp = 1;
	char	*pszExternIP = NULL;
	if(0 > Upnp_Init(&pszExternIP, ctx.szLanIp, SRV_TCP_PORT, 0) || pszExternIP == NULL)
	{
		bUpnp = 0;
	}

	ctx.bMainCtrl = 0;
	ctx.nLanPort = SRV_TCP_PORT;
	strcpy(ctx.szDevId, DEVICEID);

	if(bUpnp)
	{
		strcpy(ctx.szUPNPIP, pszExternIP);
		ctx.nUPNPPort = SRV_TCP_PORT;

		ctx.bUPNP = 1;
	}

	if(pszExternIP)
	{
		free(pszExternIP);
		pszExternIP = NULL;
	}

	TCPCLIENT_InitEx(&ctx, SVR_IP, SVR_PORT);

	TCPCLIENT_Start();

	TCPCLIENT_Login();

	TCPCLIENT_Keepalive(KEEPALIVE_SECOND);

	return 0;
}

int main(void)
{
	if(0> Server_LogIn())
		return -1;

	int backlog = 10;
	int srvsock;
	pthread_t tid;
	ev_event_loop *loop = ev_create_event_loop(512);
	srvsock = enet_tcp_server(NULL, SRV_TCP_PORT, NULL, backlog);

	if (ev_create_file_event(loop, srvsock, EV_READABLE, accept_tcp_handle, NULL) == EV_ERR) {
		return -1;
	}

	if (pthread_create(&tid, NULL, (void*)video_thread, NULL) != 0) {
		printf("create thread failed!\n");
	}

	ev_main(loop);
	ev_delete_event_loop(loop);

	return 0;
}


