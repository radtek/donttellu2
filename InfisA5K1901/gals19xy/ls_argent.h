/**
 * @file	ls_argent.h
 * @brief	阿根廷指掌纹采集仪接口规范相关说明
 * @author	北京东方金指科技有限公司
 * @date	2012/11/19 16:45:57
 * @note	
 * @copyright	版权所有(@) 1996-2012
 *				北京东方金指科技有限公司
 */

#ifndef _INC_LS_ARGENT_H_20121119164618_
#define _INC_LS_ARGENT_H_20121119164618_

#pragma warning( disable : 4996)


//!< 阿根廷规范分类
enum { ARGENT_LIVESCAN, ARGENT_PASSPORT, ARGENT_IDFP };


//!< 阿根廷活体指纹采集接口标准中有关采集窗口设置的返回值
enum
{
	ARGENT_CAPTWND_NONE = 0,	//!< 采集接口不支持设置采集窗口
	ARGENT_CAPTWND_SINGLE,		//!< 采集接口只支持单指指纹采集窗口的设置
	ARGENT_CAPTWND_FOURFINGER,	//!< 采集接口只支持平面右手四连指、平面左手四连指或平面左右手拇指指纹采集窗口的设置
	ARGENT_CAPTWND_ALL			//!< 采集接口支持单指、平面右手四连指、平面左手四连指或平面左右手拇指指纹采集窗口的设置
};

//!< 指纹、掌纹、拼接、居民身份证采集接口的调用约定
#define ARGENT_LSCAPT_API		int __stdcall
#define ARGENT_PALMCAPT_API		int __stdcall
#define ARGENT_GAMOSAIC_API		int __stdcall



//!< 阿根廷指纹、掌纹、拼接标准接口

#ifdef	__cplusplus
extern	"C" {
#endif

	ARGENT_LSCAPT_API		GFS_LIVESCAN_Init();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_Close();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetChannelCount();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_SetBright(int nChannel, int nBright);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_SetContrast(int nChannel, int nContrast);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetBright(int nChannel, int* pnBright);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetContrast(int nChannel, int* pnContrast);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetMaxImageSize(int nChannel, int* pnWidth, int* pnHeight);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_Setup();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_BeginCapture(int nChannel);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetRawData(int nChannel, unsigned char* pRawData);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_EndCapture(int nChannel);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_IsSupportCaptWindow(int nChannel);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_IsSupportSetup();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetVersion();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetDesc(char pszDesc[1024]);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_INIT)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_CLOSE)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETCHANNELCOUNT)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETBRIGHT)(int nChannel, int nBright);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETCONTRAST)(int nChannel, int nContrast);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETBRIGHT)(int nChannel, int* pnBright);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETCONTRAST)(int nChannel, int* pnContrast);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETMAXIMAGESIZE)(int nChannel, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETCAPTWINDOW)(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETCAPTWINDOW)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETUP)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_BEGINCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETRAWDATA)(int nChannel, unsigned char* pRawData);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_ENDCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_ISSUPPORTCAPTWINDOW)(int nChannel);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_ISSUPPORTSETUP)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETVERSION)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETDESC)(char pszDesc[1024]);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);


#if 0
	//!< 活体图像拼接应用程序接口
	GF_GAMOSAIC_API		MOSAIC_Init();
	GF_GAMOSAIC_API		MOSAIC_Close();
	GF_GAMOSAIC_API		MOSAIC_IsSupportIdentifyFinger();
	GF_GAMOSAIC_API		MOSAIC_IsSupportImageQuality();	
	GF_GAMOSAIC_API		MOSAIC_IsSupportFingerQuality();
	GF_GAMOSAIC_API		MOSAIC_IsSupportImageEnhance();
	GF_GAMOSAIC_API		MOSAIC_IsSupportRollCap();
	GF_GAMOSAIC_API		MOSAIC_SetRollMode(int nRollMode);
	GF_GAMOSAIC_API		MOSAIC_Start(unsigned char* pFingerBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_DoMosaic(unsigned char* pDataBuf, int nWidht, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_Stop();
	GF_GAMOSAIC_API		MOSAIC_ImageQuality(unsigned char* pDataBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_FingerQuality(unsigned char* pDataBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_ImageEnhance(unsigned char* pSrcImg, int nWidth, int nHeight, unsigned char* pTargetImg);
	GF_GAMOSAIC_API		MOSAIC_IsFinger(unsigned char* pDataBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);
	GF_GAMOSAIC_API		MOSAIC_GetVersion();
	GF_GAMOSAIC_API		MOSAIC_GetDesc(char pszDesc[1024]);

	typedef	int (__stdcall *	GFLS_MOSAIC_INIT)();
	typedef	int (__stdcall *	GFLS_MOSAIC_CLOSE)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTIDENTIFYFINGER)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTIMAGEQUALITY)();	
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTFINGERQUALITY)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTIMAGEENHANCE)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTROLLCAP)();
	typedef	int (__stdcall *	GFLS_MOSAIC_SETROLLMODE)(int nRollMode);
	typedef	int (__stdcall *	GFLS_MOSAIC_START)(unsigned char* pFingerBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_DOMOSAIC)(unsigned char* pDataBuf, int nWidht, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_STOP)();
	typedef	int (__stdcall *	GFLS_MOSAIC_IMAGEQUALITY)(unsigned char* pDataBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_FINGERQUALITY)(unsigned char* pDataBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_IMAGEENHANCE)(unsigned char* pSrcImg, int nWidth, int nHeight, unsigned char* pTargetImg);
	typedef	int (__stdcall *	GFLS_MOSAIC_ISFINGER)(unsigned char* pDataBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);
	typedef	int (__stdcall *	GFLS_MOSAIC_GETVERSION)();
	typedef	int (__stdcall *	GFLS_MOSAIC_GETDESC)(char pszDesc[1024]);

	//!< 活体掌纹图像采集接口
	GF_PALMCAPT_API		PALM_Init();
	GF_PALMCAPT_API		PALM_Close();
	GF_PALMCAPT_API		PALM_GetChannelCount();
	GF_PALMCAPT_API		PALM_SetBright(int nChannel, int nBright);
	GF_PALMCAPT_API		PALM_SetContrast(int nChannel, int nContrast);
	GF_PALMCAPT_API		PALM_GetBright(int nChannel, int* pnBright);
	GF_PALMCAPT_API		PALM_GetContrast(int nChannel, int* pnContrast);
	GF_PALMCAPT_API		PALM_GetMaxImageSize(int nChannel, int* pnWidth, int* pnHeight);
	GF_PALMCAPT_API		PALM_GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	GF_PALMCAPT_API		PALM_SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	GF_PALMCAPT_API		PALM_Setup();
	GF_PALMCAPT_API		PALM_BeginCapture(int nChannel);
	GF_PALMCAPT_API		PALM_GetFPRawData(int nChannel, unsigned char* pRawData);
	GF_PALMCAPT_API		PALM_EndCapture(int nChannel);
	GF_PALMCAPT_API		PALM_IsSupportCaptWindow(int nChannel);
	GF_PALMCAPT_API		PALM_IsSupportSetup();
	GF_PALMCAPT_API		PALM_GetPreviewImageSize(int nChannel,int *pnWidth,int *pnHeight);
	GF_PALMCAPT_API		PALM_GetPreviewData(int nChannel, unsigned char* pRawData );
	GF_PALMCAPT_API		PALM_IsSupportPreview();
	GF_PALMCAPT_API		PALM_GetVersion();
	GF_PALMCAPT_API		PALM_GetDesc(char pszDesc[1024]);
	GF_PALMCAPT_API		PALM_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	typedef int	(__stdcall *	GFLS_PALM_INIT)();
	typedef int	(__stdcall *	GFLS_PALM_CLOSE)();
	typedef int	(__stdcall *	GFLS_PALM_GETCHANNELCOUNT)();
	typedef int	(__stdcall *	GFLS_PALM_SETBRIGHT)(int nChannel, int nBright);
	typedef int	(__stdcall *	GFLS_PALM_SETCONTRAST)(int nChannel, int nContrast);
	typedef int	(__stdcall *	GFLS_PALM_GETBRIGHT)(int nChannel, int* pnBright);
	typedef int	(__stdcall *	GFLS_PALM_GETCONTRAST)(int nChannel, int* pnContrast);
	typedef int	(__stdcall *	GFLS_PALM_GETMAXIMAGESIZE)(int nChannel, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_PALM_GETCAPTWINDOW)(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_PALM_SETCAPTWINDOW)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	typedef int	(__stdcall *	GFLS_PALM_SETUP)();
	typedef int	(__stdcall *	GFLS_PALM_BEGINCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_PALM_GETFPRAWDATA)(int nChannel, unsigned char* pRawData);
	typedef int	(__stdcall *	GFLS_PALM_ENDCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_PALM_ISSUPPORTCAPTWINDOW)(int nChannel);
	typedef int	(__stdcall *	GFLS_PALM_ISSUPPORTSETUP)();
	typedef	int	(__stdcall *	GFLS_PALM_GETPREVIEWIMAGESIZE)(int nChannel, int *pnWidth, int *pnHeight);
	typedef	int	(__stdcall *	GFLS_PALM_GETPREVIEWDATA)(int nChannel, unsigned char *pRawData);
	typedef	int (__stdcall *	GFLS_PALM_ISSUPPORTPREVIEW)(void);
	typedef int	(__stdcall *	GFLS_PALM_GETVERSION)();
	typedef int	(__stdcall *	GFLS_PALM_GETDESC)(char pszDesc[1024]);
	typedef int	(__stdcall *	GFLS_PALM_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);
#endif

#ifdef	__cplusplus
}
#endif



/**
 * 阿根廷标准接口错误代码
 */

//!< 活体指纹、掌纹图像采集接口错误代码
#define ARGENT_GALSCAPT_ERROR_SUCCESS			1
#define ARGENT_GALSCAPT_ERROR_PARAM				-1			// 参数错误。给定函数的参数有错误。
#define ARGENT_GALSCAPT_ERROR_MEMORY			-2			// 内存分配失败。没有分配到足够的内存。
#define ARGENT_GALSCAPT_ERROR_NOTIMPL			-3			// 功能未实现。调用函数的功能没有实现。
#define ARGENT_GALSCAPT_ERROR_NODEVICE			-4			// 设备不存在。初始化的时候，检测到设备不存在。
#define ARGENT_GALSCAPT_ERROR_NOTINIT			-5			// 设备未初始化。
#define ARGENT_GALSCAPT_ERROR_INVALIDENO		-6			// 非法的错误号。
#define ARGENT_GALSCAPT_ERROR_NOLICENSE			-7			// 没有授权。
#define ARGENT_GALSCAPT_ERROR_OTHERERR			-9			// 其他错误。

#define ARGENT_GALSCAPT_ERROR_INVALIDCHANNLE	-101		// 无效的通道号
#define ARGENT_GALSCAPT_ERROR_INVALIDCAPTWIN	-102		// 非法采集区域
#define ARGENT_GALSCAPT_ERROR_NOPRIVILEGE		-103        // 没有特权
#define ARGENT_GALSCAPT_ERROR_FAIL			    -104        // 函数调用失败（不做更详细的提示）
#define ARGENT_GALSCAPT_ERROR_WRONG			    -105        // 调用错误
#define ARGENT_GALSCAPT_ERROR_RECOVERYSUCCESS   -106        // 设备进行了恢复并且恢复成功
#define ARGENT_GALSCAPT_ERROR_RECOVERYFAIL      -107        // 设备进行了恢复，但恢复失败了
#define ARGENT_GALSCAPT_ERROR_BADFRAME          -108        // 由于UC500C在某些参数设置（RowSize, ColumnSize, RowSkip, ColumnSkip）后
														//，紧接着的一帧会是无效帧
#define ARGENT_GALSCAPT_ERROR_INVALIDFRAME      -109		// 数据传输中有错误，图像有部分数据丢失或没有图像
														// 数据（返回此值并不代表设备工作不正常）
#define ARGENT_GALSCAPT_ERROR_VIOLATION         -110        //此函数的调用与其他功能有冲突

#define ARGENT_GALSCAPT_ERROR_UNKNOWN			-111		// 未知错误

#define ARGENT_GALSCAPT_ERROR_BADQUALITY		-112		//!< 采集指纹的图像质量差

#if 0
//!< 活体图像拼接应用程序接口错误代码
#define GF_GAMOSAIC_ERROR_SUCCESS			1
#define GF_GAMOSAIC_ERROR_PARAM				-1			// 参数错误。给定函数的参数有错误
#define GF_GAMOSAIC_ERROR_MOMERY			-2			// 内存分配失败。没有分配到足够的内存
#define GF_GAMOSAIC_ERROR_NOTIMPL			-3			// 功能未实现。调用函数的功能没有实现。
#define GF_GAMOSAIC_ERROR_INVALIDENO		-6			// 非法错误号
#define GF_GAMOSAIC_ERROR_NOLICENSE			-7			// 没有授权
#define GF_GAMOSAIC_ERROR_NOTINIT			-8			// 拼接未初始化

#define GF_GAMOSAIC_ERROR_INVALIDROLLMODE	-101		// 无效的滚动方式
#define GF_GAMOSAIC_ERROR_TOOQUICK			-102		//滚动速度太快
#define GF_GAMOSAIC_ERROR_ROLLBACK			-103		//大幅度回滚
#define GF_GAMOSAIC_ERROR_DISLOCATION		-104		//由于回滚或者捺印变形导致纹线错位

#define	GF_GAMOSAIC_ERROR_OVERTIME			-151	// 操作超时。
#define	GF_GAMOSAIC_ERROR_MEMERROR			-152	// 内存问题。


/**
 * 阿根廷标准指纹、掌纹大小
 */
#define  GALS_ALLPALM_WIDTH			2304
#define  GALS_ALLPALM_HEIGHT		3584

#define  GALS_PALMSUB_WIDTH			2304
#define  GALS_PALMSUB_HEIGHT		2304

#define  GALS_WRITERPALM_WIDTH		1024
#define  GALS_WRITERPALM_HEIGHT		1968

#define GALS_FOURFINGER_WIDTH		1600
#define GALS_FOURFINGER_HEIGHT		1500

#define GALS_FINGER_WIDTH			640
#define GALS_FINGER_HEIGHT			640 
#endif

#endif	// _INC_LS_ARGENT_H_20121119164618_