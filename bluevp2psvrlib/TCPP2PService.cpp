#include "bluevp2psvrdef.h"
#include <sstream>
#include <process.h>

namespace BLUEV{
	namespace BVP2PSVRLIB{
		
		using BLUEVNET::BLUEV_NetReqHead;
		using BLUEVNET::BLUEV_NetAnsHead;
		using BLUEVNET::BLUEVOpClass;
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::UTIL_BASE64;
		using BLUEVBASE::BluevError;
		using namespace Json;

		extern IServiceStatus*	g_svcStatus;
		unsigned WINAPI	TCPChkDead_Thread_Proc(void *pParam)
		{
			TCPP2PService	*pService = (TCPP2PService *)pParam;
			if(pService == NULL)
				return -1;

			if(!g_svcStatus)
				return -1;

			char	*pszThreadName = "TCPCHKDEAD 线程";

			g_svcStatus->incThreadCnt();

			char	szBuf[100] = {0};
			sprintf(szBuf, "%s %d started. \r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			//__int64	llCount = pService->_llCount;
			__int64 llDelCout = 0;
			while(!g_svcStatus->isWillStop())
			{
				//llDelCout = pService->_ctxMgr.checkDeadEx(llCount);
				llDelCout = pService->_ctxTcpMgr.checkDead();
				if(llDelCout)
				{
					memset(szBuf, 0, sizeof(szBuf));
					sprintf(szBuf, "删除%lld个dead连接\r\n", llDelCout);
					std::cout << szBuf;
				}

				Sleep(10000);
			}

			memset(szBuf, 0, sizeof(szBuf));
			sprintf(szBuf, "%s %d 结束.\r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			g_svcStatus->decThreadCnt();

			return 0;
		}

		int TCPP2PService::doWork(BLUEVConnection &conn, ServiceParamContext &ctx)
		{
			BLUEV_NetReqHead	&req = conn.req;
			BLUEV_NetAnsHead	&ans = conn.ans;

			int	retval = -1;
			
			int	opCode = UTIL_CONV::Char2To_uint2(req.nOpCode);

			switch(opCode)
			{
			case BLUEVOpClass::OP_TCP_LOGIN:
				retval = login(conn, ctx);
				break;
			case BLUEVOpClass::OP_TCP_LOGOUT:
				retval = logout(conn, ctx);
				break;
			case BLUEVOpClass::OP_TCP_GETDEVINFO:
				retval = getDevInfo(conn, ctx);
				break;
			case BLUEVOpClass::OP_TCP_KEEPALIVE:
				retval = keepAlive(conn, ctx);
				break;
			case BLUEVOpClass::OP_TCP_PROXY:
				retval = proxyRequestEx(conn, ctx);
				break;
			case BLUEVOpClass::OP_TCP_PROXYANS:
				//retval = proxyAnswerEx(conn, ctx);
				break;
			case BLUEVOpClass::OP_TCP_PROXYCONN:
				//retval = proxyConn(conn, ctx);
				break;
			default:
				return -1;
			}

			return retval;
		}

		int TCPP2PService::login(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			//登陆信息
			TCPUserData			data;
			TCPUserData			queryData;
			std::ostringstream	oss;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;
			
			std::string	strJson((char* )ctx.pReq, nJsonLen);

			oss.str("");
			oss << "LOGIN REQJSON: " << strJson << std::endl;
			std::cout << oss.str();

			try
			{
				if(0 > TCPJsonParser::json2UserData(strJson, data))
					throw BluevError(__FILE__, __LINE__);

				//NAT
				data._strNATIP = (char *)ctx.bnIP;
				data._nNATPort = ctx.nPort;

				data._nSocket = conn.nSockfd;

				//验证账号
				DbUserData		userData;
				DbDeviceData	devData;

				int	nResult = 0;
				if(data._bMainCtrl)
				{
					nResult = _ctxPstMgr.checkUser(data._strUserName, data._strPassWord, userData);
					data._strSessionId = userData._strUUID;
				}
				else
				{
					nResult = _ctxPstMgr.checkDev(data._strDeviceId, devData);
					data._strSessionId = devData._strUUID;
				}

				if(nResult < 0)
				{
					char	szBuf[32] = {0};
					std::string strLog;

					log(strLog, &data);
					
					strLog.append("身份验证失败:");
					sprintf(szBuf, "%d", nResult);
					strLog.append((char *)szBuf);
					strLog.append("\r\n");

					std::cout << strLog;

					return BluevError::ERR_AUTHENTICATION;
				}

				//验证UPNP
				if(!data._bMainCtrl)
				{
					if(data._bUPNP)
					{
						//是否有效的WANIP.非10.x或192.x
						//data._strNATIP = data._strUPNPIP;
					}
				}

				//是否重复登陆
				bool	bNeedLogin = true;
				if(!data._bMainCtrl)
				{
					//被控端不能同时登陆，因此需要清除之前的
					while(_ctxTcpMgr.isLogin(data, queryData))
					{
						_ctxTcpMgr.logout(data._strSessionId, data._nSocket);
					}
				}
				else
				{	
					//主控端已登陆，返回身份标志
					if(_ctxTcpMgr.isLogin(data, queryData))
					{
						data = queryData;
						bNeedLogin = false;
					}
				}

				if(bNeedLogin)
				{
					if(0 > _ctxTcpMgr.login(data))
						throw BluevError(__FILE__, __LINE__);

					//第一个用户登入。
					if(!_bChkDeadCreated)
					{
						//发起连接超时检测线程
						HANDLE		hHandle;
						unsigned	tid;

						hHandle = (HANDLE)_beginthreadex(0, 0, TCPChkDead_Thread_Proc, this, 0, &tid);
						if(hHandle == INVALID_HANDLE_VALUE)
							return -1;

						CloseHandle(hHandle);
						_bChkDeadCreated = true;
					}

					queryData = data;
				}

				//返回json串  sessionid
				{
					std::string strJsonAns;
					if(0 > TCPJsonBuilder::session2Json(queryData._strSessionId, strJsonAns))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);

					ctx.pAns = new unsigned char[strJsonAns.size()];
					if(ctx.pAns == NULL)
						throw BluevError(__FILE__, __LINE__);

					memcpy(ctx.pAns, strJsonAns.c_str(), strJsonAns.size());
					ctx.nAnsLen = strJsonAns.size();

					oss.str("");
					oss << "LOGIN ANSJSON: " << strJsonAns << std::endl;
					std::cout << oss.str();
				}
			}
			catch (BluevError &e)
			{
				e.print();

				return -1;
			}

			std::string	strLog;
			log(strLog, &queryData);

			strLog.append("登入成功\r\n");

			std::cout << strLog;
			
			return 0;
		}

		int TCPP2PService::logout(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			std::string			strSessionId;
			std::ostringstream	oss;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char *)ctx.pReq, nJsonLen);

			oss.str("");
			oss << "LOGOUT REQJSON: " << strJson << std::endl;
			std::cout << oss.str();

			try
			{
				//获取sessionid, JSON
				if(0 > TCPJsonParser::json2Session(strJson, strSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				if(0 > _ctxTcpMgr.logout(strSessionId, conn.nSockfd))
					throw BluevError(__FILE__, __LINE__);

				//不用返回json。只返回ans头
			}
			catch(BluevError &e)
			{
				e.print();

				return -1;
			}

			oss.str("");
			oss << "TCP客户:SOCKET:" << conn.nSockfd << " SESSIONID:" << strSessionId << "\r\n登出成功\r\n";

			std::cout << oss.str();

			return 0;
		}
	
		int TCPP2PService::getDevInfo(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			//用于获取登陆信息
			std::string			strSessionId;
			TCPUserData			data;
			std::ostringstream	oss;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			oss.str("");
			oss << "GETDEVINFO REQJSON: " << strJson << std::endl;
			std::cout << oss.str();

			try
			{
				//获取sessionid, JSON
				if(0 > TCPJsonParser::json2Session(strJson, strSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//验证是否登陆
				if(false == _ctxTcpMgr.isUserLogin(strSessionId, conn.nSockfd, data))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是主控端
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//1.获取绑定的设备UUID
				TCPUserData					devData;
				std::vector<std::string>	vctDevUUID;

				if(0 > _ctxPstMgr.queryBindsByUser(data._strSessionId, vctDevUUID))
					throw BluevError(__FILE__, __LINE__);

				if(vctDevUUID.empty())
					return BluevError::ERR_NULLDEVINFO;

				//2.通过UUID获取登录信息
				{
					Json::Value			root, devs;
					Json::FastWriter	writer;
					std::string			strJsonAns;

					std::vector<std::string>::const_iterator itr;
					for(itr = vctDevUUID.begin();
						itr != vctDevUUID.end();
						itr++)
					{
						std::string	strDevUUID = *itr;

						//3.通过UUID获取登录信息
						if(0 > _ctxTcpMgr.queryDev(strDevUUID, devData))
							continue;

						//判断是否同一内网
						bool bSameLan = data.isSameLan(devData);

						Json::Value		dev;
						if(0 > TCPJsonBuilder::devData2Json(devData, bSameLan, dev))
							throw BluevError(__FILE__, __LINE__);

						devs[devs.size()] = dev;
					}

					if(devs.size() == 0)
						return BluevError::ERR_NULLLOGINDEV;

					root[JSON_DEVS] = devs;
					strJsonAns = writer.write(root);

					ctx.pAns = new unsigned char[strJsonAns.size()];
					if(ctx.pAns == NULL)
						throw BluevError(__FILE__, __LINE__);

					memcpy(ctx.pAns, strJsonAns.c_str(), strJsonAns.size());
					ctx.nAnsLen = strJsonAns.size();

					oss.str("");
					oss << "GETDEVINFO ANSJSON: " << strJsonAns << std::endl;
					std::cout << oss.str();
				}
			}
			catch(BluevError &e)
			{
				e.print();

				return -1;
			}

			std::string strLog;
			log(strLog, &data);
			strLog.append("获取设备信息成功\r\n");

			std::cout << strLog;

			return 0;
		}

		int TCPP2PService::keepAlive(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			std::string			strSessionId;
			std::ostringstream	oss;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				//获取sessionid, JSON
				if(0 > TCPJsonParser::json2Session(strJson, strSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				if(0 > _ctxTcpMgr.keepAlive(strSessionId, conn.nSockfd))
					throw BluevError(__FILE__, __LINE__, strSessionId.c_str());

				//不用返回json。只返回ans头
			}
			catch(BluevError &e)
			{
				e.print();

				return -1;
			}

			oss.str("");
			oss << "TCP客户:SOCKET:" << conn.nSockfd << " SESSIONID:" << strSessionId << "\r\n发来心跳包\r\n";

			std::cout << oss.str();

			return 0;
		}

		int TCPP2PService::proxyRequestEx(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			int					nRetVal = -1;
			std::string			strSessionId, strDevSessionId;
			std::ostringstream	oss;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//解析request数据 json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			//std::cout << "receive req proxyreqex json:" << strJson << std::endl;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//解析session节点
				if(0 > TCPJsonParser::json2Session(root[JSON_SESSION], strSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//解析devinfo节点
				std::string strDevId;
				if(0 > TCPJsonParser::json2DevInfo(root[JSON_DEVINFO], strDevSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//验证主控端是否登陆
				TCPUserData	data;
				if(!_ctxTcpMgr.isUserLogin(strSessionId, conn.nSockfd, data))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是主控端
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//验证是否存在绑定关系
				std::vector<std::string>	vctDevUUID;
				if(0 > _ctxPstMgr.queryBindsByUser(data._strSessionId, vctDevUUID))
					throw BluevError(__FILE__, __LINE__);

				if(vctDevUUID.empty())
					throw BluevError(__FILE__, __LINE__);

				bool	bFound = false;
				std::vector<std::string>::const_iterator	itr;
				for(itr = vctDevUUID.begin(); itr != vctDevUUID.end(); ++itr)
				{
					if((*itr) == strDevSessionId)
					{
						bFound = true;
						break;
					}
				}

				if(!bFound)
					throw BluevError(__FILE__, __LINE__);

				//验证dev是否登陆
				TCPUserData	devData;
				if(!_ctxTcpMgr.isDevLogin(strDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是被控端
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//重构json和包头
				BLUEV_NetReqHead	req;
				std::string			strProxyJson;
				if(0 > TCPJsonBuilder::rebuildProxyReq(req, strProxyJson, root[JSON_PROXYDATA], strSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDREQ);

				//LOCK
				//投递写

				//开始转发
				if(0 > SocketHelper::BLUEV_SOCK_Send(devData._nSocket, &req, sizeof(req)))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYREQ);
					
				if(0 > SocketHelper::BLUEV_SOCK_Send(devData._nSocket, (void *)strProxyJson.c_str(), strProxyJson.size()))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYREQ);

				oss.str("");
				oss << "REBUILD PROXYREQ FROM SOCKET " << conn.nSockfd << ", " << strSessionId << " TO SOCKET " << devData._nSocket << ", " << strDevSessionId << " JSON:\r\n" << strProxyJson << std::endl;
				std::cout << oss.str();

				//两者建立中转连接
				//_ctxTcpMgr.addUpdateConnInfo(strSessionId, strDevSessionId, CP2PUserCtxMgr::CONN_PROXY);

				if(0 > _ctxTcpMgr.keepAlive(strSessionId, conn.nSockfd))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_KEEPALIVE);
			}
			catch(BluevError &e)
			{
				e.print();

				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			return 0;
		}

		//只是用于TCPServer。不适用于TCPServerEx，IOCPServer
		//建立代理通道，请求者肯定是主控端
		int TCPP2PService::proxyConn(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			std::string	strSessionId, strDevSessionId;
			
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
				if(0 > TCPJsonParser::json2Session(root[JSON_SESSION], strSessionId))
					throw BluevError(__FILE__, __LINE__);

				//解析devinfo节点
				if(0 > TCPJsonParser::json2DevInfo(root[JSON_DEVINFO], strDevSessionId))
					throw BluevError(__FILE__, __LINE__);

				//验证主控端是否登陆
				TCPUserData	data;
				if(false == _ctxTcpMgr.isUserLogin(strSessionId, conn.nSockfd, data))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是主控端
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//验证是否存在绑定关系
				//先从简

				//验证dev是否登陆
				TCPUserData	devData;
				if(0 > _ctxTcpMgr.queryDev(strDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__);

				//验证是否是被控端
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				try
				{
					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__);

					//告知被控端做好代理准备
					//SocketHelper::BLUEV_SOCK_Send(devData._nSocket, (void *)szBuf, nLen);
					//SocketHelper::BLUEV_SOCK_Recv(devData._nSocket, (void *)szBuf, nLen);

					//告知主控端代理准备完毕
					//SocketHelper::BLUEV_SOCK_Send(conn._nSocket, (void *)szBuf, nLen);

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
							while(!_pServer->_svcStatus.isWillStop() && !bHasData[0] && !bHasData[1]);

							if(_pServer->_svcStatus.isWillStop())
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

											i == 0 ? strLog.append(strSessionId) : strLog.append(strDevSessionId);
											strLog.append("至");
											i == 0 ? strLog.append(strDevSessionId) : strLog.append(strSessionId);
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
						while(!_pServer->_svcStatus.isWillStop());
					}
				}
				catch(BluevError & e)
				{
					throw e;
				}

			}
			catch(BluevError &e)
			{
				e.print();

				return -1;
			}

			return 0;
		}



		///////////////////////////////////////////////////
		int TCPJsonParser::json2UserData(const std::string &strJson, TCPUserData &data) throw() 
		{
			if(strJson.empty())
				return -1;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__);

				const Value &lanip = root[JSON_LANIP];
				if(lanip.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &lanport = root[JSON_LANPORT];
				if(lanport.type() != Json::intValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &lanmask = root[JSON_MASK];
				if(lanmask.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &gateway = root[JSON_GATEWAY];
				if(gateway.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &bmainctrl = root[JSON_ISMAINCTRL];
				if(bmainctrl.type() != Json::intValue)
					throw BluevError(__FILE__, __LINE__);

				data._bMainCtrl = bmainctrl.asInt() == 1 ? true : false;
				if(data._bMainCtrl)
				{
					//mac
					const Value &mac = root[JSON_MAC];
					if(mac.type() != Json::stringValue)
						throw BluevError(__FILE__, __LINE__);

					data._strMAC = mac.asCString();

					const Value &username = root[JSON_USERNAME];
					if(username.type() != Json::stringValue)
						throw BluevError(__FILE__, __LINE__);

					data._strUserName = username.asCString();

					const Value &password = root[JSON_PASSWORD];
					if(password.type() != Json::stringValue)
						throw BluevError(__FILE__, __LINE__);

					data._strPassWord = password.asCString();
				}
				else
				{
					//devid
					const Value &devid = root[JSON_DEVID];
					if(devid.type() != Json::stringValue)
						throw BluevError(__FILE__, __LINE__);

					data._strDeviceId = devid.asCString();

					const Value &bupnp = root[JSON_ISUPNP];
					if(bupnp.type() != Json::intValue)
						throw BluevError(__FILE__, __LINE__);

					data._bUPNP = bupnp.asInt() == 1 ? true : false;

					if(data._bUPNP)
					{
						const Value &upnpip = root[JSON_UPNPIP];
						if(upnpip.type() != Json::stringValue)
							throw BluevError(__FILE__, __LINE__);

						const Value &upnpport = root[JSON_UPNPPORT];
						if(upnpport.type() != Json::intValue)
							throw BluevError(__FILE__, __LINE__);

						data._strUPNPIP = upnpip.asCString();
						data._nUPNPPort = upnpport.asInt();
					}
				}

				//devtype
				//remark

				data._strLANIP = lanip.asCString();
				data._nLANPort = lanport.asInt();
				data._strLANMask = lanmask.asCString();
				data._strGateWay = gateway.asCString();
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonParser::json2Session(const std::string &strJson, std::string &strSessionId) throw()
		{
			if(strJson.empty())
				return -1;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__);

				if(0 > json2Session(root, strSessionId))
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonParser::json2Session(const Json::Value &session, std::string &strSessionId) throw()
		{
			try
			{
				if(session.type() != Json::objectValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &sessionid = session[JSON_SESSIONID];
				if(sessionid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				strSessionId = sessionid.asCString();
				if(strSessionId.empty())
					throw BluevError(__FILE__, __LINE__);

			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonParser::json2DevInfo(const Json::Value &devinfo, std::string &strDevSessionId) throw()
		{
			try
			{
				if(devinfo.type() != Json::objectValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &dev_sessionid = devinfo[JSON_SESSIONID];
				if(dev_sessionid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				strDevSessionId = dev_sessionid.asCString();
				if(strDevSessionId.empty())
					throw BluevError(__FILE__, __LINE__);

			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonParser::json2DevInfo(const Json::Value &devinfo, std::string &strDevSessionId, bool &bSameLan) throw()
		{
			try
			{
				if(devinfo.type() != Json::objectValue)
					throw BluevError(__FILE__, __LINE__);

				if(0 > json2DevInfo(devinfo, strDevSessionId))
					throw BluevError(__FILE__, __LINE__);

				/////////////////////////////////////////////////////
				const Value &issamelan = devinfo[JSON_ISSAMELAN];
				if(issamelan.type() != Json::intValue)
					throw BluevError(__FILE__, __LINE__);

				bSameLan = issamelan.asInt() ? true : false;
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}
/*
		int TCPJsonParser::json2MainCtrl(const Json::Value &mainctrl, __int64 &llSessionId, bool &bBroadCast) throw()
		{
			try
			{
				if(mainctrl.type() != Json::objectValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &main_sessionid = mainctrl[JSON_SESSIONID];
				if(main_sessionid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &broadcast = mainctrl[JSON_BROADCAST];
				if(broadcast.type() != Json::intValue)
					throw BluevError(__FILE__, __LINE__);

				llSessionId = _atoi64(main_sessionid.asCString());
				if(llSessionId <= 0)
					throw BluevError(__FILE__, __LINE__);

				bBroadCast = broadcast.asInt() ? true : false;
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}
			catch(...)
			{
				return -1;
			}

			return 0;
		}

		int TCPJsonParser::json2MainCtrl(const Json::Value &mainctrl, __int64 &llSessionId, std::string &strIP, int &nPort) throw()
		{
			try
			{
				if(!mainctrl.isObject())
					throw BluevError(__FILE__, __LINE__);

				const Json::Value	&ip = mainctrl[JSON_IP];
				if(ip.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				const Json::Value	&port = mainctrl[JSON_PORT];
				if(port.type() != Json::intValue)
					throw BluevError(__FILE__, __LINE__);

				const Json::Value	&sessionid = mainctrl[JSON_SESSIONID];
				if(sessionid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				strIP = ip.asString();
				if(strIP.empty())
					throw BluevError(__FILE__, __LINE__);

				nPort = port.asInt();
				if(nPort <= 0)
					throw BluevError(__FILE__, __LINE__);

				llSessionId = _atoi64(sessionid.asCString());
				if(llSessionId <= 0)
					throw BluevError(__FILE__, __LINE__);

			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}
			catch(...)
			{
				return -1;
			}

			return 0;
		}


		int TCPJsonParser::json2ProxyBinary(const Json::Value &proxydata, char * &pData, int &nDataLen) throw()
		{
			try
			{
				if(proxydata.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				//parse proxydata
				std::string strProxy = proxydata.asString();
				if(strProxy.empty())
					throw BluevError(__FILE__, __LINE__);

				//base64 to binary
				pData = NULL;
				nDataLen = 0;

				if(0 > UTIL_BASE64::decodeEx(strProxy.c_str(), strProxy.size(), &pData, &nDataLen, 0) ||
					pData == NULL || nDataLen <= 0)
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}
			catch(...)
			{
				return -1;
			}

			return 0;
		}
*/

		//////////////////////////////////////////////////////////
		int TCPJsonBuilder::session2Json(const std::string &strSessionId, std::string &strJson)
		{
			if(strSessionId.empty())
				return -1;

			strJson.clear();

			try
			{
				Json::Value	root;
				root[JSON_SESSIONID] = Value(strSessionId);

				Json::FastWriter	writer;
				strJson = writer.write(root);

				if(strJson.empty())
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonBuilder::session2Json(const std::string &strSessionId,  Json::Value &session)
		{
			if(strSessionId.empty())
				return -1;

			try
			{
				session[JSON_SESSIONID] = Value(strSessionId);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonBuilder::mainctrl2Json(const std::string &strSessionId, const std::string &strIP, const int &nPort, std::string &strJson)
		{
			if(strSessionId.empty() || strIP.empty() || nPort <= 0)
				return -1;

			strJson.clear();

			try
			{
				Json::Value			root;
				Json::FastWriter	writer;
				std::string			strJsonSend;
				Json::Value			mainctrl;

				mainctrl[JSON_IP] = Value(strIP);
				mainctrl[JSON_PORT] = Value(nPort);
				mainctrl[JSON_SESSIONID] = Value(strSessionId);

				root[JSON_MAINCTRL] = mainctrl;

				strJson = writer.write(root);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonBuilder::mainctrl2Json(const std::string &strSessionId, Json::Value &mainctrl)
		{
			if(strSessionId.empty())
				return -1;

			mainctrl.clear();

			try
			{
				mainctrl[JSON_SESSIONID] = Value(strSessionId);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonBuilder::devData2Json(const TCPUserData &devdata, bool bSameLan, Json::Value &dev)
		{
			dev.clear();

			try
			{
				dev[JSON_SESSIONID] = Value(devdata._strSessionId);
				dev[JSON_IP] = Value(devdata._strNATIP);
				dev[JSON_PORT] = Value(devdata._nNATPort);

				//是否同一内网
				dev[JSON_ISSAMELAN] = Value(bSameLan ? 1 : 0);

				if(bSameLan)
				{
					dev[JSON_LANIP] = Value(devdata._strLANIP);
					dev[JSON_LANPORT] = Value(devdata._nLANPort);
				}

				dev[JSON_ISUPNP] = Value(devdata._bUPNP ? 1 : 0);
				if(devdata._bUPNP)
				{
					dev[JSON_UPNPIP] = Value(devdata._strUPNPIP);
					dev[JSON_UPNPPORT] = Value(devdata._nUPNPPort);
				}

			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonBuilder::devData2Json(const std::string &strDevSessionId, bool bSameLan, Json::Value &dev)
		{
			if(strDevSessionId.empty())
				return -1;

			dev.clear();

			try
			{
				if(0 > devData2Json(strDevSessionId, dev))
					throw BluevError(__FILE__, __LINE__);

				dev[JSON_ISSAMELAN] = Value(bSameLan ? 1 : 0);

			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonBuilder::devData2Json(const std::string &strDevSessionId, Json::Value &dev)
		{
			if(strDevSessionId.empty())
				return -1;

			try
			{
				dev[JSON_SESSIONID] = Value(strDevSessionId);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}
		
		int TCPJsonBuilder::userData2Json(const TCPUserLogin &user, std::string &strJson)
		{
			if(user.nLanPort <= 0)
				return -1;

			strJson.clear();

			try
			{
				Json::Value	root;
				Json::FastWriter	writer;

				root[JSON_LANIP] = Json::Value(user.strLanIp);
				root[JSON_LANPORT] = Json::Value(user.nLanPort);
				root[JSON_MASK] = Json::Value(user.strMask);
				root[JSON_GATEWAY] = Json::Value(user.strGateWay);
				root[JSON_ISMAINCTRL] = Json::Value(user.bMainCtrl ? 1 : 0);
				if(user.bMainCtrl)
				{
					root[JSON_MAC] = Json::Value(user.strId);
					root[JSON_USERNAME] = Json::Value(user.strUserName);
					root[JSON_PASSWORD] = Json::Value(user.strPassWord);
				}
				else
				{
					root[JSON_DEVID] = Json::Value(user.strId);
					//被控端提供UPNP信息
					root[JSON_ISUPNP] = Json::Value(user.bUPNP ? 1 : 0);
					if(user.bUPNP)
					{
						root[JSON_UPNPIP] = Json::Value(user.strUPNPIP);
						root[JSON_UPNPPORT] = Json::Value(user.nUPNPPort);
					}
				}

				strJson = writer.write(root);
				if(strJson.empty())
					return -1;
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}
		
		int TCPJsonBuilder::proxyBinary2json(const char * pData, int nDataLen, Json::Value &root) throw()
		{
			if(pData == NULL || nDataLen <= 0)
				return -1;

			try
			{
				//base64编码
				char	*pBase64 = NULL;
				int		nBase64Len = 0;

				if(0 > UTIL_BASE64::encode((char *)pData, nDataLen, &pBase64, &nBase64Len, 0) ||
					pBase64 == NULL || nBase64Len <= 0)
					throw BluevError(__FILE__, __LINE__);

				std::string	strProxyData(pBase64, nBase64Len);
				root[JSON_PROXYDATA] = Json::Value(strProxyData);

				//手动释放内存
				if(pBase64)
				{
					delete[] pBase64;
					pBase64 = NULL;
					nBase64Len = 0;
				}
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int TCPJsonBuilder::rebuildProxyReq(BLUEV_NetReqHead &req, std::string &strJson, const Json::Value &proxydata, const std::string &strSessionId)
		{
			if(strSessionId.empty())
				return -1;

			try
			{
				if(proxydata.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);
				
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_P2PPROXY, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCP_PROXY, req.nOpCode);

				strJson.clear();

				Json::Value			root;
				Json::FastWriter	writer;
				root[JSON_PROXYDATA] = proxydata;
				root[JSON_SESSIONID] = Json::Value(strSessionId);

				strJson = writer.write(root);
				if(strJson.empty())
					throw BluevError(__FILE__, __LINE__);

				UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}
/*
		int TCPJsonBuilder::rebuildProxyAns(BLUEV_UDPAnsHead &ans, std::string &strJson, const Json::Value &proxydata, const __int64 &llSessionId)
		{
			if(llSessionId <= 0)
				return -1;

			try
			{
				if(proxydata.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				ans.init();
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PPROXYANS, ans.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_PROXYANS, ans.nOpCode);

				char	szBuf[64] = {0};
				sprintf(szBuf, "%lld", llSessionId);

				strJson.clear();

				Json::Value			root;
				Json::FastWriter	writer;
				root[JSON_PROXYDATA] = proxydata;
				root[JSON_SESSIONID] = Json::Value(szBuf);

				time_t		tt;

				time(&tt);
				sprintf(szBuf, "%d", tt);

				root[JSON_STAMP] = Json::Value(szBuf);

				strJson = writer.write(root);
				if(strJson.empty())
					throw BluevError(__FILE__, __LINE__);

				UTIL_CONV::uint2ToChar2(strJson.size(), ans.nDataLen);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}
		*/
	}
}