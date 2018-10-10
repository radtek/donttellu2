// SetSerialNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "SetSerialNoDlg.h"
#include "afxdialogex.h"


// CSetSerialNoDlg dialog

IMPLEMENT_DYNAMIC(CSetSerialNoDlg, CDialogEx)

CSetSerialNoDlg::CSetSerialNoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetSerialNoDlg::IDD, pParent)
{
	m_nSerialNo = 0;
}

CSetSerialNoDlg::~CSetSerialNoDlg()
{
}

void CSetSerialNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERIALNO, m_nSerialNo);
}


BEGIN_MESSAGE_MAP(CSetSerialNoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetSerialNoDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CSetSerialNoDlg::OnOK()
{
	UpdateData(TRUE);
	return CDialogEx::OnOK();
}

void CSetSerialNoDlg::OnCancel()
{
	return CDialogEx::OnCancel();
}
// CSetSerialNoDlg message handlers


void CSetSerialNoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
