#ifndef _INC_TCPSERVERWRAPPER_H_201504131123_
#define _INC_TCPSERVERWRAPPER_H_201504131123_

namespace BLUEV{
	namespace BVP2PSVRLIB{

		using BVP2PSVRLIB::IServerWrapper;
		using BLUEVNET::TCPServerParam;
		using BLUEVNET::ServiceMgr;
		using BLUEVNET::ILogger;

		class TCPServerWrapper : public IServerWrapper
		{
		public:
			TCPServerArgs	&_tcpArgs;
			TCPServer		*_pServer;

			TCPUserCtxMgr	_ctxTcpMgr;	//�û�������

			SYNCUserCtxMgr	_ctxSyncMgr;//ͬ��������
			PSTUserCtxMgr	_ctxPstMgr;	//�־û�������

			ServiceMgr		_tcpServiceMgr;
			TCPServerParam	_tcpParam;

			TCPServerWrapper(ServiceStatus &locSvcStatus,
				ILogger	*logger,
				TCPServerArgs &tcpArgs) :
			IServerWrapper(locSvcStatus, logger),
				_tcpArgs(tcpArgs),
				_ctxPstMgr(
				_ctxSyncMgr._mapDbUsers, 
				_ctxSyncMgr._mapDbDevs,
				_ctxSyncMgr._mapDbBinds,
				_ctxSyncMgr._rwLock)
			{
				_pServer = NULL;
			}

			~TCPServerWrapper()
			{
				if(_pServer)
					delete _pServer;
			}

			bool isReady()
			{
				return _ctxSyncMgr._bLoad;
			}

			int run();
		};

	}}

#endif //_INC_TCPSERVERWRAPPER_H_201504131123_