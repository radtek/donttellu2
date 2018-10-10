/**
 * @file	gfsCustom.h
 * @brief	����ͨ����Ŀͼ��ɼ���������ȡ��ָ�ƱȶԽӿڹ淶
 * @author	����������ָ�Ƽ����޹�˾
 * @date	2011/04/02 16:33:10
 * @note	
 * @copyright	��Ȩ����(@) 1996-2011
 *				����������ָ�Ƽ����޹�˾
 */

#ifndef _INC_GFSCUSTOM_H_20110402163458_
#define _INC_GFSCUSTOM_H_20110402163458_


/**
 * ָ�ơ����ơ�ƴ�ӽӿڵĵ���Լ��
 */
#define GFS_CUSTOM_API		int __stdcall


/**
 * �ӿ�
 */

#ifdef	__cplusplus
extern	"C" {
#endif

	//!< ����ָ��ͼ��ɼ��ӿ�
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


//!< �ӿڴ������
#define GFS_CUSTOM_ERROR_SUCCESS			1		//!< �������óɹ�
#define GFS_CUSTOM_ERROR_PARAM				-1		// �������󡣸��������Ĳ����д���
#define GFS_CUSTOM_ERROR_MEMORY				-2		// �ڴ����ʧ�ܡ�û�з��䵽�㹻���ڴ档
#define GFS_CUSTOM_ERROR_NOTIMPL			-3		// ����δʵ�֡����ú����Ĺ���û��ʵ�֡�
#define GFS_CUSTOM_ERROR_INVALIDENO			-4		// �Ƿ��Ĵ���š�
#define GFS_CUSTOM_ERROR_NOLICENSE			-5		// û����Ȩ��

#define GFS_CUSTOM_ERROR_UNKNOWN			-100	// δ֪����

#define GFS_CUSTOM_ERROR_NODEVICE			-101	// �豸�����ڡ���ʼ����ʱ�򣬼�⵽�豸�����ڡ�
#define GFS_CUSTOM_ERROR_NOTINIT			-102	// �豸δ��ʼ����

#define GFS_CUSTOM_ERROR_BLANKIMAGE			-201		// ͼ��Ϊ�հ�ͼ
#define GFS_CUSTOM_ERROR_POORQUALITY		-202		// ͼ������̫��
#define GFS_CUSTOM_ERROR_TOOSMALLAREA		-203        // �ɼ�����ͼ��̫С
#define GFS_CUSTOM_ERROR_SHIFTIMAGE			-204        // ͼ��û�о��У�̫ƫ�ˣ�


#endif	// _INC_GFSCUSTOM_H_20110402163458_
