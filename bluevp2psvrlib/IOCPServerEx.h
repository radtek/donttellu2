#ifndef	_INC_IOCPSERVEREX_H_201504231006_
#define _INC_IOCPSERVEREX_H_201504231006_

namespace	BLUEV	{ namespace BLUEVNET {

	class ILogger;
	using BLUEV::BLUEVBASE::IRunnable;
	using BLUEV::BLUEVNET::BLUEV_NetReqHead;
	using BLUEV::BLUEVNET::BLUEV_NetAnsHead;


	using BLUEVNET::PER_IO_OPERATION_DATA;
	using BLUEVNET::PER_HANDLE_DATA;
	using BLUEVNET::IOCPContext;
	using BLUEVBASE::DataQue;

	class BVNETLIB_API IOCP_Request
	{
	public:
		BLUEV_NetReqHead	req;
		bool				bValid;

		IOCP_Request()
		{
			bValid = false;
		}

		IOCP_Request(BLUEV_NetReqHead &request) : req(request)
		{
			bValid = true;
		}
	};

	class BVNETLIB_API IOCP_TASK
	{
	public:
		typedef enum
		{	
			IOCPTASK_NULL,
			IOCPTASK_READHEAD,
			IOCPTASK_READDATA,
			IOCPTASK_WRITE,
		} enumTask;

		typedef enum
		{
			IOCPTASK_STATUS_ERR,
			IOCPTASK_STATUS_WAIT,
			IOCPTASK_STATUS_WORKING,
			IOCPTASK_STATUS_DONE
		} enumStatus;

		enumTask	_enTask;
		enumStatus	_enStatus;
		IOCPContext	*_pContext;

		IOCP_TASK()
		{
			_enStatus = IOCPTASK_STATUS_ERR;
			_enTask = IOCPTASK_NULL;
			_pContext = NULL;
		}

		int initTask(enumTask enTask, const IOCPContext &ctx)
		{
			if(IOCPTASK_STATUS_ERR != InterlockedCompareExchange((long *)&_enTask, IOCPTASK_STATUS_WAIT, IOCPTASK_STATUS_ERR))
				return -1;

			_enTask = enTask;
			_pContext = &ctx;
			return 0;
		}

		int restartTask()
		{
			if(IOCPTASK_STATUS_DONE != InterlockedCompareExchange(&_enTask, IOCPTASK_STATUS_WAIT, IOCPTASK_STATUS_DONE))
				return -1;

			return 0;
		}

		enumTask getTaskType()
		{
			return _enTask;
		}
	};
	
	typedef DataQue<IOCPContext>	que_ctx_t;
	class IOCP_TASKWorker
	{
	public:
		IOCP_TASK	&_task;
		que_ctx_t	&_queTaskRHead;
		que_ctx_t	&_queTaskRData;

		IOCP_TASKWorker(
			IOCP_TASK	&task,
			que_ctx_t	&queTaskRHead,
			que_ctx_t	&queTaskRData,
			) : 
			_task(task),
			_queTaskRHead(queTaskRHead),
			_queTaskRData(queTaskRData)
		{

		}

		int doTask()
		{
			try
			{
				if(_task._pContext == NULL)
					throw BluevError(__FILE__, __LINE__);

				if(IOCP_TASK::IOCPTASK_STATUS_WAIT != InterlockedCompareExchange(&_task._enTask, IOCP_TASK::IOCPTASK_STATUS_WORKING, IOCP_TASK::IOCPTASK_STATUS_WAIT))
					throw BluevError(__FILE__, __LINE__);

				PER_HANDLE_DATA			&stHandle = _task._pContext->_stHandle;
				PER_IO_OPERATION_DATA	&stIoOpt = _task._pContext->_stIoOpt;
				WSABUF	buffer;
				if(stHandle._nsockfd == INVALID_SOCKET)
					throw BluevError(__FILE__, __LINE__);

				switch(_task._enTask)
				{
				case IOCP_TASK::IOCPTASK_READHEAD:
					{
						if(0 > stIoOpt.getBuffer(buffer, sizeof(BLUEV_NetReqHead)))
							throw BluevError(__FILE__, __LINE__);

						stIoOpt._enumOperationType = RECV_POSTED;

						WSARecv(stHandle._nsockfd, &stIoOpt._buffer, 1,
							&stIoOpt._dwBytesRecved,
							&stIoOpt._nFlags,
							&stIoOpt._overlap,
							NULL);
						break;
					}
				case IOCP_TASK::IOCPTASK_READDATA:

					break;
				case IOCP_TASK::IOCPTASK_WRITE:

					break;
				case IOCP_TASK::IOCPTASK_NULL:
				default:
					throw BluevError(__FILE__, __LINE__);
				}
			}
			catch(BluevError &e)
			{
				e.print();

				InterlockedCompareExchange(&_enTask, IOCP_TASK::IOCPTASK_STATUS_ERR, IOCP_TASK::IOCPTASK_STATUS_WORKING)
					return -1;
			}

			return 0;
		}

		int finishTask()
		{
			if(_task._enStatus != IOCP_TASK::IOCPTASK_STATUS_WORKING)
				return -1;

			try
			{
				if(_task._pContext == NULL)
					throw BluevError(__FILE__, __LINE__);

				PER_HANDLE_DATA			&stHandle = _task._pContext->_stHandle;
				PER_IO_OPERATION_DATA	&stIoOpt = _task._pContext->_stIoOpt;
				WSABUF	buffer;
				if(stHandle._nsockfd == INVALID_SOCKET)
					throw BluevError(__FILE__, __LINE__);

				switch(_task._enTask)
				{
				case IOCP_TASK::IOCPTASK_READHEAD:
					{
						//std::ostringstream	oss;
						//BLUEV_NetReqHead	&req = *(BLUEV_NetReqHead *)stIoOpt._buffer.buf;

						//if(sizeof(req) != stIoOpt._buffer.len)
						//	throw BluevError(__FILE__, __LINE__);

						//// 校验用户请求
						//if(!req.isValid())
						//	throw BluevError(__FILE__, __LINE__, "非法头部信息");

						//放入请求头队列
						_queTaskRHead.addData(_task._pContext);

						break;
					}
				case IOCP_TASK::IOCPTASK_READDATA:
					{
						//放入队列
						_queTaskRData.addData(_task._pContext);

					}
					break;
				case IOCP_TASK::IOCPTASK_WRITE:

					break;
				case IOCP_TASK::IOCPTASK_NULL:
				default:
					throw BluevError(__FILE__, __LINE__);
				}

				if(IOCP_TASK::IOCPTASK_STATUS_WORKING != InterlockedCompareExchange(&_enTask, IOCP_TASK::IOCPTASK_STATUS_DONE, IOCP_TASK::IOCPTASK_STATUS_WORKING))
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				e.print();

				InterlockedCompareExchange(&_task._enTask, IOCP_TASK::IOCPTASK_STATUS_ERR, IOCP_TASK::IOCPTASK_STATUS_WORKING)
					return -1;
			}

			return 0;
		}
	};

	typedef std::map<SOCKET, IOCP_TASK>	map_task_t;
	class IOCP_TASKMgr
	{
	public:
		map_task_t		_mapTask;
		RWLocker		_lockRW4MapTask;

		int addTask(IOCP_TASK::enumTask enTask, const SOCKET &nSocket, const IOCPContext &ctx)
		{
			IOCP_TASK	task;
			if(0 > task.initTask(enTask, ctx))
				return -1;

			{
				RWLockerEx	lock(_lockRW4MapTask, false);
				_mapTask[nSocket] = task;
			}

			return 0;
		}

		int getTask(const SOCKET &nSocket, IOCP_TASK &task)
		{
			RWLockerEx	lock(_lockRW4MapTask, true);

			map_task_t::iterator	itr;
			itr = _mapTask.find(nSocket);
			if(itr == _mapTask.end())
				return -1;

			task = itr->second;
		}
	};

	//IOCPTASK_NULL -> IOCPTASK_READHEAD -> IOCPTASK_READDATA -> IOCPTASK_WRITE -> IOCPTASK_READHEAD
	typedef std::vector<enumTask> vct_tasktype_t;
	class IOCP_TASKConvertor
	{
	private:
		vct_tasktype_t	_vctTaskType;

	public:

		IOCP_TASKConvertor()
		{
			_vctTaskType.push_back(IOCP_TASK::IOCPTASK_NULL);
			_vctTaskType.push_back(IOCP_TASK::IOCPTASK_READHEAD);
			_vctTaskType.push_back(IOCP_TASK::IOCPTASK_READDATA);
			_vctTaskType.push_back(IOCP_TASK::IOCPTASK_WRITE);
			_vctTaskType.push_back(IOCP_TASK::IOCPTASK_READHEAD);
		}

		int convert(IOCP_TASK &task)
		{
			if(_vctTaskType.empty() || task._enStatus != IOCP_TASK::IOCPTASK_STATUS_DONE)
				return -1;

			vct_tasktype_t::const_iterator itr;
			for(itr = _vctTaskType.begin(); itr != _vctTaskType.end(); ++itr)
			{
				if(task._enTask != *itr)
					continue;

				if(++itr == _vctTaskType.end())
					return -1;

				task._enTask = *itr;
				break;
			}

			//if(IOCPTASK_STATUS_WORKING != InterlockedCompareExchange(&task._enStatus, IOCPTASK_STATUS_DONE, IOCPTASK_STATUS_WORKING))
			//	return -1;

			return 0;
		}
	};

	
	

	typedef std::map<SOCKET, IOCP_Request>	map_iocpbuffer_t;
	class BVNETLIB_API IOCPServerEx : public TCPServer
	{
	public:
		HANDLE					_hIocp;
		HANDLE					*_phIocpThread;

		IOCP_TASKMgr			_taskMgr;

		que_ctx_t				_queTaskRHead;
		que_ctx_t				_queTaskRData;

		map_iocpbuffer_t		_mapIOBuffer;
		RWLocker				_lockRW;

		//内存池
		MemPool<IOCPContext>	_memPool;

		IOCPServerEx(
			int				port,
			IServiceMgr		&svrMgr,
			IServiceStatus	&svcStatus,
			ILogger			*logger,
			TCPServerParam	&param
			) : 
		TCPServer(port, svrMgr, svcStatus, logger, param)
		{
			_svrSocket = INVALID_SOCKET;
			_hIocp = NULL;
			_phIocpThread = NULL;
		}

		~IOCPServerEx()
		{
			if(_phIocpThread)
			{
				for(int i = 0; i < _param.threadLimit; ++i)
				{
					HANDLE	&h = _phIocpThread[i];
					if(h == INVALID_HANDLE_VALUE)
						continue;

					CloseHandle(h);
				}

				delete[] _phIocpThread;
			}

		}

		int		open();

		int		start();

		int		close();
	};

	class BVNETLIB_API IOCPServerThreadEx : public IRunnable
	{
	protected:
		IServiceStatus			&_svcStatus;
		SOCKET					_nSocket;
		ILogger					*_pLogger;
		ConnectionCounter		_stCounter;
		const TCPServerParam	&_stParam;
		IServiceMgr				&_svcMgr;

		HANDLE					_hIocp;

		MemPool<IOCPContext>	&_memPool;
		IOCPServerEx			_server;
	public:
		IOCPServerThreadEx(
			SOCKET					socket,
			IServiceStatus			&svcStatus,
			ILogger					*plogger,
			const TCPServerParam	&param,
			IServiceMgr				&svcMgr,
			HANDLE					hIocp,
			MemPool<IOCPContext>	&memPool,
			IOCPServerEx			&server
			) : 
		_svcStatus(svcStatus),
			_nSocket(socket),
			_pLogger(plogger),
			_stParam(param),
			_svcMgr(svcMgr),
			_hIocp(hIocp),
			_memPool(memPool),
			_server(server)
		{
		}

		~IOCPServerThreadEx()
		{

		}

		int run();
	};

}
}

#endif	//_INC_IOCPSERVEREX_H_201504231006_