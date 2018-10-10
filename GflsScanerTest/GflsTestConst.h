//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GflsTestConst.h	define macro, const and struct
// create date: 2006-04-17
// 
//
#pragma once

#include <math.h>


#define GFLS_EDZCS_DEF    1
//#define GFLS_EDZCS_GSTEST 1

#if GFLS_EDZCS_DEF1
#define GFLS_SCANNER_IMAGEWIDTH			640//256
#define GFLS_SCANNER_IMAGEHEIGHT		640//360
#else
#define GFLS_SCANNER_IMAGEWIDTH			2304//640
#define GFLS_SCANNER_IMAGEHEIGHT		2304//640

#endif

#ifdef INFIS_FOURFINGER

//for infis four fingers
#define GFLS_SCANNER_FINGER_WIDTH	1600
#define GFLS_SCANNER_FINGER_HEIGHT	1500

#else

#define GFLS_SCANNER_FINGER_WIDTH	640
#define GFLS_SCANNER_FINGER_HEIGHT	640

#endif

#define GFLS_SCANNER_CENTERWIDTH		600
#define GFLS_SCANNER_CENTERHEIGHT		600

#define GFLS_SCANNER_MAXSCALE			5
#define GFLS_SCANNER_MINSCALE			1

#define GFLS_SCANNER_TESTTITLE			_T("活体指纹采集仪测试")
#define GFLS_SCANNER_STARTCAPTURE		_T("采集图像")
#define GFLS_SCANNER_STOPCAPTURE		_T("停止采集")

#define GFLS_IMAGEINFODLG_TITLE			_T("图像信息(%d x %d @ %d%%)")


#define WM_SETIMAGEINFO_TOSTAT			WM_USER + 0x101				// 传送采集得到的图像信息，以进行直方图统计, wParam标记是否只统计选中的图像区域, lParam参数为空
#define WM_SHOWIMAGE_STATINFO			WM_USER + 0x102				// 显示统计信息, wParam和lParam参数为空


#define GFLS_SCANNER_STDCHKTYPE_NORMAL				0		// 正常显示模式,没有检测情况
#define GFLS_SCANNER_STDCHKTYPE_OPTICSDEVICE		1		// 光学硬件检测
#define GFLS_SCANNER_STDCHKTYPE_CAPTURESIZE			2		// 采集窗口尺寸检测
#define GFLS_SCANNER_STDCHKTYPE_VALIDIMAGESIZE		3		// 有效图像尺寸检测
#define GFLS_SCANNER_STDCHKTYPE_IMAGEPIXELCOUNT		4		// 图像像素数检测
#define GFLS_SCANNER_STDCHKTYPE_CENTERDIFF			5		// 图像中心偏差检测
#define GFLS_SCANNER_STDCHKTYPE_IMGRESOLUTION		6		// 图像分辨率检测
#define GFLS_SCANNER_STDCHKTYPE_ABERRANCE			7		// 检测图像畸变
#define GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYAVERAGE	8		// 图像背景灰度值检测
#define GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYUNAVE	9		// 检测图像灰度不均匀度
#define GFLS_SCANNER_STDCHKTYPE_CENTERGRAY			10		// 检测中心区域图像灰度值
#define GFLS_SCANNER_STDCHKTYPE_GRAYVARIA			11		// 检测灰度动态范围
#define GFLS_SCANNER_STDCHKTYPE_BADPOINT			12		// 检测图像疵点
#define GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT			13		// 检测环境光
#define GFLS_SCANNER_STDCHKTYPE_PARSE				14		// 检测图像解析度
#define GFLS_SCANNER_STDCHKTYPE_CAPTSPEED			15		// 检测采集速率
#define GFLS_SCANNER_STDCHKTYPE_MOVESQUARE  			16		// 移动矩形框

#include "GfStdLiveApi.h"
#include "GflsCaptMosaic.h"
#include "GfDlgBarEx.h"

// struct define
typedef struct tagGf_256BitmapInfo 
{
	BITMAPINFOHEADER	bmiHeader; 
	RGBQUAD				bmiColors[256]; 
} GF_256BITMAPINFO;
typedef struct tagGflsTest_ParamInfo
{
	UCHAR*	pDispImage;			// 用来显示的图像数据
	UCHAR*	pSrcImage;			// 采集到的原始图像数据;
	int		nImgWidth;			// 原始图像数据的宽度和高度，分别为GFLS_SCANNER_IMAGEWIDTH和GFLS_SCANNER_IMAGEHEIGHT
	int		nImgHeight;
	int		nImgSize;			// 原始图像数据的大小

	int		nDispWidth;
	int		nDispHeight;
	int		nDispImgSize;

	int		nCurScale;			// 当前的放大倍数，有效值为1,2,3,4,5，最多放大5倍
	int		nCrossLength;		// 中心十字刻线的长度，100像素
	int		nSquare1;			// 两个居中的正方形线框的宽度300、500, 440
	int		nSquare2;
	int		nSquare3;
	int		nSquare4;       //正方形线框的宽度、高度、偏移
	//int		nSquare4H;
	int		nSquare4OffX;
	int		nSquare4OffY;
	CRect   rectSquare4;   

	POINT	ptOffSet;			// 图像显示时的偏移

	COLORREF	clCross;		// 中心十字刻线的颜色
	COLORREF	clSquare1;		// 居中的正方形线框的颜色
	COLORREF	clSquare2;
	COLORREF	clSquare3;
	COLORREF	clSquare4;
	COLORREF	clBack;

	POINT		ptAreaLeftTop;			// 当前选中的矩形区域的左上点和右下点，以显示图像的左上点为原点
	POINT		ptAreaRightBottom;
	COLORREF	clArea;					// 矩形区域的颜色
//	POINT		ptLineForEnvir1;		// 环境光检测时的两个部分的分割线
//	POINT		ptLineForEnvir2;		// 环境光检测时的两个部分的分割线

	UCHAR	bCrossSquareScaled;			// 十字刻线和线框是否也同步缩放
	UCHAR	bOnlyShowImage;				// 是否只显示图像
	UCHAR	bInsertZoom;				// 是否插值缩放图像

	UCHAR	nStdChkType;				// 标准检测类型
	UCHAR	bDrawBadPoint;				// 在检测疵点时，是否显示疵点分布图
	UCHAR	bCanStdCheck;				// 是否可以检测
	UCHAR	nReserved[2];
}GFLSTEST_PARAMINFO;

typedef struct tagGf_ImageParseInfo
{
	RECT	rtCArea[9];					// TP-F02中9个C区域的位置
	double	fBackMedian[9];				// 理解有误，应该为均值而不是中间值
	double	fWhiteMedian[9];
	int		nCurSelArea;				// 当前选中的区域
	POINT	ptPrevMouse;				// 
}GF_IMAGEPARSEINFO;

typedef struct tagGf_ChkToolSampleData
{
	GF_256BITMAPINFO	*pstbmpInfo;
	UCHAR	*pSampleData;
	int		nDataWidth;					// 检测样板数据的宽度和高度
	int		nDataHeight;

	int		nDPI;						// 检测样板的DPI
	int		nLength;					// 检测样板的长度(单位为mm的十分之一)

	UCHAR	*pDispData;
	int		nDispWidth;
	int		nDispHeight;

	UCHAR	nChkToolType;				// 检测样板类型
	UCHAR	nReserved[7];
}GF_CHKTOOLSAMPLEDATA;


#define GFLS_SCANNER_CHKTOOL_GRIDSAMPLE			1		// 标准网格标定检测样板
#define GFLS_SCANNER_CHKTOOL_RESOLUTION			2		// 标准分辨率检测样板

extern void Gf_SaveData2BitmapFile(UCHAR* pData, int nw, int nh, const char* pszFileName);
extern void EnlargeOrShinkImage(UCHAR *pSrcData, int nSrcWidth, int nSrcHeight, UCHAR *pDestData, int nDestWidth, int nDestHeight);
extern void ZoomImageByScale(UCHAR *pSrcData, int nSrcWidth, int nSrcHeight, UCHAR *pDesData, int nScale);
extern int  GetBitmapFileName(char* pszFileName, int ncbFileName, BOOL bOpenFile, CWnd* pParentWnd);
extern int	ShowScannerErrorInfo(int nError, CWnd* pParentWnd, UCHAR bLiveCapt);
extern int  ShowTestAppErrorInfo(const TCHAR* pszErrorInfo, CWnd* pParentWnd);
extern int	ZoomPointByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg);
extern int	ZoomPointXByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg);
extern int	ZoomPointYByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg);
extern int	ZoomPointFromOld2New(RECT *prtArea, SIZE *psize, POINT *ppt, int nOldScale, int nNewScale);

extern void ReverseImageData(BYTE* pData, int nw, int nh);
typedef struct tagGf_ScannerInfo
{
	HANDLE	hCaptThread;	// 采集线程

	int nMaxChannel;		// 通道数
	int	nCurChannel;		// 当前使用的通道

	int	nBright;			// 亮度、对比度
	int	nContrast;

#ifdef UNSTD_BLACK_BALANCE
	int	nBlackBalance;		//黑平衡
#endif
	int	nWidth;				// 宽度和高度
	int nHeight;

	int	nXOrigin;
	int	nYOrigin;

	int	nVersion;

	UCHAR	bInitialed;		// 初始化是否成功
	UCHAR	bExitCapture;	// 是否退出采集线程
	UCHAR	bCaptureImage;	// 是否正在采集图像
	UCHAR   bReverseImage;  // 是否反象图像

	UCHAR	bFaseDev;		// 是否快设备
	UCHAR	bSptCaptWnd;	// 是否支持采集窗口的设置
	UCHAR	bSptSetup;		// 是否支持Setup设置对话框
	UCHAR	bExitMosaic;	// 是否退出拼接过程
	UCHAR	bHasMosaicThrd;	// 拼接线程是否已经启动
	UCHAR	nReserved[1];
	int		nCaptFrames;
}GF_SCANNERINFO;

extern GF_SCANNERINFO	g_stScannerInfo;

extern int a;
typedef struct tagGf_CaptSpeedInfo
{
	int		nFrameCount;		// 采集的帧数
	int		nUsedTimes;			// 耗费的时间
	double	fSpeed;				// 采集速率
}GF_CAPTSPEEDINFO;


typedef struct tagGf_ImageSaveInfo
{
	char	szFilePath[256];
	char	szFileName[32];
	int		nCurSerial;
	UCHAR	bSaveCaptImage;
	UCHAR	nFileType;				// 0 -- bmp, 1 -- raw
	UCHAR	nReserved[2];
}GF_IMAGESAVEINFO;


typedef struct tagGf_ImageGrayStatStruct
{
	UCHAR	*pImageData;		// 要检测的8位256级灰度图像
	int		*pnGrayValueCnt;	// NULL或256个长度的整形数组,用来保存各灰度值的个数,即pnGrayValueCnt[i]表示灰度值为i的像素个数, i=0 -- 255
	int		nw, nh;				// 图像的宽度和高度
	RECT	rtArea;				// 要统计的图像区域

	int		nPixelCount;		// 被统计的像素数
	int		nTotalGrayCount;	// 给定区域的灰度值总和
	int		nMaxCntGrayValue;	// 具有最多灰度值的灰度,以及具有该灰度值的像素个数
	int		nMaxCntGrayCount;
	int		nMinCntGrayValue;	// 具有最少灰度值的灰度,以及具有该灰度值的像素个数(必须大于0)
	int		nMinCntGrayCount;

	int		nMinGrayValue;		// 该图像所具有的最大和最小的非0灰度值
	int		nMaxGrayValue;

	double	fAverage;			// 平均灰度值
	double	fDiff;				// 标准偏差
	double	fMedian;			// 中间值
}GF_IMAGEGRAYSTATSTRUCT;

int GetImageGrayStatisticInfo(GF_IMAGEGRAYSTATSTRUCT *pstGrayStat);
int GetImageAreaLineGray(GF_IMAGEGRAYSTATSTRUCT *pstGrayStat, double *pfBackMedian, double *pfWhiteMedian);


#define	GFLSTEST_FILEEXIST(fname)	(_access(fname, 0) < 0 ? 0 : 1)
int	GflsTest_DirCreateRecur(char *pathname, int flag);


void	StopCaptureThread();
DWORD	WINAPI StartCaptureThread(LPVOID lParam);

//int	Gfls_PointCompare(const void* pelem1, const void* pelem2);


// 标准检测

enum GF_ABERPARAMPOS
{
	ABERPOS_UNKNOWN,
	ABERPOS_A1,
	ABERPOS_A2,
	ABERPOS_A3,
	ABERPOS_A4,
	ABERPOS_B1,
	ABERPOS_B2,
	ABERPOS_B3,
	ABERPOS_B4,
	ABERPOS_CENTER
};


extern inline double GetTowPointLength(LPPOINT ppt1, LPPOINT ppt2);


typedef struct tagGf_BadPointInfo
{
	POINT*	paBadPoint;			// 连续的疵点数组；
	int		nBadptCount;		// 连续疵点个数
	int		nIndex;				// 当前疵点索引（在数组中的索引）
	int		xRadio, yRadio;		// x,y方向的直径
	int		nRadio;

//	int		xStart, yStart;		// 起始点;
//	int		xCurr,  yCurr;		// 当前检查点
	
	UCHAR	bCenter;			// 是否主视区
	UCHAR	nReserved[3];
}GF_BADPOINTINFO;

typedef struct tagGf_BadPointData
{
	POINT*	paBadPoint;			// 连续的疵点数组；
	int		nBadptCount;		// 连续疵点个数
	int		nRadio;				// 直径
	UCHAR	bCenter;
	UCHAR	nReserved[3];
}GF_BADPOINTDATA;

typedef struct tagGf_BadptDataArray
{
	GF_BADPOINTDATA	*pstBadData;
	int				nBadData;
	int				nCurDataBuf;
	int				nStep;
}GF_BADPTDATAARRAY;


// image info 
typedef struct tagGfls_ImageInfoStruct
{
	UCHAR			 *pImgData;		// 图像数据缓冲区,  长度为(宽度 * 高度 * 位数 / 8)
	RGBQUAD			 *prgbQuad;		// Color-mapped(位数 < 24)图像的颜色表信息，长度由像素位数决定
	BITMAPINFOHEADER bmpHead;	// 用来保存图像的宽度、高度、像素位数等信息
}GFLS_IMAGEINFOSTRUCT;

extern int Gfls_LoadImageFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_SaveImageFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_LoadRawImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_LoadBmpImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_SaveRawImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_SaveBmpImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern void VerticalFlipBuffer(UCHAR *pBuf, int nw, int nh, int nbits);

__int64	UTIL_HiResoTickCount();
__int64	UTIL_HiResoTickPerSec();
double	UTIL_HiResoTickToSec(__int64 nTimeUsed);
