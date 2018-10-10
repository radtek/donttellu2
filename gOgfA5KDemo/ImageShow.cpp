// ImageShow.cpp : 实现文件
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"
#include "ImageShow.h"
#include "afxdialogex.h"


// CImageShow 对话框

IMPLEMENT_DYNAMIC(CImageShow, CDialogEx)

CImageShow::CImageShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageShow::IDD, pParent)
{

}

CImageShow::~CImageShow()
{
}

void CImageShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, _stImage);
}


BEGIN_MESSAGE_MAP(CImageShow, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CImageShow 消息处理程序
BOOL CImageShow::PreTranslateMessage(MSG* pMsg)
{
	BOOL	bTrans = false;

	if (IsWindow(_stImage.GetSafeHwnd()))
	{
		bTrans = _stImage.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CImageShow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (IsWindow(_stImage.GetSafeHwnd()))
	{
		CRect	rect;
		GetClientRect(&rect);

		_stImage.MoveWindow(rect);
	}
}


BOOL CImageShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//得到显示器大小
	int   cx, cy;
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);
	//再用MoveWindow
	CRect   rcTemp;
	rcTemp.BottomRight() = CPoint(cx, cy);
	rcTemp.TopLeft() = CPoint(0, 0);
	

	if (_stImage._pbmInfo == NULL)
		MoveWindow(0, 0, 1600, 1500, 1);
	else
	{
		int nWidth = _stImage._pbmInfo->bmiHeader.biWidth;
		int nHeight = abs(_stImage._pbmInfo->bmiHeader.biHeight);

		//MoveWindow(0, 0, nWidth, nHeight, 1);
		CRect	rect;
		rect.left = abs(nWidth - rcTemp.Width()) / 2;
		rect.right = rect.left + nWidth;
		rect.top = abs(nHeight - rcTemp.Height()) / 2;
		rect.bottom = rect.top + nHeight;

		MoveWindow(rect);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
