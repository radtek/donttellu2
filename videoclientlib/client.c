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

int main(void)
{
	char ip[] = {"127.0.0.1"};
	short port = 5011;
	pthread_t tid;
	int ret;
	int devid, dlen;
	short cmd, arg;
	char *p;

	net_init();
	if (net_connect(ip, port) != 0) {
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

