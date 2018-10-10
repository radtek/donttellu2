#ifndef _INC_TCPSERVICE_H_201412261337_
#define _INC_TCPSERVICE_H_201412261337_

namespace BLUEV{
	namespace BVP2PSVRLIB{

		using BLUEVNET::IService;
		using BLUEVNET::TCPServer;
		using BLUEVNET::BLUEVConnection;
		using BLUEVNET::ServiceParamContext;

		class TCPService : public IService
		{
		protected:
			CP2PUserCtxMgr	&_ctxMgr;
			TCPServer		&_server;
		public:
			TCPService(CP2PUserCtxMgr& ctxMgr, TCPServer &server) : _ctxMgr(ctxMgr), _server(server)
			{
				;
			}

			int execute(BLUEVConnection &conn, ServiceParamContext &ctx);
			virtual int doWork(BLUEVConnection &conn, ServiceParamContext &ctx) = 0;
		};

	}
}

#endif	//_INC_TCPSERVICE_H_201412261337_