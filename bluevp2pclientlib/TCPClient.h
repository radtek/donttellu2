#pragma once


namespace BLUEV{
	namespace BVP2PCLIENTLIB{


		using BLUEVNET::BLUEV_NetReqHead;
		using BLUEVNET::BLUEV_NetAnsHead;
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::UTIL_GUID;
		using BLUEVBASE::UTIL_NET;
		using BLUEVBASE::AdapterInfo;
		using BVP2PSVRLIB::JsonBuilder;
		using BVP2PSVRLIB::ServiceHelper;
		using BVP2PSVRLIB::TCPUserLogin;
		using BVP2PSVRLIB::TCPJsonBuilder;

		using BLUEVBASE::RWLocker;
		using BLUEVBASE::RWLockerEx;
		using BLUEVBASE::LockerEx;

		class	TCPInfo
		{
		public:
			std::string	_strSessionId;
			
			std::string	_strIP;		//默认指NAT
			int			_nPort;
			SOCKET		_nSocket;
			bool		_bConnected;

		public:
			TCPInfo()
			{
				_bConnected = false;
				_nPort = 0;
				_nSocket = INVALID_SOCKET;
			}

			~TCPInfo()
			{
			}

			
		};

		class	TCPTaskInfo : public TCPInfo
		{
		public:
			TCPTaskInfo()
			{

			}
		};

		class	TCPMainCtrlInfo : public TCPInfo
		{
		public:
			TCPMainCtrlInfo()
			{

			}
		};

		class	TCPDevInfo : public TCPInfo
		{
		public:
			std::string	_strLANIP;
			int			_nLANPort;
			bool		_bSameLan;
		
			bool		_bProxy;

			std::string	_strUPNPIP;
			int			_nUPNPPort;
			bool		_bUPNP;
		public:
			TCPDevInfo()
			{
				_nLANPort = 0;
				_bSameLan = false;
				_bProxy = false;

				_nUPNPPort = 0;
				_bUPNP = false;
			}

			~TCPDevInfo()
			{
			}
		};

		typedef std::map<std::string, TCPDevInfo *> map_devs_t;
		class BVP2PCLIENTLIB_API TCPClient
		{
		public:
			std::string	_strSessionId;	//本机的SESSIONID
			int			_nPort;			
			bool		_bMainCtrl;		
			volatile bool		_bPrepared;
			
			map_devs_t	_mapDevs;		//主控端时，这里存多个设备
			RWLocker	_rwLock;		//for _mapDevs
			
			//服务端地址。
			std::string _strSvrAddr;
			int			_nSvrPort;

			//与服务端的连接
			CRITICAL_SECTION	_stCrit;	//for _nSocket
			SOCKET				_nSocket;
			struct				sockaddr_in	_sinAddr;

			TCPClient()
			{
				_nPort = 0;
				_bMainCtrl = false;
				_bPrepared = false;
				_nSvrPort = 0;
				_nSocket = INVALID_SOCKET;
				memset(&_sinAddr, 0, sizeof(_sinAddr));

				InitializeCriticalSection(&_stCrit);
			}

			~TCPClient()
			{
				DeleteCriticalSection(&_stCrit);
			}

			int open()
			{
				_nSocket = SocketHelper::BLUEV_SOCK_OpenSocket();
				if(_nSocket == INVALID_SOCKET)
					return -1;

				//struct sockaddr_in  serveraddr;
				//SocketHelper::BLUEV_SOCK_SetAddrN(&serveraddr, INADDR_ANY, _nPort);
				// 
				//SocketHelper::BLUEV_SOCK_ReuseSocket(_nSocket);

				//// Bind the address to this socket to make it 'known' 
				//if(bind(_nSocket, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)) != 0)
				//{
				//	closesocket(_nSocket);
				//	return -1;
				//}
	
				return 0;
			}

			int start();

			int close()
			{
				closesocket(_nSocket);

				return 0;
			}

			int	login()
			{
				LockerEx	lock(_stCrit);

				BLUEV_NetReqHead	req;

				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_P2P, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCP_LOGIN, req.nOpCode);

				if(_bMainCtrl)
					req.reuseConnection = 1;

				req.longConnection = 1;

				std::string	strLanIp = "127.0.0.1";
				int			nLanPort = 7689;
				std::string	strMask = "255.255.0.0";
				std::string	strGateWay = "255.255.0.0";
				std::string	strMAC;
				std::string	strDevId;

				{
					char	szBuf[100] = {0};
					//sprintf(szBuf, "threadid:%d_curtime:%d", GetCurrentThreadId(), GetCurrentTime());
					sprintf(szBuf, "threadid:%d", GetCurrentThreadId());
					strDevId = szBuf;
				}

				std::vector<AdapterInfo>	vctNet;
				UTIL_NET::GetMacAddress(vctNet);
				for(int i = 0; i < (int)vctNet.size(); i++)
				{
					if(vctNet[i].strIP == "0.0.0.0")
						continue;

					if(vctNet[i].strGateWayIP.empty() || 
						vctNet[i].strGateWayIP == "0.0.0.0")
						continue;

					strMAC = vctNet[i].strMac;
					strLanIp = vctNet[i].strIP;
					strMask = vctNet[i].strMask;
					strGateWay = vctNet[i].strGateWayIP;
				}

				TCPUserLogin	user;
				user.strLanIp = strLanIp;
				user.nLanPort = nLanPort;
				user.strMask = strMask;
				user.strGateWay = strGateWay;
				user.bMainCtrl = _bMainCtrl;
				user.strId = _bMainCtrl ? strMAC : strDevId;

				if(_bMainCtrl)
				{
					user.strUserName = "admin1";
					user.strPassWord = "admin1";
				}
				else
				{
					user.strId = "deviceid1";

					//user.bUPNP = 0;
					user.bUPNP = 1;
					user.strUPNPIP = user.strLanIp;
					user.nUPNPPort = user.nLanPort;	//另起一个server
				}

				std::string	strJson;
				if(0 > TCPJsonBuilder::userData2Json(user, strJson))
					return -1;

				std::cout << "req login json : " << strJson << std::endl;
				
				UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);
				
				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, &req, sizeof(req)))
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, (void *)strJson.c_str(), strJson.size()))
					return -1;

				BLUEV_NetAnsHead	ans;
				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, &ans, sizeof(ans)))
					return -1;

				if(!ans.isValid())
					return -1;

				int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
				if(nRetVal < 0)
					return nRetVal;

				int nDataLen = UTIL_CONV::Char4To_uint4(ans.nDataLen);
				if(nDataLen <= 0 || nDataLen >= 10*1024*1024)	//10M
					return -1;

				char	*pData = new char[nDataLen];
				if(pData == NULL)
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, pData, nDataLen))
				{
					delete[] pData;
					return -1;
				}

				strJson.assign(pData, nDataLen);
				if(strJson.empty())
				{
					delete[] pData;
					return -1;
				}

				std::cout << "receive tcp login json:" << strJson << std::endl;

				Json::Value		root;
				Json::Reader	reader;

				if(false == reader.parse(strJson, root))
					return -1;

				Json::Value	&sessionid = root[JSON_SESSIONID];
				if(sessionid.type() != Json::stringValue)
					return -1;

				_strSessionId = sessionid.asCString();
				if(_strSessionId.empty())
					return -1;

				std::cout << "登陆成功，session id:" << _strSessionId << std::endl;

				return 0;
			}

			int	logout()
			{
				LockerEx	lock(_stCrit);

				BLUEV_NetReqHead	req;

				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_P2P, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCP_LOGOUT, req.nOpCode);

				std::string	strJson;
				if(0 > TCPJsonBuilder::session2Json(_strSessionId, strJson))
					return -1;

				std::cout << "req logout json : " << strJson << std::endl;

				UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, &req, sizeof(req)))
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, (void *)strJson.c_str(), strJson.size()))
					return -1;

				BLUEV_NetAnsHead	ans;
				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, &ans, sizeof(ans)))
					return -1;

				if(!ans.isValid())
					return -1;

				int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
				if(nRetVal < 0)
					return nRetVal;

				//登出成功
				_strSessionId.clear();

				std::cout << "登出成功" <<std::endl;

				return 0;
			}

			int	getDevInfo();

			int	connDevWithSvr(const TCPDevInfo &dev)
			{
				return 0;
				//TCP 不支持P2P，改为UPNP实现
#if 0
				BLUEV_NetReqHead	req;

				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_CONNDEV, req.nOpCode);

				Json::Value	root, session, devinfo;
				Json::FastWriter	writer;

				if(0 > TCPJsonBuilder::session2Json(_strSessionId, session))
					return -1;

				if(0 > TCPJsonBuilder::devData2Json(dev._strSessionId, dev._strDevId, dev._bSameLan, devinfo))
					return -1;

				root[JSON_DEVINFO] = devinfo;
				root[JSON_SESSION] = session;

				std::string	strJson = writer.write(root);
				std::cout << "req connDev json : " << strJson << std::endl;

				UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, &req, sizeof(req)))
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, (void *)strJson.c_str(), strJson.size()))
					return -1;

				BLUEV_NetAnsHead	ans;
				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, &ans, sizeof(ans)))
					return -1;

				if(!ans.isValid())
					return -1;
				
				int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
				if(nRetVal < 0)
					return nRetVal;
#endif
				return 0;
			}

			int	connDevDirect(TCPDevInfo &dev)
			{
				std::string		strIp;
				int				nPort;

				//同一内网直连
				if(dev._bSameLan)
				{
					strIp = dev._strLANIP;
					nPort = dev._nLANPort;
				}
				else if(dev._bUPNP)
				{
					strIp = dev._strUPNPIP;
					nPort = dev._nUPNPPort;
				}
				else
				{
					return -1;
				}

				//连接
				struct sockaddr_in	devSin = {0};  
				int			nAddrLen = 0;
				SOCKET		devSocket;

				devSocket = SocketHelper::BLUEV_SOCK_OpenSocket();
				if(devSocket == INVALID_SOCKET)
					return -1;

				nAddrLen = sizeof(devSin);

				if(0 > ServiceHelper::rebuildConn(strIp, nPort, devSin, nAddrLen))
					return -1;

				//循环5次
				{
					//阻塞
					int nRet = connect(devSocket, (struct sockaddr *)&devSin, nAddrLen);
					if(nRet < 0)
						return nRet;

					dev._nSocket = devSocket;
					dev._bConnected = true;
				}

#if 0
				//
				BLUEV_NetReqHead	req;

				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENT, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_CONNDEV, req.nOpCode);

				Json::Value	root;
				Json::FastWriter	writer;
				root[JSON_SESSIONID] = Json::Value(_strSessionId);
			
				std::string	strJson = writer.write(root);
				if(strJson.empty())
					return -1;

				UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);

				std::string	strIp;
				int			nPort = 0;

				//同一内网直连
				if(dev._bSameLan)
				{
					strIp = dev._strLANIP;
					nPort = dev._nLANPort;
				}
				else if(dev._bUPNP)
				{
					strIp = dev._strUPNPIP;
					nPort = dev._nUPNPPort;
				}
				else
				{
					strIp = dev.getIP();
					nPort = dev.getPort();
				}

				//连接
				struct sockaddr_in	devSin = {0};  
				int			nAddrLen = 0;
				SOCKET		devSocket;
				
				devSocket = SocketHelper::BLUEV_SOCK_OpenSocket();
				if(devSocket == INVALID_SOCKET)
					return -1;

				nAddrLen = sizeof(devSin);

				if(0 > ServiceHelper::rebuildConn(strIp, nPort, devSin, nAddrLen))
					return -1;

				//循环5次
				{
					//阻塞
					int nRet = connect(devSocket, (struct sockaddr *)&devSin, nAddrLen);
					if(nRet < 0)
						return nRet;

					dev.setConnected(true);
				}

				if(0 > SocketHelper::BLUEV_SOCK_Send(devSocket, &req, sizeof(req)))
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Send(devSocket, (void *)strJson.c_str(), strJson.size()))
					return -1;

				BLUEV_NetAnsHead	ans;
				if(0 > SocketHelper::BLUEV_SOCK_Recv(devSocket, &ans, sizeof(ans)))
					return -1;

				if(!ans.isValid())
					return -1;

				int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
				if(nRetVal < 0)
					return nRetVal;

				char	szBuf[256] = {0};
				memset(szBuf, 0, sizeof(szBuf));
				if(dev._bSameLan)
					sprintf(szBuf, "同一内网，直接连接设备:%s:%d\r\n", strIp.c_str(), nPort);
				else
				{
					if(nPort != dev.getPort())
						sprintf(szBuf, "停止主动连接设备:%s:%d，因为发现端口已更新为:%d\r\n", strIp.c_str(), nPort, dev.getPort());
					else
						sprintf(szBuf, "请求协助后主动连接设备:%s:%d\r\n", strIp.c_str(), nPort);
				}

				std::cout << szBuf;
#endif
				return 0;
			}

			int	pulse(SOCKET nSocket)
			{
				if(nSocket == INVALID_SOCKET)
					return -1;

				BLUEV_NetReqHead	req;

				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_P2P, req.nOpClass);
				//UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_P2PCLIENT, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCP_KEEPALIVE, req.nOpCode);

				std::string	strJson;

				{
					Json::Value	root;
					Json::FastWriter	writer;

					root[JSON_SESSIONID] = Json::Value(_strSessionId);

					strJson = writer.write(root);
					if(strJson.empty())
						return -1;
		
				}

				UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);

				if(0 > SocketHelper::BLUEV_SOCK_Send(nSocket, &req, sizeof(req)))
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Send(nSocket, (void *)strJson.c_str(), strJson.size()))
					return -1;

				BLUEV_NetAnsHead	ans;
				if(0 > SocketHelper::BLUEV_SOCK_Recv(nSocket, &ans, sizeof(ans)))
					return -1;

				if(!ans.isValid())
					return -1;

				int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
				if(nRetVal < 0)
					return nRetVal;

				std::cout << "发送心跳成功\r\n";

				return 0;
			}

			int proxyRequest(const TCPDevInfo &dev, const std::string &strData)
			{
				if(strData.empty())
					return -1;

				LockerEx	lock(_stCrit);
				BLUEV_NetReqHead	req;

				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_P2P, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCP_PROXY, req.nOpCode);

				
				//构造json
				Json::Value	root, session, devinfo;
				Json::FastWriter	writer;

				if(0 > TCPJsonBuilder::session2Json(_strSessionId, session))
					return -1;

				if(0 > TCPJsonBuilder::devData2Json(dev._strSessionId, devinfo))
					return -1;

				root[JSON_DEVINFO] = devinfo;
				root[JSON_SESSION] = session;

				//构造代理数据
				if(0 > TCPJsonBuilder::proxyBinary2json(strData.c_str(), strData.size(), root))
					return -1;

				std::string	strJson = writer.write(root);
				
				std::cout << "req proxy json : " << strJson << std::endl;

				UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, &req, sizeof(req)))
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, (void *)strJson.c_str(), strJson.size()))
					return -1;

				BLUEV_NetAnsHead	ans;
				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, &ans, sizeof(ans)))
					return -1;

				if(!ans.isValid())
					return -1;

				int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
				if(nRetVal < 0)
					return nRetVal;
				
				int nDataLen = UTIL_CONV::Char4To_uint4(ans.nDataLen);
				if(nDataLen <= 0 || nDataLen >= 10*1024*1024)	//10M
					return -1;

				char	*pData = new char[nDataLen];
				if(pData == NULL)
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, pData, nDataLen))
				{
					delete[] pData;
					return -1;
				}

				strJson.assign(pData, nDataLen);
				if(strJson.empty())
				{
					delete[] pData;
					return -1;
				}

				std::cout << "ans proxy json : " << strJson << std::endl;

				return 0;
			}

			int chatMirror(std::string &strSend)
			{
				if(strSend.empty())
					return -1;

				LockerEx	lock(_stCrit);
				BLUEV_NetReqHead	req;

				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_CHAT, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CHAT_MIRROR, req.nOpCode);
				//req.reuseConnection = 1;
				req.longConnection = 1;

				std::cout << "chat send:" << strSend << std::endl;

				UTIL_CONV::uint4ToChar4(strSend.size(), req.nDataLen);

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, &req, sizeof(req)))
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, (void *)strSend.c_str(), strSend.size()))
					return -1;

				BLUEV_NetAnsHead	ans;
				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, &ans, sizeof(ans)))
					return -1;

				if(!ans.isValid())
					return -1;

				int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
				if(nRetVal < 0)
					return nRetVal;

				int nDataLen = UTIL_CONV::Char4To_uint4(ans.nDataLen);
				if(nDataLen <= 0 || nDataLen >= 10*1024*1024)	//10M
					return -1;

				char	*pData = new char[nDataLen];
				if(pData == NULL)
					return -1;

				if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, pData, nDataLen))
				{
					delete[] pData;
					return -1;
				}

				std::string strRecv;
				strRecv.assign(pData, nDataLen);
				delete[] pData;

				std::cout << "chat recv:" << strRecv << std::endl;

				return 0;
			}

			int TCPjson2DevInfo(TCPDevInfo &devInfo, const Json::Value &dev);
		};

		
	}
}