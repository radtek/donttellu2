// DlgCaptImages2Save.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "DlgCaptImages2Save.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCaptImages2Save dialog


CDlgCaptImages2Save::CDlgCaptImages2Save(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCaptImages2Save::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCaptImages2Save)
	m_bSaveCaptImage = FALSE;
	m_nFileType = -1;
	//}}AFX_DATA_INIT
	m_pTestView   = NULL;
	memset(&m_stSaveInfo, 0, sizeof(m_stSaveInfo));
	m_stSaveInfo.nFileType = 1;
	strcpy(m_stSaveInfo.szFileName, "LIMG");
}


void CDlgCaptImages2Save::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCaptImages2Save)
	DDX_Check(pDX, IDC_CHECK_SAVECAPTIMAGE, m_bSaveCaptImage);
	DDX_Radio(pDX, IDC_RADIO_IMGFMTBMP, m_nFileType);
	DDX_Control(pDX, IDC_EDIT_IMAGEFILENAME, m_editImageFileName);
	DDX_Control(pDX, IDC_EDIT_IMAGEFILEPATH, m_editImageFilePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCaptImages2Save, CDialog)
	//{{AFX_MSG_MAP(CDlgCaptImages2Save)
	ON_BN_CLICKED(IDC_BTN_IMAGEFILEPATH, OnBtnImageFilePath)
	ON_BN_CLICKED(IDC_CHECK_SAVECAPTIMAGE, OnCheckSaveCaptImage)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_IMGFMTBMP, IDC_RADIO_IMGFMTRAW, OnRadioImageFormat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCaptImages2Save message handlers

BOOL CDlgCaptImages2Save::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_editImageFilePath.SetLimitText(sizeof(m_stSaveInfo.szFilePath) - 1);
	m_editImageFileName.SetLimitText(sizeof(m_stSaveInfo.szFileName) - 6);
#ifdef UNICODE
	{
		ATL::CA2W pwstr(m_stSaveInfo.szFilePath);
		m_editImageFilePath.SetWindowText((LPWSTR)pwstr);
		ATL::CA2W pwstr2(m_stSaveInfo.szFileName);
		m_editImageFileName.SetWindowText((LPWSTR)pwstr2);
	}
#else
	m_editImageFilePath.SetWindowText(m_stSaveInfo.szFilePath);
	m_editImageFileName.SetWindowText(m_stSaveInfo.szFileName);
#endif
	m_bSaveCaptImage = m_stSaveInfo.bSaveCaptImage ? TRUE : FALSE;
	m_nFileType = m_stSaveInfo.nFileType;

	CenterWindow(CWnd::GetDesktopWindow());
	
	UpdateData(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCaptImages2Save::OnOK() 
{
	// TODO: Add extra validation here
	return;
	
	CDialog::OnOK();
}

void CDlgCaptImages2Save::OnCancel() 
{
	// TODO: Add extra cleanup here
	if(m_bSaveCaptImage)
		return;
	ShowWindow(SW_HIDE);
	
	CDialog::OnCancel();
}

BOOL CDlgCaptImages2Save::InitCaptImages2SaveDlg(CWnd* pParentWnd)
{
	if(!Create(IDD, pParentWnd))
		return FALSE;
	ShowWindow(SW_HIDE);
	return TRUE;
}

void CDlgCaptImages2Save::OnBtnImageFilePath() 
{
	// TODO: Add your control notification handler code here
	//获取文件夹
	BROWSEINFO bi_Info = {0};
	TCHAR  ch_aName[MAX_PATH] = {0}, ch_aPath[MAX_PATH] = {0};
	ITEMIDLIST *p_ListPath = NULL;
    
	bi_Info.hwndOwner = this->m_hWnd;
	bi_Info.iImage = 0;
	bi_Info.lpszTitle = _T("选择图像文件保存路径");
	bi_Info.pidlRoot = NULL;
	bi_Info.pszDisplayName = ch_aName;
	//bi_Info.ulFlags = BIF_USENEWUI;
	p_ListPath = SHBrowseForFolder(&bi_Info); 
    if (p_ListPath)
	{
		SHGetPathFromIDList(p_ListPath, ch_aPath);
		m_editImageFilePath.SetWindowText(ch_aPath);
	}	
}

void CDlgCaptImages2Save::OnCheckSaveCaptImage() 
{
	// TODO: Add your control notification handler code here
	int bSaveInfoSucc = 0;
	TCHAR szErrorInfo[256] = {0};
	CEdit *pEdit = &m_editImageFilePath;
	CString	strTemp;

	if(m_bSaveCaptImage)
	{
		bSaveInfoSucc = 1;
		m_bSaveCaptImage = FALSE;
		m_stSaveInfo.bSaveCaptImage = 0;
		UpdateData(0);
		goto FINISH_EXIT;
	}
	m_bSaveCaptImage = TRUE;
	UpdateData(1);

	memset(m_stSaveInfo.szFilePath, 0, sizeof(m_stSaveInfo.szFilePath));
#ifdef UNICODE
	m_editImageFilePath.GetWindowText(strTemp);
	{
		ATL::CW2A pstr(strTemp.GetString());
		strncpy(m_stSaveInfo.szFilePath, (LPSTR)pstr, sizeof(m_stSaveInfo.szFilePath) - 1);
	}
#else
	m_editImageFilePath.GetWindowText(m_stSaveInfo.szFilePath, sizeof(m_stSaveInfo.szFilePath) - 1);
#endif
	if(!*m_stSaveInfo.szFilePath)
	{
		_tcscpy(szErrorInfo, _T("请设置图像文件的保存路径!"));
		goto FINISH_EXIT;
	}

	memset(m_stSaveInfo.szFileName, 0, sizeof(m_stSaveInfo.szFileName));
#ifdef UNICODE
	m_editImageFileName.GetWindowText(strTemp);
	{
		ATL::CW2A pstr(strTemp.GetString());
		strncpy(m_stSaveInfo.szFileName, (LPSTR)pstr, sizeof(m_stSaveInfo.szFileName) - 1);
	}
#else
	m_editImageFileName.GetWindowText(m_stSaveInfo.szFileName, sizeof(m_stSaveInfo.szFileName) - 1);
#endif
	
	if(!*m_stSaveInfo.szFileName)
	{
		_tcscpy(szErrorInfo, _T("请设置图像文件的前缀名!"));
		pEdit = &m_editImageFileName;
		goto FINISH_EXIT;
	}

	//确保文件夹存在
	if(GflsTest_DirCreateRecur(m_stSaveInfo.szFilePath, 0) < 1)
	{
		_tcscpy(szErrorInfo, _T("无效的图像文件路径!"));
		goto FINISH_EXIT;
	}

	m_stSaveInfo.nFileType = (UCHAR)m_nFileType;
	m_stSaveInfo.nCurSerial = 0;
	m_stSaveInfo.bSaveCaptImage = 1;

	bSaveInfoSucc = 1;

FINISH_EXIT:
	if(!bSaveInfoSucc)
	{
		ShowTestAppErrorInfo(szErrorInfo, this);
		OnCheckSaveCaptImage();
		pEdit->SetFocus();
	}
	GetDlgItem(IDCANCEL)->EnableWindow(!m_bSaveCaptImage);
	m_editImageFilePath.SetReadOnly(m_bSaveCaptImage);
	m_editImageFileName.SetReadOnly(m_bSaveCaptImage);
	GetDlgItem(IDC_RADIO_IMGFMTBMP)->EnableWindow(!m_bSaveCaptImage);
	GetDlgItem(IDC_RADIO_IMGFMTRAW)->EnableWindow(!m_bSaveCaptImage);
	GetDlgItem(IDC_BTN_IMAGEFILEPATH)->EnableWindow(!m_bSaveCaptImage);
}

void CDlgCaptImages2Save::OnRadioImageFormat(UINT nID) 
{
	// TODO: Add your control notification handler code here
	m_nFileType = nID - IDC_RADIO_IMGFMTBMP;
}
