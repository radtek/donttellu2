#pragma once

// DRFPTUploaderPropPage.h : CDRFPTUploaderPropPage ����ҳ���������


// CDRFPTUploaderPropPage : �й�ʵ�ֵ���Ϣ������� DRFPTUploaderPropPage.cpp��

class CDRFPTUploaderPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDRFPTUploaderPropPage)
	DECLARE_OLECREATE_EX(CDRFPTUploaderPropPage)

// ���캯��
public:
	CDRFPTUploaderPropPage();

// �Ի�������
	enum { IDD = IDD_PROPPAGE_DRFPTUPLOADER };

// ʵ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ��Ϣӳ��
protected:
	DECLARE_MESSAGE_MAP()
};

