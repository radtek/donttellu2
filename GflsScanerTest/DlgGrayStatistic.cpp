// DlgGrayStatistic.cpp : implementation file
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "DlgGrayStatistic.h"
#include "GflsScanerTestView.h"
#include "DlgImageInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGrayStatistic dialog


CDlgGrayStatistic::CDlgGrayStatistic(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGrayStatistic::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGrayStatistic)
	m_strAverage = _T("");
	m_strCntInRange = _T("");
	m_strDiffValue = _T("");
	m_strMedian = _T("");
	m_strPercent = _T("");
	m_strPixelCount = _T("");
	m_strRange = _T("");
	//}}AFX_DATA_INIT
	m_pTestView = NULL;
}


void CDlgGrayStatistic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGrayStatistic)
	DDX_Control(pDX, IDC_STATIC_GRAYSTAT, m_staticGrayValue);
	DDX_Text(pDX, IDC_STATIC_AVERAGE, m_strAverage);
	DDX_Text(pDX, IDC_STATIC_CNTINRANGE, m_strCntInRange);
	DDX_Text(pDX, IDC_STATIC_DIFF, m_strDiffValue);
	DDX_Text(pDX, IDC_STATIC_MEDIAN, m_strMedian);
	DDX_Text(pDX, IDC_STATIC_PERCENT, m_strPercent);
	DDX_Text(pDX, IDC_STATIC_PIXELCNT, m_strPixelCount);
	DDX_Text(pDX, IDC_STATIC_RANGE, m_strRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGrayStatistic, CDialog)
	//{{AFX_MSG_MAP(CDlgGrayStatistic)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOWIMAGE_STATINFO, OnSetImageStatInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGrayStatistic message handlers

BOOL CDlgGrayStatistic::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_staticGrayValue.m_pParentWnd   = this;
	m_staticGrayValue.m_pDlgGrayStat = this;

	CenterWindow(CWnd::GetDesktopWindow());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGrayStatistic::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CDlgGrayStatistic::OnOK() 
{
	// TODO: Add extra validation here
	return;
	
	CDialog::OnOK();
}

void CDlgGrayStatistic::OnCancel() 
{
	// TODO: Add extra cleanup here
	ShowWindow(SW_HIDE);
	return;
	
	CDialog::OnCancel();
}

LRESULT CDlgGrayStatistic::OnSetImageStatInfo(WPARAM wParam, LPARAM lParam)
{
	SetImageStatInfo();
	return	(LRESULT)0;
}

void CDlgGrayStatistic::SetImageStatInfo()
{
#if 1	
	if(!m_staticGrayValue.m_bHaveGrayInfo)
	{
		m_strAverage	= "";
		m_strDiffValue	= "";
		m_strMedian		= "";
		m_strPixelCount	= "";
		m_strRange		= "";
		m_strCntInRange	= "";
		m_strPercent	= "";
	}
	else
	{
		m_strAverage.Format(_T("%.2f"), m_staticGrayValue.m_stGrayStat.fAverage);
		m_strDiffValue.Format(_T("%.2f"), m_staticGrayValue.m_stGrayStat.fDiff);
		m_strMedian.Format(_T("%.2f"), m_staticGrayValue.m_stGrayStat.fMedian);
		m_strPixelCount.Format(_T("%d"), m_staticGrayValue.m_stGrayStat.nPixelCount);

		int x1, x2;
		x1 = min(m_staticGrayValue.m_nGrayValue1, m_staticGrayValue.m_nGrayValue2);
		x2 = max(m_staticGrayValue.m_nGrayValue1, m_staticGrayValue.m_nGrayValue2);
		if(x1 == x2)
			m_strRange.Format(_T("%d"), x1);
		else
			m_strRange.Format(_T("%d...%d"), x1, x2);
		m_strCntInRange.Format(_T("%d"), m_staticGrayValue.m_nGrayCntInRang);
		m_strPercent.Format(_T("%.2f"), m_staticGrayValue.m_fPercent);
	}
#endif

	m_pTestView->m_dlgImgInfo.GetDlgItem(IDC_STATIC_AVERAGE3)->SetWindowText(m_strAverage);
	m_pTestView->m_dlgImgInfo.GetDlgItem(IDC_STATIC_MEDIAN3)->SetWindowText(m_strMedian);
	m_pTestView->m_dlgImgInfo.GetDlgItem(IDC_STATIC_DIFF3)->SetWindowText(m_strDiffValue);
	m_pTestView->m_dlgImgInfo.GetDlgItem(IDC_STATIC_PIXELCNT3)->SetWindowText(m_strPixelCount);

//	UpdateData(0);

//	return 1;
}

BOOL CDlgGrayStatistic::CreateGrayStatDlg(CWnd* pParentWnd, CRect &rtFrame)
{
	if(!Create(IDD, pParentWnd))
		return FALSE;

	CRect rtWnd, rtClient;
	GetClientRect(rtClient);
	m_staticGrayValue.GetWindowRect(rtWnd);
	ScreenToClient(rtWnd);
	rtWnd.left  = (rtClient.right - 256) / 2;
	rtWnd.right = rtWnd.left + 256;
	m_staticGrayValue.MoveWindow(rtWnd);

#if 0
	CRect rtWnd, rtClient;

	GetClientRect(rtClient);
	GetWindowRect(rtWnd);

	int nWedge, nTitle;
	nWedge = rtWnd.Width() - rtClient.right;
	nTitle = rtWnd.Height() - rtClient.bottom;

	rtWnd.left = rtFrame.left;
	rtWnd.right = rtWnd.left + rtClient.right + nWedge;
	rtWnd.top = rtFrame.bottom;
	rtWnd.bottom = rtWnd.top + rtClient.bottom + nTitle;

	rtFrame = rtWnd;

	MoveWindow(rtWnd, FALSE);

	m_staticGrayValue.GetWindowRect(rtWnd);
	ScreenToClient(rtWnd);
	rtWnd.left  = (rtClient.right - 256) / 2;
	rtWnd.right = rtWnd.left + 256;
	m_staticGrayValue.MoveWindow(rtWnd);
#endif

	ShowWindow(SW_HIDE);
	return TRUE;	
}

BOOL CDlgGrayStatistic::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(IsWindow(m_staticGrayValue.GetSafeHwnd()))
	{
		if(m_staticGrayValue.PreTranslateMessage(pMsg))
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgGrayStatistic::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(IsWindow(m_staticGrayValue.GetSafeHwnd()))
	{
		if(m_staticGrayValue.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
