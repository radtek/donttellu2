/**
 * @file   gOgfA3k1901.cpp
 * @brief  封装王曙光A3000活体采集仪，符合一体化采集动态库标准接口
 * @author 北京东方金指科技有限公司
 * @date   2012/12/28 14:15
 * @note   Edit by guosong 
 * @copyright  版权所有(@) 1996-2012
 *             北京东方金指科技有限公司
 */

#include "stdafx.h"

CFingerSensor g_stFingerSensor;

extern void	GYTHHELPER_StopVideoThread(GYTHIMAGE_HELPDATA *pstHelpData);

extern int		GYTHHELPER_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

GFS_YTHCAPTURE_API	GFS_DEVICE_Init()
{
	return InitialCaptureData();
}

GFS_YTHCAPTURE_API	GFS_DEVICE_Close()
{
	return	UnInitCaptureData();
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetID(char pszDevID[32])
{
	if ( !pszDevID ) return GFS_DEVICE_ERROR_PARAM;

	sprintf(pszDevID, "%02d.%02d.%d", OGFA3K_PRODUCTID, APPLY_FINGER, OGFA3K_VERSION);

	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_IsSupportSetup()
{
	return 0;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_Setup()
{
	return GFS_DEVICE_ERROR_NOTIMPL;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetVersion()
{
	return	OGFA3K_VERSION;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetDesc(char pszDesc[1024])
{
	if ( !pszDesc ) return GFS_DEVICE_ERROR_PARAM;

	strcpy(pszDesc, OGFA3K_DEVICE_DESC);

	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetErrorInfo(int nErrorNo, char pszErrorInfo[256])
{
	return GYTHHELPER_GetErrorInfo(nErrorNo, pszErrorInfo);
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetChannelCount()
{
	if ( g_stOgfA3KData.bInited ) return g_stOgfA3KData.nMaxChannel;
	else return GFS_DEVICE_ERROR_NOTINIT;
}

inline	int		GA3K_CheckChannel(int nChannel)
{
	if ( !g_stOgfA3KData.bInited ) return GFS_DEVICE_ERROR_NOTINIT;

	if ( nChannel < 0 ) return GFS_DEVICE_ERROR_PARAM;
	if ( nChannel >= g_stOgfA3KData.nMaxChannel ) return GFS_DEVICE_ERROR_PARAM;

	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetImagePropInfo(int nChannel, GFS_DEVICE_IMAGEPROPSTRUCT *pstImageProp)
{
	int		retval;

	if ( !pstImageProp ) return GFS_DEVICE_ERROR_PARAM;

	retval = GA3K_CheckChannel(nChannel);
	if ( retval != GFS_DEVICE_ERROR_SUCCESS ) return retval;

	memcpy(pstImageProp, &g_stOgfA3KData.stData.stImageProp,sizeof(g_stOgfA3KData.stData.stImageProp));
	return GFS_DEVICE_ERROR_SUCCESS;
}


inline	int		UTIL_IsValidChannel(int nChannel)
{
	if ( !g_stOgfA3KData.bInited ) return 0;

	if ( nChannel < 0 ) return 0;
	if ( nChannel >= g_stOgfA3KData.nMaxChannel ) return 0;
	return 1;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_SetBright(int nChannel, int nBright)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return GetSetCaptBright(nChannel, &nBright, 0);
}

GFS_YTHCAPTURE_API	GFS_DEVICE_SetContrast(int nChannel, int nContrast)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;
	
	return GetSetCaptContrast(nChannel, &nContrast, 0);
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetBright(int nChannel, int *pnBright)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return GetSetCaptBright(nChannel, pnBright, 1);
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetContrast(int nChannel, int *pnContrast)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;
	
	return GetSetCaptContrast(nChannel,pnContrast, 1);
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetCaptWindow(int nChannel, 
	                                         int *pnOriginX, int *pnOriginY,
	                                         int *pnWidth, int *pnHeight)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return GetCaptWindow(nChannel, pnOriginX, pnOriginY, pnWidth, pnHeight);
	
}

GFS_YTHCAPTURE_API	GFS_DEVICE_IsSupportCaptWindow(int nChannel)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return 1;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetCaptWndType(int nChannel, int* pnCaptWndType)
{
	if ( !pnCaptWndType ) return GFS_DEVICE_ERROR_PARAM;

	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	*pnCaptWndType = 0;

	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_SetCaptWindow(int nChannel, 	int nOriginX, int nOriginY,
	                                   int nWidth, int nHeight)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return SetCaptWindow(nChannel, nOriginX, nOriginY, nWidth, nHeight);
}


GFS_YTHCAPTURE_API	GFS_DEVICE_StartAcquisition(int nChannel, int nOption)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	nOption = 0;
	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_BeginCapture(int nChannel)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return GFS_DEVICE_ERROR_SUCCESS;
}


int		UTIL_GetOneRawData(UCHAR *pbnCaptured)
{
	//if ( !g_stFingerSensor.ReadImage(g_stOgfA3KData.pDataBuffer) ) return GF_GALSCAPT_ERROR_UNKNOWN;

	int nCnt = 0;
	while ( !g_stFingerSensor.ReadImage(g_stOgfA3KData.pDataBuffer) )
	{
		++nCnt;
		if(nCnt == 3)
		{
			return GF_GALSCAPT_ERROR_UNKNOWN;
		}
	}

	UCHAR	*pbnSrc, *pbnDes;

	pbnSrc = g_stOgfA3KData.pDataBuffer;
	pbnDes = pbnCaptured;

	if ( (g_stOgfA3KData.nWidth  == g_stOgfA3KData.nCaptWidth) && 
		(g_stOgfA3KData.nHeight == g_stOgfA3KData.nCaptHeight) )
	{
		memcpy(pbnDes, pbnSrc, g_stOgfA3KData.nWidth * g_stOgfA3KData.nHeight);
		return GFS_DEVICE_ERROR_SUCCESS;
	}

	int		nxOff, nyOff;

	nxOff = (g_stOgfA3KData.nWidth - g_stOgfA3KData.nCaptWidth) / 2;	//!< 水平居中
	nyOff = (g_stOgfA3KData.nHeight - g_stOgfA3KData.nCaptHeight) /2;	//!< 垂直居中

	int		ncpWidth, ncpHeight;

	if ( g_stOgfA3KData.nWidth >= g_stOgfA3KData.nCaptWidth )
	{
		ncpWidth = g_stOgfA3KData.nCaptWidth;
		pbnDes += nxOff;
	}
	else
	{
		ncpWidth = g_stOgfA3KData.nWidth;
		pbnSrc -= nxOff;
	}

	if ( g_stOgfA3KData.nHeight >= g_stOgfA3KData.nCaptHeight )
	{
		ncpHeight = g_stOgfA3KData.nCaptHeight;
		pbnDes += (nyOff * g_stOgfA3KData.nWidth);
	}
	else
	{
		ncpHeight = g_stOgfA3KData.nHeight;
		pbnSrc -= (nyOff * g_stOgfA3KData.nCaptWidth);
	}

	for ( int i = 0; i < ncpHeight; ++i, pbnSrc += g_stOgfA3KData.nCaptWidth, pbnDes += g_stOgfA3KData.nWidth )
	{
		memcpy(pbnDes, pbnSrc, ncpWidth);
	}

	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetRawData(int nChannel, unsigned char *pRawData)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	if ( !pRawData ) return GFS_DEVICE_ERROR_PARAM;

	return UTIL_GetOneRawData(pRawData);
}

GFS_YTHCAPTURE_API	GFS_DEVICE_EndCapture(int nChannel)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return GFS_DEVICE_ERROR_SUCCESS;
}


GFS_YTHCAPTURE_API	GFS_DEVICE_StopAcquisition(int nChannel, int nOption)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	nOption = 0;

	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_IsSupportPreview()
{
	return 0;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetPreviewImageSize(int nChannel, int *pnWidth, int *pnHeight)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return GFS_DEVICE_ERROR_NOTIMPL;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_GetPreviewData(int nChannel, unsigned char *pPreviewData)
{
	if ( !UTIL_IsValidChannel(nChannel ) ) return GFS_DEVICE_ERROR_NOTINIT;

	return  GFS_DEVICE_ERROR_NOTIMPL;
}


GFS_YTHCAPTURE_API	GFS_DEVICE_SetVideoWindow(HWND hVideo)
{
	GYTHHELPER_StopVideoThread(&g_stOgfA3KData.stData);

	g_stOgfA3KData.stData.hWndVideo = hVideo;

	return GFS_DEVICE_ERROR_SUCCESS;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_StartVideo()
{

	if ( !g_stOgfA3KData.bInited ) return GFS_DEVICE_ERROR_NOTINIT;
	if ( !g_stOgfA3KData.stData.bCloseVideo ) return GFS_DEVICE_ERROR_SUCCESS;
	if ( !g_stOgfA3KData.stData.hWndVideo || !IsWindow(g_stOgfA3KData.stData.hWndVideo) ) return GFS_DEVICE_ERROR_INVALIDVIDEOWND;

	DWORD	dwThread = 0;
	g_stOgfA3KData.stData.hThreadVideo = CreateThread(NULL, 0, &OgfGA3K_THREAD_StartVideo, NULL, 0, &dwThread);
	
	return  GFS_DEVICE_ERROR_NOTIMPL;
}

GFS_YTHCAPTURE_API	GFS_DEVICE_StopVideo()
{	
	if ( !g_stOgfA3KData.bInited ) return GFS_DEVICE_ERROR_NOTINIT;

	GYTHHELPER_StopVideoThread(&g_stOgfA3KData.stData);

	return  GFS_DEVICE_ERROR_NOTIMPL;
}

void	GYTHHELPER_StopVideoThread(GYTHIMAGE_HELPDATA *pstHelpData)
{
	if (!pstHelpData) return;

	pstHelpData->bCloseVideo = 1;
	if (!pstHelpData->hThreadVideo) return;

	WaitForSingleObject(pstHelpData->hThreadVideo, INFINITE);
	CloseHandle(pstHelpData->hThreadVideo);
	pstHelpData->hThreadVideo = NULL;
}

int		GYTHHELPER_GetErrorInfo(int nErrorNo, char pszErrorInfo[256])
{
	if (!pszErrorInfo) return GFS_DEVICE_ERROR_PARAM;

	int		ncbLength, retval = GFS_DEVICE_ERROR_SUCCESS;

	ncbLength = 256;
	memset(pszErrorInfo, 0, ncbLength);

	ncbLength--;
	switch (nErrorNo)
	{
	case GFS_DEVICE_ERROR_SUCCESS:
		strncpy(pszErrorInfo, "调用成功!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_PARAM:
		strncpy(pszErrorInfo, "给定函数的参数有错误!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_MEMORY:
		strncpy(pszErrorInfo, "没有分配到足够的内存!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_NOTIMPL:
		strncpy(pszErrorInfo, "调用函数的功能没有实现!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_NODEVICE:
		strncpy(pszErrorInfo, "初始化的时候，检测到设备不存在!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_NOTINIT:
		strncpy(pszErrorInfo, "设备未初始化!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_NOLICENSE:
		strncpy(pszErrorInfo, "没有授权!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_SMALLBUFFER:
		strncpy(pszErrorInfo, "分配的内存缓冲区太小!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_INVALIDVIDEOWND:
		strncpy(pszErrorInfo, "无效的视频显示窗口!", ncbLength);
		break;
	case GFS_DEVICE_ERROR_INVALIDWND:
		strncpy(pszErrorInfo, "控件窗口创建失败!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_UNKNOWN:
		strncpy(pszErrorInfo, "未知错误!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_LOADLIBRAY:
		strncpy(pszErrorInfo, "装载接口动态库失败!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_INVALIDEDLL:
		strncpy(pszErrorInfo, "不是满足接口规范的DLL!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_INVALIDEAPPLY:
		strncpy(pszErrorInfo, "接口DLL不能用来实现指定的应用!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_GALIVECAPTDLL:
		strncpy(pszErrorInfo, "没有提供满足公安部活体采集接口标准的dll文件!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_CLOSETWAINDS:
		strncpy(pszErrorInfo, "用户取消或关闭了当前的TWAIN扫描过程!", ncbLength);
		break;

	case GFS_DEVICE_ERROR_FILEPATHISEMPTY:
		strncpy(pszErrorInfo, "未设置文件路径!", ncbLength);
		break;

	default:
		strncpy(pszErrorInfo, "非法错误号!", ncbLength);
		retval = GFS_DEVICE_ERROR_INVALIDENO;
		break;
	}

	return retval;
}