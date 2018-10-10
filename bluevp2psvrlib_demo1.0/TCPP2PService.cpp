#include "bluevp2psvrdef.h"

namespace BLUEV{
	namespace BVP2PSVRLIB{
		
		using BLUEVNET::BLUEV_NetReqHead;
		using BLUEVNET::BLUEV_NetAnsHead;
		using BLUEVNET::BLUEVOpClass;
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::BluevError;

		int TCPP2PService::doWork(BLUEVConnection &conn, ServiceParamContext &ctx)
		{
			BLUEV_NetReqHead	&req = conn.req;
			BLUEV_NetAnsHead	&ans = conn.ans;

			int	retval = -1;
			
			int	opCode = UTIL_CONV::Char2To_uint2(req.nOpCode);

			switch(opCode)
			{
			case BLUEVOpClass::OP_P2P_LOGIN:
				retval = login(conn, ctx);
				break;
			case BLUEVOpClass::OP_P2P_LOGOUT:
				retval = logout(conn, ctx);
				break;
			case BLUEVOpClass::OP_P2P_GETDEVINFO:
				retval = getDevInfo(conn, ctx);
				break;
			case BLUEVOpClass::OP_P2P_CONNDEV:
				retval = connDevice(conn, ctx);
				break;
			case BLUEVOpClass::OP_P2P_KEEPALIVE:
				//retval = keepAlive(conn, ctx);
				break;
			case BLUEVOpClass::OP_P2P_PROXY:
				retval = proxyConn(conn, ctx);
				break;
			case BLUEVOpClass::OP_P2P_PROXYANS:
				//retval = proxyAnswer(conn, ctx);
				break;
			default:
				return -1;
			}

			return retval;
		}

		int TCPP2PService::login(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			//登陆信息
			P2PUserData			data;
			P2PUserData			queryData;
			//BLUEV_NetReqHead	&req = conn.req;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;
			
			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				if(0 > JsonParser::json2UserData(strJson, data))
					throw BluevError(__FILE__, __LINE__);

				//NAT
				data._strNATIP = (char *)ctx.bnIP;
				data._nNATPort = ctx.nPort;

				data._nSocket = conn.nSockfd;

				//是否已登陆
				/*if(!_ctxMgr.isLogin(data._strNATIP, data._nNATPort, data._strLANIP, data._nLANPort, 
					data._bMainCtrl, data._bMainCtrl ? data._strMAC : data._strDeviceId, queryData)
					)*/
				if(!_ctxMgr.isLogin(data, queryData))
				{
					if(0 > _ctxMgr.login(data))
						throw BluevError(__FILE__, __LINE__);

					//第一个用户登入。
					/*if(!_bChkDeadCreated)
					{
						//发起连接超时检测线程
						HANDLE		hHandle;
						unsigned	tid;

						hHandle = (HANDLE)_beginthreadex(0, 0, UDPP2PChkDead_Thread_Proc, this, 0, &tid);
						if(hHandle == INVALID_HANDLE_VALUE)
							return -1;

						CloseHandle(hHandle);
						_bChkDeadCreated = true;
					}*/

					queryData = data;
				}
				else	//已登陆，返回身份标志
				{
					data = queryData;
				}

				//返回json串  sessionid + guid
				{
					std::string strJsonAns;
					if(0 > JsonBuilder::session2Json(queryData._llSessionId, queryData._stGuid, strJsonAns))
						throw BluevError(__FILE__, __LINE__);

					ctx.pAns = new unsigned char[strJsonAns.size()];
					if(ctx.pAns == NULL)
						throw BluevError(__FILE__, __LINE__);

					memcpy(ctx.pAns, strJsonAns.c_str(), strJsonAns.size());
					ctx.nAnsLen = strJsonAns.size();

					std::string strLog = "tcp login ans json: ";
					strLog.append(strJsonAns);
					strLog.append("\r\n");
					std::cout << strLog;

				}
			}
			catch (BluevError &e)
			{
				std::string strLog = "tcp登入发生异常";
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");

				std::cout << strLog;
				return -1;
			}

			char	szBuf[64] = {0};
			sprintf(szBuf, "%lld", queryData._llSessionId);

			std::string strLog = "有tcp客户登入:";
			strLog.append((char *)szBuf);
			strLog.append("ip:");
			strLog.append(queryData._strNATIP);
			strLog.append(" port:");

			sprintf(szBuf, "%d", queryData._nNATPort);
			strLog.append((char *)szBuf);
			strLog.append("\r\n");

			std::cout << strLog;
			
			return 0;
		}

		int TCPP2PService::logout(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			__int64		llSessionId = 0;
			//BLUEV_NetReqHead	&req = conn.req;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				std::string strGUID;
				if(0 > JsonParser::json2Session(strJson, llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				if(0 > _ctxMgr.logout(llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				//不用返回json。只返回ans头
			}
			catch(BluevError &e)
			{
				std::string strLog = "tcp登出发生异常";
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");

				std::cout << strLog;
				return -1;
			}

			char	szBuf[64] = {0};
			sprintf(szBuf, "%lld", llSessionId);

			std::string strLog = "有tcp客户登出:";
			strLog.append((char *)szBuf);
			strLog.append("\r\n");

			std::cout << strLog;

			return 0;
		}
	
		int TCPP2PService::getDevInfo(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			__int64		llSessionId = 0;
			//BLUEV_NetReqHead	&req = conn.req;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			//用于获取登陆信息
			P2PUserData	data;

			try
			{
				std::string strGUID;
				if(0 > JsonParser::json2Session(strJson, llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				//验证是否登陆
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是主控端
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//检查IP，PORT是否与登陆信息一致

				//1。获取绑定的id	目前缺少维护此关系数据结构multimap<sessionid, dev_sessionid>, 
				//可放到业务服务器中，访问其接口从数据库获取。

				//2。通过id查询获取设备信息
				//考虑只返回可用的sessionid + devid.主控端以此通过tcp查询
				//不可返回guid，否则就可替其logout了。


				//先从简，忽略权限问题,返回最多9个未被占用，非主控端信息
				vct_users_t	vctUsers;
				P2PUserData	devData;
				if(0 > _ctxMgr.getUsers(vctUsers))
					throw BluevError(__FILE__, __LINE__);

				//返回json串  sessionid + devid + natip + natport +
				//判断是否同一网关，是则 issamelan + lanip + lanport
				//每个算100字节，最多返回9个
				{
					Json::Value			root, devs;
					Json::FastWriter	writer;
					std::string			strJsonAns;

					vct_users_t::const_iterator itr;
					for(itr = vctUsers.begin();
						itr != vctUsers.end();
						itr++)
					{
						if(devs.size() >= 9)
							break;

						if(itr->_llSessionId == llSessionId)
							continue;

						devData = *(itr);

						//判断是否同一内网
						bool bSameLan = data.isSameLan(devData);

						Json::Value		dev;
						if(0 > JsonBuilder::devData2Json(devData, bSameLan, dev))
							throw BluevError(__FILE__, __LINE__);

						devs[devs.size()] = dev;
					}

					root[JSON_DEVS] = devs;
					strJsonAns = writer.write(root);

					ctx.pAns = new unsigned char[strJsonAns.size()];
					if(ctx.pAns == NULL)
						throw BluevError(__FILE__, __LINE__);

					memcpy(ctx.pAns, strJsonAns.c_str(), strJsonAns.size());
					ctx.nAnsLen = strJsonAns.size();

					std::string strLog = "getdevinfo ans json: ";
					strLog.append(strJsonAns);
					strLog.append("\r\n");
					std::cout << strLog;
				}
			}
			catch(BluevError &e)
			{
				std::string strLog = "tcp获取设备信息发生异常";
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");

				return -1;
			}

			char	szBuf[64] = {0};
			sprintf(szBuf, "%lld", data._llSessionId);

			std::string strLog = "tcp客户:";
			strLog.append((char *)szBuf);
			strLog.append("获取设备信息 ");
			strLog.append("ip:");
			strLog.append(data._strNATIP);
			strLog.append(" port:");
			sprintf(szBuf, "%d", data._nNATPort);
			strLog.append((char *)szBuf);
			strLog.append("\r\n");

			std::cout << strLog;

			return 0;
		}
	
		int TCPP2PService::connDevice(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			__int64		llSessionId = 0, llDevSessionId = 0;
			
			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				Json::Value		root;
				Json::Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__);

				//验证主控端是否登陆
				std::string strGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				P2PUserData	data;
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是主控端
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//验证是否存在绑定关系
				//先从简

				//验证dev是否登陆
				std::string strDevId;
				bool	bSameLan = false;
				if(0 > JsonParser::json2DevInfo(root[JSON_DEVINFO], llDevSessionId, strDevId, bSameLan))
					throw BluevError(__FILE__, __LINE__);

				P2PUserData	devData;
				if(0 > _ctxMgr.query(llDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__);
				
				//验证是否是被控端
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//验证devid是否相等
				if(devData._strDeviceId != strDevId)
					throw BluevError(__FILE__, __LINE__);

				//主控端声明同一网段已连通
				bool	bSuccess = bSameLan;

				if(bSuccess)	//内网已连通
				{
					//验证是否同一内网网段
					bool	bSameLan = data.isSameLan(devData);

					if(bSameLan)
					{
						//真的则更改占用标记，返回成功
						//ctxMgr.setUsed(pDevData->_llSessionId);

						char	szBuf[64] = {0};
						sprintf(szBuf, "%lld", data._llSessionId);

						std::string strLog = "tcp客户:";
						strLog.append((char *)szBuf);
						strLog.append("natip:");
						strLog.append(data._strNATIP);
						strLog.append(" natport:");

						sprintf(szBuf, "%d", data._nNATPort);
						strLog.append((char *)szBuf);

						strLog.append("lanip:");
						strLog.append(data._strLANIP);
						strLog.append(" lanport:");

						sprintf(szBuf, "%d", data._nLANPort);
						strLog.append((char *)szBuf);

						strLog.append(" 成功连接内网设备");

						strLog.append(" natip:");
						strLog.append(devData._strNATIP);
						strLog.append(" natport:");

						sprintf(szBuf, "%d", devData._nNATPort);
						strLog.append((char *)szBuf);

						strLog.append(" lanip:");
						strLog.append(devData._strLANIP);
						strLog.append(" lanport:");

						sprintf(szBuf, "%d", devData._nLANPort);
						strLog.append((char *)szBuf);

						strLog.append("\r\n");

						std::cout << strLog;

						return 0;
					}
					else
					{
						//假的直接放回err
						return -1;
					}
				}
				
				//非内网，未联通
				{
					//验证dev是否被其他主控占用
					/*
					if(pDevData->_bUsed)
					{
						//是否是被自身占用
						//if(pDevData->xx)
							//接受协助，因为可能打洞仍未成功
						//else
						throw BluevError(__FILE__, __LINE__);
					}*/
					
					//读无锁，写需要加锁，且pDevData为const
					//更改占用标记
					//ctxMgr.setUsed(pDevData->_llSessionId);
				}
				
				char	szBuf[64] = {0};
				sprintf(szBuf, "%lld", data._llSessionId);

				std::string strLog = "tcp客户:";
				strLog.append((char *)szBuf);
				strLog.append("ip:");
				strLog.append(data._strNATIP);
				strLog.append(" port:");

				sprintf(szBuf, "%d", data._nNATPort);
				strLog.append((char *)szBuf);

				strLog.append("请求连接设备");

				sprintf(szBuf, "%lld", devData._llSessionId);
				strLog.append((char *)szBuf);

				strLog.append("ip:");
				strLog.append(devData._strNATIP);
				strLog.append(" port:");

				sprintf(szBuf, "%d", devData._nNATPort);
				strLog.append((char *)szBuf);
				strLog.append("\r\n");

				std::cout << strLog;
				
				//发送协助打洞指令
				
				//发送json串  ip + port
				{
					char		szSendBuf[1024] = {0};
					std::string	strJsonSend;
					if(0 > JsonBuilder::mainctrl2Json(data._llSessionId, data._strNATIP, data._nNATPort, strJsonSend))
						throw BluevError(__FILE__, __LINE__);

					std::string strLog;
					strLog = "tcp conn dev to dig send json: ";
					strLog.append(strJsonSend);
					strLog.append("\r\n");

					std::cout << strLog;

					//生成请求头
					BLUEV_NetReqHead	req;

					UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, req.nOpClass);
					UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_DIG, req.nOpCode);
					UTIL_CONV::uint4ToChar4(strJsonSend.size(), req.nDataLen);
					
					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__);
					
					std::cout << "开始发送协助打洞指令\r\n";

					int nSend = SocketHelper::BLUEV_SOCK_Send(devData._nSocket, &req, sizeof(req));
					if(nSend < sizeof(req))
						throw BluevError(__FILE__, __LINE__);

					nSend = SocketHelper::BLUEV_SOCK_Send(devData._nSocket, (void *)strJsonSend.c_str(), strJsonSend.size());
					if(nSend < (int)strJsonSend.size())
						throw BluevError(__FILE__, __LINE__);

					BLUEV_NetAnsHead	ans;
					int nRecv = SocketHelper::BLUEV_SOCK_Recv(devData._nSocket, &ans, sizeof(ans));
					if(nRecv < sizeof(ans))
						throw BluevError(__FILE__, __LINE__);

					if(!ans.isValid())
						throw BluevError(__FILE__, __LINE__);

					int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
					if(nRetVal < 0)
						return nRetVal;

					//客户端A一直请求，当收到客户端B的打洞消息后停止即可。
				}
				
			}
			catch(BluevError &e)
			{
				std::string strLog = "TCP客户请求连接设备出现异常";
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");
				std::cout << strLog;
				return -1;
			}

			return 0;
		}

		int TCPP2PService::proxyConn(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			__int64		llSessionId = 0, llDevSessionId = 0;
			
			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				Json::Value	root;
				Json::Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__);

				//解析session节点
				std::string strGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				//解析devinfo节点
				std::string strDevId;
				if(0 > JsonParser::json2DevInfo(root[JSON_DEVINFO], llDevSessionId, strDevId))
					throw BluevError(__FILE__, __LINE__);

				//验证主控端是否登陆
				P2PUserData	data;
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是主控端
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//验证是否存在绑定关系
				//先从简

				//验证dev是否登陆
				P2PUserData	devData;
				if(0 > _ctxMgr.query(llDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是被控端
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//验证devid是否相等
				if(devData._strDeviceId != strDevId)
					throw BluevError(__FILE__, __LINE__);

				try
				{
					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__);

					//反馈成功

					//建立代理
					{
						SOCKET	sockfd[2] = {INVALID_SOCKET};
						int		bHasData[2] = {0}, nFdCount = 0, bClosed[2] = {0};
						char	szBuf[1024] = {0};

						sockfd[0] = conn.nSockfd;
						sockfd[1] = devData._nSocket;
						nFdCount = 2;

						do
						{
							do
							{
								if(0 > SocketHelper::BLUEV_SOCK_WaitDataToRead(sockfd, nFdCount, 3, 0, bHasData))
									throw BluevError(__FILE__, __LINE__);
							}
							while(!_server._svcStatus.isWillStop() && !bHasData[0] && !bHasData[1]);

							if(_server._svcStatus.isWillStop())
								break;

							memset(bClosed, 0, sizeof(bClosed));
							for(int i = 0; i < nFdCount; i++)
							{
								if(!bHasData[i])
									continue;

								int nLen = 0;
								if(ioctlsocket(sockfd[i], FIONREAD, (u_long *)&nLen) != 0 || nLen == 0)
								{
									bClosed[i] = 1;
								}
								else
								{
									//while((nLen = GAFIS_SOCK_BufDataLen(sockfd[i])) > 0)
									while(ioctlsocket(sockfd[i], FIONREAD, (u_long *)&nLen) == 0 && nLen > 0)
									{
										if(nLen > sizeof(szBuf))
											nLen = sizeof(szBuf);

										// if one sock closed, we still receive data
										SocketHelper::BLUEV_SOCK_Recv(sockfd[i], (void *)szBuf, nLen);
										if(nFdCount > 1)
										{
											SocketHelper::BLUEV_SOCK_Send(sockfd[nFdCount - 1 - i], (void *)szBuf, nLen);

											char	szBuf[64] = {0};
											std::string strLog = "tcp代理转发从";

											sprintf(szBuf, "%lld", i == 0 ? llSessionId : llDevSessionId);
											strLog.append(szBuf);
											strLog.append("至");
											sprintf(szBuf, "%lld", i == 0 ? llDevSessionId : llSessionId);
											strLog.append(szBuf);
											strLog.append("\r\n");

											std::cout <<  strLog;
										}
									}
								}
							}

							for(int i = 0; i < nFdCount; i++)
							{
								if(bClosed[i])
								{
									// one sock closed
									if(i < nFdCount - 1)
										sockfd[i] = sockfd[i+1];
									nFdCount--;
									i--;
								}
							}
							if(nFdCount == 0)
								return 1;
						}
						while(!_server._svcStatus.isWillStop());
					}
				}
				catch(BluevError & e)
				{
					throw e;
				}

			}
			catch(BluevError &e)
			{
				char	szBuf[64] = {0};
				std::string strLog = "tcp代理转发从";

				sprintf(szBuf, "%lld", llSessionId);
				strLog.append(szBuf);
				strLog.append("至");
				sprintf(szBuf, "%lld", llDevSessionId);
				strLog.append(szBuf);
				strLog.append("发生异常");
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");

				std::cout <<  strLog;
				return -1;
			}

			return 0;
		}


	}
}