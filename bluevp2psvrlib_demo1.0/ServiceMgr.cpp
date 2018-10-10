#include "bluevp2psvrdef.h"

namespace	BLUEV	{ namespace BLUEVNET {


	bool ServiceMgr::registerService( int opClass, IService *pService )
	{
		IService	*t = services[opClass];

		if ( t != NULL ) return false;

		services[opClass] = pService;

		return	true;
	}

	IService *ServiceMgr::lookup( int opClass)
	{
		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() ) return iter->second;

		return	NULL;
	}

	bool ServiceMgr::remove( int opClass )
	{
		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() )
		{
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
	}

	bool UDPServiceMgr::registerService( int opClass, IUDPService *pService )
	{
		IUDPService	*t = services[opClass];

		if ( t != NULL ) return false;

		services[opClass] = pService;

		return	true;
	}

	IUDPService *UDPServiceMgr::lookup( int opClass)
	{
		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() ) return iter->second;

		return	NULL;
	}

	bool UDPServiceMgr::remove( int opClass )
	{
		services_t::iterator	iter = services.find(opClass);

		if ( iter != services.end() )
		{
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
	}

}}

