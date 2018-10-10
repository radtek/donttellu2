// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GflsScanerTest.h"

#include "MainFrm.h"
#include "GflsScanerTestView.h"
//#include "DlgChkImageBadpt.h"
#include "DlgSelectChannel.h"
#include "DlgPhyDistance.h"
#include "GraBar.h"
#include "DlgImageSize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDC_CHECK_STARTCAPTURE, OnCheckStartcapture)
	ON_COMMAND(IDC_BTN_SAVE2FILE, OnBtnSave2file)
	ON_COMMAND(IDC_BTN_FROMFILE, OnBtnFromfile)
	ON_COMMAND(IDC_BTN_ZOOMIN, OnImageZoomIn)
	ON_COMMAND(IDC_BTN_ZOOMOUT, OnImageZoomOut)
	ON_COMMAND(IDC_BTN_REVERSE,OnImageReverse)
	ON_COMMAND(IDC_CHECK_ONLYSHOWIMG, OnCheckOnlyShowImg)
	ON_COMMAND(IDC_CHECK_INSERTZOOM, OnCheckInsertZoom)
	ON_COMMAND(IDC_CHECK_SHOWBADPT, OnCheckShowBadPoint)
	ON_COMMAND(ID_SCANNER_INITLIVECAPT, OnInitialLiveCapture)
	//ON_COMMAND(ID_SCANNER_INITLIVECAPT1, OnInitialLiveCapture1)
	ON_COMMAND(ID_SCANNER_SELECTCHANNEL, OnSelectScannerChannel)
	ON_UPDATE_COMMAND_UI(ID_SCANNER_INITLIVECAPT, OnUpdateInitLiveCapture)
	ON_UPDATE_COMMAND_UI(ID_SCANNER_INITLIVECAPT1, OnUpdateInitLiveCapture1)
	ON_UPDATE_COMMAND_UI(ID_SCANNER_SELECTCHANNEL, OnUpdateScannerChannel)
	ON_WM_HSCROLL()
	ON_COMMAND(IDC_BTN_CALCIMGABER, OnBtnCalcImgAber)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_STDCHK_OPTICSDEVICE, ID_LOAD_MOSAICDLL, OnStdCheckHandler)
	ON_COMMAND_RANGE(ID_SETPWH,ID_GETSETUPDLG,OnStdCheck2Handler)
	ON_COMMAND_RANGE(ID_ISISFIN,ID_IMGENH,OnCheckHandler)
	ON_UPDATE_COMMAND_UI_RANGE(ID_STDCHK_OPTICSDEVICE, ID_LOAD_MOSAICDLL, OnUpdateStdChkHandler)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_CHECK_STARTCAPTURE, IDC_BTN_ZOOMIN, OnUpdateDlgBarCtrls)
	ON_UPDATE_COMMAND_UI(IDC_BTN_REVERSE,OnUpdateImageReverse)
	ON_COMMAND_RANGE(ID_MOSAIC_ROLL1FINGER, ID_MOSAIC_PLAINFINGER, OnMosaicImageHandler)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MOSAIC_ROLL1FINGER, ID_MOSAIC_PLAINFINGER, OnUpdateMosaicImageHandler)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SETPWH, ID_GETSETUPDLG, OnUpdateCheckHandler)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ISISFIN,ID_IMGENH, OnUpdateCheck2Handler)
	ON_BN_CLICKED(IDC_BTN_PHYDISTANCE, OnBtnPhydistance)	
	ON_BN_CLICKED(IDC_BTN_GAFISTEST, OnBtnGafisTest)	
	ON_BN_CLICKED(IDC_BTN_SETSN, OnBtnSetSerialNumClick)
	ON_BN_CLICKED(IDC_BTN_GETSN, OnBtnGetSerialNumClick)
	ON_UPDATE_COMMAND_UI(IDC_BTN_SETSN, OnUpdateDlgBarSerialNumCtrls)
	ON_UPDATE_COMMAND_UI(IDC_BTN_GETSN, OnUpdateDlgBarSerialNumCtrls)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_SN, OnUpdateDlgBarSerialNumCtrls)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_pTestView = NULL;
	m_nPhyDistance = 1;
	m_nSizeSelType = 0;
	m_nSizeWidth = GFLS_SCANNER_FINGER_WIDTH;
	m_nSizeHeight = GFLS_SCANNER_FINGER_HEIGHT;
}

CMainFrame::~CMainFrame()
{
}
int a;
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	a++;
//	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
//		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
//		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
//	{
//		TRACE0("Failed to create toolbar\n");
//		return -1;      // fail to create
//	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

//	if (!m_wndDlgBar.Create(this, IDD_DIALOG_TOPBAR,
//	  CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_DIALOG_TOPBAR))
	if (!m_wndDlgBar.Create(this, CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // Fail to create.
	}

	if(!m_wndAberDlbBar.Create(this, IDD_DIALOG_ABBRANCEBAR, CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_DIALOG_ABBRANCEBAR))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // Fail to create.
	}
	m_wndAberDlbBar.ShowWindow(SW_HIDE);

	if(!m_wndStdChkBar.Create(this, IDD_DIALOG_STDCHKBAR, CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_DIALOG_STDCHKBAR))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // Fail to create.
	}
	m_wndStdChkBar.ShowWindow(SW_HIDE);
	if(!m_wndGraxBar.Create(this, IDD_DIALOG_GRADUATIONX, CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_DIALOG_GRADUATIONX))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // Fail to create.
	}
	m_wndGraxBar.m_bhor = 1;
	m_wndGraxBar.ShowWindow(SW_SHOW);
	if(!m_wndGrayBar.Create(this, IDD_DIALOG_GRADUATIONY, CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_DIALOG_GRADUATIONY))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // Fail to create.
	}
	m_wndGrayBar.m_bhor = 0;
	m_wndGrayBar.ShowWindow(SW_SHOW);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_TOP);
//	DockControlBar(&m_wndToolBar);

	SetWindowText(_T("活体指纹采集仪测试"));

	SetBrightContrastValue();

	return 0;
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~FWS_ADDTOTITLE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnCheckStartcapture() 
{
	// TODO: Add your command handler code here
	if(NULL == m_pTestView)
		return;
	UCHAR bCaptureImage = (g_stScannerInfo.bCaptureImage == 1) ? 0 : 1;
	g_stScannerInfo.bCaptureImage = bCaptureImage;

	CButton *pCheck = (CButton*)m_wndDlgBar.GetDlgItem(IDC_CHECK_STARTCAPTURE);
	pCheck->SetCheck(bCaptureImage);

	CString strTitle;
	strTitle = bCaptureImage ? GFLS_SCANNER_STOPCAPTURE : GFLS_SCANNER_STARTCAPTURE;

	if(bCaptureImage)
	{
		m_wndDlgBar.GetDlgItem(IDC_CHECK_STARTCAPTURE)->SetWindowText(strTitle);
		StopCaptureThread();
		m_pTestView->StartCaptureImage();
	}
	else
	{
		StopCaptureThread();
		m_wndDlgBar.GetDlgItem(IDC_CHECK_STARTCAPTURE)->SetWindowText(strTitle);
	}
}

void CMainFrame::UpdateCaptureBtnStateAfterStopCapt()
{
	if(NULL == m_pTestView)
		return;
	CButton *pCheck = (CButton*)m_wndDlgBar.GetDlgItem(IDC_CHECK_STARTCAPTURE);
	pCheck->SetCheck(0);
	pCheck->SetWindowText(GFLS_SCANNER_STARTCAPTURE);
}

void CMainFrame::OnCheckOnlyShowImg()
{
	if(NULL == m_pTestView)
		return;
	if(m_pTestView->m_stParamInfo.bOnlyShowImage)
		m_pTestView->m_stParamInfo.bOnlyShowImage = 0;
	else
		m_pTestView->m_stParamInfo.bOnlyShowImage = 1;
	m_pTestView->Invalidate(FALSE);
}

void CMainFrame::OnCheckInsertZoom()
{
	if(m_pTestView->m_stParamInfo.bInsertZoom)
		m_pTestView->m_stParamInfo.bInsertZoom = 0;
	else
		m_pTestView->m_stParamInfo.bInsertZoom = 1;
	if(m_pTestView->m_stParamInfo.nCurScale != 1)
		m_pTestView->ZoomImageWithStyle();
}

void CMainFrame::OnBtnSave2file() 
{
	// TODO: Add your control notification handler code here
	char szFileName[260] = {0};
	int nret, nh, nw;
	CDlgImageSize dlg;
	UCHAR* pImageBuff = NULL;

	nret = GetBitmapFileName(szFileName, sizeof(szFileName), FALSE, this);
	if(nret < 1)
		return;

	dlg.m_nSelType = m_nSizeSelType;
	dlg.m_nWidth = m_nSizeWidth;
	dlg.m_nHeight = m_nSizeHeight;
	if(dlg.DoModal() != IDOK) 
		return;
	m_nSizeHeight = dlg.m_nHeight;
	m_nSizeWidth = dlg.m_nWidth;
	m_nSizeSelType = dlg.m_nSelType;
	dlg.GetCurrentImageSize(nw, nh);

	GFLS_IMAGEINFOSTRUCT stImageInfo = {0};
	stImageInfo.pImgData = m_pTestView->m_stParamInfo.pSrcImage;
	stImageInfo.prgbQuad = m_pTestView->m_stbmpInfo.bmiColors;
	memcpy(&stImageInfo.bmpHead, &m_pTestView->m_stbmpInfo.bmiHeader, sizeof(stImageInfo.bmpHead));
	stImageInfo.bmpHead.biWidth  = m_pTestView->m_stParamInfo.nImgWidth;
	stImageInfo.bmpHead.biHeight = m_pTestView->m_stParamInfo.nImgHeight;
	stImageInfo.bmpHead.biSizeImage = m_pTestView->m_stParamInfo.nImgSize;
	if(nw >0 && nh > 0 && (nw != stImageInfo.bmpHead.biWidth || nh != stImageInfo.bmpHead.biHeight))
	{
		pImageBuff = (UCHAR*)malloc(nw*nh);
		if(!pImageBuff) return;
		memset(pImageBuff, 255, nh*nw);
		UCHAR* pSrc = m_pTestView->m_stParamInfo.pSrcImage;
		UCHAR* pDes = pImageBuff;
		if(nh <= stImageInfo.bmpHead.biHeight)
			pSrc += (stImageInfo.bmpHead.biHeight - nh)/2*stImageInfo.bmpHead.biWidth;
		else
			pDes += (nh - stImageInfo.bmpHead.biHeight)/2 * nw;
		if(nw <= stImageInfo.bmpHead.biWidth)
			pSrc += (stImageInfo.bmpHead.biWidth - nw) /2;
		else
			pDes += (nw - stImageInfo.bmpHead.biWidth) /2;
		for (int row = 0; row < min(nh, stImageInfo.bmpHead.biHeight); ++row)
		{
			memcpy(pDes, pSrc, min(nw, stImageInfo.bmpHead.biWidth));
			pSrc += stImageInfo.bmpHead.biWidth;
			pDes += nw;
		}
		stImageInfo.pImgData = pImageBuff;
		stImageInfo.bmpHead.biWidth = nw;
		stImageInfo.bmpHead.biHeight = nh;
		stImageInfo.bmpHead.biSizeImage = nw*nh;
	}
	Gfls_SaveImageFile(szFileName, &stImageInfo);
	if(pImageBuff) free(pImageBuff);
}

void CMainFrame::OnBtnFromfile() 
{
	CStringA temp,temp1;
	CFile mFile;
	char strFileName[512] = {0};
	strcpy(strFileName,"tuxiang.log");
	// TODO: Add your control notification handler code here
	char szFileName[260] = {0};
	int nret;	//, i;

	nret = GetBitmapFileName(szFileName, sizeof(szFileName), TRUE, m_pTestView);
	if(nret < 1)
		return;

	GFLS_IMAGEINFOSTRUCT stImageInfo = {0};
	nret = Gfls_LoadImageFile(szFileName, &stImageInfo);
	if(nret < 1)
		goto FINISH_EXIT;
	if((stImageInfo.bmpHead.biWidth != GFLS_SCANNER_IMAGEWIDTH) || (stImageInfo.bmpHead.biHeight != GFLS_SCANNER_IMAGEHEIGHT))
	{
		TCHAR szInfo[256] = {0};
		_sntprintf(szInfo, sizeof(szInfo) - 1, _T("图像的大小(%dx%d)不符合要求!"), stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biHeight);
		ShowTestAppErrorInfo(szInfo, m_pTestView);
		goto FINISH_EXIT;
	}
	if(g_stMosaicDll.bLoaded)
	{
		nret = g_stMosaicDll.pfnMosaic_IsSupportImageQuality();
		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"5.4拼接接口是否提供判断图像质量的函数\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1||nret == 0)
		{
			temp += "测试\"5.4拼接接口是否提供判断图像质量的函数\"成功 返回值= "+temp1+"\r\n";
		}

		nret = g_stMosaicDll.pfnMosaic_IsSupportFingerQuality();
		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"5.5拼接接口是否提供判断指纹质量的函数\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1||nret == 0)
		{
			temp += "测试\"5.5拼接接口是否提供判断指纹质量的函数\"成功 返回值= "+temp1+"\r\n";
		}
		nret = g_stMosaicDll.pfnMosaic_IsFinger(stImageInfo.pImgData, 640, 640);
		if (nret < 0) 
		{temp += "测试\"5.15判断图像为指纹\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1||nret == 0)
		{
			temp += "测试\"5.15判断图像为指纹\"成功 返回值= "+temp1+"\r\n";
		}
		nret = g_stMosaicDll.pfnMosaic_IsSupportImageEnhance();
		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"5.6接口是否提供拼接指纹的图像增强功能\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1||nret == 0)
		{
			temp += "测试\"5.6接口是否提供拼接指纹的图像增强功能\"成功 返回值= "+temp1+"\r\n";
		}
		nret = g_stMosaicDll.pfnMosaic_ImageQuality(stImageInfo.pImgData,640,640);
		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"5.12判断图像质量\"失败 返回值= "+temp1+" \r\n";}
		else 
		{
			temp += "测试\"5.12判断图像质量\"成功 返回值= "+temp1+"\r\n";
		}
		nret =  g_stMosaicDll.pfnMosaic_FingerQuality(stImageInfo.pImgData,640,640);
		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"5.13判断指纹质量\"失败 返回值= "+temp1+" \r\n";}
		else 
		{
			temp += "测试\"5.13判断指纹质量\"成功 返回值= "+temp1+"\r\n";
		}
		UCHAR *pBuffer;
		pBuffer = (UCHAR*)malloc(640*640);
		nret =  g_stMosaicDll.pfnMosaic_ImageEnhance(stImageInfo.pImgData, 640, 640, pBuffer);
		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"5.14对图像进行增强\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1)
		{
			temp += "测试\"5.14对图像进行增强\"成功 返回值= "+temp1+"\r\n";
		}
		PutInfo2Log(strFileName,temp);
		if(nret == 1)
		{
			memcpy(m_pTestView->m_stParamInfo.pSrcImage, pBuffer, m_pTestView->m_stParamInfo.nImgSize);
		}

		else
		{
			memcpy(m_pTestView->m_stParamInfo.pSrcImage, stImageInfo.pImgData, m_pTestView->m_stParamInfo.nImgSize);
		}
		
	}
	else
	{
		memcpy(m_pTestView->m_stParamInfo.pSrcImage, stImageInfo.pImgData, m_pTestView->m_stParamInfo.nImgSize);
	}
	
	m_pTestView->ZoomImageWithStyle();
	m_pTestView->SendStatImageDataMessage(0, 0);



FINISH_EXIT:
	if(stImageInfo.pImgData)
		free(stImageInfo.pImgData);
}
void CMainFrame::OnUpdateImageReverse(CCmdUI* pCmdUI)
{
	if(NULL == m_pTestView)
		return;
	pCmdUI->Enable(!g_stScannerInfo.bCaptureImage);
}

void CMainFrame::OnUpdateDlgBarCtrls(CCmdUI* pCmdUI)
{
	if(NULL == m_pTestView)
		return;

	UCHAR bStdCheck = 0, nStdChkType;
	nStdChkType = m_pTestView->m_stParamInfo.nStdChkType;
	switch(pCmdUI->m_nID)
	{
	case IDC_BTN_FROMFILE:
	case IDC_BTN_SAVE2FILE:
		
		pCmdUI->Enable(!g_stScannerInfo.bCaptureImage);
		break;

	case IDC_CHECK_STARTCAPTURE:
		if(nStdChkType != GFLS_SCANNER_STDCHKTYPE_NORMAL && nStdChkType != GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT)
		{
			bStdCheck = 1;
		}
		pCmdUI->Enable(g_stScannerInfo.bInitialed && !bStdCheck);
		break;

	case IDC_BTN_ZOOMIN:
		if(nStdChkType == GFLS_SCANNER_STDCHKTYPE_BADPOINT)
			bStdCheck = 1;
		pCmdUI->Enable((m_pTestView->m_stParamInfo.nCurScale != GFLS_SCANNER_MAXSCALE) && !bStdCheck);
		break;

	case IDC_BTN_ZOOMOUT:
		if(nStdChkType == GFLS_SCANNER_STDCHKTYPE_BADPOINT)
			bStdCheck = 1;
		pCmdUI->Enable((m_pTestView->m_stParamInfo.nCurScale != GFLS_SCANNER_MINSCALE) && !bStdCheck);
		break;
		
	}
}

void CMainFrame::OnUpdateScannerChannel(CCmdUI* pCmdUI)
{
	BOOL bOn = TRUE;

	if(!g_stScannerInfo.bInitialed)
		bOn = FALSE;
	if(g_stScannerInfo.bCaptureImage)
		bOn = FALSE;
	if(g_stScannerInfo.bHasMosaicThrd)
		bOn = FALSE;
	if(g_stScannerInfo.nMaxChannel < 2)
		bOn = FALSE;
	pCmdUI->Enable(bOn);
}

void CMainFrame::OnUpdateInitLiveCapture(CCmdUI* pCmdUI)
{	

	if (reType == 4)
	{
		pCmdUI->Enable(g_stLiveCaptDll.bLoaded && !g_stScannerInfo.bInitialed);
	}
	else
	{
		pCmdUI->Enable(g_stCaptDll.bLoaded && !g_stScannerInfo.bInitialed);
	}


	
}

void CMainFrame::OnUpdateInitLiveCapture1(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMainFrame::OnInitialLiveCapture()
{

	if (reType == 4)
	{
		//InitialLiveCapture(this);
	m_pSetSerailNoDlg = new CSetSerialNoDlg;
	m_pSetSerailNoDlg->DoModal();
	unsigned int nSerialNo = m_pSetSerailNoDlg->m_nSerialNo;
	if ( nSerialNo == 0 )
	{
		InitialLiveCapture(this);
	}else
	{
		InitialLiveCaptureEx(this, nSerialNo);
	}
	}
	else
	{
		//InitialLiveCapture(this);
	/*m_pSetSerailNoDlg = new CSetSerialNoDlg;
	m_pSetSerailNoDlg->DoModal();
	unsigned int nSerialNo = m_pSetSerailNoDlg->m_nSerialNo;
	if ( nSerialNo == 0 )
	{
		InitialLiveCapture(this);
	}else
	{
		InitialLiveCaptureEx(this, nSerialNo);
	}*/
	InitialLiveCapture(this);
	}


	
}

// void CMainFrame::OnInitialLiveCapture1()
// {
// 	m_pSetSerailNoDlg = new CSetSerialNoDlg;
// 	m_pSetSerailNoDlg->DoModal();
// 	unsigned int nSerialNo = m_pSetSerailNoDlg->m_nSerialNo;
// 	//unsigned int nSerialNo = 200;
// 	InitialLiveCaptureEx(this, nSerialNo);
// }
void CMainFrame::OnSelectScannerChannel()
{
	CDlgSelectChannel dlg;
	dlg.DoModal();
}

void CMainFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if(NULL == m_pTestView)
		return;
	if(m_pTestView->m_stParamInfo.nStdChkType != GFLS_SCANNER_STDCHKTYPE_NORMAL && 
	   m_pTestView->m_stParamInfo.nStdChkType != GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT)
		return;

	CScrollBar *pBrightBar, *pContrastBar;


#if GFLS_EDZCS_GSTEST
	OnCheckStartcapture() ;
	//m_wndDlgBar.GetDlgItem(IDC_CHECK_STARTCAPTURE)->SetWindowText(strTitle);

#endif

	pBrightBar	  = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_BRIGHT_SCROLL);
	pContrastBar = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_CONTRAST_SCROLL);
	UCHAR bBright;

	if(NULL == pScrollBar)
		return;

#ifdef UNSTD_BLACK_BALANCE
	CScrollBar *pBlackBalanceBar = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_BLACKBALANCE_SCROLL);
	if (pBlackBalanceBar->m_hWnd == pScrollBar->m_hWnd)
	{
		int nOldValue, nNewValue, nLineSize, nPageSize;
		nLineSize = 1;	nPageSize = 10;
		nOldValue = g_stScannerInfo.nBlackBalance;
		nNewValue = nOldValue;
		switch (nSBCode)
		{
		case SB_THUMBPOSITION:
			nNewValue = nPos;
			break;

		case SB_LINELEFT:
			nNewValue -= nLineSize;
			break;

		case SB_PAGELEFT:
			nNewValue -= nPageSize;
			break;

		case SB_LINERIGHT:
			nNewValue += nLineSize;
			break;

		case SB_PAGERIGHT:
			nNewValue += nPageSize;
			break;

		case SB_THUMBTRACK:
			nNewValue = nPos;
			break;
		}
		if (nNewValue < 0)
			nNewValue = 0;
		if (nNewValue > 255)
			nNewValue = 255;
		if (nNewValue == nOldValue)
			return;

		if (ChangeScannerBlackBalance(nNewValue) >= 0)
			pScrollBar->SetScrollPos(nNewValue);

#if GFLS_EDZCS_GSTEST
		OnCheckStartcapture();
#endif

		return CFrameWnd::OnHScroll(nSBCode, nPos, pScrollBar);
	}
#endif
	if(pBrightBar->m_hWnd == pScrollBar->m_hWnd)
		bBright = 1;
	else if(pContrastBar->m_hWnd == pScrollBar->m_hWnd)
		bBright = 0;
	else
		return;

	int nOldValue, nNewValue, nLineSize, nPageSize;
	nLineSize = 1;	nPageSize = 10;
	nOldValue = bBright ? g_stScannerInfo.nBright : g_stScannerInfo.nContrast;
	nNewValue = nOldValue;
	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
		nNewValue = nPos;
		break;

	case SB_LINELEFT:
		nNewValue -= nLineSize;
		break;

	case SB_PAGELEFT:
		nNewValue -= nPageSize;
		break;

	case SB_LINERIGHT:
		nNewValue += nLineSize;
		break;

	case SB_PAGERIGHT:
		nNewValue += nPageSize;
		break;

	case SB_THUMBTRACK:
		nNewValue = nPos;
		break;
	}
	if(nNewValue < 0)
		nNewValue = 0;
	if(nNewValue > 255)
		nNewValue = 255;
	if(nNewValue == nOldValue)
		return;

	
	if(ChangeScannerBrightContrast(nNewValue, bBright) > 0)
		pScrollBar->SetScrollPos(nNewValue);

#if GFLS_EDZCS_GSTEST
	OnCheckStartcapture() ;
#endif
	
	CFrameWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMainFrame::ShowCurMousePosInfo(CPoint &pt, int nw, int nh, UCHAR bOnImage)
{
	if(!IsWindow(m_wndDlgBar.GetSafeHwnd()))
		return;

	CString strInfo;

	if(bOnImage)
	{
		strInfo.Format(_T("%d"), pt.x);
		m_wndDlgBar.GetDlgItem(IDC_STATIC_FOCUSINFO_X)->SetWindowText(strInfo);
		strInfo.Format(_T("%d"), pt.y);
		m_wndDlgBar.GetDlgItem(IDC_STATIC_FOCUSINFO_Y)->SetWindowText(strInfo);
		if(m_pTestView->m_stParamInfo.pDispImage)
		{
			int nIndex = pt.y * nw + pt.x;
			strInfo.Format(_T("%d"), m_pTestView->m_stParamInfo.pDispImage[nIndex]);
			m_wndDlgBar.GetDlgItem(IDC_STATIC_FOCUSINFO_GRAY)->SetWindowText(strInfo);
		}
	}
	else
	{
		m_wndDlgBar.GetDlgItem(IDC_STATIC_FOCUSINFO_X)->SetWindowText(_T(""));
		m_wndDlgBar.GetDlgItem(IDC_STATIC_FOCUSINFO_Y)->SetWindowText(_T(""));
		m_wndDlgBar.GetDlgItem(IDC_STATIC_FOCUSINFO_GRAY)->SetWindowText(_T(""));
	}
	
//	strInfo.Format("%d x %d", m_pTestView->m_stParamInfo.nImgWidth, m_pTestView->m_stParamInfo.nImgWidth);
//	m_wndDlgBar.GetDlgItem(IDC_STATIC_IMAGESIZE)->SetWindowText(strInfo);
}

void CMainFrame::SetBrightContrastValue()
{
	CScrollBar *pBrightBar, *pContrastBar;
	pBrightBar	 = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_BRIGHT_SCROLL);
	pContrastBar = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_CONTRAST_SCROLL);

//	if(!g_stScannerInfo.bInitialed)
//	{
//		pBrightBar->EnableWindow(0);
//		pContrastBar->EnableWindow(0);
//		return;
//	}

	pBrightBar->SetScrollRange(0, 255);
	pContrastBar->SetScrollRange(0, 255);

	pBrightBar->SetScrollPos(128);
	pContrastBar->SetScrollPos(128);

	CString strInfo;
	strInfo.Format(_T("%d"), 128);
	m_wndDlgBar.GetDlgItem(IDC_STATIC_BRIGHT)->SetWindowText(strInfo);
	strInfo.Format(_T("%d"), 128);
	m_wndDlgBar.GetDlgItem(IDC_STATIC_CONTRAST)->SetWindowText(strInfo);

#ifdef UNSTD_BLACK_BALANCE
	CScrollBar *pBlackBalanceBar = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_BLACKBALANCE_SCROLL);
	pBlackBalanceBar->SetScrollRange(0, 255);
	pBlackBalanceBar->SetScrollPos(128);

	strInfo.Format(_T("%d"), 128);
	m_wndDlgBar.GetDlgItem(IDC_STATIC_BLACKBALANCE)->SetWindowText(strInfo);
#endif
}

int CMainFrame::ChangeScannerBrightContrast(int nNewValue, UCHAR bBright)
{
	CStringA temp,temp1;
	CFile mFile;
	CFileException e;
	int m_bwriten = 0;
	int m_bBright = 0;
	int m_bContrast = 0;
	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
	/*UCHAR bCaptureImage = g_stScannerInfo.bCaptureImage;
	if(bCaptureImage)
		StopCaptureThread();
	g_stScannerInfo.bCaptureImage = 0;
	UpdateCaptureBtnStateAfterStopCapt();*/
	CString strInfo;
	strInfo.Format(_T("%d"), nNewValue);

	int nret = 1, *pValue, nID;
	if(bBright)
	{
		if(g_stScannerInfo.bInitialed)
		{
			if (reType == 4)
			{
					nret = g_stLiveCaptDll.pfnLiveScan_SetBright(g_stScannerInfo.nCurChannel, nNewValue);
			}
			else
			{
				nret = g_stCaptDll.pfnScan_SetBright(g_stScannerInfo.nCurChannel, nNewValue);
			}
			
			if(m_bBright == 0)
			{
				temp1.Format("%d",nret);
				if (nret < 0) 
				{temp += "测试\"4.4设置采集设备当前的亮度\"失败  返回值= "+temp1+" \r\n";}
				else if(nret == 1)
				{
					temp += "测试\"4.4设置采集设备当前的亮度\"成功 返回值= "+temp1+"\r\n";
				}
				m_bBright = 1; 
			}
		}
		pValue = &g_stScannerInfo.nBright;
		nID = IDC_STATIC_BRIGHT;
	}
	else
	{
		if(g_stScannerInfo.bInitialed)
		{
			nret = g_stCaptDll.pfnScan_SetContrast(g_stScannerInfo.nCurChannel, nNewValue);
		
			temp1.Format("%d",nret);
			if(m_bContrast == 0)
			{
			
				if (nret < 0) 
				{
					temp += "测试\"4.5设置采集设备当前的对比度\"失败 返回值= "+temp1+" \r\n";
				}
				else if(nret == 1)
				{
					temp += "测试\"4.5设置采集设备当前的对比度\"成功 返回值= "+temp1+"\r\n";
				}
				m_bContrast = 1;
			}
		}
		
		pValue = &g_stScannerInfo.nContrast;
		nID = IDC_STATIC_CONTRAST;
	}

	PutInfo2Log(strFileName,temp);
//	CFileStatus status;
//	if(mFile.GetStatus("caiji.log",status))
//	{
//		if (m_bwriten == 0) 
//		{
//			mFile.Open("caiji.log", CFile::modeWrite,&e);
//			mFile.SeekToEnd();
//			dwPos = mFile.GetPosition();
//			mFile.LockRange(dwPos, 1);
//			mFile.Write(temp,temp.GetLength());
//			mFile.UnlockRange(dwPos,1);
//			mFile.Flush();
//			mFile.Close(); 
//			m_bwriten = 1;
//		}
//	}
	if(nret == 1)
	{
		m_wndDlgBar.GetDlgItem(nID)->SetWindowText(strInfo);
		*pValue = nNewValue;

//		if(bCaptureImage)
//			m_pTestView->StartCaptureImage();
	}
	else
	{
		ShowScannerErrorInfo(nret, m_pTestView, 1);
	}
	
	return nret;
}

#ifdef UNSTD_BLACK_BALANCE
int CMainFrame::ChangeScannerBlackBalance(int nNewValue)
{
	CStringA temp, temp1;
	CFile mFile;
	CFileException e;
	int m_bwriten = 0;
	int bSucceed = 0;
	char strFileName[512] = { 0 };
	strcpy(strFileName, "caiji.log");
	/*UCHAR bCaptureImage = g_stScannerInfo.bCaptureImage;
	if(bCaptureImage)
	StopCaptureThread();
	g_stScannerInfo.bCaptureImage = 0;
	UpdateCaptureBtnStateAfterStopCapt();*/
	CString strInfo;
	strInfo.Format(_T("%d"), nNewValue);

	int nret = 1, *pValue, nID;
	
	if (g_stCaptDll.pfnScan_SetBlackBalance && g_stScannerInfo.bInitialed)
	{
		nret = g_stCaptDll.pfnScan_SetBlackBalance(g_stScannerInfo.nCurChannel, nNewValue);

		temp1.Format("%d", nret);
		if (bSucceed == 0)
		{
			if (nret < 0)
			{
				temp += "测试\"4.5设置采集设备当前的黑平衡\"失败 返回值= " + temp1 + " \r\n";
			}
			else if (nret == 1)
			{
				temp += "测试\"4.5设置采集设备当前的黑平衡\"成功 返回值= " + temp1 + "\r\n";
			}
			bSucceed = 1;
		}
	}

	pValue = &g_stScannerInfo.nBlackBalance;
	nID = IDC_STATIC_BLACKBALANCE;

	PutInfo2Log(strFileName, temp);
	//	CFileStatus status;
	//	if(mFile.GetStatus("caiji.log",status))
	//	{
	//		if (m_bwriten == 0) 
	//		{
	//			mFile.Open("caiji.log", CFile::modeWrite,&e);
	//			mFile.SeekToEnd();
	//			dwPos = mFile.GetPosition();
	//			mFile.LockRange(dwPos, 1);
	//			mFile.Write(temp,temp.GetLength());
	//			mFile.UnlockRange(dwPos,1);
	//			mFile.Flush();
	//			mFile.Close(); 
	//			m_bwriten = 1;
	//		}
	//	}
	if (nret == 1)
	{
		m_wndDlgBar.GetDlgItem(nID)->SetWindowText(strInfo);
		*pValue = nNewValue;

		//		if(bCaptureImage)
		//			m_pTestView->StartCaptureImage();
	}
	else
	{
		ShowScannerErrorInfo(nret, m_pTestView, 1);
	}

	return nret;
}
#endif

void CMainFrame::ShowCurSelectAreaInfo(LPPOINT pptLeftTop, LPPOINT pptRightBottom, int nOption)
{
	double fLine = 0.0, ftmp;
	int x1, x2, y1, y2;
	double  sum = 0;

	ftmp = (double)(pptLeftTop->x - pptRightBottom->x);
	fLine = ftmp * ftmp;
	ftmp = (double)(pptLeftTop->y - pptRightBottom->y);
	fLine += ftmp * ftmp;
	fLine = sqrt(fLine);

	x1 = min(pptLeftTop->x, pptRightBottom->x);
	x2 = max(pptLeftTop->x, pptRightBottom->x);
	y1 = min(pptLeftTop->y, pptRightBottom->y);
	y2 = max(pptLeftTop->y, pptRightBottom->y);

	CString strInfo;
	strInfo.Format(_T("%d,%d"), x1, y1);
	m_wndDlgBar.GetDlgItem(IDC_STATIC_AREALEFTTOP)->SetWindowText(strInfo);
	strInfo.Format(_T("%d,%d"), x2, y2);
	m_wndDlgBar.GetDlgItem(IDC_STATIC_AREARIGHTBOTTOM)->SetWindowText(strInfo);
	strInfo.Format(_T("%d x %d"), x2 - x1, y2 - y1);
	m_wndDlgBar.GetDlgItem(IDC_STATIC_AREASIZE)->SetWindowText(strInfo);
	strInfo.Format(_T("%.2f"), fLine);
	m_wndDlgBar.GetDlgItem(IDC_STATIC_AREAINFO)->SetWindowText(strInfo);

	if ( !nOption )
	{
		sum =sqrt(double( abs(x2 - x1)*abs(x2 - x1)+abs(y2 - y1)* abs(y2 - y1)) );
		strInfo.Format(_T("%.4f"), sum / m_nPhyDistance* 25.4);
		m_wndDlgBar.GetDlgItem(IDC_STATIC_DPI)->SetWindowText(strInfo);
	}	
}

void CMainFrame::OnImageZoomIn()
{
	ZoomImage(1);
}

void CMainFrame::OnImageZoomOut()
{
	ZoomImage(0);
}
void CMainFrame::OnImageReverse()
{

	m_pTestView->ReverseImg();

}

void CMainFrame::ZoomImage(UCHAR bEnlarge)
{
	if(NULL == m_pTestView->m_stParamInfo.pDispImage)
		return;
	int nScale = m_pTestView->m_stParamInfo.nCurScale;
	if(bEnlarge)
	{
		if(nScale == GFLS_SCANNER_MAXSCALE)
			return;
		nScale++;
	}
	else
	{
		if(nScale == GFLS_SCANNER_MINSCALE)
			return;
		nScale--;
	}

	m_pTestView->ZoomImage2PointedScale(nScale);	
}

void CMainFrame::ShowDialogBarWithStdChkType(UCHAR nStdChkType)
{
	if(NULL == m_pTestView)
		return;

	UCHAR bEnable = 0;
	switch(nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_NORMAL:
	case GFLS_SCANNER_STDCHKTYPE_OPTICSDEVICE:
	case GFLS_SCANNER_STDCHKTYPE_CAPTURESIZE:
	case GFLS_SCANNER_STDCHKTYPE_VALIDIMAGESIZE:
	case GFLS_SCANNER_STDCHKTYPE_IMAGEPIXELCOUNT:
	case GFLS_SCANNER_STDCHKTYPE_CENTERDIFF:
	case GFLS_SCANNER_STDCHKTYPE_IMGRESOLUTION:
	case GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYAVERAGE:
		m_wndAberDlbBar.ShowWindow(SW_HIDE);
		m_wndStdChkBar.ShowWindow(SW_HIDE);
		bEnable = 1;
		break;

	case GFLS_SCANNER_STDCHKTYPE_ABERRANCE:
		m_wndStdChkBar.ShowWindow(SW_HIDE);
		m_wndAberDlbBar.ShowWindow(SW_SHOW);
		
		break;

	default:
		m_wndAberDlbBar.ShowWindow(SW_HIDE);
		m_wndStdChkBar.ShowWindow(SW_SHOW);
		if(nStdChkType  == GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT)
			bEnable = 1;
		break;
	}

	RecalcLayout();

	CScrollBar *pScroll;
	pScroll = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_BRIGHT_SCROLL);
	pScroll->EnableWindow(bEnable);
	pScroll = (CScrollBar*)m_wndDlgBar.GetDlgItem(IDC_CONTRAST_SCROLL);
	pScroll->EnableWindow(bEnable);

	m_pTestView->SetStdCheckType(nStdChkType);
}
void CMainFrame::OnUpdateCheckHandler(CCmdUI* pCmdUI)
{
if (reType == 4)
{
	if(!g_stLiveCaptDll.bLoaded || !g_stScannerInfo.bInitialed)
	{
		pCmdUI->Enable(0);
		return;
	}
			
}
else
{
	if(!g_stCaptDll.bLoaded || !g_stScannerInfo.bInitialed)
	{
		pCmdUI->Enable(0);
		return;
	}	
}


}
void CMainFrame::OnUpdateCheck2Handler(CCmdUI* pCmdUI)
{
//	if(!g_stMosaicDll.bLoaded || !g_stScannerInfo.bInitialed)
//	{
//		pCmdUI->Enable(0);
//		return;
	//}
		switch(pCmdUI->m_nID) 
	{
		case ID_ISISFIN:
		if(!g_stMosaicDll.pfnMosaic_IsSupportIdentifyFinger)
		pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
		}
		break;
			case ID_ISSIMGQUA:
		if(!g_stMosaicDll.pfnMosaic_IsSupportImageQuality)
		pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
		}
		break;
		case ID_ISSFINQUA:
		if(!g_stMosaicDll.pfnMosaic_IsSupportFingerQuality)
		pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
		}
		break;
			case ID_ISSIMGEN:
		if(!g_stMosaicDll.pfnMosaic_IsSupportImageEnhance)
		pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
		}
		break;
	case ID_IMGQUA:
		if(!g_stMosaicDll.pfnMosaic_IsSupportImageQuality)
			pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
		}
		break;

	case ID_FINQUA:
		if(!g_stMosaicDll.pfnMosaic_IsSupportFingerQuality)
			pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
		}
		break;
	case ID_IMGENH:
		if(!g_stMosaicDll.pfnMosaic_ImageEnhance)
			pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
		}
		break;
	}

}
void CMainFrame::OnStdCheck2Handler(UINT nID)
{
	
		switch(nID)
		{
		case ID_SETPWH:
			m_pTestView->OnSETPWH();
			return;
		case ID_GETSETUPDLG:
			m_pTestView->OnGETSETUPDLG();
			
			return;
			break;
	
		}
	
	
}
void CMainFrame::OnCheckHandler(UINT nID)
{
	
		switch(nID)
		{
		case ID_ISISFIN :
			m_pTestView->OnISISFIN();
			return;
		case ID_ISSIMGQUA :
			m_pTestView->OnISSIMGQUA();
			
			return;
		case ID_ISSFINQUA:
			m_pTestView->OnISSFINQUA();
			return;
		case ID_ISSIMGEN:
			m_pTestView->OnISSIMGEN();
			return;
		case ID_IMGQUA:
			m_pTestView->OnIMGQUA();
			return;
		case ID_FINQUA:
			m_pTestView->OnFINQUA();
			return;
		case ID_IMGENH:
			m_pTestView->OnIMGENH();
			return;
			break;
		}
	
	
}

void CMainFrame::OnStdCheckHandler(UINT nID)
{
	const TCHAR* pszCheckType = NULL;
	UCHAR nStdChkType, bShowBadPt;

	bShowBadPt = 0;

	nStdChkType = GFLS_SCANNER_STDCHKTYPE_NORMAL;
	if(nID >= ID_STDCHK_CAPTSPEED)
	{
		switch(nID)
		{
		case ID_STDCHK_CAPTSPEED:
			m_pTestView->OnStdChkCaptSpeed();
			return;
		case ID_STDCHK_CAPTIMAGESTOSAVE:
			m_pTestView->OnCaptImages2Save();
			nStdChkType = GFLS_SCANNER_STDCHKTYPE_NORMAL;
			break;

		case ID_LOAD_LIVECAPTDLL:
			{
				LoadLiveCaptureDll(this);
				UpdateCaptureBtnStateAfterStopCapt();
//				SetBrightContrastValue();
			}
			break;

		case ID_LOAD_MOSAICDLL:
			{
				LoadMosaicDll(this);
			}
			break;
			
		default:
			return;
		}
	}
	else
	{
		nStdChkType = (UCHAR)(nID - ID_STDCHK_OPTICSDEVICE + GFLS_SCANNER_STDCHKTYPE_OPTICSDEVICE);
		if(nStdChkType != m_pTestView->m_stParamInfo.nStdChkType)
		{
			switch(nID)
			{
			case ID_STDCHK_IMGABERRANCE:
				StopCaptureThread();
				UpdateCaptureBtnStateAfterStopCapt();
				//m_pTestView->ZoomImage2PointedScale(1);
				break;
			case ID_STDCHK_IMGBACKGRAYUNAVE:
				StopCaptureThread();
				UpdateCaptureBtnStateAfterStopCapt();
				pszCheckType = _T("背景灰度不均匀度检测结果:");
				break;
			case ID_STDCHK_IMGCENTERGRAY:
				StopCaptureThread();
				UpdateCaptureBtnStateAfterStopCapt();
				pszCheckType = _T("中心区域灰度值检测结果:");
				break;
			case ID_STDCHK_IMGGRAYVARIAN:
				StopCaptureThread();
				UpdateCaptureBtnStateAfterStopCapt();
				pszCheckType = _T("灰度动态范围检测结果:");
				break;
			case ID_STDCHK_IMGBADPOINT:
				StopCaptureThread();
				UpdateCaptureBtnStateAfterStopCapt();
				pszCheckType = _T("图像疵点检测结果:");
				m_pTestView->ZoomImage2PointedScale(1);
				bShowBadPt = 1;
				break;
			case ID_STDCHK_ENVIRONMENT:
				pszCheckType = _T("环境光检测结果:");
				break;
			case ID_STDCHK_IMGPARSE:
				StopCaptureThread();
				UpdateCaptureBtnStateAfterStopCapt();
				pszCheckType = _T("解析度检测结果:");
				break;
			}
		}
		else
			nStdChkType = GFLS_SCANNER_STDCHKTYPE_NORMAL;
	}

	m_wndStdChkBar.GetDlgItem(IDC_CHECK_SHOWBADPT)->ShowWindow(bShowBadPt ? SW_SHOW : SW_HIDE);
	m_wndStdChkBar.GetDlgItem(IDC_STATIC_STDCHKRESULT)->SetWindowText(pszCheckType);
	m_wndStdChkBar.GetDlgItem(IDC_EDIT_STDCHKRESULT)->SetWindowText(_T(""));

	ShowDialogBarWithStdChkType(nStdChkType);
}

void CMainFrame::StartOrStopStdCheck(UCHAR nStdChkType, const TCHAR* pszCheckType)
{
	if(m_pTestView->m_stParamInfo.nStdChkType == nStdChkType)
	{
		nStdChkType = GFLS_SCANNER_STDCHKTYPE_NORMAL;
	}
	else
	{
		if(nStdChkType != GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT)
		{
			StopCaptureThread();
			UpdateCaptureBtnStateAfterStopCapt();
		}
		if(nStdChkType == GFLS_SCANNER_STDCHKTYPE_ABERRANCE || nStdChkType == GFLS_SCANNER_STDCHKTYPE_BADPOINT)
			m_pTestView->ZoomImage2PointedScale(1);
		if(nStdChkType != GFLS_SCANNER_STDCHKTYPE_BADPOINT)
			m_wndStdChkBar.GetDlgItem(IDC_CHECK_SHOWBADPT)->ShowWindow(SW_HIDE);
		else
			m_wndStdChkBar.GetDlgItem(IDC_CHECK_SHOWBADPT)->ShowWindow(SW_SHOW);
		m_wndStdChkBar.GetDlgItem(IDC_STATIC_STDCHKRESULT)->SetWindowText(pszCheckType);
	}
	m_wndStdChkBar.GetDlgItem(IDC_EDIT_STDCHKRESULT)->SetWindowText(_T(""));

	ShowDialogBarWithStdChkType(nStdChkType);
}

//void CMainFrame::OnStdChkCaptSpeed()
//{
//	if(NULL == m_pTestView)
//		return;
//	if(!IsWindow(m_pTestView->m_dlgImgInfo.GetSafeHwnd()))
//		return;
//	m_pTestView->m_dlgImgInfo.CheckCaptSpeed();
//}

void CMainFrame::CalculateAberParam(LPPOINT ppoint1, LPPOINT ppoint2)
{
	int nAberPos1, nAberPos2, nID;

	nAberPos1 = PointInAberPosValue(ppoint1);
	nAberPos2 = PointInAberPosValue(ppoint2);
	if(nAberPos2 == nAberPos1)
		return;

	nID = 0;
	switch(nAberPos1)
	{
	case ABERPOS_CENTER:
		{
			switch(nAberPos2)
			{
			case ABERPOS_A1:
				nID = IDC_EDIT_A1TOCENTER;
				break;

			case ABERPOS_A2:
				nID = IDC_EDIT_A2TOCENTER;
				break;

			case ABERPOS_A3:
				nID = IDC_EDIT_A3TOCENTER;
				break;

			case ABERPOS_A4:
				nID = IDC_EDIT_A4TOCENTER;
				break;
			}
		}
		break;

	case ABERPOS_A1:
		{
			switch(nAberPos2)
			{
			case ABERPOS_CENTER:
				nID = IDC_EDIT_A1TOCENTER;
				break;

			case ABERPOS_B1:
				nID = IDC_EDIT_B1TOA1DIST;
				break;

			case ABERPOS_B4:
				nID = IDC_EDIT_B4TOA1DIST;
				break;
			}
		}
		break;

	case ABERPOS_A2:
		{
			switch(nAberPos2)
			{
			case ABERPOS_CENTER:
				nID = IDC_EDIT_A2TOCENTER;
				break;

			case ABERPOS_B1:
				nID = IDC_EDIT_B1TOA2DIST;
				break;

			case ABERPOS_B2:
				nID = IDC_EDIT_B2TOA2DIST;
				break;
			}
		}
		break;

	case ABERPOS_A3:
		{
			switch(nAberPos2)
			{
			case ABERPOS_CENTER:
				nID = IDC_EDIT_A3TOCENTER;
				break;

			case ABERPOS_B2:
				nID = IDC_EDIT_B2TOA3DIST;
				break;

			case ABERPOS_B3:
				nID = IDC_EDIT_B3TOA3DIST;
				break;
			}
		}
		break;

	case ABERPOS_A4:
		{
			switch(nAberPos2)
			{
			case ABERPOS_CENTER:
				nID = IDC_EDIT_A4TOCENTER;
				break;

			case ABERPOS_B3:
				nID = IDC_EDIT_B3TOA4DIST;
				break;

			case ABERPOS_B4:
				nID = IDC_EDIT_B4TOA4DIST;
				break;
			}
		}
		break;

	case ABERPOS_B1:
		{
			switch(nAberPos2)
			{
			case ABERPOS_A1:
				nID = IDC_EDIT_B1TOA1DIST;
				break;

			case ABERPOS_A2:
				nID = IDC_EDIT_B1TOA2DIST;
				break;
			}
		}
		break;

	case ABERPOS_B2:
		{
			switch(nAberPos2)
			{
			case ABERPOS_A2:
				nID = IDC_EDIT_B2TOA2DIST;
				break;

			case ABERPOS_A3:
				nID = IDC_EDIT_B2TOA3DIST;
				break;
			}
		}
		break;

	case ABERPOS_B3:
		{
			switch(nAberPos2)
			{
			case ABERPOS_A3:
				nID = IDC_EDIT_B3TOA3DIST;
				break;

			case ABERPOS_A4:
				nID = IDC_EDIT_B3TOA4DIST;
				break;
			}
		}
		break;

	case ABERPOS_B4:
		{
			switch(nAberPos2)
			{
			case ABERPOS_A4:
				nID = IDC_EDIT_B4TOA4DIST;
				break;

			case ABERPOS_A1:
				nID = IDC_EDIT_B4TOA1DIST;
				break;
			}
		}
		break;
	}
	if(nID == 0)
		return;

	double fLength;
	fLength = GetTowPointLength(ppoint1, ppoint2);
	CString strInfo;
	strInfo.Format(_T("%.2f"), fLength);
	m_wndAberDlbBar.GetDlgItem(nID)->SetWindowText(strInfo);
}

inline int CMainFrame::PosIsCenterNear(LPPOINT pptCenter, LPPOINT pptPos, int nDist)
{
	return ((pptPos->x > pptCenter->x - nDist && pptPos->x < pptCenter->x + nDist && pptPos->y > pptCenter->y - nDist && pptPos->y < pptCenter->y + nDist) ? 1 : 0); 
}

int CMainFrame::PointInAberPosValue(LPPOINT ppt)
{
	int nDist  = 8;

	POINT	ptCenter;

	ptCenter.x = 320;	ptCenter.y = 320;	// Center point
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_CENTER;

	ptCenter.x = 620;	ptCenter.y = 320;	// A1
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_A1;

	ptCenter.x = 320;	ptCenter.y = 20;	// A2
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_A2;

	ptCenter.x = 20;	ptCenter.y = 320;	// A3
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_A3;

	ptCenter.x = 320;	ptCenter.y = 620;	// A4
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_A4;

	ptCenter.x = 620;	ptCenter.y = 20;	// B1
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_B1;

	ptCenter.x = 20;	ptCenter.y = 20;	// B2
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_B2;

	ptCenter.x = 20;	ptCenter.y = 620;	// B3
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_B3;

	ptCenter.x = 620;	ptCenter.y = 620;	// B4
	if(PosIsCenterNear(&ptCenter, ppt, nDist))
		return ABERPOS_B4;

	return ABERPOS_UNKNOWN;
}

void CMainFrame::OnUpdateStdChkHandler(CCmdUI* pCmdUI)
{
	if(NULL == m_pTestView)
		return;

	int bCheck = 0;
	UCHAR nStdChkType = (UCHAR)((pCmdUI->m_nID - ID_STDCHK_OPTICSDEVICE) + GFLS_SCANNER_STDCHKTYPE_OPTICSDEVICE);
	if(m_pTestView->m_stParamInfo.nStdChkType == nStdChkType)
		bCheck = 1;
	pCmdUI->SetCheck(bCheck);

/*
	switch(pCmdUI->m_nID)
	{
	case ID_STDCHK_IMGBADPOINT:
		if(m_pTestView->m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_BADPOINT)
			bCheck = 1;
		pCmdUI->SetCheck(bCheck);
		return;

	case ID_STDCHK_IMGABERRANCE:
		if(m_pTestView->m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_ABERRANCE)
			bCheck = 1;
		pCmdUI->SetCheck(bCheck);
		return;

	case ID_STDCHK_IMGPARSE:
		if(m_pTestView->m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_PARSE)
			bCheck = 1;
		pCmdUI->SetCheck(bCheck);
		return;

	case ID_STDCHK_IMGBACKGRAYUNAVE:
		if(m_pTestView->m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYUNAVE)
			bCheck = 1;
		pCmdUI->SetCheck(bCheck);
		return;

	case ID_STDCHK_IMGCENTERGRAY:
		if(m_pTestView->m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_CENTERGRAY)
			bCheck = 1;
		pCmdUI->SetCheck(bCheck);
		return;

	case ID_STDCHK_IMGGRAYVARIAN:
		if(m_pTestView->m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_GRAYVARIA)
			bCheck = 1;
		pCmdUI->SetCheck(bCheck);
		return;

	case ID_STDCHK_ENVIRONMENT:
		if(m_pTestView->m_stParamInfo.nStdChkType == GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT)
			bCheck = 1;
		pCmdUI->SetCheck(bCheck);
		return;
	}
*/
}

void CMainFrame::OnMosaicImageHandler(UINT nID)
{
	switch(nID)
	{
	case ID_MOSAIC_ROLL1FINGER:
		StartFingerCaptureThread(m_pTestView, 1);
		break;
	case ID_MOSAIC_ROLL2FINGER:
		StartFingerCaptureThread(m_pTestView, 2);
		break;
	case ID_MOSAIC_ROLL3FINGER:
		StartFingerCaptureThread(m_pTestView, 3);
		break;

	case ID_MOSAIC_PLAINFINGER:
		StartFingerCaptureThread(m_pTestView, 0);
		break;
	}

	UpdateCaptureBtnStateAfterStopCapt();
}

void CMainFrame::OnUpdateMosaicImageHandler(CCmdUI* pCmdUI)
{
	if(!g_stMosaicDll.bLoaded || !g_stScannerInfo.bInitialed)
	{
		pCmdUI->Enable(0);
		return;
	}

	switch(pCmdUI->m_nID) 
	{
		
	case ID_MOSAIC_ROLL1FINGER:
		if(g_stMosaicDll.nRollMode != 1 && g_stMosaicDll.nRollMode != 3)
			pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
			pCmdUI->SetCheck(g_stScannerInfo.bHasMosaicThrd && g_stMosaicDll.bMosaicRoll==1);
		}
		break;
			case ID_MOSAIC_ROLL2FINGER:
		if(g_stMosaicDll.nRollMode != 2 && g_stMosaicDll.nRollMode != 3)
			pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
			pCmdUI->SetCheck(g_stScannerInfo.bHasMosaicThrd && g_stMosaicDll.bMosaicRoll==2);
		}
		break;
		case ID_MOSAIC_ROLL3FINGER:
		if(g_stMosaicDll.nRollMode != 3)
			pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
			pCmdUI->SetCheck(g_stScannerInfo.bHasMosaicThrd && g_stMosaicDll.bMosaicRoll==3);
		}
		break;

	case ID_MOSAIC_PLAINFINGER:
		if(g_stMosaicDll.nRollMode != 3)
			pCmdUI->Enable(0);
		else
		{
			pCmdUI->Enable(1);
			pCmdUI->SetCheck(g_stScannerInfo.bHasMosaicThrd && g_stMosaicDll.bMosaicRoll==0);
		}
		break;
	}
}

void CMainFrame::OnBtnCalcImgAber()
{
	double fDenominator, fValue, fAberrance[12], fMaxAber;
	int i, nret, nID;
	TCHAR szValue[256] = { 0 };
//	float	fValue;

	fDenominator = 300.0;
	fMaxAber = 0.0;
	memset(fAberrance, 0, sizeof(fAberrance));
	nret = 1;

	nID = IDC_EDIT_A1TOCENTER;
	for(i = 0; i < 12; i++, nID++)
	{
		memset(szValue, 0, sizeof(szValue));
		m_wndAberDlbBar.GetDlgItem(nID)->GetWindowText(szValue, sizeof(szValue) - 1);
//		fValue = 0.0;
//		sscanf(szValue, "%f", &fValue);
		fValue = _ttof(szValue);
		if(fValue < 1.0)
		{
			nret = -1;
			break;
		}
		fAberrance[i] = fabs(fValue - fDenominator) / fDenominator * 100.0;
		if(fMaxAber < fAberrance[i])
			fMaxAber = fAberrance[i];
	}

	if(nret > 0)
	{
		nret = 0;
		for(i = 0; i < 12; i++)
			nret += _stprintf(szValue + nret, _T("D%d=%.3f%% "), i, fAberrance[i]);
//		sprintf(szValue, "%.3f%%", fAberrance);
		m_wndAberDlbBar.GetDlgItem(IDC_EDIT_ABRRANCEVALUE)->SetWindowText(szValue);
		return;
	}

	if(i < 4)
		_stprintf(szValue, _T("请首先计算A%d到样板中心点的距离!"), i + 1);
	else
	{
		int nb, na;
		if(i < 6)
		{
			nb = 1;
			na = (i == 4) ? 1 : 2;
		}
		else if(i < 8)
		{
			nb = 2;
			na = (i == 6) ? 2 : 3;
		}
		else if(i < 10)
		{
			nb = 3;
			na = (i == 8) ? 3 : 4;
		}
		else
		{
			nb = 4;
			na = (i == 10) ? 4 : 1;
		}
		_stprintf(szValue, _T("请首先计算B%d到A%d的距离!"), nb, na);
	}
	ShowTestAppErrorInfo(szValue, m_pTestView);
//	GetDlgItem(nID)
}

void CMainFrame::ClearImageAberParamInfo()
{
	int i, nID;
	CWnd* pWnd;

	nID = IDC_EDIT_A1TOCENTER;
	for(i = 0; i < 13; i++, nID++)
	{
		pWnd = m_wndAberDlbBar.GetDlgItem(nID);
		if(!pWnd || !IsWindow(pWnd->GetSafeHwnd()))
			continue;
		pWnd->SetWindowText(_T(""));
	}
}

void CMainFrame::OnCheckShowBadPoint()
{
	if(m_pTestView->m_stParamInfo.bDrawBadPoint)
		m_pTestView->m_stParamInfo.bDrawBadPoint = 0;
	else
		m_pTestView->m_stParamInfo.bDrawBadPoint = 1;
	m_pTestView->Invalidate(FALSE);
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	theApp.m_bCloseFrame = 1;
	
	CFrameWnd::OnClose();
}

void CMainFrame::OnBtnPhydistance()
{
	CDlgPhyDistance phyDistDlg(NULL, &m_nPhyDistance);
	phyDistDlg.DoModal();
}

void CMainFrame::OnBtnGafisTest()
{
	LoadLiveCaptureDll(this, 1);
	UpdateCaptureBtnStateAfterStopCapt();
	InitialLiveCapture(this);
	OnCheckStartcapture() ;
}

void CMainFrame::OnUpdateDlgBarSerialNumCtrls(CCmdUI* pCmdUI)
{
// 	BOOL bCheck = FALSE;
// 	if(g_stLiveCaptDll.pfnLiveScan_SetSerialNum 
// 		&& g_stLiveCaptDll.pfnLiveScan_GetSerialNum
// 		&& g_stScannerInfo.bInitialed)
// 		bCheck = TRUE;
// 	switch(pCmdUI->m_nID)
// 	{
// 	case IDC_EDIT_SN:
// 	case IDC_BTN_SETSN:
// 	case IDC_BTN_GETSN:
// 		pCmdUI->Enable(bCheck);
// 		break;
// 	}
	//add by wangkui
	BOOL bCheck = FALSE;
	if(g_stScannerInfo.bInitialed)
		bCheck = TRUE;
	switch(pCmdUI->m_nID)
	{
	case IDC_EDIT_SN:
	case IDC_BTN_SETSN:
		{
			if (g_stCaptDll.pfnScan_SetSerialNum)
			{
				pCmdUI->Enable(bCheck);
			}
		}
	case IDC_BTN_GETSN:
		{
			if (g_stCaptDll.pfnScan_GetSerialNum)
			{
				pCmdUI->Enable(bCheck);
			}
		}
		break;
	}
}

void CMainFrame::OnBtnSetSerialNumClick()
{
	CString strSetNum, strGetNum, strInfo;
	unsigned int nNum = 0;
	if(!g_stCaptDll.pfnScan_SetSerialNum || !g_stCaptDll.pfnScan_GetSerialNum)
	{
		MessageBox(_T("链接动态库不支持此功能。"), _T("活体采集测试程序"), MB_OK);
		return;
	}
	m_wndDlgBar.GetDlgItem(IDC_EDIT_SN)->GetWindowText(strSetNum);
	
	_stscanf(strSetNum.GetString(), _T("%d"), &nNum);
	if(nNum < 100000001 || nNum > 3999999999)
	{
		MessageBox(_T("无效的序号值！"), _T("活体采集测试程序"), MB_OK);
		return;
	}
// 	if(nNum <= 0 || nNum > 60000)
// 	{
// 		MessageBox("无效的序号值！", "活体采集测试程序", MB_OK);
// 		return;
// 	}
	if(g_stCaptDll.pfnScan_SetSerialNum(nNum) != TRUE)
	{
		MessageBox(_T("写入失败！"), _T("活体采集测试程序"), MB_OK);
		return;
	}
	nNum = 0;
	if(g_stCaptDll.pfnScan_GetSerialNum(&nNum) != TRUE)
	{
		MessageBox(_T("读取失败！"), _T("活体采集测试程序"), MB_OK);
		return;
	}
	strGetNum.Format(_T("%u"), nNum);
	m_wndDlgBar.GetDlgItem(IDC_EDIT_SN)->SetWindowText(strGetNum);
	
	if (strSetNum.CompareNoCase(strGetNum) != 0)
	{
		strInfo = _T("写入的值和读出的值不一致!\n写入值为:") + strSetNum + _T("\n读出的值为:") + strGetNum;
		MessageBox(strInfo.GetString(), _T("活体采集测试程序"), MB_OK);
		return;
	}
	MessageBox(_T("写入成功！"), _T("活体采集测试程序"), MB_OK);
}

void CMainFrame::OnBtnGetSerialNumClick()
{
	CString strGetNum;
	unsigned int nNum = 0;
	if(!g_stCaptDll.pfnScan_GetSerialNum)
	{
		MessageBox(_T("链接动态库不支持此功能。"), _T("活体采集测试程序"), MB_OK);
		return;
	}
	if(g_stCaptDll.pfnScan_GetSerialNum(&nNum) != TRUE)
	{
		MessageBox(_T("读取失败！"), _T("活体采集测试程序"), MB_OK);
		return;
	}
	strGetNum.Format(_T("%u"), nNum);
	m_wndDlgBar.GetDlgItem(IDC_EDIT_SN)->SetWindowText(strGetNum);
}