
#ifndef	_INC_TCPSERVER_H_201412221652_
#define	_INC_TCPSERVER_H_201412221652_

#include <deque>

namespace	BLUEV	{ namespace BLUEVNET {

	class ILogger;
	using BLUEV::BLUEVBASE::IRunnable;

	class	BVNETLIB_API	TCPServerParam
	{
	public:
		bool	preForkService;
		int		threadLimit;
		int		nServerRecvTimeOut;	//second

		TCPServerParam()
		{
			preForkService = false;
			threadLimit = 100;
			nServerRecvTimeOut = 1;
		}
	};

	class	BVNETLIB_API	TCPServerContext
	{
	public:
		IServiceStatus		&svcStatus;
		IServiceMgr			&svcMgr;
		ILogger				*logger;
		TCPServerParam		*param;
		void				*pCounter;

		SOCKET				nSocket;	
		__int64				llClientNo;				
		int					nIPAddr;				
		int					nPort;					

		TCPServerContext(IServiceStatus &st, IServiceMgr &mgr) : 
		svcStatus(st), svcMgr(mgr), logger(0), param(0), pCounter(0)
		{
			nSocket = INVALID_SOCKET;
			llClientNo = 0;
			nIPAddr = 0;
			nPort = 0;
		}

	};

	using BLUEV::BLUEVBASE::DataQue;

	class	BVNETLIB_API	TCPServer
	{
	public:
		int				_nPort;
		SOCKET			_svrSocket;
		IServiceMgr		&_svrMgr;
		IServiceStatus	&_svcStatus;
		ILogger			*_logger;
		TCPServerParam	&_param;

		DataQue<TCPServerContext>		_queContext;

	public:

		TCPServer(int port,
				  IServiceMgr &svrMgr,
				  IServiceStatus &svcStatus,
				  ILogger *logger,
				  TCPServerParam &param
				 ) : _nPort(port), _svrMgr(svrMgr), _svcStatus(svcStatus), _logger(logger), _param(param)
		{
			_svrSocket = INVALID_SOCKET;
		}
		
		~TCPServer(){};

		int		open();

		int		start();
				
		int		close();
		
	};

	class	ConnectionCounter
	{
	public:
		__int64				_llTotalClient;	/* time of requests */
		int					_nCurThreadNum;	/* total threads */
		CRITICAL_SECTION	_stCrit;
	public:
		ConnectionCounter()
		{
			memset(this, 0, sizeof(*this));
			InitializeCriticalSection(&_stCrit);
		}

		~ConnectionCounter()
		{
			DeleteCriticalSection(&_stCrit);
		}

		void	clientComing()
		{
			EnterCriticalSection(&_stCrit);
			++_llTotalClient;
			LeaveCriticalSection(&_stCrit);
		}

		void	threadStart()
		{
			EnterCriticalSection(&_stCrit);
			++_nCurThreadNum;
			LeaveCriticalSection(&_stCrit);
		}

		void	threadTerminate()
		{
			EnterCriticalSection(&_stCrit);
			if(_nCurThreadNum > 0)
				--_nCurThreadNum;
			LeaveCriticalSection(&_stCrit);
		}

	};

	class	TCPServerThread : public IRunnable
	{
	private:
		IServiceStatus		&_svcStatus;
		SOCKET				_nSocket;
		ILogger				*_pLogger;
		ConnectionCounter	_stCounter;
		TCPServerParam		&_stParam;
		IServiceMgr			&_svcMgr;
		DataQue<TCPServerContext>	&_queContext;
		
		CRITICAL_SECTION	_stCrit;


	public:
		TCPServerThread(SOCKET socket,
			IServiceStatus &svcStatus,
			ILogger *plogger,
			TCPServerParam &param,
			IServiceMgr &svcMgr,
			DataQue<TCPServerContext>	&queContext
			) : 
			_svcStatus(svcStatus),
			_nSocket(socket),
			_pLogger(plogger),
			_stParam(param),
			_svcMgr(svcMgr),
			_queContext(queContext)
		{
			InitializeCriticalSection(&_stCrit);
		}

		~TCPServerThread()
		{
			DeleteCriticalSection(&_stCrit);
		}

		int run();
	};
}}


#endif	// _INC_TCPSERVER_H_201412221652_

