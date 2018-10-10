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
			// ����TCP������
			_tcpParam.preForkService = _tcpArgs._bPreFork;
			_tcpParam.nServerRecvTimeOut = 1;

			//cpu����*2+2 ͨ��Ϊ����߳���  
			SYSTEM_INFO		sysInfo;  
			GetSystemInfo(&sysInfo);  

			//tcpParam.threadLimit = sysInfo.dwNumberOfProcessors * 2 + 2;
			_tcpParam.threadLimit = 2;

			//ѡ�����������
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
				std::cout << ("��TCP����˿�ʧ��\r\n");
				throw BluevError(__FILE__, __LINE__);
			}
			else
			{
				std::cout << ("��TCP����˿ڳɹ�\r\n");
			}

			nRet = _pServer->start();
			if(nRet < 0)
			{
				std::cout << ("����TCP�����߳�ʧ��\r\n");
				throw BluevError(__FILE__, __LINE__); 
			}
			else
			{
				char	szBuf[100] = {0};
				sprintf(szBuf, ("����TCP�����̳߳ɹ����˿ں�=%d\r\n"), _tcpArgs._nPort);
				std::cout << szBuf;
			}

			std::cout << ("P2Pͨ�ŷ����������ɹ���\r\n");

			// �ȴ��������
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