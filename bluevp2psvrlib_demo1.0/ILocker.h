
#pragma once

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
	}
}