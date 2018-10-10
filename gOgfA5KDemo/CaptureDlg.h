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

#define WM_MOSAIC_END WM_USER + 0x101 //是否重采。
// CCaptureDlg 对话框



class CaptureControl
{
public:

	class CaptureType
	{
	public:
		CaptureType()
		{
		}

		int		_nType = 0;
		int		_nFgp = 0;
		bool	_bCaptured = false;
		bool	_bHalfWindow = false;
		int		_nGain = 0;
		int		_nExposure = 0;
	};

	CaptureControl()
	{

	}

	typedef CaptureType	captype_t;
	typedef std::vector<captype_t>	vct_capproc_t;
private:
	//采集流程
	//typedef std::pair<int, bool>		pair_captype_t;
	//captype 2 fgp
	
	vct_capproc_t					_vctCaptureProc;

	//有效指位
	/*typedef std::pair<int, bool>		pair_fgp_t;
	typedef std::map<int, pair_fgp_t>	map_validfgp_t;
	map_validfgp_t						_mapValidFgp;*/

	vct_capproc_t::iterator	_itrCapProc = _vctCaptureProc.end();
	//int				_nCurCapturePos = 0;	//pos in 
	//int				_nCurFgp;

public:
	int setCaptureProc(vct_capproc_t &vctCapType)
	{
		_vctCaptureProc = std::move(vctCapType);
		_itrCapProc = _vctCaptureProc.begin();

		return 0;
	}

	int getCurrentCapType()
	{
		if (_vctCaptureProc.empty() || _itrCapProc == _vctCaptureProc.end())
			return -1;

		return _itrCapProc->_nType;
	}

	int finishCurCapProc()
	{
		if (_vctCaptureProc.empty() || _itrCapProc == _vctCaptureProc.end())
			return -1;

		_itrCapProc->_bCaptured = true;

		return 0;
	}

	int nextCapProc(/*bool bCaptured = true*/)
	{
		//_itrCapProc->_bCaptured = bCaptured;
		if (_itrCapProc != _vctCaptureProc.end())
			_itrCapProc++;
		
		return getCurrentCapType();
	}

	int preCapProc()
	{
		if (_itrCapProc != _vctCaptureProc.begin())
			_itrCapProc--;

		return getCurrentCapType();
	}

	int getCurFgp()
	{
		if (_vctCaptureProc.empty() || _itrCapProc == _vctCaptureProc.end())
			return -1;

		return _itrCapProc->_nFgp;
	}

	bool isCurHalfWindow()
	{
		if (_vctCaptureProc.empty() || _itrCapProc == _vctCaptureProc.end())
			return false;

		return _itrCapProc->_bHalfWindow;
	}

	int getCurGain()
	{
		if (_vctCaptureProc.empty() || _itrCapProc == _vctCaptureProc.end())
			return -1;

		return _itrCapProc->_nGain;
	}

	int getCurExposure()
	{
		if (_vctCaptureProc.empty() || _itrCapProc == _vctCaptureProc.end())
			return -1;

		return _itrCapProc->_nExposure;
	}
};

class CCaptureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCaptureDlg)

public:
	CCaptureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCaptureDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CAPTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	enum
	{
		CAPTURE_SLAP = 1,			
		CAPTURE_ROLLFINGER = 2,		
		CAPTURE_FLATFINGER = 3,	
		CAPTURE_PALM = 4,
	};

	enum	CAPTWNDINFO_ENUM
	{
		CAPTWND_FINGER = 1, CAPTWND_SLAP = 2,
		CAPTWND_PALM = 3, CAPTWND_WRITER = 4, CAPTWND_HAND = 5, CAPTWND_COUNT = 5,
		FINGER_WIDTH = 640, FINGER_HEIGHT = 640,
		SLAP_WIDTH = 1600, SLAP_HEIGHT = 1500,
		PALM_WIDHT = 2304, PALM_HEIGHT = 2304,
		WRITER_WIDTH = 1024, WRITER_HEIGHT = 1968,
		HAND_WIDTH = 2304, HAND_HEIGHT = 3584,
		CAPTWND_INVALIDE = 0
	};

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

	CFingerSensor	_fingerSensor;
	bool			_bHalfWindow = false;;
	bool			_bReady = false;
	int				_nGain = 20;
	int				_nExposure = 298;

	CFingerSensor::DEVICEPARAMS _devParams;

	int				_nImgWidth = 0;
	int				_nImgHeight = 0;
	int				_nMaxW = 0;
	int				_nMaxH = 0;

	UCHAR			*_pImageBuf = NULL;
	UCHAR			*_pMosaicBuf = NULL;

	UCHAR			*_pImageEnhance = NULL;
	bool			_bNeedEnhance = true;
	
	std::vector<std::thread>	_vctCapThread;
	std::vector<std::thread>	_vctMosaicThread;

	std::atomic<bool>	_bStop = false;
	std::atomic<bool>	_bStopMosaic = false;

	std::map<int, std::string>	_mapSplitedSlap[SLAP_RL_THUMB];

	std::string			_strImageSlap[SLAP_RL_THUMB];

	std::string			_strFlatFinger[10];
	std::string			_strRollFinger[10];
	
	bool				_bSlapCanCapture[SLAP_RL_THUMB];
	bool				_bRollCanCapture[10];
	bool				_bFlatCanCapture[10];

	int					_nFgp2Led[10];

	//采集流程控制
	CaptureControl	_ctrlCap;

	CString						_strSlapString[SLAP_RL_THUMB];
	CString						_strFingerString[FINGER_L_LITTLE];

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
	int RestartMosaicThread();
	int ExitMosaicThread();


	BOOL			_bShowNormal = FALSE;
	CImageStatic	_stCapture;
	CButton			_chkShowNormal;
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
	afx_msg void OnShowNormal();
	afx_msg void OnSplit();
	afx_msg void OnClose();
	afx_msg void OnExit();
	afx_msg void OnNMCustomdrawSliderContrast(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderBright(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditContrast();
	afx_msg void OnCaptureConfirm();
	afx_msg void OnNextStep();
	afx_msg void OnPreStep();
	afx_msg void OnStopCapture();
	afx_msg LRESULT OnMosaicEnd(WPARAM wparam, LPARAM lparam);

	bool OnFinishCapture(int nret);
	void ShowCurCaptureType();

	typedef std::pair<int, int> pair_gainandexposure_t;
	typedef std::pair<int, pair_gainandexposure_t> pair_typeandparam_t;
	typedef std::vector<pair_typeandparam_t> vct_captype_t;
	void PushCaptureProc(const vct_captype_t &vctCapType);
	void SetFingerNotExist(BOOL bExistArray[10]);
	int ResetDevice();
	void SetCurLed(BOOL bRoll);
	void CloseAllLed();
	CButton _btnStart;
	CButton _btnConfirm;

	BOOL	_bCapturingSlap = FALSE;
	CButton _btnStop;
	std::atomic<bool>	_bMosaicFinishing = false;	//OnFinishCapture，正在弹出质量判断，应阻止其它按钮生效。
};


class Worker
{
public:

	static int doCaptureSlap(void *param);
	static int doCapture(void *param);
	static int doMosaic(void *param);
};


class CUtilHelper
{
public:
	static	int		GetImageLine(int nWidth, int nBitsPerPixel)
	{
		return (nWidth * nBitsPerPixel + 7) / 8;
	}
	static	int		GetBmpImgLine(int nWidth, int nBitsPerPixel)
	{
		return (nWidth * nBitsPerPixel + 31) / 32 * 4;
	}
	static	int		GetImageSize(int nWidth, int nHeight, int nBitsPerPixel)
	{
		return GetImageLine(nWidth, nBitsPerPixel) * nHeight;
	}
	static	int		GetBmpImgSize(int nWidth, int nHeight, int nBitsPerPixel)
	{
		return GetBmpImgLine(nWidth, nBitsPerPixel) * nHeight;
	}

	static int UTIL_BuildCaptImageData(UCHAR *pbnDestData, int nDesWidth, int nDesHeight, int nDesBPP,
		UCHAR *pbnSrcData, int nSrcWidth, int nSrcHeight, int ncbSrcLine, int nSrcBPP);
};