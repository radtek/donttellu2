// OGF_usbtst.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OGF_usbtst.h"
#include "OGF_usbtstDlg.h"
#include <tlhelp32.h>  //20120118

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COGF_usbtstApp

BEGIN_MESSAGE_MAP(COGF_usbtstApp, CWinApp)
	//{{AFX_MSG_MAP(COGF_usbtstApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COGF_usbtstApp construction

COGF_usbtstApp::COGF_usbtstApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COGF_usbtstApp object

COGF_usbtstApp theApp;

/////////////////////////////////////////////////////////////////////////////

// COGF_usbtstApp initialization
//20120118
DWORD GetProcessIdFromName(LPCTSTR name)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hSnapshot,&pe) )
		return 0;
	do
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if( Process32Next(hSnapshot,&pe)==FALSE )
			break;
		if(strcmp(pe.szExeFile,name) == 0)
		{
			id = pe.th32ProcessID;
			break;
		}
	} while(1);
	CloseHandle(hSnapshot);
	return id;
}

BOOL COGF_usbtstApp::InitInstance()
{
	// Robin 20120118
	//////////////////////////////////////////////////////////////////////////
	//防止多重启动的方法
	HANDLE Handle;
	Handle = CreateMutex(NULL,FALSE,_T("OGF_usbtst"));
	if (Handle == NULL)
		return FALSE;

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// for hardware testing
		DWORD id = GetProcessIdFromName(_T("OGF_usbtst.exe")) ;		
		if (id > 0)   
		{   
			DWORD idcur = GetCurrentProcessId() ;
			if (id != idcur)
			{
				if (IDYES == MessageBox(NULL,_T("检测到另一个OGF_usbtst进程正在运行， 是否现在终止？"),_T("提示"),MB_YESNO|MB_ICONQUESTION))
				{
					//HANDLE myhandle = OpenProcess(PROCESS_ALL_ACCESS,TRUE,id);   
					HANDLE myhandle = OpenProcess(PROCESS_TERMINATE,FALSE,id);   
					DWORD exitcode = 0;   
					TerminateProcess(myhandle,exitcode); 
					AfxMessageBox(_T("OGF_usbtst进程已终止，请再次执行OGF_usbtst.exe")) ;
					CloseHandle(Handle) ;
					return FALSE ;
				}
				else
				{
					//CloseHandle(Handle) ;
					//return FALSE ;
				}
			}
		}
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	COGF_usbtstDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
