
#ifndef	_INC_UDPSERVER_H_201412081051_
#define	_INC_UDPSERVER_H_201412081051_

#include "IServiceMgr.h"
#include "IServiceStatus.h"

namespace	BLUEV	{ namespace BLUEVNET {

	using BLUEV::BLUEVBASE::IServiceStatus;


	struct	BVNETLIB_API	UDPServerParam
	{
		bool	preForkService;
		int		threadLimit;
	};

	class ILogger;

	class	BVNETLIB_API	UDPServer
	{
	protected:
		int					port;
		IUDPServiceMgr		&svrMgr;
		SOCKET				svrSocket;
		IServiceStatus		&svcStatus;
		ILogger				*logger;
		UDPServerParam		&param;

	public:
		bool				bPrepared;
	public:
		friend	unsigned WINAPI UDPServer_Thread_Proc(void *pParam);

	public:
		
		UDPServer(int port,
				  IUDPServiceMgr &psvrMgr,
				  IServiceStatus &psvcStatus,
				  ILogger *plogger,
				  UDPServerParam &pparam
				 ) :
					svrMgr(psvrMgr),  svcStatus(psvcStatus),  logger(plogger), param(pparam)
		{
			this->port = port;
			svrSocket = INVALID_SOCKET;
			bPrepared = false;
		}
		
		int		open();

		int		start();
				
		int		close();
		
	};

}}


#endif	// _INC_UDPSERVER_H_201412081051_

