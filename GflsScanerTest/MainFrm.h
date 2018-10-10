// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__B3778699_75B0_4CBD_A474_72BDF95FD8CC__INCLUDED_)
#define AFX_MAINFRM_H__B3778699_75B0_4CBD_A474_72BDF95FD8CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GfDlgBarEx.h"
#include "GraBar.h"
#include "SetSerialNoDlg.h"

class CGflsScanerTestView;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CGflsScanerTestView	*m_pTestView;
	CSetSerialNoDlg * m_pSetSerailNoDlg;

// Operations
public:
	void ShowCurMousePosInfo(CPoint &pt, int nw, int nh, UCHAR bOnImage);
	void ShowCurSelectAreaInfo(LPPOINT pptLeftTop, LPPOINT pptRightBottom, int nOption = 0) ;
	void SetBrightContrastValue();
	int  ChangeScannerBrightContrast(int nNewValue, UCHAR bBright);
#ifdef UNSTD_BLACK_BALANCE
	int  ChangeScannerBlackBalance(int nNewValue);
#endif
//	void showImageZoomInfo();
	void ZoomImage(UCHAR bEnlarge);
	void ReverseImage(int nstyle);
	void CalculateAberParam(LPPOINT ppoint1, LPPOINT ppoint2);
	inline int  PosIsCenterNear(LPPOINT pptCenter, LPPOINT pptPos, int nDist);
	int  PointInAberPosValue(LPPOINT ppt);
	void ClearImageAberParamInfo();

	void ShowDialogBarWithStdChkType(UCHAR nStdChkType);
	void StartOrStopStdCheck(UCHAR nStdChkType, const TCHAR* pszCheckType);

	void UpdateCaptureBtnStateAfterStopCapt();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
public:
	CStatusBar  m_wndStatusBar;
//	CToolBar    m_wndToolBar;
//	CDialogBar  m_wndDlgBar;
	CGfDlgBarEx	m_wndDlgBar;
	CDialogBar	m_wndAberDlbBar;
	CDialogBar	m_wndStdChkBar;
    CGraBar     m_wndGraxBar;
	CGraBar     m_wndGrayBar;
	int         m_nPhyDistance;
	CString		m_strSerialNum;

	int			m_nSizeSelType;
	int			m_nSizeWidth;
	int			m_nSizeHeight;
	
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCheckStartcapture();
	afx_msg void OnBtnSave2file();
	afx_msg void OnBtnFromfile();
	afx_msg void OnImageZoomIn();
	afx_msg void OnImageZoomOut();
	afx_msg void OnImageReverse();
	afx_msg void OnCheckOnlyShowImg();
	afx_msg void OnCheckInsertZoom();
	afx_msg void OnCheckShowBadPoint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBtnCalcImgAber();
	afx_msg void OnClose();
	afx_msg void OnInitialLiveCapture();
	afx_msg void OnInitialLiveCapture1();
	afx_msg void OnUpdateInitLiveCapture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInitLiveCapture1(CCmdUI* pCmdUI);
	afx_msg void OnSelectScannerChannel();
	afx_msg void OnUpdateScannerChannel(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnStdCheckHandler(UINT nID);
	afx_msg void OnStdCheck2Handler(UINT nID);
	afx_msg void OnUpdateStdChkHandler(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDlgBarCtrls(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageReverse(CCmdUI* pCmdUI);
	afx_msg void OnMosaicImageHandler(UINT nID);
	afx_msg void OnUpdateMosaicImageHandler(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCheckHandler(CCmdUI* pCmdUI);
	afx_msg void OnCheckHandler(UINT nID);
	afx_msg void OnUpdateCheck2Handler(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnPhydistance();
	afx_msg void OnBtnGafisTest();
	afx_msg void OnBtnSetSerialNumClick();
	afx_msg void OnBtnGetSerialNumClick();
	afx_msg void OnUpdateDlgBarSerialNumCtrls(CCmdUI* pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__B3778699_75B0_4CBD_A474_72BDF95FD8CC__INCLUDED_)
