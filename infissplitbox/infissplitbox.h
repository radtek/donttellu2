// infissplitbox.h : infissplitbox DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CinfissplitboxApp
// �йش���ʵ�ֵ���Ϣ������� infissplitbox.cpp
//

class CinfissplitboxApp : public CWinApp
{
public:
	CinfissplitboxApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

