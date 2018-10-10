#include "bluevp2psvrdef.h"
#include <iostream>
#include <sstream>

namespace BLUEV{
	namespace BLUEVNET{

		using BLUEVBASE::BluevError;

		int MainService::doService()
		{
			int			retval	= -1;
			std::ostringstream	oss;

			ServiceParamContext	svcParam;
			BLUEVConnection		conn;
			BLUEV_NetReqHead	&req = conn.req;
			BLUEV_NetAnsHead	&ans = conn.ans;

			// 设置用户连接
			conn.nSockfd = _nSocket;

			try
			{
				int	nSecond = _param.nServerRecvTimeOut * 1000;	//ms
				setsockopt(_nSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nSecond, sizeof(int));

				do
				{
					//读取一个头
					if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, &req, sizeof(req)))
						throw BluevError(__FILE__, __LINE__, "recv 超时");

					// 校验用户请求
					if(!req.isValid())
						throw BluevError(__FILE__, __LINE__, "非法头部信息");

					// 然后调用不同的处理器进行处理。
					int			opClass = UTIL_CONV::Char2To_uint2(req.nOpClass);
					IService	*service = _svcMgr.lookup(opClass);
					if(service == NULL)
					{
						oss.str("");
						oss << "无效的用户请求：opClass=" << opClass;
						throw BluevError(__FILE__, __LINE__, oss.str().c_str());
					}

					int dataLen = UTIL_CONV::Char4To_uint4(req.nDataLen);
					if(dataLen < 0 || dataLen > 10*1024*1024) //10M
					{
						oss.str("");
						oss << "无效的数据长度：length=" << dataLen;
						throw BluevError(__FILE__, __LINE__, oss.str().c_str());
					}
					
					svcParam.nAnsLen = 0;
					svcParam.nReqLen = dataLen;
					svcParam.nPort = ntohs(_sin_port);
					
					char	*pIp = inet_ntoa(_sin_addr);
					memcpy(svcParam.bnIP, pIp, strlen(pIp));

					if(dataLen > 0)
					{
						// 读取剩余的数据。
						svcParam.pReq = new unsigned char[dataLen];
						if(svcParam.pReq == NULL)
							throw BluevError(__FILE__, __LINE__);

						if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, svcParam.pReq, dataLen))
							throw BluevError(__FILE__, __LINE__);
					}

					// 执行用户请求
					retval = service->execute(conn, svcParam);

					// 把结果发送给客户端。
					UTIL_CONV::uint2ToChar2(retval, ans.nRetVal);
					UTIL_CONV::uint4ToChar4(svcParam.nAnsLen, ans.nDataLen);

					// 发送执行结果
					if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, &ans, sizeof(ans)))
						throw BluevError(__FILE__, __LINE__);
					if(svcParam.pAns != NULL && svcParam.nAnsLen > 0)
					{
						if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, svcParam.pAns, svcParam.nAnsLen))
							throw BluevError(__FILE__, __LINE__);
					}

					// 释放内存
					if(svcParam.pReq)
					{
						delete[] svcParam.pReq;
						svcParam.pReq = NULL;
					}
					if(svcParam.pAns)
					{
						delete[] svcParam.pAns;
						svcParam.pAns = NULL;
					}
				
					//长连接
				} while(req.reuseConnection != 0);
			}
			catch(BluevError& e)
			{
				e.print();

				// 释放内存
				if(svcParam.pReq)
				{
					delete[] svcParam.pReq;
					svcParam.pReq = NULL;
				}
				if(svcParam.pAns)
				{
					delete[] svcParam.pAns;
					svcParam.pAns = NULL;
				}

				closesocket(_nSocket);
				return	retval;
			}

			//长连接，不关闭
			if(req.longConnection)
			{
				_bLongConn = true;
				return	retval;
			}

			// 关闭连接
			closesocket(_nSocket);
			return	retval;
		}

	}
}