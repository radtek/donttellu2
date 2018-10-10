#pragma once
#include "afxwin.h"
#include "ImageStatic.h"

// CSlapShow �Ի���

class CSlapShow : public CDialogEx
{
	DECLARE_DYNAMIC(CSlapShow)

public:
	CSlapShow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSlapShow();

// �Ի�������
	enum { IDD = IDD_TAB_SLAPSHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CImageStatic _stRFour;
	CImageStatic _stRLThumb;
	CImageStatic _stLFour;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//afx_msg void OnStnDblclickImgRfour();
};
