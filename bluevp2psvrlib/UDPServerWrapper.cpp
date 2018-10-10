#include "bluevp2psvrdef.h"


namespace BLUEV {	namespace BVP2PSVRLIB{

	using BLUEVNET::UDPServer;
	using BLUEVNET::UDPServerEx;

	int UDPServerWrapper::run()	//����
	{
		try
		{
			//ѡ�����������
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

			//�û�������
			_ctxMgr.setSecond(60);	//������ʱ����� s

			UDPP2PService	*pUdpService = new UDPP2PService(_ctxMgr, *_pServer);
			UDPP2PService	*pUdpService2 = new UDPP2PService(_ctxMgr, *_pServer);

			_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2P, pUdpService);
			_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2PCLIENTANS, pUdpService2);

			int nRet = _pServer->open();
			if(nRet < 0)
			{
				std::cout << ("��UDP����˿�ʧ��\r\n");
				throw BluevError(__FILE__, __LINE__);
			}
			else
			{
				std::cout << ("��UDP����˿ڳɹ�\r\n");
			}

			nRet = _pServer->start();
			if(nRet < 0)
			{
				std::cout << ("����UDP�����߳�ʧ��\r\n");
				throw BluevError(__FILE__, __LINE__); 
			}
			else
			{
				char	szBuf[100] = {0};
				sprintf(szBuf, ("����UDP�����̳߳ɹ����˿ں�=%d\r\n"), _udpArgs._nPort);
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