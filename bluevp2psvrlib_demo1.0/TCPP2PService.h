#ifndef _INC_TCPP2PSERVICE_H_201412261423_
#define _INC_TCPP2PSERVICE_H_201412261423_

namespace BLUEV{
	namespace BVP2PSVRLIB{

		class TCPP2PService : public TCPService
		{
		public:
			TCPP2PService(CP2PUserCtxMgr& ctxMgr, TCPServer &server) : TCPService(ctxMgr, server)
			{
				;
			}

			int doWork(BLUEVConnection &conn, ServiceParamContext &ctx);

			int login(BLUEVConnection& conn, ServiceParamContext& ctx);
		
			int logout(BLUEVConnection& conn, ServiceParamContext& ctx);
		
			int getDevInfo(BLUEVConnection& conn, ServiceParamContext& ctx);

			int connDevice(BLUEVConnection& conn, ServiceParamContext& ctx);

			int proxyConn(BLUEVConnection& conn, ServiceParamContext& ctx);
		};

	}
}


#endif // _INC_TCPP2PSERVICE_H_201412261423_