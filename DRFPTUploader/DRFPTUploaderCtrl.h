#pragma once

// DRFPTUploaderCtrl.h : CDRFPTUploaderCtrl ActiveX �ؼ����������


// CDRFPTUploaderCtrl : �й�ʵ�ֵ���Ϣ������� DRFPTUploaderCtrl.cpp��

class CDRFPTUploaderCtrl : public COleControl
{
	DECLARE_DYNCREATE(CDRFPTUploaderCtrl)

// ���캯��
public:
	CDRFPTUploaderCtrl();

// ��д
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// ʵ��
protected:
	~CDRFPTUploaderCtrl();

	DECLARE_OLECREATE_EX(CDRFPTUploaderCtrl)    // �๤���� guid
	DECLARE_OLETYPELIB(CDRFPTUploaderCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CDRFPTUploaderCtrl)     // ����ҳ ID
	DECLARE_OLECTLTYPE(CDRFPTUploaderCtrl)		// �������ƺ�����״̬

// ��Ϣӳ��
	DECLARE_MESSAGE_MAP()

// ����ӳ��
	DECLARE_DISPATCH_MAP()

// �¼�ӳ��
	DECLARE_EVENT_MAP()

// ���Ⱥ��¼� ID
public:
	enum {
		dispidSelectFile = 1L
	};
protected:
	LONG SelectFile(LONG nOption);
};

