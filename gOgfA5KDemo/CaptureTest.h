#pragma once

#include "afxwin.h"

#include "FingerSensor.h"
#include <thread>
#include <atomic>
#include <vector>
#include "afxcmn.h"
#include <map>
#include "ImageStatic.h"
#include <mutex>
#include <condition_variable>

#define FINGER_GAIN	20
#define FINGER_EXPOSURE	298
#define SLAP_GAIN	20
#define SLAP_EXPOSURE	298
// CCaptureTest 对话框

class CCaptureTest : public CDialogEx
{
	DECLARE_DYNAMIC(CCaptureTest)

public:
	CCaptureTest(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCaptureTest();

// 对话框数据
	enum { IDD = IDD_DLG_CAPTURE_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void SetTestType(bool bSlap, int nGain = 20, int nExposure = 298)
	{
		if (bSlap)
		{
			_bCaptureSlap = true;
			_bHalfWindow = false;
			_nGain = nGain;
			_nExposure = nExposure;
		}
		else
		{
			_bCaptureSlap = false;
			_bHalfWindow = true;
			_nGain = nGain;
			_nExposure = nExposure;
		}
	}

	int GetGainAndExposure(int &nGain, int &nExposure)
	{
		if (!_bValidGainAndExposure)
			return -1;

		nGain = _pairGain2Exp.first;
		nExposure = _pairGain2Exp.second;

		return 0;
	}
public:
	CFingerSensor	_fingerSensor;
	bool			_bCaptureSlap = true;
	bool			_bHalfWindow = false;
	bool			_bReady = false;
	bool			_bValidGainAndExposure = false;
	typedef std::pair<int, int>	pair_gain2exposure_t;
	pair_gain2exposure_t	_pairGain2Exp;
	int				_nGain = FINGER_GAIN;
	int				_nExposure = FINGER_EXPOSURE;

	CFingerSensor::DEVICEPARAMS _devParams;

	int				_nImgWidth = 0;
	int				_nImgHeight = 0;
	int				_nMaxW = 0;
	int				_nMaxH = 0;

	UCHAR			*_pImageBuf = NULL;

	UCHAR			*_pImageEnhance = NULL;
	bool			_bNeedEnhance = true;

	std::string		_strImage;

	std::vector<std::thread>	_vctCapThread;

	std::atomic<bool>	_bStop = false;
	std::atomic<bool>	_bStopMosaic = false;

	//防止主线程与采集线程互相等待
	std::mutex	_mtxLockShow;

	//采集线程取得一帧时notify
	std::atomic<bool>			_bCapValid = false;
	std::mutex					_mtxCondCapOne;
	std::condition_variable		_condCapOneVar;

	//拼接线程完成一帧时notify
	std::atomic<bool>			_bMosaicValid = false;
	std::mutex					_mtxCondMosaicOne;
	std::condition_variable		_condMosaicOneVar;

	int RestartCaptureThread();
	int ExitCaptureThread();

	CImageStatic	_stCapture;
	CSliderCtrl		_sliderContrast;
	CSliderCtrl		_sliderBright;
	CEdit			_editContrast;
	CEdit			_editBright;
	CProgressCtrl	_proImgQual;
	CStatic			_stCaptureType;

	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	afx_msg void OnStartCapture();
	afx_msg void OnClose();
	afx_msg void OnExit();
	afx_msg void OnNMCustomdrawSliderContrast(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderBright(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCaptureConfirm();
	afx_msg void OnStopCapture();
	int ResetDevice();
	CButton _btnStart;
	CButton _btnConfirm;

	CButton _btnStop;
};

class WorkerTest
{
public:

	static int doCaptureSlap(void *param);
	//static int doCapture(void *param);
	//static int doMosaic(void *param);
};