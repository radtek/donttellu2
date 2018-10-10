#ifndef _INC_NETREQANS_H_201412081716_
#define _INC_NETREQANS_H_201412081716_

namespace BLUEV
{
	namespace BLUEVNET
	{
		class  BVNETLIB_API	BLUEVOpClass
		{
		public:
			enum tagOPCLASS
			{
				OP_CLASS_P2P = 1,	//请求
				OP_CLASS_P2PANS,	//应答
				OP_CLASS_P2PCLIENT,	//客户端之间请求
				OP_CLASS_P2PCLIENTANS,	//客户端之间应答
				OP_CLASS_P2PPROXY,	//服务器代理中转请求
				OP_CLASS_P2PPROXYANS,	//服务器代理中转应答
				OP_CLASS_USER,
				OP_CLASS_CHAT,
			};

			enum tagOPCODE
			{
				OP_P2P_LOGIN = 1,
				OP_P2P_LOGOUT = 2,
				OP_P2P_GETDEVINFO = 3,
				OP_P2P_CONNDEV = 4,
				OP_P2P_DIG = 5,
				OP_P2P_DOWNLOAD = 6,
				OP_P2P_KEEPALIVE = 7,
				OP_P2P_UPLOAD = 8,
				OP_P2P_PROXY = 9,
				OP_P2P_PROXYANS = 10,
			};

			enum tagOPTCPCLASS
			{
				OP_TCPCLASS_P2P = 1,	//请求
				OP_TCPCLASS_P2PCLIENT,	//客户端之间请求
				OP_TCPCLASS_P2PPROXY,	//服务器代理中转请求
				OP_TCPCLASS_CHAT,
			};

			enum tagTCPOPCODE
			{
				OP_TCP_LOGIN = 1,
				OP_TCP_LOGOUT = 2,
				OP_TCP_GETDEVINFO = 3,
				
				OP_TCP_KEEPALIVE = 7,
				
				OP_TCP_PROXY = 9,
				OP_TCP_PROXYANS = 10,
				OP_TCP_PROXYCONN,
				OP_CHAT_MIRROR,
			};

			enum tagERRCODE
			{
				ERR_THREADFULL = 1,
				ERR_SERVICENOTFOUND,
				ERR_MEMORY,
				ERR_EXCEPTION
			};
		};
	};
};


#endif	//_INC_NETREQANS_H_201412081716_