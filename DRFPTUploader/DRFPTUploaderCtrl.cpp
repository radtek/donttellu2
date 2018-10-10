// DRFPTUploaderCtrl.cpp : CDRFPTUploaderCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include "DRFPTUploader.h"
#include "DRFPTUploaderCtrl.h"
#include "DRFPTUploaderPropPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDRFPTUploaderCtrl, COleControl)

// 消息映射

BEGIN_MESSAGE_MAP(CDRFPTUploaderCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

// 调度映射

BEGIN_DISPATCH_MAP(CDRFPTUploaderCtrl, COleControl)
	DISP_FUNCTION_ID(CDRFPTUploaderCtrl, "SelectFile", dispidSelectFile, SelectFile, VT_I4, VTS_I4)
END_DISPATCH_MAP()

// 事件映射

BEGIN_EVENT_MAP(CDRFPTUploaderCtrl, COleControl)
END_EVENT_MAP()

// 属性页

// TODO:  按需要添加更多属性页。  请记住增加计数!
BEGIN_PROPPAGEIDS(CDRFPTUploaderCtrl, 1)
	PROPPAGEID(CDRFPTUploaderPropPage::guid)
END_PROPPAGEIDS(CDRFPTUploaderCtrl)

// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CDRFPTUploaderCtrl, "DRFPTUPLOADER.DRFPTUploaderCtrl.1",
	0x63f1676, 0x5d24, 0x419a, 0x95, 0xc4, 0x61, 0x18, 0x78, 0x63, 0x17, 0x79)

// 键入库 ID 和版本

IMPLEMENT_OLETYPELIB(CDRFPTUploaderCtrl, _tlid, _wVerMajor, _wVerMinor)

// 接口 ID

const IID IID_DDRFPTUploader = { 0xC454ABFE, 0x8C51, 0x467B, { 0xAE, 0x7B, 0x6, 0x7B, 0xD4, 0x55, 0xDF, 0xA1 } };
const IID IID_DDRFPTUploaderEvents = { 0xF1C79621, 0x7334, 0x419B, { 0x9D, 0x6, 0xA, 0x5A, 0x8D, 0x9C, 0x25, 0xB4 } };

// 控件类型信息

static const DWORD _dwDRFPTUploaderOleMisc =
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDRFPTUploaderCtrl, IDS_DRFPTUPLOADER, _dwDRFPTUploaderOleMisc)

// CDRFPTUploaderCtrl::CDRFPTUploaderCtrlFactory::UpdateRegistry -
// 添加或移除 CDRFPTUploaderCtrl 的系统注册表项

BOOL CDRFPTUploaderCtrl::CDRFPTUploaderCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO:  验证您的控件是否符合单元模型线程处理规则。
	// 有关更多信息，请参考 MFC 技术说明 64。
	// 如果您的控件不符合单元模型规则，则
	// 必须修改如下代码，将第六个参数从
	// afxRegApartmentThreading 改为 0。

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_DRFPTUPLOADER,
			IDB_DRFPTUPLOADER,
			afxRegApartmentThreading,
			_dwDRFPTUploaderOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


// CDRFPTUploaderCtrl::CDRFPTUploaderCtrl - 构造函数

CDRFPTUploaderCtrl::CDRFPTUploaderCtrl()
{
	InitializeIIDs(&IID_DDRFPTUploader, &IID_DDRFPTUploaderEvents);
	// TODO:  在此初始化控件的实例数据。
}

// CDRFPTUploaderCtrl::~CDRFPTUploaderCtrl - 析构函数

CDRFPTUploaderCtrl::~CDRFPTUploaderCtrl()
{
	// TODO:  在此清理控件的实例数据。
}

// CDRFPTUploaderCtrl::OnDraw - 绘图函数

void CDRFPTUploaderCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */)
{
	if (!pdc)
		return;

	// TODO:  用您自己的绘图代码替换下面的代码。
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

// CDRFPTUploaderCtrl::DoPropExchange - 持久性支持

void CDRFPTUploaderCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO:  为每个持久的自定义属性调用 PX_ 函数。
}


// CDRFPTUploaderCtrl::OnResetState - 将控件重置为默认状态

void CDRFPTUploaderCtrl::OnResetState()
{
	COleControl::OnResetState();  // 重置 DoPropExchange 中找到的默认值

	// TODO:  在此重置任意其他控件状态。
}


// CDRFPTUploaderCtrl 消息处理程序




#include <afxinet.h>
LONG CDRFPTUploaderCtrl::SelectFile(LONG nOption)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO:  在此添加调度处理程序代码
	CString	strOpenImgFileFilter;
	strOpenImgFileFilter = _T("所有文件 (*.*)|*.*||");
	
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strOpenImgFileFilter.GetString(), this);
	if (dlg.DoModal() != IDOK)
		return __LINE__;

	CWaitCursor	waitCur;

	CString	strPathName = dlg.GetPathName();
	if (strPathName.IsEmpty())
		return __LINE__;

	CFtpConnection		*m_pFtpConnection = NULL;
	CInternetSession	m_Session;

	try
	{
		m_pFtpConnection = m_Session.GetFtpConnection(_T("123.57.72.97"), _T("bluevftp"), _T("bluevftp"), INTERNET_INVALID_PORT_NUMBER);

		m_pFtpConnection->SetCurrentDirectory(_T(".\\test"));
	}
	catch (CInternetException &e)
	{
		e.ReportError(MB_ICONEXCLAMATION);
		m_pFtpConnection = NULL;
		e.Delete();

		return __LINE__;
	}

	CString strFileRoot = strPathName;
	CString strFileName = strPathName.GetString() + strPathName.ReverseFind('\\') + 1;

	//查找重名文件
	//断点续传
	//上传进度
	//支持大于2G文件

	BOOL bUploaded = m_pFtpConnection->PutFile(strFileRoot, strFileName, FTP_TRANSFER_TYPE_BINARY, 1);

	return bUploaded;
}
