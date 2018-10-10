
#ifndef	_INC_BLUEVCONNECTION_H_201412081357_
#define	_INC_BLUEVCONNECTION_H_201412081357_

#include "bluevreqans.h"
#include <windows.h>

namespace	BLUEV	{ namespace BLUEVNET {

	class	BLUEVConnection
	{
	public:
		BLUEV_NetReqHead	req;
		BLUEV_NetAnsHead	ans;
		SOCKET			nSockfd;

		BLUEVConnection()
		{
			nSockfd = INVALID_SOCKET;
		}
	};

	class	BVNETLIB_API	UDPConnection
	{
	public:
		SOCKET				nSockfd;
		struct sockaddr_in	fromaddr;

		UDPConnection()
		{
			memset(this, 0, sizeof(*this));
			nSockfd = INVALID_SOCKET;
		}
	};
}}


#endif	// _INC_BLUEVCONNECTION_H_201412081357_

