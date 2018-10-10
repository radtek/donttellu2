// GflsScanerTestView.cpp : implementation of the CGflsScanerTestView class
//

#include "stdafx.h"
#include "GflsScanerTest.h"

#include "MainFrm.h"
#include "GflsScanerTestView.h"
#include "DlgCheckTool.h"
#include "DlgCheckSpeed.h"
#include "DlgSquareParam.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CGflsScanerTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestView

IMPLEMENT_DYNCREATE(CGflsScanerTestView, CScrollView)

BEGIN_MESSAGE_MAP(CGflsScanerTestView, CScrollView)
	//{{AFX_MSG_MAP(CGflsScanerTestView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_MESSAGE(WM_SETIMAGEINFO_TOSTAT, OnImageData2GrayStat)
	ON_BN_CLICKED(IDC_BUTTON_SETSQUARE, &CGflsScanerTestView::OnBtnSetSquare)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestView construction/destruction

CGflsScanerTestView::CGflsScanerTestView()
{
	// TODO: add construction code here
	memset(&m_stbmpInfo,	  0, sizeof(m_stbmpInfo));
	memset(&m_stParamInfo,    0, sizeof(m_stParamInfo));
	memset(&m_stImgParseInfo, 0, sizeof(m_stImgParseInfo));
	m_bLBtnDown = FALSE;
	m_pFrameWnd = NULL;
	m_bMoveSquare = FALSE;
	m_bShowRSquare = TRUE;
	m_bShowGSquare = FALSE;
	m_bShowBSquare = FALSE;
}

CGflsScanerTestView::~CGflsScanerTestView()
{
}

BOOL CGflsScanerTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestView drawing

void CGflsScanerTestView::OnDraw(CDC* pDC)
{
	CGflsScanerTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	DrawTestInfo(pDC);
}

void CGflsScanerTestView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = GFLS_SCANNER_FINGER_WIDTH;
	sizeTotal.cy = GFLS_SCANNER_FINGER_HEIGHT;
	SetScrollSizes(MM_TEXT, sizeTotal);

	m_pFrameWnd = (CMainFrame*)theApp.GetMainWnd();
	if(m_pFrameWnd)
		m_pFrameWnd->m_pTestView = this;
	m_ImageBadptStat.m_pTestView = this;

	InitialBitmapInfo();
//	InitialScanner();
	CreateAllInfoDlg();

	m_pFrameWnd->ShowCurSelectAreaInfo(&m_stParamInfo.ptAreaLeftTop, &m_stParamInfo.ptAreaRightBottom, 1 );
}

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestView printing

BOOL CGflsScanerTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGflsScanerTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGflsScanerTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestView diagnostics

#ifdef _DEBUG
void CGflsScanerTestView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGflsScanerTestView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CGflsScanerTestDoc* CGflsScanerTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGflsScanerTestDoc)));
	return (CGflsScanerTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestView message handlers

void CGflsScanerTestView::OnDestroy() 
{
	CloseAllInfoDlg();
	UnInitScanner();
	
	CScrollView::OnDestroy();
	
	// TODO: Add your message handler code here
	ClearMallocMemory();
}

void CGflsScanerTestView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	//CSize sizeTotal;
	//sizeTotal.cx = cx;
	//sizeTotal.cy = cy;
	//SetScrollSizes(MM_TEXT, sizeTotal);
}

void CGflsScanerTestView::InitialBitmapInfo()
{
	RGBQUAD *pRgbQuad = m_stbmpInfo.bmiColors;
	memcpy(pRgbQuad, theApp.m_stbmpInfo.bmiColors, sizeof(theApp.m_stbmpInfo.bmiColors));
//	UCHAR nValue = 0;
//	int i, nImgSize;
//	for(i = 0; i < 256; i++, pRgbQuad++, nValue++)
//	{
//		pRgbQuad->rgbBlue  = nValue;
//		pRgbQuad->rgbGreen = nValue;
//		pRgbQuad->rgbRed   = nValue;
//	}

	int nImgSize = GFLS_SCANNER_IMAGEWIDTH * GFLS_SCANNER_IMAGEHEIGHT;

	BITMAPINFOHEADER* pHead = &m_stbmpInfo.bmiHeader;
	pHead->biSize			= sizeof(BITMAPINFOHEADER);
	pHead->biPlanes			= 1;
	pHead->biCompression	= BI_RGB;
	pHead->biBitCount		= 8;
	pHead->biWidth			= GFLS_SCANNER_IMAGEWIDTH;
	pHead->biHeight			= GFLS_SCANNER_IMAGEHEIGHT;
	pHead->biSizeImage		= nImgSize;
	pHead->biXPelsPerMeter	= 20000;
	pHead->biYPelsPerMeter	= 20000;

	m_stParamInfo.nCurScale				= 1;
	m_stParamInfo.nImgWidth				= GFLS_SCANNER_IMAGEWIDTH;
	m_stParamInfo.nImgHeight			= GFLS_SCANNER_IMAGEHEIGHT;
	m_stParamInfo.nImgSize				= nImgSize;

	m_stParamInfo.nDispWidth			= m_stParamInfo.nImgWidth;
	m_stParamInfo.nDispHeight			= m_stParamInfo.nImgHeight;
	m_stParamInfo.nDispImgSize			= nImgSize;

	m_stParamInfo.bCrossSquareScaled	= 1;
	m_stParamInfo.nCrossLength			= 500;
	m_stParamInfo.nSquare1				= 300;
	m_stParamInfo.nSquare2				= 500;
	m_stParamInfo.nSquare3				= 440;
	m_stParamInfo.nSquare4             = 200;
	m_stParamInfo.nSquare4OffX         = (GFLS_SCANNER_IMAGEWIDTH-200)/2;
	m_stParamInfo.nSquare4OffY          =(GFLS_SCANNER_IMAGEHEIGHT-200)/2;
	m_stParamInfo.rectSquare4.SetRect(m_stParamInfo.nSquare4OffX,
		        m_stParamInfo.nSquare4OffY,
				m_stParamInfo.nSquare4OffX+m_stParamInfo.nSquare4, 
				m_stParamInfo.nSquare4OffY+ m_stParamInfo.nSquare4 );
	m_stParamInfo.clCross				= RGB(255, 0, 255);
	m_stParamInfo.clSquare1				= RGB(0, 255, 0);
	m_stParamInfo.clSquare2				= RGB(0, 255, 0);
	m_stParamInfo.clSquare3				= RGB(0, 255, 0);
	m_stParamInfo.clSquare4				= RGB(255, 0, 0);
	m_stParamInfo.clBack				= RGB(192, 192, 192);

	m_stParamInfo.ptAreaLeftTop.x = GFLS_SCANNER_IMAGEWIDTH / 2 - 100;
	m_stParamInfo.ptAreaLeftTop.y = GFLS_SCANNER_IMAGEHEIGHT / 2 - 100;
	m_stParamInfo.ptAreaRightBottom.x = m_stParamInfo.ptAreaLeftTop.x + 200;
	m_stParamInfo.ptAreaRightBottom.y = m_stParamInfo.ptAreaLeftTop.y + 200;
	m_stParamInfo.clArea		= RGB(0, 0, 255);

	m_stParamInfo.pSrcImage  = (UCHAR*)malloc(nImgSize);
	m_stParamInfo.pDispImage = (UCHAR*)malloc(nImgSize * GFLS_SCANNER_MAXSCALE * GFLS_SCANNER_MAXSCALE);
	if(m_stParamInfo.pSrcImage)
		memset(m_stParamInfo.pSrcImage, 255, nImgSize);
	if(m_stParamInfo.pDispImage)
		memset(m_stParamInfo.pDispImage, 255, nImgSize * GFLS_SCANNER_MAXSCALE * GFLS_SCANNER_MAXSCALE);
}

BOOL CGflsScanerTestView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	
	return CScrollView::OnEraseBkgnd(pDC);
}

void CGflsScanerTestView::DrawTestInfo(CDC* pDC)
{
	CClientDC clientDC(this);
//	if ( !pDC )
	{
		OnPrepareDC(&clientDC);
		pDC = &clientDC;
	}

	CRect rtClient, rt;
	GetClientRect(rtClient);
	
	POINT *pptOff = &m_stParamInfo.ptOffSet;
	int nw, nh, nSize, nDist;
	UCHAR *pDrawData;

	if(theApp.m_bHaveImageInfo)
	{
		nw = m_stParamInfo.nDispWidth;
		nh = m_stParamInfo.nDispHeight;
		nSize = m_stParamInfo.nDispImgSize;
		pDrawData = m_stParamInfo.pDispImage;
		if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_BADPOINT && m_stParamInfo.bDrawBadPoint)
			pDrawData = m_ImageBadptStat.m_pBadInfoData;

		nDist = 0;
		if(nw < rtClient.right)
		{
			pptOff->x = -(rtClient.right - nw) / 2;
			if(pptOff->x < nDist)
				pptOff->x = nDist;
		}
		else
		{
			pptOff->x = 0;
			rtClient.right = nw;
		}
		if(nh < rtClient.bottom)
		{
			pptOff->y = -(rtClient.bottom - nh) / 2;
			if(pptOff->y < nDist)
				pptOff->y = nDist;
		}
		else
		{
			pptOff->y = 0;
			rtClient.bottom = nh;
		}

		CDC dc;
		CBitmap bmp, *poldbmp = NULL;
		dc.CreateCompatibleDC(pDC);
		bmp.CreateCompatibleBitmap(pDC, nw, nh);
		poldbmp = dc.SelectObject(&bmp);

		m_stbmpInfo.bmiHeader.biWidth = nw;	m_stbmpInfo.bmiHeader.biHeight = -nh;	m_stbmpInfo.bmiHeader.biSizeImage = nSize;
		if(pDrawData)
			::SetDIBitsToDevice(dc.GetSafeHdc(), 0, 0, nw, nh, 0, 0, 0, nh, pDrawData, (BITMAPINFO*)&m_stbmpInfo, DIB_RGB_COLORS);

		DrawStdChkHelpInfo(&dc, nw, nh);
		DrawCenterCross(&dc, nw, nh);
		if(m_bShowGSquare)
		{
			DrawCenterSquare(&dc, nw, nh, m_stParamInfo.nSquare1, m_stParamInfo.clSquare1);
		}
		if(0)
		{
			DrawCenterSquare(&dc, nw, nh, m_stParamInfo.nSquare2, m_stParamInfo.clSquare2);
			DrawCenterSquare(&dc, nw, nh, m_stParamInfo.nSquare3, m_stParamInfo.clSquare3);
		}
		if(m_bShowRSquare)
		{
			DrawTestRect(&dc, m_stParamInfo.rectSquare4, m_stParamInfo.clSquare4,NULL);
		}
		if(m_bShowBSquare)DrawSelectArea(&dc);
		
		pDC->SetWindowOrg(*pptOff);
		pDC->BitBlt(0, 0, nw, nh, &dc, 0, 0, SRCCOPY);
		pDC->SetWindowOrg(0, 0);
		dc.SelectObject(poldbmp);
	}
	else
	{
		nw = 0;
		nh = 0;
	}

	CBrush	brush(m_stParamInfo.clBack);
	if(nw < rtClient.right)
	{
		rt = rtClient;
		rt.right = -pptOff->x;
		pDC->FillRect(rt, &brush);

		rt = rtClient;
		rt.left = nw - pptOff->x;
		pDC->FillRect(rt, &brush);
	}
	if(nh < rtClient.bottom)
	{
		rt = rtClient;
		rt.bottom = -pptOff->y;
		pDC->FillRect(rt, &brush);

		rt = rtClient;
		rt.top = nh - pptOff->y;
		pDC->FillRect(rt, &brush);
	}

	brush.DeleteObject();
}

void CGflsScanerTestView::DrawCenterCross(CDC* pdc, int nw, int nh)
{
	if(m_stParamInfo.bOnlyShowImage)
		return;

	int		cx, cy, nradio;

	cx = nw / 2;	cy = nh / 2;
	if(m_stParamInfo.bCrossSquareScaled && m_stParamInfo.nCurScale > 1)
		nradio = m_stParamInfo.nCrossLength * m_stParamInfo.nCurScale / 2;
	else
		nradio = m_stParamInfo.nCrossLength / 2;

	DrawPointedCross(pdc, cx, cy, nradio, m_stParamInfo.clCross);
}

void CGflsScanerTestView::DrawPointedCross(CDC* pdc, int cx, int cy, int nradio, COLORREF clr)
{
	CPen	pen, *poldpen;
	pen.CreatePen(PS_SOLID, 1, clr);
	poldpen = pdc->SelectObject(&pen);
	pdc->MoveTo(cx - nradio, cy);
	pdc->LineTo(cx + nradio, cy);
	pdc->MoveTo(cx, cy - nradio);
	pdc->LineTo(cx, cy + nradio);
	pdc->SelectObject(poldpen);
	pen.DeleteObject();
}

void CGflsScanerTestView::DrawCenterSquare(CDC* pdc, int nw, int nh, int nWidth, COLORREF clr)
{
	if(m_stParamInfo.bOnlyShowImage)
		return;

	int		cx, cy, nHalf;
	CRect	rt;
	
	cx = nw / 2;	cy = nh / 2;
	if(m_stParamInfo.bCrossSquareScaled && m_stParamInfo.nCurScale > 1)
		nHalf = nWidth * m_stParamInfo.nCurScale / 2;
	else
		nHalf = nWidth / 2;

	rt.SetRect(cx - nHalf, cy - nHalf, cx + nHalf, cy + nHalf);
	DrawTestRect(pdc, rt, clr, NULL);
}


void CGflsScanerTestView::DrawSelectArea(CDC* pdc)
{
	switch(m_stParamInfo.nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_BADPOINT:
	case GFLS_SCANNER_STDCHKTYPE_PARSE:
	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		return;

	case GFLS_SCANNER_STDCHKTYPE_ABERRANCE:
	case GFLS_SCANNER_STDCHKTYPE_IMGRESOLUTION:
		return;

	default:
		if(m_stParamInfo.bOnlyShowImage)
			return;
		break;
	}

	int x1, y1, x2, y2, nRadius;
	x1 = m_stParamInfo.ptAreaLeftTop.x;			y1 = m_stParamInfo.ptAreaLeftTop.y;
	x2 = m_stParamInfo.ptAreaRightBottom.x;		y2 = m_stParamInfo.ptAreaRightBottom.y;
	nRadius = 3;

	CPen	pen, *poldpen;
	CBrush	brush, *poldbrush;

	pen.CreatePen(PS_DASH, 1, m_stParamInfo.clArea);
	poldpen = pdc->SelectObject(&pen);
	brush.CreateStockObject(BS_HOLLOW);
	poldbrush = pdc->SelectObject(&brush);
	
	pdc->MoveTo(x1, y1);
//	if(m_stParamInfo.nStdChkType != GFLS_SCANNER_STDCHKTYPE_ABERRANCE)
	{
		pdc->LineTo(x1, y2);
		pdc->LineTo(x2, y2);
		pdc->LineTo(x2, y1);
		pdc->LineTo(x1, y1);
	}
	pdc->LineTo(x2, y2);

	pdc->Ellipse(x1 - nRadius, y1 - nRadius, x1 + nRadius, y1 + nRadius);
	pdc->Ellipse(x2 - nRadius, y2 - nRadius, x2 + nRadius, y2 + nRadius);

	pdc->SelectObject(poldpen);
	pdc->SelectObject(poldbrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CGflsScanerTestView::DrawTestRect(CDC* pdc, CRect rt, COLORREF clr, const TCHAR* pszText)
{
	CPen	pen, *poldpen;
	CBrush	brush, *poldbrush;

	pen.CreatePen(PS_SOLID, 1, clr);
	poldpen = pdc->SelectObject(&pen);
	brush.CreateStockObject(NULL_BRUSH);
	poldbrush = pdc->SelectObject(&brush);
	pdc->Rectangle(&rt);
	if(pszText)
	{
		COLORREF clText, clOld;
		clText = RGB(255, 0, 0);
		clOld = pdc->SetTextColor(clText);
		pdc->DrawText(pszText, -1, rt, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		pdc->SetTextColor(clOld);
	}
	pdc->SelectObject(poldpen);
	pdc->SelectObject(poldbrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CGflsScanerTestView::DrawStdChkHelpInfo(CDC *pdc, int nw, int nh)
{
	switch(m_stParamInfo.nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_PARSE:
		DrawStdChkParseInfo(pdc, nw, nh);
		return;

	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
		DrawStdChkCenterGrayInfo(pdc, nw, nh);
		return;

	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		drawStdChkGrayVarian(pdc, nw, nh);
		return;

	case GFLS_SCANNER_STDCHKTYPE_ABERRANCE:
		DrawStdChkAberrance(pdc, nw, nh);
		return;

	case GFLS_SCANNER_STDCHKTYPE_IMGRESOLUTION:
		DrawStdChkImgResolution(pdc, nw, nh);
		return;

	case GFLS_SCANNER_STDCHKTYPE_BADPOINT:
		if(m_stParamInfo.bDrawBadPoint)
			m_ImageBadptStat.DrawBadPointInfo(pdc, nw, nh);
		return;

	default:
		return;
	}
}

void CGflsScanerTestView::DrawStdChkParseInfo(CDC *pdc, int nw, int nh)
{
	for(int i = 0; i < 9; i++)
		DrawTestRect(pdc, m_stImgParseInfo.rtCArea + i, m_stParamInfo.clArea, m_stParamInfo.bOnlyShowImage ? NULL : _T("C"));
}

void CGflsScanerTestView::DrawStdChkCenterGrayInfo(CDC *pdc, int nw, int nh)
{
	DrawTestRect(pdc, m_stImgParseInfo.rtCArea, m_stParamInfo.clArea, m_stParamInfo.bOnlyShowImage ? NULL : _T("A"));
}

void CGflsScanerTestView::drawStdChkGrayVarian(CDC *pdc, int nw, int nh)
{
	TCHAR szInfo[2][4] = {0};
	szInfo[0][0] = 'G';	szInfo[0][1] = '0';
	szInfo[1][0] = 'G';	szInfo[1][1] = '1';	szInfo[1][2] = '0';
	for(int i = 0; i < 2; i++)
		DrawTestRect(pdc, m_stImgParseInfo.rtCArea + i, m_stParamInfo.clArea, m_stParamInfo.bOnlyShowImage ? NULL : szInfo[i]);
}

void CGflsScanerTestView::DrawStdChkAberrance(CDC *pdc, int nw, int nh)
{
	int x1, y1, x2, y2, nRadius;
	x1 = m_stParamInfo.ptAreaLeftTop.x;			y1 = m_stParamInfo.ptAreaLeftTop.y;
	x2 = m_stParamInfo.ptAreaRightBottom.x;		y2 = m_stParamInfo.ptAreaRightBottom.y;
	nRadius = 3;

	CPen		pen, *poldpen;
	CBrush		brush, *poldbrush;
	COLORREF	clText, clOld, clCross;

	clText  = RGB(255, 0, 0);
	clCross = RGB(0, 255, 0);

	clOld = pdc->SetTextColor(clText);
	pen.CreatePen(PS_DASH, 1, m_stParamInfo.clArea);
	poldpen = pdc->SelectObject(&pen);
	brush.CreateStockObject(BS_HOLLOW);
	poldbrush = pdc->SelectObject(&brush);
	
	pdc->MoveTo(x1, y1);
	pdc->LineTo(x2, y2);
	pdc->Ellipse(x1 - nRadius, y1 - nRadius, x1 + nRadius, y1 + nRadius);
	pdc->Ellipse(x2 - nRadius, y2 - nRadius, x2 + nRadius, y2 + nRadius);

	TCHAR aszText[3][4];
	POINT ptPos,ptPos1,ptPos2;
	int i, j, nWedge;
		ptPos2.x = 300;
	ptPos2.y = 300;
		ptPos1.x = 0;
	ptPos1.y = 0;
	ZoomPointXByScale(NULL, NULL, &ptPos1, m_stParamInfo.nCurScale, 1);
ZoomPointXByScale(NULL, NULL, &ptPos2, m_stParamInfo.nCurScale, 1);
int nlong = ptPos2.x - ptPos1.x;
	nWedge = 15;
	ptPos.y = 20;
	ZoomPointYByScale(NULL, NULL, &ptPos, m_stParamInfo.nCurScale, 1);


	for(i = 0; i < 3; i++, ptPos.y+= nlong)
	{
		memset(aszText, 0, sizeof(aszText));
		
		if(i == 0)
		{
			_tcscpy(aszText[0], _T("B2"));	_tcscpy(aszText[1], _T("A2"));	_tcscpy(aszText[2], _T("B1"));
		}
	
		else if(i == 1)
		{
			_tcscpy(aszText[0], _T("A3"));	_tcscpy(aszText[1], _T("  "));	_tcscpy(aszText[2], _T("A1"));
		}
		
		else
		{
			_tcscpy(aszText[0], _T("B3"));	_tcscpy(aszText[1], _T("A4"));	_tcscpy(aszText[2], _T("B4"));
		}

		ptPos.x = 20;

	  ZoomPointXByScale(NULL, NULL, &ptPos, m_stParamInfo.nCurScale, 1);
		for(j = 0; j < 3; j++, ptPos.x += nlong)
		{
		
			if((i != 1 || j != 1) && !m_stParamInfo.bOnlyShowImage)
				pdc->TextOut(ptPos.x - nWedge, ptPos.y - nWedge, aszText[j], 2);
			DrawPointedCross(pdc, ptPos.x, ptPos.y, 5, clCross);
		}
	}

	pdc->SetTextColor(clOld);
	pdc->SelectObject(poldpen);
	pdc->SelectObject(poldbrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CGflsScanerTestView::DrawStdChkImgResolution(CDC *pdc, int nw, int nh)
{
	int x1, y1, x2, y2, nRadius;
	x1 = m_stParamInfo.ptAreaLeftTop.x;			y1 = m_stParamInfo.ptAreaLeftTop.y;
	x2 = m_stParamInfo.ptAreaRightBottom.x;		y2 = m_stParamInfo.ptAreaRightBottom.y;
	nRadius = 3;

	CPen		pen, *poldpen;
	CBrush		brush, *poldbrush;
	COLORREF	clText, clOld, clCross;

	clText  = RGB(255, 0, 0);
	clCross = RGB(0, 255, 0);


	clOld = pdc->SetTextColor(clText);
	pen.CreatePen(PS_DASH, 1, m_stParamInfo.clArea);
	poldpen = pdc->SelectObject(&pen);
	brush.CreateStockObject(BS_HOLLOW);
	poldbrush = pdc->SelectObject(&brush);
	
	pdc->MoveTo(x1, y1);
	pdc->LineTo(x2, y2);
	pdc->Ellipse(x1 - nRadius, y1 - nRadius, x1 + nRadius, y1 + nRadius);
	pdc->Ellipse(x2 - nRadius, y2 - nRadius, x2 + nRadius, y2 + nRadius);

	POINT ptPos;
	int nWedge = 15;

	ptPos.x = 620;	ptPos.y = 320;
	ZoomPointByScale(NULL, NULL, &ptPos, m_stParamInfo.nCurScale, 1);
	if(!m_stParamInfo.bOnlyShowImage)
		pdc->TextOut(ptPos.x - nWedge, ptPos.y - nWedge, _T("A1"), 2);
	DrawPointedCross(pdc, ptPos.x, ptPos.y, nRadius, clCross);

	ptPos.x = 20;	ptPos.y = 320;
	ZoomPointByScale(NULL, NULL, &ptPos, m_stParamInfo.nCurScale, 1);
	if(!m_stParamInfo.bOnlyShowImage)
		pdc->TextOut(ptPos.x - nWedge, ptPos.y - nWedge, _T("A3"), 2);
	DrawPointedCross(pdc, ptPos.x, ptPos.y, nRadius, clCross);

	ptPos.x = 320;	ptPos.y = 20;
	ZoomPointByScale(NULL, NULL, &ptPos, m_stParamInfo.nCurScale, 1);
	if(!m_stParamInfo.bOnlyShowImage)
		pdc->TextOut(ptPos.x - nWedge, ptPos.y - nWedge, _T("A2"), 2);
	DrawPointedCross(pdc, ptPos.x, ptPos.y, nRadius, clCross);

	ptPos.x = 320;	ptPos.y = 620;
	ZoomPointByScale(NULL, NULL, &ptPos, m_stParamInfo.nCurScale, 1);
	if(!m_stParamInfo.bOnlyShowImage)
		pdc->TextOut(ptPos.x - nWedge, ptPos.y - nWedge, _T("A4"), 2);
	DrawPointedCross(pdc, ptPos.x, ptPos.y, nRadius, clCross);
}

void CGflsScanerTestView::ClearMallocMemory()
{
	if(m_stParamInfo.pDispImage)
		free(m_stParamInfo.pDispImage);
	if(m_stParamInfo.pSrcImage)
		free(m_stParamInfo.pSrcImage);
	m_stParamInfo.pDispImage = NULL;
	m_stParamInfo.pSrcImage  = NULL;
}

void CGflsScanerTestView::CreateAllInfoDlg()
{
	CRect rtFrame;
	GetWindowRect(rtFrame);

	m_dlgImgInfo.m_pTestView = this;
	if(!m_dlgImgInfo.InitImageInfoDlg(this, rtFrame))
		return;
	m_dlgGrayStat.m_pTestView = this;
	if(!m_dlgGrayStat.CreateGrayStatDlg(this, rtFrame))
		return;
	m_dlgCapt2Save.m_pTestView = this;
	if(!m_dlgCapt2Save.InitCaptImages2SaveDlg(this))
		return;

	ShowAllInfoDlg(1, 0x1);
	m_dlgImgInfo.SetStdChkInformation(m_stParamInfo.nStdChkType);
}

void CGflsScanerTestView::CloseAllInfoDlg()
{
	ShowAllInfoDlg(0, 0xff);

	if(IsWindow(m_dlgImgInfo.GetSafeHwnd()))
		m_dlgImgInfo.DestroyWindow();
	if(IsWindow(m_dlgGrayStat.GetSafeHwnd()))
		m_dlgGrayStat.DestroyWindow();
	if(IsWindow(m_dlgCapt2Save.GetSafeHwnd()))
		m_dlgCapt2Save.DestroyWindow();
}

void CGflsScanerTestView::ShowAllInfoDlg(UCHAR bShow, UCHAR nDlgType) // 0x1-m_dlgImgInfo;	0x2--m_dlgGrayStat;	0x4--m_dlgCapt2Save;	0xff--All Dialog
{
	int nCmdShow = bShow ? SW_SHOW : SW_HIDE;

	if(nDlgType & 0x1)
	{
		if(IsWindow(m_dlgImgInfo.GetSafeHwnd()))
			m_dlgImgInfo.ShowWindow(nCmdShow);
	}
	if(nDlgType & 0x2)
	{
		if(IsWindow(m_dlgGrayStat.GetSafeHwnd()))
			m_dlgGrayStat.ShowWindow(nCmdShow);
	}
	if(nDlgType & 0x4)
	{
		if(IsWindow(m_dlgCapt2Save.GetSafeHwnd()))
			m_dlgCapt2Save.ShowWindow(nCmdShow);
	}
}

void CGflsScanerTestView::SetAreaPosByCenter(int x, int y, RECT *prtArea, UCHAR bAdjust)
{
	int xOff, yOff, xOld, yOld;

//	xOld = (prtArea->left + prtArea->right) >> 1;
//	yOld = (prtArea->top + prtArea->bottom) >> 1;
	xOld = m_stImgParseInfo.ptPrevMouse.x;
	yOld = m_stImgParseInfo.ptPrevMouse.y;

	xOff = x - xOld;
	yOff = y - yOld;

	OffsetRect(prtArea, xOff, yOff);
	if(!bAdjust)
		return;

	xOff = yOff = 0;
	if(prtArea->left < 0)
		xOff = -prtArea->left;
	if(prtArea->right > m_stParamInfo.nDispWidth)
		xOff = m_stParamInfo.nDispWidth - prtArea->right;
	if(prtArea->top < 0)
		yOff = -prtArea->top;
	if(prtArea->bottom > m_stParamInfo.nDispHeight)
		yOff = m_stParamInfo.nDispWidth - prtArea->bottom;
	
	OffsetRect(prtArea, xOff, yOff);
}


void CGflsScanerTestView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!theApp.m_bHaveImageInfo)
		return;

	CPoint pt = point;
	pt.Offset(m_stParamInfo.ptOffSet);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&pt);

	switch(m_stParamInfo.nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_PARSE:
	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		{
			int i, nCount;
			if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_PARSE)
				nCount = 9;
			else if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_GRAYVARIA)
				nCount = 2;
			else
				nCount = 1;

			LPRECT prtArea = m_stImgParseInfo.rtCArea;
			m_stImgParseInfo.nCurSelArea = -1;
			m_stImgParseInfo.ptPrevMouse.x = pt.x;
			m_stImgParseInfo.ptPrevMouse.y = pt.y;
			for(i = 0; i < nCount; i++, prtArea++)
			{
				if(!PtInRect(prtArea, m_stImgParseInfo.ptPrevMouse))
					continue;
				m_stImgParseInfo.nCurSelArea = i;
				m_bLBtnDown = TRUE;
				SetCapture();
				break;
			}
		}
		break;
	case GFLS_SCANNER_STDCHKTYPE_MOVESQUARE:
		{
			CRect rtImg(0, 0, 0, 0);
			rtImg.right  = m_stParamInfo.nDispWidth;
			rtImg.bottom = m_stParamInfo.nDispHeight;
			if(rtImg.PtInRect(pt))
			{
				m_bLBtnDown = TRUE;
				m_stImgParseInfo.ptPrevMouse.x = pt.x;
				m_stImgParseInfo.ptPrevMouse.y = pt.y;
				SetCapture();
			}

		}		
		break;
	default:
		{
			CRect rtImage(0, 0, 0, 0);
			rtImage.right  = m_stParamInfo.nDispWidth;
			rtImage.bottom = m_stParamInfo.nDispHeight;
			if(rtImage.PtInRect(pt))
			{
				if(nFlags & MK_CONTROL)
				{
					m_stParamInfo.ptAreaRightBottom.x = pt.x;
					m_stParamInfo.ptAreaRightBottom.y = pt.y;
				}
				else
				{
					m_bLBtnDown = TRUE;
					m_stParamInfo.ptAreaLeftTop.x = pt.x;
					m_stParamInfo.ptAreaLeftTop.y = pt.y;
					SetCapture();
				}
				SendStatImageDataMessage(1, 0);
				Invalidate(FALSE);
			}
		}
		break;
	}
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CGflsScanerTestView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!theApp.m_bHaveImageInfo)
		return;

	CPoint pt = point;
	pt.Offset(m_stParamInfo.ptOffSet);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&pt);

	CRect rtImage(0, 0, 0, 0);
	rtImage.right  = m_stParamInfo.nDispWidth;
	rtImage.bottom = m_stParamInfo.nDispHeight;
	UCHAR bOnImage = 0;
	if(rtImage.PtInRect(pt))
	{
		bOnImage = 1;
	}

	switch(m_stParamInfo.nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_PARSE:
	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		{
			if(!bOnImage || !m_bLBtnDown || m_stImgParseInfo.nCurSelArea < 0)
				break;
			SetAreaPosByCenter(pt.x, pt.y, m_stImgParseInfo.rtCArea + m_stImgParseInfo.nCurSelArea, 0);
			m_stImgParseInfo.ptPrevMouse.x = pt.x;
			m_stImgParseInfo.ptPrevMouse.y = pt.y;
			Invalidate(FALSE);
		}
		break;
	case  GFLS_SCANNER_STDCHKTYPE_MOVESQUARE:
		if(!bOnImage || !m_bLBtnDown )
			break;
		SetAreaPosByCenter(pt.x, pt.y, &m_stParamInfo.rectSquare4, 1);
		m_stImgParseInfo.ptPrevMouse.x = pt.x;
		m_stImgParseInfo.ptPrevMouse.y = pt.y;
		Invalidate(FALSE);
		break;
	default:
		{
			if(!bOnImage || !m_bLBtnDown)
				break;
			m_stParamInfo.ptAreaRightBottom.x = pt.x;
			m_stParamInfo.ptAreaRightBottom.y = pt.y;
			SendStatImageDataMessage(1, 0);
			Invalidate(FALSE);
		}
		break;
	}

	if(m_pFrameWnd)
		m_pFrameWnd->ShowCurMousePosInfo(pt, rtImage.right, rtImage.bottom, bOnImage);

	CScrollView::OnMouseMove(nFlags, point);
}

void CGflsScanerTestView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bLBtnDown)
	{
		m_bLBtnDown = FALSE;
		ReleaseCapture();
		GetLineInfoWhenStdChk();
	}
	CScrollView::OnLButtonUp(nFlags, point);
}

//void CGflsScanerTestView::OnMakeGridSample() 
//{
//	// TODO: Add your command handler code here
//	CDlgCheckTool dlg;
//	dlg.m_nChkToolSample = GFLS_SCANNER_CHKTOOL_GRIDSAMPLE;
//	dlg.DoModal();
//}

void CGflsScanerTestView::InitialScanner()
{
}

void CGflsScanerTestView::UnInitScanner()
{
	StopCaptureThread();
	
	DownLiveCaptureDll();
	DownMosaicDll();
//	LIVESCAN_Close();
}

void CGflsScanerTestView::StartCaptureImage()
{
	DWORD dwThread = 0;
	g_stScannerInfo.hCaptThread = ::CreateThread(NULL, 0, StartCaptureThread, this, 0, &dwThread);
}

DWORD	WINAPI StartCaptureThread(LPVOID lParam)
{
	int m_bwriten = 0;
	CStringA temp,temp1;
	CFile mFile;
	CFileException e;
	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");

	if(!g_stScannerInfo.bInitialed)
		return 1;
	CGflsScanerTestView* pTestView = (CGflsScanerTestView*)lParam;
	if(NULL == pTestView)
		return 1;

	int nret, nw, nh;
	nw = pTestView->m_stParamInfo.nDispWidth;
	nh = pTestView->m_stParamInfo.nDispHeight;
	if (reType == 4)
{
			nret = g_stLiveCaptDll.pfnLiveScan_BeginCapture(g_stScannerInfo.nCurChannel);
	
}
else
{
			nret = g_stCaptDll.pfnScan_BeginCapture(g_stScannerInfo.nCurChannel);

}
	temp1.Format("%d",nret);
	if (nret == 1) 
	{temp +="测试\"4.13准备采集一帧图像\"成功 返回值= "+temp1+"\r\n";}
	else 
	{
		temp += "测试\"4.13准备采集一帧图像\"失败 返回值= "+temp1+"\r\n";
	}
	if(nret < 1)
	{
		ShowScannerErrorInfo(nret, pTestView, 1);
		return nret;
	}
	g_stScannerInfo.bCaptureImage = 1;

	while(!g_stScannerInfo.bExitCapture)
	{
	if (reType == 4)
{
					nret = g_stLiveCaptDll.pfnLiveScan_BeginCapture(g_stScannerInfo.nCurChannel);
		nret = g_stLiveCaptDll.pfnLiveScan_GetFPRawData(g_stScannerInfo.nCurChannel, pTestView->m_stParamInfo.pSrcImage);
		nret = g_stLiveCaptDll.pfnLiveScan_EndCapture(g_stScannerInfo.nCurChannel);

}
else
{
		nret = g_stCaptDll.pfnScan_BeginCapture(g_stScannerInfo.nCurChannel);
		nret = g_stCaptDll.pfnScan_GetFPRawData(g_stScannerInfo.nCurChannel, pTestView->m_stParamInfo.pSrcImage);
		nret = g_stCaptDll.pfnScan_EndCapture(g_stScannerInfo.nCurChannel);

}
	if(m_bwriten == 0)
	{
		temp1.Format("%d",nret);
		if (nret == 1) 
		{temp += "测试\"4.14采集一帧图像\"成功 返回值= "+temp1+"\r\n ";}
		else 
		{
			temp += "测试\"4.14采集一帧图像\"失败 返回值= "+temp1+"\r\n";
		}
		m_bwriten = 1;
	}
		if(nret < 1)
			continue;
		pTestView->SaveCaptImage2File();
		pTestView->ZoomImageWithStyle();
		pTestView->SendStatImageDataMessage(0, 1);
	}
	if (reType == 4)
{
				nret = g_stLiveCaptDll.pfnLiveScan_EndCapture(g_stScannerInfo.nCurChannel);

}
else
{
			nret = g_stCaptDll.pfnScan_EndCapture(g_stScannerInfo.nCurChannel);

}
	temp1.Format("%d",nret);
	if (nret == 1) 
	{temp += "测试\"4.15结束采集一帧图像\"成功 返回值= "+temp1+"\r\n ";}
	else 
	{
		temp += "测试\"4.15结束采集一帧图像\"失败 返回值= "+temp1+"\r\n";
	}
	PutInfo2Log(strFileName,temp);
//	CFileStatus status;
//	if(mFile.GetStatus("caiji.log",status))
//	{
//		mFile.Open("caiji.log", CFile::modeWrite,&e);
//		dwPos = mFile.GetPosition();
//		mFile.SeekToEnd();
//		mFile.LockRange(dwPos, 1);
//		mFile.Write(temp,temp.GetLength());
//		mFile.UnlockRange(dwPos,1);
//		mFile.Flush();
//		mFile.Close(); 
//		
//	}
	g_stScannerInfo.bCaptureImage = 0;

	return nret;
}

LRESULT CGflsScanerTestView::OnImageData2GrayStat(WPARAM wParam, LPARAM lParam)
{
	UCHAR bOnlySelArea = (UCHAR)wParam;
	DoImageData2GrayStat(bOnlySelArea);
	return 0;
}

void CGflsScanerTestView::DoImageData2GrayStat(UCHAR bOnlySelArea)
{
	CRect rtArea(0, 0, 0, 0);
	int nw, nh;

	nw = m_stParamInfo.nImgWidth;
	nh = m_stParamInfo.nImgHeight;

	if(!bOnlySelArea)
	{
		rtArea.right  = nw;
		rtArea.bottom = nh;
		if(IsWindow(m_dlgGrayStat.GetSafeHwnd()))// && m_dlgGrayStat.IsWindowVisible())
			m_dlgGrayStat.m_staticGrayValue.SetStatGrayData(m_stParamInfo.pSrcImage, nw, nh, rtArea);
		theApp.m_bHaveImageInfo = 1;
		DoStandardCheck();
		Invalidate(FALSE);
	}

	rtArea.left		= min(m_stParamInfo.ptAreaLeftTop.x, m_stParamInfo.ptAreaRightBottom.x);	rtArea.left		/= m_stParamInfo.nCurScale;
	rtArea.right	= max(m_stParamInfo.ptAreaLeftTop.x, m_stParamInfo.ptAreaRightBottom.x);	rtArea.right	/= m_stParamInfo.nCurScale;
	rtArea.top		= min(m_stParamInfo.ptAreaLeftTop.y, m_stParamInfo.ptAreaRightBottom.y);	rtArea.top		/= m_stParamInfo.nCurScale;
	rtArea.bottom	= max(m_stParamInfo.ptAreaLeftTop.y, m_stParamInfo.ptAreaRightBottom.y);	rtArea.bottom	/= m_stParamInfo.nCurScale;
	if(IsWindow(m_dlgImgInfo.GetSafeHwnd()))
		m_dlgImgInfo.m_staticGrayValue.SetStatGrayData(m_stParamInfo.pSrcImage, nw, nh, rtArea);
	if(bOnlySelArea && m_pFrameWnd)
		m_pFrameWnd->ShowCurSelectAreaInfo(&m_stParamInfo.ptAreaLeftTop, &m_stParamInfo.ptAreaRightBottom);
}

void CGflsScanerTestView::SendStatImageDataMessage(UCHAR bOnlySelArea, UCHAR bFromThread)
{
//	OnImageData2GrayStat((WPARAM)bOnlySelArea, 0);
	if(!bFromThread)
	{
		DoImageData2GrayStat(bOnlySelArea);
		return;
	}
	
//		pTestView->SendMessage(WM_SETIMAGEINFO_TOSTAT);

//	UINT fuFlags;
//	DWORD dwRet = 0;
	WPARAM wParam = bOnlySelArea;
//	fuFlags = SMTO_BLOCK;

//	SendMessageTimeout(m_hWnd, WM_SETIMAGEINFO_TOSTAT, wParam, 0, fuFlags, 2 * 1000, &dwRet);	
	SendMessage(WM_SETIMAGEINFO_TOSTAT, wParam, 0);
}

void CGflsScanerTestView::SetTestViewScrollSizes()
{
	CSize sizeTotal;

	sizeTotal.cx = m_stParamInfo.nDispWidth;
	sizeTotal.cy = m_stParamInfo.nDispHeight;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CGflsScanerTestView::ZoomImageWithStyle()
{
	if(!m_stParamInfo.pDispImage || !m_stParamInfo.pSrcImage)
		return;

	if(m_stParamInfo.nCurScale == 1)
		memcpy(m_stParamInfo.pDispImage, m_stParamInfo.pSrcImage, m_stParamInfo.nDispImgSize);
	else	
	{
		if(m_stParamInfo.bInsertZoom)
			EnlargeOrShinkImage(m_stParamInfo.pSrcImage,  m_stParamInfo.nImgWidth,  m_stParamInfo.nImgHeight, 
								m_stParamInfo.pDispImage, m_stParamInfo.nDispWidth, m_stParamInfo.nDispHeight);
		else
			ZoomImageByScale(m_stParamInfo.pSrcImage, m_stParamInfo.nImgWidth, m_stParamInfo.nImgHeight, m_stParamInfo.pDispImage, m_stParamInfo.nCurScale);
	}

	//DrawTestInfo(NULL);
	Invalidate();
}

void CGflsScanerTestView::SaveCaptImage2File()
{
	GF_IMAGESAVEINFO *pstSaveInfo = &m_dlgCapt2Save.m_stSaveInfo;
	if(!pstSaveInfo->bSaveCaptImage)
		return;
	char szExt[6] = {0}, szFileName[300] = {0};

	if(pstSaveInfo->nFileType == 0)
		strcpy(szExt, (".bmp"));
	else
		strcpy(szExt, (".raw"));

	sprintf(szFileName, ("%s%c%s%03d%s"), pstSaveInfo->szFilePath, '\\', pstSaveInfo->szFileName, pstSaveInfo->nCurSerial, szExt);
	pstSaveInfo->nCurSerial++;
	if(pstSaveInfo->nCurSerial == 1000)
		pstSaveInfo->nCurSerial = 0;

	GFLS_IMAGEINFOSTRUCT stImageInfo = {0};
	stImageInfo.pImgData = m_stParamInfo.pSrcImage;
	stImageInfo.prgbQuad = m_stbmpInfo.bmiColors;
	memcpy(&stImageInfo.bmpHead, &m_stbmpInfo.bmiHeader, sizeof(stImageInfo.bmpHead));
	stImageInfo.bmpHead.biWidth  = m_stParamInfo.nImgWidth;
	stImageInfo.bmpHead.biHeight = m_stParamInfo.nImgHeight;
	stImageInfo.bmpHead.biSizeImage = m_stParamInfo.nImgSize;
	if(pstSaveInfo->nFileType == 0)
		Gfls_SaveBmpImgFile(szFileName, &stImageInfo);
	else
		Gfls_SaveRawImgFile(szFileName, &stImageInfo);
}

void CGflsScanerTestView::SetStdCheckType(UCHAR nStdchkType)
{
	m_stParamInfo.nStdChkType = nStdchkType;
	m_dlgImgInfo.SetStdChkInformation(nStdchkType);

	switch(nStdchkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
		{
			memset(&m_stImgParseInfo, 0, sizeof(m_stImgParseInfo));
			m_stImgParseInfo.nCurSelArea = -1;
			RECT *lprtArea = m_stImgParseInfo.rtCArea;
			lprtArea->left = 320;	lprtArea->right  = 370;
			lprtArea->top  = 270;	lprtArea->bottom = 320;
			ZoomPointByScale(lprtArea, NULL, NULL, m_stParamInfo.nCurScale, 1);
		}
		break;

	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		{
			memset(&m_stImgParseInfo, 0, sizeof(m_stImgParseInfo));
			m_stImgParseInfo.nCurSelArea = -1;
			RECT *lprtArea = m_stImgParseInfo.rtCArea;
			lprtArea->left = 120;	lprtArea->right  = 520;
			lprtArea->top  = 70;	lprtArea->bottom = 120;
			ZoomPointByScale(lprtArea, NULL, NULL, m_stParamInfo.nCurScale, 1);
			lprtArea++;
			lprtArea->left = 70;	lprtArea->right  = 120;
			lprtArea->top  = 120;	lprtArea->bottom = 270;
			ZoomPointByScale(lprtArea, NULL, NULL, m_stParamInfo.nCurScale, 1);
		}
		break;

	case GFLS_SCANNER_STDCHKTYPE_PARSE:
		{
			memset(&m_stImgParseInfo, 0, sizeof(m_stImgParseInfo));
			m_stImgParseInfo.nCurSelArea = -1;
			RECT *lprtArea = m_stImgParseInfo.rtCArea;
			int row, col, top, left, bottom, right, nStep;
			nStep = 250 * m_stParamInfo.nCurScale;
			top = 70 * m_stParamInfo.nCurScale;	bottom = top + 50 * m_stParamInfo.nCurScale;
			for(row = 0; row < 3; row++, top += nStep, bottom += nStep)
			{
				left = 20 * m_stParamInfo.nCurScale;	right = left + 50 * m_stParamInfo.nCurScale;
				for(col = 0; col < 3; col++, left += nStep, right += nStep, lprtArea++)
				{
					lprtArea->left = left;	lprtArea->right  = right;
					lprtArea->top  = top;	lprtArea->bottom = bottom;
				}
			}
		}
		break;
	}
	Invalidate(FALSE);

	if(m_stParamInfo.bCanStdCheck)
		DoStandardCheck();
}

void CGflsScanerTestView::GetLineInfoWhenStdChk()
{
	if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_NORMAL)
		return;

	if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_ABERRANCE)
	{
		m_pFrameWnd->CalculateAberParam(&m_stParamInfo.ptAreaLeftTop, &m_stParamInfo.ptAreaRightBottom);
		return;
	}

	if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_PARSE)
	{
		if(m_stImgParseInfo.nCurSelArea < 0)
			return;
		SetAreaPosByCenter(m_stImgParseInfo.ptPrevMouse.x, m_stImgParseInfo.ptPrevMouse.y, m_stImgParseInfo.rtCArea + m_stImgParseInfo.nCurSelArea, 1);
		Invalidate(FALSE);
		CheckImageParse(m_stImgParseInfo.nCurSelArea);
		return;
	}

	if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_CENTERGRAY)
	{
		if(m_stImgParseInfo.nCurSelArea != 0)
			return;
		SetAreaPosByCenter(m_stImgParseInfo.ptPrevMouse.x, m_stImgParseInfo.ptPrevMouse.y, m_stImgParseInfo.rtCArea, 1);
		Invalidate(FALSE);
		CheckCenterImageGray();
		return;
	}

	if(m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_GRAYVARIA)
	{
		if(m_stParamInfo.nCurScale < 0)
			return;
		SetAreaPosByCenter(m_stImgParseInfo.ptPrevMouse.x, m_stImgParseInfo.ptPrevMouse.y, m_stImgParseInfo.rtCArea + m_stImgParseInfo.nCurSelArea, 1);
		Invalidate(FALSE);
		CheckImageGrayVarian(m_stImgParseInfo.nCurSelArea);
		return;
	}
}
void CGflsScanerTestView::ReverseImg()
{
	int nh ,nw,nscale;
	
	nscale = m_stParamInfo.nCurScale;
    UCHAR * pImgData =m_stParamInfo.pSrcImage;
	
	nh = m_stParamInfo.nImgHeight;
	nw = m_stParamInfo.nImgWidth;
	ReverseImageData((BYTE*)m_stParamInfo.pSrcImage, nw, nh);

	if(m_stParamInfo.nCurScale == 1)
		memcpy(m_stParamInfo.pDispImage, m_stParamInfo.pSrcImage, m_stParamInfo.nDispImgSize);
	else	
	{
		if(m_stParamInfo.bInsertZoom)
			EnlargeOrShinkImage(m_stParamInfo.pSrcImage,  m_stParamInfo.nImgWidth,  m_stParamInfo.nImgHeight, 
								m_stParamInfo.pDispImage, m_stParamInfo.nDispWidth, m_stParamInfo.nDispHeight);
		else
			ZoomImageByScale(m_stParamInfo.pSrcImage, m_stParamInfo.nImgWidth, m_stParamInfo.nImgHeight, m_stParamInfo.pDispImage, m_stParamInfo.nCurScale);
	}
	Invalidate(FALSE);
}

void CGflsScanerTestView::ZoomImage2PointedScale(int nScale)
{
	int nOldScale, ntmp;

	nOldScale = m_stParamInfo.nCurScale;
	if(nOldScale == nScale)
		return;

	m_stParamInfo.nDispWidth   = m_stParamInfo.nImgWidth * nScale;
	m_stParamInfo.nDispHeight  = m_stParamInfo.nImgHeight * nScale;
	m_stParamInfo.nDispImgSize = m_stParamInfo.nImgSize * nScale * nScale;
	m_stParamInfo.nCurScale    = nScale;

	ZoomPointFromOld2New(NULL, NULL, &m_stParamInfo.ptAreaLeftTop, nOldScale, nScale);
	ZoomPointFromOld2New(NULL, NULL, &m_stParamInfo.ptAreaRightBottom, nOldScale, nScale);
	for(ntmp = 0; ntmp < 9; ntmp++)
		ZoomPointFromOld2New(m_stImgParseInfo.rtCArea + ntmp, NULL, NULL, nOldScale, nScale);

//	POINT *ppt;
//	ppt  = &m_stParamInfo.ptAreaLeftTop;
//	ntmp = ppt->x;	ppt->x = (ntmp * nScale / nOldScale);
//	ntmp = ppt->y;	ppt->y = (ntmp * nScale / nOldScale);
//	ppt  = &m_stParamInfo.ptAreaRightBottom;
//	ntmp = ppt->x;	ppt->x = (ntmp * nScale / nOldScale);
//	ntmp = ppt->y;	ppt->y = (ntmp * nScale / nOldScale);
	memset(m_stParamInfo.pDispImage, 255, m_stParamInfo.nDispImgSize);
	
	ZoomImageWithStyle();
	SetTestViewScrollSizes();
	m_pFrameWnd->ShowCurSelectAreaInfo(&m_stParamInfo.ptAreaLeftTop, &m_stParamInfo.ptAreaRightBottom);
	m_dlgImgInfo.showImageZoomInfo();
}

void CGflsScanerTestView::DoStandardCheck()
{
	m_stParamInfo.bCanStdCheck = 1;
	m_pFrameWnd->ClearImageAberParamInfo();

	switch(m_stParamInfo.nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_PARSE:
		CheckImageParse(-1);
		return;

	case GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYUNAVE:
		CheckBackImageGray(0);
		return;

	case GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT:
		CheckBackImageGray(1);
		return;

	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
		CheckCenterImageGray();
		return;

	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		CheckImageGrayVarian(-1);
		return;

	case GFLS_SCANNER_STDCHKTYPE_BADPOINT:
		CheckImageBadPoint();
		return;
	}
}

void CGflsScanerTestView::CheckBackImageGray(UCHAR bChkEnvirn)
{
	if(!m_stParamInfo.bCanStdCheck)
		return;

	CEdit* pEdit;
	CString strInfo;
	if(!bChkEnvirn)
		strInfo = "正在检测背景图像的灰度不均匀度...";
	else
		strInfo = "正在检测环境光产生的灰度不均匀度...";

	pEdit = (CEdit*)m_pFrameWnd->m_wndStdChkBar.GetDlgItem(IDC_EDIT_STDCHKRESULT);
	pEdit->SetWindowText(strInfo);
	pEdit->UpdateWindow();

	CWaitCursor waitCur;
	double	fMedian[1024] = {0}, *pfOneMedia;
	GF_IMAGEGRAYSTATSTRUCT	stGrayStat = {0};
	RECT	*lprtArea = &stGrayStat.rtArea;
	stGrayStat.pImageData = m_stParamInfo.pSrcImage;
	stGrayStat.nw		  = m_stParamInfo.nImgWidth;
	stGrayStat.nh		  = m_stParamInfo.nImgHeight;

	int row, col;

	lprtArea->top  = 20;	lprtArea->bottom = 40;
	

	pfOneMedia = fMedian;
	for(row = 20; row < 620; row += 580,lprtArea->top += 580, lprtArea->bottom += 580)
	{
		lprtArea->left = 20;	lprtArea->right  = 40; 
		for(col = 20; col < 620; col += 580, lprtArea->left += 580, lprtArea->right += 580,pfOneMedia++)
		{
			GetImageGrayStatisticInfo(&stGrayStat);
			*pfOneMedia = stGrayStat.fAverage;
		}
	}
	lprtArea->top  = 170;	lprtArea->bottom = 190;
		for(row = 170; row < 470; row += 280,lprtArea->top += 280, lprtArea->bottom += 280)
	{
		lprtArea->left = 170;	lprtArea->right  = 190; 
		for(col = 170; col < 470; col += 280, lprtArea->left += 280, lprtArea->right += 280,pfOneMedia++)
		{
			GetImageGrayStatisticInfo(&stGrayStat);
			*pfOneMedia = stGrayStat.fAverage;
		}
	}
	lprtArea->top  = 300;	lprtArea->bottom = 320;lprtArea->left = 320;lprtArea->right =340;
	GetImageGrayStatisticInfo(&stGrayStat);
	*pfOneMedia = stGrayStat.fAverage;	
	double fMinMedian, fMaxMedian, fResult;
	pfOneMedia = fMedian + 1;
	fMinMedian = fMaxMedian = fMedian[0];
	for(row = 1; row < 9; row++, pfOneMedia++)
	{
		if(*pfOneMedia > fMaxMedian)
			fMaxMedian = *pfOneMedia;
		if(*pfOneMedia < fMinMedian)
			fMinMedian = *pfOneMedia;
	}
	fResult = (fMaxMedian - fMinMedian) /(fMaxMedian + fMinMedian) * 100;

//	strInfo.Format("对均匀分割的20x20小矩形区域计算得到的灰度中间值中：\r\n灰度中间值的最大值为：%.3f\r\n灰度中间值的最小值为：%.3f\r\n图像背景的灰度不均匀度为：%.3f",
	strInfo.Format(_T("灰度平均值的最大值为：%.3f\r\n灰度平均值的最小值为：%.3f\r\n图像背景的灰度不均匀度为：%.3f%%"),
				   fMaxMedian, fMinMedian, fResult);
//	CString strTemp;
//	if(!bChkEnvirn)
//		strTemp = "\r\n警用指纹采集仪图像背景灰度不均匀度要求：<=10%";
//	else
//		strTemp = "\r\n环境光检测要求：生成的图像背景灰度不均匀度小于等于15%";
//	strInfo += strTemp;
	pEdit->SetWindowText(strInfo);
}

void CGflsScanerTestView::CheckImageParse(int nSelIdx)
{
	if(!m_stParamInfo.bCanStdCheck)
		return;

	CEdit* pEdit;
	CString strInfo;

	strInfo = _T("正在检测图像解析度...");
	pEdit = (CEdit*)m_pFrameWnd->m_wndStdChkBar.GetDlgItem(IDC_EDIT_STDCHKRESULT);
	pEdit->SetWindowText(strInfo);
	pEdit->UpdateWindow();

	CWaitCursor waitCur;

	int naGrayCount[256] = {0}, i, nIndex;
	GF_IMAGEGRAYSTATSTRUCT	stGrayStat = {0};
	stGrayStat.pImageData		= m_stParamInfo.pSrcImage;
	stGrayStat.pnGrayValueCnt	= naGrayCount;
	stGrayStat.nw				= m_stParamInfo.nImgWidth;
	stGrayStat.nh				= m_stParamInfo.nImgHeight;

	if(nSelIdx < 0)
	{
		i = 0;
		nSelIdx = 8;
	}
	else
	{
		i = nSelIdx;
	}
	for(; i <= nSelIdx; i++)
	{
		memcpy(&stGrayStat.rtArea, m_stImgParseInfo.rtCArea + i, sizeof(stGrayStat.rtArea));
		ZoomPointByScale(&stGrayStat.rtArea, NULL, NULL, m_stParamInfo.nCurScale, 0);
		GetImageAreaLineGray(&stGrayStat, m_stImgParseInfo.fBackMedian + i, m_stImgParseInfo.fWhiteMedian + i);
	}

	const TCHAR* pszFormat = _T("的三个C块中的白色线条和深色线条的灰度平均值分别为：(%.2f, %.2f)\t(%.2f, %.2f)\t(%.2f, %.2f)\r\n");
	CString strTmp;

	strInfo.Empty();
	nIndex = 0;
	for(i = 0; i < 3; i++, nIndex += 3)
	{
		if(i == 0)
			strInfo += _T("上面");
		else if(i == 1)
			strInfo += _T("中间");
		else
			strInfo += _T("小面");
		strTmp.Format(pszFormat, 
					  m_stImgParseInfo.fWhiteMedian[nIndex], m_stImgParseInfo.fBackMedian[nIndex],
					  m_stImgParseInfo.fWhiteMedian[nIndex + 1], m_stImgParseInfo.fBackMedian[nIndex + 1],
					  m_stImgParseInfo.fWhiteMedian[nIndex + 2], m_stImgParseInfo.fBackMedian[nIndex + 2]);
		strInfo += strTmp;
	}
//	strTmp = "解析度要求：白色线条和深色线条的灰度平均值之差不小于40。";
//	strInfo += strTmp;

	pEdit->SetWindowText(strInfo);
}

void CGflsScanerTestView::CheckCenterImageGray()
{
	if(!m_stParamInfo.bCanStdCheck)
		return;

	CEdit* pEdit;
	CString strInfo;	//, strTemp;

	strInfo = "正在检测中心区域的图像灰度值...";
	pEdit = (CEdit*)m_pFrameWnd->m_wndStdChkBar.GetDlgItem(IDC_EDIT_STDCHKRESULT);
	pEdit->SetWindowText(strInfo);
	pEdit->UpdateWindow();

	CWaitCursor waitCur;

	GF_IMAGEGRAYSTATSTRUCT	stGrayStat = {0};
	stGrayStat.pImageData		= m_stParamInfo.pSrcImage;
	stGrayStat.nw				= m_stParamInfo.nImgWidth;
	stGrayStat.nh				= m_stParamInfo.nImgHeight;
	memcpy(&stGrayStat.rtArea, m_stImgParseInfo.rtCArea, sizeof(stGrayStat.rtArea));
	ZoomPointByScale(&stGrayStat.rtArea, NULL, NULL, m_stParamInfo.nCurScale, 0);
	GetImageGrayStatisticInfo(&stGrayStat);

	strInfo.Format(_T("由TP-F04产生的图像中A色块图像的灰度平均值为：%.3f\r\n"), stGrayStat.fAverage);	// stGrayStat.fMedian);
//	strTemp = "指纹采集仪中心区域图像灰度值要求：A色块图像的灰度平均值不大于95。";
//	strInfo += strTemp;

	pEdit->SetWindowText(strInfo);
}

void CGflsScanerTestView::CheckImageGrayVarian(int nSelIdx)
{
	if(!m_stParamInfo.bCanStdCheck)
		return;

	CEdit* pEdit;
	CString strInfo;

	strInfo = "正在检测图像灰度动态范围...";
	pEdit = (CEdit*)m_pFrameWnd->m_wndStdChkBar.GetDlgItem(IDC_EDIT_STDCHKRESULT);
	pEdit->SetWindowText(strInfo);
	pEdit->UpdateWindow();

	CWaitCursor waitCur;

	int i;
	GF_IMAGEGRAYSTATSTRUCT	stGrayStat = {0};
	stGrayStat.pImageData		= m_stParamInfo.pSrcImage;
	stGrayStat.nw				= m_stParamInfo.nImgWidth;
	stGrayStat.nh				= m_stParamInfo.nImgHeight;

	if(nSelIdx < 0)
	{
		i = 0;
		nSelIdx = 1;
	}
	else
	{
		i = nSelIdx;
	}
	for(; i <= nSelIdx; i++)
	{
		memcpy(&stGrayStat.rtArea, m_stImgParseInfo.rtCArea + i, sizeof(stGrayStat.rtArea));
		ZoomPointByScale(&stGrayStat.rtArea, NULL, NULL, m_stParamInfo.nCurScale, 0);
		GetImageGrayStatisticInfo(&stGrayStat);
		m_stImgParseInfo.fBackMedian[i] = stGrayStat.fAverage;	//stGrayStat.fMedian;
	}

	CString strTemp;
	strInfo.Format(_T("G0色块的灰度平均值为：%.3f\r\nG10色块的灰度平均值为：%.3f\r\n"), m_stImgParseInfo.fBackMedian[0], m_stImgParseInfo.fBackMedian[1]);
	strTemp.Format(_T("灰度动态范围为：%.3f\r\n"), m_stImgParseInfo.fBackMedian[0] - m_stImgParseInfo.fBackMedian[1]);
	strInfo += strTemp;
//	strTemp = "灰度动态范围的要求：>= 125";
//	strInfo += strTemp;

	pEdit->SetWindowText(strInfo);
}

void CGflsScanerTestView::CheckImageBadPoint()
{
	if(!m_stParamInfo.bCanStdCheck)
		return;

	CEdit* pEdit;
	TCHAR szInfo[2048] = {0};
	CString strInfo;

	_tcscpy(szInfo, _T("正在检测图像疵点..."));
	pEdit = (CEdit*)m_pFrameWnd->m_wndStdChkBar.GetDlgItem(IDC_EDIT_STDCHKRESULT);
	pEdit->SetWindowText(szInfo);
	pEdit->UpdateWindow();

	CWaitCursor waitCur;

	memset(szInfo, 0, sizeof(szInfo));
	m_ImageBadptStat.CheckImageBadPoint();
	m_ImageBadptStat.GetBadPointStatInfo(szInfo);
	pEdit->SetWindowText(szInfo);
}

void CGflsScanerTestView::OnStdChkCaptSpeed()
{
	CDlgCheckSpeed dlg;
	dlg.m_pTestView = this;
	dlg.DoModal();
}
void CGflsScanerTestView::OnSETPWH()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	if (reType == 4)
{
			nret = g_stLiveCaptDll.pfnLiveScan_GetCaptWindow(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nXOrigin, &g_stScannerInfo.nYOrigin,
													 &g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
	
}
else
{
			nret = g_stCaptDll.pfnScan_GetCaptWindow(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nXOrigin, &g_stScannerInfo.nYOrigin,
													 &g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);

}
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"4.10设置当前图像的采集位置、宽度和高度\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1)
	{
		temp += "测试\"4.10设置当前图像的采集位置、宽度和高度\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}
void CGflsScanerTestView::OnGETSETUPDLG()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
	CFileStatus status;
	CFileException e;
	if (reType == 4)
{
				nret = g_stLiveCaptDll.pfnLiveScan_Setup();

}
else
{
			nret = g_stCaptDll.pfnScan_Setup();
}
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"4.11调用采集设备的属性设置对话框\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1)
	{
		temp += "测试\"4.11调用采集设备的属性设置对话框\"成功 返回值= "+temp1+"\r\n";
	}
	PutInfo2Log(strFileName,temp);
//		if(mFile.GetStatus("caiji.log",status))
//	{
//		mFile.Open("caiji.log",CFile::modeWrite,&e);
//		mFile.SeekToEnd();
//		dwPos = mFile.GetPosition();
//		mFile.LockRange(dwPos,1);
//		mFile.Write(temp,temp.GetLength());		
//		mFile.UnlockRange(dwPos,1);
//		mFile.Flush();
//		mFile.Close(); 
//	}
}
void CGflsScanerTestView::OnISISFIN()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	nret = g_stMosaicDll.pfnMosaic_IsSupportIdentifyFinger();
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"拼接接口是否提供判断图像为指纹的函数\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1||nret == 0)
	{
		temp += "测试\"拼接接口是否提供判断图像为指纹的函数\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}
void CGflsScanerTestView::OnISSIMGQUA()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	nret = g_stMosaicDll.pfnMosaic_IsSupportImageQuality();
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"拼接接口是否提供判断图像质量的函数\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1||nret == 0)
	{
		temp += "测试\"拼接接口是否提供判断图像质量的函数\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}
void CGflsScanerTestView::OnISSFINQUA()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	nret = g_stMosaicDll.pfnMosaic_IsSupportFingerQuality();
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"拼接接口是否提供判断指纹质量的函数\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1||nret == 0)
	{
		temp += "测试\"拼接接口是否提供判断指纹质量的函数\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}
void CGflsScanerTestView::OnISSIMGEN()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	nret = g_stMosaicDll.pfnMosaic_IsSupportImageEnhance();
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"接口是否提供拼接指纹的图像增强功能\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1||nret == 0)
	{
		temp += "测试\"接口是否提供拼接指纹的图像增强功能\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}
void CGflsScanerTestView::OnIMGQUA()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	UCHAR *pData;
	pData = (UCHAR*)malloc(1024*768);
	nret = g_stMosaicDll.pfnMosaic_ImageQuality(pData,1024,768);
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"判断图像质量\"失败 返回值= "+temp1+" \r\n";}
	else 
	{
		temp += "测试\"判断图像质量\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}
void CGflsScanerTestView::OnFINQUA()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	UCHAR *pData;
	pData = (UCHAR*)malloc(1024*768);
	nret =  g_stMosaicDll.pfnMosaic_FingerQuality(pData, 1024, 768);
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"判断指纹质量\"失败 返回值= "+temp1+" \r\n";}
	else 
	{
		temp += "测试\"判断指纹质量\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}
void CGflsScanerTestView::OnIMGENH()
{
	CStringA temp,temp1;
	CFile mFile;
	int nret;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
//	CFileStatus status;
//	CFileException e;
	/*DWORD dwPos;*/
	UCHAR *pData;
	pData = (UCHAR*)malloc(1024*768);
	UCHAR *pBuffer;
	pBuffer = (UCHAR*)malloc(640*640);
	nret =  g_stMosaicDll.pfnMosaic_ImageEnhance(pBuffer, 1024, 768,pData );
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"5.14对图像进行增强\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1)
	{
		temp += "测试\"5.14对图像进行增强\"成功 返回值= "+temp1+"\r\n";
	}
	 PutInfo2Log(strFileName,temp);
}

void CGflsScanerTestView::OnCaptImages2Save()
{
	ShowAllInfoDlg(1, 0x04);
}



void CGflsScanerTestView::OnBtnSetSquare()
{
	CDlgSquareParam dlg;
	dlg.SetParamInfo(&m_stParamInfo, m_bMoveSquare,	m_bShowRSquare,
		m_bShowGSquare, m_bShowBSquare);
	if( IDOK == dlg.DoModal())
	{
		int nwS4, nwS1;
		CRect rect;
		nwS4 = dlg.m_nWS4;
		nwS1 = dlg.m_nWS1;		
		rect = m_stParamInfo.rectSquare4;	

		if(m_bMoveSquare != dlg.m_bMoveSquare)
		{
			m_bMoveSquare = dlg.m_bMoveSquare;
			if(m_bMoveSquare) m_stParamInfo.nStdChkType = GFLS_SCANNER_STDCHKTYPE_MOVESQUARE;
			else m_stParamInfo.nStdChkType = GFLS_SCANNER_STDCHKTYPE_NORMAL;
		}	
		if(m_bShowRSquare != dlg.m_bShowRSquare)m_bShowRSquare = dlg.m_bShowRSquare;
		if(m_bShowGSquare != dlg.m_bShowGSquare)m_bShowGSquare = dlg.m_bShowGSquare;
		if(m_bShowBSquare != dlg.m_bShowBSquare) m_bShowBSquare = dlg.m_bShowBSquare;

		if(m_stParamInfo.nSquare4 != nwS4 || m_stParamInfo.nSquare1 != nwS1)
		{
			if(nwS4 > GFLS_SCANNER_IMAGEWIDTH || nwS1 > GFLS_SCANNER_IMAGEWIDTH )
			{
				MessageBox(_T("无效的矩形参数!参数值过大！"), _T("活体采集测试程序"), MB_OK);
				return;
			}

			if(nwS4 < 10	|| nwS1 < 10)
			{
				MessageBox(_T("无效的矩形参数!参数值过小！"), _T("活体采集测试程序"), MB_OK);
				return;
			}

			m_stParamInfo.nSquare4 = nwS4;
			m_stParamInfo.nSquare1 = nwS1;
			ReSetSquare4Rect(nwS4, nwS4);								
		}	
		Invalidate(FALSE);	
	}

}
void CGflsScanerTestView::ReSetSquare4Rect(int nwidth, int nheight)
{
	CRect rect;
	int nLeft, nTop;
	rect = m_stParamInfo.rectSquare4;

	nLeft = (GFLS_SCANNER_IMAGEWIDTH - nwidth)/2;
	nTop = (GFLS_SCANNER_IMAGEHEIGHT - nheight)/2;
	m_stParamInfo.rectSquare4.SetRect(nLeft , nTop,
		                       nLeft+ nwidth, nTop + nheight);
}
