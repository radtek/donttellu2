#ifndef OGFUSB_DLL_H
#define OGFUSB_DLL_H
//
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
//#include"CHardware.h"
//
#define OK	1
#define FAIL 0
//
#define IMG_BUF_WIDTH	640
#define IMG_BUF_HEIGHT	480//480
//
//#define IMG_WIDTH	640
//#define IMG_HEIGHT	480//0
//
#define IMG_WIDTH	452
#define IMG_HEIGHT 452//0

//#define FRAME_WIDTH	905
//#define FRAME_OFFSET	55
#define IMG_LINE_OFFSET 0
//
BOOL APIENTRY OGFUSB_Open(void);
BOOL APIENTRY OGFUSB_Close(void);
BOOL APIENTRY OGFUSB_Active(void);
BOOL APIENTRY OGFUSB_Deactive(void);
BOOL APIENTRY OGFUSB_Led(BOOL on);
BOOL APIENTRY OGFUSB_ReadImage(unsigned char *imagebuff);
BOOL APIENTRY OGFUSB_SetLight(unsigned char light);
BOOL APIENTRY OGFUSB_SetContrast(unsigned char contrast);
BOOL APIENTRY OGFUSB_GetParamData(long *pData);
//
#endif
