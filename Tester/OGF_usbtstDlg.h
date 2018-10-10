// OGF_usbtstDlg.h : header file
//

#include "afxwin.h"
#include "afxcmn.h"
#if !defined(AFX_OGF_USBTSTDLG_H__86B7BACF_442D_4702_992F_D28B1E3E3C32__INCLUDED_)
#define AFX_OGF_USBTSTDLG_H__86B7BACF_442D_4702_992F_D28B1E3E3C32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COGF_usbtstDlg dialog

class COGF_usbtstDlg : public CDialog
{
// Construction
public:
	COGF_usbtstDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(COGF_usbtstDlg)
	enum { IDD = IDD_OGF_USBTST_DIALOG };
	CButton	m_CheckReverse;
	CComboBox	m_cbWindowSize;
	CEdit	m_edtExp;
	CEdit	m_edtGain;
	CSliderCtrl	m_SliderExp;
	CSliderCtrl	m_SliderGain;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGF_usbtstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COGF_usbtstDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnCapture();
	afx_msg void OnSave();
	virtual void OnCancel();
	afx_msg void OnOpen();
	afx_msg void OnCled();
	afx_msg void OnPause();
	afx_msg void OnSetreg();
	afx_msg void OnReleasedcaptureSliderGain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderExp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboWindowsize();
	afx_msg void OnCheckReverse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnStnClickedView();
	CComboBox m_cbCamList;
	afx_msg void OnCbnSelchangeComboCamlist();
	afx_msg void OnBnClickedGetreg();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckNormal();
	CButton m_CheckNormal;
	CSliderCtrl m_SliderLed;
	CEdit m_edtLed;
	afx_msg void OnNMReleasedcaptureSliderLed(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_CheckLed1;
	CButton m_CheckLed2;
	CButton m_CheckLed3;
	CButton m_CheckLed4;
	CButton m_CheckLed5;
	CButton m_CheckLed6;
	CButton m_CheckLed7;
	CButton m_CheckLed8;
	CButton m_CheckLed9;
	CButton m_CheckLed10;
	CButton m_CheckLed11;
	afx_msg void OnBnClickedLed1();
	afx_msg void OnBnClickedLed2();
	afx_msg void OnBnClickedLed3();
	afx_msg void OnBnClickedLed4();
	afx_msg void OnBnClickedLed5();
	afx_msg void OnBnClickedLed6();
	afx_msg void OnBnClickedLed7();
	afx_msg void OnBnClickedLed8();

	void SaveConfigFile() ; 
	BOOL LoadConfigFile() ; 
	afx_msg void OnBnClickedLed9();
	afx_msg void OnBnClickedLed10();
	afx_msg void OnBnClickedLed11();
	afx_msg void OnBnClickedCut();
	int _nCaptureType;
	CComboBox _combCaptureType;
	afx_msg void OnCbnSelchangeComboCapturetype();
	CButton _btnCut;

	void CutImage();
	enum
	{
		CAPTURE_TYPE_FLATFINGER = 0,
		CAPTURE_TYPE_RFOUR = 1,
		CAPTURE_TYPE_LFOUR = 2,
		CAPTURE_TYPE_RLTHUMB = 3,
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OGF_USBTSTDLG_H__86B7BACF_442D_4702_992F_D28B1E3E3C32__INCLUDED_)
