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

#define GFLS_CAPTCHANNEL_MINBRIGHT          100//100  //!<  ������� 1-511
#define GFLS_CAPTCHANNEL_MAXBRIGHT          700//400

#define GFLS_CAPTCHANNEL_MINCONTRAST         5//10   //!<  �Աȶ���� 0-127
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

	int		nWidth, nHeight;			//!< ��׼Ҫ�������ͼ��Ŀ�Ⱥ͸߶�
	int		nCaptWidth, nCaptHeight;	//!< �ɼ��ǲɼ�����ͼ��Ŀ�Ⱥ͸߶�
	int		nMaxChannel;				//!< ���ͨ���������������1
	int		nChannelIndex;				//!< ��ǰѡ�е�ͨ���ţ�Ĭ��Ϊ0
	int		nBright, nContrast;			//!< ���ȡ��Աȶ�
#ifdef UNSTD_BLACK_BALANCE
	int		nBlackBalance;				//!< ��ƽ��
#endif
	int		nOriginX, nOriginY;			//!< ͼ��ɼ����ڵĲɼ�ԭ������X��Yֵ

	UCHAR	*pDataBuffer;		//!< ������Ųɼ�ͼ���Buffer
	UCHAR	*pCutBuffer;        //!< ��������и��ƫ���ͼ��


	int   nNum;
	UCHAR	*pBuffer;        //!< �������ͼ��
	//int		nMaxWidth, nMaxHeight; //!< ����Ⱥ͸߶�	
	//UTIL_HELPDATA		stData;
	//int		nDispDataBuffer;	//!< pDataBuffer�Ĵ�С

	int		bInited;		//!< �Ƿ��Ѿ���ʼ��
	int		bOpen;
	int		bActive;
	int     nCaptMode;  //!< Ĭ��Ϊ0�� �Բɼ�����ͼ������ȡ�;�ƫ��ֵΪ1��

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
//���һ̨����ʹ�ö�ָ̨���ǵ�����
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

