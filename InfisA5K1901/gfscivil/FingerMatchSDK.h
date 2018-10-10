/**
 * �ļ���  ��	FingerMatchSDK.h
 * ��˾���ƣ�	����������ָ�Ƽ����޹�˾
 * ����    ��	����ʹ�õĺꡢ�ṹ��,�����ӿ�
 * �汾��  ��	 6.2.1.1
 * ��Ȩ    ��	��Ȩ���С�(C)��2000��2008  ����������ָ�Ƽ����޹�˾
*/

#ifndef _INC_FINGERMATCHSDK_H
#define _INC_FINGERMATCHSDK_H

#ifdef FINGERMATCHSDK_EXPORTS
#define FINGERMATCHSDK_API __declspec(dllexport)
#else
#define FINGERMATCHSDK_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define  GAFIS_OPERATE_OK					0       //  �����ɹ�

#define  GAIFS_DOG_NOTFIND                  -1      //  û���ҵ����ܹ�
#define  GAFIS_DOG_VERIFYPASSWORDFALSE	    -2      //  ���ܹ���ȡ�û�����ʧ��
#define  GAFIS_DOG_READPRODUCTFALSE         -3      //  ���ܹ���ȡ��Ʒ��Ϣʧ��
#define  GAFIS_DOG_VISITLICENSEFALSE        -4      //  �������֤ʧ����Ȩ����
	
#define	 GAFIS_FINGERIMG_NULL				-5		//��ָ��ͼ������Ϊ��
#define  GAFIS_FINGERIMG_SMALL				-6  	//��ָ��ͼ��̫С
#define  GAFIS_FINGERIMG_BIG				-7      //��ָ��ͼ��̫��
#define GAFIS_FINGER_NOTBMPFORMAT			-8		//  ��BMP��ʽͼ������
#define GAFIS_FINGER_NOT24BITCOUNT			-9		//  ��24λͼ
#define GAFIS_FINGER_COMPRESS				-10		//  ͼ��Ϊѹ��ͼ
#define GAFIS_FINGER_PELSSMALL				-11		//  �ֱ��ʲ���
#define GAFIS_FINGER_PLANES					-12		//  Ŀ���豸�ļ���Ϊ1
#define GAFIS_FINGER_BMPDATAERROR			-13		//  �����ͼ������
#define GAFIS_FINGERIMG_NONOMAL				-14		//	��768*576��С������

#define  GAFIS_FINGERIMG_POORQUALITY		-15		//  ͼ������̫��
#define	 GAFIS_FINGERPOS_ERROR				-16     //  �ɼ�����ָ��ͼ�񲻾���
#define  GAFIS_FINGERIMG_WHITEIMG			-17		//  ͼ��Ϊ�հ�ͼ
#define  GAFIS_FINGERIMG_NOENOUGHMINUATIAE  -18		//  ��ȡ��ָ����������
#define  GAFIS_FINGERIMG_BITCOUNTERROR		-19		//	ͼ��λ�����ô���
#define	 GAFIS_OPERATE_ERROR_PARAM			-20	    //  �������ô���

#define  GAFIS_RV_DATAERROR					-21		//	�Ƿ���������
#define	 GAFIS_INITIAL_CREATERROR			-22		//	����ʶ������ʧ��
#define  GAFIS_INITIAL_SETGETARUERROR		-23		//	��ȡ�������ò���ʧ��
#define	 GAFIS_INITIAL_GETFEATURESIZEERROR  -24		//	��ȡ�����������ֵʧ��
#define	 GAFIS_RV_MALLOCERROR				-25		//	�����ڴ�ʧ��
#define  GAFIS_RV_SETMNTDATAERROR			-26		//	����ָ������ʧ��
#define  GAFIS_RV_IOERROR					-27		//	IO����ʧ��
#define	 GAFIS_INITIAL_BEYONDDATE			-28		//  �Ѿ�������Ȩ����
#define  GAFIS_OPERATE_ROATEERROR			-29     //  ��תָ��ͼ��180�Ȳ��ɹ�
#define FEATURE_MAXSIZE						656		//ָ�������������ֵ

typedef struct tagFingerFeatureData
{
	unsigned char	szFeatureData[FEATURE_MAXSIZE];
	int			    nDataLength;
}FINGERFEATUREDATA;


/******************************************************************************
��������: GAFIS_InitialSystem
��������: ����ʶ�����溯��.���������ӿ�ǰҪ�ȵ��ô˺��� 
��  ��ֵ: ���շ���ֵ˵��
����˵��: ��
******************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_InitialSystem();


/*************************************************************************************
��������: GAFIS_MatchEx
��������: ����ƥ�亯��.���ô˺���ǰ���ȵ���GAFIS_SetMNTFromDB���п����������
��  ��ֵ: ���շ���ֵ˵��
����˵��: 
	����1: *pInImg					ָ��ͼƬ�ļ�����
	����2��*pnScore					���ص���ߵ÷�
***************************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_MatchEx(unsigned char *pInImg, 
											   int *pnScore
											  );


/***************************************************************************************
��������: GAFIS_SetMNTFromDB
��������: ����������ú���
��  ��ֵ: ���շ���ֵ˵��
����˵��:
    ����1: *pFeatureOld 			ָ����������;
	����2: nCount					ָ�Ƹ���;
****************************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_SetMNTFromDB(FINGERFEATUREDATA *pFeatureOld,
													int nCount
													);


/*************************************************************************************
��������: GAFIS_ExtractMnt
��������: ������ȡ����.
��  ��ֵ: ���շ���ֵ˵��
����˵��: 
	����1: *pInImg					ָ��ͼƬ�ļ����ݣ���ʽ��GAFIS_MatchEx�е�pInImgһ��
	����2��*pFeature				���ص��������ݣ����е�nDataLength��ʾ��szFeatureData�������ݵĳ���
***************************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_ExtractMnt(unsigned char *pInImg, 
												  FINGERFEATUREDATA *pFeature
												  );


#ifdef __cplusplus
}
#endif

#endif