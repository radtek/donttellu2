// GflsScanerTest.h : main header file for the GFLSSCANERTEST application
//

#if !defined(AFX_GFLSSCANERTEST_H__D421CB0B_EA3D_4972_B266_A44C790C8DBC__INCLUDED_)
#define AFX_GFLSSCANERTEST_H__D421CB0B_EA3D_4972_B266_A44C790C8DBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestApp:
// See GflsScanerTest.cpp for the implementation of this class
//

class CMainFrame;
class CGflsScanerTestApp : public CWinApp
{
public:
	CGflsScanerTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGflsScanerTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	GF_256BITMAPINFO	m_stbmpInfo;
	void InitialScannerTestInfo();
	UCHAR	m_bCloseFrame;
	UCHAR	m_bHaveImageInfo;
	int ttt;


    CMainFrame			*m_pFrameWnd;
// Implementation
	//{{AFX_MSG(CGflsScanerTestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBtnGafisTest();
};

extern CGflsScanerTestApp theApp;
extern int reType;


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GFLSSCANERTEST_H__D421CB0B_EA3D_4972_B266_A44C790C8DBC__INCLUDED_)
