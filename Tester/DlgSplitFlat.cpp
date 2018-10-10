// DlgSplitFlat.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSplitFlat.h"


// CDlgSplitFlat dialog

IMPLEMENT_DYNAMIC(CDlgSplitFlat, CDialog)

CDlgSplitFlat::CDlgSplitFlat(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSplitFlat::IDD, pParent)
{
	m_hbkBrush = NULL;
}

CDlgSplitFlat::~CDlgSplitFlat()
{
}

void CDlgSplitFlat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STATIC_SPLITWND, m_staticSplit);
	DDX_Control(pDX, IDC_STATIC_SPLITTIP, m_staticTip);
}


BEGIN_MESSAGE_MAP(CDlgSplitFlat, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgSplitFlat message handlers

BOOL CDlgSplitFlat::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect	rtWnd, rtClient;
	int		nwBorder, nhBorder, ntop;

	GetWindowRect(rtWnd);
	GetClientRect(rtClient);

	nwBorder = rtWnd.Width() - rtClient.Width();
	nhBorder = rtWnd.Height() - rtClient.Height();

	CRect	rtCltr;

	rtCltr.SetRect(DLG_WEDGE, DLG_WEDGE, DLG_WEDGE + SPLITSTATIC_WIDTH, DLG_WEDGE + SPLITSTATIC_HEIGHT);
	m_staticSplit.MoveWindow(rtCltr);

	m_btnOK.GetClientRect(rtWnd);
	rtCltr.top = rtCltr.bottom + DLG_WEDGE * 2;
	rtCltr.bottom = rtCltr.top + rtWnd.Height();
	rtCltr.left = rtCltr.right - rtWnd.Width();
	m_btnOK.MoveWindow(rtCltr);

	ntop = rtCltr.top;
	rtCltr.top = rtCltr.bottom + 2;
	rtCltr.bottom = rtCltr.top + rtWnd.Height();
	m_btnCancel.MoveWindow(rtCltr);

	rtCltr.top = ntop;
	rtCltr.right = rtCltr.left - 2;
	rtCltr.left = DLG_WEDGE;
	m_staticTip.MoveWindow(rtCltr);

	char	szSplitTip[128] = { 0 };

	//GAFISRC_LoadString(IDS_SPLITFLATFINGER_TIP, szSplitTip, sizeof(szSplitTip));
	m_staticTip.SetWindowText(szSplitTip);

	rtWnd.top = 0;
	rtWnd.bottom = rtCltr.bottom + DLG_WEDGE + nhBorder;
	rtWnd.left = 0;
	rtWnd.right = SPLITSTATIC_WIDTH + DLG_WEDGE * 2 + nwBorder;
	MoveWindow(rtWnd);

	CenterWindow(GetParent());

	m_staticSplit.InitSplitFlatPlace();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSplitFlat::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

void CDlgSplitFlat::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	//return;

	CDialog::OnCancel();
}

HBRUSH CDlgSplitFlat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	int		nID = pWnd->GetDlgCtrlID();

	if ( nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkMode(TRANSPARENT);

		if ( (nID == IDC_STATIC_SPLITTIP) && m_hbkBrush ) return m_hbkBrush;
		hbr = (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CDlgSplitFlat::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

BOOL CDlgSplitFlat::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect	rtClient;
	CBrush	brush(CUtilHelper::m_crBackground);

	GetClientRect(rtClient);
	pDC->SetBkMode(TRANSPARENT);

	pDC->FillRect(rtClient, &brush);
	brush.DeleteObject();
	return TRUE;

	return CDialog::OnEraseBkgnd(pDC);
}