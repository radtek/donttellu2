#if !defined(AFX_DLGGRAYSTATISTIC_H__BC1954A6_6008_4F5D_825D_C81986A076EE__INCLUDED_)
#define AFX_DLGGRAYSTATISTIC_H__BC1954A6_6008_4F5D_825D_C81986A076EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgGrayStatistic.h : header file
//

#include "StaticGrayValue.h"

class CGflsScanerTestView;

/////////////////////////////////////////////////////////////////////////////
// CDlgGrayStatistic dialog

class CDlgGrayStatistic : public CDialog
{
// Construction
public:
	CDlgGrayStatistic(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgGrayStatistic)
	enum { IDD = IDD_DIALOG_GRAYSTAT };
	CStaticGrayValue	m_staticGrayValue;
	CString	m_strAverage;
	CString	m_strCntInRange;
	CString	m_strDiffValue;
	CString	m_strMedian;
	CString	m_strPercent;
	CString	m_strPixelCount;
	CString	m_strRange;
	//}}AFX_DATA

	CGflsScanerTestView*	m_pTestView;
	BOOL CreateGrayStatDlg(CWnd* pParentWnd, CRect &rtFrame);
	void SetImageStatInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGrayStatistic)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgGrayStatistic)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnSetImageStatInfo(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGRAYSTATISTIC_H__BC1954A6_6008_4F5D_825D_C81986A076EE__INCLUDED_)
