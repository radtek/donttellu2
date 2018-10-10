// DRFPTUploaderCtrl.cpp : CDRFPTUploaderCtrl ActiveX �ؼ����ʵ�֡�

#include "stdafx.h"
#include "DRFPTUploader.h"
#include "DRFPTUploaderCtrl.h"
#include "DRFPTUploaderPropPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDRFPTUploaderCtrl, COleControl)

// ��Ϣӳ��

BEGIN_MESSAGE_MAP(CDRFPTUploaderCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

// ����ӳ��

BEGIN_DISPATCH_MAP(CDRFPTUploaderCtrl, COleControl)
	DISP_FUNCTION_ID(CDRFPTUploaderCtrl, "SelectFile", dispidSelectFile, SelectFile, VT_I4, VTS_I4)
END_DISPATCH_MAP()

// �¼�ӳ��

BEGIN_EVENT_MAP(CDRFPTUploaderCtrl, COleControl)
END_EVENT_MAP()

// ����ҳ

// TODO:  ����Ҫ��Ӹ�������ҳ��  ���ס���Ӽ���!
BEGIN_PROPPAGEIDS(CDRFPTUploaderCtrl, 1)
	PROPPAGEID(CDRFPTUploaderPropPage::guid)
END_PROPPAGEIDS(CDRFPTUploaderCtrl)

// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CDRFPTUploaderCtrl, "DRFPTUPLOADER.DRFPTUploaderCtrl.1",
	0x63f1676, 0x5d24, 0x419a, 0x95, 0xc4, 0x61, 0x18, 0x78, 0x63, 0x17, 0x79)

// ����� ID �Ͱ汾

IMPLEMENT_OLETYPELIB(CDRFPTUploaderCtrl, _tlid, _wVerMajor, _wVerMinor)

// �ӿ� ID

const IID IID_DDRFPTUploader = { 0xC454ABFE, 0x8C51, 0x467B, { 0xAE, 0x7B, 0x6, 0x7B, 0xD4, 0x55, 0xDF, 0xA1 } };
const IID IID_DDRFPTUploaderEvents = { 0xF1C79621, 0x7334, 0x419B, { 0x9D, 0x6, 0xA, 0x5A, 0x8D, 0x9C, 0x25, 0xB4 } };

// �ؼ�������Ϣ

static const DWORD _dwDRFPTUploaderOleMisc =
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDRFPTUploaderCtrl, IDS_DRFPTUPLOADER, _dwDRFPTUploaderOleMisc)

// CDRFPTUploaderCtrl::CDRFPTUploaderCtrlFactory::UpdateRegistry -
// ��ӻ��Ƴ� CDRFPTUploaderCtrl ��ϵͳע�����

BOOL CDRFPTUploaderCtrl::CDRFPTUploaderCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO:  ��֤���Ŀؼ��Ƿ���ϵ�Ԫģ���̴߳������
	// �йظ�����Ϣ����ο� MFC ����˵�� 64��
	// ������Ŀؼ������ϵ�Ԫģ�͹�����
	// �����޸����´��룬��������������
	// afxRegApartmentThreading ��Ϊ 0��

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


// CDRFPTUploaderCtrl::CDRFPTUploaderCtrl - ���캯��

CDRFPTUploaderCtrl::CDRFPTUploaderCtrl()
{
	InitializeIIDs(&IID_DDRFPTUploader, &IID_DDRFPTUploaderEvents);
	// TODO:  �ڴ˳�ʼ���ؼ���ʵ�����ݡ�
}

// CDRFPTUploaderCtrl::~CDRFPTUploaderCtrl - ��������

CDRFPTUploaderCtrl::~CDRFPTUploaderCtrl()
{
	// TODO:  �ڴ�����ؼ���ʵ�����ݡ�
}

// CDRFPTUploaderCtrl::OnDraw - ��ͼ����

void CDRFPTUploaderCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */)
{
	if (!pdc)
		return;

	// TODO:  �����Լ��Ļ�ͼ�����滻����Ĵ��롣
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

// CDRFPTUploaderCtrl::DoPropExchange - �־���֧��

void CDRFPTUploaderCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO:  Ϊÿ���־õ��Զ������Ե��� PX_ ������
}


// CDRFPTUploaderCtrl::OnResetState - ���ؼ�����ΪĬ��״̬

void CDRFPTUploaderCtrl::OnResetState()
{
	COleControl::OnResetState();  // ���� DoPropExchange ���ҵ���Ĭ��ֵ

	// TODO:  �ڴ��������������ؼ�״̬��
}


// CDRFPTUploaderCtrl ��Ϣ�������




#include <afxinet.h>
LONG CDRFPTUploaderCtrl::SelectFile(LONG nOption)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO:  �ڴ���ӵ��ȴ���������
	CString	strOpenImgFileFilter;
	strOpenImgFileFilter = _T("�����ļ� (*.*)|*.*||");
	
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

	//���������ļ�
	//�ϵ�����
	//�ϴ�����
	//֧�ִ���2G�ļ�

	BOOL bUploaded = m_pFtpConnection->PutFile(strFileRoot, strFileName, FTP_TRANSFER_TYPE_BINARY, 1);

	return bUploaded;
}
