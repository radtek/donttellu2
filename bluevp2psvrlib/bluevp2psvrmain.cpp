#include "bluevp2psvrdef.h"
#include <iostream>

namespace BLUEV
{
	namespace BVP2PSVRLIB
	{
		using BLUEV::BLUEVBASE::IServiceStatus;
		using BLUEV::BLUEVBASE::ServiceStatus;
		
		using BLUEVNET::ILogger;
		using BLUEVBASE::BluevError;
		using BLUEVBASE::UTIL_CONV;

		/*static */IServiceStatus*	g_svcStatus = 0;

		extern	"C"
		static void	STOP_Handler(void)
		{
			if ( g_svcStatus == NULL ) return;
			g_svcStatus->setWillStop(true);
		}

		extern	"C"
		BOOL	WINAPI	DealStopSignal(DWORD dwCtrlType)
		{
			if ( dwCtrlType==CTRL_C_EVENT || dwCtrlType==CTRL_BREAK_EVENT ||
				dwCtrlType==CTRL_CLOSE_EVENT || dwCtrlType==CTRL_LOGOFF_EVENT ||
				dwCtrlType==CTRL_SHUTDOWN_EVENT )
			{
				std::cout << "用户请求结束\r\n";
				STOP_Handler();
			}

			return	TRUE;
		}

		void Exit()
		{
			int i = _CrtDumpMemoryLeaks();

			assert( i == 0);
		}

		/*
		argv[0]	proc name
		argv[1] udp svr port  if this port is 0, then only open tcpserver
		argv[2] tcp svr port
		argv[3] is tcp prefork 线程池

		*/

		BVP2PSVRLIB_API int BLUEVP2P_SvrMain(int argc, wchar_t **argv, IServiceStatus& svcStatus)
		{
			ServiceStatus	locSvcStatus;
			ILogger			*logger = NULL;
			int				retval = -1;
			bool			bTCPServer = false;

			TCPServerArgs	tcpArg;
			UDPServerArgs	udpArg;

			tcpArg._nType = TCPServerArgs::IOCPSERVER;
			udpArg._nType = UDPServerArgs::UDPSERVEREX;

			if(0 > udpArg.parseArgs(argc, argv))
			{
				bTCPServer = true;
				
				if(0 > tcpArg.parseArgs(argc, argv))
					return -1;
			}

			g_svcStatus = &svcStatus;
			svcStatus.addOther(&locSvcStatus);

			//Explicitly sets the locale to the current OEM code page obtained from the operating system.
			setlocale(LC_ALL, ".OCP");

			std::cout << ("启动控制台监控服务") << std::endl;

			//_CrtSetBreakAlloc(390);
			atexit(Exit);
			SetConsoleCtrlHandler(DealStopSignal, TRUE);

			UDPServerWrapper	udpWrapper(locSvcStatus, logger, udpArg);
			TCPServerWrapper	tcpWrapper(locSvcStatus, logger, tcpArg);

			if(!bTCPServer)
			{
				udpWrapper.run();
			}
			else
			{	
				while(!tcpWrapper.isReady())
				{
					std::cout << "服务器初始化中。。\r\n";
					Sleep(2000);
					if(svcStatus.isWillStop())
						break;
				}

				if(tcpWrapper.isReady())
				{
					std::cout << "服务器初始化完毕\r\n";

					tcpWrapper.run();
				}
			}
				
			svcStatus.setWillStop(true);

			// 等待本程序创建的线程全部结束。
			do
			{
				Sleep(1000);
			}
			while(locSvcStatus.getThreadCount() > 0);

			svcStatus.removeOther(&locSvcStatus);

			return	0;
		}

	}
}