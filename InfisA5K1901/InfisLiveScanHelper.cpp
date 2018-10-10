#include "infislivescanhelper.h"
#include <math.h>
#include <malloc.h>
#include <memory>


GALSFPRCAP2000_DATA	g_stFprCapData = { 0 };

void FreeCaptDataAndClose()
{
	if (g_stFprCapData.pDataBuffer) free(g_stFprCapData.pDataBuffer);
	if (g_stFprCapData.pCutBuffer) free(g_stFprCapData.pCutBuffer);

	//if ( g_stFprCapData.bActive ) g_stFingerSensor.Deactive();
	if (g_stFprCapData.bOpen) g_stFingerSensor.Close();

	memset(&g_stFprCapData, 0, sizeof(g_stFprCapData));
}

int		InitialCaptureData()
{
	if (g_stFprCapData.bInited) return GF_GALSCAPT_ERROR_SUCCESS;

	int		i, nDevice, nCamType;
	char	szDevName[128], szCM[32] = { 0 };
	char szFileName[MAX_PATH + 8] = { 0 };

	char *szCaptMode = "CAPTMODE";
	unsigned int nSerialNo = 0;
	unsigned int nSerialNo1 = 0;
	bool bRet = false;

	nDevice = g_stFingerSensor.GetCameraCount();
	if (nDevice <= 0) return GF_GALSCAPT_ERROR_NODEVICE;
	//add by wangkui
	g_stFprCapData.nMaxChannel = nDevice;
	// 	g_stFprCapData.nChannelIndex = 0;

	for (i = 0; i < nDevice; ++i)
	{
		if (!g_stFingerSensor.GetCameraName(i, szDevName, nCamType)) continue;
#ifdef ID_BUILD_USEA1000
		if (CAM_A1000 == nCamType) break;
#endif
#ifdef ID_BUILD_USEA2000
		if (CAM_A2000 == nCamType) break;
#endif
#ifdef ID_BUILD_USEA3000
		if (CAM_A3000 == nCamType) break;
#endif
#ifdef ID_BUILD_USEA5000
		if (CAM_A5000 == nCamType) break;
#endif
		//if ( (CAM_A2000 == nCamType) || (CAM_A1000 == nCamType) || (CAM_A3000 == nCamType)) break;

	}
	if (i == nDevice) return GF_GALSCAPT_ERROR_NODEVICE;

	if (!g_stFingerSensor.Open(i)) return GF_GALSCAPT_ERROR_NOTINIT;

	g_stFprCapData.bOpen = 1;

	int		retval, nBright, nControst;

	if (!g_stFingerSensor.GetDeviceParams(&g_stFprCapData.DeviceParams))
	{
		retval = GF_GALSCAPT_ERROR_NOTINIT;
		goto Finish_Exit;
	}
	g_stFprCapData.DeviceParams.nCamType = nCamType;

	if ((g_stFprCapData.DeviceParams.nMaxWindowWidth < GFLS_IDFP_WIDTH) ||
		(g_stFprCapData.DeviceParams.nMaxWindowHeight < GFLS_IDFP_HEIGHT))
	{
		retval = GF_GALSCAPT_ERROR_NOTINIT;
		goto Finish_Exit;
	}
	g_stFprCapData.nMaxChannel = 1;
	g_stFprCapData.nChannelIndex = 0;

	g_stFprCapData.nWidth = GFLS_IDFP_WIDTH;
	g_stFprCapData.nHeight = GFLS_IDFP_HEIGHT;
	if (nCamType == CAM_A2000)
	{
		g_stFprCapData.nCaptWidth = A2000_WIDTH;
		g_stFprCapData.nCaptHeight = A2000_HEIGHT;
	}
	else if (nCamType == CAM_A3000)
	{
		g_stFprCapData.nCaptWidth = A3000_WIDTH;
		g_stFprCapData.nCaptHeight = A3000_HEIGHT;
	}
	else if (nCamType == CAM_A5000)
	{
		g_stFprCapData.nCaptWidth = A5000_WIDTH;
		g_stFprCapData.nCaptHeight = A5000_HEIGHT;
	}
	else
	{
		g_stFprCapData.nCaptWidth = A1000_WIDTH;
		g_stFprCapData.nCaptHeight = A1000_HEIGHT;
	}
	if (g_stFprCapData.DeviceParams.nMaxWindowWidth  < g_stFprCapData.nCaptWidth) g_stFprCapData.DeviceParams.nMaxWindowWidth = g_stFprCapData.nCaptWidth;
	if (g_stFprCapData.DeviceParams.nMaxWindowHeight < g_stFprCapData.nCaptHeight) g_stFprCapData.DeviceParams.nMaxWindowHeight = g_stFprCapData.nCaptHeight;

	g_stFprCapData.nBright = GFLS_CAPTCHANNEL_INITBRIGHT;
	g_stFprCapData.nContrast = GFLS_CAPTCHANNEL_INITCONTRAST;
#ifdef UNSTD_BLACK_BALANCE
	g_stFprCapData.nBlackBalance = GFLS_CAPTCHANNEL_INITBLACKBALANCE;				//!< 黑平衡
#endif
	g_stFprCapData.nOriginX = (g_stFprCapData.nCaptWidth - g_stFprCapData.nWidth) / 2;
	g_stFprCapData.nOriginY = g_stFprCapData.nCaptHeight - g_stFprCapData.nHeight;

	g_stFprCapData.pDataBuffer = (UCHAR*)malloc(g_stFprCapData.DeviceParams.nMaxWindowWidth * g_stFprCapData.DeviceParams.nMaxWindowHeight);
	if (!g_stFprCapData.pDataBuffer)
	{
		retval = GF_GALSCAPT_ERROR_MEMORY;
		goto Finish_Exit;
	}
	g_stFprCapData.pCutBuffer = (UCHAR*)malloc(g_stFprCapData.DeviceParams.nMaxWindowWidth * g_stFprCapData.DeviceParams.nMaxWindowHeight);
	if (!g_stFprCapData.pCutBuffer)
	{
		retval = GF_GALSCAPT_ERROR_MEMORY;
		goto Finish_Exit;
	}

	//if ( !g_stFingerSensor.Active() )
	//{
	//	retval = GF_GALSCAPT_ERROR_NOTINIT;
	//	goto Finish_Exit;
	//}
	g_stFprCapData.bActive = 1;
#if 0	//未提供
	g_stFingerSensor.SetReverse(true);
#endif
	nBright = /*UTIL_ConvertBright*/(g_stFprCapData.nBright);
	nControst = /*UTIL_ConvertContrast*/(g_stFprCapData.nContrast);
	g_stFingerSensor.SetExposure(nBright);
	g_stFingerSensor.SetGain(nControst);
#ifdef UNSTD_BLACK_BALANCE
	g_stFingerSensor.SetLed(g_stFprCapData.nBlackBalance);
#endif
	GetModulePath(szFileName);
	strcat_s(szFileName, "\\");
	strcat_s(szFileName, "ID_Finit.dat");
	if (PARAMETER_GetValue(szFileName, szCaptMode, szCM) > 0)
	{
		g_stFprCapData.nCaptMode = atoi(szCM);
	}

	g_stFprCapData.bInited = 1;

	retval = GF_GALSCAPT_ERROR_SUCCESS;

Finish_Exit:
	if (retval != GF_GALSCAPT_ERROR_SUCCESS) FreeCaptDataAndClose();
	return retval;
}

int		InitialCaptureDataEx(int nSerialNo)
{

	if (g_stFprCapData.bInited) return GF_GALSCAPT_ERROR_SUCCESS;

	int		i, nDevice, nCamType;
	char	szDevName[128], szCM[32] = { 0 };
	char szFileName[MAX_PATH + 8] = { 0 };

	char *szCaptMode = "CAPTMODE";
	//unsigned int nSerialNo = 0;
	unsigned int nSerialNoTrue = 0;
	bool bRet = false;

	nDevice = g_stFingerSensor.GetCameraCount();
	if (nDevice <= 0) return GF_GALSCAPT_ERROR_NODEVICE;
	//add by wangkui
	g_stFprCapData.nMaxChannel = nDevice;
	g_stFprCapData.nChannelIndex = 0;

	for (i = 0; i < nDevice; ++i)
	{
		if (!g_stFingerSensor.GetCameraName(i, szDevName, nCamType)) continue;
#ifdef ID_BUILD_USEA1000
		if (CAM_A1000 == nCamType) break;
#endif
#ifdef ID_BUILD_USEA2000
		if (CAM_A2000 == nCamType) break;
#endif
#ifdef ID_BUILD_USEA3000
		if (CAM_A3000 == nCamType) break;
#endif
#ifdef ID_BUILD_USEA5000
		if (CAM_A5000 == nCamType) break;
#endif
		//if ( (CAM_A2000 == nCamType) || (CAM_A1000 == nCamType) || (CAM_A3000 == nCamType)) break;

	}
	if (i == nDevice) return GF_GALSCAPT_ERROR_NODEVICE;
	//if ( !g_stFingerSensor.Open(szDevName) ) return GF_GALSCAPT_ERROR_NOTINIT;

	// 		if ( !g_stFingerSensor.Open(i) ) return GF_GALSCAPT_ERROR_NOTINIT;
	// 		bRet = g_stFingerSensor.GetSerialNo(nSerialNoTrue);
	// 		if ( nSerialNoTrue != nSerialNo )
	// 		{
	// 			g_stFingerSensor.Close();
	// 			g_stFingerSensor.Open(nDevice-i-1);
	// 			bRet = g_stFingerSensor.GetSerialNo(nSerialNoTrue);
	// 			if ( nSerialNoTrue != nSerialNo )
	// 			{
	// 				g_stFingerSensor.Close();
	// 				return	GF_GALSCAPT_ERROR_NODEVICE;
	// 			}
	// 		}
	for (i = 0; i< nDevice; i++)
	{
		if (!g_stFingerSensor.Open(i)) return GF_GALSCAPT_ERROR_NOTINIT;
#if 0	//未提供
		bRet = g_stFingerSensor.GetSerialNo(nSerialNoTrue);
		if (nSerialNoTrue != nSerialNo)
		{
			g_stFingerSensor.Close();
			continue;
		}
		else break;
#endif
	}
	if (i == nDevice) return GF_GALSCAPT_ERROR_NOTINIT;

	//	if ( !g_stFingerSensor.Open(0) ) return GF_GALSCAPT_ERROR_NOTINIT;
	//		bRet = g_stFingerSensor.GetSerialNo(nSerialNo);
	g_stFprCapData.bOpen = 1;

	int		retval, nBright, nControst;

	if (!g_stFingerSensor.GetDeviceParams(&g_stFprCapData.DeviceParams))
	{
		retval = GF_GALSCAPT_ERROR_NOTINIT;
		goto Finish_Exit;
	}
	g_stFprCapData.DeviceParams.nCamType = nCamType;

	if ((g_stFprCapData.DeviceParams.nMaxWindowWidth < GFLS_IDFP_WIDTH) ||
		(g_stFprCapData.DeviceParams.nMaxWindowHeight < GFLS_IDFP_HEIGHT))
	{
		retval = GF_GALSCAPT_ERROR_NOTINIT;
		goto Finish_Exit;
	}
	g_stFprCapData.nMaxChannel = nDevice;
	g_stFprCapData.nChannelIndex = 0;

	g_stFprCapData.nWidth = GFLS_IDFP_WIDTH;
	g_stFprCapData.nHeight = GFLS_IDFP_HEIGHT;
	if (nCamType == CAM_A2000)
	{
		g_stFprCapData.nCaptWidth = A2000_WIDTH;
		g_stFprCapData.nCaptHeight = A2000_HEIGHT;
	}
	else if (nCamType == CAM_A3000)
	{
		g_stFprCapData.nCaptWidth = A3000_WIDTH;
		g_stFprCapData.nCaptHeight = A3000_HEIGHT;
	}
	else if (nCamType == CAM_A5000)
	{
		g_stFprCapData.nCaptWidth = A5000_WIDTH;
		g_stFprCapData.nCaptHeight = A5000_HEIGHT;
	}
	else
	{
		g_stFprCapData.nCaptWidth = A1000_WIDTH;
		g_stFprCapData.nCaptHeight = A1000_HEIGHT;
	}
	if (g_stFprCapData.DeviceParams.nMaxWindowWidth  < g_stFprCapData.nCaptWidth) g_stFprCapData.DeviceParams.nMaxWindowWidth = g_stFprCapData.nCaptWidth;
	if (g_stFprCapData.DeviceParams.nMaxWindowHeight < g_stFprCapData.nCaptHeight) g_stFprCapData.DeviceParams.nMaxWindowHeight = g_stFprCapData.nCaptHeight;

	g_stFprCapData.nBright = GFLS_CAPTCHANNEL_INITBRIGHT;
	g_stFprCapData.nContrast = GFLS_CAPTCHANNEL_INITCONTRAST;
	g_stFprCapData.nOriginX = (g_stFprCapData.nCaptWidth - g_stFprCapData.nWidth) / 2;
	g_stFprCapData.nOriginY = g_stFprCapData.nCaptHeight - g_stFprCapData.nHeight;

	g_stFprCapData.pDataBuffer = (UCHAR*)malloc(g_stFprCapData.DeviceParams.nMaxWindowWidth * g_stFprCapData.DeviceParams.nMaxWindowHeight);
	if (!g_stFprCapData.pDataBuffer)
	{
		retval = GF_GALSCAPT_ERROR_MEMORY;
		goto Finish_Exit;
	}
	g_stFprCapData.pCutBuffer = (UCHAR*)malloc(g_stFprCapData.DeviceParams.nMaxWindowWidth * g_stFprCapData.DeviceParams.nMaxWindowHeight);
	if (!g_stFprCapData.pCutBuffer)
	{
		retval = GF_GALSCAPT_ERROR_MEMORY;
		goto Finish_Exit;
	}

	//if ( !g_stFingerSensor.Active() )
	//{
	//	retval = GF_GALSCAPT_ERROR_NOTINIT;
	//	goto Finish_Exit;
	//}
	g_stFprCapData.bActive = 1;
#if 0	//未提供
	g_stFingerSensor.SetReverse(true);
#endif
	nBright = /*UTIL_ConvertBright*/(g_stFprCapData.nBright);
	nControst = /*UTIL_ConvertContrast*/(g_stFprCapData.nContrast);
	g_stFingerSensor.SetExposure(nBright);
	g_stFingerSensor.SetGain(nControst);

	GetModulePath(szFileName);
	strcat_s(szFileName, "\\");
	strcat_s(szFileName, "ID_Finit.dat");
	if (PARAMETER_GetValue(szFileName, szCaptMode, szCM) > 0)
	{
		g_stFprCapData.nCaptMode = atoi(szCM);
	}

	g_stFprCapData.bInited = 1;

	retval = GF_GALSCAPT_ERROR_SUCCESS;

Finish_Exit:
	if (retval != GF_GALSCAPT_ERROR_SUCCESS) FreeCaptDataAndClose();
	return retval;

}



int		GetSerialNo(unsigned int& nSerialNo)
{
	int nRet = 0;
	int bInited = g_stFprCapData.bInited;
	InitialCaptureData();
#if 0	//未提供
	if (g_stFingerSensor.GetSerialNo(nSerialNo)) nRet = 1;
#endif
	if (!bInited)
	{
		UnInitCaptureData();
	}
	return nRet;
}

int		OpenSerialNo(unsigned int nSerialNo, unsigned nWantedSerialNo)
{
	int nRet = 0;
	int bInited = g_stFprCapData.bInited;
	InitialCaptureDataEx(nWantedSerialNo);
#if 0	//未提供
	if (g_stFingerSensor.GetSerialNo(nSerialNo)) nRet = 1;
#endif
	if (!bInited)
	{
		UnInitCaptureData();
	}
	return nRet;
}

int		UnInitCaptureData()
{
	if (g_stFprCapData.bInited)
	{
		//if(g_stFprCapData.pDispDataBuffer)
		//	free(g_stFprCapData.pDispDataBuffer);
		FreeCaptDataAndClose();
	}
	return GF_GALSCAPT_ERROR_SUCCESS;
}



int		UTIL_ConvertBright2Exposure(int nValue, BOOL bReverse)
{
	//if (nValue < GFLS_CAPTCHANNEL_MINBRIGHT)
	//	nValue = GFLS_CAPTCHANNEL_MINBRIGHT;
	//else if (nValue > GFLS_CAPTCHANNEL_MAXBRIGHT) 
	//	nValue = GFLS_CAPTCHANNEL_MAXBRIGHT;

	//return nValue;
	int		nMaxExposure, nMinExposure;;
	double	fScale;
	 
	if(bReverse) nValue = 255 - nValue;
	nMaxExposure = GFLS_CAPTCHANNEL_MAXBRIGHT;
	nMinExposure = GFLS_CAPTCHANNEL_MINBRIGHT;
	fScale = (nMaxExposure - nMinExposure) / 255.0;
	 
	double	fBright = fScale * nValue + nMinExposure;
	int		nBright =(int)(fBright);
	 
	if ( nBright <= nMinExposure) nBright = nMinExposure;
	if ( nBright >= nMaxExposure ) nBright = nMaxExposure;
	 
	return nBright;
}

int		UTIL_ConvertExposure2Bright(int nValue, BOOL bReverse)
{
	//if (nValue < GFLS_CAPTCHANNEL_MINBRIGHT)
	//	nValue = GFLS_CAPTCHANNEL_MINBRIGHT;
	//else if (nValue > GFLS_CAPTCHANNEL_MAXBRIGHT) 
	//	nValue = GFLS_CAPTCHANNEL_MAXBRIGHT;

	//return nValue;
	int		nMaxExposure, nMinExposure;;
	double	fScale;

	nMaxExposure = GFLS_CAPTCHANNEL_MAXBRIGHT;
	nMinExposure = GFLS_CAPTCHANNEL_MINBRIGHT;

	if (nValue <= nMinExposure) nValue = nMinExposure;
	if (nValue >= nMaxExposure) nValue = nMaxExposure;

	fScale = (nMaxExposure - nMinExposure) / 255.0;

	double	fBright = (nValue - nMinExposure) / fScale;
	int		nBright = (int)(fBright);
	if (bReverse) nBright = 255 - nBright;

	return nBright;
}

int		SetOriginParam(int nOriginX, int nOriginY)
{
	if (nOriginX < 0 || nOriginY < 0
		|| nOriginX > g_stFprCapData.nCaptWidth
		|| nOriginY > g_stFprCapData.nCaptHeight)
	{
		return GF_GALSCAPT_ERROR_PARAM;
	}
	if (nOriginX + g_stFprCapData.nWidth > g_stFprCapData.nCaptWidth)
	{
		nOriginX = g_stFprCapData.nCaptWidth - g_stFprCapData.nWidth;
	}
	if (nOriginY + g_stFprCapData.nHeight > g_stFprCapData.nCaptHeight)
	{
		nOriginY = g_stFprCapData.nCaptHeight - g_stFprCapData.nHeight;
	}
	g_stFprCapData.nOriginX = nOriginX;
	g_stFprCapData.nOriginY = nOriginY;

	return GF_GALSCAPT_ERROR_SUCCESS;
}

int		GetSetCaptBright(int nChannel, int* pnBright, UCHAR bGet)
{
	if (NULL == pnBright) return GF_GALSCAPT_ERROR_PARAM;

	int	nBright;

	if (bGet)
	{
		//*pnBright = g_stFprCapData.nBright;
		*pnBright = (int)UTIL_ConvertExposure2Bright(g_stFprCapData.nBright);
	}
	else if (*pnBright != g_stFprCapData.nBright)
	{
		g_stFprCapData.nBright = (*pnBright);
		nBright = (int)UTIL_ConvertBright2Exposure(*pnBright);
		if (!g_stFingerSensor.SetExposure(nBright)) return GF_GALSCAPT_ERROR_PARAM;
		g_stFprCapData.bAdjustBright = 1;
		g_stFprCapData.bAdjustContrast = 0;
	}

	return GF_GALSCAPT_ERROR_SUCCESS;
}


int		UTIL_ConvertContrast2Gain(int nValue, BOOL bReverse)
{
	//if (nValue < GFLS_CAPTCHANNEL_MINCONTRAST) 
	//	nValue = GFLS_CAPTCHANNEL_MINCONTRAST;
	//else if (nValue > GFLS_CAPTCHANNEL_MAXCONTRAST)
	//	nValue = GFLS_CAPTCHANNEL_MAXCONTRAST;

	//return nValue;
	int		nMaxGain, nMinGain;
	double	fScale;
	 
	if(bReverse) nValue = 255 - nValue;
	nMaxGain = GFLS_CAPTCHANNEL_MAXCONTRAST;
	nMinGain = GFLS_CAPTCHANNEL_MINCONTRAST;
	fScale = (nMaxGain - nMinGain) / 255.0;
	 
	double	fContrast = fScale * nValue + nMinGain;
	int		nContrast = (int)(fContrast);
	 
	if ( nContrast <= nMinGain ) nContrast = nMinGain;
	if ( nContrast >= nMaxGain ) nContrast = nMaxGain;
	 
	return nContrast;
}

int		UTIL_ConvertGain2Contrast(int nValue, BOOL bReverse)
{
	//if (nValue < GFLS_CAPTCHANNEL_MINCONTRAST) 
	//	nValue = GFLS_CAPTCHANNEL_MINCONTRAST;
	//else if (nValue > GFLS_CAPTCHANNEL_MAXCONTRAST)
	//	nValue = GFLS_CAPTCHANNEL_MAXCONTRAST;

	//return nValue;
	int		nMaxGain, nMinGain;
	double	fScale;

	nMaxGain = GFLS_CAPTCHANNEL_MAXCONTRAST;
	nMinGain = GFLS_CAPTCHANNEL_MINCONTRAST;

	if (nValue <= nMinGain) nValue = nMinGain;
	if (nValue >= nMaxGain) nValue = nMaxGain;

	fScale = (nMaxGain - nMinGain) / 255.0;

	double	fContrast = (nValue - nMinGain) / fScale;
	int		nContrast = (int)(fContrast);
	if (bReverse) nContrast = 255 - nContrast;

	return nContrast;
}

int GetSetCaptContrast(int nChannel, int* pnContrast, UCHAR bGet)
{
	if (NULL == pnContrast) return GF_GALSCAPT_ERROR_PARAM;

	if (bGet)
	{
		//*pnContrast = g_stFprCapData.nContrast;
		*pnContrast = UTIL_ConvertGain2Contrast(g_stFprCapData.nContrast);
	}
	else if (*pnContrast != g_stFprCapData.nContrast)
	{
		g_stFprCapData.nContrast = (*pnContrast);

		int	nContrast = UTIL_ConvertContrast2Gain(g_stFprCapData.nContrast);
		if (!g_stFingerSensor.SetGain(nContrast))
			return GF_GALSCAPT_ERROR_PARAM;
		g_stFprCapData.bAdjustBright = 0;
		g_stFprCapData.bAdjustContrast = 1;
	}
	return GF_GALSCAPT_ERROR_SUCCESS;
}
#ifdef UNSTD_BLACK_BALANCE
int GetSetCaptBlackBalance(int nChannel, int* pnBlackBalance, UCHAR bGet)
{
	if (NULL == pnBlackBalance) return GF_GALSCAPT_ERROR_PARAM;

	if (bGet)
	{
		*pnBlackBalance = g_stFprCapData.nBlackBalance;
		//*pnBlackBalance = UTIL_ConvertGain2Contrast(g_stFprCapData.nBlackBalance);
	}
	else if (*pnBlackBalance != g_stFprCapData.nBlackBalance)
	{
		g_stFprCapData.nBlackBalance = (*pnBlackBalance);

		//int	nBlackBalance = UTIL_ConvertContrast2Gain(g_stFprCapData.nBlackBalance);
		int	nBlackBalance = g_stFprCapData.nBlackBalance;
		if (!g_stFingerSensor.SetLed(nBlackBalance))
			return GF_GALSCAPT_ERROR_PARAM;
		//g_stFprCapData.bAdjustBright = 0;
		//g_stFprCapData.bAdjustContrast = 1;
	}
	return GF_GALSCAPT_ERROR_SUCCESS;
}
#endif
int  GetCaptMaxImageSize(int nChannel, int *pnWidth, int* pnHeight)
{
	if (!pnWidth || !pnHeight) return GF_GALSCAPT_ERROR_PARAM;

	*pnWidth = g_stFprCapData.nCaptWidth;
	*pnHeight = g_stFprCapData.nCaptHeight;

	return GF_GALSCAPT_ERROR_SUCCESS;
}

int		GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight)
{
	if (!pnOriginX || !pnOriginY || !pnWidth || !pnHeight) return GF_GALSCAPT_ERROR_PARAM;

	*pnOriginX = g_stFprCapData.nOriginX;
	*pnOriginY = g_stFprCapData.nOriginY;
	*pnWidth = g_stFprCapData.nWidth;
	*pnHeight = g_stFprCapData.nHeight;

	return GF_GALSCAPT_ERROR_SUCCESS;
}

int		SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight)
{
	int nSrcWidth, nSrcHeight;
	if ((nWidth > g_stFprCapData.nCaptWidth) || (nHeight > g_stFprCapData.nCaptHeight)
		//		|| nOriginX != 0 || nOriginY !=0 ) 
		|| nWidth < GFLS_IDFP_WIDTH || nHeight < GFLS_IDFP_HEIGHT)
	{
		return GF_GALSCAPT_ERROR_PARAM;
	}
	nSrcHeight = g_stFprCapData.nHeight;
	nSrcWidth = g_stFprCapData.nWidth;

	g_stFprCapData.nWidth = nWidth;
	g_stFprCapData.nHeight = nHeight;
	if (SetOriginParam(nOriginX, nOriginY) != GF_GALSCAPT_ERROR_SUCCESS)
	{
		g_stFprCapData.nWidth = nSrcWidth;
		g_stFprCapData.nHeight = nSrcHeight;
		return GF_GALSCAPT_ERROR_PARAM;
	}

	return GF_GALSCAPT_ERROR_SUCCESS;
}

int		GetGAStdErrorInfo(int nErrorNo, char *pszErrorInfo, int ncbErrorInfo, int nGAStdType)
{
	if (!pszErrorInfo || (ncbErrorInfo < 1)) return GF_GALSCAPT_ERROR_PARAM;

	int		ncbLength = ncbErrorInfo, nret = GF_GALSCAPT_ERROR_SUCCESS;
	char	szErr[256];

	memset(pszErrorInfo, 0, ncbLength);
	memset(szErr, 0, sizeof(szErr));

	ncbLength--;
	switch (nErrorNo)
	{
	case GF_GALSCAPT_ERROR_PARAM:
		strncpy(szErr, "param error", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_MEMORY:
		strncpy(szErr, "malloc memory failed", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_NOTIMPL:
		strncpy(szErr, "not support", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_NODEVICE:
		strncpy(szErr, "not found device", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_NOTINIT:
		strncpy(szErr, "not init device", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_NOLICENSE:
		strncpy(szErr, "no license", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_INVALIDCHANNLE:
		strncpy(szErr, "invalid channle", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_INVALIDCAPTWIN:
		strncpy(szErr, "invalid capture window", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_BADQUALITY:
		strncpy(szErr, "bad quality", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_SUCCESS:
		strncpy(szErr, "success", ncbLength);
		break;

	case GF_GALSCAPT_ERROR_OTHERERR:
		strncpy(szErr, "other error", ncbLength);
		break;

	default:
		strncpy(szErr, "false error num", ncbLength);
		nret = GF_GALSCAPT_ERROR_INVALIDENO;
		break;
	}

	mbstowcs((wchar_t*)pszErrorInfo, szErr, ncbErrorInfo);

	return nret;
}


/*
int		UTIL_ReMallocBuffer(UCHAR **ppBuffer, int *pnCurBuffer, int nWantBuffer)
{
if ( !ppBuffer || !pnCurBuffer ) return -1;

int		nCurBuffer = *pnCurBuffer;

if ( *ppBuffer && (nWantBuffer <= nCurBuffer) ) return 1;

int		nAlign = 1024;
UCHAR	*pDataBuffer;

nWantBuffer = (nWantBuffer + nAlign - 1) / nAlign * nAlign;
pDataBuffer = (UCHAR*)malloc(nWantBuffer);
if ( !pDataBuffer ) return -1;
memset(pDataBuffer, 0, nWantBuffer);

if ( *ppBuffer ) free(*ppBuffer);
*ppBuffer = pDataBuffer;
*pnCurBuffer = nWantBuffer;

return 1;
}


void	UTIL_DeInitHelpData(UTIL_HELPDATA *pstHelpData)
{
if ( !pstHelpData || !pstHelpData->bHelpDataInit ) return;

UTIL_DeInitHelpData(pstHelpData);
pstHelpData->bHelpDataInit = 0;
}

int		UTIL_InitHelpData(UTIL_HELPDATA *pstHelpData, int nWidth, int nHeight)
{
if ( !pstHelpData || ( nWidth < 1 ) || ( nHeight < 1 ) ) return -1;
if ( pstHelpData->bHelpDataInit )
{
if ( ( pstHelpData->nWidth != nWidth ) || ( pstHelpData->nHeight != nHeight ) ) UTIL_DeInitHelpData(pstHelpData);
else return 1;
}

pstHelpData->nWidth = nWidth;
pstHelpData->nHeight = nHeight;

UCHAR	*pBuffer;
int		retval = -1;

pstHelpData->nFrameBuffer = nWidth * nHeight + nWidth * 2;
pBuffer = (UCHAR*)malloc(pstHelpData->nFrameBuffer);
if ( !pBuffer ) goto Finish_Exit;
memset(pBuffer, 0, pstHelpData->nFrameBuffer);
pstHelpData->pCaptureData = pBuffer;

pBuffer = (UCHAR*)malloc(pstHelpData->nFrameBuffer);
if ( !pBuffer ) goto Finish_Exit;
memset(pBuffer, 0, pstHelpData->nFrameBuffer);
pstHelpData->pPreviewData = pBuffer;

pstHelpData->nDataBuffer = pstHelpData->nFrameBuffer * 2;
pBuffer = (UCHAR*)malloc(pstHelpData->nDataBuffer);
if ( !pBuffer ) goto Finish_Exit;
memset(pBuffer, 0, pstHelpData->nDataBuffer);
pstHelpData->pDataBuffer = pBuffer;

if ( UTIL_ReMallocBuffer(&pstHelpData->pOneLineData, &pstHelpData->nOneLineData, 1024) < 0 ) goto Finish_Exit;

if ( !pstHelpData->bCritInited )
{
InitializeCriticalSection(&pstHelpData->stCrit);
pstHelpData->bCritInited = 1;
}

UTIL_Init256BitmapInfo(&pstHelpData->st256BmpInfo, pstHelpData->nWidth, pstHelpData->nHeight);

pstHelpData->bHelpDataInit = 1;
retval = 1;

Finish_Exit:

if ( retval < 0 ) UTIL_DeInitHelpData(pstHelpData);

return retval;
}

void	UTIL_Init256BitmapInfo(UTIL_256BITMAPINFO *pst256BmpInfo, int nWidth, int nHeight)
{
BITMAPINFOHEADER	*pInfoHead = &pst256BmpInfo->bmiHeader;

pInfoHead->biSize		= sizeof(*pInfoHead);
pInfoHead->biWidth		= nWidth;
pInfoHead->biHeight		= -nHeight;
pInfoHead->biPlanes		= 1;
pInfoHead->biBitCount	= 8;
pInfoHead->biCompression = BI_RGB;
pInfoHead->biSizeImage	 = nWidth * nHeight;
pInfoHead->biXPelsPerMeter = 19700;
pInfoHead->biYPelsPerMeter = 19700;
pInfoHead->biClrUsed = 256;
pInfoHead->biClrImportant = 0;

int		i;
UCHAR	nrgb;
RGBQUAD	*prgbQuad = pst256BmpInfo->bmiColors;
for ( i = 0, nrgb = 0; i < 256; ++i, ++nrgb, ++prgbQuad )
{
prgbQuad->rgbBlue = prgbQuad->rgbGreen = prgbQuad->rgbRed = nrgb;
prgbQuad->rgbReserved = 0;
}
}
*/

void	UTIL_SaveToBmpFileFormat(UCHAR *pBmpFileData, int nWidth, int nHeight)
{
	BITMAPFILEHEADER	*pFileHead;
	BITMAPINFOHEADER	*pInfoHead;
	RGBQUAD	*prgbQuad;
	DWORD	bfSize, bfOffBits;
	int		i, nColors;
	UCHAR	nrgb;

	pFileHead = (BITMAPFILEHEADER*)pBmpFileData;
	bfOffBits = sizeof(BITMAPFILEHEADER);

	pInfoHead = (BITMAPINFOHEADER*)(pBmpFileData + bfOffBits);
	bfOffBits += sizeof(BITMAPINFOHEADER);

	prgbQuad = (RGBQUAD*)(pBmpFileData + bfOffBits);
	nColors = 256;
	bfOffBits += (sizeof(RGBQUAD)* nColors);

	bfSize = bfOffBits + nWidth * nHeight;

	//!< FileHead
	pFileHead->bfType = 0x4d42;	// == 'BM'
	pFileHead->bfSize = bfSize;
	pFileHead->bfReserved1 = 0;
	pFileHead->bfReserved2 = 0;
	pFileHead->bfOffBits = bfOffBits;

	//!< BitmapInfo Head
	pInfoHead->biSize = sizeof(*pInfoHead);
	pInfoHead->biWidth = nWidth;
	pInfoHead->biHeight = nHeight;
	pInfoHead->biPlanes = 1;
	pInfoHead->biBitCount = 8;
	pInfoHead->biCompression = BI_RGB;
	pInfoHead->biSizeImage = nWidth * nHeight;
	pInfoHead->biXPelsPerMeter = 19700;
	pInfoHead->biYPelsPerMeter = 19700;
	pInfoHead->biClrUsed = nColors;
	pInfoHead->biClrImportant = 0;

	//!< Color table
	for (i = 0, nrgb = 0; i < nColors; ++i, ++nrgb, ++prgbQuad)
	{
		prgbQuad->rgbBlue = prgbQuad->rgbGreen = prgbQuad->rgbRed = nrgb;
		prgbQuad->rgbReserved = 0;
	}
}


int	ga_strlen(CONST char *pszStr)
{
	int		n;
	if (pszStr == NULL) return 0;

	n = 0;
	while (*pszStr++) n++;
	return	n;
}

void GetModulePath(char *pszBufferFileName)
{
	char *pDelchar = NULL;
	char szFilePath[MAX_PATH + 1];

	memset(pszBufferFileName, 0, sizeof(pszBufferFileName));
	memset(szFilePath, 0, sizeof(szFilePath));

	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);

	pDelchar = strrchr(szFilePath, '\\');

	strncpy(pszBufferFileName, szFilePath, ga_strlen(szFilePath) - ga_strlen(pDelchar));
}

int	PARAMETER_GetValueArray(char *fname, char **sParaName, char(*sParaValue)[PARAVALUE_BUFFER], int nItem)
{
	FILE	*fp;
	int		i, j;
	char	ch[2048], buf[2048];
	int		n;
	char	*c;

	if ((fp = fopen(fname, "a+t")) == NULL)
	{
		if ((fp = fopen(fname, "rt")) == NULL)
		{
			return -1;
		}
	}

	for (n = 0; n<nItem; n++) sParaValue[n][0] = '\0';

	while (fgets(ch, sizeof(ch), fp) != NULL)
	{
		i = 0;
		while (ch[i] != '\0' && (ch[i] == ' ' || ch[i] == '\t')) i++;
		if (ch[i] == '\0' || ch[i] == '\n' || ch[i] == '#') continue;
		/* may be a parameter line */
		/* like this
		* ServerName = xu_chunguang aksdf skdf  # annotation
		* UnitName = Eastern Golden Fingers
		*
		* read parameter name
		*/
		j = 0;
		while (ch[i] != '\0') {
			if (ch[i] == ' ' || ch[i] == '\t' || ch[i] == '#' || ch[i] == '=' || ch[i] == '\n') break;
			buf[j] = ch[i];
			j++;
			i++;
		}
		if (ch[i] == '\0' || j <= 0) continue;
		buf[j] = '\0';
		for (n = 0; n<nItem; n++)
		{
			if (strcmp(sParaName[n], buf) == 0) break;
		}
		if (n == nItem) continue;
		c = sParaValue[n];
		while (ch[i] != '\0' && (ch[i] == ' ' || ch[i] == '\t')) i++;
		if (ch[i] == '\0' || ch[i] != '=' || ch[i] == '\n') continue;
		i++;
		while (ch[i] != '\0' && (ch[i] == ' ' || ch[i] == '\t')) i++;
		j = 0;
		while (ch[i] != '\0') {
			//if ( ch[i]==' ' || ch[i]=='\t' || ch[i]=='#' || ch[i]=='\n') break;
			if (ch[i] == '#' || ch[i] == '\n' || ch[i] == '\t') break;
			c[j] = ch[i];
			i++;	j++;
		}
		c[j] = '\0';
	}
	fclose(fp);
	return 1;
}

int	PARAMETER_GetValue(char *fname, char *sParaName, char *sParaValue)
{
	return	PARAMETER_GetValueArray(fname, &sParaName, (char(*)[PARAVALUE_BUFFER])sParaValue, 1);
}

int GflsInternalSetOriginParam()
{
	if (g_stFprCapData.nCaptMode > 0) return 0;
	int nOriginX, nOriginY;
	char szFileName[MAX_PATH + 8] = { 0 };
	char szXS[32] = { 0 };
	char szYS[32] = { 0 };
	char szCM[32] = { 0 };

#ifdef ID_BUILD_USEA1000
	char *sXOriginStart = "XS1K";
	char *sYOriginStart = "YS1K";
#endif

#ifdef  ID_BUILD_USEA2000
	char *sXOriginStart = "XS2K";
	char *sYOriginStart = "YS2K";
#endif

#ifdef ID_BUILD_USEA5000
	char *sXOriginStart = "XS5K";
	char *sYOriginStart = "YS5K";
#endif
	char *szCaptMode = "CAPTMODE";


	GetModulePath(szFileName);
	strcat_s(szFileName, "\\");
	strcat_s(szFileName, "ID_Finit.dat");

	if (PARAMETER_GetValue(szFileName, sXOriginStart, szXS) > 0 ||
		PARAMETER_GetValue(szFileName, sYOriginStart, szYS) > 0)
	{
		nOriginX = nOriginY = 0;

		nOriginX = atoi(szXS);
		nOriginY = atoi(szYS);

		SetOriginParam(nOriginX, nOriginY);
	}

	return 1;
}


void	GetCuttedImageData(UCHAR *pImageData, int nwSrc, int nhSrc, UCHAR *pCuttedImage, RECT *prtCut)
{
	int		xSrcOff, ySrcOff, xDesOff, yDesOff;
	int		nwDes, nhDes, nwCopy, nhCopy;
	RECT	rtIntersec = { 0 }, rtSrc;

	nwDes = prtCut->right - prtCut->left;
	nhDes = prtCut->bottom - prtCut->top;

	rtSrc.left = rtSrc.top = 0;
	rtSrc.right = nwSrc;
	rtSrc.bottom = nhSrc;

	::IntersectRect(&rtIntersec, &rtSrc, prtCut);
	nwCopy = rtIntersec.right - rtIntersec.left;
	nhCopy = rtIntersec.bottom - rtIntersec.top;

	xSrcOff = rtIntersec.left;
	ySrcOff = rtIntersec.top;
	xDesOff = rtIntersec.left - prtCut->left;
	yDesOff = rtIntersec.top - prtCut->top;

	UCHAR	*pbnSrc = pImageData + nwSrc * ySrcOff + xSrcOff;
	UCHAR	*pbnDes = pCuttedImage + nwDes * yDesOff + xDesOff;
	for (int i = 0; i < nhCopy; ++i, pbnSrc += nwSrc, pbnDes += nwDes)
	{
		memcpy(pbnDes, pbnSrc, nwCopy);
	}
}

void	GetRotatedCutImageData(UCHAR *pImageData, int nSrcWidth, int nSrcHeight,
	UCHAR *pRotatedCutData, UTIL19_REGIONRECT *pstRectifyInfo)
{
	double	fAngle = -(3.14159 * pstRectifyInfo->nAngle / 180.0);
	double	fcos, fsin;

	fcos = cos(fAngle);
	fsin = sin(fAngle);

	int		nDestWidth, nDestHeight, xOrigin, yOrigin;

	xOrigin = pstRectifyInfo->x;
	yOrigin = pstRectifyInfo->y;
	nDestWidth = pstRectifyInfo->nWidth;
	nDestHeight = pstRectifyInfo->nHeight;

	int		i, j, xInt, yInt, nSrcPos;
	double	x, y, fxOff, fyOff, xDelta, yDelta;
	UCHAR	c11, c12, c21, c22;

	fxOff = (double)xOrigin;	fyOff = (double)yOrigin;
	for (j = 0; j < nDestHeight; j++, fxOff -= fsin, fyOff += fcos)
	{
		x = fxOff;
		y = fyOff;
		for (i = 0; i < nDestWidth; i++, pRotatedCutData++, x += fcos, y += fsin)
		{
			xInt = (int)x;
			yInt = (int)y;

			if (xInt < 0 || yInt < 0 || (xInt + 1) >= nSrcWidth || (yInt + 1) >= nSrcHeight)
			{
				continue;
			}

			xDelta = x - xInt;	yDelta = y - yInt;

			nSrcPos = yInt * nSrcWidth + xInt;
			c11 = pImageData[nSrcPos];	c12 = pImageData[nSrcPos + 1];
			nSrcPos += nSrcWidth;
			c21 = pImageData[nSrcPos];	c22 = pImageData[nSrcPos + 1];

			*pRotatedCutData = (UCHAR)((1.0 - yDelta) * ((1.0 - xDelta) * c11 + xDelta * c12) + yDelta * ((1.0 - xDelta) * c21 + xDelta * c22));
		}
	}
}
int		GetRectifyImageData(UCHAR *pCaptureData, int nwCapture, int nhCapture,
	UCHAR *pRectifyData, UTIL19_REGIONRECT *pstRectifyInfo)
{
	if (!pCaptureData || !pRectifyData || !pstRectifyInfo) return -1;

	memset(pRectifyData, 255, pstRectifyInfo->nWidth * pstRectifyInfo->nHeight);

	if (pstRectifyInfo->nAngle == 0)
	{
		RECT	rtCutted = { 0 };

		rtCutted.left = pstRectifyInfo->x;
		rtCutted.right = rtCutted.left + pstRectifyInfo->nWidth;
		rtCutted.top = pstRectifyInfo->y;
		rtCutted.bottom = rtCutted.top + pstRectifyInfo->nHeight;

		GetCuttedImageData(pCaptureData, nwCapture, nhCapture, pRectifyData, &rtCutted);
	}
	else GetRotatedCutImageData(pCaptureData, nwCapture, nhCapture, pRectifyData, pstRectifyInfo);

	return 1;
}


int		GetImageRectifyInfo(UCHAR *pCaptureData, int nwCapture, int nhCapture, UTIL19_REGIONRECT *pstRectifyInfo)
{
	if (!pCaptureData || !pstRectifyInfo) return -1;

	UTIL19_REGIONRECT	stRegion = { 0 };
	int		retval;

	pstRectifyInfo->beExist = 1;
	pstRectifyInfo->nAngle = 0;
	pstRectifyInfo->x = (nwCapture - pstRectifyInfo->nWidth) / 2;
	pstRectifyInfo->y = (nhCapture - pstRectifyInfo->nHeight) / 2;

	stRegion.nWidth = pstRectifyInfo->nWidth;
	stRegion.nHeight = pstRectifyInfo->nHeight;
	if ((pstRectifyInfo->nObjectType >= 21) && (pstRectifyInfo->nObjectType <= 30))
	{
		stRegion.nObjectType = pstRectifyInfo->nObjectType;
	}
	else stRegion.nObjectType = -1;

	retval = PlainImage_Splite(pCaptureData, nwCapture, nhCapture, &stRegion);
	if (retval >= -1)
	{
		if ((retval > -1) || (stRegion.x != 0) || (stRegion.y != 0))
		{
			if (stRegion.nWidth <= pstRectifyInfo->nWidth) pstRectifyInfo->x = stRegion.y;
			else pstRectifyInfo->x = stRegion.y + (stRegion.nWidth - pstRectifyInfo->nWidth) / 2;
			if (stRegion.nHeight <= pstRectifyInfo->nHeight) pstRectifyInfo->y = stRegion.x;
			else pstRectifyInfo->y = stRegion.x + (stRegion.nHeight - pstRectifyInfo->nHeight) / 2;
		}
		if (retval > -1) pstRectifyInfo->nAngle = stRegion.nAngle;
	}

	retval = 1;

	return retval;
}

int GetRectifyInfo(UCHAR *pDataBuffer, int nWidth, int nHeight, UTIL19_REGIONRECT *pstRegion)
{
	if (NULL == pstRegion) return 0;
	memset(pstRegion, 0, sizeof(UTIL19_REGIONRECT));
	if (g_stFprCapData.DeviceParams.nCamType == CAM_A5000)
	{
		pstRegion->nWidth = GFLS_CAPT_CUTWIDTH;
		pstRegion->nHeight = GFLS_CAPT_CUTHEIGHT;
		pstRegion->nObjectType = 23;
	}
	else if (g_stFprCapData.DeviceParams.nCamType == CAM_A3000)
	{
		pstRegion->nWidth = GFLS_CAPT_CUTWIDTH;
		pstRegion->nHeight = GFLS_CAPT_CUTHEIGHT;
		pstRegion->nObjectType = 23;
	}
	else if (g_stFprCapData.DeviceParams.nCamType == CAM_A2000)
	{
		pstRegion->nWidth = GFLS_CAPT_CUTWIDTH;
		pstRegion->nHeight = GFLS_CAPT_CUTHEIGHT;
		pstRegion->nObjectType = 23;
	}
	else return 0;

	int		retval;

	retval = GetImageRectifyInfo(pDataBuffer, nWidth, nHeight, pstRegion);

	return retval;
}


int	SaveDataToBitmapFile(const char* szFileName, UCHAR *ImgData, int nWidth, int nHeight, int nBits)
{
	int		nbits, nw, nh, nColors;	//, btopDown;

	nbits = nBits;
	nw = nWidth;
	nh = abs(nHeight);

	if (nHeight < 0)
	{
		nHeight = nh;
	}

	WORD  bfType = 0x4d42;	// == 'BM'
	DWORD bfSize, bfReserved, bfOffBits, dwWrites;
	BITMAPINFOHEADER BitmapHead = { 0 };
	BITMAPINFOHEADER *pHead = &BitmapHead;
	pHead->biSize = sizeof(BITMAPINFOHEADER);
	pHead->biPlanes = 1;
	pHead->biCompression = BI_RGB;
	pHead->biBitCount = 8;
	pHead->biWidth = nWidth;
	pHead->biHeight = nHeight;
	pHead->biSizeImage = nbits;
	pHead->biXPelsPerMeter = 20000;
	pHead->biYPelsPerMeter = 20000;

	FILE  *fp;
	int nActWidth, nVirtWidth;
	UCHAR *pImgBuf, bnDummy[4] = { 0 };


	nColors = 256;

	nActWidth = (nw * nbits + 7) / 8;
	nVirtWidth = (nActWidth + 3) / 4 * 4;

	bfOffBits = 14 + 40 + sizeof(RGBQUAD)* nColors;
	bfSize = bfOffBits + nVirtWidth * nh;
	bfReserved = 0;

	dwWrites = 0;

	fp = fopen(szFileName, "wb");
	if (NULL == fp)
		return 0;

	// BITMAPFILEHEADER
	fwrite(&bfType, sizeof(WORD), 1, fp);
	fwrite(&bfSize, sizeof(DWORD), 1, fp);
	fwrite(&bfReserved, sizeof(DWORD), 1, fp);
	fwrite(&bfOffBits, sizeof(DWORD), 1, fp);
	dwWrites += 14;

	// BITMAPINFOHEADER
	fwrite(pHead, sizeof(BITMAPINFOHEADER), 1, fp);
	dwWrites += 40;

	// RGBQUAD
	RGBQUAD bmiColors[256] = { 0 };
	UCHAR nValue = 0;

	for (int i = 0; i < 255; i++)
	{
		nValue = (UCHAR)i;
		bmiColors[i].rgbBlue = nValue;
		bmiColors[i].rgbGreen = nValue;
		bmiColors[i].rgbRed = nValue;
	}

	if (nColors > 0)
	{
		fwrite(bmiColors, sizeof(RGBQUAD), nColors, fp);
		dwWrites += sizeof(RGBQUAD)* nColors;
	}

	for (int row = 0; row < nh; row++)
	{
		pImgBuf = ImgData + (nh - 1 - row) * nActWidth;

		fwrite(pImgBuf, 1, nActWidth, fp);
		if (nVirtWidth != nActWidth)
			fwrite(bnDummy, 1, nVirtWidth - nActWidth, fp);
	}
	fclose(fp);

	return 1;
}
