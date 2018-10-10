
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
		int		socketLimit;
		int		nServerRecvTimeOut;	//second

		TCPServerParam()
		{
			preForkService = false;
			threadLimit = 100;
			socketLimit = FD_SETSIZE;
			nServerRecvTimeOut = 1;
		}
	};

	class	BVNETLIB_API	TCPServerContext
	{
	public:
		IServiceStatus		&svcStatus;
		IServiceMgr			&svcMgr;
		ILogger				*logger;
		void				*pCounter;

		const TCPServerParam	*param;

		SOCKET				nSocket;
		__int64				llClientNo;
		
		struct in_addr		sin_addr;
		u_short				sin_port;

		//int					nIPAddr;
		//int					nPort;

		TCPServerContext(
			IServiceStatus	&st,
			IServiceMgr		&mgr
			) : 
			svcStatus(st),
			svcMgr(mgr)
		{
			logger = NULL;
			param = NULL;
			pCounter = NULL;
			nSocket = INVALID_SOCKET;
			llClientNo = 0;
			sin_addr.S_un.S_addr = 0;
			sin_port = 0;
		}

	};

	using BLUEV::BLUEVBASE::DataQue;
	using BLUEV::BLUEVBASE::RWLocker;
	using BLUEV::BLUEVBASE::LockerEx;
	using BLUEV::BLUEVBASE::BluevError;

	//简单线程-连接1对1模型（+线程池）
	class	BVNETLIB_API	TCPServer
	{
	public:
		int				_nPort;
		SOCKET			_svrSocket;
		IServiceMgr		&_svrMgr;
		IServiceStatus	&_svcStatus;
		ILogger			*_logger;
		TCPServerParam	&_param;

		DataQue<TCPServerContext>	_queContext;

	public:
		TCPServer(
			int				port,
			IServiceMgr		&svrMgr,
			IServiceStatus	&svcStatus,
			ILogger			*logger,
			TCPServerParam	&param
			) : 
			_nPort(port),
			_svrMgr(svrMgr),
			_svcStatus(svcStatus),
			_logger(logger),
			_param(param)
		{
			_svrSocket = INVALID_SOCKET;
		}
		
		virtual ~TCPServer(){};

		virtual int		open();

		virtual int		start();
				
		virtual int		close();
		
	};

	class	BVNETLIB_API	ConnectionCounter
	{
	public:
		__int64	_llTotalClient;	/* time of requests */
		int		_nCurThreadNum;	/* total threads */	

	public:
		ConnectionCounter()
		{
			_llTotalClient = 0;
			_nCurThreadNum = 0;
		}

		~ConnectionCounter()
		{
		}

		void	clientComing()
		{
			InterlockedIncrement((unsigned __int64 *)&_llTotalClient);
		}

		void	threadStart()
		{
			InterlockedIncrement((unsigned *)&_nCurThreadNum);
		}

		void	threadTerminate() throw(...)
		{
			if(0 > InterlockedDecrement((unsigned *)&_nCurThreadNum))
				throw BluevError(__FILE__, __LINE__);
		}

	};

	class	BVNETLIB_API	TCPServerThread : public IRunnable
	{
	protected:
		IServiceStatus			&_svcStatus;
		SOCKET					_nSocket;
		ILogger					*_pLogger;
		ConnectionCounter		_stCounter;
		const TCPServerParam	&_stParam;
		IServiceMgr				&_svcMgr;
		DataQue<TCPServerContext>	&_queContext;
		
	public:
		TCPServerThread(
			SOCKET						socket,
			IServiceStatus				&svcStatus,
			ILogger						*plogger,
			const TCPServerParam		&param,
			IServiceMgr					&svcMgr,
			DataQue<TCPServerContext>	&queContext
			) : 
			_svcStatus(svcStatus),
			_nSocket(socket),
			_pLogger(plogger),
			_stParam(param),
			_svcMgr(svcMgr),
			_queContext(queContext)
		{

		}

		~TCPServerThread()
		{

		}

		int run();
	};

}}


#endif	// _INC_TCPSERVER_H_201412221652_

