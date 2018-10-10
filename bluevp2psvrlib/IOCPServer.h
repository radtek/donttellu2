#ifndef	_INC_IOCPSERVER_H_201503251354_
#define _INC_IOCPSERVER_H_201503251354_

namespace	BLUEV	{ namespace BLUEVNET {

	class ILogger;
	using BLUEV::BLUEVBASE::IRunnable;
	using BLUEV::BLUEVNET::BLUEV_NetReqHead;
	using BLUEV::BLUEVNET::BLUEV_NetAnsHead;

	using BLUEVBASE::MemPool;

#define MSGSIZE	1024
#define IOCP_MAX_CLIENTS	(1024 * 10)

	typedef enum
	{
		RECV_POSTED,
		SEND_POSTED
	} OPERATION_TYPE;

	typedef struct _PER_HANDLE_DATA
	{
		SOCKET      _nsockfd;
		sockaddr_in _addr;

		_PER_HANDLE_DATA()
		{
			memset(this, 0, sizeof(*this));
			_nsockfd = INVALID_SOCKET;
		}

	} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

	typedef struct _PER_IO_OPERATION_DATA
	{
		WSAOVERLAPPED	_overlap;
		WSABUF			_buffer;				
		char			_szMessage[MSGSIZE];	//小于MSGSIZE时使用栈空间
		char			*_pData;				//大内存使用堆空间
		int				_nDataLen;
		int				_nDataUsed;
		DWORD			_dwBytesSent;
		DWORD			_dwBytesRecved;
		DWORD			_nFlags;
		OPERATION_TYPE	_enumOperationType;

		_PER_IO_OPERATION_DATA()
		{
			memset(this, 0, sizeof(*this));
		}

		~_PER_IO_OPERATION_DATA()
		{
			freeBuffer();
		}
	private:
		void freeBuffer()
		{
			if(_pData)
			{
				delete[] _pData;
				_pData = NULL;
			}

			_nDataUsed = 0;
			_nDataLen = 0;
		}

		int allocBuffer(int nDataLen)
		{
			if(nDataLen <= 0)
				return -1;

			freeBuffer();
			_pData = new char[nDataLen];
			if(_pData == NULL)
				return -1;

			_nDataLen = nDataLen;
			_nDataUsed = 0;
			memset(_pData, 0, nDataLen);

			return 0;
		}

	public:
		int getBuffer(WSABUF &buffer, int nDataLen)
		{
			if(nDataLen < 0)
				return -1;
#ifdef IOCP_MEM_REUSE
			if(nDataLen > MSGSIZE && nDataLen > _nDataLen)
			{
				allocBuffer(nDataLen);

				_buffer.buf = _pData;
				_buffer.len = nDataLen;

				_nDataUsed = nDataLen;
			}
			else if(nDataLen > MSGSIZE && nDataLen <= _nDataLen)
			{
				_buffer.buf = _pData;
				_buffer.len = nDataLen;

				_nDataUsed = nDataLen;
			}
			else	//nDataLen <= MSGSIZE
			{
				_buffer.buf = _szMessage;
				_buffer.len = nDataLen;

				_nDataUsed = 0;
			}
#else
			if(nDataLen > MSGSIZE)
			{
				allocBuffer(nDataLen);

				_buffer.buf = _pData;
				_buffer.len = nDataLen;

				_nDataUsed = nDataLen;
			}
			else
			{
				freeBuffer();

				_buffer.buf = _szMessage;
				_buffer.len = nDataLen;

				_nDataUsed = 0;
			}
#endif

			buffer = _buffer;

			return 0;
		}

		int setBuffer(char *pData, int nDataLen)
		{
			if(pData == NULL || nDataLen <= 0)
				return -1;

			WSABUF	buffer;
			if(0 > getBuffer(buffer, nDataLen))
				return -1;

			memcpy(buffer.buf, pData, nDataLen);

			return 0;
		}

	} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

	class BVNETLIB_API IOCPContext
	{
	public:
		PER_HANDLE_DATA			_stHandle;
		PER_IO_OPERATION_DATA	_stIoOpt;

	private:
		bool		bUsed;

	public:
		IOCPContext()
		{
			bUsed = true;
		}

		~IOCPContext()
		{
			bUsed = false;
		}

		bool isUsed()
		{
			return bUsed;
		}
	};

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


	typedef std::map<SOCKET, IOCP_Request>	map_iocpbuffer_t;
	class BVNETLIB_API IOCPServer : public TCPServer
	{
	public:
		HANDLE					_hIocp;
		HANDLE					*_phIocpThread;
		
		map_iocpbuffer_t		_mapIOBuffer;
		RWLocker				_lockRW;

		//内存池
		MemPool<IOCPContext>	_memPool;

		IOCPServer(
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

		~IOCPServer()
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

	class BVNETLIB_API IOCPServerThread : public IRunnable
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
	public:
		IOCPServerThread(SOCKET		socket,
			IServiceStatus			&svcStatus,
			ILogger					*plogger,
			const TCPServerParam	&param,
			IServiceMgr				&svcMgr,
			HANDLE					hIocp,
			MemPool<IOCPContext>	&memPool
			) : 
			_svcStatus(svcStatus),
			_nSocket(socket),
			_pLogger(plogger),
			_stParam(param),
			_svcMgr(svcMgr),
			_hIocp(hIocp),
			_memPool(memPool)
		{
		}

		~IOCPServerThread()
		{

		}

		int run();
	};

}
}

#endif	//_INC_IOCPSERVER_H_201503251354_