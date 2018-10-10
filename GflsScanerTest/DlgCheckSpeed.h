#if !defined(AFX_DLGCHECKSPEED_H__8FEC86B7_E2F4_4B12_A81D_1C6DC325352C__INCLUDED_)
#define AFX_DLGCHECKSPEED_H__8FEC86B7_E2F4_4B12_A81D_1C6DC325352C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCheckSpeed.h : header file
//

class CGflsScanerTestView;

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckSpeed dialog

class CDlgCheckSpeed : public CDialog
{
// Construction
public:
	CDlgCheckSpeed(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCheckSpeed)
	enum { IDD = IDD_STDCHK_CAPTSPEED };
	CProgressCtrl	m_CaptureProgress;
	CEdit	m_editCaptFrmCnt;
	CEdit	m_editCaptUseTime;
	CEdit	m_editCaptSpeed;
	//}}AFX_DATA

	CGflsScanerTestView		*m_pTestView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCheckSpeed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCheckSpeed)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnCapturespeed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHECKSPEED_H__8FEC86B7_E2F4_4B12_A81D_1C6DC325352C__INCLUDED_)
