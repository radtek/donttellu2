// GfDlgBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "GfDlgBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGfDlgBarEx
IMPLEMENT_DYNAMIC(CGfDlgBarEx, CDialogBar)
CGfDlgBarEx::CGfDlgBarEx()
{
}

CGfDlgBarEx::~CGfDlgBarEx()
{
}


void CGfDlgBarEx::DoDataExchange(CDataExchange* pDX)
{
	// TODO: Add your specialized code here and/or call the base class
	//DDX_Text(pDX, IDC_EDIT_IMGHEIGHT, m_nHeight);
	CDialogBar::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGfDlgBarEx, CDialogBar)
	//{{AFX_MSG_MAP(CGfDlgBarEx)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGfDlgBarEx message handlers

BOOL CGfDlgBarEx::Create(CWnd* pParentWnd, UINT nStyle) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialogBar::Create (pParentWnd, IDD_DIALOG_TOPBAR, nStyle, IDD_DIALOG_TOPBAR);
	
//	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CGfDlgBarEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	GetOwner()->SendMessage(WM_HSCROLL, (WPARAM)MAKELONG((WORD)nSBCode,(WORD)nPos), (LPARAM)pScrollBar->m_hWnd);

	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}


