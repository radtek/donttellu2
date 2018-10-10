// DlgCheckSpeed.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "DlgCheckSpeed.h"
#include "GflsScanerTestView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckSpeed dialog


CDlgCheckSpeed::CDlgCheckSpeed(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCheckSpeed::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCheckSpeed)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pTestView = NULL;
}


void CDlgCheckSpeed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCheckSpeed)
	DDX_Control(pDX, IDC_CAPTURE_PROGRESS, m_CaptureProgress);
	DDX_Control(pDX, IDC_EDIT_CAPTUREFRAME, m_editCaptFrmCnt);
	DDX_Control(pDX, IDC_EDIT_CAPTURETIME, m_editCaptUseTime);
	DDX_Control(pDX, IDC_EDIT_CAPTURESPEED, m_editCaptSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCheckSpeed, CDialog)
	//{{AFX_MSG_MAP(CDlgCheckSpeed)
	ON_BN_CLICKED(IDC_BTN_CAPTURESPEED, OnBtnCapturespeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckSpeed message handlers

BOOL CDlgCheckSpeed::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ASSERT(m_pTestView);

	m_editCaptFrmCnt.SetLimitText(4);	// 最多9999帧
	CString strInfo;
	strInfo.Format(_T("%d"), m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount);
	m_editCaptFrmCnt.SetWindowText(strInfo);

	GetDlgItem(IDC_BTN_CAPTURESPEED)->EnableWindow(g_stScannerInfo.bInitialed);
	
	UpdateData(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCheckSpeed::OnBtnCapturespeed() 
{
	// TODO: Add your control notification handler code here
	if(NULL == m_pTestView->m_dlgImgInfo.m_pCaptBuffer)
		return;
	
	if(g_stScannerInfo.bCaptureImage)
		m_pTestView->m_pFrameWnd->SendMessage(WM_COMMAND, IDC_CHECK_STARTCAPTURE, 0);

	TCHAR szInfo[64] = {0};
	m_editCaptFrmCnt.GetWindowText(szInfo, sizeof(szInfo) - 1);
	if(*szInfo)
		_stscanf(szInfo, _T("%d"), &m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount);
	else
		m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount = 0;
	if(m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount < 1)
	{
		TCHAR szError[256] = {0};
		_tcsncpy(szError, _T("请设置要采集的图像帧数!"), sizeof(szError) - 1);
		ShowTestAppErrorInfo(szError, this);
		m_editCaptFrmCnt.SetFocus();
		return;
	}
	
	CWaitCursor waitCur;

	int		nret, i;
	DWORD	dwStart, dwEnded;
	double	fSecond, fOneMaxFrame, fOneMinFrame;
	__int64	nFrameTimeUsed, nTimeUsed;
	__int64 nMaxFrameTime, nMinFrameTime;
	if (reType == 4)
{
	nret = g_stLiveCaptDll.pfnLiveScan_BeginCapture(g_stScannerInfo.nCurChannel);
	if(nret < 1)
	{
		ShowScannerErrorInfo(nret, this, 1);
		return;
	}
	nret = g_stLiveCaptDll.pfnLiveScan_GetFPRawData(g_stScannerInfo.nCurChannel, m_pTestView->m_dlgImgInfo.m_pCaptBuffer);
	nret = g_stLiveCaptDll.pfnLiveScan_EndCapture(g_stScannerInfo.nCurChannel);	

	m_CaptureProgress.SetRange32(0, m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount - 1);
	m_CaptureProgress.SetPos(0);
	dwStart = GetTickCount();

	nFrameTimeUsed = nMaxFrameTime = 0;
	nMinFrameTime = 0xffffffffffff;
	for(i = 0; i < m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount; i++)
	{
		m_CaptureProgress.SetPos(i);
		nTimeUsed = UTIL_HiResoTickCount();
		g_stLiveCaptDll.pfnLiveScan_BeginCapture(g_stScannerInfo.nCurChannel);
		g_stLiveCaptDll.pfnLiveScan_GetFPRawData(g_stScannerInfo.nCurChannel, m_pTestView->m_dlgImgInfo.m_pCaptBuffer);
		g_stLiveCaptDll.pfnLiveScan_EndCapture(g_stScannerInfo.nCurChannel);	
		nTimeUsed = UTIL_HiResoTickCount() - nTimeUsed;
		nFrameTimeUsed += nTimeUsed;
		if ( nTimeUsed > nMaxFrameTime ) nMaxFrameTime = nTimeUsed;
		if ( nTimeUsed < nMinFrameTime ) nMinFrameTime = nTimeUsed;
	}

			
}
else
{
	nret = g_stCaptDll.pfnScan_BeginCapture(g_stScannerInfo.nCurChannel);
	if(nret < 1)
	{
		ShowScannerErrorInfo(nret, this, 1);
		return;
	}
	nret = g_stCaptDll.pfnScan_GetFPRawData(g_stScannerInfo.nCurChannel, m_pTestView->m_dlgImgInfo.m_pCaptBuffer);
	nret = g_stCaptDll.pfnScan_EndCapture(g_stScannerInfo.nCurChannel);	

	m_CaptureProgress.SetRange32(0, m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount - 1);
	m_CaptureProgress.SetPos(0);
	dwStart = GetTickCount();

	nFrameTimeUsed = nMaxFrameTime = 0;
	nMinFrameTime = 0xffffffffffff;
	for(i = 0; i < m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount; i++)
	{
		m_CaptureProgress.SetPos(i);
		nTimeUsed = UTIL_HiResoTickCount();
		g_stCaptDll.pfnScan_BeginCapture(g_stScannerInfo.nCurChannel);
		g_stCaptDll.pfnScan_GetFPRawData(g_stScannerInfo.nCurChannel, m_pTestView->m_dlgImgInfo.m_pCaptBuffer);
		g_stCaptDll.pfnScan_EndCapture(g_stScannerInfo.nCurChannel);	
		nTimeUsed = UTIL_HiResoTickCount() - nTimeUsed;
		nFrameTimeUsed += nTimeUsed;
		if ( nTimeUsed > nMaxFrameTime ) nMaxFrameTime = nTimeUsed;
		if ( nTimeUsed < nMinFrameTime ) nMinFrameTime = nTimeUsed;
	}		
}


	dwEnded = GetTickCount();
	if (dwEnded == dwStart) fSecond = 0.0001;
	else fSecond = (double)(dwEnded - dwStart) / 1000.0;

	fSecond = UTIL_HiResoTickToSec(nFrameTimeUsed);

	fOneMaxFrame = UTIL_HiResoTickToSec(nMaxFrameTime);
	fOneMinFrame = UTIL_HiResoTickToSec(nMinFrameTime);

	m_pTestView->m_dlgImgInfo.m_stCaptSpeed.fSpeed = (double)m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nFrameCount / fSecond;
	m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nUsedTimes  = (int)(fSecond + 0.95);
//	sprintf(szInfo, "%d", m_pTestView->m_dlgImgInfo.m_stCaptSpeed.nUsedTimes);
	_stprintf(szInfo, _T("%.3f"), fSecond);
	m_editCaptUseTime.SetWindowText(szInfo);
	_stprintf(szInfo, _T("%.3f"), m_pTestView->m_dlgImgInfo.m_stCaptSpeed.fSpeed);
	m_editCaptSpeed.SetWindowText(szInfo);

	
}
