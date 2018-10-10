#ifndef _INC_UDPP2PSERVICE_H_201412081730_
#define _INC_UDPP2PSERVICE_H_201412081730_

#include "IService.h"
#include "UDPService.h"
#include "UDPServer.h"

namespace BLUEV
{
	namespace BVP2PSVRLIB
	{
		//req
#define JSON_LANIP		"lanip"
#define JSON_LANPORT	"lanport"
#define JSON_MASK		"mask"
#define JSON_GATEWAY	"gateway"
#define JSON_MAC		"mac"
#define JSON_DEVID		"devid"
#define JSON_DEVTYPE	"devtype"
#define JSON_REMARK		"remark"
#define JSON_ISMAINCTRL	"ismainctrl"
#define JSON_USERNAME	"username"
#define JSON_PASSWORD	"password"



#define JSON_SESSION	"session"
#define JSON_DEVINFO	"devinfo"
#define JSON_PROXYDATA	"proxydata"
#define JSON_STAMP		"stamp"
#define JSON_ISUPNP		"isupnp"
#define JSON_UPNPPORT	"upnpport"
#define JSON_UPNPIP		"upnpip"

		//ans
#define JSON_SESSIONID	"sessionid"
#define JSON_GUID		"guid"
#define JSON_IP			"ip"
#define	JSON_PORT		"port"
#define JSON_ISSAMELAN	"issamelan"

#define JSON_DEVS		"devs"
#define JSON_MAINCTRL	"mainctrl"
#define JSON_BROADCAST	"broadcast"

		using BLUEV::BLUEVNET::IUDPService;
		using BLUEVNET::UDPConnection;
		using BLUEVNET::UDPServiceParamContext;
		using BVP2PSVRLIB::CP2PUserCtxMgr;
		using BLUEVNET::UDPServer;
		using BLUEVNET::BLUEV_UDPReqHead;
		using BLUEVNET::BLUEV_UDPAnsHead;

		class	BVP2PSVRLIB_API	UDPP2PService : public IUDPService
		{
		private:
			CP2PUserCtxMgr	&_ctxMgr;
			UDPServer		&_server;
			bool			_bChkDeadCreated;

		public:
			__int64			_llCount;	//线程每次检查个数
		public:
			UDPP2PService(CP2PUserCtxMgr& ctxMgr, UDPServer &server) : _ctxMgr(ctxMgr), _server(server)
			{
				_bChkDeadCreated = false;
				_llCount = 1000;
			}

			~UDPP2PService(){};

			friend unsigned WINAPI	UDPP2PChkDead_Thread_Proc(void *pParam);
		public:
			virtual int	execute(UDPConnection &conn, UDPServiceParamContext &ctx) throw();

			int login(UDPConnection& conn, UDPServiceParamContext& ctx);
			int logout(UDPConnection& conn, UDPServiceParamContext& ctx);
			int getDevInfo(UDPConnection& conn, UDPServiceParamContext& ctx);
			int connDevice(UDPConnection& conn, UDPServiceParamContext& ctx);

			int keepAlive(UDPConnection& conn, UDPServiceParamContext& ctx);
			int proxyRequest(UDPConnection& conn, UDPServiceParamContext& ctx);
			int proxyAnswer(UDPConnection& conn, UDPServiceParamContext& ctx);
			int proxyRequestEx(UDPConnection& conn, UDPServiceParamContext& ctx);
			int proxyAnswerEx(UDPConnection& conn, UDPServiceParamContext& ctx);
		};

		class	BVP2PSVRLIB_API UserLogin
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

			UserLogin()
			{
				nLanPort = 0;
				bMainCtrl = false;
			}
		};

		class BVP2PSVRLIB_API JsonParser
		{
		public:
			static int json2UserData(const std::string &strJson, P2PUserData &data) throw();
			static int json2Session(const std::string &strJson, __int64 &llSessionId, std::string &strGUID) throw();
			static int json2Session(const Json::Value &session, __int64 &llSessionId, std::string &strGUID) throw();
			static int json2DevInfo(const Json::Value &devinfo, __int64 &llDevSessionId, std::string &strDevId) throw();
			static int json2DevInfo(const Json::Value &devinfo, __int64 &llDevSessionId, std::string &strDevId, bool	&bSameLan) throw();
			static int json2MainCtrl(const Json::Value &mainctrl, __int64 &llSessionId, bool &bBroadCast) throw();
			static int json2MainCtrl(const Json::Value &mainctrl, __int64 &llSessionId, std::string &strIP, int &nPort) throw();
			static int json2ProxyBinary(const Json::Value &proxydata, char * &pData, int &nDataLen) throw();

		};

		class BVP2PSVRLIB_API JsonBuilder
		{
		public:
			static int session2Json(const __int64 &llSessionId, const std::string &strGuid, std::string &strJson) throw();
			static int session2Json(const __int64 &llSessionId, const std::string &strGuid, Json::Value &session) throw();
			static int session2Json(const __int64 &llSessionId, const GUID &stGuid, std::string &strJson) throw();
			
			static int mainctrl2Json(const __int64 &llSessionId, const std::string &strIP, const int &nPort, std::string &strJson) throw();
			static int mainctrl2Json(const __int64 &llSessionId, Json::Value &mainctrl);
			
			static int devData2Json(const P2PUserData &devdata, bool bSameLan, Json::Value &dev) throw();
			static int devData2Json(const __int64 &llSessionId, const std::string &strDevId, bool bSameLan, Json::Value &dev) throw();
			static int devData2Json(const __int64 &llSessionId, const std::string &strDevId, Json::Value &dev) throw();
			static int proxyBinary2json(const char *pData, int nDataLen, Json::Value &root) throw();
			static int userData2Json(const UserLogin &user, std::string &strJson) throw();

			static int rebuildProxyReq(BLUEV_UDPReqHead &req, std::string &strJson, const Json::Value &proxydata, const __int64 &llSessionId) throw();
			static int rebuildProxyAns(BLUEV_UDPAnsHead &ans, std::string &strJson, const Json::Value &proxydata, const __int64 &llSessionId) throw();
		};
	
		class BVP2PSVRLIB_API ServiceHelper
		{
		public:
			static int rebuildConn(const std::string &strIP, const int &nPort, struct sockaddr_in &sin, int &nAddrLen) throw();
		};
	};
};


#endif	//_INC_UDPP2PSERVICE_H_201412081730_