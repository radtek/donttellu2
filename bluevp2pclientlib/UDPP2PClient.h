#ifndef _INC_UDPP2PCLIENT_H_201412121617_
#define _INC_UDPP2PCLIENT_H_201412121617_

#include "udpp2pclientservice.h"
namespace	BLUEV	{ namespace BVP2PCLIENTLIB {


	using BLUEVNET::UDPServer;
	using BLUEVNET::BLUEVOpClass;
	using BLUEVBASE::LockerEx;
	using BLUEVBASE::Locker;
	using BLUEVNET::IUDPServiceMgr;
	using BLUEV::BLUEVBASE::IServiceStatus;
	using BLUEV::BLUEVBASE::ServiceStatus;
	using BLUEVNET::ILogger;
	using BLUEVNET::UDPServerParam;
	using BLUEVNET::BLUEV_UDPReqHead;
	using BLUEVNET::SocketHelper;
	using BLUEVNET::UTIL_CONV;

	class	P2PInfo
	{
	public:
		__int64		_llSessionId;
	private:
		bool		_bConnected;
		std::string	_strIP;		//默认指NAT
		int			_nPort;

		CRITICAL_SECTION	stCrit;

	public:
		P2PInfo()
		{
			memset(this, 0, sizeof(*this));
			InitializeCriticalSection(&stCrit);
		}

		~P2PInfo()
		{
			DeleteCriticalSection(&stCrit);
		}

		void setConnected(bool bConned)
		{
			LockerEx	lock(stCrit);

			_bConnected = bConned;
		}

		bool getConnected() const
		{
			return _bConnected;
		}

		void setIP(std::string strIP)
		{
			LockerEx	lock(stCrit);

			_strIP = strIP;
		}

		std::string	getIP() const
		{
			return _strIP;
		}

		void setPort(int nPort)
		{
			LockerEx	lock(stCrit);

			_nPort = nPort;
		}

		int	getPort() const
		{
			return _nPort;
		}
	};

	class	P2PTaskInfo : public P2PInfo
	{
	public:
		P2PTaskInfo()
		{

		}
	};

	class	P2PMainCtrlInfo : public P2PInfo
	{
	public:
		P2PMainCtrlInfo()
		{

		}
	};

	class	P2PDevInfo : public P2PInfo
	{
	public:
		std::string	_strDevId;
	
	public:
		std::string	_strLANIP;
		int			_nLANPort;
		bool		_bSameLan;
		
		bool		_bProxy;
	public:
		P2PDevInfo()
		{
			_nLANPort = 0;
			_bSameLan = false;
		}

		~P2PDevInfo()
		{
		}

		/*P2PDevInfo& operator=(P2PDevInfo &dev)
		{
			_llSessionId = dev._llSessionId;
			_strDevId = dev._strDevId;
			_strNATIP = dev._strNATIP;
			_nNATPort = dev.getNATPort();
			_strLANIP = dev._strLANIP;
			_nLANPort = dev._nLANPort;
			_bSameLan = dev._bSameLan;
			_bConnected = dev._bConnected;

			return *this;
		}*/
	};

	class	UDPP2PClient : public UDPServer
	{
	//protected:
	//	int					port;
	//	IUDPServiceMgr		&svcMgr;
	//	SOCKET				clientSocket;
	//	IServiceStatus		&svcStatus;
	//	ILogger				*logger;
	//	UDPServerParam		&param;
		
	public:
		SOCKET				&clientSocket;
		__int64				sessionId;	//本机在服务器上的SESSIONID
		std::string			strGuid;
		
		bool				bMainCtrl;
		
		//主控端时，这里存多个设备
		std::map<__int64, P2PDevInfo *> mapDevs;
		
		Locker				lock;
		//服务端要求设备打洞的任务列表
		std::map<__int64, P2PTaskInfo *> mapTask;
		//建立连接的主控端列表
		//std::map<__int64, P2PClientInfo *> mapClients;


		bool		bConnSuccess;

		//p2p服务器地址。
		std::string strSvrAddr;
		int			svrPort;

		//扫描端口范围
		int			nRange;

		friend unsigned WINAPI	UDPP2PClient_Thread_Proc(void *pParam);

		UDPP2PClient(int port,
			IUDPServiceMgr &psvcMgr,
			IServiceStatus &psvcStatus,
			ILogger *plogger,
			UDPServerParam &pparam,
			bool	bMainCtrl = false
			) : UDPServer(port, psvcMgr, psvcStatus, plogger, pparam),
			clientSocket(svrSocket)
		{
			/*this->port = port;
			clientSocket = INVALID_SOCKET;*/
			bConnSuccess = false;
			sessionId = 0;
			
			bPrepared = false;

			this->bMainCtrl = bMainCtrl;

			svrPort = 0;
			nRange = 0;
		}
		
		/*int		open();

		int		start();

		int		close();*/

		
		int		pulse(std::string strIP, int nPort, bool bToSvr)
		{
			if(strIP.empty() || nPort <= 0)
				return -1;

			char				bnReqBuf[300] = {0};
			int					nAddrLen = 0;
			int					nIP = 0;
			struct sockaddr_in	sin; 

			nAddrLen = sizeof(sin);
			if(0 > SocketHelper::BLUEV_SOCK_NameToIP((char*)strIP.c_str(), &nIP) || nIP == 0)
				return -1;

			SocketHelper::BLUEV_SOCK_SetAddrN(&sin, nIP, nPort);

			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			if(bToSvr)
			{
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, pReq->nOpClass);
			}
			else
			{
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENT, pReq->nOpClass);
			}

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_KEEPALIVE, pReq->nOpCode);

			std::string	strJson;

			//if(bToSvr)
			{
				Json::Value	root;
				Json::FastWriter	writer;

				char	szBuf[50] = {0};
				sprintf(szBuf, "%lld", sessionId);
				root[JSON_SESSIONID] = Json::Value(szBuf);
				//root[JSON_GUID] = Json::Value(strGuid);

				strJson = writer.write(root);
				if(strJson.empty())
					return -1;

				if(sizeof(BLUEV_UDPReqHead) + strJson.size() >= sizeof(bnReqBuf))
					return -1;

				UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
				memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			}
			
			// 发送数据   
			if(0 > sendto(clientSocket, bnReqBuf, sizeof(BLUEV_UDPReqHead) + strJson.size(), 0, (struct sockaddr *)&sin, nAddrLen))
				return -1;  

			memset(bnReqBuf, 0, sizeof(bnReqBuf));
			sprintf(bnReqBuf, "发送心跳,dest:%s:%d\r\n", strIP.c_str(), nPort);
			std::cout << bnReqBuf;

			return 0;
		}

		int		pause()
		{
			//			svcStatus.setWillStop(true);

			Sleep(1000);
			return 0;
		}

		int		login();

		int		logout();

		int		getDevInfo();

		int		connDevWithSvr(const P2PDevInfo &dev);
		int		connDevDirect(const P2PDevInfo &dev);

		int		getServerInfo(std::string& strSvrIP, int& nSvrPort);
		int		sendToSvr(const char *pReq, const int &nReqLen);
		
		int		proxyRequest(const P2PDevInfo &dev, const std::string &strData);
	};
}}

#endif	//	_INC_UDPP2PCLIENT_H_201412121617_