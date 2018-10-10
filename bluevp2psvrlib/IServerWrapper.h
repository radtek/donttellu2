#ifndef _INC_ISERVERWRAPPER_H_201504131111_
#define _INC_ISERVERWRAPPER_H_201504131111_

namespace BLUEV{
	namespace BVP2PSVRLIB{

		using BLUEV::BLUEVBASE::ServiceStatus;
		using BLUEVNET::ILogger;

		class BVP2PSVRLIB_API IServerWrapper
		{
		protected:
			ServiceStatus	&_locSvcStatus;
			ILogger			*_logger;

		public:
			IServerWrapper(ServiceStatus &locSvcStatus,
				ILogger *logger
				) :
			_locSvcStatus(locSvcStatus),
			_logger(logger)
			{

			}

			virtual ~IServerWrapper()
			{

			}

			virtual int run() = 0;
		};



}}

#endif	//_INC_ISERVERWRAPPER_H_201504131111_