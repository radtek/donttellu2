#pragma once

#include <windows.h>
#include "FingerSensor.h"
#include "liveapp.h"
#include "./gfscivil/galsutil19.h"

//#define ID_BUILD_USEA1000
//#define ID_BUILD_USEA2000
#define ID_BUILD_USEA5000

#define GFLS_VERSION			       100

#define GFLS_CAPTCHANNEL_INITBRIGHT			298
#define GFLS_CAPTCHANNEL_INITCONTRAST		10
#ifdef UNSTD_BLACK_BALANCE
#define GFLS_CAPTCHANNEL_INITBLACKBALANCE		190
#endif
//#define GFLS_CAPTCHANNEL_ACTUALINITBRIGHT		300
//#define GFLS_CAPTCHANNEL_ACTUALINITCONTRAST		20

#define GFLS_CAPTCHANNEL_MINBRIGHT          100//100  //!<  亮度最大 1-511
#define GFLS_CAPTCHANNEL_MAXBRIGHT          700//400

#define GFLS_CAPTCHANNEL_MINCONTRAST         5//10   //!<  对比度最大 0-127
#define GFLS_CAPTCHANNEL_MAXCONTRAST         25//40

#ifdef ID_BUILD_USEA5000
#define GFLS_CAPT_DESCSTART		L"801217912"
#define GFLS_CAPT_DESCEND		L",LiveScan Capture Machine,A5000"
#define GFLS_CAPT_CARDDESC		L"80121791220082423832,LiveScan Capture Machine,A5000"
#define GFLS_CAPT_CARDDESCFIRST			11
#define GFLS_CAPT_CUTWIDTH          440
#define GFLS_CAPT_CUTHEIGHT         440
#endif			//ID_BUILD_USEA3000

typedef	struct	tagGALSFPRCAP2000_Data
{
	CFingerSensor::DEVICEPARAMS DeviceParams;

	int		nWidth, nHeight;			//!< 标准要求的最终图像的宽度和高度
	int		nCaptWidth, nCaptHeight;	//!< 采集仪采集到的图像的宽度和高度
	int		nMaxChannel;				//!< 最大通道数，在这里等于1
	int		nChannelIndex;				//!< 当前选中的通道号，默认为0
	int		nBright, nContrast;			//!< 亮度、对比度
#ifdef UNSTD_BLACK_BALANCE
	int		nBlackBalance;				//!< 黑平衡
#endif
	int		nOriginX, nOriginY;			//!< 图像采集窗口的采集原点坐标X、Y值

	UCHAR	*pDataBuffer;		//!< 用来存放采集图像的Buffer
	UCHAR	*pCutBuffer;        //!< 用来存放切割纠偏后的图像。


	int   nNum;
	UCHAR	*pBuffer;        //!< 用来存放图像。
	//int		nMaxWidth, nMaxHeight; //!< 最大宽度和高度	
	//UTIL_HELPDATA		stData;
	//int		nDispDataBuffer;	//!< pDataBuffer的大小

	int		bInited;		//!< 是否已经初始化
	int		bOpen;
	int		bActive;
	int     nCaptMode;  //!< 默认为0； 对采集到的图像做截取和纠偏，值为1；

	UCHAR	bAdjustBright;
	UCHAR	bAdjustContrast;
	UCHAR	bnReserved[2];

}GALSFPRCAP2000_DATA;

extern	GALSFPRCAP2000_DATA	g_stFprCapData;
extern	CFingerSensor g_stFingerSensor;

void    FreeCaptDataAndClose();
int		InitialCaptureData();
int		UnInitCaptureData();
int		UTIL_ConvertBright2Exposure(int nValue, BOOL bReverse = FALSE);
int		UTIL_ConvertExposure2Bright(int nValue, BOOL bReverse = FALSE);
int		GetSetCaptBright(int nChannel, int* pnBright, UCHAR bGet);
int		UTIL_ConvertContrast2Gain(int nValue, BOOL bReverse = FALSE);
int		UTIL_ConvertGain2Contrast(int nValue, BOOL bReverse = FALSE);
int     GetSetCaptContrast(int nChannel, int* pnContrast, UCHAR bGet);
#ifdef UNSTD_BLACK_BALANCE
int     GetSetCaptBlackBalance(int nChannel, int* pBlackBalance, UCHAR bGet);
#endif
int     GetCaptMaxImageSize(int nChannel, int *pnWidth, int* pnHeight);
int		GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
int		SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);

int		GetGAStdErrorInfo(int nErrorNo, char *pszErrorInfo, int ncbErrorInfo, int nGAStdType);
void	UTIL_SaveToBmpFileFormat(UCHAR *pBmpFileData, int nWidth, int nHeight);
int		GetSerialNo(unsigned int& nSerialNo);
//解决一台机器使用二台指纹仪的问题
int		InitialCaptureDataEx(int nSerialNo);
int		OpenSerialNo(unsigned int nSerialNo, unsigned nWantedSerialNo);


#define	PARAVALUE_BUFFER	128
void    GetModulePath(char *pszBufferFileName);
int	    PARAMETER_GetValueArray(char *fname, char **sParaName, char(*sParaValue)[PARAVALUE_BUFFER], int nItem);
int		PARAMETER_GetValue(char *fname, char *sParaName, char *sParaValue);
int     GflsInternalSetOriginParam();

#define GFLS_CAPTMODE_SPLITE  1
int     GetRectifyInfo(UCHAR *pDataBuffer, int nWidth, int nHeight, UTIL19_REGIONRECT *pstRegion);
int		GetRectifyImageData(UCHAR *pCaptureData, int nwCapture, int nhCapture,
	UCHAR *pRectifyData, UTIL19_REGIONRECT *pstRectifyInfo);
int		GetImageRectifyInfo(UCHAR *pCaptureData, int nwCapture, int nhCapture, UTIL19_REGIONRECT *pstRectifyInfo);

int	SaveDataToBitmapFile(const char* szFileName, UCHAR *ImgData, int nWidth, int nHeight, int nBits);

