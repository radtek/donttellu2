// DlgImageInfo.cpp : implementation file
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "DlgImageInfo.h"
#include "GflsScanerTestView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CGflsScanerTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDlgImageInfo dialog


CDlgImageInfo::CDlgImageInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImageInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImageInfo)
	m_strAverage = _T("");
	m_strCntInRange = _T("");
	m_strDiffValue = _T("");
	m_strMedian = _T("");
	m_strPercent = _T("");
	m_strPixelCount = _T("");
	m_strRange = _T("");
	//}}AFX_DATA_INIT
	m_pTestView   = NULL;
	m_pCaptBuffer = NULL;

	memset(&m_stCaptSpeed, 0, sizeof(m_stCaptSpeed));
	m_stCaptSpeed.nFrameCount = 300;
}


void CDlgImageInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImageInfo)
	DDX_Control(pDX, IDC_STATIC_STDCHKTYPE, m_staticStdChkType);
	DDX_Control(pDX, IDC_EDIT_STDCHK_ROLE, m_editStdChkRole);
	DDX_Control(pDX, IDC_STATIC_GRAYSTAT2, m_staticGrayValue);
	DDX_Text(pDX, IDC_STATIC_AVERAGE2, m_strAverage);
	DDX_Text(pDX, IDC_STATIC_CNTINRANGE2, m_strCntInRange);
	DDX_Text(pDX, IDC_STATIC_DIFF2, m_strDiffValue);
	DDX_Text(pDX, IDC_STATIC_MEDIAN2, m_strMedian);
	DDX_Text(pDX, IDC_STATIC_PERCENT2, m_strPercent);
	DDX_Text(pDX, IDC_STATIC_PIXELCNT2, m_strPixelCount);
	DDX_Text(pDX, IDC_STATIC_RANGE2, m_strRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImageInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgImageInfo)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOWIMAGE_STATINFO, OnSetImageStatInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImageInfo message handlers

BOOL CDlgImageInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_pCaptBuffer = (UCHAR*)malloc(GFLS_SCANNER_IMAGEWIDTH * GFLS_SCANNER_IMAGEHEIGHT);
	m_pCaptBuffer = (UCHAR*)malloc(GFLS_SCANNER_IMAGEWIDTH * GFLS_SCANNER_IMAGEHEIGHT);
	ASSERT(m_pCaptBuffer);
	m_staticGrayValue.m_pParentWnd  = this;
	m_staticGrayValue.m_pDlgImgInfo = this;

	InitCtrlWndStateAndValue();

	UpdateData(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImageInfo::OnDestroy() 
{
	if(m_pCaptBuffer)
		free(m_pCaptBuffer);
	m_pCaptBuffer = NULL;

	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CDlgImageInfo::OnCancel()
{
	return;

	CDialog::OnCancel();
}

void CDlgImageInfo::OnOK()
{
	return;

	CDialog::OnOK();
}

BOOL CDlgImageInfo::InitImageInfoDlg(CWnd* pParentWnd, CRect &rtFrame)
{
	if(!Create(IDD, pParentWnd))
		return FALSE;
	CRect rtWnd, rtClient;

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);

	GetClientRect(rtClient);
	GetWindowRect(rtWnd);

	int nWedge, nTitle;
	nWedge = rtWnd.Width() - rtClient.right;
	nTitle = rtWnd.Height() - rtClient.bottom;

	rtWnd = rtFrame;
	if(rtFrame.top > 120)
		rtWnd.top = 120;
//	rtWnd.top	 = rtFrame.bottom + 5;
	rtWnd.bottom = rtWnd.top + rtClient.bottom + nTitle;
	rtWnd.right  = cxScreen - nWedge * 2;
	rtWnd.left   = rtWnd.right - rtClient.right - nWedge;

	rtFrame = rtWnd;

	MoveWindow(rtWnd, FALSE);

//	ShowWindow(SW_HIDE);

	return TRUE;
}

void CDlgImageInfo::InitCtrlWndStateAndValue()
{
}

LRESULT CDlgImageInfo::OnSetImageStatInfo(WPARAM wParam, LPARAM lParam)
{
	SetImageStatInfo();
	return	0;
}

void CDlgImageInfo::SetImageStatInfo()
{
	if(!m_staticGrayValue.m_bHaveGrayInfo)
	{
		m_strAverage	= "";
		m_strDiffValue	= "";
		m_strMedian		= "";
		m_strPixelCount	= "";
		m_strRange		= "";
		m_strCntInRange	= "";
		m_strPercent	= "";
	}
	else
	{
		m_strAverage.Format(_T("%.2f"), m_staticGrayValue.m_stGrayStat.fAverage);
		m_strDiffValue.Format(_T("%.2f"), m_staticGrayValue.m_stGrayStat.fDiff);
		m_strMedian.Format(_T("%.2f"), m_staticGrayValue.m_stGrayStat.fMedian);
		m_strPixelCount.Format(_T("%d"), m_staticGrayValue.m_stGrayStat.nPixelCount);

		int x1, x2;
		x1 = min(m_staticGrayValue.m_nGrayValue1, m_staticGrayValue.m_nGrayValue2);
		x2 = max(m_staticGrayValue.m_nGrayValue1, m_staticGrayValue.m_nGrayValue2);
		if(x1 == x2)
			m_strRange.Format(_T("%d"), x1);
		else
			m_strRange.Format(_T("%d..%d"), x1,  x2);
		m_strCntInRange.Format(_T("%d"), m_staticGrayValue.m_nGrayCntInRang);
		m_strPercent.Format(_T("%.2f"), m_staticGrayValue.m_fPercent);
	}

	UpdateData(0);

//	return 1;
}

void CDlgImageInfo::showImageZoomInfo()
{
	CString strInfo;
	strInfo.Format(GFLS_IMAGEINFODLG_TITLE, m_pTestView->m_stParamInfo.nImgWidth, m_pTestView->m_stParamInfo.nImgHeight, m_pTestView->m_stParamInfo.nCurScale * 100);
	SetWindowText(strInfo);
}

void CDlgImageInfo::SetStdChkInformation(UCHAR nStdChkType)
{
	CString strStdCheck, strRole;

	strStdCheck = "各项指标检测标准：";
	strRole	    = "    通过\"标准检测\"菜单来选择要检测的内容。";
	switch(nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_VALIDIMAGESIZE:
		strStdCheck = "有效采集窗口尺寸检测标准：";
		strRole		= "    警用采集仪检查TP-F01产生的图像。图像中应当包含全部最外测的边框以及所有的网格线，或者包含除去最外侧边框外的所有网格线。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMAGEPIXELCOUNT:
		strStdCheck = "图像像素数检测标准：";
		strRole		= "    检查存储的原始数据文件的大小。警用指纹采集仪采集到的图像文件大小应当为409600个字节。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_CENTERDIFF:
		strStdCheck = "图像中心偏差检测标准：";
		strRole		= "    警用指纹采集仪检查TP-F01产生的图像。以图像的左上角为原点，则任何一个像素的坐标可以表示为(x,y)其中0≤x,y≤639。检测样板的对角线交点在图像当中的位置应当在一个正方形内，正方形的左上角的坐标为（305,305），右下角的坐标为（335,335）。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMGRESOLUTION:
		strStdCheck = "图像分辨率检测标准：";
		strRole		= "    警用指纹采集仪：检查TP-F02产生的图像。度量A1-A3、A2-A4之间的像素个数得到两个值记为Px和Py。若594≤Px≤606、594≤Py≤606分辨率为500dpi。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_ABERRANCE:
		strStdCheck = "图像畸变检测标准：";
		strRole		= "    警用指纹采集仪：检查TP-F02产生的图像。设置N为300。正方形B1B2B3B4必须完全显示；计算A1、A2、A3、A4与样板中心点的距离（像素数量）得到4个值，记为V1, V2, V3, V4；计算B1-A1，B1-A2, B2-A2, B2-A3, B3-A3, B3-A4，B4-A4, B4-A1之间的距离，得到8个值，分别记为V5到V12。分别按下式计算图像畸变：\r\n \tDi = │Vi - N│／N ×100%\r\n其中1≤i≤12。\r\n警用指纹采集仪综合图像畸变：≤±1%。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYAVERAGE:
		strStdCheck = "图像背景灰度值检测标准：";
		strRole		= "    使用Bg-Image图像，像素的灰度平均值大于等于225。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYUNAVE:
		strStdCheck = "图像背景灰度不均匀度检测标准：";
		strRole		= "    使用Bg-Image图像及TP-F03产生的图像；在主视场（600X600）内，分割9个40X40的小矩形，分别计算每个矩形的灰度均值。灰度均值的最大值为Vmax, 最小值为Vmin，按下式计算图像灰度不均匀度：\r\n\t图像背景灰度不均匀度=(Vmax-Vmin)/ (Vmax+Vmin)x100%\r\n采集仪的背景灰度不均匀度为背景图像及TP-03图像背景灰度不均匀度的最大值。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
		strStdCheck = "中心区域图像灰度值检测标准：";
		strRole		= "    警用指纹采集仪：使用TP-F04产生的图像；计算图像中A色块图像的灰度平均值。平均值应当小于95。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		strStdCheck = "灰度动态范围检测标准：";
		strRole		= "    分别检测TP-F05主视场区域均值与TP-F06主视场区域均值，其差值为灰度动态范围，要求大于125为合格。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_BADPOINT:
		strStdCheck = "图像疵点检测标准：";
		strRole		= "    使用Bg-Image图像产生的图像；图像像素值与背景灰度均值之差大于25的为图像疵点；连续图像疵点：若某个疵点在其周围八个像素中存在疵点，这2个疵点为相邻连续疵点。若从一个疵点通过一系列相邻疵点到达另一个疵点，这两个疵点也是连续疵点。连续的一系列疵点也称为疵点。疵点的直径为疵点在X轴与Y轴投影的像素点个数最大值。主视场[警用指纹采集仪中心600 像素×600 像素]范围内不能含有直径大于3个像素点的疵点。直径≥2的疵点在整个图像范围内不能够超过5个。直径=1的疵点在整个图像范围内不能超过50个。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT:
		strStdCheck = "环境光检测标准：";
		strRole		= "    照度计传感探头置于采集面上，方向朝上，每台采集仪测试光源照度一致，各部位读数为300Lx，采集仪可正常工作。正常工作的定义： 用一张不透明黑纸分别遮盖采集窗口的左半部、上半部、右半部、下半部采集图像，生成图像的背景灰度不均匀度之差不超过15。";
		break;

	case GFLS_SCANNER_STDCHKTYPE_PARSE:
		strStdCheck = "图像解析度检测标准：";
		strRole		= "    警用指纹采集仪：检查TP-F02产生的图像；如果所有C块中的线条都能够正确的分辨则认为达到了相应的解析度。分辨准则为：计算深色线条的灰度均值和白色线条的灰度均值，其差值不小于40。";
		break;
	}

	m_staticStdChkType.SetWindowText(strStdCheck);
	m_editStdChkRole.SetWindowText(strRole);
}