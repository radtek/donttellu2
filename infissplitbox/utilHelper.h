/**
 * @file	utilHelper.h
 * @brief	通用工具类：提供辅助功能
 * @author	北京东方金指科技有限公司
 * @date	2011/02/18 16:12:40
 * @note	
 * @copyright	版权所有(@) 1996-2011
 *				北京东方金指科技有限公司
 */

#pragma once

//暂时无法去除，因为很多类与m_pCtrl没有联系，无法根据接口获知当前区域。
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

	static	COLORREF	m_crBackground;	//!< 控件的背景色
	static	COLORREF	m_crFill;		//!< Button等子窗口的填充颜色，目前没使用
	static	COLORREF	m_crFrame;		//!< 子窗口的边框的颜色
	static	COLORREF	m_crTipInfo;	//!< 提示信息框的文本颜色RGB(144, 120, 225)

	static	COLORREF	m_crHandDescText;	//!< 指掌纹框中的描述文字的颜色

	static	char m_szNotExist[32];	//!< 缺指
	static	char m_szReGather[32];	//!< 重采
	static	char m_szRollFinger[32];
	static	char m_szFlatFinger[32];
	static	char m_szFingerName[10][32];
	static  char m_szPalmName[4][32];
	static	char m_szSlap[3][32];
	static	void GetStringResource();

	static	CBmpResource	m_bmResBtnNormal;		//!< 子窗口的背景位图资源:正常状态以及下压状态
	static	CBmpResource	m_bmResBtnPress;

	//!< 贵州新界面
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

	//!< 当GAFIS6的压缩方法是各家指纹系统厂家提供的压缩方法时，根据G6的压缩方法，设置相应的nVendorId、nProductId
	static	void	CprMethod_G6ToVendorProductID(int nG6Method, int &nVendorId, int &nProductId);

	//!< 四联指、左右手拇指索引：右手四指、左手四指、左右手拇指
	enum { SLAP_R, SLAP_L, SLAP_THUMB, SLAP_CNT };
//	static int SLAP_FGPToIndex(int nFGP);
//	static int SLAP_IndexToFGP(int nIndex);

	//!< 掌纹各部位索引
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

	//!< 对于四联指、左右手拇指，判断其是否是左右手拇指、是否右手四指、是否左手四指
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

	//!< 窗口布局
	enum 
	{
		//!< 采集显示区域的最小大小及左边操作区域的宽度
		FRAME_MINSIZE = 640, OCX_WEDGE = 5, LEFTAREA_WIDTH = 210, OCX_BTNHEIGHT = 25,

		//!< 左侧已采集指纹各个显示框的大小，边距(Wedge)，水平垂直间距(Inter)
		FGAREA_WIDHT = 80, FGAREA_HEIGHT = 80,	FGAREA_WEDGE = 10, FGAREA_HDIST = 20, FGAREA_VDIST = 10,

		//!< 四指框
		FOURFG_WIDTH = 180, FOURFG_HEIGHT = 150,

		//!< 掌纹框
		PALMSUB_WIDTH = 180, PALMSUB_HEIGHT = 150, WRITER_WIDTH = 80, WRITER_HEIGTH = 150,

		OCXCTRL_DIST = 10, OCXEDIT_MINHEIGHT = 60,

		VITICAL_DISP = 2				//!< 在画图时，垂直方向从顶端(TOP)显示时，缺省留出2个像素
	};

	static int	GetMinWidth()
	{
		return LEFTAREA_WIDTH + FRAME_MINSIZE + OCX_WEDGE * 2;
	}

	//!< 一些缺省值
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