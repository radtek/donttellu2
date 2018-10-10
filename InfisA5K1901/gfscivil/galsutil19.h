/**
 * @file	galsutil19.h
 * @brief	公安出入境管理电子旅行证件、居民身份证指纹采集设备接口规范相关辅助函数和定义
 * @author	北京东方金指科技有限公司
 * @date	2010/12/27 11:32:44
 * @note	
 * @copyright	版权所有(@) 1996-2010
 *				北京东方金指科技有限公司
 */

#include "../gals19xy/gals19xy.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _INC_GALSUTIL19_H_20101227113513_
#define _INC_GALSUTIL19_H_20101227113513_

//!< 公安出入境管理电子旅行证件指纹最终图像大小
#define GFLS_PASSPORT_WIDTH			400
#define GFLS_PASSPORT_HEIGHT		400

//!< 公安居民身份证指纹最终图像大小
#define GFLS_IDFP_WIDTH			256
#define GFLS_IDFP_HEIGHT		360
#define GFLS_IDFP_GOODQUALITY	80		//!< 优良指纹质量
#define GFLS_IDFP_ACCEPTQUALITY	70		//!< 可以接受的指纹质量

#define GFLS_MNTVERSION			"1901"
#define GFLS_IDFP_MNTVERSION	"0410"
#define GFLS_VERSION			100

//!< 公司内部使用时定义此值，非内部使用注释掉,add by xff 2012-12-19
//!< 目的：当为长春编译时需要此值，目的增加标准DLL中的2个处理序号的接口
//#define BUILDGALSFORLOCALGAFIS

//!< 256级灰度图像头
typedef struct tagUTIL19_256BitmapInfo 
{
	BITMAPINFOHEADER	bmiHeader; 
	RGBQUAD				bmiColors[256]; 
} UTIL19_256BITMAPINFO;


/**
 * 图像采集、预览、提取特征和比对用到的相关参数
 */
typedef	struct	tagUTIL19_HelpData
{
	CRITICAL_SECTION	stCrit;		//!< 同步

	HWND	hWndPreview;		//!< 预览窗口句柄
	HANDLE	hThreadPreview;		//!< 预览线程句柄
	HANDLE	hThreadGetFPBmp;	//!< 自动采集一帧指纹图像数据的线程句柄，目前用在身份证指纹采集SDK	

	UCHAR	*pDataBuffer;	//!< 用来存放采集图像的Buffer
	UCHAR	*pPreviewData;	//!< 用来存放预览图像的Buffer
	UCHAR	*pCaptureData;	//!< 用来存放采集一帧图像的Buffer
	int		nDataBuffer;	//!< pDataBuffer的大小
	int		nFrameBuffer;	//!< pPreviewData、pCaptureData缓冲区的大小

	UCHAR	*pFingerData;	//!< 用来存放待提取特征的指纹数据，其大小是不确定的
	UCHAR	*pOneLineData;	//!< 用来存放一行图像数据
	int		nFingerData;	//!< pFingerData的大小
	int		nOneLineData;

	/**
	 * 采集仪采集到得原始图像的大小
	 */
	int		nWidth, nHeight;

	UCHAR	bCritInited;	//!< stCrit是否已经初始化	
	UCHAR	bClosePreview;	//!< 是否需要关闭预览
	UCHAR	bHelpDataInit;
	UCHAR	bCloseGetFPBmp;	//!< 是否停止自动采集一幅指纹图像

	//!< 显示预览图像用
	UTIL19_256BITMAPINFO	st256BmpInfo;
	RECT	rtImage;

	FILE	*fpLog;				//!< 调试用的输出log
}UTIL19_HELPDATA;

/**
 * 图像纠偏信息
 */
typedef struct tagUTIL19_RegionRect
{
	int x;				//	（输入输出）对象的左上角位置（相对图象），
	int y;				
	int beExist;		//	（输出）存在标记，0...不存在，1...存在
	int nAngle;			//	（输出）旋转角度  垂直向下是0度，右边是正角度，左边是负角度，只是对平面捺印指纹有用
	int nWidth;			//	（输入输出）对象宽度
	int nHeight;		//	（输入输出）对象长度
	int nObjectType;	//	（输入输出）对象类型，(1-10为滚动十指，21-30为平面十指，0表示指位未知)
} UTIL19_REGIONRECT;


#ifdef	__cplusplus
extern	"C" {
#endif


//typedef	UTIL19_REGIONRECT	REGIONRECT;
//!< 许公望在LiveApp.dll中提供的两个API函数

//判断平面指纹捺印区域和方向，如果捺印方向无法判断，
//函数返回值,    1....		判断成功，在pDestObj结构中返回采集框的左上角位置和指纹方向（垂直向下为0度，角度左负右正）
//				-1....		捺印方向无法判断时候，根据捺印区域的中心在pDestObj中返回采集框的左上角位置
//				KERROR_XXX..其他错误
//调用函数之前，需要在pDestObj中给采集框宽度和高度赋值
//pDestObj中指位信息nObjectType 赋值21...30, -1表示指位未知
/*extern "C"*/	int PlainImage_Splite(unsigned char *pImage, int nWidth, int nHeight, UTIL19_REGIONRECT *pDestObj);

//判断指掌纹捺印区域信息
//函数返回值：	>=0  区域的大小
//				< 0  错误信息 KERROR_***
/*extern "C"*/	int FPImage_AreaInfo(unsigned char *pImage,		//	输入的图像指针
									 int nWidth,				//	图像的宽度
									 int nHeight,				//	图像的高度
									 UTIL19_REGIONRECT *pDestObj);		//	返回的图像外接矩形信息，左上角位置和矩形的宽度高度



/**
 * 成功返回1，否则返回-1
 */
int		UTIL19_InitHelpData(UTIL19_HELPDATA *pstHelpData, int nWidth, int nHeight);
int		UTIL19_InitHelpDataEx(UTIL19_HELPDATA *pstHelpData, int nMaxWidth, int nMaxHeight, int nCaptWidth, int nCaptHeight);

void	UTIL19_UnInitHelpData(UTIL19_HELPDATA *pstHelpData);
void	UTIL19_StopPreviewThread(UTIL19_HELPDATA *pstHelpData);
void	UTIL19_StopGetFPBmpThread(UTIL19_HELPDATA *pstHelpData);

/**
 * 如果nWantBuffer>*pnCurBuffer，则重新分配内存
 * 成功分配后，原来的*ppBuffer将被释放，*pnCurBuffer=nWantBuffer
 */
int		UTIL19_ReMallocBuffer(UCHAR **ppBuffer, int *pnCurBuffer, int nWantBuffer);

void	UTIL19_VMirrorData(UCHAR *pBitData, int nWidth, int nHeight, UCHAR *pOneLineData);

/**
 * 该函数只是把pBmpFileData的前1078个字节封装成bmp文件的：BITMAPFILEHEADER、BITMAPINFOHEADER、RGBQUAD（颜色表）
 */
void	UTIL19_SaveToBmpFileFormat(UCHAR *pBmpFileData, int nWidth, int nHeight);

void	UTIL19_GetCurVideoWndRect(UTIL19_HELPDATA *pstHelpData);
void	UTIL19_DrawVideoPreview(UTIL19_HELPDATA *pstHelpData);

int		UTIL19_ImageDeviationRectify(UCHAR *pCaptureData, int nwCapture, int nhCapture, UCHAR *pRectifyData, int nwClip, int nhClip);

/**
 * 在调用该函数前，pstRectifyInfo的nWidth和nHeight必须指定了最终纠偏后图像的宽度和高度
 *	pstRectifyInfo的nObjectType用来指定纠偏的指位，具体信息请参考UTIL19_REGIONRECT的定义
 */
int		UTIL19_GetImageRectifyInfo(UCHAR *pCaptureData, int nwCapture, int nhCapture, UTIL19_REGIONRECT *pstRectifyInfo);


/**
 * 根据pstRectifyInfo从pCaptureData中得到纠偏后的图像数据
 * pRectifyData必须由调用者分配，其大小不能小于pstRectifyInfo中的宽度x高度
 */
int		UTIL19_GetRectifyImageData(UCHAR *pCaptureData, int nwCapture, int nhCapture, 
								   UCHAR *pRectifyData, UTIL19_REGIONRECT *pstRectifyInfo);

/**
 * 下面的这两个函数中的参数与《公安出入境管理电子旅行证件指纹采集设备接口规范》中对应函数
 * （提取特征函数和比对函数）中的一致（pstHelpData除外）
 */
int		UTIL19_FeatureExtractMem(UCHAR *pcImageData, long lDataLen, UCHAR *pcTemplateData, UTIL19_HELPDATA *pstHelpData);
int		UTIL19_FeatureMatchMem(unsigned char* pcTemplateDataA, unsigned char *pcTemplateDataB, float *pfSim);

/**
 * 根据指定的图像数据提取特征
 * @pImageData：8位灰度图像数据
 * @pbnMntData：存放特征数据的Buffer，调用者负责分配足够大的空间
 * @pnMntSize：提取的特征的大小
 * @返回值：>0，指纹质量值[1,100]；0，不是指纹；<0 失败
 */
int		UTIL19_ExtractFeature(UCHAR *pImageData, int nWidth, int nHeight, UCHAR *pbnMntData, int *pnMntSize);

/**
 * 根据指定的图像数据提取特征
 * @pImageData：8位灰度图像数据
 * @pbnMntData：存放特征数据的Buffer，调用者负责分配足够大的空间
 * @pnMntSize：提取的特征的大小
 * @pnInnerError：内部错误号
 * @返回值：>0，指纹质量值[1,100]；<=0 失败
 */
int		UTIL19_ExtractFeatureEx(UCHAR *pImageData, int nWidth, int nHeight, UCHAR *pbnMntData, int *pnMntSize, int *pnInnerError);


int		UTIL19_GetMaxFeatureSize();

#define	UTIL19_MATCHSCORE_MAXSCORE		9999
int		UTIL19_MatchFeature(UCHAR *pbnTemplateA, UCHAR *pbnTemplateB, int *pnScore);

/**
 * 根据公安部标准错误代码，返回相应的错误描述
 * @nGAStdType：gals19xy.h中定义的公安部规范分类
 */
int		UTIL19_GetGAStdErrorInfo(int nErrorNo, char *pszErrorInfo, int ncbErrorInfo, int nGAStdType);

#ifdef	__cplusplus
}
#endif

#endif	// _INC_GALSUTIL19_H_20101227113513_