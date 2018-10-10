#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static int fsize[3] = {10581, 3874, 5069};
static char buf[1024 * 100];
static FILE *fp, *fps;
static int pos = 0;
static int init = 0;
static char *pframe[3];

int frame_init(void)
{
	if ((fp = fopen("c.264", "rb")) == NULL) {
		printf("open failed!\n");
			return -1;
	}

	fread(buf, 1, 10581 + 3874 + 5069, fp);
	fclose(fp);
	pframe[0] = buf;
	pframe[1] = &buf[10581];
	pframe[2] = &buf[10581 + 3874];

	return 0;
}

int get_frame(char *pbuf)
{
	int len;

	if (init == 0) {
		frame_init();
		init = 1;
	}
	len = fsize[pos];
	memcpy(pbuf, &buf[pos],len);
	pos++;
	pos %= 3;
	return len;
}

