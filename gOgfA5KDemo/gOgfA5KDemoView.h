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

// gOgfA5KDemoView.h : CgOgfA5KDemoView ��Ľӿ�
//

#pragma once
#include "FingershowArea.h"

class CgOgfA5KDemoDoc;
class CgOgfA5KDemoView : public CView
{
protected: // �������л�����
	CgOgfA5KDemoView();
	DECLARE_DYNCREATE(CgOgfA5KDemoView)

// ����
public:
	CgOgfA5KDemoDoc* GetDocument() const;

// ����
public:
	CFingerShowArea	_dlgFingerShowArea;

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CgOgfA5KDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // gOgfA5KDemoView.cpp �еĵ��԰汾
inline CgOgfA5KDemoDoc* CgOgfA5KDemoView::GetDocument() const
   { return reinterpret_cast<CgOgfA5KDemoDoc*>(m_pDocument); }
#endif

