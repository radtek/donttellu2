#if !defined(AFX_DLGCAPTIMAGES2SAVE_H__5550AB13_6A91_47B7_B7B2_753059584F52__INCLUDED_)
#define AFX_DLGCAPTIMAGES2SAVE_H__5550AB13_6A91_47B7_B7B2_753059584F52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCaptImages2Save.h : header file
//

class CGflsScanerTestView;

/////////////////////////////////////////////////////////////////////////////
// CDlgCaptImages2Save dialog

class CDlgCaptImages2Save : public CDialog
{
// Construction
public:
	CDlgCaptImages2Save(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCaptImages2Save)
	enum { IDD = IDD_DIALOG_CAPTIMAGES2SAVE };
	BOOL	m_bSaveCaptImage;
	int		m_nFileType;
	CEdit	m_editImageFilePath;
	CEdit	m_editImageFileName;
	//}}AFX_DATA

	CGflsScanerTestView*	m_pTestView;
	GF_IMAGESAVEINFO		m_stSaveInfo;

	BOOL InitCaptImages2SaveDlg(CWnd* pParentWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCaptImages2Save)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCaptImages2Save)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnImageFilePath();
	afx_msg void OnCheckSaveCaptImage();
	//}}AFX_MSG
	afx_msg void OnRadioImageFormat(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCAPTIMAGES2SAVE_H__5550AB13_6A91_47B7_B7B2_753059584F52__INCLUDED_)
