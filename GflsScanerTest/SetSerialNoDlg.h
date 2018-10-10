#pragma once


// CSetSerialNoDlg dialog

class CSetSerialNoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSerialNoDlg)

public:
	CSetSerialNoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetSerialNoDlg();

	CEdit m_SerialNoButton;
	unsigned int m_nSerialNo;

// Dialog Data
	enum { IDD = IDD_DIALOG_SETSERIALNO };

	void OnOK();
	void OnCancel();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
