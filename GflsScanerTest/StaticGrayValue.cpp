// StaticGrayValue.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "StaticGrayValue.h"
#include "DlgImageInfo.h"
#include "DlgGrayStatistic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticGrayValue

CStaticGrayValue::CStaticGrayValue()
{
	m_pParentWnd   = NULL;
	m_pDlgImgInfo  = NULL;
	m_pDlgGrayStat = NULL;
	InitialGrayStatInfo();
}

CStaticGrayValue::~CStaticGrayValue()
{
}


BEGIN_MESSAGE_MAP(CStaticGrayValue, CStatic)
	//{{AFX_MSG_MAP(CStaticGrayValue)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticGrayValue message handlers

void CStaticGrayValue::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawGrayStatData(&dc);
	
	// Do not call CStatic::OnPaint() for painting messages
}

void CStaticGrayValue::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int x;
	x = point.x;
	SetValueInRange(x);
	m_nGrayValue1 = m_nGrayValue2 = x;
	m_bLBtnDown = TRUE;
	GetSelectGrayStatInfo();
	SetCapture();
	Invalidate(FALSE);
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CStaticGrayValue::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int x;
	x = point.x;
	SetValueInRange(x);
	m_nGrayValue2 = x;
	if(!m_bLBtnDown)
		m_nGrayValue1 = x;
	GetSelectGrayStatInfo();
	Invalidate(FALSE);
	
	CStatic::OnMouseMove(nFlags, point);
}

void CStaticGrayValue::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bLBtnDown)
	{
		int x;
		x = point.x;
		SetValueInRange(x);
		m_nGrayValue1 = m_nGrayValue2 = x;
		m_bLBtnDown = FALSE;
		ReleaseCapture();
		GetSelectGrayStatInfo();
		Invalidate(FALSE);
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CStaticGrayValue::InitialGrayStatInfo()
{
	m_bHaveGrayInfo = 0;
	m_bLBtnDown	    = 0;

	memset(m_nGrayDataCount, 0, sizeof(m_nGrayDataCount));
	memset(m_nYStart, 0, sizeof(m_nYStart));
	memset(&m_stGrayStat, 0, sizeof(m_stGrayStat));

	m_nGrayValue1 = m_nGrayValue2 = 0;
	m_nGrayCntInRang = 0;
	m_fPercent = 0.0;

	m_rtArea.SetRectEmpty();
}

void CStaticGrayValue::GetGrayStatInfo()
{
#if 0
	if(!m_bHaveGrayInfo)
		return;
	int i, ndSum, *pnGrayCount;

	ndSum = 0;
	m_nPixelCount = 0;
	pnGrayCount = m_nGrayDataCount;
	for(i = 0; i < 256; i++, pnGrayCount++)
	{
		m_nPixelCount += *pnGrayCount;
		ndSum += (*pnGrayCount * i);
	}
	m_fAverage = (double)ndSum / (double)m_nPixelCount;

	pnGrayCount = m_nGrayDataCount;
	for(i = 0; i < 256; i++, pnGrayCount++)
	{
		if(0 == *pnGrayCount)
			continue;
		m_nMinGray = i;
		break;
	}
	pnGrayCount = m_nGrayDataCount + 255;
	for(i = 255; i >= 0; i--, pnGrayCount--)
	{
		if(0 == *pnGrayCount)
			continue;
		m_nMaxGray = i;
		break;
	}
	ndSum = m_nMaxGray + m_nMinGray;
	m_fMedian = (double)ndSum / 2.0;

	double fDiff, fValue;
	pnGrayCount = m_nGrayDataCount;
	fValue = 0.0;
	for(i = 0; i < 256; i++, pnGrayCount++)
	{
		fDiff = i - m_fAverage;
		fValue += (fDiff * fDiff * (*pnGrayCount));
	}
	m_fDiff = sqrt(fValue / (double)m_nPixelCount);
#endif
}

void CStaticGrayValue::GetSelectGrayStatInfo()
{
	if(!m_bHaveGrayInfo)
		return;
	int x1, x2, i, *pnGrayCount;
	x1 = min(m_nGrayValue1, m_nGrayValue2);
	x2 = max(m_nGrayValue1, m_nGrayValue2);
	m_nGrayCntInRang = 0;
	pnGrayCount = m_nGrayDataCount + x1;
	for(i = x1; i <= x2; i++, pnGrayCount++)
	{
		m_nGrayCntInRang += *pnGrayCount;
	}
	m_fPercent = (double)m_nGrayCntInRang / (double)m_stGrayStat.nPixelCount * 100.0;

	SetImageStatisticInfo();
}

void CStaticGrayValue::DrawGrayStatData(CDC* pDC)
{
	CRect rtBack, rtFocus;
	GetClientRect(rtBack);
	rtFocus = rtBack;
	rtFocus.left  = min(m_nGrayValue1, m_nGrayValue2);
	rtFocus.right = max(m_nGrayValue1, m_nGrayValue2) + 1;

	CDC dc;
	CBitmap bitmap, *poldbmp;
	dc.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC, rtBack.right, rtBack.bottom);
	poldbmp = dc.SelectObject(&bitmap);

	int nPenStyle, nPenWidth, nBottom, *pnYStart, i;
	nPenStyle = PS_SOLID;
	nPenWidth = 1;
	CPen penBack(nPenStyle, nPenWidth, RGB(0, 0, 255)), penFocus(nPenStyle, nPenWidth, RGB(0, 255, 0)), *poldPen;
	CBrush bhBack(RGB(200, 200, 200)), bhFocus(RGB(100, 100, 100)), *pOldBrush;

	nBottom = rtBack.bottom - 1;
	pOldBrush = dc.SelectObject(&bhBack);
	poldPen   = dc.SelectObject(&penBack);
	dc.FillRect(&rtBack, &bhBack);
//	dc.MoveTo(0, nBottom);
//	dc.LineTo(255, nBottom);
	pnYStart = m_nYStart;
	for(i = 0; i < 256; i++, pnYStart++)
	{
//		if(nBottom == *pnYStart)
//			continue;
		dc.MoveTo(i, *pnYStart);
		dc.LineTo(i, nBottom);
	}

	if(m_bLBtnDown)
	{
		dc.SelectObject(&bhFocus);
		dc.SelectObject(&penFocus);
		dc.FillRect(&rtFocus, &bhFocus);
		pnYStart = m_nYStart + rtFocus.left;
		for(i = rtFocus.left; i < rtFocus.right; i++, pnYStart++)
		{
			dc.MoveTo(i, *pnYStart);
			dc.LineTo(i, nBottom);
		}
	}
	dc.SelectObject(pOldBrush);
	dc.SelectObject(poldPen);
	penBack.DeleteObject();
	penFocus.DeleteObject();
	bhBack.DeleteObject();
	bhFocus.DeleteObject();

	pDC->BitBlt(0, 0, rtBack.right, rtBack.bottom, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(poldbmp);
}

void CStaticGrayValue::SetStatGrayData(UCHAR* pImageData, int nw, int nh, CRect rtArea)
{
	InitialGrayStatInfo();
	m_stGrayStat.pImageData		= pImageData;
	m_stGrayStat.pnGrayValueCnt	= m_nGrayDataCount;
	m_stGrayStat.nw				= nw;
	m_stGrayStat.nh				= nh;
	memcpy(&m_stGrayStat.rtArea, (LPRECT)&rtArea, sizeof(m_stGrayStat.rtArea));
	
	if(GetImageGrayStatisticInfo(&m_stGrayStat) < 1)
		return;
	m_rtArea = m_stGrayStat.rtArea;

	CRect rect;
	int h, i, *pnGrayCnt, *pnYStart;

	GetClientRect(rect);
	h = rect.bottom;

	if(m_stGrayStat.nMaxCntGrayCount > 0)
	{
		m_bHaveGrayInfo = 1;
		pnGrayCnt = m_nGrayDataCount;
		pnYStart  = m_nYStart;
		for(i = 0; i < 256; i++, pnYStart++, pnGrayCnt++)
			*pnYStart = h - 1 - (*pnGrayCnt * (h - 1) / m_stGrayStat.nMaxCntGrayCount);
 	}
	
	SetImageStatisticInfo();
	Invalidate(FALSE);
}

void CStaticGrayValue::SetImageStatisticInfo()
{
	if(!m_pParentWnd || !IsWindow(m_pParentWnd->GetSafeHwnd()))
		return;

	if(m_pDlgImgInfo)
		m_pDlgImgInfo->SetImageStatInfo();
	else if(m_pDlgGrayStat)
		m_pDlgGrayStat->SetImageStatInfo();
	return;

//	UINT fuFlags;
//	DWORD dwRet = 0;

//	fuFlags = SMTO_BLOCK;
//	SendMessageTimeout(m_pParentWnd->m_hWnd, WM_SHOWIMAGE_STATINFO, 0, 0, fuFlags, 500, &dwRet);	
	m_pParentWnd->SendMessage(WM_SHOWIMAGE_STATINFO, 0, 0);
}

