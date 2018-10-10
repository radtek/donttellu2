#pragma once

// DRFPTUploaderPropPage.h : CDRFPTUploaderPropPage 属性页类的声明。


// CDRFPTUploaderPropPage : 有关实现的信息，请参阅 DRFPTUploaderPropPage.cpp。

class CDRFPTUploaderPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDRFPTUploaderPropPage)
	DECLARE_OLECREATE_EX(CDRFPTUploaderPropPage)

// 构造函数
public:
	CDRFPTUploaderPropPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_DRFPTUPLOADER };

// 实现
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 消息映射
protected:
	DECLARE_MESSAGE_MAP()
};

