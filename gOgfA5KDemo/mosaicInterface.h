#pragma once

#ifndef _XV_TRAPPING_INSPECTION_TOOL_H_
#define _XV_TRAPPING_INSPECTION_TOOL_H_

#ifdef MOSAIC_API
#else 
#define MOSAIC_API __declspec(dllimport)
#endif

//1 ��ʼ��ƴ�Ӷ�̬��
MOSAIC_API int _stdcall MOSAIC_Init();

//2 �ͷ�ƴ�Ӷ�̬��
MOSAIC_API int _stdcall MOSAIC_Close();

//3 ƴ�ӽӿ��Ƿ��ṩ�ж�ͼ��Ϊָ�Ƶĺ���
MOSAIC_API int _stdcall MOSAIC_IsSupportIdentifyFinger();

//4 ƴ�ӽӿ��Ƿ��ṩ�ж�ͼ�������ĺ���
MOSAIC_API int _stdcall MOSAIC_IsSupportImageQuality();

//5 ƴ�ӽӿ��Ƿ��ṩ�ж�ָ�������ĺ���
MOSAIC_API int _stdcall MOSAIC_IsSupportFingerQuality();

//6 �ӿ��Ƿ��ṩƴ��ָ�Ƶ�ͼ����ǿ����
MOSAIC_API int _stdcall MOSAIC_IsSupportImageEnhance();

//7 �ж��Ƿ�֧�ֹ����ɼ�����
MOSAIC_API int _stdcall MOSAIC_IsSupportRollCap();

//8 ѡ��ƴ�ӷ�ʽ�ĺ���
MOSAIC_API int _stdcall MOSAIC_SetRollMode(int nRollMode);

//9 ��ʼ��ƴ�ӹ���
MOSAIC_API int _stdcall MOSAIC_Start(unsigned char* pFingerBuf, int nWidth, int nHeight);

//10 ƴ�ӹ���
MOSAIC_API int _stdcall MOSAIC_DoMosaic(unsigned char* pDataBuf, int nWidth, int nHeight);

//11 ����ƴ��
MOSAIC_API int _stdcall MOSAIC_Stop();

//12 �ж�ͼ������
MOSAIC_API int _stdcall MOSAIC_ImageQuality(unsigned char* pDataBuf, int nWidlh, int nHeight);

//13 �ж�ָ������
MOSAIC_API int _stdcall MOSAIC_FingerQuality(unsigned char* pDataBuf, int nWidth, int nHeight);

//14 ��ͼ�������ǿ
MOSAIC_API int _stdcall MOSAIC_ImageEnhance(unsigned char* pSrcImg, int nWidth, int nHeight, unsigned char* pTargetImg);

//15 �ж�ͼ���Ƿ�Ϊָ��
MOSAIC_API int _stdcall MOSAIC_IsFinger(unsigned char* pDataBuf, int nWidth, int nHeight);

//16 ȡ��ƴ�ӽӿڴ�����Ϣ
MOSAIC_API int _stdcall MOSAIC_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

//17 ȡ�ýӿڵİ汾
MOSAIC_API int _stdcall MOSAIC_GetVersion();

//18 ���ƴ�ӽӿڵ�˵��
MOSAIC_API int _stdcall MOSAIC_GetDesc(char pszDesc[1024]);

#endif