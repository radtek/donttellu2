#pragma once

#ifndef _XV_TRAPPING_INSPECTION_TOOL_H_
#define _XV_TRAPPING_INSPECTION_TOOL_H_

#ifdef MOSAIC_API
#else 
#define MOSAIC_API __declspec(dllimport)
#endif

//1 初始化拼接动态库
MOSAIC_API int _stdcall MOSAIC_Init();

//2 释放拼接动态库
MOSAIC_API int _stdcall MOSAIC_Close();

//3 拼接接口是否提供判断图像为指纹的函数
MOSAIC_API int _stdcall MOSAIC_IsSupportIdentifyFinger();

//4 拼接接口是否提供判断图像质量的函数
MOSAIC_API int _stdcall MOSAIC_IsSupportImageQuality();

//5 拼接接口是否提供判断指纹质量的函数
MOSAIC_API int _stdcall MOSAIC_IsSupportFingerQuality();

//6 接口是否提供拼接指纹的图像增强功能
MOSAIC_API int _stdcall MOSAIC_IsSupportImageEnhance();

//7 判断是否支持滚动采集函数
MOSAIC_API int _stdcall MOSAIC_IsSupportRollCap();

//8 选择拼接方式的函数
MOSAIC_API int _stdcall MOSAIC_SetRollMode(int nRollMode);

//9 初始化拼接过程
MOSAIC_API int _stdcall MOSAIC_Start(unsigned char* pFingerBuf, int nWidth, int nHeight);

//10 拼接过程
MOSAIC_API int _stdcall MOSAIC_DoMosaic(unsigned char* pDataBuf, int nWidth, int nHeight);

//11 结束拼接
MOSAIC_API int _stdcall MOSAIC_Stop();

//12 判断图像质量
MOSAIC_API int _stdcall MOSAIC_ImageQuality(unsigned char* pDataBuf, int nWidlh, int nHeight);

//13 判断指纹质量
MOSAIC_API int _stdcall MOSAIC_FingerQuality(unsigned char* pDataBuf, int nWidth, int nHeight);

//14 对图像进行增强
MOSAIC_API int _stdcall MOSAIC_ImageEnhance(unsigned char* pSrcImg, int nWidth, int nHeight, unsigned char* pTargetImg);

//15 判断图像是否为指纹
MOSAIC_API int _stdcall MOSAIC_IsFinger(unsigned char* pDataBuf, int nWidth, int nHeight);

//16 取得拼接接口错误信息
MOSAIC_API int _stdcall MOSAIC_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

//17 取得接口的版本
MOSAIC_API int _stdcall MOSAIC_GetVersion();

//18 获得拼接接口的说明
MOSAIC_API int _stdcall MOSAIC_GetDesc(char pszDesc[1024]);

#endif