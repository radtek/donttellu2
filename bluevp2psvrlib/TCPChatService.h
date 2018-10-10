#ifndef _INC_TCPCHATSERVICE_H_201503231046_
#define _INC_TCPCHATSERVICE_H_201503231046_

namespace BLUEV{
	namespace BVP2PSVRLIB{

		class TCPChatService : public TCPService
		{
		public:
			TCPChatService(TCPServer *pServer) : TCPService(pServer)
			{
				;
			}

			int doWork(BLUEVConnection &conn, ServiceParamContext &ctx);

			int chatMirror(BLUEVConnection& conn, ServiceParamContext& ctx);
		};

	}
}


#endif // _INC_TCPCHATSERVICE_H_201503231046_