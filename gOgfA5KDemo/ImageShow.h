#pragma once
#include "afxwin.h"
#include "ImageStatic.h"

// CImageShow �Ի���

class CImageShow : public CDialogEx
{
	DECLARE_DYNAMIC(CImageShow)

public:
	CImageShow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CImageShow();

// �Ի�������
	enum { IDD = IDD_IMAGESHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CImageStatic _stImage;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
