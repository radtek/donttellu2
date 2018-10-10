
#include "bluevp2pclientdef.h"
#include <iostream>
#include <process.h>

namespace BLUEV
{
	namespace BVP2PCLIENTLIB
	{
		using BLUEVNET::ILogger;
		using BLUEV::BLUEVBASE::IServiceStatus;
		using BLUEV::BLUEVBASE::ServiceStatus;
		using BLUEVNET::UDPServiceMgr;
		using BLUEVNET::UDPServerParam;
		using BVP2PCLIENTLIB::UDPP2PClient;
		using BVP2PCLIENTLIB::UDPP2PClientService;
		using BVP2PSVRLIB::IServerArgs;
		using BVP2PSVRLIB::IServerWrapper;
		//using BVP2PSVRLIB::TCPChatService;
		using BLUEVNET::TCPServer;
		using BLUEVNET::TCPServerEx;
		using BLUEVNET::IOCPServer;
		using BLUEVBASE::BluevError;

		static IServiceStatus*	g_svcStatus = 0;
		BVP2PCLIENTLIB_VAR  UDPP2PClient	*g_pUdpClient = NULL;
		BVP2PCLIENTLIB_VAR  TCPClient	*g_pTcpClient = NULL;

		/*
		argv[0]	proc name
		argv[1] udp client port
		argv[2] svr addr
		argv[3] svr port
		argv[4] is main ctrl
		argv[5] port scan range
		argv[6] is proxy
		argv[7] tcp client port
		
		*/
#define UDPCLIENT_PORT	6100
#define TCPCLIENT_PORT	6200

		class UDPClientArgs : public IServerArgs
		{
		public:
			std::string	_strSvrAddr;
			int			_nSvrPort;
			int			_nScanRange;
			bool		_bMainCtrl;
			bool		_bProxy;

			UDPClientArgs()
			{
				_nPort = UDPCLIENT_PORT;
				_strSvrAddr = "127.0.0.1";
				_nSvrPort = UDPSERVER_PORT;
				_nScanRange = 0;
				_bMainCtrl = false;
				_bProxy = false;

			}

			int parseArgs(int argc, wchar_t **argv)
			{
				if(argc < 1 || argv == NULL)
					return -1;

				for(int i = 0; i < argc; ++i)
				{
					switch(i)
					{
					case 0:
						_wstrProcName = argv[0];
					case 1:
						_nPort = _wtoi(argv[1]);

						if(_nPort <= 0)
							return -1;

						break;
					case 2:
						{	
							std::wstring	wstrSvrAddr = argv[2];
							std::string		temp(wstrSvrAddr.length(), ' ');
							std::copy(wstrSvrAddr.begin(), wstrSvrAddr.end(), temp.begin());
							_strSvrAddr = temp;

							break;
						}
					case 3:
						_nSvrPort = _wtoi(argv[3]);

						break;
					case 4:
						_bMainCtrl = _wtoi(argv[4]) ? true : false;

						break;
					case 5:
						_nScanRange = _wtoi(argv[5]);

						break;
					case 6:
						_bProxy = _wtoi(argv[6]) == 1 ? true : false;

						break;
					default:
						break;
					}
				}

				std::cout << "UDP PORT: " << _nPort << std::endl;
				std::cout << "SERVER ADDR: " << _strSvrAddr << std::endl;
				std::cout << "SERVER PORT: " << _nSvrPort << std::endl;
				std::cout << "MAINCTRL: " << _bMainCtrl << std::endl;
				std::cout << "SCAN RANGE: " << _nScanRange << std::endl;
				std::cout << "PROXY: " << _bProxy << std::endl;

				return 0;
			}
		};

		class TCPClientArgs : public IServerArgs
		{
		public:
			std::string	_strSvrAddr;
			int			_nSvrPort;
			int			_nScanRange;
			bool		_bMainCtrl;
			bool		_bProxy;

			TCPClientArgs()
			{
				_nPort = TCPCLIENT_PORT;
				_strSvrAddr = "127.0.0.1";
				_nSvrPort = TCPSERVER_PORT;
				_nScanRange = 0;
				_bMainCtrl = false;
				_bProxy = false;

			}

			int parseArgs(int argc, wchar_t **argv)
			{
				if(argc < 1 || argv == NULL)
					return -1;

				for(int i = 0; i < argc; ++i)
				{
					switch(i)
					{
					case 0:
						_wstrProcName = argv[0];
					case 1:
						continue;

					case 2:
						{	
							std::wstring	wstrSvrAddr = argv[2];
							std::string		temp(wstrSvrAddr.length(), ' ');
							std::copy(wstrSvrAddr.begin(), wstrSvrAddr.end(), temp.begin());
							_strSvrAddr = temp;

							break;
						}
					case 3:
						_nSvrPort = _wtoi(argv[3]);

						break;
					case 4:
						_bMainCtrl = _wtoi(argv[4]) ? true : false;

						break;
					case 5:
						_nScanRange = _wtoi(argv[5]);

						break;
					case 6:
						_bProxy = _wtoi(argv[6]) == 1 ? true : false;

						break;
					case 7:
						_nPort = _wtoi(argv[7]);

						break;
					default:

						break;
					}
				}
				
				std::cout << "TCP CLIENT PORT: " << _nPort << std::endl;
				std::cout << "SERVER ADDR: " << _strSvrAddr << std::endl;
				std::cout << "SERVER PORT: " << _nSvrPort << std::endl;
				std::cout << "MAINCTRL: " << _bMainCtrl << std::endl;
				std::cout << "SCAN RANGE: " << _nScanRange << std::endl;
				std::cout << "PROXY: " << _bProxy << std::endl;

				return 0;
			}
		};

		class UDPClientWrapper : public IServerWrapper
		{
		public:
			UDPClientArgs	&_udpArgs;
			UDPP2PClient	*_pClient;

			UDPServiceMgr	_udpServiceMgr;
			UDPServerParam	_udpParam;

			UDPClientWrapper(ServiceStatus &locSvcStatus,
				ILogger	*logger,
				UDPClientArgs &udpArgs) :
			IServerWrapper(locSvcStatus, logger),
				_udpArgs(udpArgs)
			{
				_pClient = NULL;
				memset(&_udpParam, 0, sizeof(_udpParam));
			}

			~UDPClientWrapper()
			{
				if(_pClient)
					delete _pClient;
			}

			int run()
			{
				// 创建UDP服务器
				_pClient = new UDPP2PClient(_udpArgs._nPort, _udpServiceMgr, _locSvcStatus, _logger, _udpParam, _udpArgs._bMainCtrl);
				_pClient->strSvrAddr = _udpArgs._strSvrAddr;
				_pClient->svrPort = _udpArgs._nSvrPort;
				_pClient->nRange = _udpArgs._nScanRange;

				g_pUdpClient = _pClient;

				//注册服务
				UDPP2PClientService		udpService(_pClient);
				UDPP2PClientService		udpService2(_pClient);
				UDPP2PClientService		udpService3(_pClient);
				UDPP2PClientService		udpService4(_pClient);
				UDPP2PClientService		udpService5(_pClient);
				UDPP2PClientService		udpService6(_pClient);

				if(!_udpArgs._bMainCtrl)
				{
					//被控端设备
					_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2P, &udpService);
				}

				_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2PANS, &udpService2);
				_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2PCLIENT, &udpService3);
				_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2PCLIENTANS, &udpService4);
				_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2PPROXY, &udpService5);
				_udpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_P2PPROXYANS, &udpService6);


				if(1)
				{
					if ( _pClient->open()<0 )
					{
						std::cout << ("打开UDP客户端口失败\r\n");
						throw std::exception(__FILE__, __LINE__);
					}
					else
					{
						char	szBuf[100] = {0};
						sprintf(szBuf, ("打开UDP客户接口成功，端口号为=%d\r\n"), _udpArgs._nPort);
						std::cout << szBuf;
					}

					int tempRet = _pClient->start();

					if ( tempRet < 0 )
					{
						std::cout << ("创建UDP客户线程失败\r\n");
						throw std::exception(__FILE__, __LINE__);
					}
					else
					{
						std::cout << ("创建UDP客户线程成功\r\n");
					}

					char	szBuf[100] = {0};
					sprintf(szBuf, ("BLUEV P2P客户端启动成功。端口号=%d\r\n"), _udpArgs._nPort);
					std::cout << szBuf;

				}

				// 等待程序结束
				while(!_locSvcStatus.isWillStop())
				{
					Sleep(1000);
				}

				_pClient->close();

				return 0;
			}
		};

		using BLUEVNET::ServiceMgr;
		using BLUEVNET::TCPServerParam;

		class TCPClientWrapper : public IServerWrapper
		{
		public:
			TCPClientArgs	&_tcpArgs;
			TCPClient		*_pClient;
			TCPServer		*_pServer;	//被控端需要启一个server

			ServiceMgr		_tcpServiceMgr;
			TCPServerParam	_tcpParam;

			TCPClientWrapper(
				ServiceStatus	&locSvcStatus,
				ILogger			*logger,
				TCPClientArgs	&tcpArgs
				) : 
			IServerWrapper(locSvcStatus, logger),
				_tcpArgs(tcpArgs)
			{
				_pClient = NULL;
				_pServer = NULL;
			}

			~TCPClientWrapper()
			{
				if(_pClient)
					delete _pClient;

				if(_pServer)
					delete _pServer;
			}

			int run()
			{
				ServiceStatus	locSvcStatus2;
				ServiceMgr		tcpServiceMgr2;
				TCPServerParam	tcpParam2;

				TCPServer aa(7689, tcpServiceMgr2, locSvcStatus2, _logger, tcpParam2);
				_pServer = &aa;

				if(!_tcpArgs._bMainCtrl)
				{
					// 创建TCP服务器
					tcpParam2.preForkService = 1;
					tcpParam2.nServerRecvTimeOut = 1;

					//cpu个数*2+2 通常为最佳线程数  
					//SYSTEM_INFO		sysInfo;  
					//GetSystemInfo(&sysInfo);  

					//tcpParam.threadLimit = sysInfo.dwNumberOfProcessors * 2 + 2;
					tcpParam2.threadLimit = 2;
					
					//_pServer = new TCPServer(7689, tcpServiceMgr2, locSvcStatus2, _logger, tcpParam2);
					//_pServer = new TCPServerEx(7689, tcpServiceMgr2, locSvcStatus2, _logger, tcpParam2);
					//_pServer = new IOCPServer(7689, tcpServiceMgr2, locSvcStatus2, _logger, tcpParam2);

					//TCPChatService	*pTcpService = new TCPChatService(_pServer);
					//_tcpServiceMgr.registerService(BLUEVOpClass::OP_CLASS_CHAT, pTcpService);

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
						sprintf(szBuf, ("创建TCP服务线程成功。端口号=%d\r\n"), 7689);
						std::cout << szBuf;
					}

					std::cout << ("被控端服务器启动成功。\r\n");
				}
				
				TCPClient	bb;
				_pClient = &bb;
				//_pClient = new TCPClient;

				_pClient->_strSvrAddr = _tcpArgs._strSvrAddr;
				_pClient->_nSvrPort = _tcpArgs._nSvrPort;
				_pClient->_bMainCtrl = _tcpArgs._bMainCtrl;
				_pClient->_nPort = _tcpArgs._nPort;

				g_pTcpClient = _pClient;

				_pClient->open();
				_pClient->start();

				std::cout << ("客户端启动成功。\r\n");

				// 等待程序结束
				while(!_locSvcStatus.isWillStop())
				{
					Sleep(1000);
				}

				_pClient->close();
				if(_pServer)
					_pServer->close();

				return 0;
			}
		};

		BVP2PCLIENTLIB_API int BLUEVP2P_ClientMain(int argc, wchar_t **argv, IServiceStatus& svcStatus)
		{
			ServiceStatus	locSvcStatus;
			ILogger			*logger = NULL;
			int				retval = -1;
			bool			bTCPServer = false;

			TCPClientArgs	tcpArg;
			UDPClientArgs	udpArg;
			
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

			UDPClientWrapper	udpWrapper(locSvcStatus, logger, udpArg);
			TCPClientWrapper	tcpWrapper(locSvcStatus, logger, tcpArg);

			if(!bTCPServer)
			{
				udpWrapper.run();
			}
			else
			{
				tcpWrapper.run();
			}

			svcStatus.setWillStop(true);

			// 等待本程序创建的线程全部结束。
			do
			{
				Sleep(1000);
			}
			while(locSvcStatus.getThreadCount() > 0);

			svcStatus.removeOther(&locSvcStatus);

			g_pUdpClient = NULL;

			return	0;
		}

		/*	UDP	Function	*/
		int BVP2PCLIENTLIB_API login()
		{
			if(g_pUdpClient == NULL)
				return -1;

			int i = 5;
			while(g_pUdpClient->sessionId <= 0 && i > 0)
			{
				g_pUdpClient->login();
				Sleep(1000);
				i--;
			}

			if(g_pUdpClient->sessionId <= 0)
				return -1;

			//std::cout << "登入成功 ID = " << g_pUdpClient->sessionId << std::endl;

			return 0;
		}

		int BVP2PCLIENTLIB_API logout()
		{
			if(g_pUdpClient == NULL)
				return -1;

			int i = 5;
			while(g_pUdpClient->sessionId != 0 && i > 0)
			{
				g_pUdpClient->logout();
				Sleep(1000);
				i--;
			}

			if(g_pUdpClient->sessionId != 0)
				return -1;

			//std::cout << "登出成功 " << std::endl;

			return 0;
		}

		BVP2PCLIENTLIB_API int getDevInfo()
		{
			if(g_pUdpClient == NULL)
				return -1;

			if(g_pUdpClient->sessionId <= 0)
				return -1;

			g_pUdpClient->mapDevs.clear();

			int i = 5;
			while(g_pUdpClient->mapDevs.size() <= 0 && i > 0)
			{
				g_pUdpClient->getDevInfo();
				Sleep(1000);
				i--;
			}

			if(g_pUdpClient->mapDevs.size() <= 0)
				return -1;

			std::cout << "获取客户列表成功 SIZE = " << g_pUdpClient->mapDevs.size() << std::endl;

			return 0;
		}

		BVP2PCLIENTLIB_API int connDev(int nIndex)
		{
			if(g_pUdpClient == NULL || nIndex < 0)
				return -1;

			if(nIndex > (int)g_pUdpClient->mapDevs.size())
				nIndex = g_pUdpClient->mapDevs.size();

			/*if(g_pUdpClient->mapDevs.end() == g_pUdpClient->mapDevs.find()))
				return -1;*/
			int i = 0;
			std::map<__int64, P2PDevInfo *>::iterator itr;
			for(itr = g_pUdpClient->mapDevs.begin();
				itr != g_pUdpClient->mapDevs.end();
				itr++)
			{
				if(i == nIndex)
					break;

				i++;
			}

			const P2PDevInfo	*pDev = itr->second;
			if(pDev->_bSameLan)
			{
				int i = 5;

				while(i > 0)
				{
					if(pDev->getConnected())
						break;

					if(g_pUdpClient->connDevDirect(*pDev) < 0)
						return -1;
					
					Sleep(1000);
					i--;
				}
				
				//直连成功，通知服务端
				if(pDev->getConnected())
				{
					if(g_pUdpClient->connDevWithSvr(*pDev) < 0)
						return -1;
				}
			}
			else
			{
				int i = 5;

				while(i > 0)
				{
					if(pDev->getConnected())
						break;

					if(g_pUdpClient->connDevWithSvr(*pDev) < 0)
						return -1;

					//通知服务器的同时直连设备，以求与来自设备端的打洞建立通信
					if(g_pUdpClient->connDevDirect(*pDev) < 0)
						return -1;

					Sleep(1000);
					i--;
				}

			}

			//std::cout << "请求连接客户端成功" << std::endl;

			return 0;
		}

		unsigned WINAPI UDPP2PClient_KeepAlive_Proc(void *pParam)
		{
			if(g_pUdpClient == NULL)
				return -1;

			UDPP2PClient	*pUdpClient = g_pUdpClient;
			int nSecond = 10;

			if(pParam)
				nSecond = (int)pParam;

			if(nSecond <= 0)
				nSecond = 10;

			do 
			{
				//维持与服务器连接。

				//发送心跳
				pUdpClient->pulse(pUdpClient->strSvrAddr, pUdpClient->svrPort, true);

				//维持与客户端连接。
				if(pUdpClient->mapDevs.size() > 0)
				{
					std::map<__int64, P2PDevInfo *>::const_iterator itr;
					const P2PDevInfo	*pDev = NULL;
					for(itr = pUdpClient->mapDevs.begin();
						itr != pUdpClient->mapDevs.end();
						itr++
						)
					{
						if(pUdpClient->sessionId == itr->first)
							continue;

						pDev = itr->second;
						if(!pDev->getConnected())
							continue;

						//发送心跳

						//同一内网直连
						if(pDev->_bSameLan)
						{
							pUdpClient->pulse(pDev->_strLANIP, pDev->_nLANPort, false);
						}
						else
							pUdpClient->pulse(pDev->getIP(), pDev->getPort(), false);
					}
				}

				//默认十秒
				Sleep(nSecond * 1000);

			}while(1);

			return 0;
		}

		BVP2PCLIENTLIB_API int keepAlive(int nSecond)
		{
			if(nSecond <= 0)
				nSecond = 10;

			HANDLE		hHandle;
			unsigned	tid;

			hHandle = (HANDLE)_beginthreadex(0, 0, UDPP2PClient_KeepAlive_Proc, (void *)nSecond, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			return 0;
		}

		BVP2PCLIENTLIB_API int proxyConn(int nIndex)
		{
			if(g_pUdpClient == NULL)
				return -1;

			if(nIndex < 0)
				return -1;

			if(nIndex > (int)g_pUdpClient->mapDevs.size())
				nIndex = g_pUdpClient->mapDevs.size();

			/*if(g_pUdpClient->mapDevs.end() == g_pUdpClient->mapDevs.find()))
				return -1;*/
			int i = 0;
			std::map<__int64, P2PDevInfo *>::iterator itr;
			for(itr = g_pUdpClient->mapDevs.begin();
				itr != g_pUdpClient->mapDevs.end();
				itr++)
			{
				if(i == nIndex)
					break;

				i++;
			}

			P2PDevInfo	*pDev = itr->second;
#if 0
			if(pDev->_bSameLan)
			{
				int i = 5;

				while(i > 0)
				{
					if(pDev->getConnected())
						break;

					if(g_pUdpClient->connDevDirect(*pDev) < 0)
						return -1;
					
					Sleep(1000);
					i--;
				}
				
				//直连成功，通知服务端
				if(pDev->getConnected())
				{
					if(g_pUdpClient->connDevWithSvr(*pDev) < 0)
						return -1;
				}
			}
			else
#endif
			{
				pDev->_bProxy = true;
				int i = 5;
				
				/*{“Cmd”:”102A”,”GW”:”1a1001011010”,”SD”:[{ID:“00001001”,
”Ctrl”:”1”,”Value”:”1”}]}*/
				while(1)//i > 0)
				{

					Json::Value	root, sds, sd;
					Json::FastWriter	writer;
					root["Cmd"] = Json::Value("102A");
					root["GW"] = Json::Value(pDev->_strDevId);

					sd["ID"] = Json::Value("00001001");

					if(i % 2)
						sd["Ctrl"] = Json::Value("1");
					else
						sd["Ctrl"] = Json::Value("0");

					sd["Value"] = Json::Value("1");
					sds[0] = sd;
					root["SD"] = sds;

					std::string	strJson = writer.write(root);
					if(strJson.empty())
						return -1;

					std::string strData = "####Json=";
					strData.append(strJson);
					strData.append("****");

					std::cout << strData << std::endl;

					if(pDev->getConnected())
						break;

					if(g_pUdpClient->proxyRequest(*pDev, strData) < 0)
						return -1;

					Sleep(10000);
					i--;
				}

			}

			//std::cout << "请求连接客户端成功" << std::endl;

			return 0;
		}



		/*	TCP	Function	*/
		BVP2PCLIENTLIB_API int chatMirror(std::string &strSend)
		{
			if(g_pTcpClient == NULL || strSend.empty())
				return -1;

			g_pTcpClient->chatMirror(strSend);

			return 0;
		}

		BVP2PCLIENTLIB_API int connDevTCP(int nIndex)
		{
			if(g_pTcpClient == NULL || nIndex < 0)
				return -1;

			{
				//RWLockerEx	lock(g_pTcpClient->_rwLock, false);

				if(nIndex > (int)g_pTcpClient->_mapDevs.size())
					nIndex = g_pTcpClient->_mapDevs.size();

				int i = 0;
				map_devs_t::iterator itr;
				for(itr = g_pTcpClient->_mapDevs.begin();
					itr != g_pTcpClient->_mapDevs.end();
					++itr, ++i)
				{
					if(i == nIndex)
						break;
				}

				TCPDevInfo *pDev = itr->second;

				if(g_pTcpClient->connDevDirect(*pDev) < 0)
					return -1;
			}
			
			std::cout << "联通" << std::endl;
			return 0;
		}

		unsigned WINAPI TCPClient_KeepAlive_Proc(void *pParam)
		{
			if(g_pTcpClient == NULL)
				return -1;

			TCPClient	*pClient = g_pTcpClient;
			int nSecond = 10;

			if(pParam)
				nSecond = (int)pParam;

			if(nSecond <= 0)
				nSecond = 10;

			std::cout << "TCP心跳处理线程启动成功\r\n";

			do 
			{
				//默认十秒
				Sleep(nSecond * 1000);

				//维持与服务器连接。

				//发送心跳
				{
					LockerEx	lock(pClient->_stCrit);
					pClient->pulse(pClient->_nSocket);
				}
				

				//不需要维持
				continue;

				{
					//RWLockerEx	lock(pClient->_rwLock, true);

					//维持与客户端连接。
					if(pClient->_bMainCtrl && pClient->_mapDevs.size() > 0)
					{
						map_devs_t::const_iterator itr;
						const TCPDevInfo	*pDev = NULL;
						for(itr = pClient->_mapDevs.begin();
							itr != pClient->_mapDevs.end();
							itr++
							)
						{
							if(pClient->_strSessionId == itr->first)
								continue;

							pDev = itr->second;
							if(!pDev->_bConnected)
								continue;

							//发送心跳
							pClient->pulse(pDev->_nSocket);
						}
					}
				}
				

			}while(1);

			return 0;
		}

		BVP2PCLIENTLIB_API int keepAliveTCP(int nSecond)
		{
			if(nSecond <= 0)
				nSecond = 10;

			HANDLE		hHandle;
			unsigned	tid;

			hHandle = (HANDLE)_beginthreadex(0, 0, TCPClient_KeepAlive_Proc, (void *)nSecond, 0, &tid);
			if(hHandle == INVALID_HANDLE_VALUE)
				return -1;

			return 0;
		}

		BVP2PCLIENTLIB_API int proxyReqTCP(int nIndex)
		{
			if(g_pTcpClient == NULL || nIndex < 0)
				return -1;

			{
				//RWLockerEx	lock(g_pTcpClient->_rwLock, true);

				if(nIndex > (int)g_pTcpClient->_mapDevs.size())
					nIndex = g_pTcpClient->_mapDevs.size();

				int i = 0;
				map_devs_t::iterator itr;
				for(itr = g_pTcpClient->_mapDevs.begin();
					itr != g_pTcpClient->_mapDevs.end();
					++itr, ++i)
				{
					if(i == nIndex)
						break;
				}

				TCPDevInfo	*pDev = itr->second;
				std::string	strData = "lalala";
				if(g_pTcpClient->proxyRequest(*pDev, strData) < 0)
					return -1;
			}

			return 0;
		}
	}
}