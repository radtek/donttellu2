#if !defined(AFX_DLGCHECKTOOL_H__CDEA7A43_6D4E_46DD_BDE9_4054CFFB295E__INCLUDED_)
#define AFX_DLGCHECKTOOL_H__CDEA7A43_6D4E_46DD_BDE9_4054CFFB295E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCheckTool.h : header file
//

#include "CheckToolForm.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckTool dialog

class CDlgCheckTool : public CDialog
{
// Construction
public:
	CDlgCheckTool(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCheckTool)
	enum { IDD = IDD_DIALOG_CHECKTOOL };
	CStatic	m_staticFormPlace;
	//}}AFX_DATA

	CCheckToolForm		*m_pChkToolForm;
	int					m_nChkToolSample;		// 检测样本类型

	void CreateChkToolForm();
//	void CloseChkToolForm();
//	void ShowChkToolForm(UCHAR bShow);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCheckTool)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCheckTool)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHECKTOOL_H__CDEA7A43_6D4E_46DD_BDE9_4054CFFB295E__INCLUDED_)
