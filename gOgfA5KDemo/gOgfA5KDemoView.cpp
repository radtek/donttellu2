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

// gOgfA5KDemoView.cpp : CgOgfA5KDemoView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "gOgfA5KDemo.h"
#endif

#include "gOgfA5KDemoDoc.h"
#include "gOgfA5KDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CgOgfA5KDemoView

IMPLEMENT_DYNCREATE(CgOgfA5KDemoView, CView)

BEGIN_MESSAGE_MAP(CgOgfA5KDemoView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CgOgfA5KDemoView 构造/析构

CgOgfA5KDemoView::CgOgfA5KDemoView()
{
	// TODO:  在此处添加构造代码

}

CgOgfA5KDemoView::~CgOgfA5KDemoView()
{
}

BOOL CgOgfA5KDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CgOgfA5KDemoView 绘制

void CgOgfA5KDemoView::OnDraw(CDC* /*pDC*/)
{
	CgOgfA5KDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}

void CgOgfA5KDemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CgOgfA5KDemoView 诊断

#ifdef _DEBUG
void CgOgfA5KDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CgOgfA5KDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CgOgfA5KDemoDoc* CgOgfA5KDemoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CgOgfA5KDemoDoc)));
	return (CgOgfA5KDemoDoc*)m_pDocument;
}
#endif //_DEBUG


// CgOgfA5KDemoView 消息处理程序


int CgOgfA5KDemoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if(!_dlgFingerShowArea.Create(IDD_FINGERSHOWAREA, this))
		return -1;

	

	return 0;
}


void CgOgfA5KDemoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect rect;
	CClientDC clientDC(this);
	OnPrepareDC(&clientDC);

	GetClientRect(rect);
	//rect.SetRect(0, 0, 500, 500);
	clientDC.LPtoDP(rect);

	//CRect	rect;
	//GetClientRect(rect);

	_dlgFingerShowArea.MoveWindow(rect);
	_dlgFingerShowArea.ShowWindow(SW_SHOW);
}


BOOL CgOgfA5KDemoView::PreTranslateMessage(MSG* pMsg)
{
	BOOL	bTrans = false;

	if (IsWindow(_dlgFingerShowArea.GetSafeHwnd()))
	{
		bTrans = _dlgFingerShowArea.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}

	return CView::PreTranslateMessage(pMsg);
}

BOOL CgOgfA5KDemoView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
