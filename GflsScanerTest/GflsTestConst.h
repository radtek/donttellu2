//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GflsTestConst.h	define macro, const and struct
// create date: 2006-04-17
// 
//
#pragma once

#include <math.h>


#define GFLS_EDZCS_DEF    1
//#define GFLS_EDZCS_GSTEST 1

#if GFLS_EDZCS_DEF1
#define GFLS_SCANNER_IMAGEWIDTH			640//256
#define GFLS_SCANNER_IMAGEHEIGHT		640//360
#else
#define GFLS_SCANNER_IMAGEWIDTH			2304//640
#define GFLS_SCANNER_IMAGEHEIGHT		2304//640

#endif

#ifdef INFIS_FOURFINGER

//for infis four fingers
#define GFLS_SCANNER_FINGER_WIDTH	1600
#define GFLS_SCANNER_FINGER_HEIGHT	1500

#else

#define GFLS_SCANNER_FINGER_WIDTH	640
#define GFLS_SCANNER_FINGER_HEIGHT	640

#endif

#define GFLS_SCANNER_CENTERWIDTH		600
#define GFLS_SCANNER_CENTERHEIGHT		600

#define GFLS_SCANNER_MAXSCALE			5
#define GFLS_SCANNER_MINSCALE			1

#define GFLS_SCANNER_TESTTITLE			_T("����ָ�Ʋɼ��ǲ���")
#define GFLS_SCANNER_STARTCAPTURE		_T("�ɼ�ͼ��")
#define GFLS_SCANNER_STOPCAPTURE		_T("ֹͣ�ɼ�")

#define GFLS_IMAGEINFODLG_TITLE			_T("ͼ����Ϣ(%d x %d @ %d%%)")


#define WM_SETIMAGEINFO_TOSTAT			WM_USER + 0x101				// ���Ͳɼ��õ���ͼ����Ϣ���Խ���ֱ��ͼͳ��, wParam����Ƿ�ֻͳ��ѡ�е�ͼ������, lParam����Ϊ��
#define WM_SHOWIMAGE_STATINFO			WM_USER + 0x102				// ��ʾͳ����Ϣ, wParam��lParam����Ϊ��


#define GFLS_SCANNER_STDCHKTYPE_NORMAL				0		// ������ʾģʽ,û�м�����
#define GFLS_SCANNER_STDCHKTYPE_OPTICSDEVICE		1		// ��ѧӲ�����
#define GFLS_SCANNER_STDCHKTYPE_CAPTURESIZE			2		// �ɼ����ڳߴ���
#define GFLS_SCANNER_STDCHKTYPE_VALIDIMAGESIZE		3		// ��Чͼ��ߴ���
#define GFLS_SCANNER_STDCHKTYPE_IMAGEPIXELCOUNT		4		// ͼ�����������
#define GFLS_SCANNER_STDCHKTYPE_CENTERDIFF			5		// ͼ������ƫ����
#define GFLS_SCANNER_STDCHKTYPE_IMGRESOLUTION		6		// ͼ��ֱ��ʼ��
#define GFLS_SCANNER_STDCHKTYPE_ABERRANCE			7		// ���ͼ�����
#define GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYAVERAGE	8		// ͼ�񱳾��Ҷ�ֵ���
#define GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYUNAVE	9		// ���ͼ��ҶȲ����ȶ�
#define GFLS_SCANNER_STDCHKTYPE_CENTERGRAY			10		// �����������ͼ��Ҷ�ֵ
#define GFLS_SCANNER_STDCHKTYPE_GRAYVARIA			11		// ���Ҷȶ�̬��Χ
#define GFLS_SCANNER_STDCHKTYPE_BADPOINT			12		// ���ͼ��õ�
#define GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT			13		// ��⻷����
#define GFLS_SCANNER_STDCHKTYPE_PARSE				14		// ���ͼ�������
#define GFLS_SCANNER_STDCHKTYPE_CAPTSPEED			15		// ���ɼ�����
#define GFLS_SCANNER_STDCHKTYPE_MOVESQUARE  			16		// �ƶ����ο�

#include "GfStdLiveApi.h"
#include "GflsCaptMosaic.h"
#include "GfDlgBarEx.h"

// struct define
typedef struct tagGf_256BitmapInfo 
{
	BITMAPINFOHEADER	bmiHeader; 
	RGBQUAD				bmiColors[256]; 
} GF_256BITMAPINFO;
typedef struct tagGflsTest_ParamInfo
{
	UCHAR*	pDispImage;			// ������ʾ��ͼ������
	UCHAR*	pSrcImage;			// �ɼ�����ԭʼͼ������;
	int		nImgWidth;			// ԭʼͼ�����ݵĿ�Ⱥ͸߶ȣ��ֱ�ΪGFLS_SCANNER_IMAGEWIDTH��GFLS_SCANNER_IMAGEHEIGHT
	int		nImgHeight;
	int		nImgSize;			// ԭʼͼ�����ݵĴ�С

	int		nDispWidth;
	int		nDispHeight;
	int		nDispImgSize;

	int		nCurScale;			// ��ǰ�ķŴ�������ЧֵΪ1,2,3,4,5�����Ŵ�5��
	int		nCrossLength;		// ����ʮ�ֿ��ߵĳ��ȣ�100����
	int		nSquare1;			// �������е��������߿�Ŀ��300��500, 440
	int		nSquare2;
	int		nSquare3;
	int		nSquare4;       //�������߿�Ŀ�ȡ��߶ȡ�ƫ��
	//int		nSquare4H;
	int		nSquare4OffX;
	int		nSquare4OffY;
	CRect   rectSquare4;   

	POINT	ptOffSet;			// ͼ����ʾʱ��ƫ��

	COLORREF	clCross;		// ����ʮ�ֿ��ߵ���ɫ
	COLORREF	clSquare1;		// ���е��������߿����ɫ
	COLORREF	clSquare2;
	COLORREF	clSquare3;
	COLORREF	clSquare4;
	COLORREF	clBack;

	POINT		ptAreaLeftTop;			// ��ǰѡ�еľ�����������ϵ�����µ㣬����ʾͼ������ϵ�Ϊԭ��
	POINT		ptAreaRightBottom;
	COLORREF	clArea;					// �����������ɫ
//	POINT		ptLineForEnvir1;		// ��������ʱ���������ֵķָ���
//	POINT		ptLineForEnvir2;		// ��������ʱ���������ֵķָ���

	UCHAR	bCrossSquareScaled;			// ʮ�ֿ��ߺ��߿��Ƿ�Ҳͬ������
	UCHAR	bOnlyShowImage;				// �Ƿ�ֻ��ʾͼ��
	UCHAR	bInsertZoom;				// �Ƿ��ֵ����ͼ��

	UCHAR	nStdChkType;				// ��׼�������
	UCHAR	bDrawBadPoint;				// �ڼ��õ�ʱ���Ƿ���ʾ�õ�ֲ�ͼ
	UCHAR	bCanStdCheck;				// �Ƿ���Լ��
	UCHAR	nReserved[2];
}GFLSTEST_PARAMINFO;

typedef struct tagGf_ImageParseInfo
{
	RECT	rtCArea[9];					// TP-F02��9��C�����λ��
	double	fBackMedian[9];				// �������Ӧ��Ϊ��ֵ�������м�ֵ
	double	fWhiteMedian[9];
	int		nCurSelArea;				// ��ǰѡ�е�����
	POINT	ptPrevMouse;				// 
}GF_IMAGEPARSEINFO;

typedef struct tagGf_ChkToolSampleData
{
	GF_256BITMAPINFO	*pstbmpInfo;
	UCHAR	*pSampleData;
	int		nDataWidth;					// ����������ݵĿ�Ⱥ͸߶�
	int		nDataHeight;

	int		nDPI;						// ��������DPI
	int		nLength;					// �������ĳ���(��λΪmm��ʮ��֮һ)

	UCHAR	*pDispData;
	int		nDispWidth;
	int		nDispHeight;

	UCHAR	nChkToolType;				// �����������
	UCHAR	nReserved[7];
}GF_CHKTOOLSAMPLEDATA;


#define GFLS_SCANNER_CHKTOOL_GRIDSAMPLE			1		// ��׼����궨�������
#define GFLS_SCANNER_CHKTOOL_RESOLUTION			2		// ��׼�ֱ��ʼ������

extern void Gf_SaveData2BitmapFile(UCHAR* pData, int nw, int nh, const char* pszFileName);
extern void EnlargeOrShinkImage(UCHAR *pSrcData, int nSrcWidth, int nSrcHeight, UCHAR *pDestData, int nDestWidth, int nDestHeight);
extern void ZoomImageByScale(UCHAR *pSrcData, int nSrcWidth, int nSrcHeight, UCHAR *pDesData, int nScale);
extern int  GetBitmapFileName(char* pszFileName, int ncbFileName, BOOL bOpenFile, CWnd* pParentWnd);
extern int	ShowScannerErrorInfo(int nError, CWnd* pParentWnd, UCHAR bLiveCapt);
extern int  ShowTestAppErrorInfo(const TCHAR* pszErrorInfo, CWnd* pParentWnd);
extern int	ZoomPointByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg);
extern int	ZoomPointXByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg);
extern int	ZoomPointYByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg);
extern int	ZoomPointFromOld2New(RECT *prtArea, SIZE *psize, POINT *ppt, int nOldScale, int nNewScale);

extern void ReverseImageData(BYTE* pData, int nw, int nh);
typedef struct tagGf_ScannerInfo
{
	HANDLE	hCaptThread;	// �ɼ��߳�

	int nMaxChannel;		// ͨ����
	int	nCurChannel;		// ��ǰʹ�õ�ͨ��

	int	nBright;			// ���ȡ��Աȶ�
	int	nContrast;

#ifdef UNSTD_BLACK_BALANCE
	int	nBlackBalance;		//��ƽ��
#endif
	int	nWidth;				// ��Ⱥ͸߶�
	int nHeight;

	int	nXOrigin;
	int	nYOrigin;

	int	nVersion;

	UCHAR	bInitialed;		// ��ʼ���Ƿ�ɹ�
	UCHAR	bExitCapture;	// �Ƿ��˳��ɼ��߳�
	UCHAR	bCaptureImage;	// �Ƿ����ڲɼ�ͼ��
	UCHAR   bReverseImage;  // �Ƿ���ͼ��

	UCHAR	bFaseDev;		// �Ƿ���豸
	UCHAR	bSptCaptWnd;	// �Ƿ�֧�ֲɼ����ڵ�����
	UCHAR	bSptSetup;		// �Ƿ�֧��Setup���öԻ���
	UCHAR	bExitMosaic;	// �Ƿ��˳�ƴ�ӹ���
	UCHAR	bHasMosaicThrd;	// ƴ���߳��Ƿ��Ѿ�����
	UCHAR	nReserved[1];
	int		nCaptFrames;
}GF_SCANNERINFO;

extern GF_SCANNERINFO	g_stScannerInfo;

extern int a;
typedef struct tagGf_CaptSpeedInfo
{
	int		nFrameCount;		// �ɼ���֡��
	int		nUsedTimes;			// �ķѵ�ʱ��
	double	fSpeed;				// �ɼ�����
}GF_CAPTSPEEDINFO;


typedef struct tagGf_ImageSaveInfo
{
	char	szFilePath[256];
	char	szFileName[32];
	int		nCurSerial;
	UCHAR	bSaveCaptImage;
	UCHAR	nFileType;				// 0 -- bmp, 1 -- raw
	UCHAR	nReserved[2];
}GF_IMAGESAVEINFO;


typedef struct tagGf_ImageGrayStatStruct
{
	UCHAR	*pImageData;		// Ҫ����8λ256���Ҷ�ͼ��
	int		*pnGrayValueCnt;	// NULL��256�����ȵ���������,����������Ҷ�ֵ�ĸ���,��pnGrayValueCnt[i]��ʾ�Ҷ�ֵΪi�����ظ���, i=0 -- 255
	int		nw, nh;				// ͼ��Ŀ�Ⱥ͸߶�
	RECT	rtArea;				// Ҫͳ�Ƶ�ͼ������

	int		nPixelCount;		// ��ͳ�Ƶ�������
	int		nTotalGrayCount;	// ��������ĻҶ�ֵ�ܺ�
	int		nMaxCntGrayValue;	// �������Ҷ�ֵ�ĻҶ�,�Լ����иûҶ�ֵ�����ظ���
	int		nMaxCntGrayCount;
	int		nMinCntGrayValue;	// �������ٻҶ�ֵ�ĻҶ�,�Լ����иûҶ�ֵ�����ظ���(�������0)
	int		nMinCntGrayCount;

	int		nMinGrayValue;		// ��ͼ�������е�������С�ķ�0�Ҷ�ֵ
	int		nMaxGrayValue;

	double	fAverage;			// ƽ���Ҷ�ֵ
	double	fDiff;				// ��׼ƫ��
	double	fMedian;			// �м�ֵ
}GF_IMAGEGRAYSTATSTRUCT;

int GetImageGrayStatisticInfo(GF_IMAGEGRAYSTATSTRUCT *pstGrayStat);
int GetImageAreaLineGray(GF_IMAGEGRAYSTATSTRUCT *pstGrayStat, double *pfBackMedian, double *pfWhiteMedian);


#define	GFLSTEST_FILEEXIST(fname)	(_access(fname, 0) < 0 ? 0 : 1)
int	GflsTest_DirCreateRecur(char *pathname, int flag);


void	StopCaptureThread();
DWORD	WINAPI StartCaptureThread(LPVOID lParam);

//int	Gfls_PointCompare(const void* pelem1, const void* pelem2);


// ��׼���

enum GF_ABERPARAMPOS
{
	ABERPOS_UNKNOWN,
	ABERPOS_A1,
	ABERPOS_A2,
	ABERPOS_A3,
	ABERPOS_A4,
	ABERPOS_B1,
	ABERPOS_B2,
	ABERPOS_B3,
	ABERPOS_B4,
	ABERPOS_CENTER
};


extern inline double GetTowPointLength(LPPOINT ppt1, LPPOINT ppt2);


typedef struct tagGf_BadPointInfo
{
	POINT*	paBadPoint;			// �����Ĵõ����飻
	int		nBadptCount;		// �����õ����
	int		nIndex;				// ��ǰ�õ��������������е�������
	int		xRadio, yRadio;		// x,y�����ֱ��
	int		nRadio;

//	int		xStart, yStart;		// ��ʼ��;
//	int		xCurr,  yCurr;		// ��ǰ����
	
	UCHAR	bCenter;			// �Ƿ�������
	UCHAR	nReserved[3];
}GF_BADPOINTINFO;

typedef struct tagGf_BadPointData
{
	POINT*	paBadPoint;			// �����Ĵõ����飻
	int		nBadptCount;		// �����õ����
	int		nRadio;				// ֱ��
	UCHAR	bCenter;
	UCHAR	nReserved[3];
}GF_BADPOINTDATA;

typedef struct tagGf_BadptDataArray
{
	GF_BADPOINTDATA	*pstBadData;
	int				nBadData;
	int				nCurDataBuf;
	int				nStep;
}GF_BADPTDATAARRAY;


// image info 
typedef struct tagGfls_ImageInfoStruct
{
	UCHAR			 *pImgData;		// ͼ�����ݻ�����,  ����Ϊ(��� * �߶� * λ�� / 8)
	RGBQUAD			 *prgbQuad;		// Color-mapped(λ�� < 24)ͼ�����ɫ����Ϣ������������λ������
	BITMAPINFOHEADER bmpHead;	// ��������ͼ��Ŀ�ȡ��߶ȡ�����λ������Ϣ
}GFLS_IMAGEINFOSTRUCT;

extern int Gfls_LoadImageFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_SaveImageFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_LoadRawImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_LoadBmpImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_SaveRawImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern int Gfls_SaveBmpImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo);
extern void VerticalFlipBuffer(UCHAR *pBuf, int nw, int nh, int nbits);

__int64	UTIL_HiResoTickCount();
__int64	UTIL_HiResoTickPerSec();
double	UTIL_HiResoTickToSec(__int64 nTimeUsed);
