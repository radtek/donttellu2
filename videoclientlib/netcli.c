#include "netcli.h"
#define	BUFSIZE	(1024 * 1024)

enum PIPE_DIRECTION {
	PIPE_READ = 0,
	PIPE_WRITE
};

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

struct netstat {
	int flag;
	int sockfd;
	int pos;
	int offset;
	char *buf;

	int32_t hold;
	int32_t devid;
	int16_t cmd;
	int16_t arg;
	int32_t pipe[2];
	int32_t hit;
	pthread_mutex_t lock;

};

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

static struct netstat status;

int net_init(void)
{
	if (status.flag == 1)
		return 0;
	memset(&status, 0, sizeof(struct netstat));
	status.flag = 1;
	status.buf = malloc(BUFSIZE);
	if (status.buf == NULL)
		return -1;

	pipe(status.pipe);
	pthread_mutex_init(&(status.lock),NULL);
	return 0;
}


void net_exit(void)
{
	if (status.flag != 1)
		return;
	close(status.pipe[PIPE_READ]);
	close(status.pipe[PIPE_WRITE]);
	if (status.sockfd != -1 && status.sockfd != 0)
		close(status.sockfd);
	pthread_mutex_destroy(&(status.lock));
	free(status.buf);
	memset(&status, 0, sizeof(struct netstat));
	return;
}

static int msg_hold(int devid, short cmd, short arg, int msec)
{
	int ret, hit = 0;
	fd_set rdfds;
	struct timeval tv;
	char d;

	FD_ZERO(&rdfds);
	FD_SET(status.pipe[1], &rdfds);
	tv.tv_sec = 0;
	tv.tv_usec = msec * 1000;

	pthread_mutex_lock(&(status.lock));
	status.hold = 1;
	status.devid = devid;
	status.cmd = cmd;
	status.arg = arg;
	status.hit = 0;
	pthread_mutex_unlock(&(status.lock));

	ret = select(status.pipe[PIPE_READ] + 1, &rdfds, NULL, NULL, &tv);
	if (ret > 0) {
		pthread_mutex_lock(&(status.lock));
		status.hold = 0;
		hit = status.hit;
		pthread_mutex_unlock(&(status.lock));
	} else if (ret == 0) {
		pthread_mutex_lock(&(status.lock));
		status.hold = 0;
		hit = status.hit;
		pthread_mutex_unlock(&(status.lock));
		if (hit == 0)
			return RET_TIMEOUT;
	} else {
		return -1;
	}

	if (hit == 1) {
		read(status.pipe[PIPE_READ], &d, 1);
		return 0;
	}
	return -1;
}

static int _msg_recv(int *devid, short *cmd, short *arg, int *dlen, void **data)
{
	int ret, len;
	struct pack *pack;

	if (status.offset > 0) {
		if (status.pos >  status.offset)
			memmove(status.buf, &status.buf[status.offset], status.pos - status.offset);
		status.pos -= status.offset;
		status.offset = 0;
		if (status.pos >= 16)
			goto pack_process;
	}

	do {
		ret = read(status.sockfd, &(status.buf[status.pos]), BUFSIZE - status.pos);
		if (ret == -1 && errno == EAGAIN)
			continue;
		if (ret == -1 && ret == 0) {
			close(status.sockfd);
			status.sockfd = -1;
			return -1;
		}
		status.pos += ret;
		if (status.pos >= 16) {
pack_process:
			pack = (struct pack *)status.buf;
			len = ntohl(pack->len);
			if (len + 16 > status.pos)
				continue;
			pack_ntoh(pack);
			*devid = pack->devid;
			*cmd = pack->cmd;
			*arg = pack->arg;
			*dlen = pack->len;
			*data = pack->data;
			status.offset = len + 16;
			return 0;
		}
	} while (1);

	return 0;
}


int msg_recv(int *devid, short *cmd, short *arg, int *dlen, void **data)
{
	int ret, pipe_write = 0;
	char d = 1;

	do {
		ret = _msg_recv(devid, cmd, arg, dlen, data);
		if (ret != 0)
			return ret;

		pthread_mutex_lock(&(status.lock));
		if (status.hold == 1) {
			if (status.devid == *devid && status.cmd == *cmd && status.arg == *arg) {
				status.hit = 1;
				status.hold = 0;
				pipe_write = 1;
			}
		}
		pthread_mutex_unlock(&(status.lock));
		if (pipe_write == 1) {
			write(status.pipe[PIPE_WRITE], &d, 1);
			pipe_write = 0;
		} else {
			break;
		}

	} while (1);

	return ret;
}

static int _msg_send(int devid, short cmd, short arg, int len, void *data)
{
	struct pack *pack = (struct pack *)status.buf;
	int ret = 0;

	pack->header = 0xCCAA;
	pack->version = 1;
	pack->reserve = 0;
	pack->devid = devid;
	pack->cmd = cmd;
	pack->arg = arg;
	if (len > 0) {
		pack->len = len;
		memcpy(pack->data, data, len);
	} else {
		pack->len = 0;
	}

	pack_hton(pack);

	do {
		ret = write(status.sockfd, pack, ((len > 0) ? len : 0) + 16);
	} while (ret == -1 && errno == EAGAIN);

	if (ret == -1) {
		close(status.sockfd);
		status.sockfd = -1;
	}
	return ret;
}

int msg_send(int devid, short cmd, short arg, int len, void *data, int exp)
{
	int ret;

	ret = _msg_send(devid, cmd, arg, len, data);
	if (exp > 0) {
		return msg_hold(devid, cmd, arg, exp);
	}
	return ret;
}

int mkdid(short type, short id)
{
	int did;

	did = (type << 16) + id;

	return did;
}

short get_dev_type(int did)
{
	return (short)(did >> 16);
}

short get_dev_subid(int did)
{
	return (short)did & 0xFFFF;
}

int tcp_connect(char *ip, short port)
{
	int sockfd;
	struct sockaddr_in server_addr;
	if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket error:");
		return -1;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("connect error:");
		return -2;
	}
	return sockfd;
}

int net_connect(char *ip, short port)
{
	int ret;

	if (status.sockfd == 0 || status.sockfd == -1) {
		if ((ret = tcp_connect(ip, port)) > 0) {
			status.sockfd = ret;
			return 0;
		} else {
			return -1;
		}
	}
	return 0;
}

/**********************************************/
/* COMMON API */

int light_on(int id, int exp)
{
	return msg_send(id, CMD_LIGHT_ON, 0, 0, NULL, exp);
}

int light_off(int id, int exp)
{
	return msg_send(id, CMD_LIGHT_OFF, 0, 0, NULL, exp);
}
int light_bright(int id, int bright, int exp)
{
	return msg_send(id, CMD_LIGHT_BRIGHT, bright, 0, NULL, exp);
}

int scene_leave(int exp)
{
	int id = mkdid(DEV_TYPE_SCENE, 0);
	return msg_send(id, CMD_SCENE_LEAVE, 0, 0, NULL, exp);
}

int scene_back(int exp)
{
	int id = mkdid(DEV_TYPE_SCENE, 0);
	return msg_send(id, CMD_SCENE_BACK, 0, 0, NULL, exp);
}

int scene_dinner(int exp)
{
	int id = mkdid(DEV_TYPE_SCENE, 0);
	return msg_send(id, CMD_SCENE_DINNER, 0, 0, NULL, exp);
}

int scene_movie(int exp)
{
	int id = mkdid(DEV_TYPE_SCENE, 0);
	return msg_send(id, CMD_SCENE_MOVIE, 0, 0, NULL, exp);
}

int scene_party(int exp)
{
	int id = mkdid(DEV_TYPE_SCENE, 0);
	return msg_send(id, CMD_SCENE_PARTY, 0, 0, NULL, exp);
}

int scene_sleep(int exp)
{
	int id = mkdid(DEV_TYPE_SCENE, 0);
	return msg_send(id, CMD_SCENE_SLEEP, 0, 0, NULL, exp);
}

int camera_play(int id, int exp)
{
	return msg_send(id, CMD_CAMERA_PLAY, 0, 0, NULL, exp);
}

int camera_stop(int id, int exp)
{
	return msg_send(id, CMD_CAMERA_STOP, 0, 0, NULL, exp);
}

int curtain_open(int id)
{
	return msg_send(id, CMD_CURTAIN_OPEN, 0, 0, NULL, 0);
}

int curtain_close(int id)
{
	return msg_send(id, CMD_CURTAIN_CLOSE, 0, 0, NULL, 0);
}
int window_open(int id)
{
	return msg_send(id, CMD_WINDOW_OPEN, 0, 0, NULL, 0);
}

int window_close(int id)
{
	return msg_send(id, CMD_WINDOW_CLOSE, 0, 0, NULL, 0);
}


int glass_opacity(int id, short value, int exp)
{
	return msg_send(id, CMD_GLASS_OPACITY, value, 0, NULL, exp);
}

int projector_on(int id, int exp)
{
	return msg_send(id, CMD_PROJECTOR_ON, 0, 0, NULL, exp);
}

int projector_off(int id, int exp)
{
	return msg_send(id, CMD_PROJECTOR_OFF, 0, 0, NULL, exp);
}

int robot_forward(int id)
{
	return msg_send(id, CMD_ROBOT_FORWARD, 0, 0, NULL, 0);
}

int robot_backward(int id)
{
	return msg_send(id, CMD_ROBOT_BACKWARD, 0, 0, NULL, 0);
}

int robot_turnleft(int id)
{
	return msg_send(id, CMD_ROBOT_TURNLEFT, 0, 0, NULL, 0);
}

int robot_turnright(int id)
{
	return msg_send(id, CMD_ROBOT_TURNRIGHT, 0, 0, NULL, 0);
}

int theatre_dvd(int exp)
{
	int id = mkdid(DEV_TYPE_THEATRE, 0);
	return msg_send(id, CMD_THEATRE_DVD, 0, 0, NULL, exp);
}
int theatre_stb(int exp)
{
	int id = mkdid(DEV_TYPE_THEATRE, 0);
	return msg_send(id, CMD_THEATRE_STB, 0, 0, NULL, exp);
}
int theatre_camera(int camid, int exp)
{
	int id = mkdid(DEV_TYPE_THEATRE, 0);
	return msg_send(id, CMD_THEATRE_CAMERA, camid, 0, NULL, exp);
}

int get_status(int id)
{
	return msg_send(id, CMD_GET_STATUS, id, 0, NULL, 0);
}

