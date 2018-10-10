#if !defined(AFX_CHECKTOOLFORM_H__FBD93B48_E845_4630_9997_C8FE213620C9__INCLUDED_)
#define AFX_CHECKTOOLFORM_H__FBD93B48_E845_4630_9997_C8FE213620C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckToolForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCheckToolForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCheckToolForm : public CFormView
{
public:
	CCheckToolForm();           // protected constructor used by dynamic creation
protected:
	DECLARE_DYNCREATE(CCheckToolForm)

// Form Data
public:
	//{{AFX_DATA(CCheckToolForm)
	enum { IDD = IDD_CHECKTOOL_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	GF_CHKTOOLSAMPLEDATA	m_stSampleData;

// Operations
public:
	BOOL Gf_CreateView(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void MakeCheckToolSample();	
	void MakeGridSampleData();
	void MakeResolutionData();
	void DrawCheckToolSample();
	void DrawDataInMemDC(CDC* pdc, int nw, int nh);
	void DrawGridMarkSample(CDC* pdc, int nw, int nh);
	void DrawResolutionSample(CDC* pdc, int nw, int nh);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckToolForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCheckToolForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCheckToolForm)
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKTOOLFORM_H__FBD93B48_E845_4630_9997_C8FE213620C9__INCLUDED_)
