
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ID_FPR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ID_FPR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __ID_FPR_GFS_H__
#define __ID_FPR_GFS_H__

#if defined( WIN32 ) || defined(WIN64)

#ifdef BCAPI
#define BCAPI __declspec(dllexport)
#else
#define BCAPI __declspec(dllimport)
#endif

#else

#define ID_FPR_API

#endif

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
	// used by C++ source code
#endif

/* 4 
		输入：
				pInImg 输入图像
				width 图像宽
				height 图像高
				Contrast 对比度，取值范围0-200，建议值80
				Bright 亮度，取值范围0-100，建议值 50
				pOutImg 增强后的图像
		返回：两个特征相似度值，范围(0-1)
		功能：对图像进行增强
	*/
BCAPI int Contrast_Bright(unsigned char *pInImg,int width,int height,int Bright,int Contrast,unsigned char *pOutImg);
	
#ifdef __cplusplus
}
#endif

#endif  // __ID_FPR_GFS_H__
