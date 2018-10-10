// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// gOgfA5KDemoView.h : CgOgfA5KDemoView 类的接口
//

#pragma once
#include "FingershowArea.h"

class CgOgfA5KDemoDoc;
class CgOgfA5KDemoView : public CView
{
protected: // 仅从序列化创建
	CgOgfA5KDemoView();
	DECLARE_DYNCREATE(CgOgfA5KDemoView)

// 特性
public:
	CgOgfA5KDemoDoc* GetDocument() const;

// 操作
public:
	CFingerShowArea	_dlgFingerShowArea;

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CgOgfA5KDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

#ifndef _DEBUG  // gOgfA5KDemoView.cpp 中的调试版本
inline CgOgfA5KDemoDoc* CgOgfA5KDemoView::GetDocument() const
   { return reinterpret_cast<CgOgfA5KDemoDoc*>(m_pDocument); }
#endif

