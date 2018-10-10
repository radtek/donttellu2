// DRFPTUploader.cpp : CDRFPTUploaderApp 和 DLL 注册的实现。

#include "stdafx.h"
#include "DRFPTUploader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDRFPTUploaderApp theApp;

const GUID CDECL _tlid = { 0xDEE71F0C, 0x7228, 0x4CF0, { 0xAA, 0x2A, 0x1A, 0xF, 0xDF, 0x48, 0xA6, 0xBF } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CDRFPTUploaderApp::InitInstance - DLL 初始化

BOOL CDRFPTUploaderApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO:  在此添加您自己的模块初始化代码。
	}

	return bInit;
}



// CDRFPTUploaderApp::ExitInstance - DLL 终止

int CDRFPTUploaderApp::ExitInstance()
{
	// TODO:  在此添加您自己的模块终止代码。

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - 将项添加到系统注册表

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - 将项从系统注册表中移除

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
