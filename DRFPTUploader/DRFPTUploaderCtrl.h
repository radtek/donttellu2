#pragma once

// DRFPTUploaderCtrl.h : CDRFPTUploaderCtrl ActiveX 控件类的声明。


// CDRFPTUploaderCtrl : 有关实现的信息，请参阅 DRFPTUploaderCtrl.cpp。

class CDRFPTUploaderCtrl : public COleControl
{
	DECLARE_DYNCREATE(CDRFPTUploaderCtrl)

// 构造函数
public:
	CDRFPTUploaderCtrl();

// 重写
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// 实现
protected:
	~CDRFPTUploaderCtrl();

	DECLARE_OLECREATE_EX(CDRFPTUploaderCtrl)    // 类工厂和 guid
	DECLARE_OLETYPELIB(CDRFPTUploaderCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CDRFPTUploaderCtrl)     // 属性页 ID
	DECLARE_OLECTLTYPE(CDRFPTUploaderCtrl)		// 类型名称和杂项状态

// 消息映射
	DECLARE_MESSAGE_MAP()

// 调度映射
	DECLARE_DISPATCH_MAP()

// 事件映射
	DECLARE_EVENT_MAP()

// 调度和事件 ID
public:
	enum {
		dispidSelectFile = 1L
	};
protected:
	LONG SelectFile(LONG nOption);
};

