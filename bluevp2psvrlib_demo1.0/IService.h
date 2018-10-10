
#ifndef	_INC_ISERVICE_H_201412081354_
#define	_INC_ISERVICE_H_201412081354_

#include "BLUEVConnection.h"

namespace	BLUEV	{ namespace BLUEVNET {

	class	BVNETLIB_API	ServiceParamContext
	{
	public:
		unsigned char	*pReq;
		int				nReqLen;
		unsigned char	*pAns;
		int				nAnsLen;
		unsigned char	bnIP[16];	// IP地址
		int				nPort;		// 客户端的端口

		ServiceParamContext()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	class	BVNETLIB_API	IService
	{
	public:
		/**
		 * 
		 * @param connection  提供给服务端实现函数使用的会话信息
		 * @param ctx   一些参数。
		 * @throws BLUEVError
		 */
		virtual	int	execute(BLUEVConnection &conn, ServiceParamContext &ctx) = 0;
		virtual ~IService(){};

	};

	class	BVNETLIB_API	UDPServiceParamContext
	{
	public:
		unsigned char	bnReqBuf[1024];
		unsigned char	bnAnsBuf[1024];
		int		nReqLen;
		int		nAnsLen;

		UDPServiceParamContext()
		{
			memset(this, 0, sizeof(*this));
		}
	};


	class	BVNETLIB_API	IUDPService
	{
	public:
		/**
		 * 
		 * @param connection  提供给服务端实现函数使用的会话信息
		 * @param ctx   一些参数。
		 * @throws BLUEVError
		 */
		virtual	int	execute(UDPConnection &conn, UDPServiceParamContext &ctx) = 0;
		virtual ~IUDPService(){};

	};
}}

#endif	// _INC_ISERVICE_H_201412081354_

