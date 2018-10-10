#if !defined(AFX_DLGGRAYRAWOPT_H__9C31B6F9_2DEF_489E_95D8_C19BBA439E33__INCLUDED_)
#define AFX_DLGGRAYRAWOPT_H__9C31B6F9_2DEF_489E_95D8_C19BBA439E33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgGrayRawOpt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgGrayRawOpt dialog

class CDlgGrayRawOpt : public CDialog
{
// Construction
public:
	CDlgGrayRawOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgGrayRawOpt)
	enum { IDD = IDD_DIALOG_GRAYRAWOPT };
	CStatic m_staticDescription;
	CStatic m_staticFileSize;
	int m_nWidth;
	int m_nHeight;
	int m_nHeadSize;
	//}}AFX_DATA

	const char*	m_pszFileName;
	__int64		m_nSize;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGrayRawOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgGrayRawOpt)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGRAYRAWOPT_H__9C31B6F9_2DEF_489E_95D8_C19BBA439E33__INCLUDED_)
