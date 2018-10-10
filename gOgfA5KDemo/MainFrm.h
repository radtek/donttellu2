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

// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "CaptureTest.h"

class CMainFrame : public CFrameWndEx
{
public:

	enum
	{
		FINGER_R_THUMB = 1,
		FINGER_R_INDEX = 2,
		FINGER_R_MIDDLE = 3,
		FINGER_R_RING = 4,
		FINGER_R_LITTLE = 5,
		FINGER_L_THUMB = 6,
		FINGER_L_INDEX = 7,
		FINGER_L_MIDDLE = 8,
		FINGER_L_RING = 9,
		FINGER_L_LITTLE = 10,
	};

	enum
	{
		SLAP_R_FOUR = 1,
		SLAP_L_FOUR = 2,
		SLAP_RL_THUMB = 3,
	};


protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:
	BOOL IsFullScreen();
	BOOL ToggleFullScreenMode();

	BOOL	m_bIsFullScreen;
	WINDOWPLACEMENT		m_wpPrev;
	CRect	m_FullScreenWindowRect;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	//afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	BOOL		_bChkSlap;
	BOOL		_bChkFlat;
	BOOL		_bChkRoll;
	BOOL		_bChkPalm;

	BOOL		_bEnableSlap;
	BOOL		_bEnableFlat;
	BOOL		_bEnableRoll;
	BOOL		_bEnablePalm;

	BOOL		_bEnableStart;

	afx_msg void OnChkFlatfinger();
	afx_msg void OnUpdateChkFlatfinger(CCmdUI *pCmdUI);
	afx_msg void OnChkSlap();
	afx_msg void OnUpdateChkSlap(CCmdUI *pCmdUI);
	afx_msg void OnChkRollfinger();
	afx_msg void OnUpdateChkRollfinger(CCmdUI *pCmdUI);
	afx_msg void OnChkPalm();
	afx_msg void OnUpdateChkPalm(CCmdUI *pCmdUI);
	afx_msg void OnBtnStart();
	afx_msg void OnUpdateBtnStart(CCmdUI *pCmdUI);

	BOOL		_bChkFinger[10];
	BOOL		_bEnableFinger[10];
	int			_nFingerGain = FINGER_GAIN;
	int			_nFingerExposure = FINGER_EXPOSURE;
	int			_nSlapGain = SLAP_GAIN;
	int			_nSlapExposure = SLAP_EXPOSURE;
	bool		_bFingerTest = false;
	bool		_bSlapTest = false;


	/*BOOL		_bChkR1;
	BOOL		_bChkR2;
	BOOL		_bChkR3;
	BOOL		_bChkR4;
	BOOL		_bChkR5;

	BOOL		_bEnableR1;
	BOOL		_bEnableR2;
	BOOL		_bEnableR3;
	BOOL		_bEnableR4;
	BOOL		_bEnableR5;*/

	afx_msg void OnChkR1();
	afx_msg void OnUpdateChkR1(CCmdUI *pCmdUI);
	afx_msg void OnChkR2();
	afx_msg void OnUpdateChkR2(CCmdUI *pCmdUI);
	afx_msg void OnChkR3();
	afx_msg void OnUpdateChkR3(CCmdUI *pCmdUI);
	afx_msg void OnChkR4();
	afx_msg void OnUpdateChkR4(CCmdUI *pCmdUI);
	afx_msg void OnChkR5();
	afx_msg void OnUpdateChkR5(CCmdUI *pCmdUI);

	/*BOOL		_bChkL1;
	BOOL		_bChkL2;
	BOOL		_bChkL3;
	BOOL		_bChkL4;
	BOOL		_bChkL5;

	BOOL		_bEnableL1;
	BOOL		_bEnableL2;
	BOOL		_bEnableL3;
	BOOL		_bEnableL4;
	BOOL		_bEnableL5;*/

	afx_msg void OnChkL1();
	afx_msg void OnUpdateChkL1(CCmdUI *pCmdUI);
	afx_msg void OnChkL2();
	afx_msg void OnUpdateChkL2(CCmdUI *pCmdUI);
	afx_msg void OnChkL3();
	afx_msg void OnUpdateChkL3(CCmdUI *pCmdUI);
	afx_msg void OnChkL4();
	afx_msg void OnUpdateChkL4(CCmdUI *pCmdUI);
	afx_msg void OnChkL5();
	afx_msg void OnUpdateChkL5(CCmdUI *pCmdUI);
	afx_msg void OnBtnExportbmp();
	afx_msg void OnBtnExport2WSQ();
	afx_msg void OnBtnTestslap();
	afx_msg void OnBtnTestfinger();
};


