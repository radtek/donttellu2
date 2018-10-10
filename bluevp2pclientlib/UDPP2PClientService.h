#ifndef _INC_UDPP2PCLIENTSERVICE_H_201412121632_
#define _INC_UDPP2PCLIENTSERVICE_H_201412121632_

namespace BLUEV
{
	namespace BVP2PCLIENTLIB
	{

		using BLUEVNET::IUDPService;
		using BLUEVNET::UDPConnection;
		using BLUEVNET::UDPServiceParamContext;

		class UDPP2PClient;
		class	UDPP2PClientService : public IUDPService
		{
		private:
			UDPConnection			conn;
			UDPServiceParamContext	ctx;
			UDPP2PClient			*pClient;

		public:
			UDPP2PClientService(UDPP2PClient* client) : pClient(client)
			{

			}

			~UDPP2PClientService(){};

			//friend GAFIS_THREAD_DECLPROC GAFIS7_UDP_SendFile_Proc(void *pParam);
		public:
			int	execute(UDPConnection &conn, UDPServiceParamContext &ctx);

			/* ���ڽ��շ����Ӧ�� */
			int loginAns(UDPConnection& conn, UDPServiceParamContext& ctx);
			int logoutAns(UDPConnection& conn, UDPServiceParamContext& ctx);
			int getDevInfoAns(UDPConnection& conn, UDPServiceParamContext& ctx);
			int connDevAns(UDPConnection& conn, UDPServiceParamContext& ctx);
			int keepAliveAns(UDPConnection& conn, UDPServiceParamContext& ctx);

			/* ���ڽ��շ�������� */
			int digClient(UDPConnection& conn, UDPServiceParamContext& ctx);
			

			/* ���ڽ��տͻ������� */
			int digByDev(UDPConnection& conn, UDPServiceParamContext& ctx);
			int connByClient(UDPConnection& conn, UDPServiceParamContext& ctx, __int64 *pSessionId = NULL);
			int keepAliveByClient(UDPConnection& conn, UDPServiceParamContext& ctx);
			
			/* ���ڽ��տͻ���Ӧ�� */
			int digClientAns(UDPConnection& conn, UDPServiceParamContext& ctx);
			int connDevDirectAns(UDPConnection& conn, UDPServiceParamContext& ctx);

			/* ���ڽ��մ���ת������ */
			int reqProxy(UDPConnection& conn, UDPServiceParamContext& ctx);
			int proxyReq(UDPConnection& conn, UDPServiceParamContext& ctx);

			/* ���ڽ��մ���ת��Ӧ�� */
			int proxyAns(UDPConnection& conn, UDPServiceParamContext& ctx);

		};
	};
};


#endif	//	_INC_UDPP2PCLIENTSERVICE_H_201412121632_