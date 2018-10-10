// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// gOgfA5KDemoView.cpp : CgOgfA5KDemoView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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

// CgOgfA5KDemoView ����/����

CgOgfA5KDemoView::CgOgfA5KDemoView()
{
	// TODO:  �ڴ˴���ӹ������

}

CgOgfA5KDemoView::~CgOgfA5KDemoView()
{
}

BOOL CgOgfA5KDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CgOgfA5KDemoView ����

void CgOgfA5KDemoView::OnDraw(CDC* /*pDC*/)
{
	CgOgfA5KDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}

void CgOgfA5KDemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CgOgfA5KDemoView ���

#ifdef _DEBUG
void CgOgfA5KDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CgOgfA5KDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CgOgfA5KDemoDoc* CgOgfA5KDemoView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CgOgfA5KDemoDoc)));
	return (CgOgfA5KDemoDoc*)m_pDocument;
}
#endif //_DEBUG


// CgOgfA5KDemoView ��Ϣ�������


int CgOgfA5KDemoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	if(!_dlgFingerShowArea.Create(IDD_FINGERSHOWAREA, this))
		return -1;

	

	return 0;
}


void CgOgfA5KDemoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
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
	// TODO:  �ڴ����ר�ô����/����û���

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
