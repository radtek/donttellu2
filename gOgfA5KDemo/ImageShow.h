#pragma once
#include "afxwin.h"
#include "ImageStatic.h"

// CImageShow 对话框

class CImageShow : public CDialogEx
{
	DECLARE_DYNAMIC(CImageShow)

public:
	CImageShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CImageShow();

// 对话框数据
	enum { IDD = IDD_IMAGESHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CImageStatic _stImage;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
