// SlapShow.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"
#include "SlapShow.h"
#include "afxdialogex.h"


// CSlapShow �Ի���

IMPLEMENT_DYNAMIC(CSlapShow, CDialogEx)

CSlapShow::CSlapShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSlapShow::IDD, pParent)
{

}

CSlapShow::~CSlapShow()
{
}

void CSlapShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMG_RFOUR, _stRFour);
	DDX_Control(pDX, IDC_IMG_RLTHUMB, _stRLThumb);
	DDX_Control(pDX, IDC_IMG_LFOUR, _stLFour);
}


BEGIN_MESSAGE_MAP(CSlapShow, CDialogEx)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//ON_STN_DBLCLK(IDC_IMG_RFOUR, &CSlapShow::OnStnDblclickImgRfour)
END_MESSAGE_MAP()


// CSlapShow ��Ϣ�������


void CSlapShow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	CRect	rt;
	GetClientRect(&rt);

	CRect	rtLFour = rt;
	if (IsWindow(_stLFour.GetSafeHwnd()))
	{
		rtLFour.left += 10;
		rtLFour.top += 10;
		rtLFour.right = rtLFour.left + rt.Width() / 3 + rt.Width() % 3 - 20;
		rtLFour.bottom -= 10;

		_stLFour.MoveWindow(rtLFour);
	}
	
	CRect	rtRLThumb = rtLFour;
	if (IsWindow(_stRLThumb.GetSafeHwnd()))
	{
		rtRLThumb.left = rtLFour.right + 20;
		rtRLThumb.right = rtRLThumb.left + rt.Width() / 3 + rt.Width() % 3 - 20;

		_stRLThumb.MoveWindow(rtRLThumb);
	}

	CRect	rtRFour = rtRLThumb;
	if (IsWindow(_stRFour.GetSafeHwnd()))
	{
		rtRFour.left = rtRLThumb.right + 20;
		rtRFour.right = rt.right - 10;

		_stRFour.MoveWindow(rtRFour);
	}

}


BOOL CSlapShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


BOOL CSlapShow::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return FALSE;

	//ȥ����˸��Ҫ˫���塣��

	return CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CSlapShow::PreTranslateMessage(MSG* pMsg)
{
	BOOL	bTrans = false;

	if (IsWindow(_stRFour.GetSafeHwnd()))
	{
		bTrans = _stRFour.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}
	if (IsWindow(_stLFour.GetSafeHwnd()))
	{
		bTrans = _stLFour.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}
	if (IsWindow(_stRLThumb.GetSafeHwnd()))
	{
		bTrans = _stRLThumb.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

//void CSlapShow::OnStnDblclickImgRfour()
//{
//	// TODO:  �ڴ���ӿؼ�֪ͨ����������
//	AfxMessageBox(_T("db button"));
//}
