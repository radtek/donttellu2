
#ifndef	_INC_SERVICEMGR_H_201412081514_
#define	_INC_SERVICEMGR_H_201412081514_

#include "IServiceMgr.h"

#include <map>

namespace	BLUEV	{ namespace BLUEVNET {

	using	std::map;
	using	BLUEVNET::IService;
	using	BLUEVNET::IServiceMgr;

	class	BVNETLIB_API	ServiceMgr : public IServiceMgr
	{
	public:
		virtual	bool		registerService(int opClass, IService *pService);
		virtual	IService *	lookup(int opClass);
		virtual	bool		remove(int opClass);
		ServiceMgr();
		~ServiceMgr();
	private:
		typedef	map<int, IService *>	services_t;
		services_t	services;
		CRITICAL_SECTION	stCrit;
	};

	class	BVNETLIB_API	UDPServiceMgr : public IUDPServiceMgr
	{
	public:
		virtual	bool		registerService(int opClass, IUDPService *pService);
		virtual	IUDPService *lookup(int opClass);
		virtual	bool		remove(int opClass);
		UDPServiceMgr();
		~UDPServiceMgr();
	private:
		typedef	map<int, IUDPService *>	services_t;
		services_t	services;
		CRITICAL_SECTION	stCrit;
	};

}}

#endif	// _INC_SERVICEMGR_H_201412081514_

