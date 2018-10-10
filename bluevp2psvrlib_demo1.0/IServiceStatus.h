
#ifndef	_INC_IServiceStatus_h_201412081642
#define	_INC_IServiceStatus_h_201412081642

#include <vector>
#include <windows.h>

namespace	BLUEV	{ namespace BLUEVBASE {

	using	std::vector;


class	BVBASELIB_API	IServiceStatus
{
public:
	virtual	bool	isWillStop() = 0;
	virtual	void	setWillStop(bool willStop) = 0;
	virtual	int 	getThreadCount() = 0;
	virtual	void	incThreadCnt() = 0;
	virtual	void	decThreadCnt() = 0;
	virtual	bool	isStopped() = 0;
	virtual	void	setStopped() = 0;
	virtual	void	addOther(IServiceStatus *pOther) = 0;
	virtual	void	removeOther(IServiceStatus *pOther) = 0;
	virtual void	setRefresh(bool refresh = true) = 0;
	virtual bool	isRefresh() = 0;
	virtual	~IServiceStatus(){};
};

class	BVBASELIB_API	ServiceStatus : public IServiceStatus
{
public:
	virtual	bool	isWillStop()
	{
		return	bWillStop;
	}

	virtual	void	setWillStop(bool willStop)
	{
		this->bWillStop = willStop;
		EnterCriticalSection(&stCrit);
		vector<IServiceStatus *>::iterator iter = otherSvc.begin();
		for(; iter != otherSvc.end(); ++iter)
		{
			(*iter)->setWillStop(willStop);
		}
		LeaveCriticalSection(&stCrit);
	}

	virtual	int 	getThreadCount()
	{
		int		n;

		EnterCriticalSection(&stCrit);
		n = nThreadCnt;
		LeaveCriticalSection(&stCrit);
		return	n;
	}

	virtual	void	incThreadCnt()
	{
		EnterCriticalSection(&stCrit);
		++nThreadCnt;
		LeaveCriticalSection(&stCrit);
	}

	virtual	void	decThreadCnt()
	{
		EnterCriticalSection(&stCrit);
		--nThreadCnt;
		LeaveCriticalSection(&stCrit);
	}

	virtual	bool	isStopped()
	{
		return	bStopped;
	}

	virtual	void	setStopped()
	{
		this->bStopped = true;

		EnterCriticalSection(&stCrit);
		vector<IServiceStatus *>::iterator iter = otherSvc.begin();
		for(; iter != otherSvc.end(); ++iter)
		{
			(*iter)->setStopped();
		}
		LeaveCriticalSection(&stCrit);
	}

	virtual	void	addOther(IServiceStatus *pOther)
	{
		EnterCriticalSection(&stCrit);
		vector<IServiceStatus *>::iterator iter = otherSvc.begin();
		for(; iter != otherSvc.end(); ++iter)
		{
			if ( *iter==pOther ) break;
		}
		if ( iter==otherSvc.end() ) otherSvc.push_back(pOther);

		LeaveCriticalSection(&stCrit);
	}

	virtual	void	removeOther(IServiceStatus *pOther)
	{
		EnterCriticalSection(&stCrit);
		vector<IServiceStatus *>::iterator iter = otherSvc.begin();
		for(; iter != otherSvc.end(); ++iter)
		{
			if ( *iter!=pOther ) continue;
			otherSvc.erase(iter);
			break;
		}
		LeaveCriticalSection(&stCrit);
	}

	virtual void sleep(int seconds)
	{
		for(int i = 0; i < seconds; ++i)
		{
			if(isWillStop()) break;

			Sleep(1000);
		}
	}

	virtual void setRefresh(bool refresh)
	{
		// 此处什么都不做
	}

	virtual bool isRefresh()
	{
		return false;
	}

	ServiceStatus()
	{
		InitializeCriticalSection(&stCrit);
		bWillStop = bStopped = false;
		nThreadCnt = 0;
	}

	~ServiceStatus()
	{
		DeleteCriticalSection(&stCrit);
	}
protected:
	bool	bWillStop;
	CRITICAL_SECTION	stCrit;
	int		nThreadCnt;
	bool	bStopped;
	vector<IServiceStatus *>	otherSvc;
};

}}

#endif	// _INC_IServiceStatus_h_201012201705
