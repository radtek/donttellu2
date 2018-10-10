
#include "../bluevp2psvrlib/bluevp2psvrdef.h"
#include <windows.h>
#include <dbghelp.h>
#include <iostream>
#include <process.h>

#include <string>
#include <hash_map>

int		initMainPath()
{
	std::wcout<<L"主目录="<<std::endl;
	
	return	1;
}

/*
*  输出错误信息。
*/
LONG	WINAPI	DBG_DumpExcept(LPEXCEPTION_POINTERS exceptInfo)
{
	HANDLE	hFile = NULL;

	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId = GetCurrentThreadId();
	eInfo.ExceptionPointers = exceptInfo;
	eInfo.ClientPointers = FALSE;

	hFile = CreateFile(L"bluevp2psvr.dmp",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	if ( hFile )
	{
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpWithFullMemory,
			&eInfo,
			NULL,
			NULL);
		CloseHandle(hFile);	// 关闭文件
	}
	return	EXCEPTION_CONTINUE_SEARCH;
}

void	DBG_InstallExceptHandle(void)
{
	SetUnhandledExceptionFilter(DBG_DumpExcept);
}

int * getSingle()
{
	Sleep(0);
	static int xx = 1;
	return &xx;
}

unsigned WINAPI test_proc(void *pParam)
{
	while(1)
	{
		printf("%p\r\n", getSingle());
		Sleep(100);
	}
	return 0;
}


int wmain(int argc, wchar_t* argv[])
{
	using namespace BLUEV::BVP2PSVRLIB;

	DBG_InstallExceptHandle();
	SetConsoleTitle(TEXT("BLUEV P2P通信服务器"));

	//if ( initMainPath() < 0 ) return -1;
/*
	//发起线程，进行登入、登出等操作。
	HANDLE		hHandle;
	unsigned	tid;
	
	for(int i = 0; i < 5; ++i)
	{
		hHandle = (HANDLE)_beginthreadex(0, 0, test_proc, 0, 0, &tid);
		if(hHandle == INVALID_HANDLE_VALUE)
			return -1;
	}

	WaitForSingleObject(hHandle, INFINITE);
	*/

	

	//typedef std::hash_map<std::string, std::string> map_Tst_t;
	//map_Tst_t	mapTst;

	//mapTst.insert(std::make_pair("xx", "ff"));
	//mapTst.insert(std::make_pair("xx2", "ff"));
	//mapTst.insert(std::make_pair("xx3", "ff3"));

	//map_Tst_t::const_iterator	itr;
	//itr = mapTst.find("xx");
	


 	BLUEV::BLUEVBASE::ServiceStatus	svcStatus;
	//return 0;
	return BLUEVP2P_SvrMain(argc,  argv, svcStatus);
}
