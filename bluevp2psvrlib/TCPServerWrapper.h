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

			TCPUserCtxMgr	_ctxTcpMgr;	//用户管理器

			SYNCUserCtxMgr	_ctxSyncMgr;//同步管理器
			PSTUserCtxMgr	_ctxPstMgr;	//持久化管理器

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