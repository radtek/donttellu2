#ifndef __INCLUDE_H_DLGIMAGESIZE_20121220_16_10_22__
#define __INCLUDE_H_DLGIMAGESIZE_20121220_16_10_22__

#include "resource.h"

#if _MSC_VER > 1000
#pragma once
#endif

class CDlgImageSize : public CDialog
{
public:
	CDlgImageSize(CWnd* pParent = NULL);   // standard constructor
	
	enum { IDD = IDD_DIALOG_IMAGESIZE };


	int	m_nSelType;
	unsigned int m_nWidth;
	unsigned int m_nHeight;

	void GetCurrentImageSize(int& nWidth, int& nHeight);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
// 	virtual void OnOK();
// 	virtual void OnCancel();
	afx_msg void OnRadioImageSize(UINT nID);
	DECLARE_MESSAGE_MAP()
};



#endif	//__INCLUDE_H_DLGIMAGESIZE_20121220_16_10_22__