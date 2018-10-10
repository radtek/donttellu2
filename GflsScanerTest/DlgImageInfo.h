#if !defined(AFX_DLGIMAGEINFO_H__68834472_B2A0_4010_ADC2_A33FEC70E520__INCLUDED_)
#define AFX_DLGIMAGEINFO_H__68834472_B2A0_4010_ADC2_A33FEC70E520__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImageInfo.h : header file
//

#include "StaticGrayValue.h"
class CGflsScanerTestView;

/////////////////////////////////////////////////////////////////////////////
// CDlgImageInfo dialog

class CDlgImageInfo : public CDialog
{
// Construction
public:
	CDlgImageInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImageInfo)
	enum { IDD = IDD_DIALOG_IMAGEINFO };
	CStatic	m_staticStdChkType;
	CEdit	m_editStdChkRole;
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
	UCHAR*					m_pCaptBuffer;
	GF_CAPTSPEEDINFO		m_stCaptSpeed;

	BOOL InitImageInfoDlg(CWnd* pParentWnd, CRect &rtFrame);
	void InitCtrlWndStateAndValue();
	void showImageZoomInfo();
	void SetImageStatInfo();
	void SetStdChkInformation(UCHAR nStdChkType);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImageInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImageInfo)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnSetImageStatInfo(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMAGEINFO_H__68834472_B2A0_4010_ADC2_A33FEC70E520__INCLUDED_)
