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
			//��½��Ϣ
			P2PUserData			data;
			P2PUserData			queryData;
			//BLUEV_NetReqHead	&req = conn.req;

			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			//����request���� json
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

				//�Ƿ��ѵ�½
				/*if(!_ctxMgr.isLogin(data._strNATIP, data._nNATPort, data._strLANIP, data._nLANPort, 
					data._bMainCtrl, data._bMainCtrl ? data._strMAC : data._strDeviceId, queryData)
					)*/
				if(!_ctxMgr.isLogin(data, queryData))
				{
					if(0 > _ctxMgr.login(data))
						throw BluevError(__FILE__, __LINE__);

					//��һ���û����롣
					/*if(!_bChkDeadCreated)
					{
						//�������ӳ�ʱ����߳�
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
				else	//�ѵ�½��������ݱ�־
				{
					data = queryData;
				}

				//����json��  sessionid + guid
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
				std::string strLog = "tcp���뷢���쳣";
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");

				std::cout << strLog;
				return -1;
			}

			char	szBuf[64] = {0};
			sprintf(szBuf, "%lld", queryData._llSessionId);

			std::string strLog = "��tcp�ͻ�����:";
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

			//����request���� json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				std::string strGUID;
				if(0 > JsonParser::json2Session(strJson, llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				if(0 > _ctxMgr.logout(llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				//���÷���json��ֻ����ansͷ
			}
			catch(BluevError &e)
			{
				std::string strLog = "tcp�ǳ������쳣";
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");

				std::cout << strLog;
				return -1;
			}

			char	szBuf[64] = {0};
			sprintf(szBuf, "%lld", llSessionId);

			std::string strLog = "��tcp�ͻ��ǳ�:";
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

			//����request���� json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			//���ڻ�ȡ��½��Ϣ
			P2PUserData	data;

			try
			{
				std::string strGUID;
				if(0 > JsonParser::json2Session(strJson, llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ��½
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ������ض�
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//���IP��PORT�Ƿ����½��Ϣһ��

				//1����ȡ�󶨵�id	Ŀǰȱ��ά���˹�ϵ���ݽṹmultimap<sessionid, dev_sessionid>, 
				//�ɷŵ�ҵ��������У�������ӿڴ����ݿ��ȡ��

				//2��ͨ��id��ѯ��ȡ�豸��Ϣ
				//����ֻ���ؿ��õ�sessionid + devid.���ض��Դ�ͨ��tcp��ѯ
				//���ɷ���guid������Ϳ�����logout�ˡ�


				//�ȴӼ򣬺���Ȩ������,�������9��δ��ռ�ã������ض���Ϣ
				vct_users_t	vctUsers;
				P2PUserData	devData;
				if(0 > _ctxMgr.getUsers(vctUsers))
					throw BluevError(__FILE__, __LINE__);

				//����json��  sessionid + devid + natip + natport +
				//�ж��Ƿ�ͬһ���أ����� issamelan + lanip + lanport
				//ÿ����100�ֽڣ���෵��9��
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

						//�ж��Ƿ�ͬһ����
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
				std::string strLog = "tcp��ȡ�豸��Ϣ�����쳣";
				strLog.append(e.getFileName());
				strLog.append(e.getLineName());
				strLog.append("\r\n");

				return -1;
			}

			char	szBuf[64] = {0};
			sprintf(szBuf, "%lld", data._llSessionId);

			std::string strLog = "tcp�ͻ�:";
			strLog.append((char *)szBuf);
			strLog.append("��ȡ�豸��Ϣ ");
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

			//����request���� json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				Json::Value		root;
				Json::Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__);

				//��֤���ض��Ƿ��½
				std::string strGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				P2PUserData	data;
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ������ض�
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ���ڰ󶨹�ϵ
				//�ȴӼ�

				//��֤dev�Ƿ��½
				std::string strDevId;
				bool	bSameLan = false;
				if(0 > JsonParser::json2DevInfo(root[JSON_DEVINFO], llDevSessionId, strDevId, bSameLan))
					throw BluevError(__FILE__, __LINE__);

				P2PUserData	devData;
				if(0 > _ctxMgr.query(llDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__);
				
				//��֤�Ƿ��Ǳ��ض�
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//��֤devid�Ƿ����
				if(devData._strDeviceId != strDevId)
					throw BluevError(__FILE__, __LINE__);

				//���ض�����ͬһ��������ͨ
				bool	bSuccess = bSameLan;

				if(bSuccess)	//��������ͨ
				{
					//��֤�Ƿ�ͬһ��������
					bool	bSameLan = data.isSameLan(devData);

					if(bSameLan)
					{
						//��������ռ�ñ�ǣ����سɹ�
						//ctxMgr.setUsed(pDevData->_llSessionId);

						char	szBuf[64] = {0};
						sprintf(szBuf, "%lld", data._llSessionId);

						std::string strLog = "tcp�ͻ�:";
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

						strLog.append(" �ɹ����������豸");

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
						//�ٵ�ֱ�ӷŻ�err
						return -1;
					}
				}
				
				//��������δ��ͨ
				{
					//��֤dev�Ƿ���������ռ��
					/*
					if(pDevData->_bUsed)
					{
						//�Ƿ��Ǳ�����ռ��
						//if(pDevData->xx)
							//����Э������Ϊ���ܴ���δ�ɹ�
						//else
						throw BluevError(__FILE__, __LINE__);
					}*/
					
					//��������д��Ҫ��������pDevDataΪconst
					//����ռ�ñ��
					//ctxMgr.setUsed(pDevData->_llSessionId);
				}
				
				char	szBuf[64] = {0};
				sprintf(szBuf, "%lld", data._llSessionId);

				std::string strLog = "tcp�ͻ�:";
				strLog.append((char *)szBuf);
				strLog.append("ip:");
				strLog.append(data._strNATIP);
				strLog.append(" port:");

				sprintf(szBuf, "%d", data._nNATPort);
				strLog.append((char *)szBuf);

				strLog.append("���������豸");

				sprintf(szBuf, "%lld", devData._llSessionId);
				strLog.append((char *)szBuf);

				strLog.append("ip:");
				strLog.append(devData._strNATIP);
				strLog.append(" port:");

				sprintf(szBuf, "%d", devData._nNATPort);
				strLog.append((char *)szBuf);
				strLog.append("\r\n");

				std::cout << strLog;
				
				//����Э����ָ��
				
				//����json��  ip + port
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

					//��������ͷ
					BLUEV_NetReqHead	req;

					UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, req.nOpClass);
					UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_DIG, req.nOpCode);
					UTIL_CONV::uint4ToChar4(strJsonSend.size(), req.nDataLen);
					
					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__);
					
					std::cout << "��ʼ����Э����ָ��\r\n";

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

					//�ͻ���Aһֱ���󣬵��յ��ͻ���B�Ĵ���Ϣ��ֹͣ���ɡ�
				}
				
			}
			catch(BluevError &e)
			{
				std::string strLog = "TCP�ͻ����������豸�����쳣";
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

			//����request���� json
			int nJsonLen = ctx.nReqLen;

			std::string	strJson((char* )ctx.pReq, nJsonLen);

			try
			{
				Json::Value	root;
				Json::Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__);

				//����session�ڵ�
				std::string strGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);

				//����devinfo�ڵ�
				std::string strDevId;
				if(0 > JsonParser::json2DevInfo(root[JSON_DEVINFO], llDevSessionId, strDevId))
					throw BluevError(__FILE__, __LINE__);

				//��֤���ض��Ƿ��½
				P2PUserData	data;
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ������ض�
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ���ڰ󶨹�ϵ
				//�ȴӼ�

				//��֤dev�Ƿ��½
				P2PUserData	devData;
				if(0 > _ctxMgr.query(llDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ��Ǳ��ض�
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//��֤devid�Ƿ����
				if(devData._strDeviceId != strDevId)
					throw BluevError(__FILE__, __LINE__);

				try
				{
					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__);

					//�����ɹ�

					//��������
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
											std::string strLog = "tcp����ת����";

											sprintf(szBuf, "%lld", i == 0 ? llSessionId : llDevSessionId);
											strLog.append(szBuf);
											strLog.append("��");
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
				std::string strLog = "tcp����ת����";

				sprintf(szBuf, "%lld", llSessionId);
				strLog.append(szBuf);
				strLog.append("��");
				sprintf(szBuf, "%lld", llDevSessionId);
				strLog.append(szBuf);
				strLog.append("�����쳣");
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