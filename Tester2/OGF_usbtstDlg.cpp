// OGF_usbtstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OGF_usbtst.h"
#include "OGF_usbtstDlg.h"
#include "SDK/FingerSensor.h"
//#include "DlgSplitFlat.h"
//#include "gbaselib/gaimgop.h"
#include "infissplitboxdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_GAIN 5
#define MAX_GAIN 25
#define MIN_EXPOSURE 100
#define MAX_EXPOSURE 700
#define MIN_LED 0
#define MAX_LED 255

bool gpio1 = FALSE;//TRUE ; 
bool gpio2 = FALSE;//TRUE ; 
bool gpio3 = FALSE;//TRUE ; 
bool gpio4 = FALSE;//TRUE ; 
bool gpio5 = FALSE;//TRUE ; 
bool gpio6 = FALSE;//TRUE ; 
bool gpio7 = FALSE;//TRUE ; 
bool gpio8 = FALSE;//TRUE ; 
bool gpio9 = FALSE;//TRUE ; 
bool gpio10 = FALSE;//TRUE ; 
bool gpio11 = FALSE;//TRUE ; 
//
int nSizeSel = 0 ;
bool bHalfWindow = FALSE ; 
int nMaxW = 1600 ;
int nMaxH = 1500 ;
#define  MAX_BUF_SIZE 1600*1500

int nImgWidth = 1600 ;
int nImgHeight = 1500 ;
int nStartRow = 0 ;
int nStartColumn = 0 ;
int nGain = 14 ;
int nExposure = 298 ;
int nLedValue = 190 ; 
CFingerSensor::DEVICEPARAMS DeviceParams ;
BOOL bMatation = FALSE ; 
BYTE* pMatationBuf ;

HANDLE hconbuff;
unsigned char *imagebuff;
HWND hwndCapture;
int row,col;
CDC *pDC;
CStatic m_drawbmp; 
int status;
pmydispar dispar;
DWORD dwThreadId; 
HANDLE hThread;
LPVOID lpParam;
void ThreadDisplay(LPVOID lpparam);
void showraw(HWND hMywnd,CDC *pMyDC,unsigned char *rdata, int x, int y);

BOOL bExit = FALSE ;
BOOL bShowNormal = 0 ;

CFingerSensor fingerSensor;
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

/////////////////////////////////////////////////////////////////////////////
// COGF_usbtstDlg dialog

COGF_usbtstDlg::COGF_usbtstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COGF_usbtstDlg::IDD, pParent)
	, _nCaptureType(0)
{
	//{{AFX_DATA_INIT(COGF_usbtstDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COGF_usbtstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEW, m_drawbmp);
	//{{AFX_DATA_MAP(COGF_usbtstDlg)
	DDX_Control(pDX, IDC_CHECK_REVERSE, m_CheckReverse);
	DDX_Control(pDX, IDC_EDIT_EXP, m_edtExp);
	DDX_Control(pDX, IDC_EDIT_GAIN, m_edtGain);
	DDX_Control(pDX, IDC_SLIDER_EXP, m_SliderExp);
	DDX_Control(pDX, IDC_SLIDER_GAIN, m_SliderGain);
	DDX_Control(pDX, IDC_COMBO_WINDOWSIZE, m_cbWindowSize);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_CAMLIST, m_cbCamList);
	DDX_Control(pDX, IDC_CHECK_NORMAL, m_CheckNormal);
	DDX_Control(pDX, IDC_SLIDER_LED, m_SliderLed);
	DDX_Control(pDX, IDC_EDIT_LED, m_edtLed);
	DDX_Control(pDX, IDC_LED1, m_CheckLed1);
	DDX_Control(pDX, IDC_LED2, m_CheckLed2);
	DDX_Control(pDX, IDC_LED3, m_CheckLed3);
	DDX_Control(pDX, IDC_LED4, m_CheckLed4);
	DDX_Control(pDX, IDC_LED5, m_CheckLed5);
	DDX_Control(pDX, IDC_LED6, m_CheckLed6);
	DDX_Control(pDX, IDC_LED7, m_CheckLed7);
	DDX_Control(pDX, IDC_LED8, m_CheckLed8);
	DDX_Control(pDX, IDC_LED9, m_CheckLed9);
	DDX_Control(pDX, IDC_LED10, m_CheckLed10);
	DDX_Control(pDX, IDC_LED11, m_CheckLed11);
	DDX_CBIndex(pDX, IDC_COMBO_CAPTURETYPE, _nCaptureType);
	DDX_Control(pDX, IDC_COMBO_CAPTURETYPE, _combCaptureType);
	DDX_Control(pDX, IDC_CUT, _btnCut);
	DDX_Control(pDX, IDC_CHK_WSQ, _btnWSQ);
}

BEGIN_MESSAGE_MAP(COGF_usbtstDlg, CDialog)
	//{{AFX_MSG_MAP(COGF_usbtstDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CAPTURE, OnCapture)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDOPEN, OnOpen)
	ON_BN_CLICKED(IDCLED, OnCled)
	ON_BN_CLICKED(IDC_SETREG, OnSetreg)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_GAIN, OnReleasedcaptureSliderGain)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_EXP, OnReleasedcaptureSliderExp)
	ON_CBN_SELCHANGE(IDC_COMBO_WINDOWSIZE, OnSelchangeComboWindowsize)
	ON_BN_CLICKED(IDC_CHECK_REVERSE, OnCheckReverse)
	//}}AFX_MSG_MAP
    ON_STN_CLICKED(IDC_VIEW, &COGF_usbtstDlg::OnStnClickedView)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMLIST, &COGF_usbtstDlg::OnCbnSelchangeComboCamlist)
	ON_BN_CLICKED(IDC_GETREG, &COGF_usbtstDlg::OnBnClickedGetreg)
	ON_BN_CLICKED(IDCANCEL, &COGF_usbtstDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_NORMAL, &COGF_usbtstDlg::OnBnClickedCheckNormal)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_LED, &COGF_usbtstDlg::OnNMReleasedcaptureSliderLed)
	ON_BN_CLICKED(IDC_LED1, &COGF_usbtstDlg::OnBnClickedLed1)
	ON_BN_CLICKED(IDC_LED2, &COGF_usbtstDlg::OnBnClickedLed2)
	ON_BN_CLICKED(IDC_LED3, &COGF_usbtstDlg::OnBnClickedLed3)
	ON_BN_CLICKED(IDC_LED4, &COGF_usbtstDlg::OnBnClickedLed4)
	ON_BN_CLICKED(IDC_LED5, &COGF_usbtstDlg::OnBnClickedLed5)
	ON_BN_CLICKED(IDC_LED6, &COGF_usbtstDlg::OnBnClickedLed6)
	ON_BN_CLICKED(IDC_LED7, &COGF_usbtstDlg::OnBnClickedLed7)
	ON_BN_CLICKED(IDC_LED8, &COGF_usbtstDlg::OnBnClickedLed8)
	ON_BN_CLICKED(IDC_LED9, &COGF_usbtstDlg::OnBnClickedLed9)
	ON_BN_CLICKED(IDC_LED10, &COGF_usbtstDlg::OnBnClickedLed10)
	ON_BN_CLICKED(IDC_LED11, &COGF_usbtstDlg::OnBnClickedLed11)
	ON_BN_CLICKED(IDC_CUT, &COGF_usbtstDlg::OnBnClickedCut)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURETYPE, &COGF_usbtstDlg::OnCbnSelchangeComboCapturetype)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COGF_usbtstDlg message handlers

BOOL COGF_usbtstDlg::OnInitDialog()
{
	status=1 ;
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetDlgItemText(IDC_REGISTER,"0x35");
	SetDlgItemText(IDC_VALUE, "0x0028") ;
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_VIEW, &hwndCapture);
	pDC=m_drawbmp.GetDC();
	pMatationBuf=(BYTE *)malloc(MAX_BUF_SIZE);
	imagebuff=(unsigned char *)malloc(MAX_BUF_SIZE);
	if(imagebuff==NULL)
	{
		MessageBox("Memory allocate fail!");
		CDialog::OnCancel();
		return FALSE;
	}

	// 获取相机个数
	int nCount = fingerSensor.GetCameraCount() ;

	char szCamName[64] = {0};
    int nCamType;

	if (nCount > 0 )
	{
		for (int p=0; p<nCount; p++)
		{
			fingerSensor.GetCameraName(p, szCamName, nCamType);
            strcat(szCamName, "  ");
            if (CAM_A2000 == nCamType)
                strcat(szCamName, "A2000");
            else if (CAM_A3000 == nCamType)
                strcat(szCamName, "A3000");
			else if (CAM_A5000 == nCamType)
				strcat(szCamName, "A5000");
            else
                strcat(szCamName, "A1000");
			m_cbCamList.AddString(szCamName) ;
		}

		m_cbCamList.SetCurSel(0) ;
		m_CheckReverse.SetCheck(FALSE) ;

		OnCbnSelchangeComboCamlist() ;

		LoadConfigFile() ; 

		fingerSensor.SetGain(nGain) ;
		fingerSensor.SetExposure(nExposure) ;  
		fingerSensor.SetGpioStatus(11, gpio11) ; 
		fingerSensor.SetGpioStatus(10,gpio10) ;
		fingerSensor.SetGpioStatus(9, gpio9) ;
		fingerSensor.SetGpioStatus(7, gpio7) ;
		fingerSensor.SetGpioStatus(5, gpio5) ;
		fingerSensor.SetGpioStatus(4, gpio4) ;
		fingerSensor.SetGpioStatus(3, gpio3) ;
		fingerSensor.SetGpioStatus(2, gpio2) ;
		fingerSensor.SetGpioStatus(1, gpio1);
		fingerSensor.SetGpioStatus(6, gpio6) ;		
		fingerSensor.SetGpioStatus(8, gpio8) ;
	}

	CString str = "" ; 
	m_SliderGain .SetPos(nGain) ; 
	str.Format("%d", nGain) ;
	m_edtGain.SetWindowText(str) ;

	m_SliderExp.SetPos(nExposure) ;
	str.Format("%d", nExposure) ;
	m_edtExp.SetWindowText(str) ;

	m_SliderLed.SetPos(nLedValue) ; 
	str.Format("%d", nLedValue) ;
	m_edtLed.SetWindowText(str) ;

	m_CheckLed1.SetCheck(gpio1) ;
	m_CheckLed2.SetCheck(gpio2) ;
	m_CheckLed3.SetCheck(gpio3) ;
	m_CheckLed4.SetCheck(gpio4) ;
	m_CheckLed5.SetCheck(gpio5) ;
	m_CheckLed6.SetCheck(gpio6) ;
	m_CheckLed7.SetCheck(gpio7) ;
	m_CheckLed8.SetCheck(gpio8) ;
    m_CheckLed9.SetCheck(gpio9) ;
	m_CheckLed10.SetCheck(gpio10) ;
	m_CheckLed11.SetCheck(gpio11) ;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COGF_usbtstDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COGF_usbtstDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COGF_usbtstDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COGF_usbtstDlg::OnOK() 
{
	// TODO: Add extra validation here
    free(imagebuff);
    free(pMatationBuf);

    CDialog::OnOK();
}

void COGF_usbtstDlg::OnCapture() 
{
	if (hThread == NULL)
	{
		dispar = new mydispar;
		dispar->MyWnd = hwndCapture;
		dispar->MyCdc = pDC;
        dispar->MyDlg = this;
		dispar->Imagedata = imagebuff;

		hThread = CreateThread( 
			NULL,                        // no security attributes 
			64,                           // use default stack size  
			(LPTHREAD_START_ROUTINE)ThreadDisplay,                  // thread function 
			(LPVOID) dispar,                // argument to thread function 
			0,                           // use default creation flags 
			&dwThreadId);                // returns the thread identifier 

		// Check the return value for success. 

		if ( hThread ) 
		{
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			//CloseHandle(hThread);
		}
		else
		{
			MessageBox( NULL, "Create Thread Fail!");	
		}
	}
	
   return;	
}

void COGF_usbtstDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
    static int	no = 0;
	char		filename[256] = { 0 };
	char		*pszType = NULL;

	if (_btnWSQ.GetCheck())
		pszType = "wsq";
	else
		pszType = "bmp";

	sprintf(filename, "fingerImg_%d.%s", no, pszType);

	/*FILE *fp;
	fp=fopen(filename,"wb");
	if(fp==NULL)
	{
	MessageBox("Create file fail!");
	return;
	}
	fwrite(imagebuff,1,nImgWidth*nImgHeight,fp);
	fclose(fp);*/

	if (_btnWSQ.GetCheck())
	{
		unsigned char	*pData = NULL;
		int				nDataLen = 0;
		if (0 > INFIS_CompressByWSQ(imagebuff, nImgWidth, nImgHeight, 10, &pData, &nDataLen) ||
			pData == NULL || nDataLen <= 0)
		{
			MessageBox("compress image faild with WSQ method ！");
			return;
		}

		FILE	*fp = NULL;
		fp = fopen(filename,"wb");
		if(fp == NULL)
		{
			MessageBox("Create file fail!");
			return;
		}
		
		fwrite(pData, nDataLen, 1, fp);
		fclose(fp);

		INFIS_FreeImageWSQ(pData);
	}
	else
		INFIS_SaveDataAsBMP(filename, imagebuff, nImgWidth, nImgHeight, 8);

    ++ no;
	MessageBox("save success！");
	return;
}

void COGF_usbtstDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	bExit = TRUE ;
	if (dispar)
	{
		delete dispar;
		dispar = NULL;
	}
	status=0;
	
	if(hThread!=NULL)
	{
		TerminateThread(hThread,0);
		CloseHandle(hThread);
	}
	free(imagebuff);
	free(pMatationBuf) ;
	fingerSensor.Close();
	CDialog::OnCancel();
	return;
}


void COGF_usbtstDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
//	OGFUSB_Led(1);
	return;	
}

void COGF_usbtstDlg::OnCled() 
{
	// TODO: Add your control notification handler code here
//	OGFUSB_Led(0);
	return;
}

void ThreadDisplay(LPVOID lpparam) 
{
    static int nElapsedTime = 1;
    static int nFrameNum = 0;

 	pmydispar pdis;
	pdis=(pmydispar )lpparam;
	int i=0 ;
	while (!bExit)
	{
		if (status)
		{
            DWORD tick1 = GetTickCount();
			fingerSensor.ReadImage(pdis->Imagedata);
            DWORD tick2 = GetTickCount();
			////////////////////////////////////////////////////////////////////////////
			//////测试
			//CFile file ; 
			//if (file.Open(_T("C:\\1600x1500.raw"), CFile::modeReadWrite))
			//{
			//	file.Read(pdis->Imagedata, 1600*1500) ;
			//	file.Close() ;
			//}
			//////////////////////////////////////////////////////////////////////////////
			//fingerSensor.NoiseFilterByThreshold(pdis->Imagedata, nImgWidth, nImgHeight, 0, 224);
			showraw(pdis->MyWnd,pdis->MyCdc,pdis->Imagedata, nImgWidth, nImgHeight);
			Sleep(20);

            // compute frame rate
            ++ nFrameNum;
            int nTmp = tick2 - tick1;
            if (nTmp < 0)
                nTmp += ULONG_MAX;
            nElapsedTime += nTmp;

            CString msg = "";
            msg.Format("%d Frames/second", (int)(nFrameNum * 1000.0f / nElapsedTime + 0.5f));
            pdis->MyDlg->GetDlgItem(IDC_STATIC_FRAME_RATE)->SetWindowText(msg);
		}
		else
			Sleep(200) ;
	}
	return;
} 

//
void showraw(HWND hMywnd,CDC *pMyDC,unsigned char *rdata, int x, int y)
{
	int i;
	HANDLE hdl;
	unsigned char * mmbuf;
	RECT myrect;
	hdl=GlobalAlloc(GHND,sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256); 
	mmbuf=(unsigned char *)GlobalLock(hdl);
//Create Bitmapinfo header
	BITMAPINFOHEADER bmi;
	bmi.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.biWidth       = x;   
	bmi.biHeight      = -y;
	bmi.biPlanes      = 1;
	bmi.biBitCount    = 8;
	bmi.biCompression=0;
	bmi.biSizeImage   = 0;
    bmi.biXPelsPerMeter=0;
    bmi.biYPelsPerMeter=0; 
    bmi.biClrUsed     = 256;
    bmi.biClrImportant= 0;
 	//load infoheader
	memcpy(mmbuf, &bmi,  sizeof(BITMAPINFOHEADER));;
//create palatte,Bitmap palette: grey scale, 256 levels
	::GetWindowRect(hMywnd,&myrect);
	unsigned char *argbq= (BYTE *) mmbuf + sizeof(BITMAPINFOHEADER);
	for(i=0;i<256;i++)
	{ *argbq++=i; *argbq++=i; *argbq++=i; *argbq++=0; }

	CDC			memdc;   
	memdc.CreateCompatibleDC(pMyDC);   
	CBitmap		bmp,*poldbmp;   
	bmp.CreateCompatibleBitmap(pMyDC,x,y);  //只有客户区大小   
	//	memdc.SetViewportOrg(-pt);  //设置View的偏移   
	poldbmp  =  memdc.SelectObject(&bmp);   

	SetDIBitsToDevice ( memdc.GetSafeHdc(), 
		0, 0, 
		x,y,
		0, 0, 
		0, y, 
		rdata,
		(LPBITMAPINFO)mmbuf,
		DIB_RGB_COLORS);		

	CPen newpen;
	newpen.CreatePen(PS_SOLID,3,RGB(255,0,0));
	CPen*oldpen;
	oldpen = memdc.SelectObject(&newpen);

	int nCenterX = x/2 ; 
	int nCenterY = y/2 ; 
	memdc.MoveTo(nCenterX-200, nCenterY) ; 
	memdc.LineTo(nCenterX+200, nCenterY) ;
	memdc.MoveTo(nCenterX, nCenterY-200) ; 
	memdc.LineTo(nCenterX, nCenterY+200) ;

	memdc.SetBkMode(TRANSPARENT);  
//Display the information
	::SetStretchBltMode(pMyDC->m_hDC, COLORONCOLOR);

	RECT rc ;
	GetClientRect(hMywnd, &rc) ;
	int nSrcLeft = 0 ;
	int nSrcTop = 0 ;
	int nSrcW = x ; 
	int nSrcH = y ;
	if (bShowNormal)
	{
		nSrcLeft = (x - (rc.right-rc.left))/2 ; 
		nSrcTop = (y - (rc.bottom-rc.top))/2 ; 
		nSrcW = rc.right-rc.left ; 
		nSrcH = rc.bottom-rc.top ; 
	}


	pMyDC->StretchBlt(-rc.left, -rc.top, rc.right-rc.left,rc.bottom-rc.top,
		&memdc, 
		nSrcLeft, nSrcTop, 
		nSrcW, nSrcH,
		SRCCOPY) ;

	GlobalUnlock(mmbuf);
	GlobalFree(hdl);
	return ;
}

void COGF_usbtstDlg::OnReleasedcaptureSliderGain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	nGain = m_SliderGain.GetPos() ;
	fingerSensor.SetGain(nGain) ;
	CString str; 
	str.Format("%d", nGain) ;
	m_edtGain.SetWindowText(str) ;
	
	*pResult = 0;
}

void COGF_usbtstDlg::OnReleasedcaptureSliderExp(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	nExposure = m_SliderExp.GetPos() ;
	fingerSensor.SetExposure(nExposure) ;
	CString str; 
	str.Format("%d", nExposure) ;
	m_edtExp.SetWindowText(str) ;
	*pResult = 0;
}

void COGF_usbtstDlg::OnCheckReverse() 
{
	// TODO: Add your control notification handler code here
}

void COGF_usbtstDlg::OnStnClickedView()
{
    // TODO: 在此添加控件通知处理程序代码
}

void COGF_usbtstDlg::OnSelchangeComboWindowsize() 
{
    // TODO: Add your control notification handler code here
	int nSel = m_cbWindowSize.GetCurSel() ; 
	if (nSel == 0)
	{
		bHalfWindow = false ; 
	}
	else
	{ 
		bHalfWindow = true ; 
	}
	OnCbnSelchangeComboCamlist() ;
}

void COGF_usbtstDlg::OnCbnSelchangeComboCamlist()
{
	// TODO: Add your control notification handler code here
	status = 0;
	fingerSensor.Close() ;

	char szCamName[64] = {0};
    int  nCamType;

	int nSel = 0;
	nSel = m_cbCamList.GetCurSel();

    fingerSensor.GetCameraName(nSel, szCamName, nCamType);

    if(!fingerSensor.Open(nSel, true, 0, 0, bHalfWindow))
    {
        status = 0 ;
        MessageBox("Initial Device Fail!");
        return ;
    }

    //初始化采集仪参数
    fingerSensor.SetGain(nGain) ;
    fingerSensor.SetExposure(nExposure) ;
    status = 1 ;

	CString str ;
	if (fingerSensor.GetDeviceParams(&DeviceParams))
	{
		m_cbWindowSize.ResetContent() ;
        if (DeviceParams.nCamType == CAM_A1000)
		{
			// A1000 440*440
			GetDlgItem(IDC_STATIC_CAMTYPE)->SetWindowText("A1000") ;
			m_cbWindowSize.AddString("256*360") ;
			m_cbWindowSize.SetCurSel(0) ;
			nImgWidth = A1000_WIDTH;
			nImgHeight = A1000_HEIGHT;

			m_SliderGain.SetRange(MIN_GAIN, MAX_GAIN) ;
			m_SliderExp.SetRange(MIN_EXPOSURE, MAX_EXPOSURE) ;
			m_SliderLed.SetRange(MIN_LED, MAX_LED) ; 

			nMaxW = A1000_WIDTH;
			nMaxH = A1000_HEIGHT;
		}
		else if (DeviceParams.nCamType == CAM_A2000)
		{
			// A2000 440*440
			GetDlgItem(IDC_STATIC_CAMTYPE)->SetWindowText("A2000") ;
			m_cbWindowSize.AddString("440*440") ;
			m_cbWindowSize.SetCurSel(0) ;
			nImgWidth = A2000_WIDTH;
			nImgHeight = A2000_HEIGHT;

			m_SliderGain.SetRange(MIN_GAIN, MAX_GAIN) ;
			m_SliderExp.SetRange(MIN_EXPOSURE, MAX_EXPOSURE) ;
			m_SliderLed.SetRange(MIN_LED, MAX_LED) ; 

			nMaxW = A2000_WIDTH;
			nMaxH = A2000_HEIGHT;
		}
		else if (DeviceParams.nCamType == CAM_A3000)
		{
			// A3000 640*640
			GetDlgItem(IDC_STATIC_CAMTYPE)->SetWindowText("A3000") ;
			m_cbWindowSize.AddString("640*640") ;
			if (bHalfWindow)
			{
				m_cbWindowSize.SetCurSel(1) ;
				nImgWidth = A3000_WIDTH/2;
				nImgHeight = A3000_HEIGHT/2;


				nMaxW = A3000_WIDTH/2;
				nMaxH = A3000_HEIGHT/2;
			}
			else
			{
				m_cbWindowSize.SetCurSel(0) ;
				nImgWidth = A3000_WIDTH;
				nImgHeight = A3000_HEIGHT;

				nMaxW = A3000_WIDTH;
				nMaxH = A3000_HEIGHT;
			}
			

			m_SliderGain.SetRange(MIN_GAIN, MAX_GAIN) ;
			m_SliderExp.SetRange(MIN_EXPOSURE, MAX_EXPOSURE) ;
			m_SliderLed.SetRange(MIN_LED, MAX_LED) ; 

		}
		else if (DeviceParams.nCamType == CAM_A5000)
		{
			// A5000 1600*1500
			GetDlgItem(IDC_STATIC_CAMTYPE)->SetWindowText("A5000");
			m_cbWindowSize.AddString("1600*1500");
			m_cbWindowSize.AddString("800*750");
			if (bHalfWindow)
			{
				m_cbWindowSize.SetCurSel(1);
				nImgWidth = A5000_WIDTH / 2;
				nImgHeight = A5000_HEIGHT / 2;


				nMaxW = A5000_WIDTH / 2;
				nMaxH = A5000_HEIGHT / 2;
			}
			else
			{
				m_cbWindowSize.SetCurSel(0);
				nImgWidth = A5000_WIDTH;
				nImgHeight = A5000_HEIGHT;

				nMaxW = A5000_WIDTH;
				nMaxH = A5000_HEIGHT;
			}

			_combCaptureType.AddString(_T("flat finger"));
			_combCaptureType.AddString(_T("right four"));
			_combCaptureType.AddString(_T("left four"));
			_combCaptureType.AddString(_T("rl thumb"));

			_combCaptureType.SetCurSel(CAPTURE_TYPE_RFOUR);

			m_SliderGain.SetRange(MIN_GAIN, MAX_GAIN);
			m_SliderExp.SetRange(MIN_EXPOSURE, MAX_EXPOSURE);
			m_SliderLed.SetRange(MIN_LED, MAX_LED);
		}
	}

	m_SliderGain.SetPos(nGain) ;
	m_SliderExp.SetPos(nExposure) ;
	m_SliderLed.SetPos(nLedValue) ;
	str.Format("%d", nGain) ;
	m_edtGain.SetWindowText(str) ;
	str.Format("%d", nExposure) ;
	m_edtExp.SetWindowText(str) ;
	str.Format("%d", nLedValue) ;
	m_edtLed.SetWindowText(str) ;

	nStartColumn = 0 ;
}

void COGF_usbtstDlg::OnSetreg() 
{
	BeginWaitCursor() ;
	int nAddr;
	int nVal;
	char strTemp[32] = {0};
	CString str ; 

	//get input data
	GetDlgItem(IDC_VALUE)->GetWindowText(str) ;
	str.SpanExcluding(" ");
	sscanf((LPTSTR)(LPCTSTR)str, "%x", &nVal);

	GetDlgItem(IDC_REGISTER)->GetWindowText(str) ;
	str.SpanExcluding(" ");
	sscanf((LPTSTR)(LPCTSTR)str, "%x", &nAddr);


	fingerSensor.SetRegister(nAddr, nVal) ;

	EndWaitCursor() ;
}


void COGF_usbtstDlg::OnBnClickedGetreg()
{
	// TODO: Add your control notification handler code here
	BeginWaitCursor() ;
	CString str ; 
	int nAddr;
	GetDlgItem(IDC_REGISTER)->GetWindowText(str) ;
	str.SpanExcluding(" ");
	sscanf((LPTSTR)(LPCTSTR)str, "%x", &nAddr);

	USHORT nValue = 0 ;
	fingerSensor.GetRegister(nAddr, nValue) ;

	str.Format(_T("0x%04x"),nValue) ;
	GetDlgItem(IDC_VALUE)->SetWindowText(str) ;
	EndWaitCursor() ;
}

void COGF_usbtstDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void COGF_usbtstDlg::OnBnClickedCheckNormal()
{
	// TODO: Add your control notification handler code here
	bShowNormal = m_CheckNormal.GetCheck() ; 
}

void COGF_usbtstDlg::OnNMReleasedcaptureSliderLed(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	//nLedValue = m_SliderLed.GetPos() ;
	//fingerSensor.SetLed(nLedValue) ;
	//CString str; 
	//str.Format("%d", nLedValue) ;
	//m_edtLed.SetWindowText(str) ;
	//*pResult = 0;
}

void COGF_usbtstDlg::OnBnClickedLed1()
{
	// TODO: Add your control notification handler code here
	gpio1 = m_CheckLed1.GetCheck() ;
	fingerSensor.SetGpioStatus(1, gpio1) ;
}

void COGF_usbtstDlg::OnBnClickedLed2()
{
	// TODO: Add your control notification handler code here
	gpio2 = m_CheckLed2.GetCheck() ;
	fingerSensor.SetGpioStatus(2, gpio2) ;
}

void COGF_usbtstDlg::OnBnClickedLed3()
{
	// TODO: Add your control notification handler code here
	gpio3 = m_CheckLed3.GetCheck() ;
	fingerSensor.SetGpioStatus(3, gpio3) ;
}

void COGF_usbtstDlg::OnBnClickedLed4()
{
	// TODO: Add your control notification handler code here
	gpio4 = m_CheckLed4.GetCheck() ;
	fingerSensor.SetGpioStatus(4, gpio4) ;
}

void COGF_usbtstDlg::OnBnClickedLed5()
{
	// TODO: Add your control notification handler code here
	gpio5 = m_CheckLed5.GetCheck() ;
	fingerSensor.SetGpioStatus(5, gpio5) ;
}

void COGF_usbtstDlg::OnBnClickedLed6()
{
	// TODO: Add your control notification handler code here
	gpio6 = m_CheckLed6.GetCheck() ;
	fingerSensor.SetGpioStatus(6, gpio6) ;
}

void COGF_usbtstDlg::OnBnClickedLed7()
{
	// TODO: Add your control notification handler code here
	gpio7 = m_CheckLed7.GetCheck() ;
	fingerSensor.SetGpioStatus(7, gpio7) ;
}

void COGF_usbtstDlg::OnBnClickedLed8()
{
	// TODO: Add your control notification handler code here
	gpio8 = m_CheckLed8.GetCheck() ;
	fingerSensor.SetGpioStatus(8, gpio8) ;
}

void COGF_usbtstDlg::OnBnClickedLed9()
{
	// TODO: Add your control notification handler code here
	gpio9 = m_CheckLed9.GetCheck() ;
	fingerSensor.SetGpioStatus(9, gpio9) ;
}

void COGF_usbtstDlg::OnBnClickedLed10()
{
	// TODO: Add your control notification handler code here
	gpio10 = m_CheckLed10.GetCheck() ;
	fingerSensor.SetGpioStatus(10, gpio10) ;
}

void COGF_usbtstDlg::OnBnClickedLed11()
{
	// TODO: Add your control notification handler code here
	gpio11 = m_CheckLed11.GetCheck() ;
	fingerSensor.SetGpioStatus(11, gpio11) ; 
}




// 保存软件重启配置文件
void COGF_usbtstDlg::SaveConfigFile()
{
	// 获取配置文件目录
	CFile SaveFile ;
	TCHAR szPath[MAX_PATH];
	DWORD nBufferLength = MAX_PATH;

	GetModuleFileName(NULL, szPath, nBufferLength); 

	TCHAR* pDest = NULL ;
	pDest = _tcsrchr(szPath, '\\') ;

	CString str = _T("") ;
	CString strConfig = _T("") ;
	pDest[1] = '\0';
	str = _T("config.ini") ;
	_tcscat(szPath, str) ;

	if (!SaveFile.Open(szPath, CFile::modeWrite | CFile::modeCreate))
	{
		return ;
	}

	str.Format(_T("Gain=%d;\r\n"), nGain) ;
	strConfig += str ;

	str.Format(_T("Exposure=%d;\r\n"), nExposure) ;
	strConfig += str ;

	str.Format(_T("LedValue=%d;\r\n"), nLedValue) ;
	strConfig += str ;

	str.Format(_T("Led1=%d;\r\n"), gpio1) ;
	strConfig += str ;

	str.Format(_T("Led2=%d;\r\n"), gpio2) ;
	strConfig += str ;

	str.Format(_T("Led3=%d;\r\n"), gpio3) ;
	strConfig += str ;

	str.Format(_T("Led4=%d;\r\n"), gpio4) ;
	strConfig += str ;

	str.Format(_T("Led5=%d;\r\n"), gpio5) ;
	strConfig += str ;

	str.Format(_T("Led6=%d;\r\n"), gpio6) ;
	strConfig += str ;

	str.Format(_T("Led7=%d;\r\n"), gpio7) ;
	strConfig += str ;

	str.Format(_T("Led8=%d;\r\n"), gpio8) ;
	strConfig += str ;

		str.Format(_T("Led9=%d;\r\n"), gpio9) ;
	strConfig += str ;

		str.Format(_T("Led10=%d;\r\n"), gpio10) ;
	strConfig += str ;

		str.Format(_T("Led11=%d;\r\n"), gpio11) ;
	strConfig += str ;
	SaveFile.Write(strConfig, strlen(strConfig)) ;
	SaveFile.Close() ;

	return  ;
}

// 加载软件重启配置文件
BOOL COGF_usbtstDlg::LoadConfigFile()
{
	// 获取配置文件目录
	CFile SaveFile ;
	TCHAR szPath[MAX_PATH];
	DWORD nBufferLength = MAX_PATH;

	GetModuleFileName(NULL, szPath, nBufferLength); 
	TCHAR* pDest = NULL ;
	pDest = _tcsrchr(szPath, '\\') ;

	pDest[1] = '\0';
	CString str, strConfig ;
	str = _T("config.ini") ;
	_tcscat(szPath, str) ;

	if (!SaveFile.Open(szPath, CFile::modeRead))
	{
		return FALSE;
	}

	int nBufferSize = SaveFile.GetLength() + 1 ;
	char* pConfigBuffer = new char[nBufferSize] ;
	SaveFile.Read(pConfigBuffer, nBufferSize) ;
	SaveFile.Close() ;

	CString strValue = _T("");
	int nValue = 0 ;
	TCHAR* pStart = NULL ;
	TCHAR* pEnd = NULL ;

	pStart = pConfigBuffer ;

	pEnd = _tcsstr(pStart, _T("Gain=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Gain=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			nGain = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Exposure=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Exposure=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			nExposure = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("LedValue=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("LedValue=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			nLedValue = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led1=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led1=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio1 = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led2=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led2=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio2 = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led3=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led3=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio3 = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led4=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led4=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio4 = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led5=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led5=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio5 = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led6=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led6=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio6 = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led7=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led7=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio7 = atoi(strValue) ;
		}
	}

	pEnd = _tcsstr(pStart, _T("Led8=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led8=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio8 = atoi(strValue) ;
		}
	}

		pEnd = _tcsstr(pStart, _T("Led9=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led9=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio9 = atoi(strValue) ;
		}
	}
		pEnd = _tcsstr(pStart, _T("Led10=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led10=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio10 = atoi(strValue) ;
		}
	}
		pEnd = _tcsstr(pStart, _T("Led11=")) ;
	if (pEnd != NULL)
	{
		pStart += _tcslen(_T("Led11=")) ;
		pEnd = _tcsstr(pStart, _T(";\r\n")) ;
		if (pEnd != NULL)
		{
			*pEnd = '\0';
			strValue = pStart ;
			pStart = pEnd+1+_tcslen(_T("\r\n")) ;
			gpio11 = atoi(strValue) ;
		}
	}

	return TRUE ;
}


void COGF_usbtstDlg::OnBnClickedCut()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	int	nImageType = IMAGE_TYPE_BMP;

	if (_btnWSQ.GetCheck())
		nImageType = IMAGE_TYPE_WSQ;
	
	INFIS_CutImage(_nCaptureType, imagebuff, nImgWidth, nImgHeight, this, nImageType);
}


void COGF_usbtstDlg::OnCbnSelchangeComboCapturetype()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (_nCaptureType == CAPTURE_TYPE_FLATFINGER)
		_btnCut.EnableWindow(FALSE);
	else
		_btnCut.EnableWindow(TRUE);
}
