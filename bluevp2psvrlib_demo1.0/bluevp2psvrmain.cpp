#include "bluevp2psvrdef.h"
#include <iostream>

namespace BLUEV
{
	namespace BVP2PSVRLIB
	{
		using BLUEV::BLUEVBASE::IServiceStatus;
		using BLUEV::BLUEVBASE::ServiceStatus;
		using BLUEV::BLUEVNET::UDPServiceMgr;
		using BLUEV::BLUEVNET::UDPServer;
		using BLUEV::BLUEVNET::UDPServerEx;
		using BLUEVNET::BLUEVOpClass;
		using BLUEVNET::UDPServerParam;

		using BLUEVNET::TCPServer;
		using BLUEVNET::TCPServerParam;
		using BLUEVNET::ServiceMgr;

		using BLUEVNET::ILogger;
		using BLUEVBASE::BluevError;

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
		argv[1] udp svr port
		argv[2] tcp svr port
		argv[3] is tcp prefork 线程池

		*/

		BVP2PSVRLIB_API int BLUEVP2P_SvrMain(int argc, wchar_t **argv, IServiceStatus& svcStatus)
		{
			ServiceStatus	locSvcStatus;
			ILogger			*logger = NULL;
			char			szFileName[256] = {0};
			int				retval = -1, tempRet;

			int port = 14000;
			int tcpPort = port + 100;
			if(argc > 1)
				port = _wtoi(argv[1]);

			if(argc > 2)
				tcpPort = _wtoi(argv[2]);

			if(port <= 0 || tcpPort <= 0)
				return -1;

			int	preForkService = 0;
			if(argc > 3)
				preForkService = _wtoi(argv[3]);

			try
			{
				g_svcStatus = &svcStatus;
				svcStatus.addOther(&locSvcStatus);

				//Explicitly sets the locale to the current OEM code page obtained from the operating system.
				setlocale(LC_ALL, ".OCP");

				std::cout << ("启动控制台监控服务") << std::endl;

				//_CrtSetBreakAlloc(390);
				atexit(Exit);
				SetConsoleCtrlHandler(DealStopSignal, TRUE);

				//用户管理器
				CP2PUserCtxMgr	ctxMgr/*(var)*/;
				//ctxMgr.setSecond(60);	//心跳超时检测间隔 s
				ctxMgr.setSecond(120);	//demo版延长

				// 创建UDP服务器
				UDPServiceMgr	udpServiceMgr;

				UDPServerParam	udpParam = {0};
				UDPServerEx		udpServer(port, udpServiceMgr, locSvcStatus, logger, udpParam);

				UDPP2PService	udpService(ctxMgr, udpServer);
				udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2P, &udpService);

				tempRet = udpServer.open();
				if(tempRet < 0)
				{
					std::cout << ("打开UDP服务端口失败\r\n");
					throw BluevError(__FILE__, __LINE__);
				}
				else
				{
					std::cout << ("打开UDP服务端口成功\r\n");
				}

				tempRet = udpServer.start();
				if(tempRet < 0)
				{
					std::cout << ("创建UDP服务线程失败\r\n");
					throw BluevError(__FILE__, __LINE__); 
				}
				else
				{
					char	szBuf[100] = {0};
					sprintf(szBuf, ("创建UDP服务线程成功。端口号=%d\r\n"), port);
					std::cout << szBuf;
				}

				// 创建TCP服务器
				ServiceMgr		tcpServiceMgr;

				TCPServerParam	tcpParam;
				tcpParam.preForkService = preForkService == 1 ? 1 : 0;
				tcpParam.nServerRecvTimeOut = 1;
				tcpParam.threadLimit = 20;

				TCPServer		tcpServer(tcpPort, tcpServiceMgr, locSvcStatus, logger, tcpParam);

				TCPP2PService	tcpService(ctxMgr, tcpServer);
				tcpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2P, &tcpService);

				tempRet = tcpServer.open();
				if(tempRet < 0)
				{
					std::cout << ("打开TCP服务端口失败\r\n");
					throw BluevError(__FILE__, __LINE__);
				}
				else
				{
					std::cout << ("打开TCP服务端口成功\r\n");
				}

				tempRet = tcpServer.start();
				if(tempRet < 0)
				{
					std::cout << ("创建TCP服务线程失败\r\n");
					throw BluevError(__FILE__, __LINE__); 
				}
				else
				{
					char	szBuf[100] = {0};
					sprintf(szBuf, ("创建TCP服务线程成功。端口号=%d\r\n"), tcpPort);
					std::cout << szBuf;
				}

				std::cout << ("P2P通信服务器启动成功。\r\n");
				retval = 1;

				// 等待程序结束
				while(!locSvcStatus.isWillStop())
				{
					Sleep(1000);
				}

				udpServer.close();
				tcpServer.close();

			}
			catch(BluevError &e)
			{
				e.print();
			}
			catch(...)
			{
			}

			svcStatus.setWillStop(true);

			Sleep(1000);

			// 等待本程序创建的线程全部结束。
			while(locSvcStatus.getThreadCount() > 0)
			{
				Sleep(1000);
			}

			svcStatus.removeOther(&locSvcStatus);

			return	0;
		}

	}
}