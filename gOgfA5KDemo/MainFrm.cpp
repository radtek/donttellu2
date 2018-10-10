// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"

#include "MainFrm.h"
#include "gOgfA5KDemoView.h"
#include "CaptureDlg.h"
#include "../infissplitbox/infissplitboxdef.h"
#include "CaptureTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int WriteLoadIni(CMainFrame *frame)
{
	std::string	strPath = "D:\\gOgfA5KDemo.ini";

	char fingergain[32] = { 0 };
	char fingerexposure[32] = { 0 };
	char slapgain[32] = { 0 };
	char slapexposure[32] = { 0 };

	WritePrivateProfileStringA("gOgfA5KDemo", "fingergain", _itoa(frame->_nFingerGain, fingergain, 10), strPath.c_str());//0表示失败，非0表示成功
	WritePrivateProfileStringA("gOgfA5KDemo", "fingerexposure", _itoa(frame->_nFingerExposure, fingerexposure, 10), strPath.c_str());//0表示失败，非0表示成功
	WritePrivateProfileStringA("gOgfA5KDemo", "slapgain", _itoa(frame->_nSlapGain, slapgain, 10), strPath.c_str());//0表示失败，非0表示成功
	WritePrivateProfileStringA("gOgfA5KDemo", "slapexposure", _itoa(frame->_nSlapExposure, slapexposure, 10), strPath.c_str());//0表示失败，非0表示成功

	return 0;
}

int ReadLoadIni(CMainFrame *frame)
{
	std::string	strPath = "D:\\gOgfA5KDemo.ini";

	frame->_nFingerGain = GetPrivateProfileIntA("gOgfA5KDemo", "fingergain", FINGER_GAIN, strPath.c_str());
	frame->_nFingerExposure = GetPrivateProfileIntA("gOgfA5KDemo", "fingerexposure", FINGER_EXPOSURE, strPath.c_str());
	frame->_nSlapGain = GetPrivateProfileIntA("gOgfA5KDemo", "slapgain", SLAP_GAIN, strPath.c_str());
	frame->_nSlapExposure = GetPrivateProfileIntA("gOgfA5KDemo", "slapexposure", SLAP_EXPOSURE, strPath.c_str());

	return 0;
}

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	//ON_WM_GETMINMAXINFO()
	//ON_WM_SYSCOMMAND()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_CHK_FlatFinger, &CMainFrame::OnChkFlatfinger)
	ON_UPDATE_COMMAND_UI(ID_CHK_FlatFinger, &CMainFrame::OnUpdateChkFlatfinger)
	ON_COMMAND(ID_CHK_SLAP, &CMainFrame::OnChkSlap)
	ON_UPDATE_COMMAND_UI(ID_CHK_SLAP, &CMainFrame::OnUpdateChkSlap)
	ON_COMMAND(ID_CHK_ROLLFINGER, &CMainFrame::OnChkRollfinger)
	ON_UPDATE_COMMAND_UI(ID_CHK_ROLLFINGER, &CMainFrame::OnUpdateChkRollfinger)
	ON_COMMAND(ID_CHK_PALM, &CMainFrame::OnChkPalm)
	ON_UPDATE_COMMAND_UI(ID_CHK_PALM, &CMainFrame::OnUpdateChkPalm)
	ON_COMMAND(ID_BTN_START, &CMainFrame::OnBtnStart)
	ON_UPDATE_COMMAND_UI(ID_BTN_START, &CMainFrame::OnUpdateBtnStart)

	ON_COMMAND(ID_CHK_R_THUMB, &CMainFrame::OnChkR1)
	ON_UPDATE_COMMAND_UI(ID_CHK_R_THUMB, &CMainFrame::OnUpdateChkR1)
	ON_COMMAND(ID_CHK_R_INDEX, &CMainFrame::OnChkR2)
	ON_UPDATE_COMMAND_UI(ID_CHK_R_INDEX, &CMainFrame::OnUpdateChkR2)
	ON_COMMAND(ID_CHK_R_MIDDLE, &CMainFrame::OnChkR3)
	ON_UPDATE_COMMAND_UI(ID_CHK_R_MIDDLE, &CMainFrame::OnUpdateChkR3)
	ON_COMMAND(ID_CHK_R_RING, &CMainFrame::OnChkR4)
	ON_UPDATE_COMMAND_UI(ID_CHK_R_RING, &CMainFrame::OnUpdateChkR4)
	ON_COMMAND(ID_CHK_R_LITTLE, &CMainFrame::OnChkR5)
	ON_UPDATE_COMMAND_UI(ID_CHK_R_LITTLE, &CMainFrame::OnUpdateChkR5)

	ON_COMMAND(ID_CHK_L_THUMB, &CMainFrame::OnChkL1)
	ON_UPDATE_COMMAND_UI(ID_CHK_L_THUMB, &CMainFrame::OnUpdateChkL1)
	ON_COMMAND(ID_CHK_L_INDEX, &CMainFrame::OnChkL2)
	ON_UPDATE_COMMAND_UI(ID_CHK_L_INDEX, &CMainFrame::OnUpdateChkL2)
	ON_COMMAND(ID_CHK_L_MIDDLE, &CMainFrame::OnChkL3)
	ON_UPDATE_COMMAND_UI(ID_CHK_L_MIDDLE, &CMainFrame::OnUpdateChkL3)
	ON_COMMAND(ID_CHK_L_RING, &CMainFrame::OnChkL4)
	ON_UPDATE_COMMAND_UI(ID_CHK_L_RING, &CMainFrame::OnUpdateChkL4)
	ON_COMMAND(ID_CHK_L_LITTLE, &CMainFrame::OnChkL5)
	ON_UPDATE_COMMAND_UI(ID_CHK_L_LITTLE, &CMainFrame::OnUpdateChkL5)
	ON_COMMAND(ID_BTN_EXPORTBMP, &CMainFrame::OnBtnExportbmp)
	ON_COMMAND(ID_BTN_EXPORTWSQ, &CMainFrame::OnBtnExport2WSQ)
	ON_COMMAND(ID_BTN_TESTSLAP, &CMainFrame::OnBtnTestslap)
	ON_COMMAND(ID_BTN_TESTFINGER, &CMainFrame::OnBtnTestfinger)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO:  在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
	m_bIsFullScreen = FALSE;

	_bChkSlap = TRUE;
	_bChkFlat = FALSE;
	_bChkRoll = TRUE;
	_bChkPalm = FALSE;

	_bEnableSlap = TRUE;
	_bEnableFlat = FALSE;
	_bEnableRoll = TRUE;
	_bEnablePalm = FALSE;

	_bEnableStart = TRUE;

	memset(_bChkFinger, TRUE, sizeof(_bChkFinger));
	memset(_bEnableFinger, TRUE, sizeof(_bEnableFinger));
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	//SetMenu(NULL);
	//ToggleFullScreenMode();
	//ModifyStyle(WS_CAPTION, 0, SWP_FRAMECHANGED);
	ReadLoadIni(this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	/*cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_SYSMENU;*/
	//cs.style &= ~FWS_ADDTOTITLE;
	//cs.style &= ~WS_THICKFRAME;

	/*_bChkSlap = TRUE;
	_bChkFlat = TRUE;
	_bChkRoll = TRUE;
	_bChkPalm = TRUE;

	_bEnableSlap = TRUE;
	_bEnableFlat = TRUE;
	_bEnableRoll = TRUE;
	_bEnablePalm = TRUE;

	_bEnableStart = TRUE;*/

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

int	GfHelp_GetSystemTaskBarInfo(int *pnState, RECT *prect, int *pnEdge, int *pnhTaskbar)
{
	APPBARDATA	stabData = { 0 };
	int		nretval;
	BOOL	bretval;

	stabData.cbSize = sizeof(stabData);
	nretval = (int)SHAppBarMessage(ABM_GETSTATE, &stabData);

	if (pnState) *pnState = nretval;

	memset(&stabData, 0, sizeof(stabData));
	stabData.cbSize = sizeof(stabData);

	bretval = (BOOL)SHAppBarMessage(ABM_GETTASKBARPOS, &stabData);
	if (!bretval)
	{
		stabData.rc.bottom = 40;
		stabData.uEdge = ABE_BOTTOM;
	}

	if (prect) memcpy(prect, &stabData.rc, sizeof(stabData.rc));
	if (pnEdge) *pnEdge = stabData.uEdge;

	if (!pnhTaskbar) return 1;

	if (nretval & ABS_AUTOHIDE) *pnhTaskbar = 1;
	else if (stabData.uEdge != ABE_BOTTOM) *pnhTaskbar = 1;
	else *pnhTaskbar = stabData.rc.bottom - stabData.rc.top;

	return 1;
}

BOOL CMainFrame::ToggleFullScreenMode()
{
	RECT rectDesktop;
	WINDOWPLACEMENT wpNew;

	if (!IsFullScreen())
	{
		//		if ( GetSystemMetrics(SM_CXSCREEN)<1024 ) return FALSE;
		// need to hide all status bars
		//m_wndStatusBar.ShowWindow(SW_HIDE);
		//m_wndToolBar.ShowWindow(SW_HIDE);
		//EnableDocking(0);

		// We'll need these to restore the original state.
		GetWindowPlacement(&m_wpPrev);
		m_wpPrev.length = sizeof(WINDOWPLACEMENT);// m_wpPrev;

		//Adjust RECT to new size of window
		::GetWindowRect(::GetDesktopWindow(), &rectDesktop);
		::AdjustWindowRectEx(&rectDesktop, GetStyle(), FALSE,
			GetExStyle());

		int		nState, nEdge, nhTaskbar;
		RECT	rtTaskbar;

		GfHelp_GetSystemTaskBarInfo(&nState, &rtTaskbar, &nEdge, &nhTaskbar);
		//theApp.m_nhTaskbar = nhTaskbar;

		// Remember this for OnGetMinMaxInfo()
		rectDesktop.left -= 2;
		rectDesktop.top -= 2;
		rectDesktop.right += 2;
		rectDesktop.bottom -= nhTaskbar;
		m_FullScreenWindowRect = rectDesktop;

		wpNew = m_wpPrev;
		wpNew.showCmd = SW_SHOWNORMAL;
		wpNew.rcNormalPosition = rectDesktop;
		m_bIsFullScreen = TRUE;
	}
	else
	{
		//EnableDocking(CBRS_ALIGN_ANY);
		m_bIsFullScreen = FALSE;

		//m_wndStatusBar.ShowWindow(SW_SHOWNORMAL);
		//m_wndToolBar.ShowWindow(SW_SHOWNORMAL);
		wpNew = m_wpPrev;
	}

	SetWindowPlacement(&wpNew);
	return	TRUE;
}

BOOL CMainFrame::IsFullScreen()
{
	return	m_bIsFullScreen;
}

//void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
//{
//	// TODO: Add your message handler code here and/or call default
//	if (IsFullScreen())
//	{
//		lpMMI->ptMaxSize.y = m_FullScreenWindowRect.Height();
//		lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y;
//		lpMMI->ptMaxSize.x = m_FullScreenWindowRect.Width();
//		lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x;
//	}
//	else
//		CFrameWnd::OnGetMinMaxInfo(lpMMI);
//}
//
//void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
//{
//	// TODO: Add your message handler code here and/or call default
//	if (nID == SC_CLOSE)
//	{
//		
//		/*if (m_pEditView && IsWindow(m_pEditView->GetSafeHwnd()))
//		{
//			if (!m_pEditView->ExitTheProcess())
//				return;
//		}*/
//	}
//	//if (nID == SC_MOVE || nID == SC_SIZE || nID == SC_MAXIMIZE)
//	//	return;
//
//	CFrameWnd::OnSysCommand(nID, lParam);
//}




void CMainFrame::OnChkFlatfinger()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFlat = !_bChkFlat;
}


void CMainFrame::OnUpdateChkFlatfinger(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFlat);
	pCmdUI->SetCheck(_bChkFlat);
}


void CMainFrame::OnChkSlap()
{
	// TODO:  在此添加命令处理程序代码
	_bChkSlap = !_bChkSlap;
}


void CMainFrame::OnUpdateChkSlap(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableSlap);
	pCmdUI->SetCheck(_bChkSlap);
}


void CMainFrame::OnChkRollfinger()
{
	// TODO:  在此添加命令处理程序代码
	_bChkRoll = !_bChkRoll;
}


void CMainFrame::OnUpdateChkRollfinger(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableRoll);
	pCmdUI->SetCheck(_bChkRoll);
}


void CMainFrame::OnChkPalm()
{
	// TODO:  在此添加命令处理程序代码
	_bChkPalm = !_bChkPalm;
}


void CMainFrame::OnUpdateChkPalm(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnablePalm);
	pCmdUI->SetCheck(_bChkPalm);
}


void CMainFrame::OnBtnStart()
{
	// TODO:  在此添加命令处理程序代码

	//弹出采集窗口
	CCaptureDlg	dlg;
	//采集顺序
	CCaptureDlg::pair_gainandexposure_t pairParam;
	CCaptureDlg::pair_typeandparam_t	pairType;
	CCaptureDlg::vct_captype_t	vctProc;

	if (_bChkSlap)
	{
		pairParam.first = _nSlapGain;
		pairParam.second = _nSlapExposure;
		pairType.first = CCaptureDlg::CAPTURE_SLAP;
		pairType.second = pairParam;
		
		vctProc.push_back(pairType);
	}
	
	if (_bChkRoll)
	{
		pairParam.first = _nFingerGain;
		pairParam.second = _nFingerExposure;
		pairType.first = CCaptureDlg::CAPTURE_ROLLFINGER;
		pairType.second = pairParam;

		vctProc.push_back(pairType);
	}

	dlg.SetFingerNotExist(_bChkFinger);
	dlg.PushCaptureProc(vctProc);

	//需要增加取消按钮
	if(IDCANCEL == dlg.DoModal())
		return;

	//显示采集完成数据
	CgOgfA5KDemoView *pView = (CgOgfA5KDemoView *)GetActiveView();
	if (pView == NULL)
		return;

	pView->_dlgFingerShowArea._dlgSlap._stRFour.SetImageInfo(dlg._strImageSlap[SLAP_R_FOUR - 1], CCaptureDlg::SLAP_WIDTH, CCaptureDlg::SLAP_HEIGHT);
	pView->_dlgFingerShowArea._dlgSlap._stLFour.SetImageInfo(dlg._strImageSlap[SLAP_L_FOUR - 1], CCaptureDlg::SLAP_WIDTH, CCaptureDlg::SLAP_HEIGHT);
	pView->_dlgFingerShowArea._dlgSlap._stRLThumb.SetImageInfo(dlg._strImageSlap[SLAP_RL_THUMB - 1], CCaptureDlg::SLAP_WIDTH, CCaptureDlg::SLAP_HEIGHT);

	pView->_dlgFingerShowArea._dlgSlap._stRFour.SetFgp(CCaptureDlg::CAPTURE_SLAP, CCaptureDlg::SLAP_R_FOUR);
	pView->_dlgFingerShowArea._dlgSlap._stLFour.SetFgp(CCaptureDlg::CAPTURE_SLAP, CCaptureDlg::SLAP_L_FOUR);
	pView->_dlgFingerShowArea._dlgSlap._stRLThumb.SetFgp(CCaptureDlg::CAPTURE_SLAP, CCaptureDlg::SLAP_RL_THUMB);

	for (int i = 0; i <= sizeof(dlg._mapSplitedSlap) / sizeof(dlg._mapSplitedSlap[0]); ++i)
	{
		auto &mapSplit = dlg._mapSplitedSlap[i];
		//没取得切割的平面指纹，重置之
		if (mapSplit.empty())
		{
			if (i == SLAP_R_FOUR - 1)
			{
				pView->_dlgFingerShowArea._dlgFlat._stFinger[1].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
				pView->_dlgFingerShowArea._dlgFlat._stFinger[2].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
				pView->_dlgFingerShowArea._dlgFlat._stFinger[3].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
				pView->_dlgFingerShowArea._dlgFlat._stFinger[4].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
			}
			else if (i == SLAP_L_FOUR - 1)
			{
				pView->_dlgFingerShowArea._dlgFlat._stFinger[6].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
				pView->_dlgFingerShowArea._dlgFlat._stFinger[7].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
				pView->_dlgFingerShowArea._dlgFlat._stFinger[8].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
				pView->_dlgFingerShowArea._dlgFlat._stFinger[9].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
			}
			else if (i == SLAP_RL_THUMB - 1)
			{
				pView->_dlgFingerShowArea._dlgFlat._stFinger[0].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
				pView->_dlgFingerShowArea._dlgFlat._stFinger[5].SetImageInfo("", CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
			}

			//pView->_dlgFingerShowArea._dlgFlat._stFinger[i].SetFgp(CCaptureDlg::CAPTURE_FLATFINGER, i + 1);
			continue;
		}

		for (auto &itr = mapSplit.begin();
			itr != mapSplit.end();
			itr++)
		{
			//已经补齐
			pView->_dlgFingerShowArea._dlgFlat._stFinger[itr->first - 1].SetImageInfo(itr->second, CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
			pView->_dlgFingerShowArea._dlgFlat._stFinger[itr->first - 1].SetFgp(CCaptureDlg::CAPTURE_FLATFINGER, itr->first);
		}
	}
	
	int i = 0;
	for (auto &finger : dlg._strRollFinger)
	{
		pView->_dlgFingerShowArea._dlgRoll._stFinger[i].SetImageInfo(finger, CCaptureDlg::FINGER_WIDTH, CCaptureDlg::FINGER_HEIGHT);
		pView->_dlgFingerShowArea._dlgRoll._stFinger[i].SetFgp(CCaptureDlg::CAPTURE_ROLLFINGER, i + 1);
		i++;
	}

	pView->Invalidate(TRUE);
	pView->_dlgFingerShowArea._dlgSlap._stRFour.Invalidate(TRUE);
	pView->_dlgFingerShowArea._dlgSlap._stLFour.Invalidate(TRUE);
	pView->_dlgFingerShowArea._dlgSlap._stRLThumb.Invalidate(TRUE);

	for (auto &st : pView->_dlgFingerShowArea._dlgFlat._stFinger)
	{
		st.Invalidate(TRUE);
	}

	for (auto &st : pView->_dlgFingerShowArea._dlgRoll._stFinger)
	{
		st.Invalidate(TRUE);
	}
}


void CMainFrame::OnUpdateBtnStart(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableStart);
	
}


void CMainFrame::OnChkR1()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_R_THUMB - 1] = !_bChkFinger[FINGER_R_THUMB - 1];
}


void CMainFrame::OnUpdateChkR1(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_R_THUMB - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_R_THUMB - 1]);
}


void CMainFrame::OnChkR2()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_R_INDEX - 1] = !_bChkFinger[FINGER_R_INDEX - 1];
}


void CMainFrame::OnUpdateChkR2(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_R_INDEX - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_R_INDEX - 1]);
}

void CMainFrame::OnChkR3()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_R_MIDDLE - 1] = !_bChkFinger[FINGER_R_MIDDLE - 1];
}


void CMainFrame::OnUpdateChkR3(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_R_MIDDLE - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_R_MIDDLE - 1]);
}

void CMainFrame::OnChkR4()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_R_RING - 1] = !_bChkFinger[FINGER_R_RING - 1];
}


void CMainFrame::OnUpdateChkR4(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_R_RING - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_R_RING - 1]);
}

void CMainFrame::OnChkR5()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_R_LITTLE - 1] = !_bChkFinger[FINGER_R_LITTLE - 1];
}


void CMainFrame::OnUpdateChkR5(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_R_LITTLE - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_R_LITTLE - 1]);
}

void CMainFrame::OnChkL1()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_L_THUMB - 1] = !_bChkFinger[FINGER_L_THUMB - 1];
}


void CMainFrame::OnUpdateChkL1(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_L_THUMB - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_L_THUMB - 1]);
}


void CMainFrame::OnChkL2()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_L_INDEX - 1] = !_bChkFinger[FINGER_L_INDEX - 1];
}


void CMainFrame::OnUpdateChkL2(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_L_INDEX - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_L_INDEX - 1]);
}

void CMainFrame::OnChkL3()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_L_MIDDLE - 1] = !_bChkFinger[FINGER_L_MIDDLE - 1];
}


void CMainFrame::OnUpdateChkL3(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_L_MIDDLE - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_L_MIDDLE - 1]);
}

void CMainFrame::OnChkL4()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_L_RING - 1] = !_bChkFinger[FINGER_L_RING - 1];
}


void CMainFrame::OnUpdateChkL4(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_L_RING - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_L_RING - 1]);
}

void CMainFrame::OnChkL5()
{
	// TODO:  在此添加命令处理程序代码
	_bChkFinger[FINGER_L_LITTLE - 1] = !_bChkFinger[FINGER_L_LITTLE - 1];
}


void CMainFrame::OnUpdateChkL5(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(_bEnableFinger[FINGER_L_LITTLE - 1]);
	pCmdUI->SetCheck(_bChkFinger[FINGER_L_LITTLE - 1]);
}


void CMainFrame::OnBtnExportbmp()
{
	// TODO:  在此添加命令处理程序代码
	CgOgfA5KDemoView *pView = (CgOgfA5KDemoView *)GetActiveView();
	if (pView == NULL)
		return;

	pView->_dlgFingerShowArea._dlgSlap._stRFour.SaveImage2File(IMAGE_TYPE_BMP);
	pView->_dlgFingerShowArea._dlgSlap._stLFour.SaveImage2File(IMAGE_TYPE_BMP);
	pView->_dlgFingerShowArea._dlgSlap._stRLThumb.SaveImage2File(IMAGE_TYPE_BMP);

	for (auto &finger : pView->_dlgFingerShowArea._dlgFlat._stFinger)
	{
		finger.SaveImage2File(IMAGE_TYPE_BMP);
	}

	for (auto &finger : pView->_dlgFingerShowArea._dlgRoll._stFinger)
	{
		finger.SaveImage2File(IMAGE_TYPE_BMP);
	}

	AfxMessageBox(_T("succeed to export to bmp file"));
}


void CMainFrame::OnBtnExport2WSQ()
{
	// TODO:  在此添加命令处理程序代码
	CgOgfA5KDemoView *pView = (CgOgfA5KDemoView *)GetActiveView();
	if (pView == NULL)
		return;

	pView->_dlgFingerShowArea._dlgSlap._stRFour.SaveImage2File(IMAGE_TYPE_WSQ);
	pView->_dlgFingerShowArea._dlgSlap._stLFour.SaveImage2File(IMAGE_TYPE_WSQ);
	pView->_dlgFingerShowArea._dlgSlap._stRLThumb.SaveImage2File(IMAGE_TYPE_WSQ);

	for (auto &finger : pView->_dlgFingerShowArea._dlgFlat._stFinger)
	{
		finger.SaveImage2File(IMAGE_TYPE_WSQ);
	}

	for (auto &finger : pView->_dlgFingerShowArea._dlgRoll._stFinger)
	{
		finger.SaveImage2File(IMAGE_TYPE_WSQ);
	}

	AfxMessageBox(_T("succeed to export to wsq file"));
}


void CMainFrame::OnBtnTestslap()
{
	// TODO:  在此添加命令处理程序代码
	CCaptureTest dlg;
	dlg.SetTestType(true, _nSlapGain, _nSlapExposure);
	
	if (IDCANCEL == dlg.DoModal())
		return;

	if(0 > dlg.GetGainAndExposure(_nSlapGain, _nSlapExposure))
		return;

	//保存对比度/亮度
	WriteLoadIni(this);

	_bSlapTest = true;
}


void CMainFrame::OnBtnTestfinger()
{
	// TODO:  在此添加命令处理程序代码
	CCaptureTest dlg;
	dlg.SetTestType(false, _nFingerGain, _nFingerExposure);
	
	if (IDCANCEL == dlg.DoModal())
		return;

	if (0 > dlg.GetGainAndExposure(_nFingerGain, _nFingerExposure))
		return;

	//保存对比度/亮度
	WriteLoadIni(this);

	_bFingerTest = true;
}
