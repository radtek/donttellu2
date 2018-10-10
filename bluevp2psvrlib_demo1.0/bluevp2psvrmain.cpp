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
				std::cout << "�û��������\r\n";
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
		argv[3] is tcp prefork �̳߳�

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

				std::cout << ("��������̨��ط���") << std::endl;

				//_CrtSetBreakAlloc(390);
				atexit(Exit);
				SetConsoleCtrlHandler(DealStopSignal, TRUE);

				//�û�������
				CP2PUserCtxMgr	ctxMgr/*(var)*/;
				//ctxMgr.setSecond(60);	//������ʱ����� s
				ctxMgr.setSecond(120);	//demo���ӳ�

				// ����UDP������
				UDPServiceMgr	udpServiceMgr;

				UDPServerParam	udpParam = {0};
				UDPServerEx		udpServer(port, udpServiceMgr, locSvcStatus, logger, udpParam);

				UDPP2PService	udpService(ctxMgr, udpServer);
				udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2P, &udpService);

				tempRet = udpServer.open();
				if(tempRet < 0)
				{
					std::cout << ("��UDP����˿�ʧ��\r\n");
					throw BluevError(__FILE__, __LINE__);
				}
				else
				{
					std::cout << ("��UDP����˿ڳɹ�\r\n");
				}

				tempRet = udpServer.start();
				if(tempRet < 0)
				{
					std::cout << ("����UDP�����߳�ʧ��\r\n");
					throw BluevError(__FILE__, __LINE__); 
				}
				else
				{
					char	szBuf[100] = {0};
					sprintf(szBuf, ("����UDP�����̳߳ɹ����˿ں�=%d\r\n"), port);
					std::cout << szBuf;
				}

				// ����TCP������
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
					std::cout << ("��TCP����˿�ʧ��\r\n");
					throw BluevError(__FILE__, __LINE__);
				}
				else
				{
					std::cout << ("��TCP����˿ڳɹ�\r\n");
				}

				tempRet = tcpServer.start();
				if(tempRet < 0)
				{
					std::cout << ("����TCP�����߳�ʧ��\r\n");
					throw BluevError(__FILE__, __LINE__); 
				}
				else
				{
					char	szBuf[100] = {0};
					sprintf(szBuf, ("����TCP�����̳߳ɹ����˿ں�=%d\r\n"), tcpPort);
					std::cout << szBuf;
				}

				std::cout << ("P2Pͨ�ŷ����������ɹ���\r\n");
				retval = 1;

				// �ȴ��������
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

			// �ȴ������򴴽����߳�ȫ��������
			while(locSvcStatus.getThreadCount() > 0)
			{
				Sleep(1000);
			}

			svcStatus.removeOther(&locSvcStatus);

			return	0;
		}

	}
}