/**
 * @file	utilHelper.h
 * @brief	ͨ�ù����ࣺ�ṩ��������
 * @author	����������ָ�Ƽ����޹�˾
 * @date	2011/02/18 16:12:40
 * @note	
 * @copyright	��Ȩ����(@) 1996-2011
 *				����������ָ�Ƽ����޹�˾
 */

#pragma once

//��ʱ�޷�ȥ������Ϊ�ܶ�����m_pCtrlû����ϵ���޷����ݽӿڻ�֪��ǰ����
//#define  UTIL_GNE_ZKY_MATCH  

#define  GAIMG_CPRMETHOD_GA10_CPRRATIO  10

class	CBmpResource
{
public:
	CBmpResource() 
	{
		InitResourceInfo();
	}
	~CBmpResource()
	{
		FreeResourceInfo();
	}
	void	InitResourceInfo();
	void	FreeResourceInfo();

	BOOL	LoadBitmapResource(HDC hdc, uint4 nID);
	bool	Draw(CDC *pDC, LPRECT prtDes);

private:
	HBITMAP		m_hBitmap;
	BITMAPINFO	*m_pbmpInfo;
	UCHAR		*m_pBitData;
};

class	CUtilHelper
{
public:
	CUtilHelper();
	~CUtilHelper() {};

	GYTH_256BITMAPINFO	m_stbi;

	static	COLORREF	m_crBackground;	//!< �ؼ��ı���ɫ
	static	COLORREF	m_crFill;		//!< Button���Ӵ��ڵ������ɫ��Ŀǰûʹ��
	static	COLORREF	m_crFrame;		//!< �Ӵ��ڵı߿����ɫ
	static	COLORREF	m_crTipInfo;	//!< ��ʾ��Ϣ����ı���ɫRGB(144, 120, 225)

	static	COLORREF	m_crHandDescText;	//!< ָ���ƿ��е��������ֵ���ɫ

	static	char m_szNotExist[32];	//!< ȱָ
	static	char m_szReGather[32];	//!< �ز�
	static	char m_szRollFinger[32];
	static	char m_szFlatFinger[32];
	static	char m_szFingerName[10][32];
	static  char m_szPalmName[4][32];
	static	char m_szSlap[3][32];
	static	void GetStringResource();

	static	CBmpResource	m_bmResBtnNormal;		//!< �Ӵ��ڵı���λͼ��Դ:����״̬�Լ���ѹ״̬
	static	CBmpResource	m_bmResBtnPress;

	//!< �����½���
	static	CBmpResource	m_bmResBtnNormalNew;
	static	CBmpResource	m_bmResBtnPressNew;
	static	CBmpResource	m_bmResRadioNormal;
	static	CBmpResource	m_bmResRadioPress;

	int		UTIL_BuildCaptImageData(GYTH_CAPTIMAGEDATA &stBTYCaptData, int nDesWidth, int nDesHeight, int nDesBPP,
									UCHAR *pbnSrcData, int nSrcWidth, int nSrcHeight, int ncbSrcLine, int nSrcBPP);

	static int	GetImageBuffer(BITMAPINFO *pstbi, UCHAR *&pBuffer, int &nCurBuffer, int nHeadLen)
	{
		pBuffer = (UCHAR*)pstbi;
		if ( pstbi )
		{
			nCurBuffer = GetImageSize(pstbi->bmiHeader.biWidth, 
									 abs(pstbi->bmiHeader.biHeight), 
									 pstbi->bmiHeader.biBitCount) + nHeadLen;
		}
		else nCurBuffer = 0;
		return nCurBuffer;
	}
	static int	GetBmpImgBuffer(BITMAPINFO *pstbi, UCHAR *&pBuffer, int &nCurBuffer, int nHeadLen)
	{
		pBuffer = (UCHAR *)pstbi;
		if ( pstbi )
		{
			nCurBuffer = GetBmpImgSize(pstbi->bmiHeader.biWidth,
									   abs(pstbi->bmiHeader.biHeight),
									   pstbi->bmiHeader.biBitCount) + nHeadLen;
		}
		else nCurBuffer = 0;
		return nCurBuffer;
	}

	static	int	FingerFGPToIndex(int nFGP)
	{
		if ( nFGP < GAFIS7::CodeData::FINGER_R_THUMB ) return -1;
		if ( nFGP > GAFIS7::CodeData::FINGER_L_LITTLE ) return -1;
		return (int)(nFGP - GAFIS7::CodeData::FINGER_R_THUMB);
	};
	static	int FingerIndexToFGP(int nIndex)
	{
		return nIndex + GAFIS7::CodeData::FINGER_R_THUMB;
	};
	static	UCHAR ItemData(int nBTY, int nFGP, int nViewID)
	{
		//if ( nBTY == GAFIS7::CodeData::BTY_FINGER ) return nFGP;
		//else if ( nFGP <= GAFIS7::CodeData::L_PALM_Q ) return nFGP;
		//else if ( nFGP == GAFIS7::CodeData::)
		return nFGP;
	};
	static int ItemDataToFgp(UCHAR nItemData) { return nItemData; };
	static int FingerItemDataToIndex(UCHAR nItemData) { return nItemData -1; };
	static	bool IsFlatFinger(/*int nBTY, */int nViewID)
	{
		if ( nViewID == GAFIS7::CodeData::FINGER_VIEWID_FLAT ) return true;
		return false;
	};

	static	bool	IsNeedExtact(int nBTY, int nFGP, int nViewID)
	{
		if ( nBTY == GAFIS7::CodeData::BTY_FINGER ) return true;
		else if ( nFGP <= GAFIS7::CodeData::L_PALM_Q || nFGP == GAFIS7::CodeData::L_PALM || nFGP == GAFIS7::CodeData::R_PALM) return true;
		return false;
	}

	static	int		GetImageLine(int nWidth, int nBitsPerPixel)
	{
		return (nWidth * nBitsPerPixel + 7) / 8;
	}
	static	int		GetBmpImgLine(int nWidth, int nBitsPerPixel)
	{
		return (nWidth * nBitsPerPixel + 31) / 32 * 4;
	}
	static	int		GetImageSize(int nWidth, int nHeight, int nBitsPerPixel)
	{
		return GetImageLine(nWidth, nBitsPerPixel) * nHeight;
	}
	static	int		GetBmpImgSize(int nWidth, int nHeight, int nBitsPerPixel)
	{
		return GetBmpImgLine(nWidth, nBitsPerPixel) * nHeight;
	}

	static	UCHAR	CprMethod_GAFIS7To6(int nG7Method, int nVendorId = VENDORID_UNKNOWN, int nProductId = 0);
	static	UCHAR	CprMethod_GAFIS6To7(int nG6Method, int &nVendorId, int &nProductId);
	static	UCHAR	CprMethod_UserDefindToG6(int nVendorId, int nProductId);

	//!< ��GAFIS6��ѹ�������Ǹ���ָ��ϵͳ�����ṩ��ѹ������ʱ������G6��ѹ��������������Ӧ��nVendorId��nProductId
	static	void	CprMethod_G6ToVendorProductID(int nG6Method, int &nVendorId, int &nProductId);

	//!< ����ָ��������Ĵָ������������ָ��������ָ��������Ĵָ
	enum { SLAP_R, SLAP_L, SLAP_THUMB, SLAP_CNT };
//	static int SLAP_FGPToIndex(int nFGP);
//	static int SLAP_IndexToFGP(int nIndex);

	//!< ���Ƹ���λ����
	enum { PALM_RSUB, PALM_LSUB, PALM_RWRITER, PALM_LWRITER, PALM_CNT };
	static int PalmFGPToIndex(int nFGP);
	static int PalmIndexToFGP(int nIndex, bool bSlap);
	static int PalmItemDataToIndex(UCHAR nItemData) 
	{
		return PalmFGPToIndex(nItemData);
	}

	static bool PalmFGPIsSlap(int nFGP)
	{
		if ( (nFGP == GAFIS7::CodeData::R_FOUR) || (nFGP == GAFIS7::CodeData::L_FOUR) ||
			(nFGP == GAFIS7::CodeData::RL_THUMB) ) 
		{
			return true;
		}
		else return false;
	}

	//!< ��������ָ��������Ĵָ���ж����Ƿ���������Ĵָ���Ƿ�������ָ���Ƿ�������ָ
	static bool SlapIsRLThumb(int nFGP)
	{
		if ( nFGP == GAFIS7::CodeData::RL_THUMB ) return true;
		else return false;
	}
	static	bool SlapIsRFour(int nFGP)
	{
		if ( nFGP == GAFIS7::CodeData::R_FOUR ) return true;
		else return false;
	}
	static	bool SlapIsLFour(int nFGP)
	{
		if ( nFGP == GAFIS7::CodeData::L_FOUR ) return true;
		else return false;
	}

	static int FGPToIndex(int nBty, int nFgp)
	{
		if ( nBty == GAFIS7::CodeData::BTY_FINGER ) return FingerFGPToIndex(nFgp);
		else if ( nBty == GAFIS7::CodeData::BTY_PALM ) return PalmFGPToIndex(nFgp);
		return -1;
	}

	//!< ���ڲ���
	enum 
	{
		//!< �ɼ���ʾ�������С��С����߲�������Ŀ��
		FRAME_MINSIZE = 640, OCX_WEDGE = 5, LEFTAREA_WIDTH = 210, OCX_BTNHEIGHT = 25,

		//!< ����Ѳɼ�ָ�Ƹ�����ʾ��Ĵ�С���߾�(Wedge)��ˮƽ��ֱ���(Inter)
		FGAREA_WIDHT = 80, FGAREA_HEIGHT = 80,	FGAREA_WEDGE = 10, FGAREA_HDIST = 20, FGAREA_VDIST = 10,

		//!< ��ָ��
		FOURFG_WIDTH = 180, FOURFG_HEIGHT = 150,

		//!< ���ƿ�
		PALMSUB_WIDTH = 180, PALMSUB_HEIGHT = 150, WRITER_WIDTH = 80, WRITER_HEIGTH = 150,

		OCXCTRL_DIST = 10, OCXEDIT_MINHEIGHT = 60,

		VITICAL_DISP = 2				//!< �ڻ�ͼʱ����ֱ����Ӷ���(TOP)��ʾʱ��ȱʡ����2������
	};

	static int	GetMinWidth()
	{
		return LEFTAREA_WIDTH + FRAME_MINSIZE + OCX_WEDGE * 2;
	}

	//!< һЩȱʡֵ
	enum { BPP_8 = 8, PPI_DEFAULT = 500, SENDMESSAGE_TIMEOUT = 500 };

};


#define  UTIL_MNTMAXSIZE 500000


extern	int		UTIL_DisplayAppUtilInfo(const char *szInfo, const char *szDetail, UINT fInfoType, CWnd *pParent);
extern	int		UTIL_DisplayAppUtilInfo(UINT uID, const char* szDetail, UINT fInfoType, CWnd *pParent);
extern	int		UTIL_DisplayAppUtilInfo2(UINT uInfoID, UINT nDetailID, UINT fInfoType, CWnd *pParent);
extern	void	UTIL_SetUtilInfoParentWnd(CWnd *pParent);

void	UTIL_DrawNoScaleData(CDC *pDC, CRect rtFrame, BITMAPINFO *pbmInfo, UCHAR *pData, CPoint *pptImgOff);

void	UTIL_GetSplitedMatchPlainData(UCHAR *pbnSrcData, UCHAR *pbnSplitData, REGIONRECT &stRegion, 
									  int nwSrc, int nhSrc, int &nwDes, int &nhDes);

int		UTIL_MntStd2MntDispInfo(void* pMntData, FINGERFEATQLEV* pstFeat, int bPlain);
void	UTIL_ImageIntercept(BYTE *pSrc, BYTE *pDes, int nwSrc, int nhSrc, int nwDes, int nhDes);
void	UTIL_DrawImageDataInArea(CDC* pDC, CRect rt, BITMAPINFO *pbmInfo, UCHAR *pData);