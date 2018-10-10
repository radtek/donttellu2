#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "SplitStatic.h"
#include "Btnst.h"

// CDlgSplitFlat dialog

class CDlgSplitFlat : public CDialog
{
	DECLARE_DYNAMIC(CDlgSplitFlat)

public:
	CDlgSplitFlat(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSplitFlat();

// Dialog Data
	enum { IDD = IDD_SPLITFLAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	enum { SPLITSTATIC_WIDTH = 640, SPLITSTATIC_HEIGHT = 600, DLG_WEDGE = 5 };

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	CButtonST m_btnOK, m_btnCancel;
	CSplitStatic m_staticSplit;
	CStatic	m_staticTip;

	HBRUSH	m_hbkBrush;

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
