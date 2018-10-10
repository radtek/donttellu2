/**
 * @file	gfsCustom.h
 * @brief	机场通关项目图像采集、特征提取和指纹比对接口规范
 * @author	北京东方金指科技有限公司
 * @date	2011/04/02 16:33:10
 * @note	
 * @copyright	版权所有(@) 1996-2011
 *				北京东方金指科技有限公司
 */

#ifndef _INC_GFSCUSTOM_H_20110402163458_
#define _INC_GFSCUSTOM_H_20110402163458_


/**
 * 指纹、掌纹、拼接接口的调用约定
 */
#define GFS_CUSTOM_API		int __stdcall


/**
 * 接口
 */

#ifdef	__cplusplus
extern	"C" {
#endif

	//!< 活体指纹图像采集接口
	GFS_CUSTOM_API		GFS_CUSTOM_Init();
	GFS_CUSTOM_API		GFS_CUSTOM_Close();
	GFS_CUSTOM_API		GFS_CUSTOM_GetChannelCount();
	GFS_CUSTOM_API		GFS_CUSTOM_SetBright(int nChannel, int nBright);
	GFS_CUSTOM_API		GFS_CUSTOM_SetContrast(int nChannel, int nContrast);
	GFS_CUSTOM_API		GFS_CUSTOM_GetBright(int nChannel, int* pnBright);
	GFS_CUSTOM_API		GFS_CUSTOM_GetContrast(int nChannel, int* pnContrast);
	GFS_CUSTOM_API		GFS_CUSTOM_GetImageSize(int nChannel, int* pnWidth, int* pnHeight);
	GFS_CUSTOM_API		GFS_CUSTOM_BeginCapture(int nChannel);
	GFS_CUSTOM_API		GFS_CUSTOM_GetFPRawData(int nChannel, unsigned char* pRawData);
	GFS_CUSTOM_API		GFS_CUSTOM_EndCapture(int nChannel);
	GFS_CUSTOM_API		GFS_CUSTOM_GetVersion();
	GFS_CUSTOM_API		GFS_CUSTOM_GetDesc(char pszDesc[1024]);
	GFS_CUSTOM_API		GFS_CUSTOM_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	GFS_CUSTOM_API		GFS_CUSTOM_GetMaxFeatureSize (int *pnFeatureSize);
	GFS_CUSTOM_API		GFS_CUSTOM_ExtractFeature(unsigned char * pRawData, int nWidht, int nHeight, 
												  unsigned char *pbnFeatureData, int *pnFeatureSize);
	GFS_CUSTOM_API		GFS_CUSTOM_Match(unsigned char *pbnFeatureData1, unsigned char * pbnFeatureData2, double*pfScore);


#ifdef	__cplusplus
}
#endif


//!< 接口错误代码
#define GFS_CUSTOM_ERROR_SUCCESS			1		//!< 函数调用成功
#define GFS_CUSTOM_ERROR_PARAM				-1		// 参数错误。给定函数的参数有错误。
#define GFS_CUSTOM_ERROR_MEMORY				-2		// 内存分配失败。没有分配到足够的内存。
#define GFS_CUSTOM_ERROR_NOTIMPL			-3		// 功能未实现。调用函数的功能没有实现。
#define GFS_CUSTOM_ERROR_INVALIDENO			-4		// 非法的错误号。
#define GFS_CUSTOM_ERROR_NOLICENSE			-5		// 没有授权。

#define GFS_CUSTOM_ERROR_UNKNOWN			-100	// 未知错误

#define GFS_CUSTOM_ERROR_NODEVICE			-101	// 设备不存在。初始化的时候，检测到设备不存在。
#define GFS_CUSTOM_ERROR_NOTINIT			-102	// 设备未初始化。

#define GFS_CUSTOM_ERROR_BLANKIMAGE			-201		// 图像为空白图
#define GFS_CUSTOM_ERROR_POORQUALITY		-202		// 图像质量太差
#define GFS_CUSTOM_ERROR_TOOSMALLAREA		-203        // 采集到的图像太小
#define GFS_CUSTOM_ERROR_SHIFTIMAGE			-204        // 图像没有居中（太偏了）


#endif	// _INC_GFSCUSTOM_H_20110402163458_
