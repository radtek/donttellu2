#pragma once

namespace BLUEV{
	namespace BLUEVNET{

		using BLUEVBASE::LockerEx;
		using BLUEVBASE::MemPool;
		using BLUEVBASE::DataQue;

		#define MAX_CLIENTS	1024

		class Mem
		{
			//friend class MemPool;

		private:
			bool		bUsed;

		public:
			Mem()
			{
				bUsed = true;
			}

			virtual ~Mem()
			{
				bUsed = false;
			}

			bool isUsed()
			{
				return bUsed;
			}
		};

		class UDPContext : public Mem
		{
		public:
			UDPConnection			conn;
			UDPServiceParamContext	paramCtx;
			
			UDPContext()
			{
				
			}

			virtual ~UDPContext()
			{

			}
		};

		template<typename T>
		class ReqAnsQueue : public DataQue<T>
		{
		public:
			ReqAnsQueue(int nMaxNum = MAX_CLIENTS)
			{
				if(nMaxNum <= 0)
					nMaxNum = MAX_CLIENTS;

				this->nTotalItemCnt = nMaxNum;
			}

			~ReqAnsQueue()
			{
			}

			int push_back(T *pData)
			{
				return addData(pData);
			}

			int pop_front(T * &pData)
			{
				return getData(pData);
			}
		};

		/*template<typename T>
		class ReqAnsQueue
		{
		private:
		std::deque<T *>		_deqData;

		CRITICAL_SECTION	_stCrit;
		public:
		ReqAnsQueue()
		{
		InitializeCriticalSection(&_stCrit);
		}

		~ReqAnsQueue()
		{
		DeleteCriticalSection(&_stCrit);
		}

		int push_back(T *pData)
		{
		LockerEx lock(_stCrit);

		if(pData == NULL)
		return -1;

		_deqData.push_back(pData);

		return 0;
		}

		int pop_front(T * &pData)
		{
		LockerEx lock(_stCrit);

		pData = NULL;

		if(_deqData.empty())
		return -1;

		pData = _deqData.front();
		_deqData.pop_front();

		return 0;
		}
		};*/

		class UDPServerEx : public UDPServer
		{
		public:
			friend	unsigned WINAPI UDPServerEx_ThreadRecv_Proc(void *pParam);
			friend	unsigned WINAPI UDPServerEx_ThreadSend_Proc(void *pParam);
			friend	unsigned WINAPI UDPServerEx_ThreadWork_Proc(void *pParam);

			//请求消息队列
			ReqAnsQueue<UDPContext>		_queReq;
			//应答消息队列
			ReqAnsQueue<UDPContext>		_queAns;

			//内存池
			MemPool<UDPContext>			_memPool;

		public:
			UDPServerEx(int port,
				IUDPServiceMgr &svrMgr,
				IServiceStatus &svcStatus,
				ILogger *pLogger,
				UDPServerParam &param
				) : UDPServer(port, svrMgr, svcStatus, pLogger, param)
			{

			}

			//int		open();

			int		start();

			//int		close();
		};

	}
}