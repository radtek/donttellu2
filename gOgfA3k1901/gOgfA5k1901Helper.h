/**
 * @file   gOgfA3k1901Helper.h
 * @brief  封装王曙光A3000活体采集仪，符合一体化采集动态库标准接口
 * @author 北京东方金指科技有限公司
 * @date   2012/12/28 13:24
 * @note   Edit by guosong 
 * @copyright  版权所有(@) 1996-2012
 *             北京东方金指科技有限公司
 */


#ifndef _INC_OGFA3K1901HELPER_H_20121215215311_
#define _INC_OGFA3K1901HELPER_H_20121215215311_


typedef	struct	tagGOGFA3K_AppData
{
	CFingerSensor::DEVICEPARAMS DeviceParams;

	GYTHIMAGE_HELPDATA	stData;

	int		nWidth, nHeight;			//!< 标准要求的最终图像的宽度和高度
	int		nCaptWidth, nCaptHeight;	//!< 采集仪采集到的图像的宽度和高度
	int		nMaxChannel;				//!< 最大通道数，在这里等于1
	int		nChannelIndex;				//!< 当前选中的通道号，默认为0
	int		nBright, nContrast;			//!< 亮度、对比度

	UCHAR	*pDataBuffer;				//!< 用来存放采集图像的Buffer

	int		bInited;		//!< 是否已经初始化
	int		bOpen;
	int		bActive;
}GOGFA3K_APPDATA;

extern	GOGFA3K_APPDATA	g_stOgfA3KData;
extern	CFingerSensor	g_stFingerSensor;



//!< A3000采集仪的图像大小；初始的亮度、对比度信息；每像素位数、每英寸像素数
typedef	enum	GOGFA3K_DEVICE_INFO
{
	OGFA3K_PRODUCTID = 19,
	//OGFA3K_MAXWIDTH = 640,	OGFA3K_MAXHEIGHT = 640,
	//OGFA3K_WIDTH = 640, OGFA3K_HEIGHT = 640,
	OGFA3K_INITBRIGHT = 200, OGFA3K_INITCONTRAST = 128,
	OGFA3K_XOFFSET = 270, OGFA3K_YOFFSET = 140,
	OGFA3K_BPP = 8, OGFA3K_PPI = 500, 

	OGFA3K_VERSION = 100,

	OGFA3K_UNKNOW
};

#define OGFA3K_DEVICE_DESC		"A3000指纹采集仪一体化采集标准接口"

int		InitialCaptureData();
int		UnInitCaptureData();
int		UTIL_ConvertBright(int nValue);
int		GetSetCaptBright(int nChannel, int* pnBright, UCHAR bGet);
int		UTIL_ConvertContrast(int nValue);
int     GetSetCaptContrast(int nChannel, int* pnContrast, UCHAR bGet);
int		GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
int		SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
int		UTIL_GetOneRawData(UCHAR *pbnCaptured);
DWORD	WINAPI	OgfGA3K_THREAD_StartVideo(LPVOID /*pParam*/);

#endif	// _INC_OGFA3K1901HELPER_H_20121215215311_