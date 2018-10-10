#pragma once

//#include "ImageStatic.h"
#include "GfRectTracker.h"

//!< ���ݲɼ�״̬��ָλ����ͼ������״̬����ʾ���
class CBTYStatus
{
public:
	int		nBty;
	int		Fgp;
	int		ViewID;
	bool	bSlap;

	int		nScore;

	int		*pnState, *pnExf;	//!< ͼ������״̬����Ҫ��ӳ���������̶�
	UCHAR	*pnDisabilityDesc;	//!< �м�ָ�������������˰̡���Ƥ������
	FINGERFEATQLEV		*pstFeat;

	bool	*pbNotExist;	//!< ȱָ
	//bool	*pbReGather;	//!< �ز�
	int		*pnCaptOption;	//!< �ɼ�ѡ�����ɼ�����ֹ�ɼ����ɲɿɲ���

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
	int		m_nDesObj, m_nMaxSplitFlat;	//!< ����м���ƽ��ָ����Ҫ�и�

	enum	
	{ 
		FLAT_MAXCNT = 4, FLAT_WIDTH = 640, FLAT_HEIGHT = 640, 
		SPLITSTATUS_NOTLOCATE = 0, SPLITSTATUS_LOCATED = 1, SPLITSTATUS_NOFINGER = 3,	//!< û�ж�λ������λ�ɹ���ȱָ
		NOTLOCATE_NOFINGER, LOCATED_NOFINGER			//!< ��(û��)��λ < -- > ȱָ֮���ת��
	};
	CRect	m_rtFlatView[FLAT_MAXCNT];		//!< ����ĸ�ƽ��ָ�Ƶĳ�ʼ��ȡλ�ã�����ڸô��ڣ�
	POINT	m_ptFlatVector[FLAT_MAXCNT];	//!< ����ƽ��ָ�ƽ�ȡ�����ת����
	int		m_nSpliteStatus[FLAT_MAXCNT];	//!< ƽ��ָ���Ƿ��Զ��и���Զ��и��㷨�Ƿ���Ϊ��ƽ��ָ�ƴ��ڣ�
	int		m_nObjectType[FLAT_MAXCNT];		//!< �������ͣ�����m_pstDesObj�еĶ�Ӧֵ

	double	m_fScale;			//!< ����ָ���ݼ�����и���Ϣ�ڸ�Static����ʾʱ��ѹ������
	UCHAR	*m_pViewImage;	//!< ����m_fScaleѹ�����������ʾ��ͼ��
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
	bool	Help_FlatFingerIsNotExist(int nObjType);	//!< ConfigInfo���Ƿ��Ѿ����ø�ָλ������

	//!< ��ȡ��ȡ�����ʾλ�ú���ת����
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

	//!< ����������ر���
	enum	{ MOUSEOP_NONE, MOUSEOP_LBTNDOWN, MOUSEOP_RBTNDOWN };
	int		m_nMouseOp;
	int		m_nFlatIdxMouseOver, m_nCurSelFlat;	//!< ��ǰ������ڵ�ƽ��ָ�ƿ���������ǰѡ��(�������Ҽ���ʱ��)
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
	int		Help_QualityAndFgpCheck(CBTYStatus &btyStatus);	//!< ��������ָλ����

	int		Help_RelocateGetSrcObjType(int nDesObjType);

	//!< �õ���ת����µ���������
	void	Help_GetBoundRect(CRect &rtBound, CRect &rect, POINT &ptVector);

	//!< ָ����ָλ�Ƿ���Ҫ�и�
	bool	Help_FgpNeedSplit(int nFgp);
};


