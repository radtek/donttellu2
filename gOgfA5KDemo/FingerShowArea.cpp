// FingerShowArea.cpp : 实现文件
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"
#include "FingerShowArea.h"
#include "afxdialogex.h"
//#include "DlgSplitFlat.h"

// CFingerShowArea 对话框

IMPLEMENT_DYNAMIC(CFingerShowArea, CDialogEx)

CFingerShowArea::CFingerShowArea(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFingerShowArea::IDD, pParent)
{
	_nCurSelTab = 0;

	memset(_dlgTab, 0, sizeof(_dlgTab));

	
}

CFingerShowArea::~CFingerShowArea()
{
}

void CFingerShowArea::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2, _tabImages);
}


BEGIN_MESSAGE_MAP(CFingerShowArea, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CFingerShowArea::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CFingerShowArea::OnTcnSelchangeTab)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFingerShowArea 消息处理程序
#define TAB_WIDTH	20
#define	TAB_HEIGHT	30

void CFingerShowArea::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (IsWindow(_tabImages.GetSafeHwnd()))
	{
		CRect	rect;
		GetClientRect(&rect);

		_tabImages.MoveWindow(rect);

		CRect	rt;
		//_tabImages.GetWindowRect(&rt);
		//ScreenToClient(&rt);
		_tabImages.GetClientRect(&rt);
		//ClientToScreen(&rt);

		rt.top += TAB_HEIGHT;
		
		if (IsWindow(_dlgSlap.GetSafeHwnd()))
		{
			_dlgSlap.MoveWindow(rt);
			
		}

		if (IsWindow(_dlgFlat.GetSafeHwnd()))
		{
			_dlgFlat.MoveWindow(rt);
		}

		if (IsWindow(_dlgRoll.GetSafeHwnd()))
		{
			_dlgRoll.MoveWindow(rt);
		}
	}
	
}


BOOL CFingerShowArea::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	_tabImages.InsertItem(0, _T("Slap Finger         "));  //添加参数一选项卡 
	_tabImages.InsertItem(1, _T("Flat Finger         "));  //添加参数二选项卡 
	_tabImages.InsertItem(2, _T("Roll Finger         "));  //添加参数二选项卡 
	//_tabImages.InsertItem(2, _T("结果"));    //添加结果选项卡

	_tabImages.SetItemSize(CSize(20, 30));

	_dlgSlap.Create(IDD_TAB_SLAPSHOW, &_tabImages);
	_dlgFlat.Create(IDD_TAB_FLATSHOW, &_tabImages);
	_dlgRoll.Create(IDD_TAB_FLATSHOW, &_tabImages);
	
	_dlgSlap.ShowWindow(SW_SHOW);
	_dlgFlat.ShowWindow(SW_HIDE);
	_dlgRoll.ShowWindow(SW_HIDE);

	_dlgTab[0] = &_dlgSlap;
	_dlgTab[1] = &_dlgFlat;
	_dlgTab[2] = &_dlgRoll;

	_nCurSelTab = 0;

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}




void CFingerShowArea::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

}


void CFingerShowArea::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	_dlgTab[_nCurSelTab]->ShowWindow(SW_HIDE);

	_nCurSelTab = _tabImages.GetCurSel();

	_dlgTab[_nCurSelTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}


BOOL CFingerShowArea::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return FALSE;

	return CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CFingerShowArea::PreTranslateMessage(MSG* pMsg)
{
	BOOL	bTrans = false;

	if (IsWindow(_dlgSlap.GetSafeHwnd()))
	{
		bTrans = _dlgSlap.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}
	if (IsWindow(_dlgFlat.GetSafeHwnd()))
	{
		bTrans = _dlgFlat.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}
	if (IsWindow(_dlgRoll.GetSafeHwnd()))
	{
		bTrans = _dlgRoll.PreTranslateMessage(pMsg);
		if (bTrans) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

