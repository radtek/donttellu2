
#ifndef	_INC_ISERVICEMGR_H_201412081511_
#define	_INC_ISERVICEMGR_H_201412081511_

#include "bluevp2psvrapidef.h"
#include "IService.h"


namespace	BLUEV	{ namespace BLUEVNET {

	class	BVNETLIB_API	IServiceMgr
	{
	public:
		/**
		 * 登记一个服务。
		 * @param op
		 * @param service
		 * @return
		 */
		virtual	bool	registerService(int opClass, IService *pService) = 0;
		/**
		 * 查找一个服务。
		 * @param op
		 * @return
		 */
		virtual	IService *lookup(int opClass) = 0;
		/**
		 * 删除一个服务。
		 * @param op
		 * @return
		 */
		virtual	bool	remove(int opClass) = 0;

		virtual	~IServiceMgr(){};

	};


	class	BVNETLIB_API	IUDPServiceMgr
	{
	public:
		/**
		 * 登记一个服务。
		 * @param op
		 * @param service
		 * @return
		 */
		virtual	bool	registerService(int opClass, IUDPService *pService) = 0;
		/**
		 * 查找一个服务。
		 * @param op
		 * @return
		 */
		virtual	IUDPService *lookup(int opClass) = 0;
		/**
		 * 删除一个服务。
		 * @param op
		 * @return
		 */
		virtual	bool	remove(int opClass) = 0;

		virtual	~IUDPServiceMgr(){};
	};

}}

#endif	// _INC_ISERVICEMGR_H_201412081511_

