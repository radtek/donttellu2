// GflsScanerTestView.h : interface of the CGflsScanerTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GFLSSCANERTESTVIEW_H__F1F8DAB0_74A8_45A8_8287_F91173D1984F__INCLUDED_)
#define AFX_GFLSSCANERTESTVIEW_H__F1F8DAB0_74A8_45A8_8287_F91173D1984F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GflsScanerTestDoc.h"
#include "DlgImageInfo.h"
#include "DlgGrayStatistic.h"
#include "ImageBadptStat.h"
#include "DlgCaptImages2Save.h"
#include "DlgSquareParam.h"

class CMainFrame;

class CGflsScanerTestView : public CScrollView
{
protected: // create from serialization only
	CGflsScanerTestView();
	DECLARE_DYNCREATE(CGflsScanerTestView)

// Attributes
public:
	CGflsScanerTestDoc* GetDocument();

	GF_256BITMAPINFO	m_stbmpInfo;
	GFLSTEST_PARAMINFO	m_stParamInfo;
	GF_IMAGEPARSEINFO	m_stImgParseInfo;
	
	CDlgImageInfo		m_dlgImgInfo;
	CDlgGrayStatistic	m_dlgGrayStat;
	CDlgCaptImages2Save	m_dlgCapt2Save;
	
	BOOL				m_bLBtnDown;

	CImageBadptStat		m_ImageBadptStat;
	CMainFrame			*m_pFrameWnd;

	BOOL              m_bMoveSquare;//!< 是否支持移动自定义长宽的红色矩形框
	BOOL              m_bShowRSquare;//!< 是否显示红色矩形框
	BOOL              m_bShowGSquare;  //!< 是否显示绿色矩形框
	BOOL              m_bShowBSquare;  //!< 是否显示蓝色矩形框

// Operations
public:
	void DrawTestInfo(CDC* pDC);
	void DrawCenterCross(CDC* pdc, int nw, int nh);
	void DrawPointedCross(CDC* pdc, int cx, int cy, int nradio, COLORREF clr);
	void DrawCenterSquare(CDC* pdc, int nw, int nh, int nWidth, COLORREF clr);
	void DrawSelectArea(CDC* pDC);
	void DrawGraduation(CDC * pdc,int nw, int nh);
	void DrawTestRect(CDC* pdc, CRect rt, COLORREF clr, const TCHAR* pszText);
	void InitialBitmapInfo();
	void ClearMallocMemory();
	void CreateAllInfoDlg();
	void ShowAllInfoDlg(UCHAR bShow, UCHAR nDlgType);	// 0x1-m_dlgImgInfo;	0x2--m_dlgGrayStat;	0x4--m_dlgCapt2Save;	0xff--All Dialog
	void CloseAllInfoDlg();
	void SetStdCheckType(UCHAR nStdchkType);
	void ZoomImage2PointedScale(int nScale);
	void GetLineInfoWhenStdChk();
    void ReverseImg();
	void DrawStdChkHelpInfo(CDC *pdc, int nw, int nh);
	void DrawStdChkParseInfo(CDC *pdc, int nw, int nh);
	void DrawStdChkCenterGrayInfo(CDC *pdc, int nw, int nh);
	void drawStdChkGrayVarian(CDC *pdc, int nw, int nh);
	void DrawStdChkAberrance(CDC *pdc, int nw, int nh);
	void DrawStdChkImgResolution(CDC *pdc, int nw, int nh);

	void SetTestViewScrollSizes();

	void InitialScanner();
	void UnInitScanner();
	void StartCaptureImage();
	void ZoomImageWithStyle();
	void SaveCaptImage2File();
	void OnSETPWH();
	void OnStdChkCaptSpeed();
	void OnCaptImages2Save();
    void OnGETSETUPDLG();
	void DoStandardCheck();
	void CheckImageParse(int nSelIdx);
	void CheckCenterImageGray();
	void CheckImageGrayVarian(int nSelIdx);
	void CheckBackImageGray(UCHAR bChkEnvirn);
	void CheckImageBadPoint();
	void OnISISFIN();
	void OnISSIMGQUA();
	void OnISSFINQUA();
	void OnISSIMGEN();
    void OnIMGQUA();
	void OnFINQUA();
	void OnIMGENH();
	void SetAreaPosByCenter(int x, int y, RECT *prtArea, UCHAR bAdjust);
//	void SetImageData2GrayStat(UCHAR* pImgData, int nw, int nh, CRect rtArea);
	void SendStatImageDataMessage(UCHAR bOnlySelArea, UCHAR bFromThread);		// 是否只是统计选中区域的图像信息
	void DoImageData2GrayStat(UCHAR bOnlySelArea);

	void ReSetSquare4Rect(int nwidth, int nheight);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGflsScanerTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGflsScanerTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGflsScanerTestView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnImageData2GrayStat(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnSetSquare();
};


#ifndef _DEBUG  // debug version in GflsScanerTestView.cpp
inline CGflsScanerTestDoc* CGflsScanerTestView::GetDocument()
   { return (CGflsScanerTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GFLSSCANERTESTVIEW_H__F1F8DAB0_74A8_45A8_8287_F91173D1984F__INCLUDED_)
