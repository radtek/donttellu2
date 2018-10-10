// GfRectTracker.cpp: implementation of the CGfRectTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <MATH.H>
#include "GfRectTracker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GF_RECTTRACKER_PI				(3.14159265358979323846)
#define GF_RECTTRACKER_SPLITDEGREE		23


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGfRectTracker::CGfRectTracker()
{
	Initialize();
}

CGfRectTracker::CGfRectTracker(LPRECT lprect, POINT ptVector, LPPOINT pptOrigin)
{
	Initialize();

	m_rect.CopyRect(lprect);

	if(pptOrigin)
		memcpy(&m_stTrackInfo.ptOrigin, pptOrigin, sizeof(POINT));
	else
	{
		m_stTrackInfo.ptOrigin.x = m_rect.left;
		m_stTrackInfo.ptOrigin.y = m_rect.top;
	}

	SetRectTrackerVector(ptVector);
}

CGfRectTracker::CGfRectTracker(LPRECT lprect, double fVector, LPPOINT pptOrigin)
{
	Initialize();

	m_rect.CopyRect(lprect);

	if(pptOrigin)
		memcpy(&m_stTrackInfo.ptOrigin, pptOrigin, sizeof(POINT));
	else
	{
		m_stTrackInfo.ptOrigin.x = m_rect.left;
		m_stTrackInfo.ptOrigin.y = m_rect.top;
	}

	SetRectTrackerVector(fVector);
}

/////////////////////////////////////////////////////////////////////////////
// CGfRectTracker intitialization

void CGfRectTracker::Initialize()
{
	m_rect.SetRectEmpty();

	memset(&m_stTrackInfo, 0, sizeof(GFRECTTRACKINFO));
	m_stTrackInfo.ptVector.x = 1;

	m_nresizeFlag = GFRT_RESIZE_FLAG_ALL;
	
	m_nHandleSize = GFP_RTTRACKER_HANDSIZE;

	m_sizeMin.cy = m_sizeMin.cx = m_nHandleSize * 12;
	m_sizeMinSplit.cx = m_sizeMinSplit.cy = m_nHandleSize * 6;

//	m_fxscale = m_fyscale = 1.0;

	m_bSplit = FALSE;
	m_bNoFinger = FALSE;

	m_stTrackInfo.nHSplit = m_stTrackInfo.nVSplit = 0;

	m_clrTop = RGB(255, 0, 0);
	m_hpenTop = CreatePen(PS_SOLID, 2, m_clrTop);
	m_hpenBlackDotted = CreatePen(PS_DOT, 0, RGB(0, 0, 0));

	m_rectLast.SetRectEmpty();
	m_sizeLast.cx = m_sizeLast.cy = 0;
	m_bErase = FALSE;
	m_bFinalErase = FALSE;

	m_ptSplit.x = m_ptSplit.y = 0;

	memset(&m_stTrackInfoLast, 0, sizeof(GFRECTTRACKINFO));

	m_bNeedClip = FALSE;
	m_rtClipArea.SetRectEmpty();
}


CGfRectTracker::~CGfRectTracker()
{
	if(m_hpenTop)
		::DeleteObject(m_hpenTop);

	if(m_hpenBlackDotted)
		::DeleteObject(m_hpenBlackDotted);
}


/////////////////////////////////////////////////////////////////////////////
// CGfRectTracker operations

void CGfRectTracker::SetTopflagColor(COLORREF clrTop)
{
	m_clrTop = clrTop;

	if(m_hpenTop)
		::DeleteObject(m_hpenTop);

	m_hpenTop = CreatePen(PS_SOLID, 2, m_clrTop);
}

void CGfRectTracker::SetResizeFlage(UINT nresizeFlag)
{
	m_nresizeFlag = nresizeFlag;
}

/*
void CGfRectTracker::SetRectTrackerScale(double xscale, double yscale)
{
	m_fxscale = xscale;
	m_fyscale = yscale;
}
*/

void CGfRectTracker::SetSplitValue(BOOL bSplit, int hSplit, int vSplit)
{
	if(m_rect.IsRectNull())
		return;

	m_bSplit = bSplit;

	m_stTrackInfo.nHSplit = hSplit;
	m_stTrackInfo.nVSplit = vSplit;

	m_ptSplit.y = m_rect.top  + hSplit;
	m_ptSplit.x = m_rect.left + vSplit; 

	RotateSplitByVector(m_rect, m_stTrackInfo);
}

void CGfRectTracker::SetRectTrackerVector(POINT ptVector)
{
	double fangle;

	memcpy(&m_stTrackInfo.ptVector, &ptVector, sizeof(POINT));
	fangle = Point2Angle(ptVector);

	m_stTrackInfo.fVector = fangle;

	RotateRectByVector(m_rect, m_stTrackInfo);
	RotateSplitByVector(m_rect, m_stTrackInfo);
}

void CGfRectTracker::SetRectTrackerVector(double fVector)
{
	POINT ptVector;

	Angle2Point(ptVector, fVector);

	SetRectTrackerVector(ptVector);
}

void CGfRectTracker::ChangeRotateOrigin(POINT ptOrigin)
{
	m_stTrackInfo.ptOrigin.x = ptOrigin.x;
	m_stTrackInfo.ptOrigin.y = ptOrigin.y;

	RotateRectByVector(m_rect, m_stTrackInfo);
	RotateSplitByVector(m_rect, m_stTrackInfo);
}

void CGfRectTracker::GetTrackerRect(LPRECT lprect, int &nHsplit, int &nVsplit)
{
	memcpy(lprect, (LPRECT)&m_rect, sizeof(RECT));
	nHsplit = m_stTrackInfo.nHSplit;
	nVsplit = m_stTrackInfo.nVSplit;
}

void CGfRectTracker::SetTrackedRect(LPRECT lprect, LPPOINT pptOrigin /* = NULL */)
{
	m_rect.CopyRect(lprect);

	if(pptOrigin)
		memcpy(&m_stTrackInfo.ptOrigin, pptOrigin, sizeof(POINT));
	else
	{
		m_stTrackInfo.ptOrigin.x = m_rect.left;
		m_stTrackInfo.ptOrigin.y = m_rect.top;
	}

	m_ptSplit.y = m_rect.top  + m_stTrackInfo.nHSplit;
	m_ptSplit.x = m_rect.left + m_stTrackInfo.nVSplit; 

	RotateRectByVector(m_rect, m_stTrackInfo);
	RotateSplitByVector(m_rect, m_stTrackInfo);
}

void CGfRectTracker::DrawTracker(CDC *pDC)
{
	int nOldROP, i, bflag;
	POINT *ppt;
	CBrush* pOldBrush = NULL;
	HPEN	pOldhPen = NULL;
	CRect rt;

	nOldROP = pDC->SetROP2(R2_COPYPEN);

	if ( m_bNoFinger ) pOldhPen = (HPEN)pDC->SelectObject(m_hpenTop);

	pDC->MoveTo(m_stTrackInfo.ptVertex[0]);
	for(i = 1; i < 4; i++)
		pDC->LineTo(m_stTrackInfo.ptVertex[i]);
	pDC->LineTo(m_stTrackInfo.ptVertex[0]);

	if ( !m_bNoFinger )
	{
		pOldhPen = (HPEN)pDC->SelectObject(m_hpenTop);
		pDC->LineTo(m_stTrackInfo.ptVertex[1]);
		if ( pOldhPen ) pDC->SelectObject(pOldhPen);
		pOldhPen = NULL;
	}

	if(m_bSplit)
	{
		pDC->MoveTo(m_stTrackInfo.ptHSplit[0]);
		pDC->LineTo(m_stTrackInfo.ptHSplit[1]);
		pDC->MoveTo(m_stTrackInfo.ptVSplit[0]);
		pDC->LineTo(m_stTrackInfo.ptVSplit[1]);
	}
	pDC->SetROP2(nOldROP);

	pOldBrush = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);

	for(i = 0; i < 4; i++)
	{
		bflag = ((1 << i) & m_nresizeFlag);
		
		if(bflag)
		{
			ppt = m_stTrackInfo.ptVertex + i;
			rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize, 
					   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

			pDC->Rectangle(rt);
		}

		bflag = ((1 << (i + 4)) & m_nresizeFlag);

		if(bflag)
		{
			ppt = m_stTrackInfo.ptBorderMid + i;
			rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize, 
					   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

			pDC->Rectangle(rt);
		}
	}


	if(m_bSplit)
	{
		ppt = &m_stTrackInfo.ptHsplitMiddle;
		rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize, 
				   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

		pDC->Rectangle(rt);

		ppt = &m_stTrackInfo.ptVsplitMiddle;
		rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize, 
				   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

		pDC->Rectangle(rt);
	}

	if ( pOldBrush ) pDC->SelectObject(pOldBrush);
	if ( pOldhPen )  pDC->SelectObject(pOldhPen);
}

int CGfRectTracker::HitTest(CPoint point)
{
	int i, bflag;
	CRect rt;
	POINT *ppt, ptRotate;
	Gf_TrackerHit hitResult = gf_hitNothing;

	for(i = 0; i < 4; i++)
	{
		ppt = m_stTrackInfo.ptVertex + i;

		rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize,
				   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

		bflag = ((1 << i) & m_nresizeFlag);

		if(rt.PtInRect(point))
		{
			if(bflag)
			{
				hitResult = (Gf_TrackerHit)i;
				return hitResult;
			}

			break;
		}
	}

	for(i = 0; i < 4; i++)
	{
		ppt = m_stTrackInfo.ptBorderMid + i;

		rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize,
				   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

		bflag = ((1 << (i + 4)) & m_nresizeFlag);

		if(rt.PtInRect(point))
		{
			if(bflag)
			{
				hitResult = (Gf_TrackerHit)(gf_hitTop + i);
				return hitResult;
			}

			break;
		}
	}

	if(m_bSplit)
	{
		ppt = &m_stTrackInfo.ptHsplitMiddle;

		rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize,
				   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

		if(rt.PtInRect(point))
		{
			return gf_hitHsplitTop;
		}

		ppt = &m_stTrackInfo.ptVsplitMiddle;

		rt.SetRect(ppt->x - m_nHandleSize, ppt->y - m_nHandleSize,
				   ppt->x + m_nHandleSize, ppt->y + m_nHandleSize);

		if(rt.PtInRect(point))
		{
			return gf_hitVsplitLeft;
		}
	}

	if(GFRT_RESIZE_FLAG_MOVE & m_nresizeFlag)
	{
		RotatePointByVector(point, ptRotate, -m_stTrackInfo.fVector, m_stTrackInfo.ptOrigin);
		if(m_rect.PtInRect(ptRotate))
			hitResult = gf_hitMiddle;
	}

	return hitResult;
}

void CGfRectTracker::AboutOnMouseMove(CPoint pt, CWnd* pWnd)
{
	int hit, nDegree;
	char* shap = IDC_ARROW;

	hit = HitTest(pt);
	
	nDegree = (int)(m_stTrackInfo.fVector * 180 / GF_RECTTRACKER_PI);
	if(nDegree < 0)
		nDegree += 360;

	switch(hit)
	{
		case gf_hitNothing:
			break;

		case gf_hitTopLeft:
		case gf_hitBottomRight:
			{
//				if((nDegree < GF_RECTTRACKER_SPLITDEGREE) || (nDegree >= 360 - GF_RECTTRACKER_SPLITDEGREE))
				if((nDegree < 23) || (nDegree >= 360 - 22))	// 0
					shap = IDC_SIZENWSE;
				else if(nDegree < 68)						// 45
					shap = IDC_SIZENS;
				else if(nDegree < 113)						// 90
					shap = IDC_SIZENESW;
				else if(nDegree < 158)						// 135
					shap = IDC_SIZEWE;
				else if(nDegree < 203)						// 180
					shap = IDC_SIZENWSE;
				else if(nDegree < 248)						// 225
					shap = IDC_SIZENS;
				else if(nDegree < 293)						// 270
					shap = IDC_SIZENESW;
				else
					shap = IDC_SIZEWE;
			}
			break;

		case gf_hitTopRight:
		case gf_hitBottomLeft:
			{
				if((nDegree < 23) || (nDegree >= 360 - 22))
					shap = IDC_SIZENESW;
				else if(nDegree < 68)
					shap = IDC_SIZEWE;
				else if(nDegree < 113)
					shap = IDC_SIZENWSE;
				else if(nDegree < 158)
					shap = IDC_SIZENS;
				else if(nDegree < 203)
					shap = IDC_SIZENESW;
				else if(nDegree < 248)
					shap = IDC_SIZEWE;
				else if(nDegree < 293)
					shap = IDC_SIZENWSE;
				else
					shap = IDC_SIZENS;
			}
			break;

		case gf_hitTop:
		case gf_hitBottom:
		case gf_hitHsplitTop:
			{
				if((nDegree < 23) || (nDegree >= 360 - 22))
					shap = IDC_SIZENS;
				else if(nDegree < 68)
					shap = IDC_SIZENESW;
				else if(nDegree < 113)
					shap = IDC_SIZEWE;
				else if(nDegree < 158)
					shap = IDC_SIZENWSE;
				else if(nDegree < 203)
					shap = IDC_SIZENS;
				else if(nDegree < 248)
					shap = IDC_SIZENESW;
				else if(nDegree < 293)
					shap = IDC_SIZEWE;
				else
					shap = IDC_SIZENWSE;
			}
			break;

		case gf_hitRight:
		case gf_hitLeft:
		case gf_hitVsplitLeft:
			{
				if((nDegree < 23) || (nDegree >= 360 - 22))
					shap = IDC_SIZEWE;
				else if(nDegree < 68)
					shap = IDC_SIZENWSE;
				else if(nDegree < 113)
					shap = IDC_SIZENS;
				else if(nDegree < 158)
					shap = IDC_SIZENESW;
				else if(nDegree < 203)
					shap = IDC_SIZEWE;
				else if(nDegree < 248)
					shap = IDC_SIZENWSE;
				else if(nDegree < 293)
					shap = IDC_SIZENS;
				else
					shap = IDC_SIZENESW;
			}
			break;

		case gf_hitMiddle:
			shap = IDC_SIZEALL;
			break;
	}
    HCURSOR newCursor = LoadCursor(NULL, shap);

	if(pWnd)
		::SetClassLong(pWnd->GetSafeHwnd(), GCL_HCURSOR, (LONG)newCursor);
//		::SetClassLongPtr(pWnd->GetSafeHwnd(), GCLP_HCURSOR, (LONG_PTR)newCursor);
}

BOOL CGfRectTracker::Gf_Track(CWnd* pWnd, CPoint point, CWnd* pWndClipTo /* = NULL */)
{
	BOOL bTrack;

	// perform hit testing on the handles
	int nHandle = HitTest(point);
	if (nHandle < 0)
	{
		// didn't hit a handle, so just return FALSE
		return FALSE;
	}

	// otherwise, call helper function to do the tracking
	bTrack = Gf_TrackHandle(nHandle, pWnd, point, pWndClipTo);
	if(bTrack)
	{
		CRect rect = m_rect;

		m_rect.left = m_stTrackInfo.ptVertex[0].x;
		m_rect.top  = m_stTrackInfo.ptVertex[0].y;

		m_rect.right  = m_rect.left + rect.Width();
		m_rect.bottom = m_rect.top  + rect.Height();

		m_ptSplit.x = m_rect.left + m_stTrackInfo.nVSplit;
		m_ptSplit.y = m_rect.top  + m_stTrackInfo.nHSplit;
	}

	return bTrack;
}

/////////////////////////////////////////////////////////////////////////////
// CGfRectTracker implementation helpers

double CGfRectTracker::Point2Angle(POINT &ptVector)
{
	double fangle, value;

	if((ptVector.x == 0) && (ptVector.y == 0))
		return 0.0;
	
	fangle = ptVector.x * ptVector.x + ptVector.y * ptVector.y;
	value = (ptVector.x) / sqrt(fangle);

	fangle = acos(value);

	if(ptVector.y < 0)
		fangle = -fangle;

	return fangle;
}

void CGfRectTracker::Angle2Point(POINT &ptVector, double fAngle)
{
	double factor = 10000.0;
	int x = 0, y = 0;

	x = (int)(cos(fAngle) * factor);
	y = (int)(sin(fAngle) * factor);

	ptVector.x = x;
	ptVector.y = y;
}

void CGfRectTracker::RotatePointByVector(POINT ptSrc, POINT &ptRotate, double fVector, POINT &ptOrigin)
{
	double fx, fy;
	POINT ptTmp;

	ptTmp.x = ptSrc.x - ptOrigin.x;
	ptTmp.y = ptSrc.y - ptOrigin.y;
	
	fx = cos(fVector);
	fy = sin(fVector);

	ptRotate.x = (int)(fx * ptTmp.x - fy * ptTmp.y) + ptOrigin.x;
	ptRotate.y = (int)(fy * ptTmp.x + fx * ptTmp.y) + ptOrigin.y;
}

void CGfRectTracker::RotateRectByVector(CRect &rect, GFRECTTRACKINFO &stTrackInfo)
{
	POINT pt[4], *pptSrc, *pptDes;
	int i;

	pt[0].x = rect.left;		pt[0].y = rect.top;
	pt[1].x = rect.right;		pt[1].y = rect.top;
	pt[2].x = rect.right;		pt[2].y = rect.bottom;
	pt[3].x = rect.left;		pt[3].y = rect.bottom;

	for(i = 0; i < 4; i++)	// 四个顶点
		RotatePointByVector(pt[i], stTrackInfo.ptVertex[i], stTrackInfo.fVector, m_stTrackInfo.ptOrigin);

	pptSrc = stTrackInfo.ptVertex;
	pptDes = stTrackInfo.ptBorderMid;

	pptDes[0].x = (pptSrc[0].x + pptSrc[1].x) / 2;	pptDes[0].y = (pptSrc[0].y + pptSrc[1].y) / 2;
	pptDes[1].x = (pptSrc[1].x + pptSrc[2].x) / 2;	pptDes[1].y = (pptSrc[1].y + pptSrc[2].y) / 2;
	pptDes[2].x = (pptSrc[2].x + pptSrc[3].x) / 2;	pptDes[2].y = (pptSrc[2].y + pptSrc[3].y) / 2;
	pptDes[3].x = (pptSrc[3].x + pptSrc[0].x) / 2;	pptDes[3].y = (pptSrc[3].y + pptSrc[0].y) / 2;

/*
	pt[0].x = (rect.left + rect.right) / 2;	pt[0].y = rect.top;
	pt[1].x = rect.right;					pt[1].y = (rect.top + rect.bottom) / 2;
	pt[2].x = (rect.left + rect.right) / 2;	pt[2].y = rect.bottom;
	pt[3].x = rect.left;					pt[3].y = (rect.top + rect.bottom) / 2;

	for(i = 0; i < 4; i++)	// 四条边的中点
		RotatePointByVector(pt[i], stTrackInfo.ptBorderMid[i], stTrackInfo.fVector, m_stTrackInfo.ptOrigin);
*/
}

void CGfRectTracker::RotateSplitByVector(CRect &rect, GFRECTTRACKINFO &stTrackInfo)
{
	if(!m_bSplit)
		return;

	POINT ptFromto[2], *pptSrc;
	int i;

	ptFromto[0].x = rect.left;	ptFromto[0].y = rect.top + stTrackInfo.nHSplit;
	ptFromto[1].x = rect.right;	ptFromto[1].y = rect.top + stTrackInfo.nHSplit;

	for(i = 0; i < 2; i++)
		RotatePointByVector(ptFromto[i], stTrackInfo.ptHSplit[i], stTrackInfo.fVector, m_stTrackInfo.ptOrigin);

	pptSrc = stTrackInfo.ptHSplit;
	stTrackInfo.ptHsplitMiddle.x = (pptSrc[0].x + pptSrc[1].x) / 2;
	stTrackInfo.ptHsplitMiddle.y = (pptSrc[0].y + pptSrc[1].y) / 2;

	ptFromto[0].x = rect.left + stTrackInfo.nVSplit;	ptFromto[0].y = rect.top;
	ptFromto[1].x = rect.left + stTrackInfo.nVSplit;	ptFromto[1].y = rect.bottom;

	for(i = 0; i < 2; i++)
		RotatePointByVector(ptFromto[i], stTrackInfo.ptVSplit[i], stTrackInfo.fVector, m_stTrackInfo.ptOrigin);

	pptSrc = stTrackInfo.ptVSplit;
	stTrackInfo.ptVsplitMiddle.x = (pptSrc[0].x + pptSrc[1].x) / 2;
	stTrackInfo.ptVsplitMiddle.y = (pptSrc[0].y + pptSrc[1].y) / 2;
}

BOOL CGfRectTracker::Gf_TrackHandle(int nHandle, CWnd* pWnd, CPoint point, CWnd* pWndClipTo)
{
	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	AfxLockTempMaps();  // protect maps while looping

	// save original width & height in pixels
	int nWidth  = m_rect.Width();
	int nHeight = m_rect.Height();
	int nHSplit = m_stTrackInfo.nHSplit;
	int nVSplit = m_stTrackInfo.nVSplit;
	
	// set capture to the window which received this message
	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());
	pWnd->UpdateWindow();
	if (pWndClipTo != NULL)
		pWndClipTo->UpdateWindow();
	
	CRect rectSave = m_rect;
	GFRECTTRACKINFO gfrectSave;
	POINT ptSave;
	memcpy(&ptSave, &m_ptSplit, sizeof(POINT));
	memcpy(&gfrectSave, &m_stTrackInfo, sizeof(GFRECTTRACKINFO));

	// find out what x/y coords we are supposed to modify
	int *pxStart, *pyStart, xDiff, yDiff;
	GetModifyPointers(nHandle, &pxStart, &pyStart, &xDiff, &yDiff);

	POINT ptRotate, ptRotTemp, ptCur;

	RotatePointByVector(point, ptRotate, -m_stTrackInfo.fVector, m_stTrackInfo.ptOrigin);

	xDiff = ptRotate.x - xDiff;
	yDiff = ptRotate.y - yDiff;

	// get DC for drawing
	CDC* pDrawDC;
	CRgn rgnClip;
	if (pWndClipTo != NULL)
	{
		// clip to arbitrary window by using adjusted Window DC
		pDrawDC = pWndClipTo->GetDCEx(NULL, DCX_CACHE);
	}
	else
	{
		// otherwise, just use normal DC
		pDrawDC = pWnd->GetDC();
		if(m_bNeedClip && !m_rtClipArea.IsRectEmpty())
		{
			rgnClip.CreateRectRgn(m_rtClipArea.left, m_rtClipArea.top, m_rtClipArea.right, m_rtClipArea.bottom);
			pDrawDC->SelectClipRgn(&rgnClip);
		}
	}

	CRect rectOld;
	GFRECTTRACKINFO gfrectOld;
	POINT	ptOld;
	BOOL bMoved = FALSE;

	// get messages until capture lost or cancelled/accepted
	for(;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != pWnd)
			break;

		switch (msg.message)
		{
		// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			rectOld = m_rect;
			memcpy(&ptOld, &m_ptSplit, sizeof(POINT));
			memcpy(&gfrectOld, &m_stTrackInfo, sizeof(GFRECTTRACKINFO));
			
			// handle resize cases (and part of move)
			ptCur.x = (int)(short)LOWORD(msg.lParam);
			ptCur.y = (int)(short)HIWORD(msg.lParam);

			RotatePointByVector(ptCur, ptRotTemp, -m_stTrackInfo.fVector, m_stTrackInfo.ptOrigin);

			if (pxStart != NULL)
				*pxStart = ptRotTemp.x - xDiff;
			if (pyStart != NULL)
				*pyStart = ptRotTemp.y - yDiff;

			// handle move case
			if (nHandle == gf_hitMiddle)
			{
				m_rect.right  = m_rect.left + nWidth;
				m_rect.bottom = m_rect.top  + nHeight;

				if(m_bSplit)
				{
					m_ptSplit.x = m_rect.left + nVSplit;
					m_ptSplit.y = m_rect.top  + nHSplit;
				}
			}

			// allow caller to adjust the rectangle if necessary
			AdjustRect(nHandle, &m_rect);

			// only redraw and callback if the rect actually changed!
			m_bFinalErase = (msg.message == WM_LBUTTONUP);
			if (!rectOld.EqualRect(&m_rect) || ptOld.x != m_ptSplit.x 
				|| ptOld.y != m_ptSplit.y || m_bFinalErase)
			{
				if (bMoved)
				{
					m_bErase = TRUE;
					DrawTrackerRect(&gfrectOld, pWndClipTo, pDrawDC, pWnd, &rectOld);
				}
				OnChangedRect(gfrectOld);
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (m_bFinalErase)
				goto ExitLoop;

			if (!rectOld.EqualRect(&m_rect) || ptOld.x != m_ptSplit.x || ptOld.y != m_ptSplit.y)
			{
				m_bErase = FALSE;
				RotateRectByVector(m_rect, m_stTrackInfo);
				RotateSplitByVector(m_rect, m_stTrackInfo);
				DrawTrackerRect(&m_stTrackInfo, pWndClipTo, pDrawDC, pWnd, &m_rect);
			}
			break;

		// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				m_bErase = m_bFinalErase = TRUE;
				RotateRectByVector(m_rect, m_stTrackInfo);
				RotateSplitByVector(m_rect, m_stTrackInfo);
				DrawTrackerRect(&m_stTrackInfo, pWndClipTo, pDrawDC, pWnd, &m_rect);
			}
			m_rect = rectSave;
			memcpy(&m_ptSplit, &ptSave, sizeof(POINT));
			memcpy(&m_stTrackInfo, &gfrectSave, sizeof(GFRECTTRACKINFO));
			goto ExitLoop;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	if (pWndClipTo != NULL)
		pWndClipTo->ReleaseDC(pDrawDC);
	else
	{
		rgnClip.DeleteObject();
		pWnd->ReleaseDC(pDrawDC);
	}
	ReleaseCapture();

	AfxUnlockTempMaps(FALSE);

	// restore rect in case bMoved is still FALSE
	if (!bMoved)
	{
		m_rect = rectSave;
		memcpy(&m_ptSplit, &ptSave, sizeof(POINT));
		memcpy(&m_stTrackInfo, &gfrectSave, sizeof(GFRECTTRACKINFO));
	}
	m_bFinalErase = FALSE;
	m_bErase = FALSE;

	// return TRUE only if rect has changed
	if(!rectSave.EqualRect(&m_rect))
		return TRUE;

	if((m_stTrackInfo.nHSplit != nHSplit) || (m_stTrackInfo.nVSplit != nVSplit))
		return TRUE;

	return FALSE;
}

void CGfRectTracker::GetModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py)
{
	*ppx = NULL;
	*ppy = NULL;
	
	switch(nHandle)
	{
	case gf_hitTopLeft:
	case gf_hitMiddle:
	default:
		*ppx = (int*)&m_rect.left;
		*ppy = (int*)&m_rect.top;
		break;

	case gf_hitTopRight:
		*ppx = (int*)&m_rect.right;
		*ppy = (int*)&m_rect.top;
		break;

	case gf_hitBottomRight:
		*ppx = (int*)&m_rect.right;
		*ppy = (int*)&m_rect.bottom;
		break;

	case gf_hitBottomLeft:
		*ppx = (int*)&m_rect.left;
		*ppy = (int*)&m_rect.bottom;
		break;

	case gf_hitTop:
		*ppy = (int*)&m_rect.top;
		if(px)
			*px = m_rect.left + abs(m_rect.Width()) / 2;
		break;

	case gf_hitRight:
		*ppx = (int*)&m_rect.right;
		if(py)
			*py = m_rect.top + abs(m_rect.Height()) / 2;
		break;

	case gf_hitBottom:
		*ppy = (int*)&m_rect.bottom;
		if(px)
			*px = m_rect.left + abs(m_rect.Width()) / 2;
		break;

	case gf_hitLeft:
		*ppx = (int*)&m_rect.left;
		if(py)
			*py = m_rect.top + abs(m_rect.Height()) / 2;
		break;

	case gf_hitHsplitTop:
		*ppy = (int*)&m_ptSplit.y;
		if(px)
			*px = m_rect.left + abs(m_rect.Width()) / 2;
		break;

	case gf_hitVsplitLeft:
		*ppx = (int*)&m_ptSplit.x;
		if(py)
			*py = m_rect.top + abs(m_rect.Height()) / 2;
		break;
	}

	if(*ppx && px)
		*px = **ppx;
	if(*ppy && py)
		*py = **ppy;

/*
	if(((nHandle >= 0) && (nHandle < 4)) || (nHandle == gf_hitMiddle))
	{
		*ppx = &m_stTrackInfo.ptVertex[nHandle].x;
		*ppy = &m_stTrackInfo.ptVertex[nHandle].y;
	}
	else if(nHandle < 8)
	{
		*ppx = &m_stTrackInfo.ptBorderMid[nHandle].x;
		*ppy = &m_stTrackInfo.ptBorderMid[nHandle].y;
	}
	else if(nHandle == gf_hitHsplitTop)
	{
		*ppx = &m_stTrackInfo.ptHsplitMiddle.x;
		*ppy = &m_stTrackInfo.ptHsplitMiddle.y;
	}
	else
	{
		*ppx = &m_stTrackInfo.ptVsplitMiddle.x;
		*ppy = &m_stTrackInfo.ptVsplitMiddle.y;
	}

	if(px)
		*px = **ppx;
	if(py)
		*py = **ppy;
*/
}

void CGfRectTracker::AdjustRect(int nHandle, LPRECT lpRect)
{
	if (nHandle == gf_hitMiddle)
		return;

	// convert the handle into locations within m_rect
	int *pxStart, *pyStart, nOff;
	GetModifyPointers(nHandle, &pxStart, &pyStart, NULL, NULL);
	
	int nNewWidth  = m_rect.Width();
	int nNewHeight = m_rect.Height();
	if(nHandle < gf_hitMiddle)
	{
		// enforce minimum width
		if (pxStart != NULL && nNewWidth < m_sizeMin.cx)
		{
			nNewWidth = 1;
			nOff = (int*)pxStart - (int*)&m_rect;
			if(nOff == 0)
				*pxStart = m_rect.right - nNewWidth * m_sizeMin.cx;
			else
				*pxStart = m_rect.left + nNewWidth * m_sizeMin.cx; 
		}

		// enforce minimum height
		if(pyStart != NULL && nNewHeight < m_sizeMin.cy)
		{
			nNewHeight = 1;
			nOff = (int*)pyStart - (int*)&m_rect;
			if(nOff == 1)
				*pyStart = m_rect.bottom - nNewHeight * m_sizeMin.cy;
			else
				*pyStart = m_rect.top + nNewHeight * m_sizeMin.cy;
		}

		m_ptSplit.x = m_rect.left + m_stTrackInfo.nVSplit;
		m_ptSplit.y = m_rect.top  + m_stTrackInfo.nHSplit;
	}
	else
	{
		if(nHandle == gf_hitHsplitTop)
		{
			nOff = m_ptSplit.y - m_rect.top;
			if(nOff < m_sizeMinSplit.cy)	
				m_ptSplit.y = m_sizeMinSplit.cy + m_rect.top;
			if(nOff >= nNewHeight - m_sizeMinSplit.cy)
				m_ptSplit.y = m_rect.bottom - m_sizeMinSplit.cy;
		}
		else
		{
			nOff = m_ptSplit.x - m_rect.left;
			if(nOff < m_sizeMinSplit.cx)	
				m_ptSplit.x = m_sizeMinSplit.cx + m_rect.left;
			if(nOff >= nNewWidth - m_sizeMinSplit.cx)
				m_ptSplit.x = m_rect.right - m_sizeMinSplit.cx;
		}

		m_stTrackInfo.nHSplit = m_ptSplit.y - m_rect.top;
		m_stTrackInfo.nVSplit = m_ptSplit.x - m_rect.left;
	}
}

void CGfRectTracker::OnChangedRect(const GFRECTTRACKINFO& /*stRectInfo*/)
{
	// no default implementation, useful for derived classes
}

void CGfRectTracker::DrawTrackerRect(GFRECTTRACKINFO* pstRectInfo, CWnd* pWndClipTo, 
									 CDC* pDC, CWnd* pWnd, LPRECT rect)
{
	// convert to client coordinates
	if (pWndClipTo != NULL)
	{
		// convert all point in pstRectInfo
//		pWnd->ClientToScreen(&);
//		pWndClipTo->ScreenToClient(&);
	}

	CSize size(0, 0);
	if (!m_bFinalErase)
	{
		size.cx = 1;
		size.cy = 1;
	}

	// and draw it
	if (m_bFinalErase || !m_bErase)
	{
		int nOldROP, i;
		CBrush* pBrushOld = NULL;
		CPen* pOldPen = NULL;
 		CPen drawPen(PS_DOT, 1,RGB(128,128,128));

//		pBrushOld = pDC->SelectObject(CDC::GetHalftoneBrush());
		pOldPen = pDC->SelectObject(&drawPen);
		nOldROP = pDC->SetROP2(R2_XORPEN);

		if(!m_rectLast.IsRectNull())
		{	
			pDC->MoveTo(m_stTrackInfoLast.ptVertex[0]);
			for(i = 1; i < 4; i++)
				pDC->LineTo(m_stTrackInfoLast.ptVertex[i]);
			pDC->LineTo(m_stTrackInfoLast.ptVertex[0]);
			if(m_bSplit)
			{
				pDC->MoveTo(m_stTrackInfoLast.ptHSplit[0]);
				pDC->LineTo(m_stTrackInfoLast.ptHSplit[1]);
				pDC->MoveTo(m_stTrackInfoLast.ptVSplit[0]);
				pDC->LineTo(m_stTrackInfoLast.ptVSplit[1]);
			}
		}
		
		pDC->MoveTo(pstRectInfo->ptVertex[0]);
		for(i = 1; i < 4; i++)
			pDC->LineTo(pstRectInfo->ptVertex[i]);
		pDC->LineTo(pstRectInfo->ptVertex[0]);
		if(m_bSplit)
		{
			pDC->MoveTo(pstRectInfo->ptHSplit[0]);
			pDC->LineTo(pstRectInfo->ptHSplit[1]);
			pDC->MoveTo(pstRectInfo->ptVSplit[0]);
			pDC->LineTo(pstRectInfo->ptVSplit[1]);
		}

		pDC->SetROP2(nOldROP);
		if(pBrushOld)
			pDC->SelectObject(pBrushOld);
		if(pOldPen)
			pDC->SelectObject(pOldPen);
	}
	
	// remember last recttracker
	memcpy(&m_stTrackInfoLast, pstRectInfo, sizeof(GFRECTTRACKINFO));
	m_rectLast = *rect;
	m_sizeLast = size;
}

void CGfRectTracker::SetClipRectArea(BOOL bNeedClip, LPRECT rtClip)
{
	m_bNeedClip = bNeedClip;

	if(m_bNeedClip)
		m_rtClipArea.CopyRect(rtClip);
	else
		m_rtClipArea.SetRectEmpty();
}