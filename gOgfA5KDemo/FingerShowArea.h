#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "SlapShow.h"
#include "FlatShow.h"

// CFingerShowArea �Ի���

class CFingerShowArea : public CDialogEx
{
	DECLARE_DYNAMIC(CFingerShowArea)

public:
	CFingerShowArea(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFingerShowArea();

// �Ի�������
	enum { IDD = IDD_FINGERSHOWAREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTabCtrl _tabImages;

	CSlapShow	_dlgSlap;
	FlatShow	_dlgFlat;
	FlatShow	_dlgRoll;

	CDialog		*_dlgTab[3];
	int			_nCurSelTab;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
