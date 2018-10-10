/**
 * @file	ls_argent.h
 * @brief	����ָ͢���Ʋɼ��ǽӿڹ淶���˵��
 * @author	����������ָ�Ƽ����޹�˾
 * @date	2012/11/19 16:45:57
 * @note	
 * @copyright	��Ȩ����(@) 1996-2012
 *				����������ָ�Ƽ����޹�˾
 */

#ifndef _INC_LS_ARGENT_H_20121119164618_
#define _INC_LS_ARGENT_H_20121119164618_

#pragma warning( disable : 4996)


//!< ����͢�淶����
enum { ARGENT_LIVESCAN, ARGENT_PASSPORT, ARGENT_IDFP };


//!< ����͢����ָ�Ʋɼ��ӿڱ�׼���йزɼ��������õķ���ֵ
enum
{
	ARGENT_CAPTWND_NONE = 0,	//!< �ɼ��ӿڲ�֧�����òɼ�����
	ARGENT_CAPTWND_SINGLE,		//!< �ɼ��ӿ�ֻ֧�ֵ�ָָ�Ʋɼ����ڵ�����
	ARGENT_CAPTWND_FOURFINGER,	//!< �ɼ��ӿ�ֻ֧��ƽ����������ָ��ƽ����������ָ��ƽ��������Ĵָָ�Ʋɼ����ڵ�����
	ARGENT_CAPTWND_ALL			//!< �ɼ��ӿ�֧�ֵ�ָ��ƽ����������ָ��ƽ����������ָ��ƽ��������Ĵָָ�Ʋɼ����ڵ�����
};

//!< ָ�ơ����ơ�ƴ�ӡ��������֤�ɼ��ӿڵĵ���Լ��
#define ARGENT_LSCAPT_API		int __stdcall
#define ARGENT_PALMCAPT_API		int __stdcall
#define ARGENT_GAMOSAIC_API		int __stdcall



//!< ����ָ͢�ơ����ơ�ƴ�ӱ�׼�ӿ�

#ifdef	__cplusplus
extern	"C" {
#endif

	ARGENT_LSCAPT_API		GFS_LIVESCAN_Init();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_Close();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetChannelCount();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_SetBright(int nChannel, int nBright);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_SetContrast(int nChannel, int nContrast);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetBright(int nChannel, int* pnBright);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetContrast(int nChannel, int* pnContrast);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetMaxImageSize(int nChannel, int* pnWidth, int* pnHeight);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetCaptWindow(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_SetCaptWindow(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_Setup();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_BeginCapture(int nChannel);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetRawData(int nChannel, unsigned char* pRawData);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_EndCapture(int nChannel);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_IsSupportCaptWindow(int nChannel);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_IsSupportSetup();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetVersion();
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetDesc(char pszDesc[1024]);
	ARGENT_LSCAPT_API		GFS_LIVESCAN_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_INIT)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_CLOSE)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETCHANNELCOUNT)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETBRIGHT)(int nChannel, int nBright);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETCONTRAST)(int nChannel, int nContrast);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETBRIGHT)(int nChannel, int* pnBright);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETCONTRAST)(int nChannel, int* pnContrast);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETMAXIMAGESIZE)(int nChannel, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETCAPTWINDOW)(int nChannel, int* pnOriginX, int* pnOriginY, int* pnWidth, int* pnHeight);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETCAPTWINDOW)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_SETUP)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_BEGINCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETRAWDATA)(int nChannel, unsigned char* pRawData);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_ENDCAPTURE)(int nChannel);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_ISSUPPORTCAPTWINDOW)(int nChannel);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_ISSUPPORTSETUP)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETVERSION)();
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETDESC)(char pszDesc[1024]);
	typedef int	(__stdcall *	GFLS_GFS_LIVESCAN_GETERRORINFO)(int nErrorNo, char pszErrorInfo[256]);


#if 0
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
#endif

#ifdef	__cplusplus
}
#endif



/**
 * ����͢��׼�ӿڴ������
 */

//!< ����ָ�ơ�����ͼ��ɼ��ӿڴ������
#define ARGENT_GALSCAPT_ERROR_SUCCESS			1
#define ARGENT_GALSCAPT_ERROR_PARAM				-1			// �������󡣸��������Ĳ����д���
#define ARGENT_GALSCAPT_ERROR_MEMORY			-2			// �ڴ����ʧ�ܡ�û�з��䵽�㹻���ڴ档
#define ARGENT_GALSCAPT_ERROR_NOTIMPL			-3			// ����δʵ�֡����ú����Ĺ���û��ʵ�֡�
#define ARGENT_GALSCAPT_ERROR_NODEVICE			-4			// �豸�����ڡ���ʼ����ʱ�򣬼�⵽�豸�����ڡ�
#define ARGENT_GALSCAPT_ERROR_NOTINIT			-5			// �豸δ��ʼ����
#define ARGENT_GALSCAPT_ERROR_INVALIDENO		-6			// �Ƿ��Ĵ���š�
#define ARGENT_GALSCAPT_ERROR_NOLICENSE			-7			// û����Ȩ��
#define ARGENT_GALSCAPT_ERROR_OTHERERR			-9			// ��������

#define ARGENT_GALSCAPT_ERROR_INVALIDCHANNLE	-101		// ��Ч��ͨ����
#define ARGENT_GALSCAPT_ERROR_INVALIDCAPTWIN	-102		// �Ƿ��ɼ�����
#define ARGENT_GALSCAPT_ERROR_NOPRIVILEGE		-103        // û����Ȩ
#define ARGENT_GALSCAPT_ERROR_FAIL			    -104        // ��������ʧ�ܣ���������ϸ����ʾ��
#define ARGENT_GALSCAPT_ERROR_WRONG			    -105        // ���ô���
#define ARGENT_GALSCAPT_ERROR_RECOVERYSUCCESS   -106        // �豸�����˻ָ����һָ��ɹ�
#define ARGENT_GALSCAPT_ERROR_RECOVERYFAIL      -107        // �豸�����˻ָ������ָ�ʧ����
#define ARGENT_GALSCAPT_ERROR_BADFRAME          -108        // ����UC500C��ĳЩ�������ã�RowSize, ColumnSize, RowSkip, ColumnSkip����
														//�������ŵ�һ֡������Ч֡
#define ARGENT_GALSCAPT_ERROR_INVALIDFRAME      -109		// ���ݴ������д���ͼ���в������ݶ�ʧ��û��ͼ��
														// ���ݣ����ش�ֵ���������豸������������
#define ARGENT_GALSCAPT_ERROR_VIOLATION         -110        //�˺����ĵ��������������г�ͻ

#define ARGENT_GALSCAPT_ERROR_UNKNOWN			-111		// δ֪����

#define ARGENT_GALSCAPT_ERROR_BADQUALITY		-112		//!< �ɼ�ָ�Ƶ�ͼ��������

#if 0
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
 * ����͢��׼ָ�ơ����ƴ�С
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
#endif

#endif	// _INC_LS_ARGENT_H_20121119164618_