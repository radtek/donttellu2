#pragma once
#include "afxwin.h"
#include "ImageStatic.h"

// CSlapShow 对话框

class CSlapShow : public CDialogEx
{
	DECLARE_DYNAMIC(CSlapShow)

public:
	CSlapShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSlapShow();

// 对话框数据
	enum { IDD = IDD_TAB_SLAPSHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
