// SplitStatic.cpp : implementation file
//

#include "stdafx.h"

#include "SplitStatic.h"
#include "gbaselib/gaimgop.h"

// CSplitStatic

IMPLEMENT_DYNAMIC(CSplitStatic, CStatic)

CSplitStatic::CSplitStatic()
{
//	m_pCaptView = NULL;

	m_pbtyStatus = NULL;
	m_pstSrcObj = NULL;
	m_pstDesObj = NULL;
	m_nDesObj = 0;
	m_nMaxSplitFlat = 0;

	for ( int i = 0; i < FLAT_MAXCNT; ++i )
	{
		m_ptFlatVector[i].x = 1;
		m_ptFlatVector[i].y = 0;
		m_nSpliteStatus[i] = SPLITSTATUS_NOFINGER;
		m_nObjectType[i] = 0;
	}
	m_fScale = 1.0;
	m_pViewImage = NULL;

	m_nViewWidth = m_nViewHeight = 0;

	m_ptStart = CPoint(0, 0);
	m_nMouseOp = MOUSEOP_NONE;
	m_nFlatIdxMouseOver = -1;
	m_nCurSelFlat = -1;
}

CSplitStatic::~CSplitStatic()
{
	AFIS_FREE(m_pViewImage);
}

void CSplitStatic::Help_PtOnImgToPtOnStatic(const CPoint &ptImg, CPoint &ptStatic)
{
	ptStatic.x = (int)(ptImg.x * m_fScale) + m_ptOffset.x;
	ptStatic.y = (int)(ptImg.y * m_fScale) + m_ptOffset.y;
}
void CSplitStatic::Help_PtOnStaticToPtOnImg(const CPoint &ptStatic, CPoint &ptImg)
{
	ptImg.x = (int)((ptStatic.x - m_ptOffset.x ) / m_fScale);
	ptImg.y = (int)((ptStatic.y - m_ptOffset.y ) / m_fScale);
}

void CSplitStatic::Help_InitFlatCutInfo(int nFgp, int nObjType)
{
	bool	bLeft = (nFgp >= GAFIS7::CodeData::FINGER_L_THUMB) ? true : false;
	CRect	rtFlatView;
	int		nIndex = Help_GetFlatViewIndexByFgp(nFgp);

	m_ptFlatVector[nIndex].x = 1;
	m_ptFlatVector[nIndex].y = 0;

	switch ( nFgp )
	{
	case GAFIS7::CodeData::FINGER_R_THUMB:
	case GAFIS7::CodeData::FINGER_L_THUMB:
		rtFlatView.right = m_pstSrcObj->nWidth / 2;
		rtFlatView.left = rtFlatView.right - FLAT_WIDTH;
		rtFlatView.bottom = (m_pstSrcObj->nHeight + FLAT_HEIGHT) / 2;
		rtFlatView.top = rtFlatView.bottom - FLAT_HEIGHT;
		if ( !bLeft ) rtFlatView.OffsetRect(FLAT_WIDTH, 0);
		break;
	case GAFIS7::CodeData::FINGER_R_INDEX:
	case GAFIS7::CodeData::FINGER_L_INDEX:
		rtFlatView.left = 0;
		rtFlatView.right = rtFlatView.left + FLAT_WIDTH;
		rtFlatView.bottom = (m_pstSrcObj->nHeight + FLAT_HEIGHT) / 2;
		rtFlatView.top = rtFlatView.bottom - FLAT_HEIGHT;
		if ( bLeft ) rtFlatView.OffsetRect(m_pstSrcObj->nWidth - FLAT_WIDTH, 0);
		break;
	case GAFIS7::CodeData::FINGER_R_MIDDLE:
	case GAFIS7::CodeData::FINGER_L_MIDDLE:
		rtFlatView.right = m_pstSrcObj->nWidth / 2;
		rtFlatView.left = rtFlatView.right - FLAT_WIDTH;
		rtFlatView.top = 0;
		rtFlatView.bottom = rtFlatView.top + FLAT_HEIGHT;
		if ( bLeft ) rtFlatView.OffsetRect(FLAT_WIDTH, 0);
		break;
	case GAFIS7::CodeData::FINGER_R_RING:
	case GAFIS7::CodeData::FINGER_L_RING:
		rtFlatView.left = m_pstSrcObj->nWidth / 2;
		rtFlatView.right = rtFlatView.left + FLAT_WIDTH;
		rtFlatView.top = 0;
		rtFlatView.bottom = rtFlatView.top + FLAT_HEIGHT;
		if ( bLeft ) rtFlatView.OffsetRect(-FLAT_WIDTH, 0);
		break;
	case GAFIS7::CodeData::FINGER_R_LITTLE:
	case GAFIS7::CodeData::FINGER_L_LITTLE:
		rtFlatView.left = 0;
		rtFlatView.right = rtFlatView.left + FLAT_WIDTH;
		rtFlatView.bottom = m_pstSrcObj->nHeight;
		rtFlatView.top = rtFlatView.bottom - FLAT_HEIGHT;
		if ( !bLeft ) rtFlatView.OffsetRect(m_pstSrcObj->nWidth - FLAT_WIDTH, 0);
		break;
	}

	Help_PtOnImgToPtOnStatic(rtFlatView.TopLeft(), m_rtFlatView[nIndex].TopLeft());
	Help_PtOnImgToPtOnStatic(rtFlatView.BottomRight(), m_rtFlatView[nIndex].BottomRight());

	//if ( m_pCaptView->m_pCtrl->m_ConfigInfo.FingerIsNotExist(nFgp) ) m_nSpliteStatus[nIndex] = SPLITSTATUS_NOFINGER;
	/*else*/ m_nSpliteStatus[nIndex] = SPLITSTATUS_NOTLOCATE;
	m_nObjectType[nIndex] = nObjType;
}

void CSplitStatic::Help_GetFlatCutByRegion(REGIONRECT *pstObj, int nFlatIndex)
{
	Gfp_DegreeToPoint(-pstObj->nAngle, &m_ptFlatVector[nFlatIndex]);

	CRect	rtImage(pstObj->y, pstObj->x, pstObj->y + pstObj->nWidth, pstObj->x + pstObj->nHeight);

	Help_PtOnImgToPtOnStatic(rtImage.TopLeft(), m_rtFlatView[nFlatIndex].TopLeft());
	Help_PtOnImgToPtOnStatic(rtImage.BottomRight(), m_rtFlatView[nFlatIndex].BottomRight());
	m_nSpliteStatus[nFlatIndex] = SPLITSTATUS_LOCATED;
	m_nObjectType[nFlatIndex] = pstObj->nObjectType;
}

void CSplitStatic::Help_GetFlatCutInfo(REGIONRECT *pstObj)
{
	int		nFgp = Help_GetFlatFgpByObjType(pstObj->nObjectType);
	if ( nFgp < GAFIS7::CodeData::FINGER_R_THUMB ) return;

	if ( !pstObj->beExist )
	{
		Help_InitFlatCutInfo(nFgp, pstObj->nObjectType);
		return;
	}

	int		nIndex = Help_GetFlatViewIndexByFgp(nFgp);

	Help_GetFlatCutByRegion(pstObj, nIndex);
}

void CSplitStatic::InitSplitFlatPlace()
{
	if (!m_pbtyStatus || !m_pstSrcObj || !m_pstDesObj || ( m_nDesObj < 1 ) ) return;

	CRect	rtClient;
	GetClientRect(rtClient);

	double	fScale, ftemp;

	ftemp = m_pstSrcObj->nWidth;
	m_fScale = (rtClient.Width() / 4 * 4) / ftemp;

	ftemp = m_pstSrcObj->nHeight;
	fScale = rtClient.Height() / ftemp;
	if ( fScale < m_fScale ) m_fScale = fScale;

	m_nViewWidth = m_pstSrcObj->nWidth * m_fScale;
	m_nViewWidth = m_nViewWidth / 4 * 4;
	m_nViewHeight = m_pstSrcObj->nHeight * m_fScale;

	m_ptOffset.x = (rtClient.Width() - m_nViewWidth) / 2;
	m_ptOffset.y = (rtClient.Height() - m_nViewHeight) / 2;

	m_pViewImage = (UCHAR*)GAFIS_MALLOC(m_nViewWidth * m_nViewHeight);
	if ( !m_pViewImage ) return;

	GYTHHELPER_ZoomImage(m_pbtyStatus->pstCaptImage->pBitData, m_pstSrcObj->nWidth, m_pstSrcObj->nHeight, 
			m_pViewImage, m_nViewWidth, m_nViewHeight, m_pbtyStatus->pstCaptImage->pbmInfo->bmiHeader.biBitCount);

	m_nMaxSplitFlat = CUtilHelper::SlapIsRLThumb(m_pbtyStatus->Fgp) ? 2 : 4;

	//!< 设置各个截取框的显示位置和旋转向量
	for ( int i = 0; i < m_nDesObj; ++i ) Help_GetFlatCutInfo(m_pstDesObj + i);
}


BEGIN_MESSAGE_MAP(CSplitStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CSplitStatic message handlers

void CSplitStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	DrawSplitInfo();
}

BOOL CSplitStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}

extern CUtilHelper		m_utilHelper;

void CSplitStatic::DrawViewImage(CDC *pDC)
{
	GYTH_256BITMAPINFO	stbmpInfo = {0};

	memcpy(&stbmpInfo, &m_utilHelper.m_stbi, sizeof(stbmpInfo));
	stbmpInfo.bmiHeader.biWidth = m_nViewWidth;
	stbmpInfo.bmiHeader.biHeight = -m_nViewHeight;

	::SetDIBitsToDevice(pDC->GetSafeHdc(), m_ptOffset.x, m_ptOffset.y,
		m_nViewWidth, m_nViewHeight, 0, 0,
		0, m_nViewHeight, m_pViewImage, (BITMAPINFO*)&stbmpInfo, DIB_RGB_COLORS);
}

void CSplitStatic::DrawFlatView(CDC *pDC)
{
	CPen	*pOldPen, penFlat(PS_SOLID, 2, RGB(0, 0, 255));
	CFont	*pOldFont;
	CBrush	*pOldBrush;
	int		crText, nFgpIdx;
	int		oldBkMode;
	
	oldBkMode = pDC->SetBkMode(TRANSPARENT);
	crText = pDC->SetTextColor(CUtilHelper::m_crHandDescText);
//	pOldFont = pDC->SelectObject(&m_pCaptView->m_dlgCaptArea.m_staticFont);
	pOldPen = pDC->SelectObject(&penFlat);
	pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

	for ( int i = 0; i < m_nMaxSplitFlat; ++i )
	{
		if ( m_nSpliteStatus[i] == SPLITSTATUS_NOFINGER ) continue;

		CGfRectTracker gfTracker(m_rtFlatView[i], m_ptFlatVector[i], NULL);
		gfTracker.SetResizeFlage(Help_GetTrackerResizeFlag(m_nSpliteStatus[i]));
		gfTracker.m_bNoFinger = Help_IsFingerNotExist(m_nSpliteStatus[i]);
		gfTracker.DrawTracker(pDC);

		nFgpIdx = CUtilHelper::FingerFGPToIndex(Help_GetFlatFgpByObjType(m_nObjectType[i]));
		pDC->DrawText(CUtilHelper::m_szFingerName[nFgpIdx], ga_strlen(CUtilHelper::m_szFingerName[nFgpIdx]),
			m_rtFlatView[i],  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
//	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(crText);
	pDC->SetBkMode(oldBkMode);
	penFlat.DeleteObject();
}

void CSplitStatic::DrawSplitInfo()
{
	if ( !m_pViewImage ) return;

	CDC	*pDC = GetDC();
	CDC	dc;
	CBitmap	bmp, *poldbmp;
	CBrush	brush(RGB(255, 255, 255));
	CRect	rtClient;

	GetClientRect(rtClient);

	dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rtClient.Width(), rtClient.Height());
	poldbmp = dc.SelectObject(&bmp);

	dc.FillRect(&rtClient, &brush);
	DrawViewImage(&dc);
	DrawFlatView(&dc);

	pDC->BitBlt(rtClient.left, rtClient.top, rtClient.Width(), rtClient.Height(), &dc, 0, 0, SRCCOPY);

	dc.SelectObject(poldbmp);
	brush.DeleteObject();

	ReleaseDC(pDC);
}

int	CSplitStatic::Help_PointInFlatView(CPoint point)
{
	int		nExtent = 3;
	CRect	rect;
	POINT	ptVector, ptSrc, ptRotate;

	for ( int i = 0; i < m_nMaxSplitFlat; ++i )
	{
		if ( m_nSpliteStatus[i] == SPLITSTATUS_NOFINGER ) continue;

		rect.SetRect(0, 0, m_rtFlatView[i].Width(), m_rtFlatView[i].Height());
		rect.InflateRect(nExtent, nExtent);
		ptVector.x = m_ptFlatVector[i].x;
		ptVector.y = -m_ptFlatVector[i].y;

		ptSrc.x = point.x - m_rtFlatView[i].left;
		ptSrc.y = point.y - m_rtFlatView[i].top;
		ptRotate.x = ptRotate.y = 0;

		RotatePointByVector(ptSrc, ptVector, &ptRotate);
		if ( rect.PtInRect(ptRotate) ) return i;
	}
	return -1;
}

void CSplitStatic::Help_MouseMoveWhenNone(UINT nFlags, CPoint point)
{
	int		nIndex = Help_PointInFlatView(point);
	
	if ( m_nFlatIdxMouseOver != nIndex ) 
	{
		m_nFlatIdxMouseOver = nIndex;
		//DrawSplitInfo();
	}

	CRect	rect(0, 0, 0, 0);
	POINT	ptVector = {1, 0};
	int		nSplitStatus = SPLITSTATUS_NOTLOCATE;

	if ( nIndex >= 0 )
	{
		rect = m_rtFlatView[nIndex];
		ptVector = m_ptFlatVector[nIndex];
		nSplitStatus = m_nSpliteStatus[nIndex];
	}

	int		nresizeFlag = Help_GetTrackerResizeFlag(nSplitStatus);
	CGfRectTracker gfTracker(rect, ptVector, NULL);
	
	gfTracker.SetResizeFlage(nresizeFlag);
	gfTracker.AboutOnMouseMove(point, this);
}

void CSplitStatic::Help_MouseMoveWhenLBtnDown(UINT nFlags, CPoint point)
{

}

void CSplitStatic::Help_MouseMoveWhenRBtnDown(UINT nFlags, CPoint point)
{
	if ( m_nCurSelFlat < 0 ) return;

	CPoint	pts, pte;
	POINT	ptVector = {1, 0};

	pts.x = m_ptStart.x - m_rtFlatView[m_nCurSelFlat].left;
	pts.y = m_ptStart.y - m_rtFlatView[m_nCurSelFlat].top;

	pte.x = point.x - m_rtFlatView[m_nCurSelFlat].left;
	pte.y = point.y - m_rtFlatView[m_nCurSelFlat].top;

	CalculateRotateVector(pts, pte, m_ptFlatVector[m_nCurSelFlat], &ptVector);
	memcpy(&m_ptFlatVector[m_nCurSelFlat], &ptVector, sizeof(ptVector));

	m_ptStart = point;
	DrawSplitInfo();
}

void CSplitStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ( !m_pViewImage ) return;

	if ( m_nMouseOp == MOUSEOP_RBTNDOWN ) Help_MouseMoveWhenRBtnDown(nFlags, point);
	//else if ( m_nMouseOp == MOUSEOP_LBTNDOWN ) Help_MouseMoveWhenLBtnDown(nFlags, point);
	else Help_MouseMoveWhenNone(nFlags, point);

	CStatic::OnMouseMove(nFlags, point);
}

//!< 重新定位
void CSplitStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ( !m_pViewImage || ( m_nMouseOp == MOUSEOP_RBTNDOWN ) ) return;

	int		nIndex = Help_PointInFlatView(point);
	if ( nIndex < 0 )
	{
		CStatic::OnLButtonDblClk(nFlags, point);
		return;
	}
	if ( m_nSpliteStatus[nIndex] >= SPLITSTATUS_NOFINGER ) return;

	CRect	rtReal, rtBound, rtImage;

	Help_GetBoundRect(rtBound, m_rtFlatView[nIndex], m_ptFlatVector[nIndex]);
	Help_PtOnStaticToPtOnImg(rtBound.TopLeft(), rtReal.TopLeft());
	Help_PtOnStaticToPtOnImg(rtBound.BottomRight(), rtReal.BottomRight());

	rtImage.SetRect(0, 0, m_pstSrcObj->nWidth, m_pstSrcObj->nHeight);
	rtBound.IntersectRect(rtReal, rtImage);
	rtReal = rtBound;

	REGIONRECT stSrcRegion, stDesRegion;
	int		retval;

	memset(&stSrcRegion, 0, sizeof(stSrcRegion));
	memset(&stDesRegion, 0, sizeof(stDesRegion));

	stSrcRegion.y = rtReal.left;
	stSrcRegion.x = rtReal.top;
	stSrcRegion.nWidth = rtReal.Width();
	stSrcRegion.nHeight = rtReal.Height();
	stSrcRegion.nObjectType = Help_RelocateGetSrcObjType(m_nObjectType[nIndex]);
	stDesRegion.nObjectType = m_nObjectType[nIndex];

#ifdef UTIL_GNE_ZKY_MATCH
	retval = GNE_GAFIS_SpliteImageNew(m_pbtyStatus->pstCaptImage->pBitData, m_pstSrcObj->nWidth, 
		m_pstSrcObj->nHeight, &stSrcRegion, &stDesRegion, 1);
#else
	retval = GAFIS_SpliteImageNew(m_pbtyStatus->pstCaptImage->pBitData, m_pstSrcObj->nWidth, 
		m_pstSrcObj->nHeight, &stSrcRegion, &stDesRegion, 1);
#endif


	if ( (retval >= 0) && stDesRegion.beExist )
	{
		Help_GetFlatCutByRegion(&stDesRegion, nIndex);
		DrawSplitInfo();
	}

	CStatic::OnLButtonDblClk(nFlags, point);
}

void CSplitStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ( !m_pViewImage || ( m_nMouseOp == MOUSEOP_RBTNDOWN ) ) return;
	
	int		nIndex = Help_PointInFlatView(point);
	if ( nIndex < 0 )
	{
		CStatic::OnLButtonDown(nFlags, point);
		return;
	}

	CRect	rtOldClip, rtCurClip;

	GetClipCursor(&rtOldClip);
	GetWindowRect(rtCurClip);
	ClipCursor(rtCurClip);

	//m_nCurSelFlat = nIndex;

	CGfRectTracker gfTracker(m_rtFlatView[nIndex], m_ptFlatVector[nIndex], NULL);
	gfTracker.SetResizeFlage(Help_GetTrackerResizeFlag(m_nSpliteStatus[nIndex]));

	int		nHit = gfTracker.HitTest(point);
	if ( nHit >= 0 && gfTracker.Gf_Track(this, point) )
	{
		CRect	rtNew;
		int		ntemp;

		gfTracker.GetTrackerRect(&rtNew, ntemp, ntemp);
		m_rtFlatView[nIndex] = rtNew;
		DrawSplitInfo();
	}
	
	ClipCursor(rtOldClip);

	CStatic::OnLButtonDown(nFlags, point);
}

void CSplitStatic::OnLButtonUp(UINT nFlags, CPoint point)
{

	CStatic::OnLButtonUp(nFlags, point);
}


//!< 从(没有)定位 < -- > 缺指之间的转换
void CSplitStatic::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ( !m_pViewImage || ( m_nMouseOp == MOUSEOP_RBTNDOWN ) ) return;

	int		nIndex = Help_PointInFlatView(point);
	if ( nIndex < 0 )
	{
		CStatic::OnRButtonDblClk(nFlags, point);
		return;
	}

	int		nSplitStatus = m_nSpliteStatus[nIndex];

	if ( nSplitStatus == SPLITSTATUS_NOTLOCATE ) nSplitStatus = SPLITSTATUS_LOCATED;
	else if ( nSplitStatus == SPLITSTATUS_LOCATED ) nSplitStatus = SPLITSTATUS_NOTLOCATE;
	else 
	{
		CStatic::OnRButtonDblClk(nFlags, point);
		return;
	}
	
	m_nSpliteStatus[nIndex] = nSplitStatus;
	DrawSplitInfo();

	CStatic::OnRButtonDblClk(nFlags, point);
}

void CSplitStatic::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ( !m_pViewImage ) return;

	int		nIndex = Help_PointInFlatView(point);
	if ( nIndex < 0 )
	{
		CStatic::OnRButtonDown(nFlags, point);
		return;
	}

	m_nCurSelFlat = nIndex;
	SetCapture();
	m_nMouseOp = MOUSEOP_RBTNDOWN;
	m_ptStart = point;

	CStatic::OnRButtonDown(nFlags, point);
}

void CSplitStatic::OnRButtonUp(UINT nFlags, CPoint point)
{
	if ( m_nMouseOp == MOUSEOP_RBTNDOWN ) 
	{
		ReleaseCapture();
		m_nMouseOp = MOUSEOP_NONE;
	}
	m_nCurSelFlat = -1;

	CStatic::OnRButtonUp(nFlags, point);
}

void CSplitStatic::GetSplittedFlatData()
{
	UCHAR	*pCuttedData = NULL;
	int		nCuttedData = FLAT_WIDTH * FLAT_HEIGHT;

	pCuttedData = (UCHAR*)GAFIS_MALLOC(nCuttedData);
	if ( !pCuttedData ) return;

	SIZE	szSrc, szDes, szBox;
	POINT	ptOrigin;
	CRect	rect;
	int		retval, nw, nh, nFgp;

	szSrc.cx = m_pstSrcObj->nWidth;
	szSrc.cy = m_pstSrcObj->nHeight;

	for ( int i = 0; i < m_nMaxSplitFlat; ++i )
	{
		if ( m_nSpliteStatus[i] != SPLITSTATUS_LOCATED ) continue;

		nFgp = Help_GetFlatFgpByObjType(m_nObjectType[i]);
		if ( !Help_FgpNeedSplit(nFgp) ) continue;

		Help_PtOnStaticToPtOnImg(m_rtFlatView[i].TopLeft(), rect.TopLeft());
		Help_PtOnStaticToPtOnImg(m_rtFlatView[i].BottomRight(), rect.BottomRight());

		nw = rect.Width();
		if ( nw > FLAT_WIDTH ) nw = FLAT_WIDTH;
		nh = rect.Height();
		if ( nh > FLAT_HEIGHT ) nh = FLAT_HEIGHT;

		szDes.cx = szBox.cx = nw;
		szDes.cy = szBox.cy = nh;

		ptOrigin.x = rect.left;	ptOrigin.y = rect.top;
		retval = Gfp_CutBoxImage(pCuttedData, m_pbtyStatus->pstCaptImage->pBitData, 
			szDes, szSrc, szBox, ptOrigin, m_ptFlatVector[i], 255);
		if ( retval != 1 ) continue;

		//Help_SetPointedFlatData(pCuttedData, nw, nh, nFgp);
		CString	strTemp;

		switch (m_pstSrcObj->nObjectType)
		{
		case HANDSPLITE_OBJTYPE_RLTHUMB:
			strTemp.Format(_T("rlthumb_%d.bmp"), nFgp);
			break;
		case HANDSPLITE_OBJTYPE_RPLAINFOUR:
			strTemp.Format(_T("%s_four_%d.bmp"), "r", nFgp);
			break;
		case HANDSPLITE_OBJTYPE_LPLAINFOUR:
			strTemp.Format(_T("%s_four_%d.bmp"), "l", nFgp);
			break;
		}
		
		GAFIS_SaveDataAsBMP(strTemp, pCuttedData, nw, nh, 8);
	}

	GAFIS_free(pCuttedData);
}

bool CSplitStatic::Help_FgpNeedSplit(int nFgp)
{
	using namespace GAFIS7::CodeData;
#if 0
	CgythFingerPalmCtrl	*pCtrl = m_pCaptView->m_pCtrl;
	if ( pCtrl->m_ConfigInfo.FingerIsDisabled(nFgp, GAFIS7::CodeData::FINGER_VIEWID_FLAT) ) return false;

	int		nIndex = CUtilHelper::FingerFGPToIndex(nFgp);

	if ( pCtrl->m_ConfigInfo.m_bNotSplitIfExist && CConfigInfo::IsCapturedDataValid(&pCtrl->m_stPlainData[nIndex]) )
	{
		return false;
	}
#endif
	return true;
}

int CSplitStatic::Help_SetPointedFlatData(UCHAR *pCutFlatData, int nCutWidth, int nCutHeight, int nFgp)
{
	using namespace GAFIS7::CodeData;
#if 0
	CgythFingerPalmCtrl	*pCtrl = m_pCaptView->m_pCtrl;

	if ( pCtrl->m_ConfigInfo.FingerIsDisabled(nFgp, GAFIS7::CodeData::FINGER_VIEWID_FLAT) ) return 0;

	int		nIndex = CUtilHelper::FingerFGPToIndex(nFgp);
	CImageStatic	*pFlatStatic = &m_pCaptView->m_dlgCaptArea.m_staticFgPlain[nIndex];
	CBTYStatus	&btyStatus = pFlatStatic->m_BtyStatus;

	GYTH_CAPTIMAGEDATA *pstBTYCaptData = btyStatus.pstCaptImage;
	int		nDesWidth, nDesHeight, nBPP, retval;	//nDesLine, nImgSize;

	nDesWidth = pCtrl->m_CaptCard.GetBfvWidth(BTY_FINGER, FINGER_R_THUMB, 0);
	nDesHeight = pCtrl->m_CaptCard.GetBfvHeight(BTY_FINGER, FINGER_R_THUMB, 0);
	if ( (nDesWidth < 1) || (nDesHeight < 1) )
	{
		nDesWidth  = pCtrl->m_CaptCard.GetBTYDefWidth(BTY_FINGER, nFgp);
		nDesHeight = pCtrl->m_CaptCard.GetBTYDefHeight(BTY_FINGER, nFgp);
	}
	if ( (nDesWidth < 1) || (nDesHeight < 1) ) return GFS_DEVICE_ERROR_UNKNOWN;
	nBPP = m_pbtyStatus->pstCaptImage->pbmInfo->bmiHeader.biBitCount;

	retval = m_pCaptView->m_utilHelper.UTIL_BuildCaptImageData(*pstBTYCaptData, nDesWidth, nDesHeight, nBPP,
		pCutFlatData, nCutWidth, nCutHeight, CUtilHelper::GetImageLine(nCutWidth, nBPP), nBPP);
	if ( retval < 1 ) return GFS_DEVICE_ERROR_MEMORY;

	//!< 进行质量检查和指位检验
	if ( Help_QualityAndFgpCheck(btyStatus) < 1 ) return GFS_DEVICE_ERROR_UNKNOWN;

#ifdef DEBUG
	btyStatus.SaveCaptImage2File();
#endif // DEBUG

	btyStatus.AddCaptImage2ExtractQueue(pCtrl);
#endif
	return GFS_DEVICE_ERROR_SUCCESS;
}

int CSplitStatic::Help_QualityAndFgpCheck(CBTYStatus &btyStatus)
{
	int		retval, nIndex, nError;
	char	szErrorInfo[256] = {0};
#if 0
	CFingerQualityInfo	&fingerQual = m_pCaptView->m_FingerQual;

	m_pCaptView->m_dlgFrameArea.m_staticFrame.SetBTYCaptData(btyStatus.pstCaptImage);

	nIndex = CUtilHelper::FingerFGPToIndex(btyStatus.Fgp);
	nError = 0;
	
	//!< 此时不需要语音
	m_pCaptView->m_pCtrl->m_pSoundPlayer->doSilence(true);
	m_pCaptView->ExtractFingerMntEx(btyStatus, nError, szErrorInfo, sizeof(szErrorInfo) - 1,
		fingerQual.m_nPlainState[nIndex], fingerQual.m_nPlainExf[nIndex]);
	m_pCaptView->m_pCtrl->m_pSoundPlayer->doSilence(false);

	m_pCaptView->m_dlgOpTipInfo.m_editOpTipInfo.UpdateWindow();

	retval = m_pCaptView->CheckFingerImageQualityEx(btyStatus, nError, szErrorInfo, true);
	if ( retval > 0 ) retval = m_pCaptView->CheckFingerCrossAndSame(btyStatus);

	if ( retval > 0 ) return retval;

	btyStatus.pstCaptImage->bDataIsValid = 0;
//	btyStatus.pstCaptImage->bDrawInFrame = 1;
	fingerQual.m_nPlainState[nIndex] = 0;
	fingerQual.m_nPlainExf[nIndex] = 0;
#endif
	return 0;
}

bool CSplitStatic::Help_FlatFingerIsNotExist(int nObjType)
{
	int		nFgp = Help_GetFlatFgpByObjType(nObjType);

	if ( nFgp < 0 ) return true;
#if 0
	if ( !m_pCaptView || !m_pCaptView->m_pCtrl ) return true;

	return m_pCaptView->m_pCtrl->m_ConfigInfo.m_bFingerNotExist[CUtilHelper::FingerFGPToIndex(nFgp)];
#endif
}

int CSplitStatic::Help_RelocateGetSrcObjType(int nDesObjType)
{
	if ( nDesObjType == 21 ) return 31; //!< 右手拇指
	if ( nDesObjType == 26 ) return 33;
	return nDesObjType;
}

BOOL CSplitStatic::Help_IsFingerNotExist(int nSpliteStatus)
{
	return (nSpliteStatus != SPLITSTATUS_LOCATED) ? TRUE : FALSE;
}

void CSplitStatic::Help_GetBoundRect(CRect &rtBound, CRect &rect, POINT &ptVector)
{
	CGfRectTracker	gfTracker(rect, ptVector, NULL);
	POINT	*pptVertex;

	pptVertex = gfTracker.m_stTrackInfo.ptVertex;
	rtBound.SetRect(pptVertex[0], pptVertex[2]);

	for (int i = 0; i < 4; ++i, ++pptVertex )
	{
		if ( pptVertex->x < rtBound.left ) rtBound.left = pptVertex->x;
		if ( pptVertex->y < rtBound.top ) rtBound.top = pptVertex->y;
		if ( pptVertex->x > rtBound.right ) rtBound.right = pptVertex->x;
		if ( pptVertex->y > rtBound.bottom ) rtBound.bottom = pptVertex->y;
	}
}