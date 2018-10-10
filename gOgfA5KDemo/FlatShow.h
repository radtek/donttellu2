#pragma once

#include "ImageStatic.h"
// FlatShow 对话框

class FlatShow : public CDialogEx
{
	DECLARE_DYNAMIC(FlatShow)

public:
	FlatShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~FlatShow();

// 对话框数据
	enum { IDD = IDD_TAB_FLATSHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CImageStatic	_stFinger[10];

	/*CStatic _stRThumb;
	CStatic _stRIndex;
	CStatic _stRMiddle;
	CStatic _stRRing;
	CStatic _stRLittle;
	
	CStatic _stLThumb;
	CStatic _stLIndex;
	CStatic _stLMiddle;
	CStatic _stLRing;
	CStatic _stLLittle;*/

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
