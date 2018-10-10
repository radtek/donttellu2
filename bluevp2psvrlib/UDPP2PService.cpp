
#include "bluevp2psvrdef.h"
#include <process.h>

namespace BLUEV {
	namespace BVP2PSVRLIB{

		using Json::Reader;
		using Json::Value;

		using BLUEVNET::BLUEVOpClass;
		using BLUEVNET::SocketHelper;
		using BLUEVNET::BLUEV_UDPAnsHead;
		using BLUEVNET::BLUEV_UDPReqHead;
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::UTIL_BASE64;
		using BLUEVBASE::BluevError;
		
		extern IServiceStatus*	g_svcStatus;
		unsigned WINAPI	UDPP2PChkDead_Thread_Proc(void *pParam)
		{
			UDPP2PService	*pService = (UDPP2PService *)pParam;
			if(pService == NULL)
				return -1;
			
			if(!g_svcStatus)
				return -1;

			char			*pszThreadName = "CHKDEAD �߳�";

			g_svcStatus->incThreadCnt();

			char	szBuf[100] = {0};
			sprintf(szBuf, "%s %d started. \r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			__int64	llCount = pService->_llCount;
			__int64 llDelCout = 0;
			while(!g_svcStatus->isWillStop())
			{
				llDelCout = pService->_ctxMgr.checkDeadEx(llCount);
				if(llDelCout)
				{
					memset(szBuf, 0, sizeof(szBuf));
					sprintf(szBuf, "ɾ��%lld��dead����\r\n", llDelCout);
					std::cout << szBuf;
				}

				Sleep(1000);
			}

			memset(szBuf, 0, sizeof(szBuf));
			sprintf(szBuf, "%s %d ����.\r\n", pszThreadName, GetCurrentThreadId());
			std::cout << szBuf;

			g_svcStatus->decThreadCnt();

			return 0;
		}

		int	UDPP2PService::execute(UDPConnection &conn, UDPServiceParamContext &ctx) throw()
		{
			int				retval = -1;
			BLUEV_UDPAnsHead	*pAns = NULL;

			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;
			
			int opClass = UTIL_CONV::Char2To_uint2(pReq->nOpClass);
			int opCode = UTIL_CONV::Char2To_uint2(pReq->nOpCode);
			
			ctx.nAnsLen = sizeof(BLUEV_UDPAnsHead);

			try 
			{
				if(opClass == BLUEVOpClass::OP_CLASS_P2P)
				{
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
						retval = keepAlive(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_PROXY:
						//retval = proxyRequest(conn, ctx);
						retval = proxyRequestEx(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_PROXYANS:
						//retval = proxyAnswer(conn, ctx);
						retval = proxyAnswerEx(conn, ctx);
						break;
					default:
						retval = -1;
					}
				}
				else if(opClass == BLUEVOpClass::OP_CLASS_P2PCLIENTANS)
				{
					switch(opCode)
					{
					case BLUEVOpClass::OP_P2P_DIG:
						ctx.nAnsLen = 0;
						retval = 0;

						break;
					default:
						retval = -1;
					}
				}
				
			}
			catch(...)
			{
				retval = -1;
			}

			if(ctx.nAnsLen == 0)
				return 0;

			//ans���������
			if(ctx.nAnsLen > sizeof(ctx.bnAnsBuf))
			{
				ctx.nAnsLen = 0;
				return -1;
			}

			pAns = (BLUEV_UDPAnsHead *)ctx.bnAnsBuf;

			pAns->init();
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PANS, pAns->nOpClass);
			UTIL_CONV::uint2ToChar2(opCode, pAns->nOpCode);

			UTIL_CONV::uint2ToChar2(retval, pAns->nRetVal);
			
			if(ctx.nAnsLen > sizeof(BLUEV_UDPAnsHead))
				UTIL_CONV::uint2ToChar2(ctx.nAnsLen - sizeof(BLUEV_UDPAnsHead), pAns->nDataLen);

			return	retval;
		}

		int UDPP2PService::login(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			int					nRetVal = -1;
			//��½��Ϣ
			P2PUserData			data;
			P2PUserData			queryData;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����request���� json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return BluevError::ERR_PARSEREQ;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);
			
			std::cout << "receive req login json:" << strJson << std::endl;

			try
			{
				if(0 > JsonParser::json2UserData(strJson, data))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//NAT
				data._strNATIP = inet_ntoa(conn.fromaddr.sin_addr);
				data._nNATPort = ntohs(conn.fromaddr.sin_port);

				data._nSocket = conn.nSockfd;

				//�Ƿ��ѵ�½
				/*if(!_ctxMgr.isLogin(data._strNATIP, data._nNATPort, data._strLANIP, data._nLANPort, 
					data._bMainCtrl, data._bMainCtrl ? data._strMAC : data._strDeviceId, queryData)
				)*/
				if(!_ctxMgr.isLogin(data, queryData))
				{
					if(0 > _ctxMgr.login(data))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_LOGIN);

					//��һ���û����롣
					if(!_bChkDeadCreated)
					{
						//�������ӳ�ʱ����߳�
						HANDLE		hHandle;
						unsigned	tid;

						hHandle = (HANDLE)_beginthreadex(0, 0, UDPP2PChkDead_Thread_Proc, this, 0, &tid);
						if(hHandle == INVALID_HANDLE_VALUE)
							return -1;

						CloseHandle(hHandle);
						_bChkDeadCreated = true;
					}

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
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);

					if(strJsonAns.size() >= sizeof(ctx.bnAnsBuf) - ctx.nAnsLen)
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);

					memcpy(ctx.bnAnsBuf + ctx.nAnsLen, strJsonAns.c_str(), strJsonAns.size());
					ctx.nAnsLen += strJsonAns.size();
					
					std::cout << "login ans json: " << strJsonAns << std::endl;
				}
			}
			catch (BluevError &e)
			{
				std::cout << "�ͻ�" << queryData._llSessionId << "���뷢���쳣" << e.getFileName() << e.getLineName() << std::endl;
				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			std::cout << "�пͻ�����:" << queryData._llSessionId << "ip:" << queryData._strNATIP << " port:" << queryData._nNATPort << std::endl;

			return 0;
		}

		int UDPP2PService::logout(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			int			nRetVal = -1;
			__int64		llSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����request���� json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return BluevError::ERR_PARSEREQ;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);

			std::cout << "receive req logout json:" << strJson << std::endl;

			try
			{
				std::string strGUID;
				if(0 > JsonParser::json2Session(strJson, llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				if(0 > _ctxMgr.logout(llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_LOGOUT);

				//���÷���json��ֻ����ansͷ
			}
			catch(BluevError &e)
			{
				std::cout << "�ͻ�" << llSessionId << "�ǳ������쳣" << e.getFileName() << e.getLineName() << std::endl;
				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			std::cout << "�пͻ��ǳ�:" << llSessionId << std::endl;

			return 0;
		}

		int UDPP2PService::getDevInfo(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			int			nRetVal = -1;
			__int64		llSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����request���� json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return BluevError::ERR_PARSEREQ;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);
			
			std::cout << "receive req getdevinfo json:" << strJson << std::endl;

			//���ڻ�ȡ��½��Ϣ
			P2PUserData	data;

			try
			{
				std::string strGUID;
				if(0 > JsonParser::json2Session(strJson, llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//��֤�Ƿ��½
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);

				//��֤�Ƿ������ض�
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);

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
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_GETDEVINFO);

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

						if(itr->_bMainCtrl)
							continue;

						devData = *(itr);
						
						//�ж��Ƿ�ͬһ����
						bool bSameLan = data.isSameLan(devData);
						
						Json::Value		dev;
						if(0 > JsonBuilder::devData2Json(devData, bSameLan, dev))
							throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);

						devs[devs.size()] = dev;
					}
					
					root[JSON_DEVS] = devs;
					strJsonAns = writer.write(root);

					if(strJsonAns.size() >= sizeof(ctx.bnAnsBuf) - ctx.nAnsLen)
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);
					memcpy(ctx.bnAnsBuf + ctx.nAnsLen, strJsonAns.c_str(), strJsonAns.size());
					ctx.nAnsLen += strJsonAns.size();

					std::cout << "getdevinfo ans json: " << strJsonAns << std::endl;

				}

				if(0 > _ctxMgr.keepAlive(llSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_KEEPALIVE);
			}
			catch(BluevError &e)
			{
				std::cout << "�ͻ�" << llSessionId << "��ȡ�豸��Ϣ�����쳣" << e.getFileName() << e.getLineName() << std::endl;
				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			std::cout << "�ͻ�:" << data._llSessionId << "��ȡ�豸��Ϣ" << "ip:" << data._strNATIP << " port:" << data._nNATPort << std::endl;

			return 0;
		}
		
		int UDPP2PService::connDevice(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			int			nRetVal = -1;
			__int64		llSessionId = 0, llDevSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����request���� json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return BluevError::ERR_PARSEREQ;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);

			std::cout << "receive req conndevice json:" << strJson << std::endl;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//��֤���ض��Ƿ��½
				std::string strGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				P2PUserData	data;
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);

				//��֤�Ƿ������ض�
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);

				//��֤�Ƿ���ڰ󶨹�ϵ
				//�ȴӼ�

				//��֤dev�Ƿ��½
				std::string strDevId;
				bool	bSameLan = false;
				if(0 > JsonParser::json2DevInfo(root[JSON_DEVINFO], llDevSessionId, strDevId, bSameLan))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				P2PUserData	devData;
				if(0 > _ctxMgr.query(llDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);
				
				//��֤�Ƿ��Ǳ��ض�
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);

				//��֤devid�Ƿ����
				if(devData._strDeviceId != strDevId)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);

				//���ض�����ͬһ��������ͨ
				bool	bSuccess = bSameLan;

				if(bSuccess)	//��������ͨ
				{
					//��֤�Ƿ�ͬһ��������
					bool	bSameLan = data.isSameLan(devData);

					if(bSameLan)
					{
						//���߽�����������
						_ctxMgr.addUpdateConnInfo(data._llSessionId, devData._llSessionId, CP2PUserCtxMgr::CONN_LAN);

						//std::cout << "�ͻ�:" << llSessionId << "�ɹ����������豸" << std::endl;
						std::cout << "�ͻ�:" << data._llSessionId << "natip:" << data._strNATIP << " natport:" << data._nNATPort << \
							" lanip:" << data._strLANIP << " lanport:" << data._nLANPort << \
							" �ɹ����������豸" << " natip:" << devData._strNATIP << " natport:" << devData._nNATPort << \
							" lanip:" << devData._strLANIP << " lanport:" << devData._nLANPort << std::endl;

						//�ظ�һ����ͷ
						return 0;
					}
					else
					{
						//�ٵ�ֱ�ӷ���err
						return -1;
					}
				}
				
				//��ǰ��Ϊ���߽���P2P����
				_ctxMgr.addUpdateConnInfo(data._llSessionId, devData._llSessionId, CP2PUserCtxMgr::CONN_P2P);
				
				//std::cout << "�ͻ�:" << llSessionId << "���������豸��" << llDevSessionId << std::endl;
				std::cout << "�ͻ�:" << data._llSessionId << "ip:" << data._strNATIP << " port:" << data._nNATPort \
					<< "���������豸" << devData._llSessionId << "ip:" << devData._strNATIP << " port:" << devData._nNATPort << std::endl;

				
				//����Э����ָ��
				
				//����json��  ip + port
				{
					char		szSendBuf[1024] = {0};
					std::string	strJsonSend;
					if(0 > JsonBuilder::mainctrl2Json(data._llSessionId, data._strNATIP, data._nNATPort, strJsonSend))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDREQ);

					if(strJsonSend.size() >= sizeof(szSendBuf) - sizeof(BLUEV_UDPReqHead))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDREQ);

					std::cout << "conn dev to dig send json: " << strJsonSend << std::endl;

					//��������ͷ
					BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)szSendBuf;
					req.init();

					UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, req.nOpClass);
					UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_DIG, req.nOpCode);
					UTIL_CONV::uint2ToChar2(strJsonSend.size(), req.nDataLen);

					//�ؽ����������豸������Ϣ
					struct	sockaddr_in sin;  
					int		nAddrLen = sizeof(sin);
					
					if(0 > ServiceHelper::rebuildConn(devData._strNATIP, devData._nNATPort, sin, nAddrLen))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_CONNDEV);
					
					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_CONNDEV);
					
					memcpy(szSendBuf + sizeof(req), strJsonSend.c_str(), strJsonSend.size());

					//����ͨ���첽���� TCP��ʽ ��Ϣ������
					std::cout << "��ʼ����Э����ָ��" << std::endl;

					//����˫�ߴ�������ͬһ��socket

					sendto(devData._nSocket, (char *)&req, sizeof(BLUEV_UDPReqHead) + strJsonSend.size(), 0, (struct sockaddr *)&sin, nAddrLen);
					
					//�򶴳ɹ�
					//���ȴ���ִ���ͻ���Aһֱ���󣬵��յ��ͻ���B�Ĵ���Ϣ��ֹͣ���ɡ�
				}
				
				if(0 > _ctxMgr.keepAlive(llSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_KEEPALIVE);
			}
			catch(BluevError &e)
			{
				std::cout << "�ͻ�" << llSessionId << "���������豸" << llDevSessionId << "�����쳣" << e.getFileName() << e.getLineName() << std::endl;
				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			return 0;
		}

		int UDPP2PService::keepAlive(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			int			nRetVal = -1;
			__int64		llSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����request���� json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return BluevError::ERR_PARSEREQ;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);

			std::cout << "receive req keeplive json:" << strJson << std::endl;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				const Value &sessionid = root[JSON_SESSIONID];
				if(sessionid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				/*const Value &guid = root[JSON_GUID];
				if(guid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);*/

				llSessionId = _atoi64(sessionid.asCString());
				if(llSessionId <= 0)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				/*std::string strGUID = guid.asCString();
				if(strGUID.empty())
					throw BluevError(__FILE__, __LINE__);

				//is log in
				if(0 > ctxMgr.query(llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__);*/

				if(0 > _ctxMgr.keepAlive(llSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_KEEPALIVE);

				//���÷���json��ֻ����ansͷ
			}
			catch(BluevError &e)
			{
				std::cout << "�ͻ�" << llSessionId << "���������쳣" << e.getFileName() << e.getLineName() << std::endl;
				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			std::cout << "�յ�������:" << llSessionId << std::endl;

			return 0;
		}
		
		int UDPP2PService::proxyRequest(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			__int64		llSessionId = 0, llDevSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����request���� json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return -1;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);

			std::cout << "receive req proxyreq json:" << strJson << std::endl;

			try
			{
				Value	root;
				Reader	reader;

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

				//base64 to binary
				char	*pData = NULL;
				int		nDataLen = 0;

				try
				{
					if(0 > JsonParser::json2ProxyBinary(root[JSON_PROXYDATA], pData, nDataLen))
						throw BluevError(__FILE__, __LINE__);

					//�ؽ����������豸������Ϣ
					struct	sockaddr_in sin;  
					int		nAddrLen = sizeof(sin);

					if(0 > ServiceHelper::rebuildConn(devData._strNATIP, devData._nNATPort, sin, nAddrLen))
						throw BluevError(__FILE__, __LINE__);

					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__);

					//��ʼת��
					if(0 > sendto(conn.nSockfd, pData, nDataLen, 0, (struct sockaddr *)&sin, nAddrLen))
						throw BluevError(__FILE__, __LINE__);

					std::cout << "����ת�������" << llSessionId << "��" << llDevSessionId << std::endl;

					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}
				}
				catch(BluevError & e)
				{
					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}

					throw e;
				}

				//���ظ�
				ctx.nAnsLen = 0;

				if(0 > _ctxMgr.keepAlive(llSessionId))
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				std::cout << "����ͨ�������" << llSessionId << "��" << llDevSessionId << "�����쳣" << e.getFileName() << e.getLineName() << std::endl;
				return -1;
			}

			return 0;
		}

		int UDPP2PService::proxyAnswer(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			__int64		llDevSessionId = 0, llSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����Ӧ������ json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return -1;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);

			std::cout << "receive req proxyans json:" << strJson << std::endl;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__);

				//����session�ڵ�
				std::string strDevGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llDevSessionId, strDevGUID))
					throw BluevError(__FILE__, __LINE__);

				//����mainctrl�ڵ�
				bool bBroadCast = false;
				if(0 > JsonParser::json2MainCtrl(root[JSON_MAINCTRL], llSessionId, bBroadCast))
					throw BluevError(__FILE__, __LINE__);

				//��֤dev�Ƿ��½
				P2PUserData	devData;
				if(false == _ctxMgr.isLogin(llDevSessionId, strDevGUID, devData))
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ���dev
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ���ڰ󶨹�ϵ
				//�ȴӼ�

				//��֤���ض��Ƿ��½
				P2PUserData	mainData;
				if(0 > _ctxMgr.query(llSessionId, mainData))
					throw BluevError(__FILE__, __LINE__);

				//��֤�Ƿ������ض�
				if(!mainData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__);

				//base64 to binary
				char	*pData = NULL;
				int		nDataLen = 0;

				try
				{
					//����proxydata�ڵ�
					if(0 > JsonParser::json2ProxyBinary(root[JSON_PROXYDATA], pData, nDataLen))
						throw BluevError(__FILE__, __LINE__);

					//�ؽ����������豸������Ϣ
					struct	sockaddr_in sin;  
					int		nAddrLen = 0;

					if(0 > ServiceHelper::rebuildConn(mainData._strNATIP, mainData._nNATPort, sin, nAddrLen))
						throw BluevError(__FILE__, __LINE__);

					if(mainData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__);

					//��ʼת��
					if(0 > sendto(conn.nSockfd, pData, nDataLen, 0, (struct sockaddr *)&sin, nAddrLen))
						throw BluevError(__FILE__, __LINE__);

					std::cout << "����ת��Ӧ���" << llDevSessionId << "��" << llSessionId << std::endl;

					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}
				}
				catch(BluevError & e)
				{
					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}

					throw e;
				}

				//���ظ�
				ctx.nAnsLen = 0;

				if(0 > _ctxMgr.keepAlive(llSessionId))
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				std::cout << "����ͨ��Ӧ���" << llSessionId << "��" << llDevSessionId << "�����쳣" << e.getFileName() << e.getLineName() << std::endl;
				return -1;
			}

			return 0;
		}

		int UDPP2PService::proxyRequestEx(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			int			nRetVal = -1;
			__int64		llSessionId = 0, llDevSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����request���� json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return BluevError::ERR_PARSEREQ;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);

			std::cout << "receive req proxyreqex json:" << strJson << std::endl;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//����session�ڵ�
				std::string strGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llSessionId, strGUID))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//����devinfo�ڵ�
				std::string strDevId;
				if(0 > JsonParser::json2DevInfo(root[JSON_DEVINFO], llDevSessionId, strDevId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//��֤���ض��Ƿ��½
				P2PUserData	data;
				if(false == _ctxMgr.isLogin(llSessionId, strGUID, data))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);

				//��֤�Ƿ������ض�
				if(!data._bMainCtrl)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);

				//��֤�Ƿ���ڰ󶨹�ϵ
				//�ȴӼ�

				//��֤dev�Ƿ��½
				P2PUserData	devData;
				if(0 > _ctxMgr.query(llDevSessionId, devData))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);

				//��֤�Ƿ��Ǳ��ض�
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);

				//��֤devid�Ƿ����
				if(devData._strDeviceId != strDevId)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);

				//�ع�json�Ͱ�ͷ
				BLUEV_UDPReqHead	req;
				std::string			strProxyJson;
				if(0 > JsonBuilder::rebuildProxyReq(req, strProxyJson, root[JSON_PROXYDATA], llSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDREQ);
				
				int		nDataLen = sizeof(req) + strProxyJson.size();
				if(nDataLen >= sizeof(ctx.bnReqBuf))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDREQ);

				char	*pData = NULL;

				try
				{
					pData = new char[nDataLen];
					if(pData == NULL)
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDREQ);

					memcpy(pData, &req, sizeof(req));
					memcpy(pData + sizeof(req), strProxyJson.c_str(), strProxyJson.size());

					//�ؽ����������豸������Ϣ
					struct	sockaddr_in sin;  
					int		nAddrLen = sizeof(sin);

					if(0 > ServiceHelper::rebuildConn(devData._strNATIP, devData._nNATPort, sin, nAddrLen))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYREQ);

					if(devData._nSocket == INVALID_SOCKET)
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYREQ);

					//��ʼת��
					if(0 > sendto(conn.nSockfd, pData, nDataLen, 0, (struct sockaddr *)&sin, nAddrLen))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYREQ);

					std::cout << "����ת�������" << llSessionId << "��" << llDevSessionId << std::endl;

					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}
				}
				catch(BluevError & e)
				{
					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}

					throw e;
				}

				//���߽�����ת����
				_ctxMgr.addUpdateConnInfo(llSessionId, llDevSessionId, CP2PUserCtxMgr::CONN_PROXY);

				//���ظ�
				//ctx.nAnsLen = 0;

				if(0 > _ctxMgr.keepAlive(llSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_KEEPALIVE);
			}
			catch(BluevError &e)
			{
				std::cout << "����ͨ�������" << llSessionId << "��" << llDevSessionId << "�����쳣" << e.getFileName() << e.getLineName() << std::endl;
				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			return 0;
		}

		int UDPP2PService::proxyAnswerEx(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			int			nRetVal = -1;
			__int64		llDevSessionId = 0, llSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//����Ӧ������ json
			int nJsonLen = UTIL_CONV::Char2To_uint2(pReq->nDataLen);
			if(nJsonLen <= 0)
				return BluevError::ERR_PARSEREQ;

			std::string	strJson((char* )(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), nJsonLen);

			std::cout << "receive req proxyansex json:" << strJson << std::endl;

			try
			{
				Value	root;
				Reader	reader;

				reader.parse(strJson, root);
				if(root.isArray())
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//����session�ڵ�
				std::string strDevGUID;
				if(0 > JsonParser::json2Session(root[JSON_SESSION], llDevSessionId, strDevGUID))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//����mainctrl�ڵ�
				bool bBroadCast = false;
				if(0 > JsonParser::json2MainCtrl(root[JSON_MAINCTRL], llSessionId, bBroadCast))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_PARSEREQ);

				//��֤dev�Ƿ��½
				P2PUserData	devData;
				if(false == _ctxMgr.isLogin(llDevSessionId, strDevGUID, devData))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);

				//��֤�Ƿ���dev
				if(devData._bMainCtrl)
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_UCTRLID);

				P2PUserData	mainData;
				if(!bBroadCast)
				{
					//��֤�Ƿ���ڰ󶨹�ϵ
					//�ȴӼ�

					//��֤���ض��Ƿ��½
					if(0 > _ctxMgr.query(llSessionId, mainData))
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);

					//��֤�Ƿ������ض�
					if(!mainData._bMainCtrl)
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_MCTRLID);
				}
				else
				{
					//Ⱥ��ģʽ��������֤���ض�
				}

				//�ع�json�Ͱ�ͷ
				BLUEV_UDPAnsHead	ans;
				std::string			strProxyJson;
				if(0 > JsonBuilder::rebuildProxyAns(ans, strProxyJson, root[JSON_PROXYDATA], llDevSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);

				int		nDataLen = sizeof(ans) + strProxyJson.size();
				if(nDataLen >= sizeof(ctx.bnReqBuf))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);

				char	*pData = NULL;

				try
				{
					pData = new char[nDataLen];
					if(pData == NULL)
						throw BluevError(__FILE__, __LINE__, BluevError::ERR_BUILDANS);

					memcpy(pData, &ans, sizeof(ans));
					memcpy(pData + sizeof(ans), strProxyJson.c_str(), strProxyJson.size());
				
					if(!bBroadCast)
					{
						//�ؽ����������豸������Ϣ
						struct	sockaddr_in sin;  
						int		nAddrLen = 0;

						if(0 > ServiceHelper::rebuildConn(mainData._strNATIP, mainData._nNATPort, sin, nAddrLen))
							throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYANS);

						if(mainData._nSocket == INVALID_SOCKET)
							throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYANS);

						//��ʼת��
						if(0 > sendto(conn.nSockfd, pData, nDataLen, 0, (struct sockaddr *)&sin, nAddrLen))
							throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYANS);

						std::cout << "����ת��Ӧ���" << llDevSessionId << "��" << llSessionId << std::endl;

					}
					else
					{
						//Ⱥ��������Ϊ��ͬ�������ض˵��豸״̬��
						std::vector<UserConnInfo>::const_iterator	itr;
						for(itr = devData._vctMCtrl.begin();
							itr != devData._vctMCtrl.end();
							itr++)
						{
							P2PUserData			mainData;
							const UserConnInfo	&connInfo = *itr;

							if(!connInfo._bProxyConn)
								continue;

							if(0 > _ctxMgr.query(connInfo._llSessionId, mainData))
								continue;

							//�����豸
							if(!mainData._bMainCtrl)
								continue;

							try
							{
								//�ؽ����������豸������Ϣ
								struct	sockaddr_in sin;  
								int		nAddrLen = 0;

								if(0 > ServiceHelper::rebuildConn(mainData._strNATIP, mainData._nNATPort, sin, nAddrLen))
									throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYANS);

								if(mainData._nSocket == INVALID_SOCKET)
									throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYANS);

								//��ʼת��
								if(0 > sendto(conn.nSockfd, pData, nDataLen, 0, (struct sockaddr *)&sin, nAddrLen))
									throw BluevError(__FILE__, __LINE__, BluevError::ERR_PROXYANS);

								std::cout << "����ͬ��Ⱥ��Ӧ���" << llDevSessionId << "��" << mainData._llSessionId << std::endl;

							}
							catch(BluevError &e)
							{
								e.print();
								continue;
							}
						}
					}
					
					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}
				}
				catch(BluevError & e)
				{
					if(pData)
					{
						delete[] pData;
						pData = NULL;
					}

					throw e;
				}

				//���ظ�
				//ctx.nAnsLen = 0;

				if(0 > _ctxMgr.keepAlive(llDevSessionId))
					throw BluevError(__FILE__, __LINE__, BluevError::ERR_KEEPALIVE);
			}
			catch(BluevError &e)
			{
				std::cout << "����ͨ��Ӧ���" << llDevSessionId << "��" << llSessionId << "�����쳣" << e.getFileName() << e.getLineName() << std::endl;
				nRetVal = e.getErrNum();
				return (nRetVal == 0) ? -1 : nRetVal;
			}

			return 0;
		}




		///////////////////////////////////////////////////
		int JsonParser::json2UserData(const std::string &strJson, P2PUserData &data) throw() 
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

		int JsonParser::json2Session(const std::string &strJson, __int64 &llSessionId, std::string &strGUID) throw()
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

				if(0 > json2Session(root, llSessionId, strGUID))
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

		int JsonParser::json2Session(const Json::Value &session, __int64 &llSessionId, std::string &strGUID) throw()
		{
			try
			{
				if(session.type() != Json::objectValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &sessionid = session[JSON_SESSIONID];
				if(sessionid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &guid = session[JSON_GUID];
				if(guid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				llSessionId = _atoi64(sessionid.asCString());
				if(llSessionId <= 0)
					throw BluevError(__FILE__, __LINE__);

				strGUID = guid.asCString();
				if(strGUID.empty())
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

		int JsonParser::json2DevInfo(const Json::Value &devinfo, __int64 &llDevSessionId, std::string &strDevId) throw()
		{
			try
			{
				if(devinfo.type() != Json::objectValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &dev_sessionid = devinfo[JSON_SESSIONID];
				if(dev_sessionid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				const Value &dev_devid = devinfo[JSON_DEVID];
				if(dev_devid.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				llDevSessionId = _atoi64(dev_sessionid.asCString());
				if(llDevSessionId <= 0)
					throw BluevError(__FILE__, __LINE__);

				strDevId = dev_devid.asString();
				if(strDevId.empty())
					throw BluevError(__FILE__, __LINE__);

			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int JsonParser::json2DevInfo(const Json::Value &devinfo, __int64 &llDevSessionId, std::string &strDevId, bool	&bSameLan) throw()
		{
			try
			{
				if(devinfo.type() != Json::objectValue)
					throw BluevError(__FILE__, __LINE__);

				if(0 > json2DevInfo(devinfo, llDevSessionId, strDevId))
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
			catch(...)
			{
				return -1;
			}

			return 0;
		}

		int JsonParser::json2MainCtrl(const Json::Value &mainctrl, __int64 &llSessionId, bool &bBroadCast) throw()
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

		int JsonParser::json2MainCtrl(const Json::Value &mainctrl, __int64 &llSessionId, std::string &strIP, int &nPort) throw()
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


		int JsonParser::json2ProxyBinary(const Json::Value &proxydata, char * &pData, int &nDataLen) throw()
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


		//////////////////////////////////////////////////////////
		int JsonBuilder::session2Json(const __int64 &llSessionId, const std::string &strGuid, std::string &strJson)
		{
			if(llSessionId <= 0 || strGuid.empty())
				return -1;

			strJson.clear();

			try
			{
				Json::Value	root;
				char	szBuf[64] = {0};
				sprintf(szBuf, "%I64d", llSessionId);

				root[JSON_SESSIONID] = Value(szBuf);
				root[JSON_GUID] = Value(strGuid);

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
			catch(...)
			{
				return -1;
			}

			return 0;
		}

		int JsonBuilder::session2Json(const __int64 &llSessionId, const std::string &strGuid, Json::Value &session)
		{
			if(llSessionId <= 0 || strGuid.empty())
				return -1;

			try
			{
				char	szBuf[64] = {0};
				sprintf(szBuf, "%I64d", llSessionId);

				session[JSON_SESSIONID] = Value(szBuf);
				session[JSON_GUID] = Value(strGuid);

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

		int JsonBuilder::session2Json(const __int64 &llSessionId, const GUID &stGuid, std::string &strJson)
		{
			if(llSessionId <= 0)
				return -1;

			if(UTIL_GUID::GUID_IsEmpty(stGuid))
				return -1;

			strJson.clear();

			try
			{
				Json::Value	root;
				char	szBuf[64] = {0};
				sprintf(szBuf, "%I64d", llSessionId);

				root[JSON_SESSIONID] = Value(szBuf);

				std::string strGUID;
				if(0 > UTIL_GUID::GUID_ToString(stGuid, strGUID))
					throw BluevError(__FILE__, __LINE__);

				root[JSON_GUID] = Value(strGUID);

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
			catch(...)
			{
				return -1;
			}

			return 0;
		}

		int JsonBuilder::mainctrl2Json(const __int64 &llSessionId, const std::string &strIP, const int &nPort, std::string &strJson)
		{
			if(llSessionId <= 0 || strIP.empty() || nPort <= 0)
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

				char	szBuf[50] = {0};
				sprintf(szBuf, "%lld", llSessionId);
				mainctrl[JSON_SESSIONID] = Value(szBuf);

				root[JSON_MAINCTRL] = mainctrl;

				strJson = writer.write(root);
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

		int JsonBuilder::mainctrl2Json(const __int64 &llSessionId, Json::Value &mainctrl)
		{
			if(llSessionId <= 0)
				return -1;

			mainctrl.clear();

			try
			{
				char	szBuf[50] = {0};
				sprintf(szBuf, "%lld", llSessionId);
				mainctrl[JSON_SESSIONID] = Value(szBuf);
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

		int JsonBuilder::devData2Json(const P2PUserData &devdata, bool bSameLan, Json::Value &dev)
		{
			dev.clear();

			try
			{
				char	szBuf[64] = {0};
				sprintf(szBuf, "%lld", devdata._llSessionId);
				dev[JSON_SESSIONID] = Value(szBuf);
				dev[JSON_DEVID] = Value(devdata._strDeviceId);
				dev[JSON_IP] = Value(devdata._strNATIP);
				dev[JSON_PORT] = Value(devdata._nNATPort);

				//�Ƿ�ͬһ����
				dev[JSON_ISSAMELAN] = Value(bSameLan ? 1 : 0);

				if(bSameLan)
				{
					dev[JSON_LANIP] = Value(devdata._strLANIP);
					dev[JSON_LANPORT] = Value(devdata._nLANPort);
				}

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

		int JsonBuilder::devData2Json(const __int64 &llSessionId, const std::string &strDevId, bool bSameLan, Json::Value &dev)
		{
			if(llSessionId <= 0 || strDevId.empty())
				return -1;

			dev.clear();

			try
			{
				if(0 > devData2Json(llSessionId, strDevId, dev))
					throw BluevError(__FILE__, __LINE__);

				dev[JSON_ISSAMELAN] = Value(bSameLan ? 1 : 0);
			
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

		int JsonBuilder::devData2Json(const __int64 &llSessionId, const std::string &strDevId, Json::Value &dev)
		{
			if(llSessionId <= 0 || strDevId.empty())
				return -1;

			try
			{
				char	szBuf[64] = {0};
				sprintf(szBuf, "%lld", llSessionId);
				dev[JSON_SESSIONID] = Value(szBuf);
				dev[JSON_DEVID] = Value(strDevId);

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

		int JsonBuilder::proxyBinary2json(const char * pData, int nDataLen, Json::Value &root) throw()
		{
			if(pData == NULL || nDataLen <= 0)
				return -1;

			try
			{
				//base64����
				char	*pBase64 = NULL;
				int		nBase64Len = 0;

				if(0 > UTIL_BASE64::encode((char *)pData, nDataLen, &pBase64, &nBase64Len, 0) ||
					pBase64 == NULL || nBase64Len <= 0)
					throw BluevError(__FILE__, __LINE__);

				std::string	strProxyData(pBase64, nBase64Len);
				root[JSON_PROXYDATA] = Json::Value(strProxyData);

				//�ֶ��ͷ��ڴ�
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
			catch(...)
			{
				return -1;
			}

			return 0;
		}
	
		int JsonBuilder::userData2Json(const UserLogin &user, std::string &strJson)
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
					root[JSON_MAC] = Json::Value(user.strId);
				else
					root[JSON_DEVID] = Json::Value(user.strId);

				root[JSON_USERNAME] = Json::Value(user.strUserName);
				root[JSON_PASSWORD] = Json::Value(user.strPassWord);

				strJson = writer.write(root);
				if(strJson.empty())
					return -1;
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

		int JsonBuilder::rebuildProxyReq(BLUEV_UDPReqHead &req, std::string &strJson, const Json::Value &proxydata, const __int64 &llSessionId)
		{
			if(llSessionId <= 0)
				return -1;

			try
			{
				if(proxydata.type() != Json::stringValue)
					throw BluevError(__FILE__, __LINE__);

				req.init();
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PPROXY, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_PROXY, req.nOpCode);

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

				UTIL_CONV::uint2ToChar2(strJson.size(), req.nDataLen);
			}
			catch(BluevError &e)
			{
				e.print();
				return -1;
			}

			return 0;
		}

		int JsonBuilder::rebuildProxyAns(BLUEV_UDPAnsHead &ans, std::string &strJson, const Json::Value &proxydata, const __int64 &llSessionId)
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

		//////////////////////////////////////////////////
		int ServiceHelper::rebuildConn(const std::string &strIP, const int &nPort, struct sockaddr_in &sin, int &nAddrLen) throw()
		{
			if(strIP.empty() || nPort <= 0)
				return -1;

			try
			{
				nAddrLen = sizeof(sin);

				int nIP = 0;
				if(0 > SocketHelper::BLUEV_SOCK_NameToIP((char*)strIP.c_str(), &nIP))
					throw BluevError(__FILE__, __LINE__);

				SocketHelper::BLUEV_SOCK_SetAddrN(&sin, nIP, nPort);
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
	}
}