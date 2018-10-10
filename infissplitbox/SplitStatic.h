#pragma once

//#include "ImageStatic.h"
#include "GfRectTracker.h"

//!< 数据采集状态：指位、视图、质量状态、显示框等
class CBTYStatus
{
public:
	int		nBty;
	int		Fgp;
	int		ViewID;
	bool	bSlap;

	int		nScore;

	int		*pnState, *pnExf;	//!< 图像质量状态，主要反映纹线清晰程度
	UCHAR	*pnDisabilityDesc;	//!< 残疾指纹质量描述：伤疤、脱皮、其他
	FINGERFEATQLEV		*pstFeat;

	bool	*pbNotExist;	//!< 缺指
	//bool	*pbReGather;	//!< 重采
	int		*pnCaptOption;	//!< 采集选项：必须采集、禁止采集、可采可不采

	GYTH_CAPTIMAGEDATA	*pstCaptImage;

	CBTYStatus()
	{
		memset(this, 0, sizeof(*this));
		nBty = GAFIS7::CodeData::BTY_UNKNOWN;
		Fgp = 0;
		ViewID = -1;
		bSlap = false;
		nScore = -1;
	};
	~CBTYStatus() {};

	void CBTYStatus::SetBtyViewIDFgp(int nNewBty, int nViewID, int nFgp)
	{
		nBty = nNewBty;
		Fgp = nFgp;
		ViewID = nViewID;
		bSlap = false;

		int		nIndex = GetFgIndex();

	}

	void SaveCaptImage2File()
	{
		return;

		if (!pstCaptImage || !pstCaptImage->pbmInfo) return;

		char	szImgFileInfo[256] = { 0 };
		GF_IMAGEINFOSTRUCT stImageInfo = { 0 };

		sprintf(szImgFileInfo, "d:\\temp\\%d_%d_%d.bmp", nBty, Fgp, ViewID);
		stImageInfo.fDataforPixel = GFIMGINFO_DATAPIXEL_FLAG_INDEX;
		stImageInfo.bIsPacketDIB = 1;
		stImageInfo.pImgData = pstCaptImage->pBitData;
		stImageInfo.prgbQuad = pstCaptImage->pbmInfo->bmiColors;
		memcpy(&stImageInfo.bmpHead, &pstCaptImage->pbmInfo->bmiHeader, sizeof(stImageInfo.bmpHead));
		GfImage_SaveBitmapFile(szImgFileInfo, &stImageInfo);
	}

	int	GetFgIndex() const
	{
		return CUtilHelper::FGPToIndex(nBty, Fgp);
	}
};

// CSplitStatic

class CSplitStatic : public CStatic
{
	DECLARE_DYNAMIC(CSplitStatic)

public:
	CSplitStatic();
	virtual ~CSplitStatic();

	void	SetSplitFlatInfo(CBTYStatus *pbtyStatus, REGIONRECT *pstSrcObj, 
			REGIONRECT *pstDesObj, int nDesObj/*, CCaptureView *pCaptView*/)
	{
		//m_pCaptView = pCaptView;
		m_pbtyStatus = pbtyStatus;
		m_pstSrcObj = pstSrcObj;
		m_pstDesObj = pstDesObj;
		m_nDesObj = nDesObj;
	}
	void	InitSplitFlatPlace();

	void	GetSplittedFlatData(bool bWrite2File, int nImageType);

	std::map<int, std::string>	_mapSplitedImg;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);

private:
	//CCaptureView	*m_pCaptView;
	CBTYStatus	*m_pbtyStatus;
	REGIONRECT	*m_pstSrcObj;
	REGIONRECT	*m_pstDesObj;
	int		m_nDesObj, m_nMaxSplitFlat;	//!< 最多有几个平面指纹需要切割

	enum	
	{ 
		FLAT_MAXCNT = 4, FLAT_WIDTH = 640, FLAT_HEIGHT = 640, 
		SPLITSTATUS_NOTLOCATE = 0, SPLITSTATUS_LOCATED = 1, SPLITSTATUS_NOFINGER = 3,	//!< 没有定位到，定位成功，缺指
		NOTLOCATE_NOFINGER, LOCATED_NOFINGER			//!< 从(没有)定位 < -- > 缺指之间的转换
	};
	CRect	m_rtFlatView[FLAT_MAXCNT];		//!< 最多四个平面指纹的初始截取位置（相对于该窗口）
	POINT	m_ptFlatVector[FLAT_MAXCNT];	//!< 各个平面指纹截取框的旋转向量
	int		m_nSpliteStatus[FLAT_MAXCNT];	//!< 平面指纹是否被自动切割到（自动切割算法是否认为该平面指纹存在）
	int		m_nObjectType[FLAT_MAXCNT];		//!< 对象类型，来自m_pstDesObj中的对应值

	double	m_fScale;			//!< 四联指数据及相关切割信息在该Static中显示时的压缩比例
	UCHAR	*m_pViewImage;	//!< 根据m_fScale压缩后的用来显示的图像
	int		m_nViewWidth, m_nViewHeight;
	CPoint	m_ptOffset;

	void	DrawSplitInfo();
	void	DrawViewImage(CDC *pDC);
	void	DrawFlatView(CDC *pDC);

	int		Help_GetFlatFgpByObjType(int nObjType)
	{
		int		nFgp = GAFIS7::CodeData::FINGER_R_THUMB;
		int		nIndex = nObjType - HANDSPLITE_OBJTYPE_FLAT_RTHUMB;
		if ( nIndex < 0 || nIndex >= 10 ) return 0;
		else return nFgp + nIndex;
	}
	bool	Help_FlatFingerIsNotExist(int nObjType);	//!< ConfigInfo中是否已经设置该指位不存在

	//!< 获取截取框的显示位置和旋转向量
	void	Help_GetFlatCutInfo(REGIONRECT *pstObj);
	void	Help_GetFlatCutByRegion(REGIONRECT *pstObj, int nFlatIndex);
	void	Help_InitFlatCutInfo(int nFgp, int nObjType);

	void	Help_PtOnImgToPtOnStatic(const CPoint &ptImg, CPoint &ptStatic);
	void	Help_PtOnStaticToPtOnImg(const CPoint &ptStatic, CPoint &ptImg);

	int		Help_GetFlatViewIndexByFgp(int nFgp)
	{
		bool	bLeft = (nFgp >= GAFIS7::CodeData::FINGER_L_THUMB) ? true : false;

		switch ( nFgp )
		{
		case GAFIS7::CodeData::FINGER_R_THUMB:
		case GAFIS7::CodeData::FINGER_L_THUMB:
			return bLeft ? 1 : 0;
		default:
			if ( bLeft ) return nFgp - GAFIS7::CodeData::FINGER_L_INDEX;
			else return nFgp - GAFIS7::CodeData::FINGER_R_INDEX;
		}
	}
	int		Help_GetFgpByFlatViewIndex(int nIndex)
	{
		if ( CUtilHelper::SlapIsRLThumb(m_pbtyStatus->Fgp) )
		{
			if ( nIndex == 0 ) return GAFIS7::CodeData::FINGER_R_THUMB;
			else return GAFIS7::CodeData::FINGER_L_THUMB;
		}
		else if ( CUtilHelper::SlapIsRFour(m_pbtyStatus->Fgp) )
		{
			return nIndex + GAFIS7::CodeData::FINGER_R_INDEX;
		}
		else return nIndex + GAFIS7::CodeData::FINGER_L_INDEX;
	}

	//!< 鼠标操作的相关变量
	enum	{ MOUSEOP_NONE, MOUSEOP_LBTNDOWN, MOUSEOP_RBTNDOWN };
	int		m_nMouseOp;
	int		m_nFlatIdxMouseOver, m_nCurSelFlat;	//!< 当前鼠标所在的平面指纹框索引、当前选中(按下左右键的时候)
	CPoint	m_ptStart;

	int		Help_PointInFlatView(CPoint point);
	int		Help_GetTrackerResizeFlag(int nSplitStatus)
	{
		//if ( nSplitStatus == SPLITSTATUS_LOCATED ) return GFRT_RESIZE_FLAG_MOVE;
		return GFRT_RESIZE_FLAG_ALL;
	};
	BOOL	Help_IsFingerNotExist(int nSpliteStatus);

	void	Help_MouseMoveWhenNone(UINT nFlags, CPoint point);
	void	Help_MouseMoveWhenLBtnDown(UINT nFlags, CPoint point);
	void	Help_MouseMoveWhenRBtnDown(UINT nFlags, CPoint point);

	int		Help_SetPointedFlatData(UCHAR *pCutFlatData, int nCutWidth, int nCutHeight, int nFgp);
	int		Help_QualityAndFgpCheck(CBTYStatus &btyStatus);	//!< 质量检查和指位检验

	int		Help_RelocateGetSrcObjType(int nDesObjType);

	//!< 得到旋转情况下的外接区域框
	void	Help_GetBoundRect(CRect &rtBound, CRect &rect, POINT &ptVector);

	//!< 指定的指位是否需要切割
	bool	Help_FgpNeedSplit(int nFgp);
};


