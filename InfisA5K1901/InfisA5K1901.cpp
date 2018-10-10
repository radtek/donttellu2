#include "InfisLiveScanHelper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

CFingerSensor g_stFingerSensor;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif



GF_GALSCAPT_API		LIVESCAN_Init()
{
	return InitialCaptureData();
}

GF_GALSCAPT_API		LIVESCAN_Close()
{
	return	UnInitCaptureData();
}

GF_GALSCAPT_API		LIVESCAN_GetChannelCount()
{
	// 	if ( g_stFprCapData.bInited ) 
	// 		return g_stFprCapData.nMaxChannel;
	//change for guizhou ID_CARD.
	int nDevice = g_stFingerSensor.GetCameraCount();
	if (nDevice > 0)
	{
		return nDevice;
	}

	return GF_GALSCAPT_ERROR_NOTINIT;
}


inline	int		UTIL_IsValidChannel(int nChannel)
{
	if (!g_stFprCapData.bInited) return 0;

	if (nChannel < 0) return 0;
	if (nChannel >= g_stFprCapData.nMaxChannel) return 0;
	return 1;
}

GF_GALSCAPT_API		LIVESCAN_SetBright(int nChannel, int nBright)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return GetSetCaptBright(nChannel, &nBright, 0);
}

GF_GALSCAPT_API		LIVESCAN_SetContrast(int nChannel, int nContrast)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return GetSetCaptContrast(nChannel, &nContrast, 0);
}

GF_GALSCAPT_API		LIVESCAN_GetBright(int nChannel, int* pnBright)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return GetSetCaptBright(nChannel, pnBright, 1);
}


//非标准接口,黑平衡。为四联指设备特加。
GF_GALSCAPT_API		LIVESCAN_SetBlackBalance(int nChannel, int nBlackBalance)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;
#ifdef UNSTD_BLACK_BALANCE
	return GetSetCaptBlackBalance(nChannel, &nBlackBalance, 0);
#else
	return GF_GALSCAPT_ERROR_NOTIMPL;
#endif
}
//非标准接口,黑平衡。为四联指设备特加。
GF_GALSCAPT_API		LIVESCAN_GetBlackBalance(int nChannel, int* pnBlackBalance)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

#ifdef UNSTD_BLACK_BALANCE
	return GetSetCaptBlackBalance(nChannel, pnBlackBalance, 1);
#else
	return GF_GALSCAPT_ERROR_NOTIMPL;
#endif
}

GF_GALSCAPT_API		LIVESCAN_GetContrast(int nChannel, int* pnContrast)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return GetSetCaptContrast(nChannel, pnContrast, 1);
}

GF_GALSCAPT_API		LIVESCAN_GetMaxImageSize(int nChannel, int* pnWidth, int* pnHeight)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return GetCaptMaxImageSize(nChannel, pnWidth, pnHeight);
}

GF_GALSCAPT_API		LIVESCAN_GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return GetCaptWindow(nChannel, pnOriginX, pnOriginY, pnWidth, pnHeight);
}

GF_GALSCAPT_API		LIVESCAN_SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return SetCaptWindow(nChannel, nOriginX, nOriginY, nWidth, nHeight);
}

GF_GALSCAPT_API		LIVESCAN_Setup()
{
	return GF_GALSCAPT_ERROR_NOTIMPL;
}

GF_GALSCAPT_API		LIVESCAN_GetErrorInfo(int nErrorNo, char pszErrorInfo[256])
{
	return GetGAStdErrorInfo(nErrorNo, pszErrorInfo, 256, GASTD_IDFP);
}

GF_GALSCAPT_API		LIVESCAN_BeginCapture(int nChannel)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	GflsInternalSetOriginParam();

	return GF_GALSCAPT_ERROR_SUCCESS;
}

void	UTIL_ProcessImageFor1Suo(UCHAR *pbnCaptured)
{
	if ((g_stFprCapData.nBright >= 25) && (g_stFprCapData.nBright <= 230) &&
		(g_stFprCapData.nContrast >= 25) && (g_stFprCapData.nContrast <= 230))
	{
		return;
	}

	int		nMiddle = 128;
	double	fBright = (g_stFprCapData.nBright + 1 - nMiddle) / (double)nMiddle;
	double	fContrast = (g_stFprCapData.nContrast + 1 - nMiddle) / (double)nMiddle;

	double	K = tan((45 + 44 * fContrast) / 180.0 * 3.14159);

	double	fxOffset = 128 * (1 + fBright) - (128 * (1 - fBright)) * K;

	int		i, nGray, nLength;
	double	fGray;

	nLength = g_stFprCapData.nWidth * g_stFprCapData.nHeight;

	//UCHAR* pTmp = pbnCaptured;
	//int nAverageGray = 0;
	//for (i = 0; i < nLength; ++ i, ++ pTmp)  {
	//	nAverageGray += *pTmp;
	//}
	////assert(nLength != 0);
	//nAverageGray /= nLength;
	//pTmp = pbnCaptured;
	//for (i = 0; i < nLength; ++ i, ++ pTmp)  {
	//	int nVar = *pTmp - nAverageGray;
	//	nVar = nVar * g_stFprCapData.nContrast / 128;
	//	nVar += g_stFprCapData.nBright;
	//	if (nVar < 0) nVar = 0;
	//	if (nVar > 255) nVar = 255;
	//	*pTmp = (UCHAR)nVar;
	//}


	for (i = 0; i < nLength; ++i, ++pbnCaptured)
	{
		nGray = *pbnCaptured;
		fGray = nGray * K + fxOffset;
		if (fGray < 0) nGray = 0;
		else if (fGray > 255) nGray = 255;
		else nGray = (UCHAR)((int)fGray);
		(*pbnCaptured) = (UCHAR)nGray;
	}
}

int		UTIL_GetOneRawData(UCHAR *pbnCaptured, int bBottomUp)
{
	int nCnt = 0;
	while (!g_stFingerSensor.ReadImage(g_stFprCapData.pDataBuffer))
	{
		++nCnt;
		if (nCnt == 5)
		{
			return GF_GALSCAPT_ERROR_OTHERERR;
		}
	}
	int		nXOffset, nYOffset;
	UCHAR	*pbnSrc, *pbnDes, *pbnCut;
	char szFileName[1024] = { 0 };

	//nOffset = (g_stFprCapData.nCaptWidth - g_stFprCapData.nWidth) / 2;	//水平居中
	//pbnSrc = g_stFprCapData.pDataBuffer + nOffset;

	//nOffset = (g_stFprCapData.nCaptHeight - g_stFprCapData.nHeight) / 3 * 2;	//垂直，上面2/3, 下面1/3
	//pbnSrc += (nOffset * g_stFprCapData.nCaptWidth);

	pbnSrc = g_stFprCapData.pDataBuffer + g_stFprCapData.nOriginX + (g_stFprCapData.nOriginY * g_stFprCapData.nCaptWidth);

	int		nDesLine;

	if (bBottomUp)
	{
		pbnDes = pbnCaptured + (g_stFprCapData.nHeight - 1) * g_stFprCapData.nWidth;
		nDesLine = -g_stFprCapData.nWidth;
	}
	else
	{
		pbnDes = pbnCaptured;
		nDesLine = g_stFprCapData.nWidth;
	}

	if (GFLS_CAPTMODE_SPLITE == g_stFprCapData.nCaptMode && g_stFprCapData.DeviceParams.nCamType == CAM_A5000)
	{
		pbnSrc = g_stFprCapData.pDataBuffer;

		UTIL19_REGIONRECT stRegion = { 0 };
		GetRectifyInfo(pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, &stRegion);
		GetRectifyImageData(pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, g_stFprCapData.pCutBuffer, &stRegion);

		nXOffset = (stRegion.nWidth - g_stFprCapData.nWidth) / 2;
		nYOffset = (stRegion.nHeight - g_stFprCapData.nHeight) / 2;
		pbnCut = g_stFprCapData.pCutBuffer + nXOffset + (nYOffset*stRegion.nWidth);
		for (int i = 0; i < g_stFprCapData.nHeight; ++i, pbnCut += stRegion.nWidth, pbnDes += g_stFprCapData.nWidth)
		{
			memcpy(pbnDes, pbnCut, g_stFprCapData.nWidth);
		}

#ifdef _DEBUG
		sprintf(szFileName, "d:\\temp\\src_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, 8);

		sprintf(szFileName, "d:\\temp\\cut_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, g_stFprCapData.pCutBuffer, stRegion.nWidth, stRegion.nHeight, 8);

		sprintf(szFileName, "d:\\temp\\out_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, pbnCaptured, g_stFprCapData.nWidth, g_stFprCapData.nHeight, 8);
#endif
	}
	else if (GFLS_CAPTMODE_SPLITE == g_stFprCapData.nCaptMode && g_stFprCapData.DeviceParams.nCamType == CAM_A3000)
	{
		pbnSrc = g_stFprCapData.pDataBuffer;

		UTIL19_REGIONRECT stRegion = { 0 };
		GetRectifyInfo(pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, &stRegion);
		GetRectifyImageData(pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, g_stFprCapData.pCutBuffer, &stRegion);

		nXOffset = (stRegion.nWidth - g_stFprCapData.nWidth) / 2;
		nYOffset = (stRegion.nHeight - g_stFprCapData.nHeight) / 2;
		pbnCut = g_stFprCapData.pCutBuffer + nXOffset + (nYOffset*stRegion.nWidth);
		for (int i = 0; i < g_stFprCapData.nHeight; ++i, pbnCut += stRegion.nWidth, pbnDes += g_stFprCapData.nWidth)
		{
			memcpy(pbnDes, pbnCut, g_stFprCapData.nWidth);
		}

#ifdef _DEBUG
		sprintf(szFileName, "d:\\temp\\src_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, 8);

		sprintf(szFileName, "d:\\temp\\cut_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, g_stFprCapData.pCutBuffer, stRegion.nWidth, stRegion.nHeight, 8);

		sprintf(szFileName, "d:\\temp\\out_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, pbnCaptured, g_stFprCapData.nWidth, g_stFprCapData.nHeight, 8);
#endif
	}
	else if (GFLS_CAPTMODE_SPLITE == g_stFprCapData.nCaptMode && g_stFprCapData.DeviceParams.nCamType == CAM_A2000
		&& GFLS_CAPT_CUTWIDTH == g_stFprCapData.nWidth && GFLS_CAPT_CUTHEIGHT == g_stFprCapData.nHeight)
	{
		pbnSrc = g_stFprCapData.pDataBuffer;

		UTIL19_REGIONRECT stRegion = { 0 };
		GetRectifyInfo(pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, &stRegion);
		GetRectifyImageData(pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, g_stFprCapData.pCutBuffer, &stRegion);
		memcpy(pbnDes, g_stFprCapData.pCutBuffer, g_stFprCapData.nWidth*g_stFprCapData.nHeight);

#ifdef _DEBUG
		sprintf(szFileName, "d:\\temp\\src_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, pbnSrc, g_stFprCapData.nCaptWidth, g_stFprCapData.nCaptHeight, 8);

		sprintf(szFileName, "d:\\temp\\out_%d.bmp", g_stFprCapData.nNum);
		SaveDataToBitmapFile(szFileName, pbnCaptured, g_stFprCapData.nWidth, g_stFprCapData.nHeight, 8);
#endif

	}
	else
	{
		for (int i = 0; i < g_stFprCapData.nHeight; ++i, pbnSrc += g_stFprCapData.nCaptWidth, pbnDes += nDesLine)
		{
			memcpy(pbnDes, pbnSrc, g_stFprCapData.nWidth);
		}
	}
#ifdef _DEBUG
	g_stFprCapData.nNum++;
#endif
	//!< 图像处理：一所测试的时候好像需要根据亮度、对比度进行图像处理
	//UTIL_ProcessImageFor1Suo(pbnCaptured);

	return GF_GALSCAPT_ERROR_SUCCESS;
}


GF_GALSCAPT_API		LIVESCAN_GetFPRawData(int nChannel, unsigned char* pRawData)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;
	if (!pRawData) return GF_GALSCAPT_ERROR_PARAM;

	return UTIL_GetOneRawData(pRawData, /*g_stFprCapData.nDispDataBuffer*/0);
}

GF_GALSCAPT_API		LIVESCAN_EndCapture(int nChannel)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;

	return GF_GALSCAPT_ERROR_SUCCESS;
}

GF_GALSCAPT_API		LIVESCAN_IsSupportPreview(int nChannel)
{
	return 0;
}

GF_GALSCAPT_API	 LIVESCAN_GetPreviewImageSize(int nChannel, int *pnWidth, int *pnHeight)
{
	return GF_GALSCAPT_ERROR_NOTIMPL;
}

GF_GALSCAPT_API	LIVESCAN_GetPreviewData(int nChannel, unsigned char *pRawData)
{
	return GF_GALSCAPT_ERROR_NOTIMPL;
}

GF_GALSCAPT_API		LIVESCAN_IsSupportSetup()
{
	return 0;
}

GF_GALSCAPT_API		LIVESCAN_GetVersion()
{
	return GFLS_VERSION;
}

GF_GALSCAPT_API		LIVESCAN_GetDesc(char pszDesc[1024])
{

	if (!pszDesc) return GF_GALSCAPT_ERROR_PARAM;

	int nOff = 1;
	unsigned int nSerialNo = 0;
	if (GetSerialNo(nSerialNo) != 1) return GF_GALSCAPT_ERROR_NODEVICE;
	//if(OpenSerialNo(nSerialNo, nWantedSerialNo) != 1) return GF_GALSCAPT_ERROR_NODEVICE;

	memset(pszDesc, 0, 1024);
	//pszDesc[0] = '0xFF';
	pszDesc[0] = GFLS_CAPT_CARDDESCFIRST;
	//strncpy(pszDesc+1, GFLS_CAPT_CARDDESC, 1022);
	//mbstowcs((wchar_t*)(pszDesc+1), GFLS_CAPT_CARDDESCFIRST, strlen(GFLS_CAPT_CARDDESCFIRST));

	//memcpy(pszDesc+1, GFLS_CAPT_CARDDESC, wcslen(GFLS_CAPT_CARDDESC)*2);
	memcpy(pszDesc + nOff, GFLS_CAPT_DESCSTART, wcslen(GFLS_CAPT_DESCSTART) * 2);
	nOff += wcslen(GFLS_CAPT_DESCSTART) * 2;
	//srand((int)GetTickCount());
	//for (int i = 0; i < 7; ++i)
	//{
	//	*(pszDesc+nOff) = rand()%10 + 48;
	//	nOff += 2;
	//}
	nOff += swprintf((wchar_t*)(pszDesc + nOff), L"%011d", nSerialNo);
	nOff = 41;
	memcpy(pszDesc + nOff, GFLS_CAPT_DESCEND, wcslen(GFLS_CAPT_DESCEND) * 2);

	return GF_GALSCAPT_ERROR_SUCCESS;
}
//不透接口
GF_GALSCAPT_API		LIVESCAN_SetSerialNo(unsigned int nSerialNo)
{
	int nRet = 0;
	int bInited = g_stFprCapData.bInited;
	InitialCaptureData();
#if 0	//未实现
	if (g_stFingerSensor.SetSerialNo(nSerialNo)) nRet = 1;
#endif
	if (!bInited)
	{
		UnInitCaptureData();
	}
	return nRet;
}

GF_GALSCAPT_API		LIVESCAN_GetSerialNo(unsigned int& nSerialNo)
{
	int nRet = 0;
	int bInited = g_stFprCapData.bInited;
	InitialCaptureData();
#if 0	//未实现
	if (g_stFingerSensor.GetSerialNo(nSerialNo)) nRet = 1;
#endif
	if (!bInited)
	{
		UnInitCaptureData();
	}
	return nRet;
}

GF_GALSCAPT_API		LIVESCAN_IsSupportCaptWindow(int nChannel)
{
	return GASTD_CAPTWND_NONE;
}

GF_GALSCAPT_API		LIVESCAN_GetFPBmpData(int nChannel, unsigned char* pBmpData)
{
	if (!UTIL_IsValidChannel(nChannel)) return GF_GALSCAPT_ERROR_NOTINIT;
	if (!pBmpData) return GF_GALSCAPT_ERROR_PARAM;

	int		retval, nOffSet;

	nOffSet = (int)(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)* 256); //1078
	retval = UTIL_GetOneRawData(pBmpData + nOffSet, 1);
	if (retval != GF_GALSCAPT_ERROR_SUCCESS) return retval;

	UTIL_SaveToBmpFileFormat(pBmpData, g_stFprCapData.nWidth, g_stFprCapData.nHeight);

	return retval;
}

GF_GALSCAPT_API		LIVESCAN_SetBufferEmpty(unsigned char *pImageData, long imageLength)
{
	if (!pImageData || (imageLength < 1)) return GF_GALSCAPT_ERROR_PARAM;
	//if ( !g_stFprCapData.bInited ) return GF_GALSCAPT_ERROR_NOTINIT;

	memset(pImageData, 0, imageLength);
	return GF_GALSCAPT_ERROR_SUCCESS;
}

GF_GALSCAPT_API		LIVESCAN_InitEx(unsigned int nSerialNo)
{
	return InitialCaptureDataEx(nSerialNo);
}
