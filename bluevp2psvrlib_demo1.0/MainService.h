
#ifndef	_INC_MAINSERVICE_H_201412240951_
#define	_INC_MAINSERVICE_H_201412240951_


namespace	BLUEV	{ namespace BLUEVNET {

	class	MainService
	{
	public:
		SOCKET			_nSocket;
		__int64			_llClientNo;	
		int				_nIPAddr;
		int				_nPort;
		IServiceMgr		&_svcMgr;
		TCPServerParam	&_param;
		ILogger			*_logger;

	public:
		MainService(IServiceMgr &svcMgr, TCPServerParam &pm) : _svcMgr(svcMgr), _param(pm)
		{
			_nSocket = INVALID_SOCKET;
			_llClientNo = 0;
			_nIPAddr = 0;
			_nPort = 0;
			_logger = 0;
		}

		int	doService();

	};

}}


#endif	// _INC_MAINSERVICE_H_201412240951_

