// FlatShow.cpp : 实现文件
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"
#include "FlatShow.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// FlatShow 对话框

IMPLEMENT_DYNAMIC(FlatShow, CDialogEx)

FlatShow::FlatShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(FlatShow::IDD, pParent)
{

}

FlatShow::~FlatShow()
{
}

void FlatShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_IMG_FLAT_RTHUMB, _stFinger[CMainFrame::FINGER_R_THUMB - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_RINDEX, _stFinger[CMainFrame::FINGER_R_INDEX - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_RMIDDLE, _stFinger[CMainFrame::FINGER_R_MIDDLE - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_RRING, _stFinger[CMainFrame::FINGER_R_RING - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_RLITTLE, _stFinger[CMainFrame::FINGER_R_LITTLE - 1]);

	DDX_Control(pDX, IDC_IMG_FLAT_LTHUMB, _stFinger[CMainFrame::FINGER_L_THUMB - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_LINDEX, _stFinger[CMainFrame::FINGER_L_INDEX - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_LMIDDLE, _stFinger[CMainFrame::FINGER_L_MIDDLE - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_LRING, _stFinger[CMainFrame::FINGER_L_RING - 1]);
	DDX_Control(pDX, IDC_IMG_FLAT_LLITTLE, _stFinger[CMainFrame::FINGER_L_LITTLE - 1]);
}


BEGIN_MESSAGE_MAP(FlatShow, CDialogEx)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// FlatShow 消息处理程序


void FlatShow::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect	rt;
	GetClientRect(&rt);

	CRect	rtRThumb = rt;
	if (IsWindow(_stFinger[CMainFrame::FINGER_R_THUMB - 1].GetSafeHwnd()))
	{
		rtRThumb.left += 10;
		rtRThumb.top += 10;
		rtRThumb.right = rtRThumb.left + rt.Width() / 5 + rt.Width() % 5 - 20;
		rtRThumb.bottom = rtRThumb.bottom - rt.Height() / 2 - 10;

		_stFinger[CMainFrame::FINGER_R_THUMB - 1].MoveWindow(rtRThumb);
	}

	auto arrangeRect = [](CStatic &stFinger, CRect &rtLast, CRect &rtFinger)
	{
		rtFinger = rtLast;
		if (IsWindow(stFinger.GetSafeHwnd()))
		{
			rtFinger.left = rtLast.right + 20;
			rtFinger.right = rtFinger.left + rtLast.Width();

			stFinger.MoveWindow(rtFinger);
		}
	};

	CRect	rtRIndex;
	arrangeRect(_stFinger[CMainFrame::FINGER_R_INDEX - 1], rtRThumb, rtRIndex);

	CRect	rtRMiddle;
	arrangeRect(_stFinger[CMainFrame::FINGER_R_MIDDLE - 1], rtRIndex, rtRMiddle);

	CRect	rtRRing;
	arrangeRect(_stFinger[CMainFrame::FINGER_R_RING - 1], rtRMiddle, rtRRing);

	CRect	rtRLittle;
	arrangeRect(_stFinger[CMainFrame::FINGER_R_LITTLE - 1], rtRRing, rtRLittle);

	CRect	rtLThumb = rt;
	if (IsWindow(_stFinger[CMainFrame::FINGER_L_THUMB - 1].GetSafeHwnd()))
	{
		rtLThumb.left += 10;
		rtLThumb.top = rtLThumb.top + rt.Height() / 2 + 10;
		rtLThumb.right = rtLThumb.left + rt.Width() / 5 + rt.Width() % 5 - 20;
		rtLThumb.bottom -= 10;

		_stFinger[CMainFrame::FINGER_L_THUMB - 1].MoveWindow(rtLThumb);
	}

	CRect	rtLIndex;
	arrangeRect(_stFinger[CMainFrame::FINGER_L_INDEX - 1], rtLThumb, rtLIndex);

	CRect	rtLMiddle;
	arrangeRect(_stFinger[CMainFrame::FINGER_L_MIDDLE - 1], rtLIndex, rtLMiddle);

	CRect	rtLRing;
	arrangeRect(_stFinger[CMainFrame::FINGER_L_RING - 1], rtLMiddle, rtLRing);

	CRect	rtLLittle;
	arrangeRect(_stFinger[CMainFrame::FINGER_L_LITTLE - 1], rtLRing, rtLLittle);
}


BOOL FlatShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


BOOL FlatShow::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return FALSE;

	//去除闪烁需要双缓冲。。

	return CDialogEx::OnEraseBkgnd(pDC);
}


BOOL FlatShow::PreTranslateMessage(MSG* pMsg)
{
	BOOL	bTrans = false;

	for (auto &stFinger : _stFinger)
	{
		if (IsWindow(stFinger.GetSafeHwnd()))
		{
			bTrans = stFinger.PreTranslateMessage(pMsg);
			if (bTrans) return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}