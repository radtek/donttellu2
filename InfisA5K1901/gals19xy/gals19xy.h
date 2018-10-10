/**
 * @file	gals19xy.h
 * @brief	ָ���Ʋɼ��ǽӿڹ淶���˵��
 * @author	����������ָ�Ƽ����޹�˾
 * @date	2010/10/24 17:37:32
 * @note	�����˻���ָ�ơ����ӻ��ա�����֤�ȹ淶�Ľӿڱ�׼
 * @copyright	��Ȩ����(@) 1996-2010
 *				����������ָ�Ƽ����޹�˾
 */

#ifndef _INC_GALS19XY_H_20101024173818_
#define _INC_GALS19XY_H_20101024173818_

//!< �������淶����
enum { GASTD_LIVESCAN, GASTD_PASSPORT, GASTD_IDFP };


//!< ����������ָ�Ʋɼ��ӿڱ�׼���йزɼ��������õķ���ֵ
enum
{
	GASTD_CAPTWND_NONE = 0,		//!< �ɼ��ӿڲ�֧�����òɼ�����
	GASTD_CAPTWND_SINGLE,		//!< �ɼ��ӿ�ֻ֧�ֵ�ָָ�Ʋɼ����ڵ�����
	GASTD_CAPTWND_FOURFINGER,	//!< �ɼ��ӿ�ֻ֧��ƽ����������ָ��ƽ����������ָ��ƽ��������Ĵָָ�Ʋɼ����ڵ�����
	GASTD_CAPTWND_ALL			//!< �ɼ��ӿ�֧�ֵ�ָ��ƽ����������ָ��ƽ����������ָ��ƽ��������Ĵָָ�Ʋɼ����ڵ�����
};

//!< ָ�ơ����ơ�ƴ�ӡ���������֤�ɼ��ӿڵĵ���Լ��
#define GF_GALSCAPT_API		int __stdcall
#define GF_PALMCAPT_API		int __stdcall
#define GF_GAMOSAIC_API		int __stdcall

//!< �������뾳������������֤��ָ�Ʋɼ��豸�ӿڹ淶�����ĵ���Լ��
#define GF_ALGORITHM_API		long __stdcall


//!< ������ָ�ơ����ơ�ƴ�ӱ�׼�ӿ�

#ifdef	__cplusplus
extern	"C" {
#endif

	//!< ������׼���еĽӿڣ�5��
	GF_GALSCAPT_API		LIVESCAN_Init();
	GF_GALSCAPT_API		LIVESCAN_Close();
	GF_GALSCAPT_API		LIVESCAN_GetChannelCount();
	GF_GALSCAPT_API		LIVESCAN_GetDesc(char pszDesc[1024]);
	GF_GALSCAPT_API		LIVESCAN_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	//!< ����ָ�ơ���������֤�����ӻ��գ�10��
	GF_GALSCAPT_API		LIVESCAN_SetBright(int nChannel, int nBright);
	GF_GALSCAPT_API		LIVESCAN_SetContrast(int nChannel, int nContrast);
	GF_GALSCAPT_API		LIVESCAN_GetBright(int nChannel, int* pnBright);
	GF_GALSCAPT_API		LIVESCAN_GetContrast(int nChannel, int* pnContrast);
	GF_GALSCAPT_API		LIVESCAN_Setup();
	GF_GALSCAPT_API		LIVESCAN_IsSupportSetup();
	GF_GALSCAPT_API		LIVESCAN_BeginCapture(int nChannel);
	GF_GALSCAPT_API		LIVESCAN_GetFPRawData(int nChannel, unsigned char* pRawData);
	GF_GALSCAPT_API		LIVESCAN_EndCapture(int nChannel);
	GF_GALSCAPT_API		LIVESCAN_GetVersion();

	//!< ����ָ�ơ���������֤��4��
	GF_GALSCAPT_API		LIVESCAN_GetMaxImageSize(int nChannel, int* pnWidth, int* pnHeight);
	GF_GALSCAPT_API		LIVESCAN_GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	GF_GALSCAPT_API		LIVESCAN_SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	GF_GALSCAPT_API		LIVESCAN_IsSupportCaptWindow(int nChannel);

	//!< ����ָ��ͼ��ɼ��ӿڣ�3��
	GF_GALSCAPT_API		LIVESCAN_GetPreviewImageSize(int nChannel, int *pnWidth,int *pnHeight);
	GF_GALSCAPT_API		LIVESCAN_GetPreviewData(int nChannel, unsigned char *pRawData);
	GF_GALSCAPT_API		LIVESCAN_IsSupportPreview();

	//!< ����֤�����ӻ��ա���������֤��1��
	GF_GALSCAPT_API		LIVESCAN_GetFPBmpData(int nChannel, unsigned char* pBmpData);

	//!< ���ӻ���(����3��ָ�ƱȶԽӿ�)��4��
	GF_GALSCAPT_API		LIVESCAN_PASSPORT_SetVideoWindow(HWND hWnd);

	GF_ALGORITHM_API	LIVESCAN_Get_Version(char *pcVersionInfo);
	GF_ALGORITHM_API	LIVESCAN_FeatureExtractMem(unsigned char *pcImageData, long lDataLen, unsigned char *pcTemplateData);
	GF_ALGORITHM_API	LIVESCAN_FeatureMatchMem(unsigned char* pcTemplateDataA, unsigned char *pcTemplateDataB, float *pfSim);

	//!< ��������֤��5��
	GF_GALSCAPT_API		LIVESCAN_GetCode(char pszCode[4]);
	GF_GALSCAPT_API		LIVESCAN_IDFP_SetVideoWindow(int nChannel, HWND hWnd);
	GF_GALSCAPT_API		LIVESCAN_CancelSetVideoWindow(int nChannel);
	GF_GALSCAPT_API		LIVESCAN_AutoGetFPBmpData(int nChannel, unsigned char* pBmpData,int nTimeOut);
	GF_GALSCAPT_API		LIVESCAN_CancelAutoGetFPBmpData(int nChannel);

	//!< ����֤
	GF_GALSCAPT_API		LIVESCAN_SetBufferEmpty(unsigned char *pImageData, long imageLength);

	typedef int	(__stdcall *	GFLS_LIVESCAN_INIT)();
	typedef int	(__stdcall *	GFLS_LIVESCAN_CLOSE)();
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETCHANNELCOUNT)();
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETDESC)(char pszDesc[1024]);
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);

	typedef int	(__stdcall *	GFLS_LIVESCAN_SETBRIGHT)(int nChannel, int nBright);
	typedef int	(__stdcall *	GFLS_LIVESCAN_SETCONTRAST)(int nChannel, int nContrast);
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETBRIGHT)(int nChannel, int* pnBright);
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETCONTRAST)(int nChannel, int* pnContrast);
	typedef int	(__stdcall *	GFLS_LIVESCAN_SETUP)();
	typedef int	(__stdcall *	GFLS_LIVESCAN_ISSUPPORTSETUP)();
	typedef int	(__stdcall *	GFLS_LIVESCAN_BEGINCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETFPRAWDATA)(int nChannel, unsigned char* pRawData);
	typedef int	(__stdcall *	GFLS_LIVESCAN_ENDCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETVERSION)();

	typedef int	(__stdcall *	GFLS_LIVESCAN_GETMAXIMAGESIZE)(int nChannel, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_LIVESCAN_GETCAPTWINDOW)(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_LIVESCAN_SETCAPTWINDOW)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	typedef int	(__stdcall *	GFLS_LIVESCAN_ISSUPPORTCAPTWINDOW)(int nChannel);

	typedef	int	(__stdcall *	GFLS_LIVESCAN_GETPREVIEWIMAGESIZE)(int nChannel, int *pnWidth, int *pnHeight);
	typedef	int	(__stdcall *	GFLS_LIVESCAN_GETPREVIEWDATA)(int nChannel, unsigned char *pRawData);
	typedef	int (__stdcall *	GFLS_LIVESCAN_ISSUPPORTPREVIEW)(void);

	typedef int	(__stdcall *	GFLS_LIVESCAN_GETFPBMPDATA)(int nChannel, unsigned char* pBmpData);

	typedef int	(__stdcall *	GFLS_LIVESCAN_PASSPORT_SETVIDEOWINDOW)(HWND hWnd);

	typedef long (__stdcall *	GFLS_LIVESCAN_GET_VERSION)(char *pcVersionInfo);
	typedef long (__stdcall *	GFLS_LIVESCAN_FEATUREEXTRACTMEM)(unsigned char *pcImageData, long lDataLen, unsigned char *pcTemplateData);
	typedef long (__stdcall *	GFLS_LIVESCAN_FEATUREMATCHMEM)(unsigned char* pcTemplateDataA, unsigned char *pcTemplateDataB, float *pfSim);

	typedef long (__stdcall *	GFLS_LIVESCAN_GETCODE)(char pszCode[4]);
	typedef long (__stdcall *	GFLS_LIVESCAN_IDFP_SETVIDEOWINDOW)(int nChannel, HWND hWnd);
	typedef long (__stdcall *	GFLS_LIVESCAN_CANCELSETVIDEOWINDOW)(int nChannel);
	typedef long (__stdcall *	GFLS_LIVESCAN_AUTOGETFPBMPDATA)(int nChannel, unsigned char* pBmpData,int nTimeOut);
	typedef long (__stdcall *	GFLS_LIVESCAN_CANCELAUTOGETFPBMPDATA)(int nChannel);

	typedef long (__stdcall *	GFLS_LIVESCAN_SETBUFFEREMPTY)(unsigned char *pImageData, long imageLength);


	//!< ����ͼ��ƴ��Ӧ�ó���ӿ�
	GF_GAMOSAIC_API		MOSAIC_Init();
	GF_GAMOSAIC_API		MOSAIC_Close();
	GF_GAMOSAIC_API		MOSAIC_IsSupportIdentifyFinger();
	GF_GAMOSAIC_API		MOSAIC_IsSupportImageQuality();	
	GF_GAMOSAIC_API		MOSAIC_IsSupportFingerQuality();
	GF_GAMOSAIC_API		MOSAIC_IsSupportImageEnhance();
	GF_GAMOSAIC_API		MOSAIC_IsSupportRollCap();
	GF_GAMOSAIC_API		MOSAIC_SetRollMode(int nRollMode);
	GF_GAMOSAIC_API		MOSAIC_Start(unsigned char* pFingerBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_DoMosaic(unsigned char* pDataBuf, int nWidht, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_Stop();
	GF_GAMOSAIC_API		MOSAIC_ImageQuality(unsigned char* pDataBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_FingerQuality(unsigned char* pDataBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_ImageEnhance(unsigned char* pSrcImg, int nWidth, int nHeight, unsigned char* pTargetImg);
	GF_GAMOSAIC_API		MOSAIC_IsFinger(unsigned char* pDataBuf, int nWidth, int nHeight);
	GF_GAMOSAIC_API		MOSAIC_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);
	GF_GAMOSAIC_API		MOSAIC_GetVersion();
	GF_GAMOSAIC_API		MOSAIC_GetDesc(char pszDesc[1024]);

	typedef	int (__stdcall *	GFLS_MOSAIC_INIT)();
	typedef	int (__stdcall *	GFLS_MOSAIC_CLOSE)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTIDENTIFYFINGER)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTIMAGEQUALITY)();	
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTFINGERQUALITY)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTIMAGEENHANCE)();
	typedef	int (__stdcall *	GFLS_MOSAIC_ISSUPPORTROLLCAP)();
	typedef	int (__stdcall *	GFLS_MOSAIC_SETROLLMODE)(int nRollMode);
	typedef	int (__stdcall *	GFLS_MOSAIC_START)(unsigned char* pFingerBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_DOMOSAIC)(unsigned char* pDataBuf, int nWidht, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_STOP)();
	typedef	int (__stdcall *	GFLS_MOSAIC_IMAGEQUALITY)(unsigned char* pDataBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_FINGERQUALITY)(unsigned char* pDataBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_IMAGEENHANCE)(unsigned char* pSrcImg, int nWidth, int nHeight, unsigned char* pTargetImg);
	typedef	int (__stdcall *	GFLS_MOSAIC_ISFINGER)(unsigned char* pDataBuf, int nWidth, int nHeight);
	typedef	int (__stdcall *	GFLS_MOSAIC_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);
	typedef	int (__stdcall *	GFLS_MOSAIC_GETVERSION)();
	typedef	int (__stdcall *	GFLS_MOSAIC_GETDESC)(char pszDesc[1024]);

	//!< ��������ͼ��ɼ��ӿ�
	GF_PALMCAPT_API		PALM_Init();
	GF_PALMCAPT_API		PALM_Close();
	GF_PALMCAPT_API		PALM_GetChannelCount();
	GF_PALMCAPT_API		PALM_SetBright(int nChannel, int nBright);
	GF_PALMCAPT_API		PALM_SetContrast(int nChannel, int nContrast);
	GF_PALMCAPT_API		PALM_GetBright(int nChannel, int* pnBright);
	GF_PALMCAPT_API		PALM_GetContrast(int nChannel, int* pnContrast);
	GF_PALMCAPT_API		PALM_GetMaxImageSize(int nChannel, int* pnWidth, int* pnHeight);
	GF_PALMCAPT_API		PALM_GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	GF_PALMCAPT_API		PALM_SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	GF_PALMCAPT_API		PALM_Setup();
	GF_PALMCAPT_API		PALM_BeginCapture(int nChannel);
	GF_PALMCAPT_API		PALM_GetFPRawData(int nChannel, unsigned char* pRawData);
	GF_PALMCAPT_API		PALM_EndCapture(int nChannel);
	GF_PALMCAPT_API		PALM_IsSupportCaptWindow(int nChannel);
	GF_PALMCAPT_API		PALM_IsSupportSetup();
	GF_PALMCAPT_API		PALM_GetPreviewImageSize(int nChannel,int *pnWidth,int *pnHeight);
	GF_PALMCAPT_API		PALM_GetPreviewData(int nChannel, unsigned char* pRawData );
	GF_PALMCAPT_API		PALM_IsSupportPreview();
	GF_PALMCAPT_API		PALM_GetVersion();
	GF_PALMCAPT_API		PALM_GetDesc(char pszDesc[1024]);
	GF_PALMCAPT_API		PALM_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	typedef int	(__stdcall *	GFLS_PALM_INIT)();
	typedef int	(__stdcall *	GFLS_PALM_CLOSE)();
	typedef int	(__stdcall *	GFLS_PALM_GETCHANNELCOUNT)();
	typedef int	(__stdcall *	GFLS_PALM_SETBRIGHT)(int nChannel, int nBright);
	typedef int	(__stdcall *	GFLS_PALM_SETCONTRAST)(int nChannel, int nContrast);
	typedef int	(__stdcall *	GFLS_PALM_GETBRIGHT)(int nChannel, int* pnBright);
	typedef int	(__stdcall *	GFLS_PALM_GETCONTRAST)(int nChannel, int* pnContrast);
	typedef int	(__stdcall *	GFLS_PALM_GETMAXIMAGESIZE)(int nChannel, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_PALM_GETCAPTWINDOW)(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_PALM_SETCAPTWINDOW)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	typedef int	(__stdcall *	GFLS_PALM_SETUP)();
	typedef int	(__stdcall *	GFLS_PALM_BEGINCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_PALM_GETFPRAWDATA)(int nChannel, unsigned char* pRawData);
	typedef int	(__stdcall *	GFLS_PALM_ENDCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_PALM_ISSUPPORTCAPTWINDOW)(int nChannel);
	typedef int	(__stdcall *	GFLS_PALM_ISSUPPORTSETUP)();
	typedef	int	(__stdcall *	GFLS_PALM_GETPREVIEWIMAGESIZE)(int nChannel, int *pnWidth, int *pnHeight);
	typedef	int	(__stdcall *	GFLS_PALM_GETPREVIEWDATA)(int nChannel, unsigned char *pRawData);
	typedef	int (__stdcall *	GFLS_PALM_ISSUPPORTPREVIEW)(void);
	typedef int	(__stdcall *	GFLS_PALM_GETVERSION)();
	typedef int	(__stdcall *	GFLS_PALM_GETDESC)(char pszDesc[1024]);
	typedef int	(__stdcall *	GFLS_PALM_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);


#ifdef	__cplusplus
}
#endif



/**
 * ��������׼�ӿڴ������
 */

/**
 * �������뾳������������֤��ָ�Ʋɼ��豸�ӿڹ淶�����ķ���ֵ
 */
#define GF_GALSCAPT_ERROR_SUCCESSRECTIFY	3			//�ɼ��ɹ�����ƫ�ɹ�
#define GF_GALSCAPT_ERROR_FAILRECTIFY		2			//�ɼ��ɹ�����ƫʧ��


//!< ����ָ�ơ�����ͼ��ɼ��ӿڴ������
#define GF_GALSCAPT_ERROR_SUCCESS			1
#define GF_GALSCAPT_ERROR_PARAM				-1			// �������󡣸��������Ĳ����д���
#define GF_GALSCAPT_ERROR_MEMORY			-2			// �ڴ����ʧ�ܡ�û�з��䵽�㹻���ڴ档
#define GF_GALSCAPT_ERROR_NOTIMPL			-3			// ����δʵ�֡����ú����Ĺ���û��ʵ�֡�
#define GF_GALSCAPT_ERROR_NODEVICE			-4			// �豸�����ڡ���ʼ����ʱ�򣬼�⵽�豸�����ڡ�
#define GF_GALSCAPT_ERROR_NOTINIT			-5			// �豸δ��ʼ����
#define GF_GALSCAPT_ERROR_INVALIDENO		-6			// �Ƿ��Ĵ���š�
#define GF_GALSCAPT_ERROR_NOLICENSE			-7			// û����Ȩ��
#define GF_GALSCAPT_ERROR_OTHERERR			-9			// ��������

#define GF_GALSCAPT_ERROR_INVALIDCHANNLE	-101		// ��Ч��ͨ����
#define GF_GALSCAPT_ERROR_INVALIDCAPTWIN	-102		// �Ƿ��ɼ�����
#define GF_GALSCAPT_ERROR_NOPRIVILEGE		-103        // û����Ȩ
#define GF_GALSCAPT_ERROR_FAIL			    -104        // ��������ʧ�ܣ���������ϸ����ʾ��
#define GF_GALSCAPT_ERROR_WRONG			    -105        // ���ô���
#define GF_GALSCAPT_ERROR_RECOVERYSUCCESS   -106        // �豸�����˻ָ����һָ��ɹ�
#define GF_GALSCAPT_ERROR_RECOVERYFAIL      -107        // �豸�����˻ָ������ָ�ʧ����
#define GF_GALSCAPT_ERROR_BADFRAME          -108        // ����UC500C��ĳЩ�������ã�RowSize, ColumnSize, RowSkip, ColumnSkip����
														//�������ŵ�һ֡������Ч֡
#define GF_GALSCAPT_ERROR_INVALIDFRAME      -109		// ���ݴ������д���ͼ���в������ݶ�ʧ��û��ͼ��
														// ���ݣ����ش�ֵ���������豸������������
#define GF_GALSCAPT_ERROR_VIOLATION         -110        //�˺����ĵ��������������г�ͻ

#define GF_GALSCAPT_ERROR_UNKNOWN			-111		// δ֪����

#define GF_GALSCAPT_ERROR_BADQUALITY		-112		//!< �ɼ�ָ�Ƶ�ͼ��������


//!< ����ͼ��ƴ��Ӧ�ó���ӿڴ������
#define GF_GAMOSAIC_ERROR_SUCCESS			1
#define GF_GAMOSAIC_ERROR_PARAM				-1			// �������󡣸��������Ĳ����д���
#define GF_GAMOSAIC_ERROR_MOMERY			-2			// �ڴ����ʧ�ܡ�û�з��䵽�㹻���ڴ�
#define GF_GAMOSAIC_ERROR_NOTIMPL			-3			// ����δʵ�֡����ú����Ĺ���û��ʵ�֡�
#define GF_GAMOSAIC_ERROR_INVALIDENO		-6			// �Ƿ������
#define GF_GAMOSAIC_ERROR_NOLICENSE			-7			// û����Ȩ
#define GF_GAMOSAIC_ERROR_NOTINIT			-8			// ƴ��δ��ʼ��

#define GF_GAMOSAIC_ERROR_INVALIDROLLMODE	-101		// ��Ч�Ĺ�����ʽ
#define GF_GAMOSAIC_ERROR_TOOQUICK			-102		//�����ٶ�̫��
#define GF_GAMOSAIC_ERROR_ROLLBACK			-103		//����Ȼع�
#define GF_GAMOSAIC_ERROR_DISLOCATION		-104		//���ڻع�������ӡ���ε������ߴ�λ

#define	GF_GAMOSAIC_ERROR_OVERTIME			-151	// ������ʱ��
#define	GF_GAMOSAIC_ERROR_MEMERROR			-152	// �ڴ����⡣


/**
 * ��������׼ָ�ơ����ƴ�С
 */
#define  GALS_ALLPALM_WIDTH			2304
#define  GALS_ALLPALM_HEIGHT		3584

#define  GALS_PALMSUB_WIDTH			2304
#define  GALS_PALMSUB_HEIGHT		2304

#define  GALS_WRITERPALM_WIDTH		1024
#define  GALS_WRITERPALM_HEIGHT		1968

#define GALS_FOURFINGER_WIDTH		1600
#define GALS_FOURFINGER_HEIGHT		1500

#define GALS_FINGER_WIDTH			640
#define GALS_FINGER_HEIGHT			640 


#endif	// _INC_GALS19XY_H_20101024173818_