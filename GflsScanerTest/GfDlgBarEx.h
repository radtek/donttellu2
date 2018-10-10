#if !defined(AFX_GFDLGBAREX_H__72764BC0_0650_4F1B_BF0E_81DABA8D882C__INCLUDED_)
#define AFX_GFDLGBAREX_H__72764BC0_0650_4F1B_BF0E_81DABA8D882C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GfDlgBarEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGfDlgBarEx window

class CGfDlgBarEx : public CDialogBar
{
// Construction
	DECLARE_DYNAMIC(CGfDlgBarEx)
public:
	CGfDlgBarEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGfDlgBarEx)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nStyle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGfDlgBarEx();
	// Generated message map functions
protected:
	//{{AFX_MSG(CGfDlgBarEx)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void DoDataExchange(CDataExchange* pDX);
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GFDLGBAREX_H__72764BC0_0650_4F1B_BF0E_81DABA8D882C__INCLUDED_)
