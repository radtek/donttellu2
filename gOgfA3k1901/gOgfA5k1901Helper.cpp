/**
 * @file   gOgfA3k1901Helper.cpp
 * @brief  封装王曙光A3000活体采集仪，符合一体化采集动态库标准接口
 * @author 北京东方金指科技有限公司
 * @date   2012/12/28 13:48
 * @note   Edit by guosong 
 * @copyright  版权所有(@) 1996-2012
 *             北京东方金指科技有限公司
 */

#include "stdafx.h"

extern	int		GYTHHELPER_Init256BitmapInfo(GYTH_256BITMAPINFO *pst256BmpInfo, int nWidth, int nHeight, int nBitsPerPixel);
extern	void	UTIL_ReleaseImageHelpData(GYTHIMAGE_HELPDATA *pstHelpData);
extern	int		GYTHHELPER_ReleaseImageHelpData(GYTHIMAGE_HELPDATA *pstHelpData);
extern	int		GYTHHELPER_InitImageHelpData(GYTHIMAGE_HELPDATA *pstHelpData);
extern	void	UTIL_GetImageDispRect(RECT &rtWnd, RECT &rtImage, int nImgWidth, int nImgHeight, UCHAR nFormat);
extern	void	GYTHHELPER_GetCurVideoWndRect(GYTHIMAGE_HELPDATA *pstHelpData, int nWidth, int nHeight);
extern	void	GYTHHELPER_DisplayVideoImage(GYTHIMAGE_HELPDATA *pstHelpData, int nVideoWidth, int nVideoHeight);


GOGFA3K_APPDATA g_stOgfA3KData = {0};


void FreeCaptDataAndClose()
{

	GYTHHELPER_ReleaseImageHelpData(&g_stOgfA3KData.stData);

	if ( g_stOgfA3KData.pDataBuffer ) free(g_stOgfA3KData.pDataBuffer);

	if ( g_stOgfA3KData.bOpen ) g_stFingerSensor.Close();

	memset(&g_stOgfA3KData, 0, sizeof(g_stOgfA3KData));
}


int	InitialCaptureData()
{
	if ( g_stOgfA3KData.bInited ) return GFS_DEVICE_ERROR_SUCCESS;

	int		i, nDevice, nCamType;
	char	szDevName[128];

	nDevice = g_stFingerSensor.GetCameraCount();
	if ( nDevice <= 0 ) return GFS_DEVICE_ERROR_NODEVICE;

	for ( i = 0; i < nDevice; ++i )
	{
		if ( g_stFingerSensor.GetCameraName(i, szDevName, nCamType) &&
			  nCamType == CAM_A3000 )
		{
			break;
		}			
	}
	if ( i == nDevice ) return GFS_DEVICE_ERROR_NODEVICE;

	if ( !g_stFingerSensor.Open(i) ) return GFS_DEVICE_ERROR_NOTINIT;

	g_stOgfA3KData.bOpen = 1;

	int		retval, nBright, nControst;

	if ( !g_stFingerSensor.GetDeviceParams(&g_stOgfA3KData.DeviceParams) ) 
	{
		retval = GF_GALSCAPT_ERROR_NOTINIT;
		goto Finish_Exit;
	}
	g_stOgfA3KData.DeviceParams.nCamType = nCamType;

	g_stOgfA3KData.nMaxChannel = 1;
	g_stOgfA3KData.nChannelIndex = 0;

	g_stOgfA3KData.nWidth   = A3000_WIDTH;
	g_stOgfA3KData.nHeight  = A3000_HEIGHT;

	g_stOgfA3KData.nCaptWidth  = A3000_WIDTH;
	g_stOgfA3KData.nCaptHeight = A3000_HEIGHT;
	
	if ( g_stOgfA3KData.DeviceParams.nMaxWindowWidth  < g_stOgfA3KData.nCaptWidth  ) 
	{
		g_stOgfA3KData.DeviceParams.nMaxWindowWidth = g_stOgfA3KData.nCaptWidth;
	}
	if ( g_stOgfA3KData.DeviceParams.nMaxWindowHeight < g_stOgfA3KData.nCaptHeight ) 
	{
		g_stOgfA3KData.DeviceParams.nMaxWindowHeight = g_stOgfA3KData.nCaptHeight;
	}

	g_stOgfA3KData.nBright   = OGFA3K_INITBRIGHT;
	g_stOgfA3KData.nContrast = OGFA3K_INITCONTRAST;


	//!< 图像相关性能数据
	g_stOgfA3KData.stData.stImageProp.ncbSize	  = sizeof(g_stOgfA3KData.stData.stImageProp);
	g_stOgfA3KData.stData.stImageProp.nMaxWidth  = A3000_WIDTH;
	g_stOgfA3KData.stData.stImageProp.nMaxHeight = A3000_HEIGHT;
	g_stOgfA3KData.stData.stImageProp.nBitsPerPixel = OGFA3K_BPP;
	g_stOgfA3KData.stData.stImageProp.nPixelsPerInch = OGFA3K_PPI;

	if ( (retval = GYTHHELPER_InitImageHelpData(&g_stOgfA3KData.stData)) < 0 )
	{
		goto Finish_Exit;
	}


	g_stOgfA3KData.pDataBuffer = (UCHAR*)malloc(g_stOgfA3KData.DeviceParams.nMaxWindowWidth * g_stOgfA3KData.DeviceParams.nMaxWindowHeight);
	if ( !g_stOgfA3KData.pDataBuffer )
	{
		retval = GF_GALSCAPT_ERROR_MEMORY;
		goto Finish_Exit;
	}
	
	g_stOgfA3KData.bActive = 1;

	//g_stFingerSensor.SetReverse(true);

	nBright = UTIL_ConvertBright(g_stOgfA3KData.nBright);
	nControst = UTIL_ConvertContrast(g_stOgfA3KData.nContrast);
	g_stFingerSensor.SetExposure(nBright);
	g_stFingerSensor.SetGain(nControst);

	g_stOgfA3KData.bInited = 1;

	retval = GF_GALSCAPT_ERROR_SUCCESS;

Finish_Exit:
	if ( retval != GF_GALSCAPT_ERROR_SUCCESS ) FreeCaptDataAndClose();
	return retval;
}

int		UnInitCaptureData()
{
	if ( g_stOgfA3KData.bInited ) FreeCaptDataAndClose();

	return GF_GALSCAPT_ERROR_SUCCESS;
}



int		UTIL_ConvertBright(int nValue)
{
	if ( nValue < 0 ) nValue = 0;
	else if ( nValue > 255 ) nValue = 255;
	nValue = 255 - nValue;

	int		nMaxExposure, nMinExposure;;
	double	fScale;

	nMaxExposure = g_stOgfA3KData.DeviceParams.nMaxExposure;	// GFLS_CAPTCHANNEL_MAXBRIGHT;
	nMinExposure = g_stOgfA3KData.DeviceParams.nMinExposure;	// GFLS_CAPTCHANNEL_MINBRIGHT;
	fScale = (nMaxExposure - nMinExposure) / 255.0;

	double	fBright = fScale * nValue + nMinExposure;
	int		nBright =(int)(fBright);	//!< [1, 511]

	if ( nBright <= nMinExposure) nBright = nMinExposure;
	if ( nBright >= nMaxExposure ) nBright = nMaxExposure;

	return nBright;
}


int		GetSetCaptBright(int nChannel, int* pnBright, UCHAR bGet)
{
	if ( NULL == pnBright ) return GF_GALSCAPT_ERROR_PARAM;

	int	nBright;

	if ( bGet ) *pnBright = g_stOgfA3KData.nBright;
	else if ( *pnBright != g_stOgfA3KData.nBright )
	{		
		nBright = UTIL_ConvertBright(*pnBright);

		if( !g_stFingerSensor.SetExposure(nBright) ) return GF_GALSCAPT_ERROR_PARAM;

		g_stOgfA3KData.nBright = (*pnBright);

	}

	return GF_GALSCAPT_ERROR_SUCCESS;
}

int		UTIL_ConvertContrast(int nValue)
{
	if ( nValue < 0 ) nValue = 0;
	else if ( nValue > 255 ) nValue = 255;

	int		nMaxGain, nMinGain;
	double	fScale;

	nMaxGain = g_stOgfA3KData.DeviceParams.nMaxGain;	// GFLS_CAPTCHANNEL_MAXCONTRAST;
	nMinGain = g_stOgfA3KData.DeviceParams.nMinGain;	// GFLS_CAPTCHANNEL_MINCONTRAST;
	fScale = (nMaxGain - nMinGain) / 255.0;

	double	fContrast = fScale * nValue + nMinGain;
	int		nContrast = (int)(fContrast);

	if ( nContrast <= nMinGain ) nContrast = nMinGain;
	if ( nContrast >= nMaxGain ) nContrast = nMaxGain;

	return nContrast;
}

int GetSetCaptContrast(int nChannel, int* pnContrast, UCHAR bGet)
{
	if ( NULL == pnContrast ) return GF_GALSCAPT_ERROR_PARAM;

	if ( bGet ) *pnContrast = g_stOgfA3KData.nContrast;
	else if ( *pnContrast != g_stOgfA3KData.nContrast )
	{
		int	nContrast = UTIL_ConvertContrast(g_stOgfA3KData.nContrast);

		if( !g_stFingerSensor.SetGain(nContrast)) return GF_GALSCAPT_ERROR_PARAM;

		g_stOgfA3KData.nContrast = (*pnContrast);

	}
	return GF_GALSCAPT_ERROR_SUCCESS;
}

int		GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight)
{
	if(!pnOriginX || !pnOriginY || !pnWidth || !pnHeight) return GF_GALSCAPT_ERROR_PARAM;

	*pnOriginX = 0;
	*pnOriginY = 0;
	*pnWidth   = g_stOgfA3KData.nWidth;
	*pnHeight  = g_stOgfA3KData.nHeight;

	return GF_GALSCAPT_ERROR_SUCCESS;
}

int		SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight)
{
	if ( ( nWidth != g_stOgfA3KData.nWidth ) || ( nHeight != g_stOgfA3KData.nHeight ) )
	{
		return GF_GALSCAPT_ERROR_PARAM;
	}

	return GF_GALSCAPT_ERROR_SUCCESS;
}


DWORD	WINAPI	OgfGA3K_THREAD_StartVideo(LPVOID /*pParam*/)
{
	g_stOgfA3KData.stData.bCloseVideo = 0;

	int		retval;

	while ( !g_stOgfA3KData.stData.bCloseVideo )
	{
		retval = UTIL_GetOneRawData(g_stOgfA3KData.stData.pVideoData);
		if ( retval != GFS_DEVICE_ERROR_SUCCESS ) continue;

		GYTHHELPER_DisplayVideoImage(&g_stOgfA3KData.stData, g_stOgfA3KData.nWidth, g_stOgfA3KData.nHeight);
	}

	return 0;
}


int		GYTHHELPER_Init256BitmapInfo(GYTH_256BITMAPINFO *pst256BmpInfo, int nWidth, int nHeight, int nBitsPerPixel)
{
	if (!pst256BmpInfo) return GFS_DEVICE_ERROR_PARAM;

	BITMAPINFOHEADER	*pInfoHead = &pst256BmpInfo->bmiHeader;

	pInfoHead->biSize = sizeof(*pInfoHead);
	pInfoHead->biWidth = nWidth;
	pInfoHead->biHeight = -nHeight;
	pInfoHead->biPlanes = 1;
	pInfoHead->biBitCount = (WORD)nBitsPerPixel;
	pInfoHead->biCompression = BI_RGB;
	pInfoHead->biSizeImage = (nWidth * nBitsPerPixel + 7) / 8 * nHeight;
	pInfoHead->biXPelsPerMeter = 19700;
	pInfoHead->biYPelsPerMeter = 19700;
	pInfoHead->biClrUsed = 256;
	pInfoHead->biClrImportant = 0;

	int		i;
	UCHAR	nrgb;
	RGBQUAD	*prgbQuad = pst256BmpInfo->bmiColors;
	for (i = 0, nrgb = 0; i < 256; ++i, ++nrgb, ++prgbQuad)
	{
		prgbQuad->rgbBlue = prgbQuad->rgbGreen = prgbQuad->rgbRed = nrgb;
		prgbQuad->rgbReserved = 0;
	}

	return GFS_DEVICE_ERROR_SUCCESS;
}

void	UTIL_ReleaseImageHelpData(GYTHIMAGE_HELPDATA *pstHelpData)
{
	//if ( pstHelpData->pDataBuffer ) free(pstHelpData->pDataBuffer);
	//if ( pstHelpData->pCaptureData ) free(pstHelpData->pCaptureData);
	if (pstHelpData->pVideoData) free(pstHelpData->pVideoData);

	//pstHelpData->pDataBuffer = NULL;
	//pstHelpData->pCaptureData = NULL;
	pstHelpData->pVideoData = NULL;

	pstHelpData->nDataBuffer = 0;
	//pstHelpData->nFrameBuffer = 0;

	if (pstHelpData->bCritInited)
	{
		DeleteCriticalSection(&pstHelpData->stCrit);
		pstHelpData->bCritInited = 0;
	}
}

int		GYTHHELPER_ReleaseImageHelpData(GYTHIMAGE_HELPDATA *pstHelpData)
{
	if (!pstHelpData || !pstHelpData->bHelpDataInit) return GFS_DEVICE_ERROR_SUCCESS;

	UTIL_ReleaseImageHelpData(pstHelpData);
	pstHelpData->bHelpDataInit = 0;
	return GFS_DEVICE_ERROR_SUCCESS;
}

int		GYTHHELPER_InitImageHelpData(GYTHIMAGE_HELPDATA *pstHelpData)
{
	if (!pstHelpData) return GFS_DEVICE_ERROR_PARAM;
	if ((pstHelpData->stImageProp.nMaxWidth < 1) || (pstHelpData->stImageProp.nMaxHeight < 1) ||
		(pstHelpData->stImageProp.nBitsPerPixel < 1))
	{
		return GFS_DEVICE_ERROR_NOTINIT;
	}

	if (pstHelpData->bHelpDataInit) return GFS_DEVICE_ERROR_SUCCESS;

	UCHAR	*pBuffer;
	int		nLine, retval;

	retval = GFS_DEVICE_ERROR_MEMORY;
	nLine = pstHelpData->stImageProp.nMaxWidth * pstHelpData->stImageProp.nBitsPerPixel / 8;
	nLine = (nLine + 3) / 4 * 4;

	if (pstHelpData->stImageProp.nImageForming == GFS_DEVICE_IMAGEFORM_LINEAR) pstHelpData->bNoVideo = 1;
	if (!pstHelpData->bNoVideo) pstHelpData->bNoCritical = 0;

	if (!pstHelpData->bNoVideo)
	{
		pstHelpData->nDataBuffer = (pstHelpData->stImageProp.nMaxHeight + 2) * nLine;
		pBuffer = (UCHAR*)malloc(pstHelpData->nDataBuffer);
		if (!pBuffer) goto Finish_Exit;
		pstHelpData->pVideoData = pBuffer;
	}

	if (!pstHelpData->bNoCritical && !pstHelpData->bCritInited)
	{
		InitializeCriticalSection(&pstHelpData->stCrit);
		pstHelpData->bCritInited = 1;
	}

	GYTHHELPER_Init256BitmapInfo(&pstHelpData->st256BmpInfo,
		pstHelpData->stImageProp.nMaxWidth, pstHelpData->stImageProp.nMaxHeight, pstHelpData->stImageProp.nBitsPerPixel);

	pstHelpData->bHelpDataInit = 1;
	retval = GFS_DEVICE_ERROR_SUCCESS;

Finish_Exit:
	if (retval < 0) UTIL_ReleaseImageHelpData(pstHelpData);
	return retval;
}

//!< 这里已经保证了nImgWidth、nImgHeight不会比rtWnd的Size大
void	UTIL_GetImageDispRect(RECT &rtWnd, RECT &rtImage, int nImgWidth, int nImgHeight, UCHAR nFormat)
{
	int		nLeft, nTop;

	memcpy(&rtImage, &rtWnd, sizeof(rtWnd));

	nLeft = rtImage.left;
	nTop = rtImage.top;
	switch (nFormat)
	{
	case YTHVIDEO_LT:
	default:
		break;
	case YTHVIDEO_CT:
		nLeft += ((rtWnd.right - rtWnd.left) - nImgWidth) / 2;
		break;
	case YTHVIDEO_RT:
		nLeft = rtWnd.right - nImgWidth;
		break;
	case YTHVIDEO_LC:
		nTop += ((rtWnd.bottom - rtWnd.top) - nImgHeight) / 2;
		break;
	case YTHVIDEO_CC:
		nLeft += ((rtWnd.right - rtWnd.left) - nImgWidth) / 2;
		nTop += ((rtWnd.bottom - rtWnd.top) - nImgHeight) / 2;
		break;
	case YTHVIDEO_RC:
		nLeft = rtWnd.right - nImgWidth;
		nTop += ((rtWnd.bottom - rtWnd.top) - nImgHeight) / 2;
		break;
	case YTHVIDEO_LB:
		nTop = rtWnd.bottom - nImgHeight;
		break;
	case YTHVIDEO_CB:
		nLeft += ((rtWnd.right - rtWnd.left) - nImgWidth) / 2;
		nTop = rtWnd.bottom - nImgHeight;
		break;
	case YTHVIDEO_RB:
		nLeft = rtWnd.right - nImgWidth;
		nTop = rtWnd.bottom - nImgHeight;
		break;
	}

	rtImage.left = nLeft;
	rtImage.right = rtImage.left + nImgWidth;
	rtImage.top = nTop;
	rtImage.bottom = rtImage.top + nImgHeight;
}

void	GYTHHELPER_GetCurVideoWndRect(GYTHIMAGE_HELPDATA *pstHelpData, int nWidth, int nHeight)
{
	if (!pstHelpData || !pstHelpData->bHelpDataInit || (nWidth < 1) || (nHeight < 1)) return;

	memset(&pstHelpData->rtImage, 0, sizeof(pstHelpData->rtImage));
	if (!pstHelpData->hWndVideo || !IsWindow(pstHelpData->hWndVideo)) return;

	RECT	rtWnd;

	::GetClientRect(pstHelpData->hWndVideo, &rtWnd);
	double	fx = (rtWnd.right - rtWnd.left) * 1.0 / nWidth;
	double	fy = (rtWnd.bottom - rtWnd.top) * 1.0 / nHeight;
	int		nwDisp, nhDisp, ntmpsize;	//, nxDeflate, nyDeflate;

	if (fx < fy)
	{
		nwDisp = (rtWnd.right - rtWnd.left) / 4 * 4;
		ntmpsize = (int)(nHeight * fx);
		nhDisp = ntmpsize / 4 * 4;
	}
	else
	{
		nhDisp = (rtWnd.bottom - rtWnd.top) / 4 * 4;
		ntmpsize = (int)(nWidth * fy);
		nwDisp = ntmpsize / 4 * 4;
	}
	UTIL_GetImageDispRect(rtWnd, pstHelpData->rtImage, nwDisp, nhDisp, pstHelpData->nDrawFormat);

	//nxDeflate = ((rtWnd.right - rtWnd.left) - nwDisp) / 2;
	//nyDeflate = ((rtWnd.bottom - rtWnd.top) - nhDisp ) / 2;
	//::InflateRect(&rtWnd, -nxDeflate, -nyDeflate);
	//memcpy(&pstHelpData->rtImage, &rtWnd, sizeof(rtWnd));
}


void	GYTHHELPER_DisplayVideoImage(GYTHIMAGE_HELPDATA *pstHelpData, int nVideoWidth, int nVideoHeight)
{
	if (!pstHelpData || !pstHelpData->bHelpDataInit || !pstHelpData->pVideoData) return;
	if (!IsWindow(pstHelpData->hWndVideo)) return;

	HDC		hDC = NULL;

	hDC = ::GetDC(pstHelpData->hWndVideo);
	if (NULL == hDC) return;

	RECT	rtClient;
	int		nWidth, nHeight;
	int		nImgWidth, nImgHeight;

	GYTHHELPER_GetCurVideoWndRect(pstHelpData, nVideoWidth, nVideoHeight);

	::GetClientRect(pstHelpData->hWndVideo, &rtClient);
	nWidth = (rtClient.right - rtClient.left) / 4 * 4;
	nHeight = (rtClient.bottom - rtClient.top);
	rtClient.right = nWidth;

	nImgWidth = pstHelpData->rtImage.right - pstHelpData->rtImage.left;
	nImgHeight = pstHelpData->rtImage.bottom - pstHelpData->rtImage.top;

	BITMAPINFO	*lpbmi = (BITMAPINFO*)&pstHelpData->st256BmpInfo;
	HDC		hdc;
	HBITMAP	hbmp, holdbmp;
	HBRUSH	hbrush;

	hbrush = ::CreateSolidBrush(RGB(255, 255, 255));
	hdc = ::CreateCompatibleDC(hDC);
	hbmp = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
	holdbmp = (HBITMAP)::SelectObject(hdc, hbmp);

	::SetBkMode(hdc, TRANSPARENT);
	::FillRect(hdc, &rtClient, hbrush);

	::SetStretchBltMode(hdc, COLORONCOLOR);
	::StretchDIBits(hdc,
		pstHelpData->rtImage.left, pstHelpData->rtImage.top, nImgWidth, nImgHeight,
		0, 0, nVideoWidth, nVideoHeight, pstHelpData->pVideoData, lpbmi, DIB_RGB_COLORS, SRCCOPY);

	::BitBlt(hDC, rtClient.left, rtClient.top, nWidth, nHeight, hdc, 0, 0, SRCCOPY);

	if (holdbmp) ::SelectObject(hdc, holdbmp);
	::DeleteObject(hbrush);
	::DeleteObject(hbmp);
	::DeleteDC(hdc);
	::ReleaseDC(pstHelpData->hWndVideo, hDC);
}