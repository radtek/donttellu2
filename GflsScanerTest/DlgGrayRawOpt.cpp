// DlgGrayRawOpt.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "DlgGrayRawOpt.h"
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGrayRawOpt dialog


CDlgGrayRawOpt::CDlgGrayRawOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGrayRawOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGrayRawOpt)
	m_nWidth  = 0;
	m_nHeight = 0;
	m_nHeadSize = 0;
	//}}AFX_DATA_INIT
	m_pszFileName = NULL;
	m_nSize = 0;
}


void CDlgGrayRawOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGrayRawOpt)
	DDX_Control(pDX, IDC_STATIC_DESC, m_staticDescription);
	DDX_Control(pDX, IDC_STATIC_FILESIZE, m_staticFileSize);
	DDX_Text(pDX, IDC_EDIT_IMGWIDTH, m_nWidth);
	DDX_Text(pDX, IDC_EDIT_IMGHEIGHT, m_nHeight);
	DDX_Text(pDX, IDC_EDIT_IMAGEHEAD, m_nHeadSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGrayRawOpt, CDialog)
	//{{AFX_MSG_MAP(CDlgGrayRawOpt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGrayRawOpt message handlers

BOOL CDlgGrayRawOpt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ASSERT(m_pszFileName);

	struct _stati64 buf;
	_stati64(m_pszFileName, &buf);
	CString strTitle, strSize;

	strTitle.Format(_T("描述\"%s\":"), m_pszFileName);
	strSize.Format(_T("(%I64u)字节"), buf.st_size);
	m_staticDescription.SetWindowText(strTitle);
	m_staticFileSize.SetWindowText(strSize);

	m_nSize = buf.st_size;

	m_nWidth  = (int)sqrt((double)m_nSize);
	m_nHeight = (int)(m_nSize / m_nWidth);
	m_nHeadSize = (int)(m_nSize - m_nWidth * m_nHeight);

	UpdateData(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGrayRawOpt::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(1);

	__int64 nSize = 1;

	nSize = (m_nWidth * m_nHeight);
	nSize += m_nHeadSize;

	CString strTitle = "";
	GetWindowText(strTitle);
	if(nSize > m_nSize)
	{
		MessageBox(_T("指定的图像比文件大!"), strTitle, MB_OK);
		return;
	}
	else if(nSize < m_nSize)
	{
		int nret = MessageBox(_T("指定的图像比文件小，是否继续？"), strTitle, MB_YESNO);
		if(nret != IDYES)
			return;
	}
	
	CDialog::OnOK();
}
