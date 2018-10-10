// DlgSelectChannel.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "DlgSelectChannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectChannel dialog


CDlgSelectChannel::CDlgSelectChannel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectChannel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectChannel)
	m_nChannel = 0;
	//}}AFX_DATA_INIT
}


void CDlgSelectChannel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectChannel)
	DDX_Control(pDX, IDC_SPIN_CHANNEL, m_SpinBtn);
	DDX_Text(pDX, IDC_EDIT_CHANNEL, m_nChannel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectChannel, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectChannel message handlers

BOOL CDlgSelectChannel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nChannel = g_stScannerInfo.nCurChannel;
	m_SpinBtn.SetRange(0, g_stScannerInfo.nMaxChannel - 1);
	((CEdit*)GetDlgItem(IDC_EDIT_CHANNEL))->SetLimitText(1);

	CenterWindow(CWnd::GetDesktopWindow());
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectChannel::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_nChannel >= g_stScannerInfo.nMaxChannel)
	{
		ShowTestAppErrorInfo(_T("无效通道号!"), this);
		GetDlgItem(IDC_EDIT_CHANNEL)->SetFocus();
		return;
	}
	g_stScannerInfo.nCurChannel = m_nChannel;
	
	CDialog::OnOK();
}
