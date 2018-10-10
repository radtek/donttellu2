#ifndef NETCLI_H
#define NETCLI_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>
#include <errno.h>
#include "flags.h"

#define RET_OK		0
#define RET_ERROR	-1
#define RET_TIMEOUT	-2

/* basic api */
int net_init(void);
void net_exit(void);
int msg_recv(int *devid, short *cmd, short *arg, int *dlen, void **data);
int msg_send(int devid, short cmd, short arg, int len, void *data, int exp);
int mkdid(short type, short subid);
short get_dev_type(int did);
short get_dev_subid(int did);
int net_connect(char *ip, short port);

/**********************************************/
/* COMMON API */
/* return :
 * 0 : ok
 * -1: error
 */
int light_on(int id, int exp);
int light_off(int id, int exp);
int light_bright(int id, int bright, int exp);
int scene_leave(int exp);
int scene_back(int exp);
int scene_dinner(int exp);
int scene_movie(int exp);
int scene_party(int exp);
int scene_sleep(int exp);
int camera_play(int id, int exp);
int camera_stop(int id, int exp);
int curtain_open(int id);
int curtain_close(int id);
int window_open(int id);
int window_close(int id);
int glass_opacity(int id, short value, int exp);
int projector_on(int id, int exp);
int projector_off(int id, int exp);
int robot_forward(int id);
int robot_backward(int id);
int robot_turnleft(int id);
int robot_turnright(int id);
int theatre_dvd(int exp);
int theatre_stb(int exp);
int theatre_camera(int camid, int exp);

/* id:
 * if id == 0 return all device status
 */
int get_status(int id);
#endif

