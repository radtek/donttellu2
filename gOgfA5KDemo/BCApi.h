
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
		���룺
				pInImg ����ͼ��
				width ͼ���
				height ͼ���
				Contrast �Աȶȣ�ȡֵ��Χ0-200������ֵ80
				Bright ���ȣ�ȡֵ��Χ0-100������ֵ 50
				pOutImg ��ǿ���ͼ��
		���أ������������ƶ�ֵ����Χ(0-1)
		���ܣ���ͼ�������ǿ
	*/
BCAPI int Contrast_Bright(unsigned char *pInImg,int width,int height,int Bright,int Contrast,unsigned char *pOutImg);
	
#ifdef __cplusplus
}
#endif

#endif  // __ID_FPR_GFS_H__
