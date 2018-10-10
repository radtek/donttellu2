// CaptureTest.cpp : 实现文件
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"
#include "CaptureTest.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "CaptureDlg.h"
#include "BCApi.h"

#define  MAX_BUF_SIZE 1600 * 1500
#define MIN_GAIN 5
#define MAX_GAIN 25
#define MIN_EXPOSURE 100
#define MAX_EXPOSURE 700

#define CONTRAST_AFTER_CAPTURE 80
#define BRIGHT_AFTER_CAPTURE 50

#define BLACK_ENHANCE_VALUE	1

extern int EnhanceImage(UCHAR	*pSrcImg, int nImageWidth, int nImageHeight, UCHAR *pEnhanceImg);

// CCaptureTest 对话框

int WorkerTest::doCaptureSlap(void *param)
{
	if (param == NULL)
		return -1;

	static int nElapsedTime = 1;
	static int nFrameNum = 0;

	CCaptureTest	&dlg = *(CCaptureTest *)param;

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


IMPLEMENT_DYNAMIC(CCaptureTest, CDialogEx)

CCaptureTest::CCaptureTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCaptureTest::IDD, pParent)
{

}

CCaptureTest::~CCaptureTest()
{
	if (_pImageBuf)
	{
		delete[] _pImageBuf;
		_pImageBuf = NULL;
	}

	if (_pImageEnhance)
	{
		delete[] _pImageEnhance;
		_pImageEnhance = NULL;
	}
}

void CCaptureTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CAPTURE, _stCapture);
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


BEGIN_MESSAGE_MAP(CCaptureTest, CDialogEx)
	ON_BN_CLICKED(ID_BTN_EXIT, &CCaptureTest::OnExit)
	ON_BN_CLICKED(ID_BTN_OK, &CCaptureTest::OnStartCapture)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONTRAST, &CCaptureTest::OnNMCustomdrawSliderContrast)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BRIGHT, &CCaptureTest::OnNMCustomdrawSliderBright)
	ON_BN_CLICKED(IDC_BTN_CONFIRM, &CCaptureTest::OnCaptureConfirm)
	ON_BN_CLICKED(IDC_BTN_STOP, &CCaptureTest::OnStopCapture)
END_MESSAGE_MAP()


// CCaptureTest 消息处理程序
void CCaptureTest::OnExit()
{
	// TODO:  在此添加控件通知处理程序代码
	if (_bValidGainAndExposure == false)
	{
		int nret = AfxMessageBox(_T("confirm to save bright and contrast?"), MB_YESNOCANCEL);
		switch (nret)
		{
		case IDCANCEL:

			return;
		case IDYES:

			return CCaptureTest::OnOK();
		case IDNO:

			return CCaptureTest::OnCancel();
		default:
			return;
		}
	}
	else
	{
		return CCaptureTest::OnCancel();
	}
}


int CCaptureTest::ResetDevice()
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


BOOL CCaptureTest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (IsWindow(_stCaptureType.GetSafeHwnd()))
	{
		if(_bCaptureSlap)
			_stCaptureType.SetWindowText(_T("Slap Finger"));
		else
			_stCaptureType.SetWindowText(_T("Normal Finger"));
	}

	_sliderContrast.SetRange(MIN_GAIN, MAX_GAIN);
	_sliderBright.SetRange(MIN_EXPOSURE, MAX_EXPOSURE);

	_sliderContrast.SetPos(_nGain);
	_sliderBright.SetPos(_nExposure);

	_pImageBuf = new UCHAR[MAX_BUF_SIZE];
	_pImageEnhance = new UCHAR[MAX_BUF_SIZE];
	if (_pImageBuf == NULL || _pImageEnhance == NULL)
	{
		AfxMessageBox(_T("Memory allocate fail!"));
		CDialog::OnCancel();

		return FALSE;
	}

	memset(_pImageBuf, 0, MAX_BUF_SIZE);
	memset(_pImageEnhance, 0, MAX_BUF_SIZE);

	_bReady = false;
	ResetDevice();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


int CCaptureTest::RestartCaptureThread()
{
	ExitCaptureThread();

	_bStop = false;

	_vctCapThread.push_back(std::thread(WorkerTest::doCaptureSlap, this));

	SetThreadPriority(_vctCapThread[0].native_handle(), THREAD_PRIORITY_NORMAL);

	return 0;
}


int CCaptureTest::ExitCaptureThread()
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


void CCaptureTest::OnStartCapture()
{
	// TODO:  在此添加控件通知处理程序代码
	//_btnStart.ShowWindow(FALSE);
	//_btnStop.ShowWindow(TRUE);
	//_btnConfirm.ShowWindow(TRUE);

	RestartCaptureThread();
}


void CCaptureTest::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CCaptureTest::OnCancel();

	CDialogEx::OnClose();
}


void CCaptureTest::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类
	ExitCaptureThread();

	_fingerSensor.Close();

	CDialogEx::OnCancel();
}


void CCaptureTest::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类
	_bValidGainAndExposure = true;
	_pairGain2Exp.first = _nGain;
	_pairGain2Exp.second = _nExposure;

	ExitCaptureThread();
	_fingerSensor.Close();

	CDialogEx::OnOK();
}


void CCaptureTest::OnNMCustomdrawSliderContrast(NMHDR *pNMHDR, LRESULT *pResult)
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


void CCaptureTest::OnNMCustomdrawSliderBright(NMHDR *pNMHDR, LRESULT *pResult)
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


void CCaptureTest::OnCaptureConfirm()
{
	// TODO:  在此添加控件通知处理程序代码
	if (_vctCapThread.empty())
		return;
	
	if (_bNeedEnhance)
		_strImage.assign((char *)_pImageEnhance, _nImgWidth * _nImgHeight);
	else
		_strImage.assign((char *)_pImageBuf, _nImgWidth * _nImgHeight);

	//停止采集线程
	ExitCaptureThread();

	_stCapture.SetImageInfoAndDraw(_strImage.c_str(), _strImage.size(), _nImgWidth, _nImgHeight);

	_pairGain2Exp.first = _nGain;
	_pairGain2Exp.second = _nExposure;
	_bValidGainAndExposure = true;

	this->OnOK();
}


void CCaptureTest::OnStopCapture()
{
	// TODO:  在此添加控件通知处理程序代码

	{
		_btnStart.ShowWindow(TRUE);
		_btnStop.ShowWindow(FALSE);
		_btnConfirm.ShowWindow(TRUE);
	}

	ExitCaptureThread();
}
