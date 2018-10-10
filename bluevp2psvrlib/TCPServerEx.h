
#ifndef	_INC_TCPSERVEREX_H_201503241100_
#define	_INC_TCPSERVEREX_H_201503241100_

#include <deque>

namespace	BLUEV	{ namespace BLUEVNET {

	class ILogger;
	using BLUEV::BLUEVBASE::IRunnable;

	using BLUEV::BLUEVBASE::DataQue;
	using BLUEV::BLUEVBASE::RWLocker;
	using BLUEV::BLUEVBASE::LockerEx;

	class	BVNETLIB_API	TCPServerThreadExContext
	{
	public:
		std::map<SOCKET, __int64>	_mapClientNum;

		fd_set		_fdClientSets;	/* socket set for select */
		int			_nfds;			/* max sockets for linux select */

		RWLocker	_rwLock;		

		TCPServerThreadExContext()
		{
			_nfds = 0;
			FD_ZERO(&_fdClientSets);
		}
	};

	//select + 线程池
	//单线程accept
	//单线程select（支持1024个连接）
	//线程池处理recv
	class	BVNETLIB_API	TCPServerEx : public TCPServer
	{
	public:
		TCPServerThreadExContext	_context;

	public:
		TCPServerEx(
			int				port,
			IServiceMgr		&svrMgr,
			IServiceStatus	&svcStatus,
			ILogger			*logger,
			TCPServerParam	&param
			) : 
		TCPServer(port, svrMgr, svcStatus, logger, param)
		{
			_queContext.nTotalItemCnt = FD_SETSIZE;
		}

		~TCPServerEx(){};

		//int		open();

		int		start();

		//int		close();

	};

	class	BVNETLIB_API	TCPServerThreadEx : public TCPServerThread
	{
	public:
		TCPServerThreadExContext	&_context;

		bool	_bMajor;	//true : accept 
							//false : select

		TCPServerThreadEx(
			SOCKET						socket,
			IServiceStatus				&svcStatus,
			ILogger						*plogger,
			const TCPServerParam		&param,
			IServiceMgr					&svcMgr,
			DataQue<TCPServerContext>	&queContext,
			TCPServerThreadExContext	&context
			) : 
		TCPServerThread(socket, svcStatus, plogger, param, svcMgr, queContext),
			_context(context)
		{
			_bMajor = true;
		}

		~TCPServerThreadEx()
		{

		}

		int run();

		int runMajor();

		int runMinor();
	};
}}


#endif	// _INC_TCPSERVEREX_H_201503241100_

