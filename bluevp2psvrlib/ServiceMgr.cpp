#include "bluevp2psvrdef.h"

namespace	BLUEV	{ namespace BLUEVNET {

	using BLUEV::BLUEVBASE::LockerEx;

	bool ServiceMgr::registerService( int opClass, IService *pService )
	{
		LockerEx lock(stCrit);

		IService	*t = services[opClass];

		if ( t != NULL ) return false;

		services[opClass] = pService;

		return	true;
	}

	IService *ServiceMgr::lookup( int opClass)
	{
		LockerEx lock(stCrit);

		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() ) return iter->second;

		return	NULL;
	}

	bool ServiceMgr::remove( int opClass )
	{
		LockerEx lock(stCrit);

		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() )
		{
			delete iter->second;
			services.erase(iter);
			return	true;
		}
		return	false;
	}

	ServiceMgr::ServiceMgr()
	{
		InitializeCriticalSection(&stCrit);
	}

	ServiceMgr::~ServiceMgr()
	{
		DeleteCriticalSection(&stCrit);
		
		services_t::iterator iter;
		for(iter = services.begin();
			iter != services.end();
			iter++)
		{
			if(iter->second)
				delete iter->second;
		}
	}

	bool UDPServiceMgr::registerService( int opClass, IUDPService *pService )
	{
		LockerEx lock(stCrit);

		IUDPService	*t = services[opClass];

		if ( t != NULL ) return false;

		services[opClass] = pService;

		return	true;
	}

	IUDPService *UDPServiceMgr::lookup( int opClass)
	{
		LockerEx lock(stCrit);

		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() ) return iter->second;

		return	NULL;
	}

	bool UDPServiceMgr::remove( int opClass )
	{
		LockerEx lock(stCrit);

		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() )
		{
			delete iter->second;
			services.erase(iter);
			return	true;
		}
		return	false;
	}

	UDPServiceMgr::UDPServiceMgr()
	{
		InitializeCriticalSection(&stCrit);
	}

	UDPServiceMgr::~UDPServiceMgr()
	{
		DeleteCriticalSection(&stCrit);

		services_t::iterator iter;
		for(iter = services.begin();
			iter != services.end();
			iter++)
		{
			if(iter->second)
				delete iter->second;
		}

	}

}}

