#include "bluevp2psvrdef.h"


namespace BLUEV {	namespace BVP2PSVRLIB{

	using BLUEVNET::UDPServer;
	using BLUEVNET::UDPServerEx;

	int UDPServerWrapper::run()	//阻塞
	{
		try
		{
			//选择服务器类型
			switch(_udpArgs._nType)
			{
			case UDPServerArgs::UDPSERVER:
				_pServer = new UDPServer(_udpArgs._nPort, _udpServiceMgr, _locSvcStatus, _logger, _udpParam);
				break;
			case UDPServerArgs::UDPSERVEREX:
				_pServer = new UDPServerEx(_udpArgs._nPort, _udpServiceMgr, _locSvcStatus, _logger, _udpParam);
				break;
			default:
				return -1;
			}

			//用户管理器
			_ctxMgr.setSecond(60);	//心跳超时检测间隔 s

			UDPP2PService	*pUdpService = new UDPP2PService(_ctxMgr, *_pServer);
			UDPP2PService	*pUdpService2 = new UDPP2PService(_ctxMgr, *_pServer);

			_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2P, pUdpService);
			_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2PCLIENTANS, pUdpService2);

			int nRet = _pServer->open();
			if(nRet < 0)
			{
				std::cout << ("打开UDP服务端口失败\r\n");
				throw BluevError(__FILE__, __LINE__);
			}
			else
			{
				std::cout << ("打开UDP服务端口成功\r\n");
			}

			nRet = _pServer->start();
			if(nRet < 0)
			{
				std::cout << ("创建UDP服务线程失败\r\n");
				throw BluevError(__FILE__, __LINE__); 
			}
			else
			{
				char	szBuf[100] = {0};
				sprintf(szBuf, ("创建UDP服务线程成功。端口号=%d\r\n"), _udpArgs._nPort);
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