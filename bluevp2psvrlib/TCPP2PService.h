#ifndef _INC_TCPP2PSERVICE_H_201412261423_
#define _INC_TCPP2PSERVICE_H_201412261423_

namespace BLUEV{
	namespace BVP2PSVRLIB{
		using BLUEVNET::BLUEV_NetReqHead;
		using BLUEVNET::BLUEV_NetAnsHead;

		class TCPP2PService : public TCPService
		{
		protected:
			TCPUserCtxMgr	&_ctxTcpMgr;
			PSTUserCtxMgr	&_ctxPstMgr;

			bool			_bChkDeadCreated;
		public:
			TCPP2PService(
				TCPUserCtxMgr	&ctxTcpMgr,
				PSTUserCtxMgr	&ctxPstMgr, 
				TCPServer		*pServer) 
				: TCPService(pServer), 
				_ctxTcpMgr(ctxTcpMgr), 
				_ctxPstMgr(ctxPstMgr)
			{
				_bChkDeadCreated = false;
			}

			friend unsigned WINAPI TCPChkDead_Thread_Proc(void *pParam);

			int	log(std::string &strLog, TCPUserData *pData)
			{
				if(pData == NULL)
					return -1;

				strLog.clear();

				char	szBuf[256] = {0};
				char	*pszDesp = "TCP客户 SOCKET:%d IP:%s PORT:%d\r\n";

				sprintf(szBuf, pszDesp, pData->_nSocket, pData->_strNATIP.c_str(), pData->_nNATPort);

				strLog = szBuf;

				return 0;
			}

			int doWork(BLUEVConnection &conn, ServiceParamContext &ctx);

			int login(BLUEVConnection& conn, ServiceParamContext& ctx);
		
			int logout(BLUEVConnection& conn, ServiceParamContext& ctx);
		
			int getDevInfo(BLUEVConnection& conn, ServiceParamContext& ctx);

			int keepAlive(BLUEVConnection& conn, ServiceParamContext& ctx);

			int proxyRequestEx(BLUEVConnection& conn, ServiceParamContext& ctx);

			int proxyConn(BLUEVConnection& conn, ServiceParamContext& ctx);
		};

		class	BVP2PSVRLIB_API TCPUserLogin
		{
		public:
			std::string	strLanIp;
			int			nLanPort;
			std::string	strMask;
			std::string	strGateWay;
			std::string	strId;		//MAC OR DEVID
			std::string	strUserName;
			std::string	strPassWord;

			bool		bMainCtrl;
			//被控端提供UPNP info
			bool		bUPNP;
			int			nUPNPPort;
			std::string	strUPNPIP;

			TCPUserLogin()
			{
				nLanPort = 0;
				bMainCtrl = false;
				bUPNP = false;
				nUPNPPort = 0;

			}
		};

		class BVP2PSVRLIB_API TCPJsonParser
		{
		public:
			static int json2UserData(const std::string &strJson, TCPUserData &data) throw();
			static int json2Session(const std::string &strJson, std::string &strSessionId) throw();
			static int json2Session(const Json::Value &session, std::string &strSessionId) throw();
			static int json2DevInfo(const Json::Value &devinfo, std::string &strDevSessionId) throw();
			static int json2DevInfo(const Json::Value &devinfo, std::string &strDevSessionId, bool &bSameLan) throw();
			static int json2MainCtrl(const Json::Value &mainctrl, std::string &strSessionId, bool &bBroadCast) throw();
			static int json2MainCtrl(const Json::Value &mainctrl, std::string &strSessionId, std::string &strIP, int &nPort) throw();
			static int json2ProxyBinary(const Json::Value &proxydata, char * &pData, int &nDataLen) throw();

		};

		class BVP2PSVRLIB_API TCPJsonBuilder
		{
		public:
			static int session2Json(const std::string &strSessionId, std::string &strJson) throw();
			static int session2Json(const std::string &strSessionId, Json::Value &session) throw();

			static int mainctrl2Json(const std::string &strSessionId, const std::string &strIP, const int &nPort, std::string &strJson) throw();
			static int mainctrl2Json(const std::string &strSessionId, Json::Value &mainctrl);

			static int devData2Json(const TCPUserData &devdata, bool bSameLan, Json::Value &dev) throw();
			static int devData2Json(const std::string &strDevSessionId, bool bSameLan, Json::Value &dev) throw();
			static int devData2Json(const std::string &strDevSessionId, Json::Value &dev) throw();
			static int proxyBinary2json(const char *pData, int nDataLen, Json::Value &root) throw();
			static int userData2Json(const TCPUserLogin &user, std::string &strJson) throw();

			static int rebuildProxyReq(BLUEV_NetReqHead &req, std::string &strJson, const Json::Value &proxydata, const std::string &strSessionId) throw();
			static int rebuildProxyAns(BLUEV_NetAnsHead &ans, std::string &strJson, const Json::Value &proxydata, const std::string &strSessionId) throw();
		};
	}
}


#endif // _INC_TCPP2PSERVICE_H_201412261423_