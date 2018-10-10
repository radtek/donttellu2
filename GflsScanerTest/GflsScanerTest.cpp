// GflsScanerTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "GfDlgBarEx.h"
#include "MainFrm.h"
#include "GflsScanerTestDoc.h"
#include "GflsScanerTestView.h"
#include <sys/stat.h>
#include <direct.h>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include <errno.h>

int reType = RELEASELIVETYPE_FINGER_PALM_SCAN;
//RELEASELIVETYPE_FINGER_SCAN 代表编译出的版本仅是支持指纹识别
//RELEASELIVETYPE_PALM_SCAN 代表编译出的版本仅是支持掌纹识别
//RELEASELIVETYPE_FINGER_PALM_SCAN 代表编译出的版本是支持指纹和掌纹识别的
//RELEASELIVETYPE_FINGER_GUIZHOU 代表贵州串口的版本

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestApp

BEGIN_MESSAGE_MAP(CGflsScanerTestApp, CWinApp)
	//{{AFX_MSG_MAP(CGflsScanerTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestApp construction

CGflsScanerTestApp::CGflsScanerTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	memset(&m_stbmpInfo, 0, sizeof(m_stbmpInfo));
	m_bCloseFrame = 0;
	m_bHaveImageInfo = 0;
	m_pFrameWnd = 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGflsScanerTestApp object

CGflsScanerTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestApp initialization

BOOL CGflsScanerTestApp::InitInstance()
{
	ttt = 1;
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	InitialScannerTestInfo();

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

//	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGflsScanerTestDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGflsScanerTestView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CGflsScanerTestApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	Gfls_Thread_Finish();
	
	return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CGflsScanerTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CGflsScanerTestApp message handlers

void CGflsScanerTestApp::InitialScannerTestInfo()
{
	UCHAR nValue = 0;
	RGBQUAD *pRgbQuad = m_stbmpInfo.bmiColors;
	for(int i = 0; i < 256; i++, pRgbQuad++, nValue++)
	{
		pRgbQuad->rgbBlue  = nValue;
		pRgbQuad->rgbGreen = nValue;
		pRgbQuad->rgbRed   = nValue;
	}

	BITMAPINFOHEADER* pHead = &m_stbmpInfo.bmiHeader;
	pHead->biSize			= sizeof(BITMAPINFOHEADER);
	pHead->biPlanes			= 1;
	pHead->biCompression	= BI_RGB;
	pHead->biBitCount		= 8;

	memset(&g_stScannerInfo, 0, sizeof(g_stScannerInfo));
	if (reType == 4)
	{
		memset(&g_stLiveCaptDll, 0, sizeof(g_stLiveCaptDll));
	}
	else
	{
		memset(&g_stCaptDll, 0, sizeof(g_stCaptDll));
	}
	memset(&g_stMosaicDll,   0, sizeof(g_stMosaicDll));
	memset(&g_stCaptMosiac,  0, sizeof(g_stCaptMosiac));

	Gfls_Thread_InitParam();
}


void Gf_SaveData2BitmapFile(UCHAR* pData, int nw, int nh, const char* pszFileName)
{
	FILE  *fp;
	UCHAR bnDummy[4] = {255};

	// BITMAPFILEHEADER
	WORD  bfType = 0x4d42;	//'BM'
	DWORD bfSize, bfReserved, bfOffBits;

	int nActWidth, nVirtWidth, nsize, i;

	nActWidth  = nw;
	nVirtWidth = (nActWidth + 3) / 4 * 4;

	bfReserved = 0;
	bfOffBits  = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	fp = fopen(pszFileName, "wb");
	if(NULL == fp)
		return ;

	bfOffBits += sizeof(theApp.m_stbmpInfo.bmiColors);
	bfSize = nVirtWidth * nh + bfOffBits;

	fwrite(&bfType,		2, 1, fp);
	fwrite(&bfSize,		4, 1, fp);
	fwrite(&bfReserved,	4, 1, fp);
	fwrite(&bfOffBits,	4, 1, fp);

	fwrite(&theApp.m_stbmpInfo.bmiHeader,  sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(theApp.m_stbmpInfo.bmiColors,   sizeof(RGBQUAD), 256, fp);

	nsize = nw * nh;
	if(nActWidth == nVirtWidth)
		fwrite(pData, 1, nsize, fp);
	else
	{
		for(i = 0; i < nh; i++)
		{
			fwrite(pData + i * nActWidth, 1, nActWidth, fp);
			fwrite(bnDummy, 1, nVirtWidth - nActWidth, fp);
		}
	}

	fclose(fp);
}

void EnlargeOrShinkImage(UCHAR *pSrcData, int nSrcWidth, int nSrcHeight, UCHAR *pDestData, int nDestWidth, int nDestHeight)
{
	double	fWidth, fHeight, *pfWidth, ftemp, fDiff, yDouble;
	int		*piWidth, i, j, yInt;
	int		f11, f12, f21, f22;
	UCHAR	*pTemp, *pOffPos, *ptmpDest;

	fWidth  = (double)(nSrcWidth) / (double)nDestWidth;
	fHeight = 0.0;
	pfWidth = (double*)malloc(nDestWidth * sizeof(double));
	if(NULL == pfWidth)
		return;
	piWidth = (int*)malloc(nDestWidth * sizeof(int));
	if(NULL == piWidth)
	{
		free(pfWidth);
		return;
	}

	ptmpDest = pDestData;

	ftemp = 0.0;
	for(i = 0; i < nDestWidth; i++, ftemp += fWidth)
	{
		piWidth[i] = (int)ftemp;
		pfWidth[i] = ftemp - piWidth[i];
	}
	ftemp = 0.0;
	for(j = 0; j < nDestHeight; j++, ftemp += fWidth)
	{
		yInt = (int)ftemp;
		yDouble = ftemp - yInt;
		pTemp = pSrcData + yInt * nSrcWidth;
		if(yInt >= nSrcHeight - 1)
			break;
		for(i = 0; i < nDestWidth; i++, ptmpDest++)
		{
			pOffPos = pTemp + piWidth[i];
			f11 = *pOffPos;
			f12 = *(pOffPos + 1);
			f21 = *(pOffPos + nSrcWidth);
			f22 = *(pOffPos + nSrcWidth + 1);
			fDiff = 1.0 - pfWidth[i];
			*ptmpDest = (UCHAR)(yDouble * (fDiff * f21 + pfWidth[i] * f22) + (1.0 - yDouble) * (fDiff * f11 + pfWidth[i] * f12));
		}
	}
	i = j - 1;
	for(; j < nDestHeight; j++)
		memcpy(pDestData + j * nDestWidth, pDestData + i * nDestWidth, nDestWidth);

	free(pfWidth);
	free(piWidth);
}

void ZoomImageByScale(UCHAR *pSrcData, int nSrcWidth, int nSrcHeight, UCHAR *pDesData, int nScale)
{
	int row, col, nwDes, ntmp, j;
	UCHAR *pSrcTemp, *pDesTemp, *pDesRow;

	nwDes    = nSrcWidth * nScale;
	ntmp	 = nwDes * nScale;
	pSrcTemp = pSrcData;
	pDesTemp = pDesData;
	pDesRow  = pDesData;
	for(row = 0; row < nSrcHeight; row++, pDesRow += ntmp)
	{
		for(col = 0; col < nSrcWidth; col++, pSrcTemp++)
		{
			for(j = 0; j < nScale; j++, pDesTemp++)
				*pDesTemp = *pSrcTemp;
		}
		for(j = 1; j < nScale; j++, pDesTemp += nwDes)
			memcpy(pDesTemp, pDesRow, nwDes);
	}
}

GF_SCANNERINFO	g_stScannerInfo;

void StopCaptureThread()
{
	g_stScannerInfo.bExitCapture = 1;
	g_stScannerInfo.bExitMosaic  = 1;
	Gfls_Thread_ReleaseSemaphore();

	BOOL bRet;
	MSG msg = {0};
	HANDLE hThread[3];
	hThread[0] = g_stScannerInfo.hCaptThread;
	hThread[1] = g_stCaptMosiac.hCaptureThread;
	hThread[2] = g_stCaptMosiac.hMosaicThread;

	if(theApp.m_bCloseFrame)
	{
		for(int i = 0; i < 3; i++)
		{
			if(NULL == hThread[i])
				continue;
			if(WaitForSingleObject(hThread[i], 1000) == WAIT_TIMEOUT)
				TerminateThread(hThread[i], 0);
		}
	}
	else
	{
		UCHAR bSignal[3] = {0};
		while((bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
		{ 
			if (bRet == -1)
			{
				// handle the error and possibly exit
				break;
			}
			else
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
			for(int i = 0; i < 3; i++)
			{
				if(NULL == hThread[i])
					bSignal[i] = 1;
				else if(WaitForSingleObject(hThread[i], 0) != WAIT_TIMEOUT)
					bSignal[i] = 1;
			}
			if(bSignal[0] && bSignal[1] && bSignal[2])
				break;
		}
	}


	for (int i = 0; i < 3; i++)
	{
		if (hThread[i])
			CloseHandle(hThread[i]);
	}
	g_stScannerInfo.hCaptThread   = NULL;
	g_stCaptMosiac.hCaptureThread = NULL;
	g_stCaptMosiac.hMosaicThread  = NULL;

	g_stScannerInfo.bExitCapture = 0;
	g_stScannerInfo.bExitMosaic  = 0;
	g_stScannerInfo.bHasMosaicThrd = 0;
}

int  GetBitmapFileName(char* pszFileName, int ncbFileName, BOOL bOpenFile, CWnd* pParentWnd)
{
	const TCHAR* pszFilter = _T("位图文件(*.bmp)|*.bmp|RAW文件(*.raw)|*.raw||");
	CFileDialog dlg(bOpenFile, _T("bmp"), NULL, OFN_OVERWRITEPROMPT, pszFilter, pParentWnd);
	if(dlg.DoModal() != IDOK)
		return 0;

#ifdef UNICODE
	ATL::CW2A pstr(dlg.GetPathName().GetString());
	strncpy(pszFileName, (LPSTR)pstr, ncbFileName - 1);
#else
	_tcsncpy(pszFileName, (LPCTSTR)dlg.GetPathName(), ncbFileName - 1);
#endif
	return 1;	
}

int	ShowScannerErrorInfo(int nError, CWnd* pParentWnd, UCHAR bLiveCapt)
{
	int n;
	CStringA temp, temp1;
	CFile mFile;
	CFileException e;
	char strFileName[512] = { 0 };
	strcpy(strFileName, "caiji.log");
	char szErrorInfo[256] = { 0 };
	if (bLiveCapt)
	{
		if (reType == 4)
		{
			n = g_stLiveCaptDll.pfnLiveScan_GetErrorInfo(nError, szErrorInfo);
		}
		else
		{
			n = g_stCaptDll.pfnScan_GetErrorInfo(nError, szErrorInfo);
		}

		temp1.Format("%d", n);
		if (n == 1)
		{
			temp += "测试\"4.19取得采集接口错误信息\"成功 返回值=  " + temp1 + "\r\n";
		}
		else
		{
			temp += "测试\"4.19取得采集接口错误信息\"失败 返回值= " + temp1 + "\r\n";
		}
	}
	else
	{
		n = g_stMosaicDll.pfnMosaic_GetErrorInfo(nError, szErrorInfo);
		temp1.Format("%d", n);
		if (n == 1)
		{
			temp += "测试\"5.16取得拼接接口错误信息\"成功 返回值=" + temp1 + "\r\n";
		}
		else
		{
			temp += "测试\"5.16取得拼接接口错误信息\"失败 返回值=" + temp1 + "\r\n";
		}
	}
	PutInfo2Log(strFileName, temp);
	//	CFileStatus status;
	//	if(mFile.GetStatus("pinjie.log",status))
	//	{
	//		mFile.Open("pinjie.log", CFile::modeWrite,&e);
	//		mFile.SeekToEnd();
	//		dwPos = mFile.GetPosition();
	//		mFile.LockRange(dwPos, 1);
	//		mFile.Write(temp,temp.GetLength());
	//		mFile.UnlockRange(dwPos,1);
	//		mFile.Flush();
	//		mFile.Close();  
	//	}
#ifdef UNICODE
	{
		ATL::CA2W	pwstr(szErrorInfo);
		return ShowTestAppErrorInfo((LPWSTR)pwstr, pParentWnd);
	}
#else
	return ShowTestAppErrorInfo(szErrorInfo, pParentWnd);
#endif
	
}

int  ShowTestAppErrorInfo(const TCHAR* pszErrorInfo, CWnd* pParentWnd)
{
	return pParentWnd->MessageBox(pszErrorInfo, GFLS_SCANNER_TESTTITLE);
}

int	GflsTest_DirCreateRecur(char *pathname, int flag)
{
	char	c;
	int		i;

	i = 0;
	if ( flag == 0 ) 
	{
		flag = _S_IREAD | _S_IWRITE;
	}

	while(pathname[i] != '\0' )
	{
		if ( pathname[i] == ':' )
			break;	
		else
			i++;
	}
	if ( pathname[i] == '\0' )
	{
		/* no drive letter */
		i = 0;
	}
	else
	{
		i++;
	}

	if ( pathname[i] == '\\' )
		i++;

	do
	{
		while(pathname[i] != '/' && pathname[i] != '\\' && pathname[i] != '\0')
			i++;
		c = pathname[i];
		pathname[i] = '\0';
		if ( !GFLSTEST_FILEEXIST(pathname) ) 
		{
			if ( _mkdir(pathname) < 0 && errno != EEXIST )
			{
				// we don't just set error, check
				if ( !GFLSTEST_FILEEXIST(pathname) ) 
				{
					// still not exist, create another time
					if ( _mkdir(pathname) < 0 && errno != EEXIST )
					{
						// fatal error create by another time
						pathname[i] = c;
						return	-1;
					}
				} // else path exist
			}
		}
		pathname[i] = c;
		i++;
	} while(c != '\0');

	return	1;
}

inline double GetTowPointLength(LPPOINT ppt1, LPPOINT ppt2)
{
	double fLine = 0.0, ftmp;

	ftmp = (double)(ppt1->x - ppt2->x);
	fLine = ftmp * ftmp;
	ftmp = (double)(ppt1->y - ppt2->y);
	fLine += ftmp * ftmp;
	fLine = sqrt(fLine);

	return fLine;
}

int GetImageGrayStatisticInfo(GF_IMAGEGRAYSTATSTRUCT *pstGrayStat)
{
	if(NULL == pstGrayStat || NULL == pstGrayStat->pImageData)
		return 0;
	

	int	nGrayCount[256] = {0};
	int i, ncol, nIndex, nwArea, nhArea, *pnGrayCnt;
	UCHAR *pTmpData;
	CRect rtArea, rtAll;

	rtAll.SetRect(0, 0, pstGrayStat->nw, pstGrayStat->nh);
	rtArea.IntersectRect(rtAll, &pstGrayStat->rtArea);

	nwArea = rtArea.Width();	nhArea = rtArea.Height();
	pTmpData = pstGrayStat->pImageData + rtArea.top * pstGrayStat->nw + rtArea.left;
	pstGrayStat->nPixelCount = 0;
	pstGrayStat->nTotalGrayCount = 0;
	for(i = 0; i < nhArea; i++, pTmpData += pstGrayStat->nw)
	{
		for(ncol = 0; ncol < nwArea; ncol++, pstGrayStat->nPixelCount++)
		{
			nIndex = (int)pTmpData[ncol];
			nGrayCount[nIndex]++;
			pstGrayStat->nTotalGrayCount += nIndex;
		}
	}
	pstGrayStat->fAverage = (double)pstGrayStat->nTotalGrayCount / (double)pstGrayStat->nPixelCount;

	pnGrayCnt = nGrayCount;
	pstGrayStat->nMaxCntGrayValue = 0;
	pstGrayStat->nMaxCntGrayCount = 0;
	pstGrayStat->nMinCntGrayValue = 0;
	pstGrayStat->nMinCntGrayCount = 0;
	for(i = 0; i < 256; i++, pnGrayCnt++)
	{
		if(0 == *pnGrayCnt)
			continue;
		if(pstGrayStat->nMaxCntGrayCount < *pnGrayCnt)
		{
			pstGrayStat->nMaxCntGrayCount = *pnGrayCnt;
			pstGrayStat->nMaxCntGrayValue = i;
		}
		if(pstGrayStat->nMinCntGrayCount == 0)
		{
			pstGrayStat->nMinCntGrayCount = *pnGrayCnt;
			pstGrayStat->nMinCntGrayValue = i;
		}
		else if(pstGrayStat->nMinCntGrayCount > *pnGrayCnt)
		{
			pstGrayStat->nMinCntGrayCount = *pnGrayCnt;
			pstGrayStat->nMinCntGrayValue = i;
		}
	}

	pstGrayStat->nMinGrayValue = pstGrayStat->nMaxGrayValue = 0;
	pnGrayCnt = nGrayCount;
	for(i = 0; i < 256; i++, pnGrayCnt++)
	{
		if(0 == *pnGrayCnt)
			continue;
		pstGrayStat->nMinGrayValue = i;
		break;
	}
	pnGrayCnt = nGrayCount + 255;
	for(i = 255; i >= 0; i--, pnGrayCnt--)
	{
		if(0 == *pnGrayCnt)
			continue;
		pstGrayStat->nMaxGrayValue = i;
		break;
	}
	pstGrayStat->fMedian = (double)(pstGrayStat->nMinGrayValue + pstGrayStat->nMaxGrayValue) / 2.0;

	double fDiff, fValue;
	pnGrayCnt = nGrayCount;
	fValue = 0.0;
	for(i = 0; i < 256; i++, pnGrayCnt++)
	{
		fDiff = i - pstGrayStat->fAverage;
		fValue += (fDiff * fDiff * (*pnGrayCnt));
	}
	pstGrayStat->fDiff = sqrt(fValue / (double)pstGrayStat->nPixelCount);

	if(pstGrayStat->pnGrayValueCnt)
		memcpy(pstGrayStat->pnGrayValueCnt, nGrayCount, sizeof(nGrayCount));

	return 1;
}

int GetImageAreaLineGray(GF_IMAGEGRAYSTATSTRUCT *pstGrayStat, double *pfBackMedian, double *pfWhiteMedian)
{
	if(!GetImageGrayStatisticInfo(pstGrayStat))
		return 0;

	int *pnGrayCnt, nAverage, nTotalGrayValue, i, nWhitePixel, nBackPixel, nDiffPix;
	nAverage = (int)pstGrayStat->fAverage;

	pnGrayCnt = pstGrayStat->pnGrayValueCnt;
	nWhitePixel = nBackPixel = 0;	nTotalGrayValue = 0;
	for(i = 0; i < nAverage + 1; i++, pnGrayCnt++)
	{
		nBackPixel += *pnGrayCnt;
		nTotalGrayValue += (*pnGrayCnt) * i;
	}
	if(nBackPixel > 0)
		*pfBackMedian = (double)nTotalGrayValue / (double)pstGrayStat->nPixelCount * 2.0;	//	nBackPixel;
	else
		*pfBackMedian = pstGrayStat->fAverage;	// 0.0;

	pnGrayCnt = pstGrayStat->pnGrayValueCnt + nAverage + 1;
	nTotalGrayValue = 0;
	for(i = nAverage + 1; i < 256; i++, pnGrayCnt++)
	{
		nWhitePixel += *pnGrayCnt;
		nTotalGrayValue += (*pnGrayCnt) * i;
	}
	if(nWhitePixel > 0)
		*pfWhiteMedian = (double)nTotalGrayValue / (double)pstGrayStat->nPixelCount * 2.0;	// nWhitePixel;
	else
		*pfWhiteMedian = pstGrayStat->fAverage;	// 0.0;

	nDiffPix = abs(nWhitePixel - nBackPixel);
	if(nDiffPix * 4 > nWhitePixel + nBackPixel)	// 有问题
	{
		*pfWhiteMedian = *pfBackMedian = pstGrayStat->fAverage;
	}

#if 0
	int *pnGrayCnt, i, nMin, nMax, nAverage;
	
	pnGrayCnt = pstGrayStat->pnGrayValueCnt;
	nMin = nMax = 0;
	nAverage = (int)pstGrayStat->fAverage;
	for(i = 0; i < nAverage + 1; i++, pnGrayCnt++)
	{
		if(0 == *pnGrayCnt)
			continue;
		nMin = i;
		break;
	}
	pnGrayCnt = pstGrayStat->pnGrayValueCnt + nAverage;
	for(i = nAverage; i >= 0; i--, pnGrayCnt--)
	{
		if(0 == *pnGrayCnt)
			continue;
		nMax = i;
	}
	*pfBackMedian = (double)(nMax + nMin) / 2.0;

	pnGrayCnt = pstGrayStat->pnGrayValueCnt + nAverage + 1;
	nMin = nMax = 255;
	for(i = nAverage + 1; i < 256; i++, pnGrayCnt++)
	{
		if(0 == *pnGrayCnt)
			continue;
		nMin = i;
		break;
	}
	pnGrayCnt = pstGrayStat->pnGrayValueCnt + 255;
	for(i = 255; i > nAverage; i--, pnGrayCnt--)
	{
		if(0 == *pnGrayCnt)
			continue;
		nMax = i;
	}
	*pfWhiteMedian = (double)(nMax + nMin) / 2.0;
#endif
	
	return 1;
}

int	ZoomPointFromOld2New(RECT *prtArea, SIZE *psize, POINT *ppt, int nOldScale, int nNewScale)
{
	if(nOldScale == nNewScale)
		return 0;
	
	if(ppt)
	{
		ppt->x = (ppt->x * nNewScale / nOldScale);
		ppt->y = (ppt->y * nNewScale / nOldScale);
	}
	if(prtArea)
	{
		prtArea->left	= prtArea->left * nNewScale / nOldScale;
		prtArea->right	= prtArea->right * nNewScale / nOldScale;
		prtArea->top	= prtArea->top * nNewScale / nOldScale;
		prtArea->bottom	= prtArea->bottom * nNewScale / nOldScale;
	}
	if(psize)
	{
		psize->cx = psize->cx * nNewScale / nOldScale;
		psize->cy = psize->cy * nNewScale / nOldScale;
	}
	return 1;
}
void ReverseImageData(BYTE* pData, int nw, int nh)
{
	BYTE *pSwap = NULL;
	int i = 0, j = 0;
	for(i = 0; i < nh; i++)
	{
		for(j = 0; j < nw; j++)
		{
			pSwap = pData + nw * i + j;
			*(pSwap) = (BYTE)(255 - *(pSwap));
		}
	}
}

int	ZoomPointByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg)
{
	if(nScale == 1)
		return 0;
	
	if(bEnlarg)
	{
		if(prtArea)
		{
			prtArea->left	*= nScale;
			prtArea->right	*= nScale;
			prtArea->top	*= nScale;
			prtArea->bottom	*= nScale;
		}
		if(psize)
		{
			psize->cx *= nScale;
			psize->cy *= nScale;
		}
		if(ppt)
		{
			ppt->x *= nScale;
			ppt->y *= nScale;
		}
	}
	else
	{
		if(prtArea)
		{
			prtArea->left	/= nScale;
			prtArea->right	/= nScale;
			prtArea->top	/= nScale;
			prtArea->bottom	/= nScale;
		}
		if(psize)
		{
			psize->cx /= nScale;
			psize->cy /= nScale;
		}
		if(ppt)
		{
			ppt->x /= nScale;
			ppt->y /= nScale;
		}
	}
	return 1;
}
int	ZoomPointYByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg)
{
	if(nScale == 1)
		return 0;
	
	if(bEnlarg)
	{
		if(prtArea)
		{
			prtArea->left	*= nScale;
			prtArea->right	*= nScale;
			prtArea->top	*= nScale;
			prtArea->bottom	*= nScale;
		}
		if(psize)
		{
			psize->cx *= nScale;
			psize->cy *= nScale;
		}
		if(ppt)
		{
		//	ppt->x *= nScale;
			ppt->y *= nScale;
		}
	}
	else
	{
		if(prtArea)
		{
			prtArea->left	/= nScale;
			prtArea->right	/= nScale;
			prtArea->top	/= nScale;
			prtArea->bottom	/= nScale;
		}
		if(psize)
		{
			psize->cx /= nScale;
			psize->cy /= nScale;
		}
		if(ppt)
		{
			ppt->x /= nScale;
			ppt->y /= nScale;
		}
	}
	return 1;
}
int	ZoomPointXByScale(RECT *prtArea, SIZE *psize, POINT *ppt, int nScale, UCHAR bEnlarg)
{
	if(nScale == 1)
		return 0;
	
	if(bEnlarg)
	{
		if(prtArea)
		{
			prtArea->left	*= nScale;
			prtArea->right	*= nScale;
			prtArea->top	*= nScale;
			prtArea->bottom	*= nScale;
		}
		if(psize)
		{
			psize->cx *= nScale;
			psize->cy *= nScale;
		}
		if(ppt->x)
		{
			ppt->x *= nScale;
		//	ppt->y *= nScale;
		}
	}
	else
	{
		if(prtArea)
		{
			prtArea->left	/= nScale;
			prtArea->right	/= nScale;
			prtArea->top	/= nScale;
			prtArea->bottom	/= nScale;
		}
		if(psize)
		{
			psize->cx /= nScale;
			psize->cy /= nScale;
		}
		if(ppt)
		{
			ppt->x /= nScale;
			ppt->y /= nScale;
		}
	}
	return 1;
}
//int	Gfls_PointCompare(const void* pelem1, const void* pelem2)
//{
//	POINT *pt1, *pt2;
//	pt1 = (POINT*)pelem1;
//	pt2 = (POINT*)pelem2;
//
//	if(pt1->x <  pt2->x)
//}

//#if reType == 4
	GFLS_LIVECAPTAPIHANDLER	g_stLiveCaptDll;
//#else
	GFLS_CAPT_API_HANDLER	g_stCaptDll;
//#endif


CString GetModulePath()
{
	CString strModulePath;
	GetModuleFileName(NULL, strModulePath.GetBuffer(MAX_PATH), MAX_PATH);
	strModulePath.ReleaseBuffer();
	int intpoint = strModulePath.ReverseFind('\\');
	strModulePath = strModulePath.Left(intpoint);
	TCHAR ch = strModulePath.GetAt(intpoint - 1);
	if(ch != '\\') 
		strModulePath +="\\"; 
	return strModulePath;
}

int LoadLiveCaptureDll(CWnd* pParentWnd, bool bIsGafisTest)
{
	DownLiveCaptureDll();
    CStringA temp,temp1,temp2,temp3;
	CFile mFile,mFile1;
	CFileException ex;
    char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
	BOOL bRet;
	DWORD dwPos;

	char szInfo[1024] = {0}, szFileName[512] = {0};
	int nret = -1;	
	if ( !bIsGafisTest)
	{
		if (GFLS_EDZCS_DEF)
		{
#if reType == 4
				CFileDialog dlg(TRUE, _T("dll"), _T("ID_FprCap*"), OFN_OVERWRITEPROMPT, _T("采集DLL(*.dll)|*.dll||"), pParentWnd);
#else
				CFileDialog dlg(TRUE, _T("dll"), _T(""), OFN_OVERWRITEPROMPT, _T("采集DLL(*.dll)|*.dll||"), pParentWnd);
#endif
			if(dlg.DoModal() != IDOK)
				return 0;		
#ifdef UNICODE
			ATL::CW2A pstr(dlg.GetPathName().GetString());
			strncpy(szFileName, (LPSTR)pstr, sizeof(szFileName)-1);
#else
			_tcsncpy(szFileName, (LPCTSTR)dlg.GetPathName(), sizeof(szFileName) - 1);
#endif
		}
		else
		{
#if reType == 4
			CFileDialog dlg(TRUE, "dll", "palm*", OFN_OVERWRITEPROMPT, "采集DLL(*.dll)|*.dll||", pParentWnd);
#else
			CFileDialog dlg(TRUE, _T("dll"), _T("PALM*"), OFN_OVERWRITEPROMPT, _T("采集DLL(*.dll)|*.dll||"), pParentWnd);
#endif
			if(dlg.DoModal() != IDOK)
				return 0;			
#ifdef UNICODE
			ATL::CW2A pstr(dlg.GetPathName().GetString());
			strncpy(szFileName, (LPSTR)pstr, sizeof(szFileName)-1);
#else
			_tcsncpy(szFileName, (LPCTSTR)dlg.GetPathName(), sizeof(szFileName)-1);
#endif
		}

	} 
	else
	{
		int nret = -1;	
		CString FilePath;
		FilePath = GetModulePath();
#ifdef _DEBUG
		FilePath +="gals1908_d.dll";
#else
		FilePath +="gals1908.dll";
#endif
		strncpy(szFileName, (const char*)(LPCTSTR)FilePath, sizeof(szFileName) - 1);
	}
	
	if (reType == 4)
{
			g_stLiveCaptDll.hDllModule = LoadLibraryA(szFileName);
	if(NULL == g_stLiveCaptDll.hDllModule)
	{
		_snprintf(szInfo, sizeof(szInfo) - 1, "装载动态库%s失败!\n错误值:%d", szFileName, GetLastError());
		goto FINISH_EXIT;
	}
	
	 
	 
	g_stLiveCaptDll.pfnLiveScan_Init		= (GFLS_LIVESCAN_INIT)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_Init");

	if (!g_stLiveCaptDll.pfnLiveScan_Init) 
	{
		temp2 += "测试\"4.1初始化采集设备\"失败 \r\n";
	}
	else 
	{
		temp2 += "测试\"4.1初始化采集设备\"成功\r\n";
	}
	//add 
	g_stLiveCaptDll.pfnLiveScan_InitEx		= (GFLS_LIVESCAN_INITEX)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_InitEx");

	if (!g_stLiveCaptDll.pfnLiveScan_InitEx) 
	{
		temp2 += "测试\"4.1.1初始化采集设备\"失败 \r\n";
	}
	else 
	{
		temp2 += "测试\"4.1.1初始化采集设备\"成功\r\n";
	}

	g_stLiveCaptDll.pfnLiveScan_Close			= (GFLS_LIVESCAN_CLOSE)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_Close");
	
	if (!g_stLiveCaptDll.pfnLiveScan_Close) 
	{temp2 += "测试\"4.2释放采集设备\"失败  \r\n";}
	else 
	{
		temp2 += "测试\"4.2释放采集设备\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_GetChannelCount = (GFLS_LIVESCAN_GETCHANNELCOUNT)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetChannelCount");
	if (!g_stLiveCaptDll.pfnLiveScan_GetChannelCount) 
	{temp2 += "测试\"4.3获得采集设备通道数量\"失败 \r\n";}
	else 
	{
		temp2 += "测试\"4.3获得采集设备通道数量\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_SetBright		= (GFLS_LIVESCAN_SETBRIGHT)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_SetBright");
	if (!g_stLiveCaptDll.pfnLiveScan_SetBright) 
	{
		temp2 += "测试\"4.4设置采集设备当前的亮度\"失败\r\n";
	}
	else 
	{
		temp2 += "测试\"4.4设置采集设备当前的亮度\"成功 \r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_SetContrast		= (GFLS_LIVESCAN_SETCONTRAST)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_SetContrast");

	if (!g_stLiveCaptDll.pfnLiveScan_SetContrast) 
	{temp2 += "测试\"4.5设置采集设备当前的对比度\"失败  \r\n";}
	else 
	{
		temp2 += "测试\"4.5设置采集设备当前的对比度\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_GetBright		= (GFLS_LIVESCAN_GETBRIGHT)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetBright");

	if (!g_stLiveCaptDll.pfnLiveScan_GetBright) 
	{
		temp2 += "测试\"4.6获得采集设备当前的亮度\"失败  \r\n";
	}
	else 
	{
		temp2 += "测试\"4.6获得采集设备当前的亮度\"成功  \r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_GetContrast		= (GFLS_LIVESCAN_GETCONTRAST)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetContrast");

	if (!g_stLiveCaptDll.pfnLiveScan_GetContrast) 
	{temp2 += "测试\"4.7获得采集设备当前的对比度\"失败  \r\n";}
	else 
	{
		temp2 += "测试\"4.7获得采集设备当前的对比度\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_GetMaxImageSize = (GFLS_LIVESCAN_GETMAXIMAGESIZE)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetMaxImageSize");	
	if (!g_stLiveCaptDll.pfnLiveScan_GetMaxImageSize) 
	{temp2 += "测试\"4.8获得采集设备采集图像的宽度、高度的最大值\"失败 \r\n";}
	else 
	{
		temp2 += "测试\"4.8获得采集设备采集图像的宽度、高度的最大值\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_GetCaptWindow	= (GFLS_LIVESCAN_GETCAPTWINDOW)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetCaptWindow");

	if (!g_stLiveCaptDll.pfnLiveScan_GetCaptWindow) 
	{temp2 += "测试\"4.9获得当前图像的采集位置、宽度和高度\"失败  \r\n";}
	else 
	{
		temp2 += "测试\"4.9获得当前图像的采集位置、宽度和高度\"成功 \r\n";
	}
	g_stLiveCaptDll.pfnLivescan_SetCaptWindow	= (GFLS_LIVESCAN_SETCAPTWINDOW)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_SetCaptWindow");
	if (!g_stLiveCaptDll.pfnLivescan_SetCaptWindow) 
	{temp2 += "测试\"4.10设置当前图像的采集位置、宽度和高度\"失败 \r\n";}
	else 
	{
		temp2 += "测试\"4.10设置当前图像的采集位置、宽度和高度\"成功 \r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_Setup			= (GFLS_LIVESCAN_SETUP)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_Setup");
	if (!g_stLiveCaptDll.pfnLiveScan_Setup) 
	{temp2 += "测试\"4.11调用采集设备的属性设置对话框\"失败 \r\n";}
	else 
	{
		temp2 += "测试\"4.11调用采集设备的属性设置对话框\"成功 \r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_GetErrorInfo	= (GFLS_LIVESCAN_GETERRORINFO)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetErrorInfo");
	if (!g_stLiveCaptDll.pfnLiveScan_GetErrorInfo) 
	{temp2 += "测试\"4.12取得采集接口错误信息\"失败   \r\n";}
	else 
	{
		temp2 += "测试\"4.12取得采集接口错误信息\"成功  \r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_BeginCapture	= (GFLS_LIVESCAN_BEGINCAPTURE)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_BeginCapture");
	if (!g_stLiveCaptDll.pfnLiveScan_BeginCapture) 
	{temp2 += "测试\"4.13准备采集一帧图像\"失败  \r\n";}
	else 
	{
		temp2 += "测试\"4.13准备采集一帧图像\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_GetFPRawData	= (GFLS_LIVESCAN_GETFPRAWDATA)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetFPRawData");

	if (!g_stLiveCaptDll.pfnLiveScan_GetFPRawData) 
	{temp2 += "测试\"4.14采集一帧图像\"失败  \r\n";}
	else 
	{
		temp2 += "测试\"4.14采集一帧图像\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_EndCapture		= (GFLS_LIVESCAN_ENDCAPTURE)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_EndCapture");
	if (!g_stLiveCaptDll.pfnLiveScan_EndCapture) 
	{temp2 += "测试\"4.15结束采集一帧图像\"失败 \r\n";}
	else 
	{
		temp2 += "测试\"4.15结束采集一帧图像\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_IsSupportCaptWindow = (GFLS_LIVESCAN_ISSUPPORTCAPTWINDOW)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_IsSupportCaptWindow");
	if (!g_stLiveCaptDll.pfnLiveScan_IsSupportCaptWindow) 
	{temp2 += "测试\"4.16判断采集设备是否支持采集窗口设置\"失败 \r\n";}
	else 
	{
		temp2 += "测试\"4.16判断采集设备是否支持采集窗口设置\"成功\r\n";
	}
	g_stLiveCaptDll.pfnLiveScan_IsSupportSetup	= (GFLS_LIVESCAN_ISSUPPORTSETUP)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_IsSupportSetup");
	if (!g_stLiveCaptDll.pfnLiveScan_IsSupportSetup) 
	{
		temp2 += "测试\"4.17采集设备是否支持设置对话框\"失败 \r\n";
	}
	else 
	{
		temp2 += "测试\"4.17采集设备是否支持设置对话框\"成功 \r\n";
	}
//	g_stLiveCaptDll.pfnLiveScan_IsFastDevice	= (GFLS_LIVESCAN_ISFASTDEVICE)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_IsFastDevice");
	g_stLiveCaptDll.pfnLiveScan_GetVersion		= (GFLS_LIVESCAN_GETVERSION)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetVersion");
	if (!g_stLiveCaptDll.pfnLiveScan_GetVersion	) 
	{temp2 += "测试\"4.18取得接口的版本\"失败   \r\n";}
	else 
	{
		temp2 += "测试\"4.18取得接口的版本\"成功 \r\n";
	}
	g_stLiveCaptDll.pfnLivescan_GetDesc			= (GFLS_LIVESCAN_GETDESC)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetDesc");
	if (!g_stLiveCaptDll.pfnLivescan_GetDesc	) 
	{
		temp2 += "测试\"4.20获得接口的说明\"失败 \r\n";\
	}
	else 
	{
		temp2 += "测试\"4.20获得接口的说明\"成功 \r\n";

		char	szDesc[1032] = {0};
		g_stLiveCaptDll.pfnLivescan_GetDesc(szDesc);
		temp2 += szDesc;
	}
// 	g_stLiveCaptDll.pfnLiveScan_GetSerialNum = (GFLS_LIVESCAN_GETSERIALNUM)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetSerialNum");
// 	g_stLiveCaptDll.pfnLiveScan_SetSerialNum = (GFLS_LIVESCAN_SETSERIALNUM)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_SetSerialNum");

	g_stLiveCaptDll.pfnLiveScan_GetSerialNum = (GFLS_LIVESCAN_GETSERIALNUM)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_GetSerialNo");
	g_stLiveCaptDll.pfnLiveScan_SetSerialNum = (GFLS_LIVESCAN_SETSERIALNUM)GetProcAddress(g_stLiveCaptDll.hDllModule, "LIVESCAN_SetSerialNo");
// 	bRet1 = mFile1.Open("inicaiji.log",  CFile::modeCreate|CFile::modeWrite, &ex);
// 	if( !bRet1 )
// 	{
// 		dwPos = ::GetLastError();
// #ifdef _DEBUG
//    afxDump << "File could not be opened " << ex.m_cause << "\n";
// #endif
// 	}
// 	dwPos = mFile1.GetPosition();
// 	mFile1.LockRange(dwPos,1);
// 	mFile1.Write(temp2,temp2.GetLength()); 	
// 	mFile1.UnlockRange(dwPos,1);
// 	mFile1.Flush(); 
// 	mFile1.Close();
	
   
	nret = IsStandardCaptureDll(szInfo, sizeof(szInfo), szFileName);
	if(nret < 0)
		goto FINISH_EXIT;
	g_stLiveCaptDll.bLoaded = 1;
	g_stScannerInfo.nVersion = g_stLiveCaptDll.pfnLiveScan_GetVersion();
		
			
}
else
{

			if( access(szFileName, 0)){
		_snprintf(szInfo, sizeof(szInfo)-1, "%s\r\n文件不存在!", szFileName);
		goto FINISH_EXIT;
	}

	int nScanType = g_stCaptDll.LoadScan(szFileName);
	
	if(nScanType == GFLS_CAPT_API_HANDLER::RET_OTHER){
		_snprintf(szInfo, sizeof(szInfo)-1, "%s\r\n不是一个符合活体指纹采集仪接口规范的采集动态库!", szFileName);
		goto FINISH_EXIT;
	}
	if(nScanType == GFLS_CAPT_API_HANDLER::RET_ERROR){
		_snprintf(szInfo, sizeof(szInfo)-1, "%s\r\n加载采集仪动态库失败!", szFileName);
		goto FINISH_EXIT;
	}
	//nret = IsStandardCaptureDll(szInfo, sizeof(szInfo), szFileName);
	//if(nret < 0)
	//	goto FINISH_EXIT;
	g_stCaptDll.bLoaded = 1;
	g_stScannerInfo.nVersion = g_stCaptDll.pfnScan_GetVersion();

		
}
	
	
	
	
	
	
	
	
	temp1.Format("%f",g_stScannerInfo.nVersion);
	if (g_stScannerInfo.nVersion < 0) 
	{temp += "测试\"4.18取得接口的版本\"失败 返回值="+temp1+"\r\n";}
	else 
	{
		temp += "测试\"4.18取得接口的版本\"成功 返回值= "+temp1+"\r\n";
	}
	nret = 1;
	
	
	bRet = mFile.Open(_T("caiji.log"),  CFile::modeCreate|CFile::modeWrite, &ex);
	if( !bRet )
	{
		dwPos = ::GetLastError();
#ifdef _DEBUG
   afxDump << "File could not be opened " << ex.m_cause << "\n";
#endif
	}
	dwPos = (DWORD)mFile.GetPosition();
	mFile.LockRange(dwPos,1);
	mFile.Write(temp,temp.GetLength()); 	
	mFile.UnlockRange(dwPos,1);
	mFile.Flush(); 
	mFile.Close();
	
FINISH_EXIT:
	if(nret < 1)
	{
#ifdef UNICODE
		ATL::CA2W pwstr(szInfo);
		ShowTestAppErrorInfo((LPWSTR)pwstr, pParentWnd);
#else
		ShowTestAppErrorInfo(szInfo, pParentWnd);
#endif
	}
	return nret;
}

int InitialLiveCapture(CWnd* pParentWnd)
{

	CStringA temp,temp1;
	CFile mFile;
	CFileException e;

	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
	CScrollBar *pBrightBar, *pContrastBar;
	CString strInfo;
	CFileStatus status;
	int n;
	int nret = -1;
	
	if (reType == 4)
	{
		if(!g_stLiveCaptDll.bLoaded)
			return 0;
		if(g_stScannerInfo.bInitialed)
			return 1;

		CMainFrame			*m_pFrameWnd;
		m_pFrameWnd = (CMainFrame*)theApp.GetMainWnd();
	//	int nret = -1;
		TCHAR szInfo[1024] = {0};
		char szInfo1[1024] = {0};
		nret = g_stLiveCaptDll.pfnLiveScan_Init();
		//nret = g_stLiveCaptDll.pfnLiveScan_InitEx(100);
		temp1.Format("%d",nret);
		if (nret == 1)
		{
			temp += "测试\"4.1初始化采集设备\"成功 返回值= "+temp1+"\r\n";
		}
		else 
		{
			temp += "测试\"4.1初始化采集设备\"失败 返回值= "+temp1+"\r\n";
		}
	
		if(nret < 1)
		{
			PutInfo2Log(strFileName,temp);
			MessageBox(NULL,_T("初始化采集设备失败!"), _T("活体采集测试程序"), MB_OK);
			return -1;
		}

		n = g_stLiveCaptDll.pfnLiveScan_GetChannelCount();
	
		temp1.Format("%d",n);
		if (n < 0) 
		{
			temp += "测试\"4.3获得采集设备通道数量\"失败  返回值= "+temp1+" \r\n";
		}
		else if(n > 0)
		{
			temp += "测试\"4.3获得采集设备通道数量\"成功 返回值= "+temp1+"\r\n";
		}
		if(nret < 1)
		{
			PutInfo2Log(strFileName,temp);
	//		if(mFile.GetStatus("caiji.log",status))
	//		{
	//			mFile.Open("caiji.log",CFile::modeWrite,&e);
	//			mFile.SeekToEnd();
	//			dwPos = mFile.GetPosition();
	//			mFile.LockRange(dwPos,1);
	//			mFile.Write(temp,temp.GetLength());		
	//			mFile.UnlockRange(dwPos,1);
	//			mFile.Flush();
	//			mFile.Close(); 
	// 	}

			goto FINISH_EXIT;
		}
		g_stScannerInfo.nMaxChannel = nret;
		g_stScannerInfo.nCurChannel = 0;

		nret = g_stLiveCaptDll.pfnLivescan_SetCaptWindow(g_stScannerInfo.nCurChannel, 0, 0, GFLS_SCANNER_FINGER_WIDTH, GFLS_SCANNER_FINGER_HEIGHT);

		nret = g_stLiveCaptDll.pfnLiveScan_GetBright(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nBright);
		temp1.Format("%d",nret);
		if(nret == 1) 
		{temp += "测试\"4.6获得采集设备当前的亮度\"成功 返回值= "+temp1+" \r\n";}
		else 
		{
			temp += "测试\"4.6获得采集设备当前的亮度\"失败 返回值= "+temp1+"\r\n";
		}
		n = g_stLiveCaptDll.pfnLiveScan_GetContrast(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nContrast);
		temp1.Format("%d",n);
		if (nret == 1)
		{temp += "测试\"4.7获得采集设备当前的对比度\"成功 返回值="+temp1+" \r\n";}
		else 
		{
			temp += "测试\"4.7获得采集设备当前的对比度\"失败 返回值= "+temp1+"\r\n";
		}
		n = g_stLiveCaptDll.pfnLiveScan_GetMaxImageSize(g_stScannerInfo.nCurChannel,&g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
		temp1.Format("%d",n);
		if (n < 0) 
		{temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1)
		{
			temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"成功 返回值= "+temp1+"\r\n";
		}
		nret = g_stLiveCaptDll.pfnLiveScan_GetCaptWindow(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nXOrigin, &g_stScannerInfo.nYOrigin,
														 &g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"4.9获得当前图像的采集位置、宽度和高度\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1)
		{
			temp += "测试\"4.9获得当前图像的采集位置、宽度和高度\"成功 返回值= "+temp1+"\r\n";
		}
		if(nret < 1)
			goto FINISH_EXIT;
		if(g_stScannerInfo.nWidth != GFLS_SCANNER_IMAGEWIDTH || g_stScannerInfo.nHeight != GFLS_SCANNER_IMAGEHEIGHT)
		{
			_sntprintf(szInfo, sizeof(szInfo) - 1, _T("采集窗口的大小(%dx%d)不符合警用采集仪(%dx%d)的要求!"), 
					  g_stScannerInfo.nWidth, g_stScannerInfo.nHeight, GFLS_SCANNER_IMAGEWIDTH, GFLS_SCANNER_IMAGEHEIGHT);
			ShowTestAppErrorInfo(szInfo, pParentWnd);
			return -1;
		}

		nret = g_stLiveCaptDll.pfnLiveScan_IsSupportCaptWindow(g_stScannerInfo.nCurChannel);
	
		temp1.Format("%d",nret);
		if (nret == 1)
		{
			temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"成功 返回值= "+temp1+" \r\n";
		}
		else 
		{
			temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"失败 返回值= "+temp1+"\r\n";
		}
		g_stScannerInfo.bSptCaptWnd = (nret == 1) ? 1 : 0;
		nret = g_stLiveCaptDll.pfnLiveScan_IsSupportSetup();
		temp1.Format("%d",nret);
		if (nret == 1||nret == 0)  
		{
			temp += "测试\"4.17采集设备是否支持设置对话框\"成功 返回值= "+temp1+"\r\n";
		}
		else 
		{
			temp += "测试\"4.17采集设备是否支持设置对话框\"失败 返回值= "+temp1+"\r\n";
		}
		nret = g_stLiveCaptDll.pfnLivescan_GetDesc(szInfo1);
		temp1.Format("%d",nret);
		if (nret == 1||nret == 0)  
		{
			temp += "测试\"4.20获得接口的说明\"成功 返回值= "+temp1+"\r\n";
		}
		else 
		{
			temp += "测试\"4.20获得接口的说明\"失败 返回值= "+temp1+"\r\n";
		}
		pBrightBar	 = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_BRIGHT_SCROLL);
		pContrastBar = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_CONTRAST_SCROLL);

		pBrightBar->SetScrollPos(g_stScannerInfo.nBright);
		pContrastBar->SetScrollPos(g_stScannerInfo.nContrast);

	
		strInfo.Format(_T("%d"), g_stScannerInfo.nBright);
		m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_BRIGHT)->SetWindowText(strInfo);
		strInfo.Format(_T("%d"), g_stScannerInfo.nContrast);
		m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_CONTRAST)->SetWindowText(strInfo);
		PutInfo2Log(strFileName,temp);
	//	if(mFile.GetStatus("caiji.log",status))
	//	{
	//		mFile.Open("caiji.log",CFile::modeWrite,&e);
	//		mFile.SeekToEnd();
	//		dwPos = mFile.GetPosition();
	//		mFile.LockRange(dwPos,1);
	//		mFile.Write(temp,temp.GetLength());		
	//		mFile.UnlockRange(dwPos,1);
	//		mFile.Flush();
	//
	//		mFile.Close(); 
	//	}
	
		g_stScannerInfo.bSptSetup = (nret == 1) ? 1 : 0;
		nret = 1;	// g_stLiveCaptDll.pfnLiveScan_IsFastDevice(g_stScannerInfo.nCurChannel);
		g_stScannerInfo.bFaseDev = (nret == 1) ? 1 : 0;
		g_stScannerInfo.bInitialed = 1;
		nret = 1;
	}
	else
	{
		if(!g_stCaptDll.bLoaded)
			return 0;
		if(g_stScannerInfo.bInitialed)
			return 1;
		CMainFrame			*m_pFrameWnd;
		m_pFrameWnd = (CMainFrame*)theApp.GetMainWnd();
	//	int nret = -1;
		char szInfo[1024] = {0};
		char szInfo1[1024] = {0};
		nret = g_stCaptDll.pfnScan_Init();
		//nret = g_stCaptDll.pfnLiveScan_InitEx(100);
		temp1.Format("%d",nret);
		if (nret == 1)
		{
			temp += "测试\"4.1初始化采集设备\"成功 返回值= "+temp1+"\r\n";
		}
		else 
		{
			temp += "测试\"4.1初始化采集设备\"失败 返回值= "+temp1+"\r\n";
		}
	
		if(nret < 1)
		{
			PutInfo2Log(strFileName,temp);
			MessageBox(NULL,_T("初始化采集设备失败!"), _T("活体采集测试程序"), MB_OK);
			return -1;
		}

		n = g_stCaptDll.pfnScan_GetChannelCount();
	
		temp1.Format("%d",n);
		if (n < 0){
			temp += "测试\"4.3获得采集设备通道数量\"失败  返回值= "+temp1+" \r\n";
		}else if(n > 0){
			temp += "测试\"4.3获得采集设备通道数量\"成功 返回值= "+temp1+"\r\n";
		}
		if(nret < 1)
		{
			PutInfo2Log(strFileName,temp);
	//		if(mFile.GetStatus("caiji.log",status))
	//		{
	//			mFile.Open("caiji.log",CFile::modeWrite,&e);
	//			mFile.SeekToEnd();
	//			dwPos = mFile.GetPosition();
	//			mFile.LockRange(dwPos,1);
	//			mFile.Write(temp,temp.GetLength());		
	//			mFile.UnlockRange(dwPos,1);
	//			mFile.Flush();
	//			mFile.Close(); 
	// 	}

			goto FINISH_EXIT;
		}
		g_stScannerInfo.nMaxChannel = nret;
		g_stScannerInfo.nCurChannel = 0;

		nret = g_stCaptDll.pfnScan_SetCaptWindow(g_stScannerInfo.nCurChannel, 0, 0, g_stCaptDll.nScanWidth, g_stCaptDll.nScanHeight);

		nret = g_stCaptDll.pfnScan_GetBright(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nBright);
		temp1.Format("%d",nret);
		if(nret == 1) 
		{temp += "测试\"4.6获得采集设备当前的亮度\"成功 返回值= "+temp1+" \r\n";}
		else 
		{
			temp += "测试\"4.6获得采集设备当前的亮度\"失败 返回值= "+temp1+"\r\n";
		}
		n = g_stCaptDll.pfnScan_GetContrast(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nContrast);
		temp1.Format("%d",n);
		if (nret == 1)
		{temp += "测试\"4.7获得采集设备当前的对比度\"成功 返回值="+temp1+" \r\n";}
		else 
		{
			temp += "测试\"4.7获得采集设备当前的对比度\"失败 返回值= "+temp1+"\r\n";
		}
#ifdef UNSTD_BLACK_BALANCE
		if (g_stCaptDll.pfnScan_GetBlackBalance)
		{
			n = g_stCaptDll.pfnScan_GetBlackBalance(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nBlackBalance);
			temp1.Format("%d", n);
			if (nret == 1)
			{
				temp += "测试\"4.7获得采集设备当前的对比度\"成功 返回值=" + temp1 + " \r\n";
			}
			else
			{
				temp += "测试\"4.7获得采集设备当前的对比度\"失败 返回值= " + temp1 + "\r\n";
			}
		}
		
#endif
		n = g_stCaptDll.pfnScan_GetMaxImageSize(g_stScannerInfo.nCurChannel,&g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
		temp1.Format("%d",n);
		if (n < 0) 
		{temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1)
		{
			temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"成功 返回值= "+temp1+"\r\n";
		}
		nret = g_stCaptDll.pfnScan_GetCaptWindow(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nXOrigin, &g_stScannerInfo.nYOrigin,
														 &g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);

		temp1.Format("%d",nret);
		if (nret < 0) 
		{temp += "测试\"4.9获得当前图像的采集位置、宽度和高度\"失败 返回值= "+temp1+" \r\n";}
		else if(nret == 1)
		{
			temp += "测试\"4.9获得当前图像的采集位置、宽度和高度\"成功 返回值= "+temp1+"\r\n";
		}
		if(nret < 1)
			goto FINISH_EXIT;

		g_stCaptDll.SetWidth_Height(g_stScannerInfo.nWidth, g_stScannerInfo.nHeight);
		//nn 这里要根据返回的宽度和高度重新配置
	
		BITMAPINFOHEADER* pHead = &m_pFrameWnd->m_pTestView->m_stbmpInfo.bmiHeader;
		pHead->biWidth			= g_stScannerInfo.nWidth;
		pHead->biHeight			= g_stScannerInfo.nHeight;
		pHead->biSizeImage		= g_stScannerInfo.nWidth * g_stScannerInfo.nHeight;
	

		m_pFrameWnd->m_pTestView->m_stParamInfo.nImgWidth				= g_stScannerInfo.nWidth;
		m_pFrameWnd->m_pTestView->m_stParamInfo.nImgHeight			= g_stScannerInfo.nHeight;
		m_pFrameWnd->m_pTestView->m_stParamInfo.nImgSize				= g_stScannerInfo.nWidth * g_stScannerInfo.nHeight;

		m_pFrameWnd->m_pTestView->m_stParamInfo.nDispWidth			= m_pFrameWnd->m_pTestView->m_stParamInfo.nImgWidth;
		m_pFrameWnd->m_pTestView->m_stParamInfo.nDispHeight			= m_pFrameWnd->m_pTestView->m_stParamInfo.nImgHeight;
		m_pFrameWnd->m_pTestView->m_stParamInfo.nDispImgSize			= m_pFrameWnd->m_pTestView->m_stParamInfo.nImgSize;


		m_pFrameWnd->m_pTestView->m_stParamInfo.nSquare4OffX         = (g_stScannerInfo.nWidth-200)/2;
		m_pFrameWnd->m_pTestView->m_stParamInfo.nSquare4OffY          =(g_stScannerInfo.nHeight-200)/2;


		m_pFrameWnd->m_pTestView->m_stParamInfo.ptAreaLeftTop.x = g_stScannerInfo.nWidth / 2 - 100;
		m_pFrameWnd->m_pTestView->m_stParamInfo.ptAreaLeftTop.y = g_stScannerInfo.nHeight / 2 - 100;
		m_pFrameWnd->m_pTestView->m_stParamInfo.ptAreaRightBottom.x = m_pFrameWnd->m_pTestView->m_stParamInfo.ptAreaLeftTop.x + 200;
		m_pFrameWnd->m_pTestView->m_stParamInfo.ptAreaRightBottom.y = m_pFrameWnd->m_pTestView->m_stParamInfo.ptAreaLeftTop.y + 200;
		m_pFrameWnd->m_pTestView->m_stParamInfo.clArea		= RGB(0, 0, 255);



		m_pFrameWnd->m_pTestView->SetTestViewScrollSizes();
		//nn --------------------这里要根据返回的宽度和高度重新配置
		//if(g_stScannerInfo.nWidth != GFLS_SCANNER_IMAGEWIDTH || g_stScannerInfo.nHeight != GFLS_SCANNER_IMAGEHEIGHT)
		//{
		//	_snprintf(szInfo, sizeof(szInfo) - 1, "采集窗口的大小(%dx%d)不符合警用采集仪(%dx%d)的要求!", 
		//			  g_stScannerInfo.nWidth, g_stScannerInfo.nHeight, GFLS_SCANNER_IMAGEWIDTH, GFLS_SCANNER_IMAGEHEIGHT);
		//	ShowTestAppErrorInfo(szInfo, pParentWnd);
		//	return -1;
		//}

		nret = g_stCaptDll.pfnScan_IsSupportCaptWindow(g_stScannerInfo.nCurChannel);
	
		temp1.Format("%d",nret);
		if (nret == 1)
		{
			temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"成功 返回值= "+temp1+" \r\n";
		}
		else 
		{
			temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"失败 返回值= "+temp1+"\r\n";
		}
		g_stScannerInfo.bSptCaptWnd = (nret == 1) ? 1 : 0;
		nret = g_stCaptDll.pfnScan_IsSupportSetup();
		temp1.Format("%d",nret);
		if (nret == 1||nret == 0)  
		{
			temp += "测试\"4.17采集设备是否支持设置对话框\"成功 返回值= "+temp1+"\r\n";
		}
		else 
		{
			temp += "测试\"4.17采集设备是否支持设置对话框\"失败 返回值= "+temp1+"\r\n";
		}
		nret = g_stCaptDll.pfnScan_GetDesc(szInfo1);
		temp1.Format("%d",nret);
		if (nret == 1||nret == 0)  
		{
			temp += "测试\"4.20获得接口的说明\"成功 返回值= "+temp1+"\r\n";
		}
		else 
		{
			temp += "测试\"4.20获得接口的说明\"失败 返回值= "+temp1+"\r\n";
		}
		pBrightBar	 = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_BRIGHT_SCROLL);
		pContrastBar = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_CONTRAST_SCROLL);

		pBrightBar->SetScrollPos(g_stScannerInfo.nBright);
		pContrastBar->SetScrollPos(g_stScannerInfo.nContrast);

#ifdef UNSTD_BLACK_BALANCE
		CScrollBar *pBlackBalanceBar = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_BLACKBALANCE_SCROLL);
		pBlackBalanceBar->SetScrollPos(g_stScannerInfo.nBlackBalance);

		strInfo.Format(_T("%d"), g_stScannerInfo.nBlackBalance);
		m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_BLACKBALANCE)->SetWindowText(strInfo);
#endif
	
		strInfo.Format(_T("%d"), g_stScannerInfo.nBright);
		m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_BRIGHT)->SetWindowText(strInfo);
		strInfo.Format(_T("%d"), g_stScannerInfo.nContrast);
		m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_CONTRAST)->SetWindowText(strInfo);
		PutInfo2Log(strFileName,temp);
	//	if(mFile.GetStatus("caiji.log",status))
	//	{
	//		mFile.Open("caiji.log",CFile::modeWrite,&e);
	//		mFile.SeekToEnd();
	//		dwPos = mFile.GetPosition();
	//		mFile.LockRange(dwPos,1);
	//		mFile.Write(temp,temp.GetLength());		
	//		mFile.UnlockRange(dwPos,1);
	//		mFile.Flush();
	//
	//		mFile.Close(); 
	//	}
	
		g_stScannerInfo.bSptSetup = (nret == 1) ? 1 : 0;
		nret = 1;	// g_stCaptDll.pfnLiveScan_IsFastDevice(g_stScannerInfo.nCurChannel);
		g_stScannerInfo.bFaseDev = (nret == 1) ? 1 : 0;
		g_stScannerInfo.bInitialed = 1;
		nret = 1;

	}
	
FINISH_EXIT:
	if(nret < 1)
	{
		ShowScannerErrorInfo(nret, pParentWnd, 1);
	}
	return nret;
}

int InitialLiveCaptureEx(CWnd* pParentWnd, unsigned int nSerialNo)
{

	CStringA temp,temp1;
	CFile mFile;
	CFileException e;

	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
	CScrollBar *pBrightBar, *pContrastBar;
	CString strInfo;
	CFileStatus status;
	int n;
	int nret = -1;
	
	
	if (reType == 4)
{
		if(!g_stLiveCaptDll.bLoaded)
		return 0;
	if(g_stScannerInfo.bInitialed)
		return 1;
	CMainFrame			*m_pFrameWnd;
	m_pFrameWnd = (CMainFrame*)theApp.GetMainWnd();
//	int nret = -1;
	TCHAR szInfo[1024] = {0};
	char szInfo1[1024] = {0};
	nret = g_stLiveCaptDll.pfnLiveScan_InitEx(nSerialNo);
	temp1.Format("%d",nret);
	if (nret == 1)
	{
		temp += "测试\"4.1初始化采集设备\"成功 返回值= "+temp1+"\r\n";
	}
	else 
	{
		temp += "测试\"4.1初始化采集设备\"失败 返回值= "+temp1+"\r\n";
	}

	if(nret < 1)
	{
		PutInfo2Log(strFileName,temp);
		MessageBox(NULL,_T("初始化采集设备失败!"), _T("活体采集测试程序"), MB_OK);
		return -1;
	}

	n = g_stLiveCaptDll.pfnLiveScan_GetChannelCount();

	temp1.Format("%d",n);
	if (n < 0) 
	{temp += "测试\"4.3获得采集设备通道数量\"失败  返回值= "+temp1+" \r\n";}
	else if(n > 0)
	{
		temp += "测试\"4.3获得采集设备通道数量\"成功 返回值= "+temp1+"\r\n";
	}
	if(nret < 1)
	{
		PutInfo2Log(strFileName,temp);
		//		if(mFile.GetStatus("caiji.log",status))
		//		{
		//			mFile.Open("caiji.log",CFile::modeWrite,&e);
		//			mFile.SeekToEnd();
		//			dwPos = mFile.GetPosition();
		//			mFile.LockRange(dwPos,1);
		//			mFile.Write(temp,temp.GetLength());		
		//			mFile.UnlockRange(dwPos,1);
		//			mFile.Flush();
		//			mFile.Close(); 
		// 	}

		goto FINISH_EXIT;
	}
	g_stScannerInfo.nMaxChannel = nret;
	g_stScannerInfo.nCurChannel = 0;

	nret = g_stLiveCaptDll.pfnLivescan_SetCaptWindow(g_stScannerInfo.nCurChannel, 0, 0, GFLS_SCANNER_FINGER_WIDTH, GFLS_SCANNER_FINGER_HEIGHT);

	nret = g_stLiveCaptDll.pfnLiveScan_GetBright(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nBright);
	temp1.Format("%d",nret);
	if(nret == 1) 
	{temp += "测试\"4.6获得采集设备当前的亮度\"成功 返回值= "+temp1+" \r\n";}
	else 
	{
		temp += "测试\"4.6获得采集设备当前的亮度\"失败 返回值= "+temp1+"\r\n";
	}
	n = g_stLiveCaptDll.pfnLiveScan_GetContrast(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nContrast);
	temp1.Format("%d",n);
	if (nret == 1)
	{temp += "测试\"4.7获得采集设备当前的对比度\"成功 返回值="+temp1+" \r\n";}
	else 
	{
		temp += "测试\"4.7获得采集设备当前的对比度\"失败 返回值= "+temp1+"\r\n";
	}
	n = g_stLiveCaptDll.pfnLiveScan_GetMaxImageSize(g_stScannerInfo.nCurChannel,&g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
	temp1.Format("%d",n);
	if (n < 0) 
	{temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1)
	{
		temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"成功 返回值= "+temp1+"\r\n";
	}
	nret = g_stLiveCaptDll.pfnLiveScan_GetCaptWindow(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nXOrigin, &g_stScannerInfo.nYOrigin,
		&g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"4.9获得当前图像的采集位置、宽度和高度\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1)
	{
		temp += "测试\"4.9获得当前图像的采集位置、宽度和高度\"成功 返回值= "+temp1+"\r\n";
	}
	if(nret < 1)
		goto FINISH_EXIT;
	if(g_stScannerInfo.nWidth != GFLS_SCANNER_IMAGEWIDTH || g_stScannerInfo.nHeight != GFLS_SCANNER_IMAGEHEIGHT)
	{
		_sntprintf(szInfo, sizeof(szInfo) - 1, _T("采集窗口的大小(%dx%d)不符合警用采集仪(%dx%d)的要求!"), 
			g_stScannerInfo.nWidth, g_stScannerInfo.nHeight, GFLS_SCANNER_IMAGEWIDTH, GFLS_SCANNER_IMAGEHEIGHT);
		ShowTestAppErrorInfo(szInfo, pParentWnd);
		return -1;
	}

	nret = g_stLiveCaptDll.pfnLiveScan_IsSupportCaptWindow(g_stScannerInfo.nCurChannel);

	temp1.Format("%d",nret);
	if (nret == 1)
	{
		temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"成功 返回值= "+temp1+" \r\n";
	}
	else 
	{
		temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"失败 返回值= "+temp1+"\r\n";
	}
	g_stScannerInfo.bSptCaptWnd = (nret == 1) ? 1 : 0;
	nret = g_stLiveCaptDll.pfnLiveScan_IsSupportSetup();
	temp1.Format("%d",nret);
	if (nret == 1||nret == 0)  
	{
		temp += "测试\"4.17采集设备是否支持设置对话框\"成功 返回值= "+temp1+"\r\n";
	}
	else 
	{
		temp += "测试\"4.17采集设备是否支持设置对话框\"失败 返回值= "+temp1+"\r\n";
	}
	nret = g_stLiveCaptDll.pfnLivescan_GetDesc(szInfo1);
	temp1.Format("%d",nret);
	if (nret == 1||nret == 0)  
	{
		temp += "测试\"4.20获得接口的说明\"成功 返回值= "+temp1+"\r\n";
	}
	else 
	{
		temp += "测试\"4.20获得接口的说明\"失败 返回值= "+temp1+"\r\n";
	}
	pBrightBar	 = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_BRIGHT_SCROLL);
	pContrastBar = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_CONTRAST_SCROLL);

	pBrightBar->SetScrollPos(g_stScannerInfo.nBright);
	pContrastBar->SetScrollPos(g_stScannerInfo.nContrast);


	strInfo.Format(_T("%d"), g_stScannerInfo.nBright);
	m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_BRIGHT)->SetWindowText(strInfo);
	strInfo.Format(_T("%d"), g_stScannerInfo.nContrast);
	m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_CONTRAST)->SetWindowText(strInfo);
	PutInfo2Log(strFileName,temp);
	//	if(mFile.GetStatus("caiji.log",status))
	//	{
	//		mFile.Open("caiji.log",CFile::modeWrite,&e);
	//		mFile.SeekToEnd();
	//		dwPos = mFile.GetPosition();
	//		mFile.LockRange(dwPos,1);
	//		mFile.Write(temp,temp.GetLength());		
	//		mFile.UnlockRange(dwPos,1);
	//		mFile.Flush();
	//
	//		mFile.Close(); 
	//	}

	g_stScannerInfo.bSptSetup = (nret == 1) ? 1 : 0;
	nret = 1;	// g_stLiveCaptDll.pfnLiveScan_IsFastDevice(g_stScannerInfo.nCurChannel);
	g_stScannerInfo.bFaseDev = (nret == 1) ? 1 : 0;
	g_stScannerInfo.bInitialed = 1;
	nret = 1;

			
}
else
{
		if(!g_stCaptDll.bLoaded)
		return 0;
	if(g_stScannerInfo.bInitialed)
		return 1;
	CMainFrame			*m_pFrameWnd;
	m_pFrameWnd = (CMainFrame*)theApp.GetMainWnd();
//	int nret = -1;
	TCHAR szInfo[1024] = {0};
	char szInfo1[1024] = {0};
	nret = g_stCaptDll.pfnScan_InitEx(nSerialNo);
	temp1.Format("%d",nret);
	if (nret == 1)
	{
		temp += "测试\"4.1初始化采集设备\"成功 返回值= "+temp1+"\r\n";
	}
	else 
	{
		temp += "测试\"4.1初始化采集设备\"失败 返回值= "+temp1+"\r\n";
	}

	if(nret < 1)
	{
		PutInfo2Log(strFileName,temp);
		MessageBox(NULL,_T("初始化采集设备失败!"), _T("活体采集测试程序"), MB_OK);
		return -1;
	}

	n = g_stCaptDll.pfnScan_GetChannelCount();

	temp1.Format("%d",n);
	if (n < 0) 
	{temp += "测试\"4.3获得采集设备通道数量\"失败  返回值= "+temp1+" \r\n";}
	else if(n > 0)
	{
		temp += "测试\"4.3获得采集设备通道数量\"成功 返回值= "+temp1+"\r\n";
	}
	if(nret < 1)
	{
		PutInfo2Log(strFileName,temp);
		//		if(mFile.GetStatus("caiji.log",status))
		//		{
		//			mFile.Open("caiji.log",CFile::modeWrite,&e);
		//			mFile.SeekToEnd();
		//			dwPos = mFile.GetPosition();
		//			mFile.LockRange(dwPos,1);
		//			mFile.Write(temp,temp.GetLength());		
		//			mFile.UnlockRange(dwPos,1);
		//			mFile.Flush();
		//			mFile.Close(); 
		// 	}

		goto FINISH_EXIT;
	}
	g_stScannerInfo.nMaxChannel = nret;
	g_stScannerInfo.nCurChannel = 0;

	nret = g_stCaptDll.pfnScan_SetCaptWindow(g_stScannerInfo.nCurChannel, 0, 0, GFLS_SCANNER_FINGER_WIDTH, GFLS_SCANNER_FINGER_HEIGHT);

	nret = g_stCaptDll.pfnScan_GetBright(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nBright);
	temp1.Format("%d",nret);
	if(nret == 1) 
	{temp += "测试\"4.6获得采集设备当前的亮度\"成功 返回值= "+temp1+" \r\n";}
	else 
	{
		temp += "测试\"4.6获得采集设备当前的亮度\"失败 返回值= "+temp1+"\r\n";
	}
	n = g_stCaptDll.pfnScan_GetContrast(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nContrast);
	temp1.Format("%d",n);
	if (nret == 1)
	{temp += "测试\"4.7获得采集设备当前的对比度\"成功 返回值="+temp1+" \r\n";}
	else 
	{
		temp += "测试\"4.7获得采集设备当前的对比度\"失败 返回值= "+temp1+"\r\n";
	}
	n = g_stCaptDll.pfnScan_GetMaxImageSize(g_stScannerInfo.nCurChannel,&g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
	temp1.Format("%d",n);
	if (n < 0) 
	{temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1)
	{
		temp += "测试\"4.8获得采集设备采集图像的、宽度和高度的最大值\"成功 返回值= "+temp1+"\r\n";
	}
	nret = g_stCaptDll.pfnScan_GetCaptWindow(g_stScannerInfo.nCurChannel, &g_stScannerInfo.nXOrigin, &g_stScannerInfo.nYOrigin,
		&g_stScannerInfo.nWidth, &g_stScannerInfo.nHeight);
	temp1.Format("%d",nret);
	if (nret < 0) 
	{temp += "测试\"4.9获得当前图像的采集位置、宽度和高度\"失败 返回值= "+temp1+" \r\n";}
	else if(nret == 1)
	{
		temp += "测试\"4.g_stLiveCaptDll9获得当前图像的采集位置、宽度和高度\"成功 返回值= "+temp1+"\r\n";
	}
	if(nret < 1)
		goto FINISH_EXIT;
	if(g_stScannerInfo.nWidth != GFLS_SCANNER_IMAGEWIDTH || g_stScannerInfo.nHeight != GFLS_SCANNER_IMAGEHEIGHT)
	{
		_sntprintf(szInfo, sizeof(szInfo) - 1, _T("采集窗口的大小(%dx%d)不符合警用采集仪(%dx%d)的要求!"), 
			g_stScannerInfo.nWidth, g_stScannerInfo.nHeight, GFLS_SCANNER_IMAGEWIDTH, GFLS_SCANNER_IMAGEHEIGHT);
		ShowTestAppErrorInfo(szInfo, pParentWnd);
		return -1;
	}

	nret = g_stCaptDll.pfnScan_IsSupportCaptWindow(g_stScannerInfo.nCurChannel);

	temp1.Format("%d",nret);
	if (nret == 1)
	{
		temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"成功 返回值= "+temp1+" \r\n";
	}
	else 
	{
		temp += "测试\"4.16判断采集设备是否支持采集窗口设置\"失败 返回值= "+temp1+"\r\n";
	}
	g_stScannerInfo.bSptCaptWnd = (nret == 1) ? 1 : 0;
	nret = g_stCaptDll.pfnScan_IsSupportSetup();
	temp1.Format("%d",nret);
	if (nret == 1||nret == 0)  
	{
		temp += "测试\"4.17采集设备是否支持设置对话框\"成功 返回值= "+temp1+"\r\n";
	}
	else 
	{
		temp += "测试\"4.17采集设备是否支持设置对话框\"失败 返回值= "+temp1+"\r\n";
	}
	nret = g_stCaptDll.pfnScan_GetDesc(szInfo1);
	temp1.Format("%d",nret);
	if (nret == 1||nret == 0)  
	{
		temp += "测试\"4.20获得接口的说明\"成功 返回值= "+temp1+"\r\n";
	}
	else 
	{
		temp += "测试\"4.20获得接口的说明\"失败 返回值= "+temp1+"\r\n";
	}
	pBrightBar	 = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_BRIGHT_SCROLL);
	pContrastBar = (CScrollBar*)m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_CONTRAST_SCROLL);

	pBrightBar->SetScrollPos(g_stScannerInfo.nBright);
	pContrastBar->SetScrollPos(g_stScannerInfo.nContrast);


	strInfo.Format(_T("%d"), g_stScannerInfo.nBright);
	m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_BRIGHT)->SetWindowText(strInfo);
	strInfo.Format(_T("%d"), g_stScannerInfo.nContrast);
	m_pFrameWnd->m_wndDlgBar.GetDlgItem(IDC_STATIC_CONTRAST)->SetWindowText(strInfo);
	PutInfo2Log(strFileName,temp);
	//	if(mFile.GetStatus("caiji.log",status))
	//	{
	//		mFile.Open("caiji.log",CFile::modeWrite,&e);
	//		mFile.SeekToEnd();
	//		dwPos = mFile.GetPosition();
	//		mFile.LockRange(dwPos,1);
	//		mFile.Write(temp,temp.GetLength());		
	//		mFile.UnlockRange(dwPos,1);
	//		mFile.Flush();
	//
	//		mFile.Close(); 
	//	}

	g_stScannerInfo.bSptSetup = (nret == 1) ? 1 : 0;
	nret = 1;	// g_stLiveCaptDll.pfnLiveScan_IsFastDevice(g_stScannerInfo.nCurChannel);
	g_stScannerInfo.bFaseDev = (nret == 1) ? 1 : 0;
	g_stScannerInfo.bInitialed = 1;
	nret = 1;

		
}
	
	
	

FINISH_EXIT:
	if(nret < 1)
	{
		ShowScannerErrorInfo(nret, pParentWnd, 1);
	}
	return nret;
}

void PutInfo2Log2(LPCTSTR szFileName, CString temp)
{
	int m_bWriten = 0;

	CFile mFile;
	CFileException e;
	DWORD dwPos;
	CFileStatus status;
	if (m_bWriten == 0)
	{
		BOOL bSuccess = FALSE;
		if (mFile.GetStatus(szFileName, status))
		{
			bSuccess = mFile.Open(szFileName, CFile::modeWrite, &e);
		}
		else
		{
			bSuccess = mFile.Open(szFileName, CFile::modeCreate | CFile::modeWrite, &e);
		}
		if (!bSuccess)
			return;

		mFile.SeekToEnd();
		dwPos = (DWORD)mFile.GetPosition();
		mFile.LockRange(dwPos, 1);
		mFile.Write(temp, temp.GetLength());
		mFile.UnlockRange(dwPos, 1);
		mFile.Flush();
		mFile.Close();
		m_bWriten = 1;
	}
}

void PutInfo2Log(LPCSTR szFileName,CStringA temp)
{
#ifdef UNICODE
	{
		ATL::CA2W pwstr(szFileName);
		ATL::CA2W pwstr2(temp.GetString());
		PutInfo2Log2((LPWSTR)pwstr, (LPWSTR)pwstr2);
	}
	
#else
	PutInfo2Log2(szFileName, temp);
#endif
}

int DownLiveCaptureDll()
{
	CStringA temp,temp1;
	CFile mFile;
	CFileException e;
	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
	if (reType == 4)
{	

	if(!g_stLiveCaptDll.bLoaded)
		return 1;
		
	StopCaptureThread();
	int n = g_stLiveCaptDll.pfnLiveScan_Close();
	temp1.Format("%d",n);
	if (n == 1) 
	{temp += "\n测试\"4.2释放采集设备\"成功  返回值="+temp1+"";}
	else 
	{
		temp += "\n测试\"4.2释放采集设备\"失败 返回值= "+temp1+"";
	}
	PutInfo2Log(strFileName,temp);
//	CFileStatus rStatus;
//	if( CFile::GetStatus("caiji.log", rStatus) )
//	{
//		mFile.Open("caiji.log",CFile::modeWrite,&e);
//		mFile.SeekToEnd();
//		dwPos = mFile.GetPosition();
//		mFile.LockRange(dwPos,1);
//		mFile.Write(temp,temp.GetLength());
//		mFile.UnlockRange(dwPos,1);
//		mFile.Flush();
//		mFile.Close(); 
//	
//	}
	

	FreeLibrary(g_stLiveCaptDll.hDllModule);
	memset(&g_stLiveCaptDll, 0, sizeof(g_stLiveCaptDll));

	g_stScannerInfo.bInitialed = 0;

			
}
else
{
			if(!g_stCaptDll.bLoaded)
		return 1;
		
	StopCaptureThread();
	int n = g_stCaptDll.pfnScan_Close();
	temp1.Format("%d",n);
	if (n == 1) 
	{temp += "\n测试\"4.2释放采集设备\"成功  返回值="+temp1+"";}
	else 
	{
		temp += "\n测试\"4.2释放采集设备\"失败 返回值= "+temp1+"";
	}
	PutInfo2Log(strFileName,temp);
//	CFileStatus rStatus;
//	if( CFile::GetStatus("caiji.log", rStatus) )
//	{
//		mFile.Open("caiji.log",CFile::modeWrite,&e);
//		mFile.SeekToEnd();
//		dwPos = mFile.GetPosition();
//		mFile.LockRange(dwPos,1);
//		mFile.Write(temp,temp.GetLength());
//		mFile.UnlockRange(dwPos,1);
//		mFile.Flush();
//		mFile.Close(); 
//	
//	}
	

	FreeLibrary(g_stCaptDll.hDllModule);
	memset(&g_stCaptDll, 0, sizeof(g_stCaptDll));

	g_stScannerInfo.bInitialed = 0;

}
	
	

	return 1;
}

int IsStandardCaptureDll(char* szError, int ncbSize, const char* pszFileName)
{

if (reType == 4)
{
				if(g_stLiveCaptDll.pfnLiveScan_Init && g_stLiveCaptDll.pfnLiveScan_Close && 
	   g_stLiveCaptDll.pfnLiveScan_GetChannelCount &&
	   g_stLiveCaptDll.pfnLiveScan_SetBright && g_stLiveCaptDll.pfnLiveScan_SetContrast &&
	   g_stLiveCaptDll.pfnLiveScan_GetBright && g_stLiveCaptDll.pfnLiveScan_GetContrast &&
	   g_stLiveCaptDll.pfnLiveScan_GetMaxImageSize && g_stLiveCaptDll.pfnLiveScan_GetCaptWindow &&
	   g_stLiveCaptDll.pfnLiveScan_GetErrorInfo && g_stLiveCaptDll.pfnLiveScan_BeginCapture && 
	   g_stLiveCaptDll.pfnLiveScan_GetFPRawData && g_stLiveCaptDll.pfnLiveScan_EndCapture &&
	   g_stLiveCaptDll.pfnLiveScan_IsSupportCaptWindow && g_stLiveCaptDll.pfnLiveScan_IsSupportSetup &&
	   g_stLiveCaptDll.pfnLiveScan_GetVersion && g_stLiveCaptDll.pfnLivescan_GetDesc && g_stLiveCaptDll.pfnLiveScan_Setup)
//	   g_stLiveCaptDll.pfnLiveScan_IsFastDevice && 
				{
					return 1;
				}

}
else
{
			if(g_stCaptDll.pfnScan_Init && g_stCaptDll.pfnScan_Close && 
	   g_stCaptDll.pfnScan_GetChannelCount &&
	   g_stCaptDll.pfnScan_SetBright && g_stCaptDll.pfnScan_SetContrast &&
	   g_stCaptDll.pfnScan_GetBright && g_stCaptDll.pfnScan_GetContrast &&
	   g_stCaptDll.pfnScan_GetMaxImageSize && g_stCaptDll.pfnScan_GetCaptWindow &&
	   g_stCaptDll.pfnScan_GetErrorInfo && g_stCaptDll.pfnScan_BeginCapture && 
	   g_stCaptDll.pfnScan_GetFPRawData && g_stCaptDll.pfnScan_EndCapture &&
	   g_stCaptDll.pfnScan_IsSupportCaptWindow && g_stCaptDll.pfnScan_IsSupportSetup &&
	   g_stCaptDll.pfnScan_GetVersion && g_stCaptDll.pfnScan_GetDesc && g_stCaptDll.pfnScan_Setup)

//	   g_stLiveCaptDll.pfnScan_IsFastDevice && 
			{
				return 1;
			}

}

       

	_snprintf(szError, ncbSize - 1, "%s\r\n不是一个符合活体指纹采集仪接口规范的采集动态库!", pszFileName);
	return -1;
}

GFLS_MOSAICAPIHANDLER g_stMosaicDll;

int LoadMosaicDll(CWnd* pParentWnd)
{
	DownMosaicDll();
    int n;
	CStringA temp,temp1,temp2;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
	CFile mFile,mFile1;
	DWORD dwPos;
	CFileException e;
	CFileDialog dlg(TRUE, _T("dll"), _T("gamc*"), OFN_OVERWRITEPROMPT, _T("拼接DLL(*.dll)|*.dll||"), pParentWnd);
	if(dlg.DoModal() != IDOK)
		return 0;

	int nret = -1;
	char szInfo[1024] = {0},szInfo1[1024] = {0}, szFileName[512] = {0};
#ifdef UNICODE
	ATL::CW2A pstr(dlg.GetPathName().GetString());
	strncpy(szFileName, (LPSTR)pstr, sizeof(szFileName)-1);
#else
	_tcsncpy(szFileName, (LPCTSTR)dlg.GetPathName(), sizeof(szFileName)-1);
#endif
	g_stMosaicDll.hDllModule = LoadLibraryA(szFileName);
	if(NULL == g_stMosaicDll.hDllModule)
	{
		_snprintf(szInfo, sizeof(szInfo) - 1, "装载动态库%s失败!", szFileName);
		goto FINISH_EXIT;
	}

	g_stMosaicDll.pfnMosaic_Init					= (GFLS_MOSAIC_INIT)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_Init");
	if (!g_stMosaicDll.pfnMosaic_Init) 
	{temp2 += "测试\"5.1初始化拼接动态库\"失败\r\n";}
	else
	{
		temp2 += "测试\"5.1初始化拼接动态库\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_Close					= (GFLS_MOSAIC_CLOSE)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_Close");
	if (!g_stMosaicDll.pfnMosaic_Close) 
	{temp2 += "测试\"5.2释放拼接动态库\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.2释放拼接动态库\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_IsSupportIdentifyFinger = (GFLS_MOSAIC_ISSUPPORTIDENTIFYFINGER)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_IsSupportIdentifyFinger");

	if (!g_stMosaicDll.pfnMosaic_IsSupportIdentifyFinger) 
	{temp2 += "测试\"5.3拼接接口是否提供判断图像为指纹的函数\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.3拼接接口是否提供判断图像为指纹的函数\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_IsSupportImageQuality	= (GFLS_MOSAIC_ISSUPPORTIMAGEQUALITY)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_IsSupportImageQuality");

	if (!g_stMosaicDll.pfnMosaic_IsSupportImageQuality) 
	{temp2 += "测试\"5.4拼接接口是否提供判断图像质量的函数\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.4拼接接口是否提供判断图像质量的函数\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_IsSupportFingerQuality  = (GFLS_MOSAIC_ISSUPPORTFINGERQUALITY)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_IsSupportFingerQuality");
	if (!g_stMosaicDll.pfnMosaic_IsSupportFingerQuality) 
	{temp2 += "测试\"5.5拼接接口是否提供判断指纹质量的函数\"失败  \r\n";}
		else
	{
		temp2 += "测试\"5.5拼接接口是否提供判断指纹质量的函数\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_IsSupportImageEnhance	= (GFLS_MOSAIC_ISSUPPORTIMAGEENHANCE)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_IsSupportImageEnhance");

	if (!g_stMosaicDll.pfnMosaic_IsSupportImageEnhance) 
	{temp2 += "测试\"5.6接口是否提供拼接指纹的图像增强功能\"失败\r\n";}
		else
	{
		temp2 += "测试\"5.6接口是否提供拼接指纹的图像增强功能\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_IsSupportRollCap		= (GFLS_MOSAIC_ISSUPPORTROLLCAP)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_IsSupportRollCap");
	if (!g_stMosaicDll.pfnMosaic_IsSupportRollCap) 
	{temp2 += "测试\"5.7判断是否支持滚动采集函数\"失败\r\n";}
		else
	{
		temp2 += "测试\"5.7判断是否支持滚动采集函数\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_SetRollMode				= (GFLS_MOSAIC_SETROLLMODE)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_SetRollMode");
	if (!g_stMosaicDll.pfnMosaic_SetRollMode) 
	{temp2 += "测试\"5.8选择拼接方式的函数\"失败\r\n";}
		else
	{
		temp2 += "测试\"5.8选择拼接方式的函数\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_Start					= (GFLS_MOSAIC_START)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_Start");
		if (!g_stMosaicDll.pfnMosaic_Start) 
	{temp2 += "测试\"5.9初始化拼接过程\"失败 \r\n";}
			else
	{
		temp2 += "测试\"5.9初始化拼接过程\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_DoMosaic				= (GFLS_MOSAIC_DOMOSAIC)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_DoMosaic");
		if (!g_stMosaicDll.pfnMosaic_DoMosaic) 
	{temp2 += "测试\"5.10拼接过程\"失败 \r\n";}
			else
	{
		temp2 += "测试\"5.10拼接过程\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_Stop					= (GFLS_MOSAIC_STOP)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_Stop");
	if (!g_stMosaicDll.pfnMosaic_Stop) 
	{temp2 += "测试\"5.11结束拼接\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.11结束拼接\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_ImageQuality			= (GFLS_MOSAIC_IMAGEQUALITY)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_ImageQuality");

	if (!g_stMosaicDll.pfnMosaic_ImageQuality) 
	{temp2 += "测试\"5.12判断图像质量\"失败 \r\n ";}
		else
	{
		temp2 += "测试\"5.12判断图像质量\"成功 \r\n";
	}
	g_stMosaicDll.pfnMosaic_FingerQuality			= (GFLS_MOSAIC_FINGERQUALITY)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_FingerQuality");

	if (!g_stMosaicDll.pfnMosaic_FingerQuality) 
	{temp2 += "测试\"5.13判断指纹质量\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.13判断指纹质量\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_ImageEnhance			= (GFLS_MOSAIC_IMAGEENHANCE)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_ImageEnhance");

	if (!g_stMosaicDll.pfnMosaic_ImageEnhance) 
	{temp2 += "测试\"5.14对图像进行增强\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.14对图像进行增强\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_IsFinger				= (GFLS_MOSAIC_ISFINGER)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_IsFinger");

	if (!g_stMosaicDll.pfnMosaic_IsFinger) 
	{temp2 += "测试\"5.15判断图像是否为指纹\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.15判断图像是否为指纹\"成功 \r\n";
	}
	g_stMosaicDll.pfnMosaic_GetErrorInfo			= (GFLS_MOSAIC_GETERRORINFO)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_GetErrorInfo");
		if (!g_stMosaicDll.pfnMosaic_GetErrorInfo) 
	{temp2 += "测试\"5.16取得拼接接口错误信息\"失败 \r\n";}
			else
	{
		temp2 += "测试\"5.16取得拼接接口错误信息\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_GetVersion				= (GFLS_MOSAIC_GETVERSION)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_GetVersion");

		if (!g_stMosaicDll.pfnMosaic_GetVersion) 
	{temp2 += "测试\"5.17取得接口的版本\"失败 \r\n";}
			else
	{
		temp2 += "测试\"5.17取得接口的版本\"成功\r\n";
	}
	g_stMosaicDll.pfnMosaic_GetDesc					= (GFLS_MOSAIC_GETDESC)GetProcAddress(g_stMosaicDll.hDllModule, "MOSAIC_GetDesc");

	if (!g_stMosaicDll.pfnMosaic_GetDesc) 		
	{temp2 += "测试\"5.18获得此接口的说明\"失败 \r\n";}
		else
	{
		temp2 += "测试\"5.18获得此接口的说明\"成功\r\n";
	}
	
	mFile1.Open(_T("inipinjie.log"), CFile::modeCreate|CFile::modeWrite,&e);
	
	dwPos = (DWORD)mFile1.GetPosition();
	mFile1.LockRange(dwPos, 1);
	mFile1.Write(temp2,temp2.GetLength());
	mFile1.UnlockRange(dwPos,1);
	mFile1.Flush();
	mFile1.Close(); 
	nret = IsStandardMosaicDll(szInfo, sizeof(szInfo), szFileName);
	if(nret < 0)
		goto FINISH_EXIT;
	g_stMosaicDll.bLoaded = 1;
	g_stMosaicDll.bMosaicRoll = 1;

	nret = g_stMosaicDll.pfnMosaic_Init();
	temp1.Format("%d",nret);
	if (nret == 1) 
	{temp += "测试\"5.1初始化拼接动态库\"成功 返回值="+temp1+"\r\n";}
	else
	{
		temp +="测试\"5.1初始化拼接动态库\"失败 返回值="+temp1+" \r\n";
	}
	if(nret < 1)
	{
		PutInfo2Log(strFileName,temp);
		goto FINISH_EXIT;
	}
	n = g_stMosaicDll.pfnMosaic_GetDesc(szInfo1);
	temp1.Format("%d",n);
	if ( n == 1) 
	{
		temp += "测试\"5.18获得拼接接口说明\"成功  返回值="+temp1+"\r\n";
	}
	else
	{
		temp += "测试判断\"5.18获得拼接接口说明\"失败 返回值="+temp1+"\r\n ";
	}
	n = g_stMosaicDll.pfnMosaic_GetVersion();
	temp1.Format("%d",n);
	if ( n < 0) 
	{
		temp += "测试\"5.17取得接口的版本\"失败  返回值="+temp1+"\r\n";
	}
	else
	{
		temp += "测试判断\"5.17取得接口的版本\"成功 返回值="+temp1+"\r\n ";
	}
	nret = g_stMosaicDll.pfnMosaic_IsSupportRollCap();
	temp1.Format("%d",nret);
	if ( nret >= 0 && nret < 4) 
	{
		temp += "测试\"5.7判断是否支持滚动采集函数\"成功  返回值="+temp1+"\r\n";
	}
	else
	{
		temp += "测试判断\"5.7是否支持滚动采集函数\"失败 返回值="+temp1+"\r\n ";
	}
	
	mFile.Open(_T("pinjie.log"), CFile::modeCreate|CFile::modeWrite,&e);

	
	dwPos = (DWORD)mFile.GetPosition();
	mFile.LockRange(dwPos, 1);
	mFile.Write(temp,temp.GetLength());
	mFile.UnlockRange(dwPos,1);
	mFile.Flush();
	mFile.Close(); 
	
	if(nret < 0)
		goto FINISH_EXIT;
	g_stMosaicDll.nRollMode = (UCHAR)nret;
	nret = 1;

FINISH_EXIT:
	if(nret < 1)
	{
		if(g_stMosaicDll.bLoaded)
			ShowScannerErrorInfo(nret, pParentWnd, 0);
		else
		{
#ifdef UNICODE
			ATL::CA2W pwstr(szInfo);
			ShowTestAppErrorInfo((LPWSTR)pwstr, pParentWnd);
#else
			ShowTestAppErrorInfo(szInfo, pParentWnd);
#endif
		}

	}
	return nret;
}

int DownMosaicDll()
{
	CStringA temp,temp1;
	CFile mFile;
	CFileException e;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
	if(!g_stMosaicDll.bLoaded)
		return 1;
	StopCaptureThread();
	int n = g_stMosaicDll.pfnMosaic_Close();
	temp1.Format("%d",n);
	if (n == 1) 
	{
		temp += " 测试\"5.2释放拼接动态库\"成功 返回值="+temp1+"\r\n";
	}
    else
	{
		temp += " 测试\"5.2释放拼接动态库\"失败 返回值="+temp1+"\r\n";
	}
	PutInfo2Log(strFileName,temp);
//	CFileStatus rStatus;
//	if( CFile::GetStatus("pinjie.log", rStatus) )
//	{
//	
//		mFile.Open("pinjie.log", CFile::modeWrite,&e);	
//		dwPos = mFile.GetPosition();
//		mFile.SeekToEnd();
//		mFile.LockRange(dwPos, 1);
//		mFile.Write(temp,temp.GetLength());
//		mFile.UnlockRange(dwPos,1);
//		mFile.Flush();
//		mFile.Close(); 
//	}
	FreeLibrary(g_stMosaicDll.hDllModule);
	memset(&g_stMosaicDll, 0, sizeof(g_stMosaicDll));

	return 1;
}

int IsStandardMosaicDll(char* szError, int ncbSize, const char* pszFileName)
{
	if(g_stMosaicDll.pfnMosaic_Init && g_stMosaicDll.pfnMosaic_Close && g_stMosaicDll.pfnMosaic_IsSupportFingerQuality && 
	   g_stMosaicDll.pfnMosaic_IsSupportIdentifyFinger && g_stMosaicDll.pfnMosaic_IsSupportImageEnhance &&
	   g_stMosaicDll.pfnMosaic_IsSupportImageQuality && g_stMosaicDll.pfnMosaic_IsSupportRollCap && 
	   g_stMosaicDll.pfnMosaic_SetRollMode && g_stMosaicDll.pfnMosaic_Start && g_stMosaicDll.pfnMosaic_DoMosaic &&
	   g_stMosaicDll.pfnMosaic_Stop && g_stMosaicDll.pfnMosaic_ImageQuality && g_stMosaicDll.pfnMosaic_FingerQuality &&
	   g_stMosaicDll.pfnMosaic_ImageEnhance && g_stMosaicDll.pfnMosaic_IsFinger && g_stMosaicDll.pfnMosaic_GetErrorInfo &&
	   g_stMosaicDll.pfnMosaic_GetVersion && g_stMosaicDll.pfnMosaic_GetDesc)
	{
		return 1;
	}
	_snprintf(szError, ncbSize - 1, "%s\r\n不是一个符合活体指纹采集仪接口规范的拼接动态库!", pszFileName);
	return -1;
}

#include "DlgGrayRawOpt.h"
int Gfls_LoadImageFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo)
{
	_strlwr(lpszFileName);
	if(strstr(lpszFileName, ".raw"))
		return Gfls_LoadRawImgFile(lpszFileName, pstImageInfo);
	else
		return Gfls_LoadBmpImgFile(lpszFileName, pstImageInfo);
}

int Gfls_LoadRawImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo)
{
	CDlgGrayRawOpt dlg;
	dlg.m_pszFileName = lpszFileName;
	if(dlg.DoModal() != IDOK)
		return 0;

	UCHAR *pData, bnDummy[4];
	RGBQUAD rgbQuad[256] = {0}, *prgbQuad;
	BITMAPINFOHEADER* pHead;
	int   bufSize, row, nActWidth, nVirtWidth;
	FILE  *fp;

	fp = fopen(lpszFileName, "rb");
	if(NULL == fp)
		return -1;
	if(dlg.m_nHeadSize > 0)
		fseek(fp, dlg.m_nHeadSize, SEEK_SET);
	pHead = &(pstImageInfo->bmpHead);
	pHead->biWidth		 = dlg.m_nWidth;
	pHead->biHeight		 = dlg.m_nHeight;
	pHead->biBitCount	 = 8;
	pHead->biCompression = BI_RGB;

	prgbQuad = rgbQuad;
	for(row = 0; row < 256; row++, prgbQuad++)
	{
		prgbQuad->rgbBlue = prgbQuad->rgbGreen = prgbQuad->rgbRed = (BYTE)row;
	}
	nActWidth  = pHead->biWidth;
	nVirtWidth = (nActWidth + 3) / 4 * 4;
	bufSize = nActWidth * pHead->biHeight;
	pstImageInfo->pImgData = (UCHAR*)malloc(bufSize);
	if(NULL == pstImageInfo->pImgData)
	{
		fclose(fp);
		return -1;
	}
	memset(pstImageInfo->pImgData, 255, bufSize);

	if(nActWidth == nVirtWidth)
		fread(pstImageInfo->pImgData, 1, bufSize, fp);
	else
	{
		pData = pstImageInfo->pImgData;
		for(row = 0; row < pHead->biHeight; row++, pData += nActWidth)
		{
			fread(pData,   1, nActWidth, fp);
			fread(bnDummy, 1, nVirtWidth - nActWidth, fp);
		}
	}
	fclose(fp);
	pHead->biSizeImage = bufSize;
	return 1;	
}

int Gfls_LoadBmpImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo)
{
	WORD  bfType;	// 0x4d42 == 'BM'
	DWORD bfSize, bfReserved, bfOffBits, dwBytes;
	BITMAPINFOHEADER* pHead;
	FILE  *fp;
	int   nColors, bufSize, row, nActWidth, nVirtWidth;
	UCHAR *pData, bnDummy[4];
	RGBQUAD rgbQuad[256] = {0};
	BOOL  bTopDown = FALSE;

	fp = fopen(lpszFileName, "rb");
	if(NULL == fp)
		return -1;

	// BITMAPFILEHEADER
	fread(&bfType, sizeof(WORD), 1, fp);
	if(bfType != 0x4d42)
	{
		fclose(fp);
		return -1;
	}
	fread(&bfSize,	   sizeof(DWORD), 1, fp);
	fread(&bfReserved, sizeof(DWORD), 1, fp);
	fread(&bfOffBits,  sizeof(DWORD), 1, fp);
	dwBytes = 14;

	// BITMAPINFOHEADER
	pHead = &pstImageInfo->bmpHead;
	fread(pHead, sizeof(BITMAPINFOHEADER), 1, fp);
	dwBytes += sizeof(BITMAPINFOHEADER);
	
	nColors = pHead->biClrUsed;
	if(nColors == 0)
		nColors = (1 << pHead->biBitCount);

	// RGBQUAD
	if(pHead->biBitCount != 8)
	{
		fclose(fp);
		return -1;
	}
	fread(rgbQuad, sizeof(RGBQUAD), nColors, fp);
	dwBytes += sizeof(RGBQUAD) * nColors;

	if(pHead->biCompression != BI_RGB)
	{
		fclose(fp);
		return -1;
	}

	if(dwBytes > bfOffBits)
	{
		fclose(fp);
		return -1;
	}

	while(dwBytes < bfOffBits)
	{
		fread(bnDummy, 1, 1, fp);
		dwBytes++;
	}

	if(pHead->biHeight < 0)
	{
		bTopDown = TRUE;
		pHead->biHeight = -pHead->biHeight;
	}

	nActWidth  = pHead->biWidth;
	nVirtWidth = (nActWidth + 3) / 4 * 4;

	bufSize = nActWidth * pHead->biHeight;
	pstImageInfo->pImgData = (UCHAR*)malloc(bufSize);
	if(NULL == pstImageInfo->pImgData)
	{
		fclose(fp);
		return -1;
	}
	memset(pstImageInfo->pImgData, 255, bufSize);

	if(nActWidth == nVirtWidth)
		fread(pstImageInfo->pImgData, 1, bufSize, fp);
	else
	{
		pData = pstImageInfo->pImgData;	// + row * nActWidth;
		for(row = 0; row < pHead->biHeight; row++, pData += nActWidth)
		{
			fread(pData,   1, nActWidth, fp);
			fread(bnDummy, 1, nVirtWidth - nActWidth, fp);
		}
	}

	fclose(fp);
	pHead->biSizeImage = bufSize;

	if(!bTopDown)
		VerticalFlipBuffer(pstImageInfo->pImgData, pHead->biWidth, pHead->biHeight, pHead->biBitCount);

	return 1;
}

void VerticalFlipBuffer(UCHAR *pBuf, int nw, int nh, int nbits)
{
	if(!pBuf)
		return;

	int row, offset1, offset2, nrowlen;
	UCHAR *pTmp;

	nrowlen = nw * nbits / 8;
	pTmp = (UCHAR*)malloc(nrowlen);
	if(!pTmp)
		return;

	for(row = 0; row < nh / 2; row++)
	{
		offset1 = row * nrowlen;
		offset2 = (nh - 1 - row) * nrowlen;
		memcpy(pTmp, pBuf + offset1, nrowlen);
		memcpy(pBuf + offset1, pBuf + offset2, nrowlen);
		memcpy(pBuf + offset2, pTmp, nrowlen);
	}

	free(pTmp);
}

int Gfls_SaveImageFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo)
{
	_strlwr(lpszFileName);
	if(strstr(lpszFileName, ".raw"))
		return Gfls_SaveRawImgFile(lpszFileName, pstImageInfo);
	else
		return Gfls_SaveBmpImgFile(lpszFileName, pstImageInfo);
}

int Gfls_SaveBmpImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo)
{
	int nbits, nw, nh, nColors, btopDown;
	nbits = pstImageInfo->bmpHead.biBitCount;
	nw = pstImageInfo->bmpHead.biWidth;
	nh = abs(pstImageInfo->bmpHead.biHeight);

	if(pstImageInfo->bmpHead.biHeight < 0)
		btopDown = 1;
	else
		btopDown = 0;

	if(nbits != 1 && nbits != 4 && nbits != 8 && nbits != 24)
		return -1;

	WORD  bfType = 0x4d42;	// == 'BM'
	DWORD bfSize, bfReserved, bfOffBits, dwWrites;
	BITMAPINFOHEADER *pHead = &pstImageInfo->bmpHead;
	FILE  *fp;
	int nActWidth, nVirtWidth;
	UCHAR *pImgBuf, bnDummy[4] = {0};

	if(nbits == 24)
		nColors = 0;
	else
	{
		nColors = pHead->biClrUsed;
		if(nColors == 0)
			nColors = (1 << nbits);
	}

	nActWidth  = nw * nbits / 8;
	nVirtWidth = (nActWidth + 3) / 4 * 4;
	
	bfOffBits  = 14 + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColors;
	bfSize     = bfOffBits + nVirtWidth * nh;
	bfReserved = 0;

	dwWrites = 0;

	fp = fopen(lpszFileName, "wb");
	if(NULL == fp)
		return -1;

	// BITMAPFILEHEADER
	fwrite(&bfType,		sizeof(WORD),  1, fp);	
	fwrite(&bfSize,		sizeof(DWORD), 1, fp);	
	fwrite(&bfReserved, sizeof(DWORD), 1, fp);	
	fwrite(&bfOffBits,  sizeof(DWORD), 1, fp);	
	dwWrites += 14;
	
	// BITMAPINFOHEADER
	fwrite(pHead, sizeof(BITMAPINFOHEADER), 1, fp);
	dwWrites += sizeof(BITMAPINFOHEADER);

	// RGBQUAD
	if(nColors > 0)
	{
		fwrite(pstImageInfo->prgbQuad, sizeof(RGBQUAD), nColors, fp);
		dwWrites += sizeof(RGBQUAD) * nColors;
	}

	for(int row = 0; row < nh; row++)
	{
		if(btopDown)
			pImgBuf = pstImageInfo->pImgData + row * nActWidth;
		else
			pImgBuf = pstImageInfo->pImgData + (nh - 1 - row) * nActWidth;

		fwrite(pImgBuf, 1, nActWidth, fp);
		if(nVirtWidth != nActWidth)
			fwrite(bnDummy, 1, nVirtWidth - nActWidth, fp);
	}

	fclose(fp);

	return 1;
}

int Gfls_SaveRawImgFile(char* lpszFileName, GFLS_IMAGEINFOSTRUCT *pstImageInfo)
{
	int nbits, nw, nh, nsize;
	FILE  *fp;

	nbits = pstImageInfo->bmpHead.biBitCount;
	nw = pstImageInfo->bmpHead.biWidth;
	nh = abs(pstImageInfo->bmpHead.biHeight);
	if(nbits != 1 && nbits != 4 && nbits != 8 && nbits != 24)
		return -1;
	nsize = nw * nh * nbits / 8;

	fp = fopen(lpszFileName, "wb");
	if(NULL == fp)
		return -1;
	fwrite(pstImageInfo->pImgData, 1, nsize, fp);
	fclose(fp);
	return 1;
}



__int64	UTIL_HiResoTickCount()
{
	LARGE_INTEGER	tl, nl;

	if ( QueryPerformanceFrequency(&tl) && QueryPerformanceCounter(&nl) ) {
		return nl.QuadPart;
	}
	return	(__int64)GetTickCount();
}

__int64	UTIL_HiResoTickPerSec()
{
	LARGE_INTEGER	tl, nl;

	if ( QueryPerformanceFrequency(&tl) && QueryPerformanceCounter(&nl) ) {
		return tl.QuadPart;
	}
	return	(__int64)CLOCKS_PER_SEC;
}

double	UTIL_HiResoTickToSec(__int64 nTimeUsed)
{
	double	fTime;

	fTime = (double)UTIL_HiResoTickPerSec();
	return	nTimeUsed/fTime;
}

