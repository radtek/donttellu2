// CheckToolForm.cpp : implementation file
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "CheckToolForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CGflsScanerTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCheckToolForm

IMPLEMENT_DYNCREATE(CCheckToolForm, CFormView)

CCheckToolForm::CCheckToolForm()
	: CFormView(CCheckToolForm::IDD)
{
	//{{AFX_DATA_INIT(CCheckToolForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	memset(&m_stSampleData, 0, sizeof(m_stSampleData));
	m_stSampleData.nChkToolType = GFLS_SCANNER_CHKTOOL_GRIDSAMPLE;
}

CCheckToolForm::~CCheckToolForm()
{
}

void CCheckToolForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckToolForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckToolForm, CFormView)
	//{{AFX_MSG_MAP(CCheckToolForm)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckToolForm diagnostics

#ifdef _DEBUG
void CCheckToolForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CCheckToolForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCheckToolForm message handlers


BOOL CCheckToolForm::Gf_CreateView(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if(!Create(NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL))
		return FALSE;
//	ShowWindow(SW_HIDE);

	return TRUE;
}

void CCheckToolForm::OnDestroy() 
{
	if(m_stSampleData.pSampleData)
		free(m_stSampleData.pSampleData);
	if(m_stSampleData.pDispData)
		free(m_stSampleData.pDispData);
	m_stSampleData.pSampleData = m_stSampleData.pDispData = NULL;
	
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CCheckToolForm::OnDraw(CDC* pDC) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	DrawCheckToolSample();
}

BOOL CCheckToolForm::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	
	return CFormView::OnEraseBkgnd(pDC);
}

void CCheckToolForm::MakeCheckToolSample()
{
	switch(m_stSampleData.nChkToolType)
	{
	case GFLS_SCANNER_CHKTOOL_RESOLUTION:
		MakeResolutionData();
		break;

	default:
		MakeGridSampleData();
		break;
	}

	// 创建位图数据
	HBITMAP hBitMap = NULL, hOldbmp = NULL;
	UCHAR *pBits = NULL;
	CDC MemDC;
	CClientDC dc(this);
	MemDC.CreateCompatibleDC(&dc);
	hBitMap = CreateDIBSection(MemDC.GetSafeHdc(), (BITMAPINFO*)m_stSampleData.pstbmpInfo, DIB_RGB_COLORS, (VOID**)&pBits, NULL, 0);
	if(NULL == hBitMap || NULL == pBits)
		goto FINISH_EXIT;
	hOldbmp = (HBITMAP)MemDC.SelectObject(hBitMap);
	MemDC.SetBkMode(TRANSPARENT);
	DrawDataInMemDC(&MemDC, m_stSampleData.nDataWidth, m_stSampleData.nDataHeight);

	int nSize;
	nSize = m_stSampleData.nDataWidth * m_stSampleData.nDataHeight;
	m_stSampleData.pSampleData = (UCHAR*)malloc(nSize);
	if(m_stSampleData.pSampleData)
	{
		memset(m_stSampleData.pSampleData, 255, nSize);
		memcpy(m_stSampleData.pSampleData, pBits, m_stSampleData.nDataWidth * m_stSampleData.nDataHeight);

		nSize = m_stSampleData.nDispWidth * m_stSampleData.nDispHeight;
		m_stSampleData.pDispData = (UCHAR*)malloc(nSize);
		if(m_stSampleData.pDispData)
		{
			memset(m_stSampleData.pDispData, 255, nSize);
			EnlargeOrShinkImage(m_stSampleData.pSampleData, m_stSampleData.nDataWidth, m_stSampleData.nDataHeight, m_stSampleData.pDispData, m_stSampleData.nDispWidth, m_stSampleData.nDispHeight);
			m_stSampleData.pstbmpInfo->bmiHeader.biWidth  = m_stSampleData.nDispWidth;
			m_stSampleData.pstbmpInfo->bmiHeader.biHeight = m_stSampleData.nDispHeight;
			m_stSampleData.pstbmpInfo->bmiHeader.biSizeImage = nSize;
		}
	}

FINISH_EXIT:
	if(hOldbmp)
		MemDC.SelectObject(hOldbmp);
	if(hBitMap)
		DeleteObject(hBitMap);

//	CSize sizeTotal;
//	sizeTotal.cx = m_stSampleData.nDataWidth;
//	sizeTotal.cy = m_stSampleData.nDataHeight;
//	SetScrollSizes(MM_TEXT, sizeTotal);

}

void CCheckToolForm::MakeGridSampleData()
{
	int nmDpi, nSize;

	m_stSampleData.nDPI    = 2540 / 2;		// 分辨率
	m_stSampleData.nLength = 325;			// 长度mm*10
	nmDpi = (int)(m_stSampleData.nDPI / 2.54 * 100);

	m_stSampleData.pstbmpInfo = &theApp.m_stbmpInfo;
	BITMAPINFOHEADER* pHead = &(m_stSampleData.pstbmpInfo->bmiHeader);

	nSize = m_stSampleData.nDPI / 254 * m_stSampleData.nLength;
	nSize = (nSize + 3) / 4 * 4;
	m_stSampleData.nDataWidth  = nSize;
	m_stSampleData.nDataHeight = nSize;

	pHead->biWidth			= m_stSampleData.nDataWidth;
	pHead->biHeight			= m_stSampleData.nDataHeight;
	pHead->biSizeImage		= m_stSampleData.nDataWidth * m_stSampleData.nDataHeight;
	pHead->biXPelsPerMeter	= nmDpi;
	pHead->biYPelsPerMeter	= nmDpi;

/*
	OnPrepareDC(&dc);
	CBrush	brush(RGB(255, 255, 255));
*/
}

void CCheckToolForm::MakeResolutionData()
{
}

void CCheckToolForm::DrawCheckToolSample()
{
	CClientDC clientDC(this);
	int nw, nh;

	nw = m_stSampleData.nDispWidth;
	nh = m_stSampleData.nDispHeight;

	CDC dc;
	CBitmap bmp, *poldbmp = NULL;
	dc.CreateCompatibleDC(&clientDC);
	bmp.CreateCompatibleBitmap(&clientDC, nw, nh);
	poldbmp = dc.SelectObject(&bmp);

	if(m_stSampleData.pDispData)
		::SetDIBitsToDevice(dc.GetSafeHdc(), 0, 0, nw, nh, 0, 0, 0, nh, m_stSampleData.pDispData, (BITMAPINFO*)m_stSampleData.pstbmpInfo, DIB_RGB_COLORS);
	clientDC.BitBlt(0, 0, nw, nh, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(poldbmp);
}

void CCheckToolForm::DrawDataInMemDC(CDC* pdc, int nw, int nh)
{
	switch(m_stSampleData.nChkToolType)
	{
	case GFLS_SCANNER_CHKTOOL_RESOLUTION:
		DrawResolutionSample(pdc, nw, nh);
		break;

	default:
		DrawGridMarkSample(pdc, nw, nh);
		break;
	}
}

void CCheckToolForm::DrawGridMarkSample(CDC* pdc, int nw, int nh)
{
	CBrush brush(RGB(255, 255, 255));
	CPen   pen, *poldpen;
	CRect rt(0, 0, nw, nh);

	pdc->FillRect(rt, &brush);

	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	poldpen = pdc->SelectObject(&pen);

	int nStep, cx,cy;
	nStep = m_stSampleData.nDPI / 254 * 10;

	cx = 12;
	for(; cx < nw; cx += nStep)
	{
		pdc->MoveTo(cx, 0);
		pdc->LineTo(cx, nh);
	}
	cy = 12;
	for(; cy < nh; cy += nStep)
	{
		pdc->MoveTo(0,  cy);
		pdc->LineTo(nw, cy);
	}

	pdc->MoveTo(0, 0);
	pdc->LineTo(0, nh);
	pdc->LineTo(nw, nh);
	pdc->LineTo(nw, 0);
	pdc->LineTo(0, 0);

	pdc->SelectObject(poldpen);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CCheckToolForm::DrawResolutionSample(CDC* pdc, int nw, int nh)
{
}
