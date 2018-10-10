
#ifndef	_INC_MAINSERVICE_H_201412240951_
#define	_INC_MAINSERVICE_H_201412240951_


namespace	BLUEV	{ namespace BLUEVNET {

	class	MainService
	{
	public:
		IServiceMgr		&_svcMgr;
		ILogger			*_logger;

		const TCPServerParam	&_param;

		SOCKET			_nSocket;
		__int64			_llClientNo;	
		struct in_addr	_sin_addr;
		u_short			_sin_port;
		//int				_nIPAddr;
		//int				_nPort;

		bool			_bLongConn;
	public:
		MainService(IServiceMgr &svcMgr, const TCPServerParam &pm) : 
			_svcMgr(svcMgr), _param(pm)
		{
			_nSocket = INVALID_SOCKET;
			_llClientNo = 0;
			_logger = 0;
			_sin_addr.S_un.S_addr = 0;
			_sin_port = 0;
			_bLongConn = false;
		}

		int	doService();

	};

}}


#endif	// _INC_MAINSERVICE_H_201412240951_

