#if !defined(AFX_DLGSELECTCHANNEL_H__02A32867_89BD_4F78_93F0_5F629F7BDB35__INCLUDED_)
#define AFX_DLGSELECTCHANNEL_H__02A32867_89BD_4F78_93F0_5F629F7BDB35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectChannel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectChannel dialog

class CDlgSelectChannel : public CDialog
{
// Construction
public:
	CDlgSelectChannel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectChannel)
	enum { IDD = IDD_DIALOG_SELCHANNEL };
	CSpinButtonCtrl	m_SpinBtn;
	int		m_nChannel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectChannel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectChannel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTCHANNEL_H__02A32867_89BD_4F78_93F0_5F629F7BDB35__INCLUDED_)
