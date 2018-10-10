#ifndef _INC_UDPSERVERWRAPPER_H_201504131134_
#define _INC_UDPSERVERWRAPPER_H_201504131134_

namespace BLUEV{
	namespace BVP2PSVRLIB{

		using BVP2PSVRLIB::IServerWrapper;
		using BLUEVNET::UDPServiceMgr;
		using BLUEVNET::BLUEVOpClass;
		using BLUEVNET::UDPServerParam;
		using BLUEVNET::ILogger;

		class UDPServerWrapper : public IServerWrapper
		{
		public:
			UDPServerArgs	&_udpArgs;
			UDPServer		*_pServer;

			CP2PUserCtxMgr	_ctxMgr;
			UDPServiceMgr	_udpServiceMgr;
			UDPServerParam	_udpParam;

			UDPServerWrapper(ServiceStatus &locSvcStatus,
				ILogger	*logger,
				UDPServerArgs &udpArgs) :
			IServerWrapper(locSvcStatus, logger),
				_udpArgs(udpArgs)
			{
				_pServer = NULL;
				memset(&_udpParam, 0, sizeof(_udpParam));
			}

			~UDPServerWrapper()
			{
				if(_pServer)
					delete _pServer;
			}

			int run();
		};


	}}

#endif	//_INC_UDPSERVERWRAPPER_H_201504131134_