#include "bluevp2psvrdef.h"


namespace BLUEV {	namespace BVP2PSVRLIB{

	using BLUEVNET::IOCPServer;
	using BLUEVNET::TCPServer;
	using BLUEVNET::TCPServerEx;

	int TCPServerWrapper::run()
	{
		if(!isReady())
			return -1;

		try
		{
			// 创建TCP服务器
			_tcpParam.preForkService = _tcpArgs._bPreFork;
			_tcpParam.nServerRecvTimeOut = 1;

			//cpu个数*2+2 通常为最佳线程数  
			SYSTEM_INFO		sysInfo;  
			GetSystemInfo(&sysInfo);  

			//tcpParam.threadLimit = sysInfo.dwNumberOfProcessors * 2 + 2;
			_tcpParam.threadLimit = 2;

			//选择服务器类型
			switch(_tcpArgs._nType)
			{
			case TCPServerArgs::TCPSERVER:
				_pServer = new TCPServer(_tcpArgs._nPort, _tcpServiceMgr, _locSvcStatus, _logger, _tcpParam);
				break;
			case TCPServerArgs::TCPSERVEREX:
				_pServer = new TCPServerEx(_tcpArgs._nPort, _tcpServiceMgr, _locSvcStatus, _logger, _tcpParam);
				break;
			case TCPServerArgs::IOCPSERVER:
				_pServer = new IOCPServer(_tcpArgs._nPort, _tcpServiceMgr, _locSvcStatus, _logger, _tcpParam);
				break;
			default:
				return -1;
			}

			TCPP2PService	*pTcpService = new TCPP2PService(_ctxTcpMgr, _ctxPstMgr, _pServer);
			TCPChatService	*pTcpService2 = new TCPChatService(_pServer);

			_tcpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2P, pTcpService);
			_tcpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_CHAT, pTcpService2);

			int nRet = _pServer->open();
			if(nRet < 0)
			{
				std::cout << ("打开TCP服务端口失败\r\n");
				throw BluevError(__FILE__, __LINE__);
			}
			else
			{
				std::cout << ("打开TCP服务端口成功\r\n");
			}

			nRet = _pServer->start();
			if(nRet < 0)
			{
				std::cout << ("创建TCP服务线程失败\r\n");
				throw BluevError(__FILE__, __LINE__); 
			}
			else
			{
				char	szBuf[100] = {0};
				sprintf(szBuf, ("创建TCP服务线程成功。端口号=%d\r\n"), _tcpArgs._nPort);
				std::cout << szBuf;
			}

			std::cout << ("P2P通信服务器启动成功。\r\n");

			// 等待程序结束
			while(!_locSvcStatus.isWillStop())
			{
				Sleep(1000);
			}

			_pServer->close();
		}
		catch(BluevError &e)
		{
			e.print();

			return -1;
		}

		return 0;
	}

}}