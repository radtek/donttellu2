
#include "stdafx.h"
#include "DlgImageSize.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDlgImageSize::CDlgImageSize(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImageSize::IDD, pParent)
{
	m_nHeight = 640;
	m_nWidth = 640;
	m_nSelType = 0;
}

void CDlgImageSize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_SIZE_640_640, m_nSelType);
	DDX_Text(pDX, IDC_EDIT_SIZE_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_EDIT_SIZE_LENGTH, m_nHeight);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgImageSize, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_SIZE_640_640, IDC_RADIO_SIZE_CONFIG, OnRadioImageSize)
END_MESSAGE_MAP()


BOOL CDlgImageSize::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_nSelType != IDC_RADIO_SIZE_CONFIG - IDC_RADIO_SIZE_640_640)
	{
		GetDlgItem(IDC_EDIT_SIZE_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SIZE_LENGTH)->EnableWindow(FALSE);
	}
	CenterWindow(CWnd::GetDesktopWindow());
	UpdateData(0);
	return TRUE;
}

void CDlgImageSize::GetCurrentImageSize(int& nWidth, int& nHeight)
{
	if(m_nSelType == 0)
	{
		nWidth = nHeight = 640;
	}
	else if(m_nSelType == 1)
	{
		nWidth = nHeight = 440;
	}
	else if(m_nSelType == 2)
	{
		 nWidth = 256;
		 nHeight = 360;
	}
	else
	{
		nWidth = m_nWidth;
		nHeight = m_nHeight;
	}
}

void CDlgImageSize::OnRadioImageSize(UINT nID)
{
	m_nSelType = nID - IDC_RADIO_SIZE_640_640;
	if(nID == IDC_RADIO_SIZE_CONFIG)
	{
		GetDlgItem(IDC_EDIT_SIZE_WIDTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SIZE_LENGTH)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_SIZE_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SIZE_LENGTH)->EnableWindow(FALSE);
	}
}