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

	strStdCheck = "����ָ�����׼��";
	strRole	    = "    ͨ��\"��׼���\"�˵���ѡ��Ҫ�������ݡ�";
	switch(nStdChkType)
	{
	case GFLS_SCANNER_STDCHKTYPE_VALIDIMAGESIZE:
		strStdCheck = "��Ч�ɼ����ڳߴ����׼��";
		strRole		= "    ���òɼ��Ǽ��TP-F01������ͼ��ͼ����Ӧ������ȫ�������ı߿��Լ����е������ߣ����߰�����ȥ�����߿�������������ߡ�";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMAGEPIXELCOUNT:
		strStdCheck = "ͼ������������׼��";
		strRole		= "    ���洢��ԭʼ�����ļ��Ĵ�С������ָ�Ʋɼ��ǲɼ�����ͼ���ļ���СӦ��Ϊ409600���ֽڡ�";
		break;

	case GFLS_SCANNER_STDCHKTYPE_CENTERDIFF:
		strStdCheck = "ͼ������ƫ�����׼��";
		strRole		= "    ����ָ�Ʋɼ��Ǽ��TP-F01������ͼ����ͼ������Ͻ�Ϊԭ�㣬���κ�һ�����ص�������Ա�ʾΪ(x,y)����0��x,y��639���������ĶԽ��߽�����ͼ���е�λ��Ӧ����һ���������ڣ������ε����Ͻǵ�����Ϊ��305,305�������½ǵ�����Ϊ��335,335����";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMGRESOLUTION:
		strStdCheck = "ͼ��ֱ��ʼ���׼��";
		strRole		= "    ����ָ�Ʋɼ��ǣ����TP-F02������ͼ�񡣶���A1-A3��A2-A4֮������ظ����õ�����ֵ��ΪPx��Py����594��Px��606��594��Py��606�ֱ���Ϊ500dpi��";
		break;

	case GFLS_SCANNER_STDCHKTYPE_ABERRANCE:
		strStdCheck = "ͼ��������׼��";
		strRole		= "    ����ָ�Ʋɼ��ǣ����TP-F02������ͼ������NΪ300��������B1B2B3B4������ȫ��ʾ������A1��A2��A3��A4���������ĵ�ľ��루�����������õ�4��ֵ����ΪV1, V2, V3, V4������B1-A1��B1-A2, B2-A2, B2-A3, B3-A3, B3-A4��B4-A4, B4-A1֮��ľ��룬�õ�8��ֵ���ֱ��ΪV5��V12���ֱ���ʽ����ͼ����䣺\r\n \tDi = ��Vi - N����N ��100%\r\n����1��i��12��\r\n����ָ�Ʋɼ����ۺ�ͼ����䣺�ܡ�1%��";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYAVERAGE:
		strStdCheck = "ͼ�񱳾��Ҷ�ֵ����׼��";
		strRole		= "    ʹ��Bg-Imageͼ�����صĻҶ�ƽ��ֵ���ڵ���225��";
		break;

	case GFLS_SCANNER_STDCHKTYPE_IMGBACKGRAYUNAVE:
		strStdCheck = "ͼ�񱳾��ҶȲ����ȶȼ���׼��";
		strRole		= "    ʹ��Bg-Imageͼ��TP-F03������ͼ�������ӳ���600X600���ڣ��ָ�9��40X40��С���Σ��ֱ����ÿ�����εĻҶȾ�ֵ���ҶȾ�ֵ�����ֵΪVmax, ��СֵΪVmin������ʽ����ͼ��ҶȲ����ȶȣ�\r\n\tͼ�񱳾��ҶȲ����ȶ�=(Vmax-Vmin)/ (Vmax+Vmin)x100%\r\n�ɼ��ǵı����ҶȲ����ȶ�Ϊ����ͼ��TP-03ͼ�񱳾��ҶȲ����ȶȵ����ֵ��";
		break;

	case GFLS_SCANNER_STDCHKTYPE_CENTERGRAY:
		strStdCheck = "��������ͼ��Ҷ�ֵ����׼��";
		strRole		= "    ����ָ�Ʋɼ��ǣ�ʹ��TP-F04������ͼ�񣻼���ͼ����Aɫ��ͼ��ĻҶ�ƽ��ֵ��ƽ��ֵӦ��С��95��";
		break;

	case GFLS_SCANNER_STDCHKTYPE_GRAYVARIA:
		strStdCheck = "�Ҷȶ�̬��Χ����׼��";
		strRole		= "    �ֱ���TP-F05���ӳ������ֵ��TP-F06���ӳ������ֵ�����ֵΪ�Ҷȶ�̬��Χ��Ҫ�����125Ϊ�ϸ�";
		break;

	case GFLS_SCANNER_STDCHKTYPE_BADPOINT:
		strStdCheck = "ͼ��õ����׼��";
		strRole		= "    ʹ��Bg-Imageͼ�������ͼ��ͼ������ֵ�뱳���ҶȾ�ֵ֮�����25��Ϊͼ��õ㣻����ͼ��õ㣺��ĳ���õ�������Χ�˸������д��ڴõ㣬��2���õ�Ϊ���������õ㡣����һ���õ�ͨ��һϵ�����ڴõ㵽����һ���õ㣬�������õ�Ҳ�������õ㡣������һϵ�дõ�Ҳ��Ϊ�õ㡣�õ��ֱ��Ϊ�õ���X����Y��ͶӰ�����ص�������ֵ�����ӳ�[����ָ�Ʋɼ�������600 ���ء�600 ����]��Χ�ڲ��ܺ���ֱ������3�����ص�Ĵõ㡣ֱ����2�Ĵõ�������ͼ��Χ�ڲ��ܹ�����5����ֱ��=1�Ĵõ�������ͼ��Χ�ڲ��ܳ���50����";
		break;

	case GFLS_SCANNER_STDCHKTYPE_ENVIRONMENT:
		strStdCheck = "���������׼��";
		strRole		= "    �նȼƴ���̽ͷ���ڲɼ����ϣ������ϣ�ÿ̨�ɼ��ǲ��Թ�Դ�ն�һ�£�����λ����Ϊ300Lx���ɼ��ǿ��������������������Ķ��壺 ��һ�Ų�͸����ֽ�ֱ��ڸǲɼ����ڵ���벿���ϰ벿���Ұ벿���°벿�ɼ�ͼ������ͼ��ı����ҶȲ����ȶ�֮�����15��";
		break;

	case GFLS_SCANNER_STDCHKTYPE_PARSE:
		strStdCheck = "ͼ������ȼ���׼��";
		strRole		= "    ����ָ�Ʋɼ��ǣ����TP-F02������ͼ���������C���е��������ܹ���ȷ�ķֱ�����Ϊ�ﵽ����Ӧ�Ľ����ȡ��ֱ�׼��Ϊ��������ɫ�����ĻҶȾ�ֵ�Ͱ�ɫ�����ĻҶȾ�ֵ�����ֵ��С��40��";
		break;
	}

	m_staticStdChkType.SetWindowText(strStdCheck);
	m_editStdChkRole.SetWindowText(strRole);
}