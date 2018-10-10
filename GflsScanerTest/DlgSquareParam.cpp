// DlgSquareParam.cpp : implementation file
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "DlgSquareParam.h"
#include "afxdialogex.h"


// CDlgSquareParam dialog

IMPLEMENT_DYNAMIC(CDlgSquareParam, CDialogEx)

CDlgSquareParam::CDlgSquareParam(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSquareParam::IDD, pParent)
{
		m_nWS4 =  200;
		m_nWS1 =  300;
		m_bMoveSquare = TRUE;
		m_bShowRSquare = TRUE;
		m_bShowGSquare = FALSE;
		m_bShowBSquare = FALSE;

}

CDlgSquareParam::~CDlgSquareParam()
{
}

void CDlgSquareParam::SetParamInfo(GFLSTEST_PARAMINFO *pParmaInfo, 
	                  BOOL bMoveSquare,
					  BOOL bShowRSquare,
					  BOOL bShowGSquare,
					  BOOL bShowBSquare)
{
	m_nWS4 = pParmaInfo->nSquare4;
	m_nWS1 = pParmaInfo->nSquare1;
	m_bMoveSquare = bMoveSquare;
	m_bShowRSquare = bShowRSquare;
	m_bShowGSquare = bShowGSquare;
	m_bShowBSquare = bShowBSquare;

}

void CDlgSquareParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SQUAREWIDTH, m_nWS4);
	DDX_Text(pDX, IDC_EDIT_SQUAREHEIGHT, m_nWS1);
	DDX_Check(pDX, IDC_CHECK_MOVESSQUARE, m_bMoveSquare);
	DDX_Check(pDX, IDC_CHECK_SHOWREDSQUARE, m_bShowRSquare);
	DDX_Check(pDX, IDC_CHECK_SHOWGREENSQUARE, m_bShowGSquare);	
	DDX_Check(pDX, IDC_CHECK_SHOWBLUESQUARE, m_bShowBSquare);	
}


BEGIN_MESSAGE_MAP(CDlgSquareParam, CDialogEx)
END_MESSAGE_MAP()


// CDlgSquareParam message handlers
