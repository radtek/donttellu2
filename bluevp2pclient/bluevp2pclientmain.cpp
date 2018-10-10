
#include "../bluevp2pclientlib/bluevp2pclientdef.h"
#include <windows.h>
#include <dbghelp.h>
#include <iostream>
#include <process.h>
#include <conio.h>

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

	hFile = CreateFile(L"bluevp2pclient.dmp",
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

unsigned WINAPI UDPP2PClient_Thread_Proc(void *pParam)
{
	//bool bIsTest = *(bool*)pParam;
	using namespace BLUEV::BVP2PCLIENTLIB;

	bool bProxy = *(bool *)pParam;

	while(g_pUdpClient == NULL)
	{
		Sleep(1000);
	}

	while(g_pUdpClient->bPrepared == false)
	{
		Sleep(1000);
	}

	bool	bMainCtrl = g_pUdpClient->bMainCtrl;
	if( 0 > login())
	{
		std::cout << "登入失败" << std::endl;
		return -1;
	}

	keepAlive(10);

	if(bMainCtrl)
	{
		if(0 > getDevInfo())
		{
			;
		}
		else
		{
			if(bProxy)
				proxyConn(0);
			else
				connDev(0);
		}
	}

	//logout();

	return 0;
}


unsigned WINAPI TCPP2PClient_Thread_Proc(void *pParam)
{
	using namespace BLUEV::BVP2PCLIENTLIB;

	bool bProxy = *(bool *)pParam;

	while(g_pTcpClient == NULL || !g_pTcpClient->_bPrepared)
	{
		Sleep(1000);
	}

	bool	bMainCtrl = g_pTcpClient->_bMainCtrl;
	if( 0 > g_pTcpClient->login())
	{
		std::cout << "登入失败" << std::endl;
		return -1;
	}

	int		nSecond = 60;
	if(bMainCtrl)
	{
		if(0 > g_pTcpClient->getDevInfo())
		{
			std::cout << "获取设备信息失败" << std::endl;
			return -1;
		}

		connDevTCP(0);
		//nSecond = 60;
	}
	Sleep(5000);
	g_pTcpClient->logout();
	return 0;

	if(0 > keepAliveTCP(nSecond))
	{
		std::cout << "发起心跳线程失败" << std::endl;
		return -1;
	}

	if(bMainCtrl)
	{
		proxyReqTCP(0);
	}

	return 0;

	//g_pTcpClient->logout();

/*
	keepAlive(10);

	if(bMainCtrl)
	{
		if(0 > getDevInfo())
		{
			;
		}
		else
		{
			if(bProxy)
				proxyConn(0);
			else
				connDev(0);
		}
	}

	//logout();
*/
	return 0;
}

using namespace BLUEV::BVP2PCLIENTLIB;
using	std::wstring;

unsigned WINAPI TCPChatClient_Thread_Proc(void *pParam)
{
	wstring	s1 = L"break";
	wstring s2 = L"refresh";
	wstring s3 = L"chat";
	wstring	sbuf;

	BLUEV::BLUEVBASE::ServiceStatus	&svcStatus = *(ServiceStatus *)pParam;

	while(!svcStatus.isWillStop())
	{
		// 如果没有终止的情况下，我们就等待输入。
		while(_kbhit())
		{
			// 如果有键盘输入，我们就输入字符串。
#if 1
			int		n = _getwch();
			if ( n==0 || n==0xE0 )
			{
				n = _getwch();	// 这是一个功能键。
				continue;	// 跳过。
			}
			if ( n=='\n' || n=='\r' )
			{
				if ( sbuf==s1 )
				{
					_putwch(L'\n');
					std::cout<<"用户输入停止标志，结束运行！"<<std::endl;
					svcStatus.setWillStop(true);
					break;
				}
				else if( sbuf==s2 )
				{
					_putwch(L'\n');
					std::cout<<"用户输入刷新标志"<<std::endl;
					svcStatus.setRefresh(true);
				}
				else if(wstring::npos != sbuf.find(s3.c_str(), 0, s3.size()))
				{
					wchar_t wszCmd[100] = {0};
					wchar_t wszSend[100] = {0};
					swscanf(sbuf.c_str(), L"%s %s", wszCmd, wszSend);

					std::wstring	wstrSend = wszSend;
					std::string		temp(wstrSend.length(), ' ');
					std::copy(wstrSend.begin(), wstrSend.end(), temp.begin());

					_putwch(L'\n');
					chatMirror(temp);
				}
				else
				{
					sbuf.clear();
					_putwch(n);
					_putwch(L'\n');
				}

				sbuf.clear();
			}
			else
			{
				wchar_t	wt = (wchar_t)n;
				sbuf.append(&wt, 1);
				_putwch(wt);
			}
		}
#else
			int		n ;
			char*   dest=NULL;

			n= getch();
			strncpy(dest,(const char*)n,4);

			if ( strcmp(dest,"KEY_"))
				// 这是一个功能键。
				continue;	// 跳过。
			if ( n=='\n' || n=='\r' )
			{
				if ( sbuf==s1 )
				{
					putchar(L'\n');
					std::cout<<"用户输入停止标志，结束运行！"<<std::endl;
					svcStatus.setWillStop(true);
					break;
				}
				else if( sbuf==s2 )
				{
					putchar(L'\n');
					std::cout<<"用户输入刷新标志"<<std::endl;
					svcStatus.setRefresh(true);
				}
				else
				{
					sbuf.clear();
					putchar(n);
					putchar(L'\n');
				}
			}
			else
			{
				wchar_t	wt = (wchar_t)n;
				sbuf.append(&wt, 1);
				putchar(wt);
			}
		}
#endif
		// 睡眠0.2秒。1000微秒=1毫秒，1000毫秒=1秒。
		Sleep(200);
	}
	return	0;
}

int wmain(int argc, wchar_t* argv[])
{
	extern unsigned WINAPI UDPP2PClient_Thread_Proc(void *pParam);

	DBG_InstallExceptHandle();
	SetConsoleTitle(TEXT("BLUEV P2P通信客户端"));

	//if ( initMainPath() < 0 ) return -1;

	BLUEV::BLUEVBASE::ServiceStatus	svcStatus;

	bool	g_bProxy = false;
	bool	g_bUseTcp = false;
	
	if(argc > 1)
	{
		int	nUDPPort = _wtoi(argv[1]);
		g_bUseTcp = (nUDPPort <= 0) ? true : false;
	}

	if(argc > 6)
		g_bProxy = _wtoi(argv[6]) == 1 ? true : false;

	//发起线程，进行登入、登出等操作。
	HANDLE		hHandle;
	unsigned	tid;

	if(!g_bUseTcp)
	{
		hHandle = (HANDLE)_beginthreadex(0, 0, UDPP2PClient_Thread_Proc, &g_bProxy, 0, &tid);
		if(hHandle == INVALID_HANDLE_VALUE)
			return -1;
	}
	else
	{
#if 1
		hHandle = (HANDLE)_beginthreadex(0, 0, TCPP2PClient_Thread_Proc, &g_bProxy, 0, &tid);
		if(hHandle == INVALID_HANDLE_VALUE)
			return -1;
#else
		hHandle = (HANDLE)_beginthreadex(0, 0, TCPChatClient_Thread_Proc, &svcStatus, 0, &tid);
		if(hHandle == INVALID_HANDLE_VALUE)
			return -1;
#endif
	}

	CloseHandle(hHandle);
	

	return BLUEVP2P_ClientMain(argc, argv, svcStatus);
}
