// CaptureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"
#include "CaptureDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <atomic>
#include "../infissplitbox/infissplitboxdef.h"
#include "FingerShowArea.h"
#include "MainFrm.h"
#include "mosaicInterface.h"
#include "BCApi.h"

#define  MAX_BUF_SIZE 1600 * 1500
#define MIN_GAIN 5
#define MAX_GAIN 25
#define MIN_EXPOSURE 100
#define MAX_EXPOSURE 700

#define CONTRAST_AFTER_CAPTURE 80
#define BRIGHT_AFTER_CAPTURE 50

#define BLACK_ENHANCE_VALUE	1

int EnhanceImage(UCHAR	*pSrcImg, int nImageWidth, int nImageHeight, UCHAR *pEnhanceImg)
{
	if (pSrcImg == NULL || pEnhanceImg == NULL || nImageWidth <= 0 || nImageHeight <= 0)
		return -1;

	//图像增强
	int retval = Contrast_Bright(pSrcImg, nImageWidth, nImageHeight, BRIGHT_AFTER_CAPTURE, CONTRAST_AFTER_CAPTURE, pEnhanceImg);

	return retval;
}

void showImage(HWND hMywnd, CDC *pMyDC, unsigned char *rdata, int x, int y, bool bShowNormal = true)
{
	int i;
	HANDLE hdl;
	unsigned char * mmbuf;
	RECT myrect;
	hdl = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)* 256);
	mmbuf = (unsigned char *)GlobalLock(hdl);
	//Create Bitmapinfo header
	BITMAPINFOHEADER bmi;
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = x;
	bmi.biHeight = -y;
	bmi.biPlanes = 1;
	bmi.biBitCount = 8;
	bmi.biCompression = 0;
	bmi.biSizeImage = 0;
	bmi.biXPelsPerMeter = 0;
	bmi.biYPelsPerMeter = 0;
	bmi.biClrUsed = 256;
	bmi.biClrImportant = 0;
	//load infoheader
	memcpy(mmbuf, &bmi, sizeof(BITMAPINFOHEADER));;
	//create palatte,Bitmap palette: grey scale, 256 levels
	::GetWindowRect(hMywnd, &myrect);
	unsigned char *argbq = (BYTE *)mmbuf + sizeof(BITMAPINFOHEADER);
	for (i = 0; i < 256; i++)
	{
		*argbq++ = i; *argbq++ = i; *argbq++ = i; *argbq++ = 0;
	}

	CDC			memdc;
	memdc.CreateCompatibleDC(pMyDC);
	CBitmap		bmp, *poldbmp;
	bmp.CreateCompatibleBitmap(pMyDC, x, y);  //只有客户区大小   
	//	memdc.SetViewportOrg(-pt);  //设置View的偏移   
	poldbmp = memdc.SelectObject(&bmp);

	SetDIBitsToDevice(memdc.GetSafeHdc(),
		0, 0,
		x, y,
		0, 0,
		0, y,
		rdata,
		(LPBITMAPINFO)mmbuf,
		DIB_RGB_COLORS);

	CPen newpen;
	newpen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	CPen*oldpen;
	oldpen = memdc.SelectObject(&newpen);

	int nCenterX = x / 2;
	int nCenterY = y / 2;
	memdc.MoveTo(nCenterX - 200, nCenterY);
	memdc.LineTo(nCenterX + 200, nCenterY);
	memdc.MoveTo(nCenterX, nCenterY - 200);
	memdc.LineTo(nCenterX, nCenterY + 200);

	memdc.SetBkMode(TRANSPARENT);
	//Display the information
	::SetStretchBltMode(pMyDC->m_hDC, COLORONCOLOR);

	RECT rc;
	GetClientRect(hMywnd, &rc);
	int nSrcLeft = 0;
	int nSrcTop = 0;
	int nSrcW = x;
	int nSrcH = y;
	if (bShowNormal)
	{
		nSrcLeft = (x - (rc.right - rc.left)) / 2;
		nSrcTop = (y - (rc.bottom - rc.top)) / 2;
		nSrcW = rc.right - rc.left;
		nSrcH = rc.bottom - rc.top;
	}


	pMyDC->StretchBlt(-rc.left, -rc.top, rc.right - rc.left, rc.bottom - rc.top,
		&memdc,
		nSrcLeft, nSrcTop,
		nSrcW, nSrcH,
		SRCCOPY);

	GlobalUnlock(mmbuf);
	GlobalFree(hdl);
	return;
}

int Worker::doCapture(void *param)
{
	if (param == NULL)
		return -1;

	static int nElapsedTime = 1;
	static int nFrameNum = 0;

	CCaptureDlg	&dlg = *(CCaptureDlg *)param;

	int i = 0;
	while (!dlg._bStop)
	{
		if (dlg._bReady)
		{
			while (!dlg._bMosaicValid && !dlg._bStop)
			{
				std::unique_lock<std::mutex> locker(dlg._mtxCondMosaicOne);
				dlg._condMosaicOneVar.wait(locker);
			}

			dlg._bMosaicValid = false;

			DWORD tick1 = GetTickCount();
			dlg._fingerSensor.ReadImage(dlg._pImageBuf);
			DWORD tick2 = GetTickCount();

			dlg._bCapValid = true;

			{
				std::unique_lock<std::mutex> locker(dlg._mtxCondCapOne);
				dlg._condCapOneVar.notify_all();
			}

			//如果为四连指，不需要拼接
			//否则显示拼接图像
			dlg._pMosaicBuf;
			//showImage(dlg._stCapture.GetSafeHwnd(), dlg._stCapture.GetDC(), dlg._pImageBuf, dlg._nImgWidth, dlg._nImgHeight, dlg._bShowNormal ? true : false);
			//dlg._stCapture.SetImageInfoAndDraw((char *)dlg._pImageBuf, MAX_BUF_SIZE, CFingerShowArea::SLAP_WIDTH, CFingerShowArea::SLAP_HEIGHT);
			//dlg._stCapture.SetImageInfoAndDraw((char *)dlg._pImageBuf, MAX_BUF_SIZE, CFingerShowArea::FINGER_WIDTH, CFingerShowArea::FINGER_HEIGHT);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));

			// compute frame rate
			++nFrameNum;
			int nTmp = tick2 - tick1;
			if (nTmp < 0)
				nTmp += ULONG_MAX;
			nElapsedTime += nTmp;

			//CString msg = "";
			//msg.Format("%d Frames/second", (int)(nFrameNum * 1000.0f / nElapsedTime + 0.5f));
			//pdis->MyDlg->GetDlgItem(IDC_STATIC_FRAME_RATE)->SetWindowText(msg);
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	return 0;
}

int Worker::doCaptureSlap(void *param)
{
	if (param == NULL)
		return -1;

	static int nElapsedTime = 1;
	static int nFrameNum = 0;

	CCaptureDlg	&dlg = *(CCaptureDlg *)param;

	int		nWidth = CCaptureDlg::SLAP_WIDTH;
	int		nHeight = CCaptureDlg::SLAP_HEIGHT;

	int i = 0;
	while (!dlg._bStop)
	{
		if (dlg._bReady)
		{
			DWORD tick1 = GetTickCount();
			dlg._fingerSensor.ReadImage(dlg._pImageBuf);
			DWORD tick2 = GetTickCount();

			//如果为四连指，不需要拼接
			//否则显示拼接图像
			if (dlg._bNeedEnhance)
			{
				EnhanceImage(dlg._pImageBuf, nWidth, nHeight, dlg._pImageEnhance);
				dlg._stCapture.SetImageInfoAndDraw((char *)dlg._pImageEnhance, MAX_BUF_SIZE, dlg._nImgWidth, dlg._nImgHeight);
			}
			else
				dlg._stCapture.SetImageInfoAndDraw((char *)dlg._pImageBuf, MAX_BUF_SIZE, dlg._nImgWidth, dlg._nImgHeight);
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));

			// compute frame rate
			++nFrameNum;
			int nTmp = tick2 - tick1;
			if (nTmp < 0)
				nTmp += ULONG_MAX;
			nElapsedTime += nTmp;

			//CString msg = "";
			//msg.Format("%d Frames/second", (int)(nFrameNum * 1000.0f / nElapsedTime + 0.5f));
			//pdis->MyDlg->GetDlgItem(IDC_STATIC_FRAME_RATE)->SetWindowText(msg);
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	return 0;
}


int CUtilHelper::UTIL_BuildCaptImageData(UCHAR *pbnDestData, int nDesWidth, int nDesHeight, int nDesBPP,
	UCHAR *pbnSrcData, int nSrcWidth, int nSrcHeight, int ncbSrcLine, int nSrcBPP)
{
	if (pbnDestData == NULL || pbnSrcData == NULL)
		return -1;

	int ncbDesLine = GetBmpImgLine(nDesWidth, nDesBPP);
	int nImgBuffer = ncbDesLine * nDesHeight;

	memset(pbnDestData, 255, nImgBuffer);

	if ((ncbDesLine == ncbSrcLine) && (nDesHeight == nSrcHeight))
	{
		memcpy(pbnDestData, pbnSrcData, ncbDesLine * nDesHeight);
	}
	else
	{
		UCHAR	*pbnTempDes, *pbnTempSrc;
		int		nOffset, ncbCopyLine, nCopyHeight;

		pbnTempDes = pbnDestData;
		pbnTempSrc = pbnSrcData;
		if (nDesHeight <= nSrcHeight)
		{
			nOffset = (nSrcHeight - nDesHeight) / 2;
			//nOffset = (nSrcHeight - nDesHeight) / 1;	//不居中取图，因为指头靠下好采集。
			pbnTempSrc += nOffset * ncbSrcLine;
			nCopyHeight = nDesHeight;
		}
		else
		{
			nOffset = (nDesHeight - nSrcHeight) / 2;
			pbnTempDes += nOffset * ncbDesLine;
			nCopyHeight = nSrcHeight;
		}
		if (nDesWidth <= nSrcWidth)
		{
			nOffset = (nSrcWidth - nDesWidth) / 2;
			pbnTempSrc += GetImageLine(nOffset, nSrcBPP);
			ncbCopyLine = ncbDesLine;
		}
		else if (nDesWidth > nSrcWidth)
		{
			nOffset = (nDesWidth - nSrcWidth) / 2;
			pbnTempDes += GetImageLine(nOffset, nDesBPP);
			ncbCopyLine = ncbSrcLine;
		}
		for (int i = 0; i < nCopyHeight; ++i, pbnTempDes += ncbDesLine, pbnTempSrc += ncbSrcLine)
		{
			memcpy(pbnTempDes, pbnTempSrc, ncbCopyLine);
		}
	}

	return 1;
}

int MallocFingerData(UCHAR * &pData, int &nDataLen)
{
	nDataLen = CCaptureDlg::FINGER_WIDTH * CCaptureDlg::FINGER_HEIGHT;
	//nDataLen = 800 * 750;
	pData = new UCHAR[nDataLen];

	return 0;
}

void FreeFingerCaptData(UCHAR * & pData)
{
	if (pData)
	{
		delete[] pData;
		pData = NULL;
	}
}

int GetFingerData(UCHAR *pFingerData, int nFingerWidth, int nFingerHeight, UCHAR *pImageData, int nImgWidth, int nImgHeight)
{
	/*int nwSrc = CFingerShowArea::SLAP_WIDTH;
	int nhSrc = CFingerShowArea::SLAP_HEIGHT;
	int nBPP = 8;*/
	
	/*if (pFingerData == NULL && (nFingerWidth == nImgWidth && nFingerHeight == nImgHeight))
	{
		pFingerData = pImageData;
		return 0;
	}*/

	if (pFingerData == NULL || nFingerWidth <= 0 || nFingerHeight <= 0)
		return -1;

	int retval = CUtilHelper::UTIL_BuildCaptImageData(pFingerData, nFingerWidth, nFingerHeight, 8,
		pImageData, nImgWidth, nImgHeight, CUtilHelper::GetImageLine(nImgWidth, 8), 8);

	return retval;
}
#if 0
int MallocSlapData(UCHAR * &pData, int &nDataLen)
{
	nDataLen = CCaptureDlg::SLAP_WIDTH * CCaptureDlg::SLAP_HEIGHT;
	//nDataLen = 1600 * 1500;
	pData = new UCHAR[nDataLen];

	return 0;
}

void FreeSlapCaptData(UCHAR * & pData)
{
	if (pData)
	{
		delete[] pData;
		pData = NULL;
	}
}

int GetSlapData(UCHAR *pSlapData, int nSlapWidth, int nSlapHeight, UCHAR *pImageData, int nImgWidth, int nImgHeight)
{
	/*int nwSrc = CFingerShowArea::SLAP_WIDTH;
	int nhSrc = CFingerShowArea::SLAP_HEIGHT;
	int nBPP = 8;*/

	//普通采集，不需要增强和截取
	/*if (pSlapData == NULL && (nSlapWidth == nImgWidth && nSlapHeight == nImgHeight))
	{
		pSlapData = pImageData;
		return 0;
	}*/

	if (pSlapData == NULL || nSlapWidth <= 0 || nSlapHeight <= 0)
		return -1;

	int retval = 0;

	//目前不需要截取，大小正好
	/*retval = CUtilHelper::UTIL_BuildCaptImageData(pSlapData, nSlapWidth, nSlapHeight, 8,
		pImageData, nImgWidth, nImgHeight, CUtilHelper::GetImageLine(nImgWidth, 8), 8);*/

	//图像增强
	//{
	//	//外部提供内存
	//	/*UCHAR	*pEnhanceImg = NULL;
	//	int		nFingerLen = 0;
	//	if (0 > MallocFingerData(pEnhanceImg, nFingerLen) ||
	//		pEnhanceImg == NULL || nFingerLen <= 0)
	//		return -1;*/

	//	retval = Contrast_Bright(pSlapData, nSlapWidth, nSlapHeight, BRIGHT_AFTER_CAPTURE, CONTRAST_AFTER_CAPTURE, pEnhanceImg);

	//	//FreeFingerCaptData(pSlapData);
	//	pSlapData = pEnhanceImg;
	//}

	return retval;
}
#endif

int Worker::doMosaic(void *param)
{
	if (param == NULL)
		return -1;

	CCaptureDlg	&dlg = *(CCaptureDlg *)param;

	//这里居中切分出指纹来
	int		nFingerLen = 0;
	UCHAR	*pFinger = NULL;

	if (0 > MallocFingerData(pFinger, nFingerLen) ||
		pFinger == NULL || nFingerLen <= 0)
		return -1;

	//MOSAIC_SetRollMode(0);
	//先让采集线程可用
	dlg._bMosaicValid = true;

	{
		std::unique_lock<std::mutex> locker(dlg._mtxCondMosaicOne);
		dlg._condMosaicOneVar.notify_all();
	}

	bool	bFindFiner = false;
	int		nret = 1;
	int		nWidth = CCaptureDlg::FINGER_WIDTH;
	int		nHeight = CCaptureDlg::FINGER_HEIGHT;

	//int		nWidth = 800;
	//int		nHeight = 750;
	while (!dlg._bStopMosaic)
	{
		//if (dlg._bReady)
		{	
			while (!dlg._bCapValid && !dlg._bStopMosaic)
			{
				std::unique_lock<std::mutex> locker(dlg._mtxCondCapOne);
				dlg._condCapOneVar.wait(locker);
			}
			
			dlg._bCapValid = false;

			if (!bFindFiner)
			{
				while (!dlg._bStopMosaic)
				{
					GetFingerData(pFinger, nWidth, nHeight, dlg._pImageBuf, dlg._nImgWidth, dlg._nImgHeight);

					if (MOSAIC_IsFinger(pFinger, nWidth, nHeight) > 0)
					{
						bFindFiner = true;
						break;
					}

					dlg._bMosaicValid = true;

					{
						std::unique_lock<std::mutex> locker(dlg._mtxCondMosaicOne);
						dlg._condMosaicOneVar.notify_all();
					}
				}
			}
			else
				GetFingerData(pFinger, nWidth, nHeight, dlg._pImageBuf, dlg._nImgWidth, dlg._nImgHeight);

			if (dlg._bNeedEnhance)
			{
				EnhanceImage(pFinger, nWidth, nHeight, dlg._pImageEnhance);
				nret = MOSAIC_DoMosaic((UCHAR *)dlg._pImageEnhance, nWidth, nHeight);
			}
			else
				nret = MOSAIC_DoMosaic((UCHAR *)pFinger, nWidth, nHeight);
			//GF_GAMOSAIC_ERROR_NOLICENSE;

			dlg._stCapture.SetImageInfoAndDraw((char *)dlg._pMosaicBuf, nFingerLen, nWidth, nHeight);

			if (nret != 1)
			{
				break;
			}

			dlg._bMosaicValid = true;

			{
				std::unique_lock<std::mutex> locker(dlg._mtxCondMosaicOne);
				dlg._condMosaicOneVar.notify_all();
			}
		}
		//else
		//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	if (nret != 1 && !dlg._bStopMosaic)
	{
		int bReCapture = !dlg.OnFinishCapture(nret);
		::PostMessage(dlg.GetSafeHwnd(), WM_MOSAIC_END, nret, bReCapture);
	}

	FreeFingerCaptData(pFinger);

	return 0;
}


// CCaptureDlg 对话框

IMPLEMENT_DYNAMIC(CCaptureDlg, CDialogEx)

CCaptureDlg::CCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCaptureDlg::IDD, pParent)
{
	_strSlapString[SLAP_R_FOUR - 1] = _T("Right Four Finger");
	_strSlapString[SLAP_L_FOUR - 1] = _T("Left Four Finger");
	_strSlapString[SLAP_RL_THUMB - 1] = _T("Double Thumb");

	_strFingerString[FINGER_R_THUMB - 1] = _T("Right Thumb");
	_strFingerString[FINGER_R_INDEX - 1] = _T("Right Index");
	_strFingerString[FINGER_R_MIDDLE - 1] = _T("Right Middle");
	_strFingerString[FINGER_R_RING - 1] = _T("Right Ring");
	_strFingerString[FINGER_R_LITTLE - 1] = _T("Right Little");

	_strFingerString[FINGER_L_THUMB - 1] = _T("Left Thumb");
	_strFingerString[FINGER_L_INDEX - 1] = _T("Left Index");
	_strFingerString[FINGER_L_MIDDLE - 1] = _T("Left Middle");
	_strFingerString[FINGER_L_RING - 1] = _T("Left Ring");
	_strFingerString[FINGER_L_LITTLE - 1] = _T("Left Little");

	memset(_bSlapCanCapture, 1, sizeof(_bSlapCanCapture));
	memset(_bRollCanCapture, 1, sizeof(_bRollCanCapture));
	memset(_bFlatCanCapture, 1, sizeof(_bFlatCanCapture));

	_nFgp2Led[FINGER_R_THUMB - 1] = 7;
	_nFgp2Led[FINGER_R_INDEX - 1] = 8;
	_nFgp2Led[FINGER_R_MIDDLE - 1] = 9;
	_nFgp2Led[FINGER_R_RING - 1] = 10;
	_nFgp2Led[FINGER_R_LITTLE - 1] = 11;

	_nFgp2Led[FINGER_L_THUMB - 1] = 6;
	_nFgp2Led[FINGER_L_INDEX - 1] = 5;
	_nFgp2Led[FINGER_L_MIDDLE - 1] = 4;
	_nFgp2Led[FINGER_L_RING - 1] = 3;
	_nFgp2Led[FINGER_L_LITTLE - 1] = 2;
}

CCaptureDlg::~CCaptureDlg()
{
	if (_pImageBuf)
	{
		delete[] _pImageBuf;
		_pImageBuf = NULL;
	}

	if (_pMosaicBuf)
	{
		delete[] _pMosaicBuf;
		_pMosaicBuf = NULL;
	}

	if (_pImageEnhance)
	{
		delete[] _pImageEnhance;
		_pImageEnhance = NULL;
	}
}

void CCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CAPTURE, _stCapture);
	DDX_Control(pDX, IDC_CHK_SHOWNORMAL, _chkShowNormal);
	DDX_Check(pDX, IDC_CHK_SHOWNORMAL, _bShowNormal);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, _sliderContrast);
	DDX_Control(pDX, IDC_SLIDER_BRIGHT, _sliderBright);
	DDX_Control(pDX, IDC_EDIT_CONTRAST, _editContrast);
	DDX_Control(pDX, IDC_EDIT_BRIGHT, _editBright);
	DDX_Control(pDX, IDC_PROGRESS1, _proImgQual);
	DDX_Control(pDX, IDC_STATIC_CAPTURETYPE, _stCaptureType);
	DDX_Control(pDX, ID_BTN_OK, _btnStart);
	DDX_Control(pDX, IDC_BTN_CONFIRM, _btnConfirm);
	DDX_Control(pDX, IDC_BTN_STOP, _btnStop);
}


BEGIN_MESSAGE_MAP(CCaptureDlg, CDialogEx)
	ON_BN_CLICKED(ID_BTN_EXIT, &CCaptureDlg::OnExit)
	ON_BN_CLICKED(ID_BTN_OK, &CCaptureDlg::OnStartCapture)
	ON_BN_CLICKED(IDC_CHK_SHOWNORMAL, &CCaptureDlg::OnShowNormal)
	ON_BN_CLICKED(ID_BTN_SPLIT, &CCaptureDlg::OnSplit)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONTRAST, &CCaptureDlg::OnNMCustomdrawSliderContrast)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BRIGHT, &CCaptureDlg::OnNMCustomdrawSliderBright)
	ON_EN_CHANGE(IDC_EDIT_CONTRAST, &CCaptureDlg::OnEnChangeEditContrast)
	ON_BN_CLICKED(IDC_BTN_CONFIRM, &CCaptureDlg::OnCaptureConfirm)
	ON_BN_CLICKED(IDC_BTN_NEXTSTEP, &CCaptureDlg::OnNextStep)
	ON_BN_CLICKED(IDC_BTN_PRESTEP, &CCaptureDlg::OnPreStep)
	ON_BN_CLICKED(IDC_BTN_STOP, &CCaptureDlg::OnStopCapture)
	ON_MESSAGE(WM_MOSAIC_END, &CCaptureDlg::OnMosaicEnd)
END_MESSAGE_MAP()


// CCaptureDlg 消息处理程序


void CCaptureDlg::OnExit()
{
	// TODO:  在此添加控件通知处理程序代码
	if (_bCapturingSlap)
	{
		AfxMessageBox(_T("Capturing, click Stop first"));
		return;
	}

	if (_bMosaicFinishing)
	{
		AfxMessageBox(_T("Mosaic quality messagebox is on show, click yes/no to finish"));
		return;
	}

	int nret = AfxMessageBox(_T("confirm to save ?"), MB_YESNOCANCEL);
	switch (nret)
	{
	case IDCANCEL:
		if (0 > _ctrlCap.getCurrentCapType())
		{
			switch (_ctrlCap.preCapProc())
			{
			case CAPTURE_ROLLFINGER:
				RestartCaptureThread();
				RestartMosaicThread();
				return;
			}
		}

		return;
	case IDYES:

		return CCaptureDlg::OnOK();
	case IDNO:
	
		return CCaptureDlg::OnCancel();
	default:
		return;
	}
}

void  CCaptureDlg::CloseAllLed()
{
	if (!_bReady)
		return;

	for (int i = 1; i <= 11; ++i)
	{
		_fingerSensor.SetGpioStatus(i, false);
	}
}

void CCaptureDlg::SetCurLed(BOOL bRoll)
{
	if (!_bReady)
		return;

	if (bRoll)
		_fingerSensor.SetGpioStatus(1, true);
	else
		_fingerSensor.SetGpioStatus(1, false);

	int nCurFgp = _ctrlCap.getCurFgp();
	switch (_ctrlCap.getCurrentCapType())
	{
	case CAPTURE_SLAP:

		for (int i = FINGER_R_THUMB; i <= FINGER_L_LITTLE; ++i)
		{
			if (nCurFgp == SLAP_R_FOUR)
			{
				if (i > FINGER_R_THUMB && i <= FINGER_R_LITTLE)
					_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], true);
				else
					_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], false);
			}
			else if (nCurFgp == SLAP_L_FOUR)
			{
				if (i > FINGER_L_THUMB && i <= FINGER_L_LITTLE)
					_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], true);
				else
					_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], false);
			}
			else if (nCurFgp == SLAP_RL_THUMB)
			{
				if (i == FINGER_R_THUMB || i == FINGER_L_THUMB)
					_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], true);
				else
					_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], false);
			}
		}

		break;
	case CAPTURE_ROLLFINGER:
	case CAPTURE_FLATFINGER:

		for (int i = FINGER_R_THUMB; i <= FINGER_L_LITTLE; ++i)
		{
			if (i == nCurFgp)
				_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], true);
			else
				_fingerSensor.SetGpioStatus(_nFgp2Led[i - 1], false);
		}
		break;
	default:
		return;
	}
	
	return;
}

void CCaptureDlg::ShowCurCaptureType()
{
	if (_ctrlCap.getCurrentCapType() <= 0)
	{
		AfxMessageBox(_T("There's no exist finger!"));
		return OnExit();
	}

	int nCurCapType = _ctrlCap.getCurrentCapType();
	int nCurFgp = _ctrlCap.getCurFgp();
	switch (nCurCapType)
	{
	case CAPTURE_ROLLFINGER:

		if (IsWindow(_stCaptureType.GetSafeHwnd()))
			_stCaptureType.SetWindowText(_strFingerString[nCurFgp - 1]);

		SetCurLed(TRUE);
		break;
	case CAPTURE_SLAP:

		if (IsWindow(_stCaptureType.GetSafeHwnd()))
			_stCaptureType.SetWindowText(_strSlapString[nCurFgp - 1]);

		CloseAllLed();
		SetCurLed(FALSE);
		break;
	}
}

void CCaptureDlg::SetFingerNotExist(BOOL bExistArray[10])
{
	for (auto &bExist : _bRollCanCapture)
	{
		bExist = *bExistArray ? true : false;
		bExistArray++;
	}
}

void CCaptureDlg::PushCaptureProc(const vct_captype_t &vctCapType)
{
	CaptureControl::vct_capproc_t	vctCapProc;

	for (auto &captype : vctCapType)
	{
		CaptureControl::CaptureType	cap;

		switch (captype.first)
		{
		case CAPTURE_ROLLFINGER:

			for (int i = 0; i < sizeof(_bRollCanCapture) / sizeof(bool); ++i)
			{
				if (_bRollCanCapture[i])
				{
					cap._nType = captype.first;
					cap._nFgp = i + 1;
					cap._bCaptured = false;
					cap._bHalfWindow = true;
					cap._nGain = captype.second.first;
					cap._nExposure = captype.second.second;

					vctCapProc.push_back(cap);
				}
			}
			
			break;
		case CAPTURE_SLAP:

			for (int i = SLAP_R_FOUR; i <= SLAP_RL_THUMB; ++i)
			{
				cap._nType = captype.first;
				cap._nFgp = i;
				cap._bCaptured = false;
				cap._bHalfWindow = false;
				cap._nGain = captype.second.first;
				cap._nExposure = captype.second.second;

				vctCapProc.push_back(cap);
			}

			break;
		case CAPTURE_FLATFINGER:

			//not support;
			break;
		}
	}

	_ctrlCap.setCaptureProc(vctCapProc);
	
	//ShowCurCaptureType();
}


int CCaptureDlg::ResetDevice()
{
	// 获取相机个数
	int nCount = _fingerSensor.GetCameraCount();

	char szCamName[128] = { 0 };
	int nCamType = 0;

	if (nCount > 0)
	{
		for (int i = 0; i < nCount; i++)
		{
			_fingerSensor.GetCameraName(i, szCamName, nCamType);

			strncat(szCamName, "  ", sizeof(szCamName));
			if (CAM_A2000 == nCamType)
				strncat(szCamName, "A2000", sizeof(szCamName));
			else if (CAM_A3000 == nCamType)
				strncat(szCamName, "A3000", sizeof(szCamName));
			else if (CAM_A5000 == nCamType)
				strncat(szCamName, "A5000", sizeof(szCamName));
			else
				strncat(szCamName, "A1000", sizeof(szCamName));

			//m_cbCamList.AddString(szCamName);
		}

		_fingerSensor.Close();

		if (!_fingerSensor.Open(0, true, 0, 0, _bHalfWindow))
		{
			_bReady = false;
			AfxMessageBox(_T("Initial Device Fail!"));
			return FALSE;
		}

		//初始化采集仪参数
		_fingerSensor.SetGain(_nGain);
		_fingerSensor.SetExposure(_nExposure);
		_fingerSensor.SetLed(BLACK_ENHANCE_VALUE);

		_bReady = true;

		CString str;
		if (_fingerSensor.GetDeviceParams(&_devParams))
		{
			//m_cbWindowSize.ResetContent();
			if (_devParams.nCamType == CAM_A1000)
			{
				// A1000 440*440
				_nImgWidth = A1000_WIDTH;
				_nImgHeight = A1000_HEIGHT;

				_nMaxW = A1000_WIDTH;
				_nMaxH = A1000_HEIGHT;
			}
			else if (_devParams.nCamType == CAM_A2000)
			{
				_nImgWidth = A2000_WIDTH;
				_nImgHeight = A2000_HEIGHT;

				_nMaxW = A2000_WIDTH;
				_nMaxH = A2000_HEIGHT;
			}
			else if (_devParams.nCamType == CAM_A3000)
			{
				// A3000 640*640
				if (_bHalfWindow)
				{
					//m_cbWindowSize.SetCurSel(1);
					_nImgWidth = A3000_WIDTH / 2;
					_nImgHeight = A3000_HEIGHT / 2;

					_nMaxW = A3000_WIDTH / 2;
					_nMaxH = A3000_HEIGHT / 2;
				}
				else
				{
					//m_cbWindowSize.SetCurSel(0);
					_nImgWidth = A3000_WIDTH;
					_nImgHeight = A3000_HEIGHT;

					_nMaxW = A3000_WIDTH;
					_nMaxH = A3000_HEIGHT;
				}
			}
			else if (_devParams.nCamType == CAM_A5000)
			{
				// A5000 1600*1500
				if (_bHalfWindow)
				{
					//m_cbWindowSize.SetCurSel(1);
					_nImgWidth = A5000_WIDTH / 2;
					_nImgHeight = A5000_HEIGHT / 2;


					_nMaxW = A5000_WIDTH / 2;
					_nMaxH = A5000_HEIGHT / 2;
				}
				else
				{
					//m_cbWindowSize.SetCurSel(0);
					_nImgWidth = A5000_WIDTH;
					_nImgHeight = A5000_HEIGHT;

					_nMaxW = A5000_WIDTH;
					_nMaxH = A5000_HEIGHT;
				}
			}
		}
	}

	return 0;
}

BOOL CCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (_ctrlCap.getCurrentCapType() <= 0)
	{
		AfxMessageBox(_T("There's no exist finger!"));
		OnCancel();
		return FALSE;
	}

	_sliderContrast.SetRange(MIN_GAIN, MAX_GAIN);
	_sliderBright.SetRange(MIN_EXPOSURE, MAX_EXPOSURE);

	_nGain = _ctrlCap.getCurGain();
	_nExposure = _ctrlCap.getCurExposure();
	_sliderContrast.SetPos(_nGain);
	_sliderBright.SetPos(_nExposure);

	_pImageBuf = new UCHAR[MAX_BUF_SIZE];
	_pMosaicBuf = new UCHAR[MAX_BUF_SIZE];
	_pImageEnhance = new UCHAR[MAX_BUF_SIZE];
	if (_pImageBuf == NULL || _pMosaicBuf == NULL || _pImageEnhance == NULL)
	{
		AfxMessageBox(_T("Memory allocate fail!"));
		CDialog::OnCancel();
		
		return FALSE;
	}

	memset(_pImageBuf, 0, MAX_BUF_SIZE);
	memset(_pMosaicBuf, 0, MAX_BUF_SIZE);
	memset(_pImageEnhance, 0, MAX_BUF_SIZE);

	int nret = MOSAIC_Init();
	if (nret == -7)
	{
		AfxMessageBox(_T("init mosaic failed"));
		return FALSE;
	}

	_bHalfWindow = _ctrlCap.isCurHalfWindow();
	_bReady = false;
	ResetDevice();

	//由于要设置led灯，需要等设备启动才调用
	ShowCurCaptureType();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


int CCaptureDlg::RestartCaptureThread()
{
	ExitCaptureThread();

	_bStop = false;

	memset(_pImageBuf, 255, MAX_BUF_SIZE);
	//可能会跟拼接线程(或四联指确认采集)中的同名操作冲突（正在拼接，并重新采集。似乎不可能）
	_stCapture.SetImageInfoAndDraw((char *)_pImageBuf, MAX_BUF_SIZE, _nImgWidth, _nImgHeight);

	switch (_ctrlCap.getCurrentCapType())
	{
	case CAPTURE_SLAP:
		_vctCapThread.push_back(std::thread(Worker::doCaptureSlap, this));
		
		break;
	case CAPTURE_ROLLFINGER:
		_vctCapThread.push_back(std::thread(Worker::doCapture, this));

		break;
	case CAPTURE_FLATFINGER:
	default:
		return 0;
	}

	SetThreadPriority(_vctCapThread[0].native_handle(), THREAD_PRIORITY_NORMAL);

	return 0;
}


int CCaptureDlg::ExitCaptureThread()
{
	if (!_vctCapThread.empty())
	{
		_bStop = true;

		//Sleep(1000);

		_condMosaicOneVar.notify_all();
		_vctCapThread[0].join();
		_vctCapThread.clear();
	}

	return 0;
}


int CCaptureDlg::RestartMosaicThread()
{
	ExitMosaicThread();

	memset(_pMosaicBuf, 0, MAX_BUF_SIZE);
	MOSAIC_Start(_pMosaicBuf, 640, 640);

	_bStopMosaic = false;
	_vctMosaicThread.push_back(std::thread(Worker::doMosaic, this));
	SetThreadPriority(_vctMosaicThread[0].native_handle(), THREAD_PRIORITY_NORMAL);

	return 0;
}


int CCaptureDlg::ExitMosaicThread()
{
	if (!_vctMosaicThread.empty())
	{
		_bStopMosaic = true;

		//Sleep(1000);

		_condCapOneVar.notify_all();
		_vctMosaicThread[0].join();
		_vctMosaicThread.clear();

		MOSAIC_Stop();
	}

	return 0;
}


void CCaptureDlg::OnStartCapture()
{
	// TODO:  在此添加控件通知处理程序代码

	{
		switch (_ctrlCap.getCurrentCapType())
		{
		case CAPTURE_SLAP:

			_bCapturingSlap = TRUE;

			_btnStart.ShowWindow(FALSE);
			_btnStop.ShowWindow(TRUE);
			_btnConfirm.ShowWindow(TRUE);
			break;
		case CAPTURE_ROLLFINGER:
		case CAPTURE_FLATFINGER:

			_btnStart.ShowWindow(FALSE);
			_btnStop.ShowWindow(FALSE);
			_btnConfirm.ShowWindow(FALSE);
			RestartMosaicThread();
			break;
		}
		
		RestartCaptureThread();
	}
}


void CCaptureDlg::OnShowNormal()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();

	_bHalfWindow = _bShowNormal ? true : false;
	_bReady = false;

	RestartCaptureThread();
	RestartMosaicThread();

	ResetDevice();
}


void CCaptureDlg::OnSplit()
{
	// TODO:  在此添加控件通知处理程序代码
	char	*pImage = NULL;

	int		nCurCaptureType = _ctrlCap.getCurrentCapType();
	int		nCurFgp = _ctrlCap.getCurFgp();
	if (nCurCaptureType != CAPTURE_SLAP || nCurFgp > SLAP_RL_THUMB || nCurFgp < SLAP_R_FOUR)
		return;

	auto & mapSplit = _mapSplitedSlap[nCurFgp - 1];
	auto & strSlap = _strImageSlap[nCurFgp - 1];

	mapSplit.clear();
	if (_bNeedEnhance)
		strSlap.assign((char *)_pImageEnhance, _nImgWidth * _nImgHeight);
	else
		strSlap.assign((char *)_pImageBuf, _nImgWidth * _nImgHeight);

	pImage = (char *)strSlap.c_str();

	//停止采集线程
	ExitCaptureThread();

	//没有拼接线程
	//ExitMosaicThread();

	Infis_Image	*pInfisImg = NULL;
	int			nCount = 0;
	if (0 > INFIS_CutImage2Mem(nCurFgp, (UCHAR *)pImage, _nImgWidth, _nImgHeight, this, &pInfisImg, &nCount) ||
		pInfisImg == NULL || nCount <= 0)
	{
		AfxMessageBox(_T("get splited image failed"));
	}

	for (int i = 0; i < nCount; ++i)
	{
		std::string	strImage(pInfisImg[i].pImage, pInfisImg[i].nImageLen);

		mapSplit[pInfisImg[i].nFgp] = std::move(strImage);
	}

	INFIS_FreeCutImage(pInfisImg, nCount);

	/*m_pCaptView->ExtractFingerMntEx(btyStatus, nError, szErrorInfo, sizeof(szErrorInfo)-1,
		fingerQual.m_nPlainState[nIndex], fingerQual.m_nPlainExf[nIndex]);

	retval = m_pCaptView->CheckFingerImageQualityEx(btyStatus, nError, szErrorInfo, true);
	if (retval > 0) retval = m_pCaptView->CheckFingerCrossAndSame(btyStatus);*/
}


void CCaptureDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CCaptureDlg::OnCancel();

	CDialogEx::OnClose();
}


void CCaptureDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类
	ExitCaptureThread();
	ExitMosaicThread();

	MOSAIC_Close();
	CloseAllLed();
	_fingerSensor.Close();

	CDialogEx::OnCancel();
}


void CCaptureDlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类
	ExitCaptureThread();
	ExitMosaicThread();

	MOSAIC_Close();
	CloseAllLed();
	_fingerSensor.Close();

	CDialogEx::OnOK();
}


void CCaptureDlg::OnNMCustomdrawSliderContrast(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	int nPos = _sliderContrast.GetPos();

	_fingerSensor.SetGain(nPos);
	_nGain = nPos;

	TCHAR	szBuf[64] = { 0 };
	_stprintf(szBuf, _T("%d"), nPos);
	_editContrast.SetWindowText(szBuf);

	*pResult = 0;
}


void CCaptureDlg::OnNMCustomdrawSliderBright(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	int nPos = _sliderBright.GetPos();

	_fingerSensor.SetExposure(nPos);
	_nExposure = nPos;

	TCHAR	szBuf[64] = { 0 };
	_stprintf(szBuf, _T("%d"), nPos);
	_editBright.SetWindowText(szBuf);

	*pResult = 0;
}


void CCaptureDlg::OnEnChangeEditContrast()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	
}


void CCaptureDlg::OnCaptureConfirm()
{
	// TODO:  在此添加控件通知处理程序代码
	if (_vctCapThread.empty())
		return;

	int nCurFgp = _ctrlCap.getCurFgp();
	switch (_ctrlCap.getCurrentCapType())
	{
	case CAPTURE_SLAP:
		OnSplit();

		break;
	default:

		return;
	}

	if (_bCapturingSlap)
	{
		_btnStart.ShowWindow(TRUE);
		_btnStop.ShowWindow(FALSE);
		_bCapturingSlap = FALSE;
	}

	//未居中切割
	//std::string	&str = _strFlatFinger[_nCurFgp - 1];
	//str.assign((char *)_pImageBuf, MAX_BUF_SIZE);

	_stCapture.SetImageInfoAndDraw(_strImageSlap[nCurFgp - 1].c_str(), _strImageSlap[nCurFgp - 1].size(), SLAP_WIDTH, SLAP_HEIGHT);

	auto & mapSplit = _mapSplitedSlap[nCurFgp - 1];
	if (mapSplit.empty())
	{
		//切割失败，不应下一步
		//清除显示图像
		_stCapture.SetImageInfoAndDraw(0, 0, SLAP_WIDTH, SLAP_HEIGHT);
		return;
	}
	else
	{
		//下一步前先清除显示图像
		_stCapture.SetImageInfoAndDraw(0, 0, SLAP_WIDTH, SLAP_HEIGHT);

		OnNextStep();
	}
}


void CCaptureDlg::OnNextStep()
{
	if (_bCapturingSlap)
	{
		AfxMessageBox(_T("Capturing, click Stop first"));
		return;
	}

	if (_bMosaicFinishing)
	{
		AfxMessageBox(_T("Mosaic quality messagebox is on show, click yes/no to finish"));
		return;
	}

	int nCurCapType = _ctrlCap.getCurrentCapType();
	switch (nCurCapType)
	{
	case CAPTURE_SLAP:
		{
			_ctrlCap.finishCurCapProc();
			int nNextCapProc = _ctrlCap.nextCapProc();
			if (0 >= nNextCapProc)
				return OnExit();	//后面没有采集类型了
			else
			{
				if (nNextCapProc != nCurCapType)
				{
					_btnConfirm.ShowWindow(FALSE);
					_btnStart.ShowWindow(FALSE);
					_bHalfWindow = _ctrlCap.isCurHalfWindow();
					_bReady = false;

					//Slap->Roll
					RestartCaptureThread();
					RestartMosaicThread();

					_nGain = _ctrlCap.getCurGain();
					_nExposure = _ctrlCap.getCurExposure();
					_sliderContrast.SetPos(_nGain);
					_sliderBright.SetPos(_nExposure);

					ResetDevice();
				}
			}
		}

		break;
	case CAPTURE_ROLLFINGER:
		{
			_ctrlCap.finishCurCapProc();
			int nNextCapProc = _ctrlCap.nextCapProc();
			if (0 >= nNextCapProc)
				return OnExit();	//后面没有采集类型了
			else
			{
				if (nNextCapProc != nCurCapType)
				{
					_btnConfirm.ShowWindow(TRUE);
					_btnStart.ShowWindow(TRUE);
					_bHalfWindow = _ctrlCap.isCurHalfWindow();
					_bReady = false;

					//RestartCaptureThread();
					//RestartMosaicThread();
					ExitCaptureThread();
					ExitMosaicThread();
					//Roll->Slap  点击Start再启动

					_nGain = _ctrlCap.getCurGain();
					_nExposure = _ctrlCap.getCurExposure();
					_sliderContrast.SetPos(_nGain);
					_sliderBright.SetPos(_nExposure);

					ResetDevice();
				}
				else
					RestartMosaicThread();
			}
		}

		break;
	case CAPTURE_FLATFINGER:
		//暂不支持
	default:
		return;
	}

	ShowCurCaptureType();
	//RestartMosaicThread();
}


void CCaptureDlg::OnPreStep()
{
	if (_bCapturingSlap)
	{
		AfxMessageBox(_T("Capturing, click Stop first"));

		return;
	}

	if (_bMosaicFinishing)
	{
		AfxMessageBox(_T("Mosaic quality messagebox is on show, click yes/no to finish"));
		return;
	}

	int nCurCapType = _ctrlCap.getCurrentCapType();
	switch (nCurCapType)
	{
	case CAPTURE_SLAP:
		{
			//_ctrlCap.finishCurCapProc();
			int nPreCapProc = _ctrlCap.preCapProc();
			if (0 >= nPreCapProc)
			{
				//前面没有采集类型了
				return;

				//实际不会到达这一步，因为_ctrlCap.preCapProc()里面不会返回-1；
			}
			else
			{
				if (nPreCapProc != nCurCapType)
				{
					_btnConfirm.ShowWindow(FALSE);
					_btnStart.ShowWindow(FALSE);
					_bHalfWindow = _ctrlCap.isCurHalfWindow();
					_bReady = false;

					//Slap->Roll
					RestartCaptureThread();
					RestartMosaicThread();

					_nGain = _ctrlCap.getCurGain();
					_nExposure = _ctrlCap.getCurExposure();
					_sliderContrast.SetPos(_nGain);
					_sliderBright.SetPos(_nExposure);

					ResetDevice();
				}
			}
		}

		break;
	case CAPTURE_ROLLFINGER:
		{
			//_ctrlCap.finishCurCapProc();
			int nPreCapProc = _ctrlCap.preCapProc();
			if (0 >= nPreCapProc)
			{
				//前面没有采集类型了
				switch (_ctrlCap.nextCapProc())
				{
				case CAPTURE_ROLLFINGER:
					RestartCaptureThread();
					RestartMosaicThread();
					break;
				}

				return;	

				//实际不会到达这一步，因为_ctrlCap.preCapProc()里面不会返回-1；
			}
			else
			{
				if (nPreCapProc != nCurCapType)
				{
					_btnConfirm.ShowWindow(TRUE);
					_btnStart.ShowWindow(TRUE);
					_bHalfWindow = _ctrlCap.isCurHalfWindow();
					_bReady = false;

					//RestartCaptureThread();
					//RestartMosaicThread();
					ExitCaptureThread();
					ExitMosaicThread();
					//Roll->Slap  点击Start再启动

					_nGain = _ctrlCap.getCurGain();
					_nExposure = _ctrlCap.getCurExposure();
					_sliderContrast.SetPos(_nGain);
					_sliderBright.SetPos(_nExposure);

					ResetDevice();
				}
				else
					RestartMosaicThread();
			}
		}

		break;
	case CAPTURE_FLATFINGER:
		//暂不支持
	default:
		return;
	}

	ShowCurCaptureType();
	//RestartMosaicThread();
}


void CCaptureDlg::OnStopCapture()
{
	// TODO:  在此添加控件通知处理程序代码
	_bCapturingSlap = FALSE;

	{
		switch (_ctrlCap.getCurrentCapType())
		{
		case CAPTURE_SLAP:

			_btnStart.ShowWindow(TRUE);
			_btnStop.ShowWindow(FALSE);
			_btnConfirm.ShowWindow(TRUE);
			break;
		case CAPTURE_ROLLFINGER:
		case CAPTURE_FLATFINGER:

			_btnStart.ShowWindow(FALSE);
			_btnStop.ShowWindow(FALSE);
			_btnConfirm.ShowWindow(FALSE);
			break;
		}

	}

	ExitCaptureThread();
	ExitMosaicThread();
}


bool CCaptureDlg::OnFinishCapture(int nret)
{
	if (nret != 1)
	{
		//询问是否采集下一枚。
		char pRltStr[512] = { 0 };
		sprintf(pRltStr, ("scceed to capture image"));

		if (0 <= nret)
		{
			nret = MOSAIC_ImageQuality(_pMosaicBuf, FINGER_WIDTH, FINGER_HEIGHT);
			//pView->DrawImg(m_nWidth, m_nHeight, m_pRaw, CPoint(640, 0));//显示

			if (0 < nret)
			{
				sprintf(pRltStr, ("iamge score:%d\n"), nret);
			}
		}
		if (0 > nret)
		{
			MOSAIC_GetErrorInfo(nret, pRltStr);
		}

		char pCapQstn[256] = "\ncontinue to capture next finger？";
		strcat(pRltStr, pCapQstn);

		_bMosaicFinishing = true;
		ATL::CA2W	pwstr(pRltStr);
		if (IDYES == ::AfxMessageBox((LPWSTR)pwstr, MB_YESNO))
		{
			_bMosaicFinishing = false;
			return true;
		}
		else
		{
			_bMosaicFinishing = false;
			return false;
		}
	}

	return true;
}

LRESULT CCaptureDlg::OnMosaicEnd(WPARAM wparam, LPARAM lparam)
{
	int bReCapture = lparam;
	
	int	nCurFgp = _ctrlCap.getCurFgp();
	if (bReCapture == 0)
	{
		_strRollFinger[nCurFgp - 1].assign((char *)_stCapture._pData, FINGER_WIDTH * FINGER_HEIGHT);
		OnNextStep();
		return 1;
	}
	else
		_strRollFinger[nCurFgp - 1].clear();

	RestartMosaicThread();
	//RestartCaptureThread();	//还在waitmosaic

	return 1;
}