
#pragma once
#include "BluevError.h"

namespace BLUEV
{
	namespace BLUEVBASE
	{
		class BVBASELIB_API	ILocker
		{
			virtual int lock() = 0;
			virtual int unLock() = 0;
		};

		class BVBASELIB_API	Locker : public ILocker
		{
		private:
			CRITICAL_SECTION	_stCrt;
		public:
			
			Locker()
			{
				InitializeCriticalSection(&_stCrt);
			}

			~Locker()
			{
				DeleteCriticalSection(&_stCrt);
			}

			int lock()
			{
				EnterCriticalSection(&_stCrt);
				return 0;
			}

			int unLock()
			{
				LeaveCriticalSection(&_stCrt);
				return 0;
			}
		};

		class BVBASELIB_API	LockerEx
		{
		private:
			CRITICAL_SECTION	&_stCrt;
		public:
			LockerEx(CRITICAL_SECTION &stCrt) : _stCrt(stCrt)
			{
				EnterCriticalSection(&_stCrt);
			}

			~LockerEx()
			{
				LeaveCriticalSection(&_stCrt);
			}
		};

		class BVBASELIB_API IRWLocker
		{
		public:
			virtual int lockRead() = 0;
			virtual int lockWrite() = 0;

			virtual int unlockRead() = 0;
			virtual int unlockWrite() = 0;
		};

#ifndef CODE_FOR_WIN2003
		class BVBASELIB_API RWLocker : IRWLocker
		{
		private:
			SRWLOCK		_rwLock;	
			//Minimum supported client£ºWindows Vista
			//Minimum supported server£ºWindows Server 2008
			
		public:
			RWLocker()
			{
				InitializeSRWLock(&_rwLock);
			}

			~RWLocker()
			{
				//ÎÞÏú»Ùº¯Êý
			}

			virtual int lockRead()
			{
				AcquireSRWLockShared(&_rwLock);
				
				return 0;
			}

			virtual int lockWrite()
			{
				AcquireSRWLockExclusive(&_rwLock);

				return 0;
			}
			
			virtual int unlockRead()
			{
				ReleaseSRWLockShared(&_rwLock);

				return 0;
			}

			virtual int unlockWrite()
			{
				ReleaseSRWLockExclusive(&_rwLock);

				return 0;
			}
		};
#else
		using BLUEV::BLUEVBASE::BluevError;

		/* exception: BluevError */
		class BVBASELIB_API RWLocker : IRWLocker
		{
		private:
			CRITICAL_SECTION	_stCrt;
			int					_nRead;

		public:
			RWLocker()
			{
				_nRead = 0;
				InitializeCriticalSection(&_stCrt);
			}

			~RWLocker()
			{
				DeleteCriticalSection(&_stCrt);
			}

			virtual int lockRead()
			{
				EnterCriticalSection(&_stCrt);
				InterlockedIncrement((unsigned *)&_nRead);
				LeaveCriticalSection(&_stCrt);

				return 0;
			}

			virtual int lockWrite()
			{
				EnterCriticalSection(&_stCrt);

				while(_nRead)
				{
					SwitchToThread();
				}

				return 0;
			}
			
			virtual int unlockRead() throw (...)
			{
				if(0 > InterlockedDecrement((unsigned *)&_nRead))
					throw BluevError(__FILE__, __LINE__);

				return 0;
			}

			virtual int unlockWrite()
			{
				LeaveCriticalSection(&_stCrt);

				return 0;
			}
		};
#endif

		class BVBASELIB_API	RWLockerEx
		{
		private:
			RWLocker	&_rwLock;
			bool		_bRead;
		public:
			RWLockerEx(RWLocker &rwLock, bool bRead = true) : _rwLock(rwLock), _bRead(bRead)
			{
				if(bRead)
					_rwLock.lockRead();
				else
					_rwLock.lockWrite();
			}

			~RWLockerEx()
			{
				if(_bRead)
					_rwLock.unlockRead();
				else
					_rwLock.unlockWrite();
			}
		};
	}
}