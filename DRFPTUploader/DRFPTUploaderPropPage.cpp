// DRFPTUploaderPropPage.cpp : CDRFPTUploaderPropPage ����ҳ���ʵ�֡�

#include "stdafx.h"
#include "DRFPTUploader.h"
#include "DRFPTUploaderPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDRFPTUploaderPropPage, COlePropertyPage)

// ��Ϣӳ��

BEGIN_MESSAGE_MAP(CDRFPTUploaderPropPage, COlePropertyPage)
END_MESSAGE_MAP()

// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CDRFPTUploaderPropPage, "DRFPTUPLOADER.DRFPTUploaderPropPage.1",
	0x688d8e3d, 0x9562, 0x4f0c, 0x97, 0x9a, 0x8a, 0xf9, 0xd2, 0x4c, 0x43, 0x3c)

// CDRFPTUploaderPropPage::CDRFPTUploaderPropPageFactory::UpdateRegistry -
// ��ӻ��Ƴ� CDRFPTUploaderPropPage ��ϵͳע�����

BOOL CDRFPTUploaderPropPage::CDRFPTUploaderPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_DRFPTUPLOADER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}

// CDRFPTUploaderPropPage::CDRFPTUploaderPropPage - ���캯��

CDRFPTUploaderPropPage::CDRFPTUploaderPropPage() :
	COlePropertyPage(IDD, IDS_DRFPTUPLOADER_PPG_CAPTION)
{
}

// CDRFPTUploaderPropPage::DoDataExchange - ��ҳ�����Լ��ƶ�����

void CDRFPTUploaderPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// CDRFPTUploaderPropPage ��Ϣ�������
