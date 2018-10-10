// OGF_usbtst.h : main header file for the OGF_USBTST application
//

#if !defined(AFX_OGF_USBTST_H__FDEBDCB4_0904_4947_9C53_2992256635A9__INCLUDED_)
#define AFX_OGF_USBTST_H__FDEBDCB4_0904_4947_9C53_2992256635A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COGF_usbtstApp:
// See OGF_usbtst.cpp for the implementation of this class
//
typedef struct dispar
{
	HWND	MyWnd;
	CDC		*MyCdc;
    CDialog *MyDlg;
	unsigned char *Imagedata;
} mydispar, *pmydispar;

class COGF_usbtstApp : public CWinApp
{
public:
	COGF_usbtstApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGF_usbtstApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COGF_usbtstApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OGF_USBTST_H__FDEBDCB4_0904_4947_9C53_2992256635A9__INCLUDED_)
