#if !defined(AFX_GRABAR_H__1D81DDEA_D1E1_430F_9F13_A9E8B62B5001__INCLUDED_)
#define AFX_GRABAR_H__1D81DDEA_D1E1_430F_9F13_A9E8B62B5001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraBar window

class CGraBar : public CDialogBar
{
// Construction
public:
	CGraBar();

// Attributes
public:
	
BOOL  m_bhor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraBar)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,  UINT nStyle, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraBar)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRABAR_H__1D81DDEA_D1E1_430F_9F13_A9E8B62B5001__INCLUDED_)
