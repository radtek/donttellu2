// DlgCheckTool.cpp : implementation file
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "DlgCheckTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckTool dialog


CDlgCheckTool::CDlgCheckTool(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCheckTool::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCheckTool)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pChkToolForm   = NULL;
	m_nChkToolSample = GFLS_SCANNER_CHKTOOL_GRIDSAMPLE;
}


void CDlgCheckTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCheckTool)
	DDX_Control(pDX, IDC_STATIC_FORMPLACE, m_staticFormPlace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCheckTool, CDialog)
	//{{AFX_MSG_MAP(CDlgCheckTool)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckTool message handlers

BOOL CDlgCheckTool::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CreateChkToolForm();

	CString strTitle = "标准网格标定检测样板";
	switch(m_nChkToolSample)
	{
	case GFLS_SCANNER_CHKTOOL_RESOLUTION:
		strTitle = "标准分辨率检测样板";
		break;

	default:
		break;
	}
	SetWindowText(strTitle);

	CenterWindow(CWnd::GetDesktopWindow());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCheckTool::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CDlgCheckTool::OnOK() 
{
	// TODO: Add extra validation here
	if(NULL == m_pChkToolForm)
		return;
	GF_CHKTOOLSAMPLEDATA* pSampleData = &m_pChkToolForm->m_stSampleData;
	if(NULL == pSampleData->pSampleData)
		return;

	pSampleData->pstbmpInfo->bmiHeader.biWidth  = pSampleData->nDataWidth;
	pSampleData->pstbmpInfo->bmiHeader.biHeight = pSampleData->nDataHeight;
	pSampleData->pstbmpInfo->bmiHeader.biSizeImage = pSampleData->nDataWidth * pSampleData->nDataHeight;
	Gf_SaveData2BitmapFile(pSampleData->pSampleData, pSampleData->nDataWidth, pSampleData->nDataHeight, "c:\\temp\\Sample.bmp");
	return;
	
	CDialog::OnOK();
}

void CDlgCheckTool::CreateChkToolForm()
{
	m_pChkToolForm = new CCheckToolForm;
	if(NULL == m_pChkToolForm)
		return;
	m_pChkToolForm->m_stSampleData.nChkToolType = m_nChkToolSample;
	m_pChkToolForm->Gf_CreateView(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, 8888);

	CRect rtWnd, rtClient;
	GetWindowRect(rtWnd);
	GetClientRect(rtClient);
	int nWedge, nTitle, nw, nh, nSampleWidth, nSampleHeight;;
	nWedge = rtWnd.Width() - rtClient.right;
	nTitle = rtWnd.Height() - rtClient.bottom;
	m_staticFormPlace.GetWindowRect(rtWnd);
	ScreenToClient(rtWnd);
	m_staticFormPlace.ShowWindow(SW_HIDE);
	m_staticFormPlace.EnableWindow(0);
	switch(m_nChkToolSample)
	{
	case GFLS_SCANNER_CHKTOOL_RESOLUTION:
		nSampleWidth  = 648;
		nSampleHeight = 648;
		break;

	default:
		nSampleWidth  = 640;
		nSampleHeight = 640;
		break;
	}
	rtWnd.right  = rtWnd.left + nSampleWidth;
	rtWnd.bottom = rtWnd.top  + nSampleHeight;
	m_pChkToolForm->MoveWindow(rtWnd);
	m_pChkToolForm->m_stSampleData.nDispWidth  = nSampleWidth;
	m_pChkToolForm->m_stSampleData.nDispHeight = nSampleHeight;

	int nHDist, nVDist, nleft, ntop;
	nHDist = rtWnd.left;	nVDist = rtWnd.top;
	nw = 2 * nHDist + nSampleWidth;	nh = nSampleHeight + nVDist * 2;
	GetDlgItem(IDCANCEL)->GetWindowRect(rtWnd);
	ScreenToClient(rtWnd);
	nh += (rtClient.bottom - rtWnd.top);
	nleft = nw - nHDist - rtWnd.Width();
	ntop  = nh - (rtClient.bottom - rtWnd.top);
	GetDlgItem(IDCANCEL)->MoveWindow(nleft, ntop, rtWnd.Width(), rtWnd.Height());
	GetDlgItem(IDOK)->MoveWindow(nleft - rtWnd.Width() - 5, ntop, rtWnd.Width(), rtWnd.Height());

	MoveWindow(0, 0, nw + nWedge, nh + nTitle, FALSE);
	m_pChkToolForm->MakeCheckToolSample();
}


BOOL CDlgCheckTool::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_pChkToolForm && IsWindow(m_pChkToolForm->GetSafeHwnd()))
	{
		if(m_pChkToolForm->PreTranslateInput(pMsg))
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgCheckTool::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_pChkToolForm && IsWindow(m_pChkToolForm->GetSafeHwnd()))
	{
		if(m_pChkToolForm->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}
	
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
