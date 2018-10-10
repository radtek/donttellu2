/**
 * @file	galsutil19.h
 * @brief	�������뾳�����������֤�����������ָ֤�Ʋɼ��豸�ӿڹ淶��ظ��������Ͷ���
 * @author	����������ָ�Ƽ����޹�˾
 * @date	2010/12/27 11:32:44
 * @note	
 * @copyright	��Ȩ����(@) 1996-2010
 *				����������ָ�Ƽ����޹�˾
 */

#include "../gals19xy/gals19xy.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _INC_GALSUTIL19_H_20101227113513_
#define _INC_GALSUTIL19_H_20101227113513_

//!< �������뾳�����������֤��ָ������ͼ���С
#define GFLS_PASSPORT_WIDTH			400
#define GFLS_PASSPORT_HEIGHT		400

//!< �����������ָ֤������ͼ���С
#define GFLS_IDFP_WIDTH			256
#define GFLS_IDFP_HEIGHT		360
#define GFLS_IDFP_GOODQUALITY	80		//!< ����ָ������
#define GFLS_IDFP_ACCEPTQUALITY	70		//!< ���Խ��ܵ�ָ������

#define GFLS_MNTVERSION			"1901"
#define GFLS_IDFP_MNTVERSION	"0410"
#define GFLS_VERSION			100

//!< ��˾�ڲ�ʹ��ʱ�����ֵ�����ڲ�ʹ��ע�͵�,add by xff 2012-12-19
//!< Ŀ�ģ���Ϊ��������ʱ��Ҫ��ֵ��Ŀ�����ӱ�׼DLL�е�2��������ŵĽӿ�
//#define BUILDGALSFORLOCALGAFIS

//!< 256���Ҷ�ͼ��ͷ
typedef struct tagUTIL19_256BitmapInfo 
{
	BITMAPINFOHEADER	bmiHeader; 
	RGBQUAD				bmiColors[256]; 
} UTIL19_256BITMAPINFO;


/**
 * ͼ��ɼ���Ԥ������ȡ�����ͱȶ��õ�����ز���
 */
typedef	struct	tagUTIL19_HelpData
{
	CRITICAL_SECTION	stCrit;		//!< ͬ��

	HWND	hWndPreview;		//!< Ԥ�����ھ��
	HANDLE	hThreadPreview;		//!< Ԥ���߳̾��
	HANDLE	hThreadGetFPBmp;	//!< �Զ��ɼ�һָ֡��ͼ�����ݵ��߳̾����Ŀǰ�������ָ֤�Ʋɼ�SDK	

	UCHAR	*pDataBuffer;	//!< ������Ųɼ�ͼ���Buffer
	UCHAR	*pPreviewData;	//!< �������Ԥ��ͼ���Buffer
	UCHAR	*pCaptureData;	//!< ������Ųɼ�һ֡ͼ���Buffer
	int		nDataBuffer;	//!< pDataBuffer�Ĵ�С
	int		nFrameBuffer;	//!< pPreviewData��pCaptureData�������Ĵ�С

	UCHAR	*pFingerData;	//!< ������Ŵ���ȡ������ָ�����ݣ����С�ǲ�ȷ����
	UCHAR	*pOneLineData;	//!< �������һ��ͼ������
	int		nFingerData;	//!< pFingerData�Ĵ�С
	int		nOneLineData;

	/**
	 * �ɼ��ǲɼ�����ԭʼͼ��Ĵ�С
	 */
	int		nWidth, nHeight;

	UCHAR	bCritInited;	//!< stCrit�Ƿ��Ѿ���ʼ��	
	UCHAR	bClosePreview;	//!< �Ƿ���Ҫ�ر�Ԥ��
	UCHAR	bHelpDataInit;
	UCHAR	bCloseGetFPBmp;	//!< �Ƿ�ֹͣ�Զ��ɼ�һ��ָ��ͼ��

	//!< ��ʾԤ��ͼ����
	UTIL19_256BITMAPINFO	st256BmpInfo;
	RECT	rtImage;

	FILE	*fpLog;				//!< �����õ����log
}UTIL19_HELPDATA;

/**
 * ͼ���ƫ��Ϣ
 */
typedef struct tagUTIL19_RegionRect
{
	int x;				//	�������������������Ͻ�λ�ã����ͼ�󣩣�
	int y;				
	int beExist;		//	����������ڱ�ǣ�0...�����ڣ�1...����
	int nAngle;			//	���������ת�Ƕ�  ��ֱ������0�ȣ��ұ������Ƕȣ�����Ǹ��Ƕȣ�ֻ�Ƕ�ƽ����ӡָ������
	int nWidth;			//	�����������������
	int nHeight;		//	��������������󳤶�
	int nObjectType;	//	������������������ͣ�(1-10Ϊ����ʮָ��21-30Ϊƽ��ʮָ��0��ʾָλδ֪)
} UTIL19_REGIONRECT;


#ifdef	__cplusplus
extern	"C" {
#endif


//typedef	UTIL19_REGIONRECT	REGIONRECT;
//!< ������LiveApp.dll���ṩ������API����

//�ж�ƽ��ָ����ӡ����ͷ��������ӡ�����޷��жϣ�
//��������ֵ,    1....		�жϳɹ�����pDestObj�ṹ�з��زɼ�������Ͻ�λ�ú�ָ�Ʒ��򣨴�ֱ����Ϊ0�ȣ��Ƕ���������
//				-1....		��ӡ�����޷��ж�ʱ�򣬸�����ӡ�����������pDestObj�з��زɼ�������Ͻ�λ��
//				KERROR_XXX..��������
//���ú���֮ǰ����Ҫ��pDestObj�и��ɼ����Ⱥ͸߶ȸ�ֵ
//pDestObj��ָλ��ϢnObjectType ��ֵ21...30, -1��ʾָλδ֪
/*extern "C"*/	int PlainImage_Splite(unsigned char *pImage, int nWidth, int nHeight, UTIL19_REGIONRECT *pDestObj);

//�ж�ָ������ӡ������Ϣ
//��������ֵ��	>=0  ����Ĵ�С
//				< 0  ������Ϣ KERROR_***
/*extern "C"*/	int FPImage_AreaInfo(unsigned char *pImage,		//	�����ͼ��ָ��
									 int nWidth,				//	ͼ��Ŀ��
									 int nHeight,				//	ͼ��ĸ߶�
									 UTIL19_REGIONRECT *pDestObj);		//	���ص�ͼ����Ӿ�����Ϣ�����Ͻ�λ�ú;��εĿ�ȸ߶�



/**
 * �ɹ�����1�����򷵻�-1
 */
int		UTIL19_InitHelpData(UTIL19_HELPDATA *pstHelpData, int nWidth, int nHeight);
int		UTIL19_InitHelpDataEx(UTIL19_HELPDATA *pstHelpData, int nMaxWidth, int nMaxHeight, int nCaptWidth, int nCaptHeight);

void	UTIL19_UnInitHelpData(UTIL19_HELPDATA *pstHelpData);
void	UTIL19_StopPreviewThread(UTIL19_HELPDATA *pstHelpData);
void	UTIL19_StopGetFPBmpThread(UTIL19_HELPDATA *pstHelpData);

/**
 * ���nWantBuffer>*pnCurBuffer�������·����ڴ�
 * �ɹ������ԭ����*ppBuffer�����ͷţ�*pnCurBuffer=nWantBuffer
 */
int		UTIL19_ReMallocBuffer(UCHAR **ppBuffer, int *pnCurBuffer, int nWantBuffer);

void	UTIL19_VMirrorData(UCHAR *pBitData, int nWidth, int nHeight, UCHAR *pOneLineData);

/**
 * �ú���ֻ�ǰ�pBmpFileData��ǰ1078���ֽڷ�װ��bmp�ļ��ģ�BITMAPFILEHEADER��BITMAPINFOHEADER��RGBQUAD����ɫ��
 */
void	UTIL19_SaveToBmpFileFormat(UCHAR *pBmpFileData, int nWidth, int nHeight);

void	UTIL19_GetCurVideoWndRect(UTIL19_HELPDATA *pstHelpData);
void	UTIL19_DrawVideoPreview(UTIL19_HELPDATA *pstHelpData);

int		UTIL19_ImageDeviationRectify(UCHAR *pCaptureData, int nwCapture, int nhCapture, UCHAR *pRectifyData, int nwClip, int nhClip);

/**
 * �ڵ��øú���ǰ��pstRectifyInfo��nWidth��nHeight����ָ�������վ�ƫ��ͼ��Ŀ�Ⱥ͸߶�
 *	pstRectifyInfo��nObjectType����ָ����ƫ��ָλ��������Ϣ��ο�UTIL19_REGIONRECT�Ķ���
 */
int		UTIL19_GetImageRectifyInfo(UCHAR *pCaptureData, int nwCapture, int nhCapture, UTIL19_REGIONRECT *pstRectifyInfo);


/**
 * ����pstRectifyInfo��pCaptureData�еõ���ƫ���ͼ������
 * pRectifyData�����ɵ����߷��䣬���С����С��pstRectifyInfo�еĿ��x�߶�
 */
int		UTIL19_GetRectifyImageData(UCHAR *pCaptureData, int nwCapture, int nhCapture, 
								   UCHAR *pRectifyData, UTIL19_REGIONRECT *pstRectifyInfo);

/**
 * ����������������еĲ����롶�������뾳�����������֤��ָ�Ʋɼ��豸�ӿڹ淶���ж�Ӧ����
 * ����ȡ���������ͱȶԺ������е�һ�£�pstHelpData���⣩
 */
int		UTIL19_FeatureExtractMem(UCHAR *pcImageData, long lDataLen, UCHAR *pcTemplateData, UTIL19_HELPDATA *pstHelpData);
int		UTIL19_FeatureMatchMem(unsigned char* pcTemplateDataA, unsigned char *pcTemplateDataB, float *pfSim);

/**
 * ����ָ����ͼ��������ȡ����
 * @pImageData��8λ�Ҷ�ͼ������
 * @pbnMntData������������ݵ�Buffer�������߸�������㹻��Ŀռ�
 * @pnMntSize����ȡ�������Ĵ�С
 * @����ֵ��>0��ָ������ֵ[1,100]��0������ָ�ƣ�<0 ʧ��
 */
int		UTIL19_ExtractFeature(UCHAR *pImageData, int nWidth, int nHeight, UCHAR *pbnMntData, int *pnMntSize);

/**
 * ����ָ����ͼ��������ȡ����
 * @pImageData��8λ�Ҷ�ͼ������
 * @pbnMntData������������ݵ�Buffer�������߸�������㹻��Ŀռ�
 * @pnMntSize����ȡ�������Ĵ�С
 * @pnInnerError���ڲ������
 * @����ֵ��>0��ָ������ֵ[1,100]��<=0 ʧ��
 */
int		UTIL19_ExtractFeatureEx(UCHAR *pImageData, int nWidth, int nHeight, UCHAR *pbnMntData, int *pnMntSize, int *pnInnerError);


int		UTIL19_GetMaxFeatureSize();

#define	UTIL19_MATCHSCORE_MAXSCORE		9999
int		UTIL19_MatchFeature(UCHAR *pbnTemplateA, UCHAR *pbnTemplateB, int *pnScore);

/**
 * ���ݹ�������׼������룬������Ӧ�Ĵ�������
 * @nGAStdType��gals19xy.h�ж���Ĺ������淶����
 */
int		UTIL19_GetGAStdErrorInfo(int nErrorNo, char *pszErrorInfo, int ncbErrorInfo, int nGAStdType);

#ifdef	__cplusplus
}
#endif

#endif	// _INC_GALSUTIL19_H_20101227113513_