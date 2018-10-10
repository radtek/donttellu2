#include "bluevp2pclientdef.h"

namespace BLUEV
{
	namespace BVP2PCLIENTLIB
	{
		using BLUEVNET::BLUEV_UDPReqHead;
		using BLUEVNET::BLUEV_UDPAnsHead;
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::UTIL_GUID;
		using BLUEVNET::SocketHelper;
		using BLUEVBASE::UTIL_BASE64;
		using BVP2PSVRLIB::JsonParser;
		using BVP2PSVRLIB::ServiceHelper;
		using BVP2PSVRLIB::JsonBuilder;

		int	UDPP2PClientService::execute(UDPConnection &conn, UDPServiceParamContext &ctx)
		{
			int			retval = -1;
			bool		bReq = false, bWithSvr = false, bProxy = false;
			BLUEV_UDPAnsHead	*pAns = NULL;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;
			
			int	opClass = UTIL_CONV::Char2To_uint2(pReq->nOpClass);
			int	opCode = UTIL_CONV::Char2To_uint2(pReq->nOpCode);

			switch(opClass)
			{
			case BLUEVOpClass::OP_CLASS_P2PANS:
				{
					bReq = false;
					bWithSvr = true;
					bProxy = false;

					//Ӧ�����,���÷���
					ctx.nAnsLen = 0;

					pAns = (BLUEV_UDPAnsHead *)pReq;
					break;
				}
			case BLUEVOpClass::OP_CLASS_P2P:
				{
					bReq = true;
					bWithSvr = true;
					bProxy = false;

					//���������Ҫ����
					ctx.nAnsLen = sizeof(BLUEV_UDPAnsHead);

					break;
				}
			case BLUEVOpClass::OP_CLASS_P2PCLIENT:
				{
					bReq = true;
					bWithSvr = false;
					bProxy = false;

					//���������Ҫ����
					ctx.nAnsLen = sizeof(BLUEV_UDPAnsHead);

					break;
				}
			case BLUEVOpClass::OP_CLASS_P2PCLIENTANS:
				{
					bReq = false;
					bWithSvr = false;
					bProxy = false;

					//Ӧ����񣬲���Ҫ����
					ctx.nAnsLen = 0;

					pAns = (BLUEV_UDPAnsHead *)pReq;

					break;
				}
			case BLUEVOpClass::OP_CLASS_P2PPROXY:
				{
					bReq = true;
					bWithSvr = true;
					bProxy = true;

					//Ӧ����񣬲���Ҫ����
					ctx.nAnsLen = 0;

					break;
				}
			case BLUEVOpClass::OP_CLASS_P2PPROXYANS:
				{
					bReq = false;
					bWithSvr = true;
					bProxy = true;

					//Ӧ����񣬲���Ҫ����
					ctx.nAnsLen = 0;

					break;
				}
			default:
				break;
			}

			try 
			{
				if(!bReq && bWithSvr && !bProxy)
				{	//����˵Ļظ�
					switch(opCode)
					{
					case BLUEVOpClass::OP_P2P_LOGIN:
						retval = loginAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_LOGOUT:
						retval = logoutAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_GETDEVINFO:
						retval = getDevInfoAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_CONNDEV:
						retval = connDevAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_KEEPALIVE:
						//retval = keepAliveAns(conn, ctx);
						break;
					default:
						retval = -1;
					}
				}
				else if(bReq && bWithSvr && !bProxy)
				{
					//����˵�����
					switch(opCode)
					{
					case BLUEVOpClass::OP_P2P_DIG:
						retval = digClient(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_PROXYANS:
						retval = proxyAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_PROXY:
						retval = proxyReq(conn, ctx);
						break;
					default:
						retval = -1;
					}
				}
				else if(bWithSvr && bProxy)
				{
					//����������Ӧ��
					switch(opCode)
					{
					case BLUEVOpClass::OP_P2P_PROXYANS:
						retval = proxyAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_PROXY:
						retval = proxyReq(conn, ctx);
						return retval;
						break;
					default:
						retval = -1;
					}
				}
				else if(bReq && !bWithSvr)
				{
					//�ͻ��˵�����
					switch(opCode)
					{
					case BLUEVOpClass::OP_P2P_DIG:
						retval = digByDev(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_CONNDEV:
						retval = connByClient(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_KEEPALIVE:
						retval = keepAliveByClient(conn, ctx);
						break;
					default:
						retval = -1;
					}
				}
				else if(!bReq && !bWithSvr)
				{
					//�ͻ��˵�Ӧ��
					switch(opCode)
					{
					case BLUEVOpClass::OP_P2P_DIG:
						retval = digClientAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_CONNDEV:
						retval = connDevDirectAns(conn, ctx);
						break;
					case BLUEVOpClass::OP_P2P_KEEPALIVE:
						//retval = keepAliveAns(conn, ctx);
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
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENTANS, pAns->nOpClass);
			UTIL_CONV::uint2ToChar2(opCode, pAns->nOpCode);

			UTIL_CONV::uint2ToChar2(retval, pAns->nRetVal);

			if(ctx.nAnsLen > sizeof(BLUEV_UDPAnsHead))
				UTIL_CONV::uint2ToChar2(ctx.nAnsLen - sizeof(BLUEV_UDPAnsHead), pAns->nDataLen);

			return	retval;
		}

		int UDPP2PClientService::loginAns(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			//��¼�ɹ�
			BLUEV_UDPAnsHead	*pAns = (BLUEV_UDPAnsHead *)ctx.bnReqBuf;
			if(UTIL_CONV::Char2To_int2(pAns->nRetVal) < 0)
				return -1;

			//parse json
			std::string		strJson((char *)(ctx.bnReqBuf + sizeof(BLUEV_UDPAnsHead)), ctx.nReqLen - sizeof(BLUEV_UDPAnsHead));
			if(strJson.empty())
				return -1;
			
			std::cout << "receive login json:" << strJson << std::endl;

			__int64 llSessionId = 0;
			std::string strGuid;
			if(0 > JsonParser::json2Session(strJson, llSessionId, strGuid))
				return -1;

			pClient->sessionId = llSessionId;
			pClient->strGuid = strGuid;

			//���ظ�
			ctx.nAnsLen = 0;

			std::cout << "��½�ɹ���session id:" << llSessionId <<std::endl;

			return 0;
		}

		int UDPP2PClientService::logoutAns(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			//���ظ�
			ctx.nAnsLen = 0;

			BLUEV_UDPAnsHead	*pAns = (BLUEV_UDPAnsHead *)ctx.bnReqBuf;
			if(UTIL_CONV::Char2To_int2(pAns->nRetVal) < 0)
				return -1;

			//�ǳ��ɹ�
			pClient->sessionId = 0;

			std::cout << "�ǳ��ɹ�" <<std::endl;

			return 0;
		}

		int json2DevInfo(P2PDevInfo &devInfo, const Json::Value &dev) throw()
		{
			try
			{
				if(!dev.isObject())
					throw std::exception(__FILE__, __LINE__);

				const Json::Value	&sessionid = dev[JSON_SESSIONID];
				if(sessionid.type() != Json::stringValue)
					throw std::exception(__FILE__, __LINE__);

				const Json::Value	&devid = dev[JSON_DEVID];
				if(devid.type() != Json::stringValue)
					throw std::exception(__FILE__, __LINE__);

				const Json::Value	&ip = dev[JSON_IP];
				if(ip.type() != Json::stringValue)
					throw std::exception(__FILE__, __LINE__);

				const Json::Value	&port = dev[JSON_PORT];
				if(port.type() != Json::intValue)
					throw std::exception(__FILE__, __LINE__);

				const Json::Value	&issamelan = dev[JSON_ISSAMELAN];
				if(issamelan.type() != Json::intValue)
					throw std::exception(__FILE__, __LINE__);

				bool	bSameLan = issamelan.asInt() ? true : false;
				if(bSameLan)
				{
					const Json::Value	&lanip = dev[JSON_LANIP];
					if(lanip.type() != Json::stringValue)
						throw std::exception(__FILE__, __LINE__);

					const Json::Value	&lanport = dev[JSON_LANPORT];
					if(lanport.type() != Json::intValue)
						throw std::exception(__FILE__, __LINE__);

					std::string strIP = lanip.asString();
					if(strIP.empty())
						throw std::exception(__FILE__, __LINE__);

					int	nPort = lanport.asInt();
					if(nPort <= 0)
						throw std::exception(__FILE__, __LINE__);

					devInfo._bSameLan = bSameLan;
					devInfo._strLANIP = strIP;
					devInfo._nLANPort = nPort;
				}

				__int64	llSessionId = _atoi64(sessionid.asCString());
				if(llSessionId <= 0)
					throw std::exception(__FILE__, __LINE__);

				std::string strDevId = devid.asString();
				if(strDevId.empty())
					throw std::exception(__FILE__, __LINE__);

				std::string strIP = ip.asString();
				if(strIP.empty())
					throw std::exception(__FILE__, __LINE__);

				/*if(!bSameLan)
				{
					if(strIP == "127.0.0.1")
						strIP = pClient->strSvrAddr;
				}*/

				int	nPort = port.asInt();
				if(nPort <= 0)
					throw std::exception(__FILE__, __LINE__);

				devInfo._llSessionId = llSessionId;
				devInfo._strDevId = strDevId;
				devInfo.setIP(strIP);
				devInfo.setPort(nPort);

			}
			catch(std::exception &e)
			{
				e.what();
				return -1;
			}
			catch(...)
			{
				return -1;
			}

			return 0;
		}

		int UDPP2PClientService::getDevInfoAns(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			if(!pClient->bMainCtrl)
				return -1;

			BLUEV_UDPAnsHead	*pAns = (BLUEV_UDPAnsHead *)ctx.bnReqBuf;
			if(UTIL_CONV::Char2To_int2(pAns->nRetVal) < 0)
				return -1;

			//parse json
			std::string		strJson((char *)(ctx.bnReqBuf + sizeof(BLUEV_UDPAnsHead)), ctx.nReqLen - sizeof(BLUEV_UDPAnsHead));
			if(strJson.empty())
				return -1;

			std::cout << "receive getDevInfo json:" << strJson << std::endl;

			Json::Value		root;
			Json::Reader	reader;

			if(false == reader.parse(strJson, root))
				return -1;

			Json::Value	&devs = root[JSON_DEVS];
			if(!devs.isArray() || devs.size() < 1)
				return -1;

			std::map<__int64, P2PDevInfo *> mapDevs;
			for(int i = 0; i < (int)devs.size(); ++i)
			{
				P2PDevInfo	*pDev = NULL;
				try
				{
					pDev = new P2PDevInfo;
					if(pDev == NULL)
						throw std::exception(__FILE__, __LINE__);

					if(0 > json2DevInfo(*pDev, devs[i]))
						throw std::exception(__FILE__, __LINE__);;

					//�����ɣ�
					if(!pDev->_bSameLan)
					{
						if(pDev->getIP() == "127.0.0.1")
							pDev->setIP(pClient->strSvrAddr);
					}

					mapDevs[pDev->_llSessionId] = pDev;
					pDev = NULL;
				}
				catch(std::exception &e)
				{
					e.what();
					if(pDev)
					{
						delete pDev;
						pDev = NULL;
					}
					continue;
				}
			}

			//if(mapDevs.size() <= 0)
			//	return -1;

			pClient->mapDevs.swap(mapDevs);

			std::map<__int64, P2PDevInfo *>::const_iterator itr;
			for(itr = mapDevs.begin();
				itr != mapDevs.end();
				itr++)
			{
				if(itr->second)
					delete itr->second;
			}

			mapDevs.clear();

			//���ظ�
			ctx.nAnsLen = 0;

			std::cout << "��ȡ�豸��Ϣ�ɹ�������:" << devs.size() <<std::endl;

			return 0;
		}

		int UDPP2PClientService::connDevAns(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			if(!pClient->bMainCtrl)
				return -1;

			//���ظ�
			ctx.nAnsLen = 0;

			BLUEV_UDPAnsHead	*pAns = (BLUEV_UDPAnsHead *)ctx.bnReqBuf;
			if(UTIL_CONV::Char2To_int2(pAns->nRetVal) < 0)
				return -1;

			//����˽�������
			//����Э����
			std::cout << "����������������Э����" << std::endl;

			return 0;

		}
	
		int UDPP2PClientService::digClient(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			if(pClient->bMainCtrl)
				return -1;

			__int64				llSessionId = 0;
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)ctx.bnReqBuf;

			//parse json
			std::string		strJson((char *)(ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead)), ctx.nReqLen - sizeof(BLUEV_UDPReqHead));
			if(strJson.empty())
				return -1;

			std::cout << "receive digClient json:" << strJson << std::endl;

			Json::Value		root;
			Json::Reader	reader;

			if(false == reader.parse(strJson, root))
				return -1;

			std::string	strIP;
			int	nPort = 0;
			if(0 > JsonParser::json2MainCtrl(root[JSON_MAINCTRL], llSessionId, strIP, nPort))
				return -1;

			//˵���豸�ڷ������ϣ��������޷���ȡ����IP
			if(strIP == "127.0.0.1")
				strIP = pClient->strSvrAddr;

			//����������б�
			P2PTaskInfo			*pTaskInfoNew = NULL;
			const P2PTaskInfo	*pTaskInfo = NULL;
			
			try
			{
				pTaskInfoNew = new P2PTaskInfo;
				if(pTaskInfoNew == NULL)
					throw std::exception(__FILE__, __LINE__);

				pTaskInfoNew->_llSessionId = llSessionId;
				pTaskInfoNew->setIP(strIP);
				pTaskInfoNew->setPort(nPort);

				pClient->lock.lock();
				std::map<__int64, P2PTaskInfo *>::const_iterator	itr;
				itr = pClient->mapTask.find(llSessionId);
				if(itr == pClient->mapTask.end())
				{
					pClient->mapTask[llSessionId] = pTaskInfoNew;
					pTaskInfo = pTaskInfoNew;
				}
				else
				{	
					//�����Ѵ���
					delete pTaskInfoNew;
					pTaskInfo = itr->second;
				}
				pClient->lock.unLock();

				pTaskInfoNew = NULL;
			}
			catch(std::exception &e)
			{
				e.what();
				if(pTaskInfoNew)
				{
					delete pTaskInfoNew;
					pTaskInfoNew = NULL;
				}
			}

			if(pTaskInfo->getConnected())
			{
				//��������֮ǰ����ͨ
				ctx.nAnsLen = 0;

				if(pTaskInfo->getPort() != nPort)
					std::cout << "��������֮ǰ����ͨ��ip��" << strIP << " �Ҷ˿��Ѵ�" << nPort << "����Ϊ" << pTaskInfo->getPort() << std::endl;
				else
					std::cout << "��������֮ǰ����ͨ��ip��" << strIP << " port:" << nPort << std::endl;

				return 0;
			}

			//�Գ���·����Ҫ�²�˿�
			//ͬһ����ʱ���ã��Ҳ�����ô˺���
			int	nRange = pClient->nRange;
			int nPortSrc = nPort;
			for(nPort = nPortSrc - nRange; nPort <= nPortSrc + nRange; nPort++)
			{
				if(nPort <= 0)
					continue;

				if(pTaskInfo->getConnected())
					break;

				//����
				struct sockaddr_in	sin = {0};
				int		nAddrLen = 0;

				if(0 > ServiceHelper::rebuildConn(strIP, nPort, sin, nAddrLen))
					return -1;

				BLUEV_UDPReqHead	req;
				req.init();
			
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENT, req.nOpClass);
				UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_DIG, req.nOpCode);

				// ����һ�����ݰ������д�   
				if(0 > sendto(conn.nSockfd, (char *)&req, sizeof(BLUEV_UDPReqHead), 0, (struct sockaddr *)&sin, nAddrLen))
					return -1;

				//���֤���������Լ���sessionid��Է���sessionid
			}

			nPort = nPortSrc;

			// �ɲ��ػظ������ض����ղ��������������
			// ֱ������򶴳ɹ���
			ctx.nAnsLen = 0;

			std::cout << "���ʹ����ݰ���ip��" << strIP << " port:" << nPort << std::endl;

			return 0;
		}
	
		int UDPP2PClientService::digByDev(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			if(!pClient->bMainCtrl)
				return -1;

			//�յ�����Ϣ���ɹ���

			//��֤������� sessionid

			std::string	strIP;
			int	nPort = 0;
			strIP = inet_ntoa(conn.fromaddr.sin_addr);
			nPort = ntohs(conn.fromaddr.sin_port);
			
			//////////////////////////////////////////////////
			std::map<__int64, P2PDevInfo *>::const_iterator itr;
			for(itr = pClient->mapDevs.begin();
				itr != pClient->mapDevs.end();
				itr++)
			{
				P2PDevInfo	&dev = *(itr->second);
				if(dev._bSameLan)
				{
					//ͬһ������Ӧ��Э���򶴣���Ӧ���յ�
					//if(itr->second._strLANIP != strIP)
						continue;
				}
				else
				{
					if(dev.getIP() != strIP)
						continue;
				}

				//�޸Ķ˿ں�
				if(nPort != dev.getPort())
				{
					dev.setPort(nPort);
					std::cout << "�˿��Ѹ���\r\n";
				}

				dev.setConnected(true);
				break;
			}

			char	szBuf[100] = {0};
			sprintf(szBuf,  "�յ������ݰ�,���ӳɹ�����ip��%s port:%d\r\n", strIP.c_str(), nPort);
			std::cout << szBuf;

			/*__int64	llSessionId = 0;
			itr = pClient->mapDevs.find(llSessionId);
			if(itr == pClient->mapDevs.end())
				return -1;

			itr->second._bConnected = true;*/

			//֪ͨ������

			//���豸ά������

			//�ظ�һ����ͷ
			ctx.nAnsLen = sizeof(BLUEV_UDPAnsHead);
			BLUEV_UDPAnsHead	*pAns = (BLUEV_UDPAnsHead *)ctx.bnAnsBuf;
			pAns->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENTANS, pAns->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_DIG, pAns->nOpCode);

			return 0;
		}
	
		int UDPP2PClientService::connByClient(UDPConnection& conn, UDPServiceParamContext& ctx, __int64 *pSessionId)
		{
			if(pClient->bMainCtrl)
				return -1;

			//�յ�������Ϣ���ɹ���

			//��֤��������� sessionid

			std::string	strIP;
			int	nPort = 0;
			strIP = inet_ntoa(conn.fromaddr.sin_addr);
			nPort = ntohs(conn.fromaddr.sin_port);

			//parse json
			BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)ctx.bnReqBuf;
			if(!req.isValid())
				return -1;

			std::string	strJson((char *)ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead), ctx.nReqLen - sizeof(BLUEV_UDPReqHead));
			if(strJson.empty())
				return -1;

			Json::Reader	reader;
			Json::Value		root;

			if(!reader.parse(strJson, root))
				return -1;

			Json::Value	&sessionid = root[JSON_SESSIONID];
			if(sessionid.type() != Json::stringValue)
				return -1;

			__int64	llSessionId = _atoi64(sessionid.asCString());
			if(llSessionId <= 0)
				return -1;

			if(pSessionId)
				*pSessionId = llSessionId;

			P2PTaskInfo	*pMainCtrl = NULL;
			//////////////////////////////////////////
			pClient->lock.lock();
			std::map<__int64, P2PTaskInfo *>::const_iterator itr;
			itr = pClient->mapTask.find(llSessionId);
			if(itr == pClient->mapTask.end())
			{
				//û�д����񣬿����������������ڷ���˵���
				std::cout << "connByClient ���ڷ����digClient����\r\n";
				pMainCtrl = new P2PTaskInfo;
				if(pMainCtrl == NULL)
					return -1;

				pMainCtrl->_llSessionId = llSessionId;
				pMainCtrl->setIP(strIP);
				pMainCtrl->setPort(nPort);
				pMainCtrl->setConnected(true);

				pClient->mapTask[llSessionId] = pMainCtrl;
				pMainCtrl = NULL;
			}
			else
			{/*
				//���д�����ֻ����
				for(itr = pClient->mapTask.begin();
					itr != pClient->mapTask.end();
					itr++)
				{
					P2PTaskInfo	&mainCtrl = *(itr->second);
					if(mainCtrl.getIP() != strIP)
						continue;

					//�޸Ķ˿ں�
					if(nPort != mainCtrl.getPort())
					{
						mainCtrl.setPort(nPort);
						std::cout << "�˿��Ѹ���\r\n";
					}

					mainCtrl.setConnected(true);
					break;
				}
				*/
				P2PTaskInfo	&mainCtrl = *(itr->second);

				//�޸Ķ˿ں�
				if(nPort != mainCtrl.getPort())
				{
					mainCtrl.setPort(nPort);
					std::cout << "�˿��Ѹ���\r\n";
				}

				mainCtrl.setConnected(true);
			}
			pClient->lock.unLock();
			/////////////////////////////////////////////////////
			if(pMainCtrl)
			{
				delete pMainCtrl;
				pMainCtrl = NULL;
			}

			char	szBuf[100] = {0};
			sprintf(szBuf,  "�յ������ݰ�,���ӳɹ�����ip��%s port:%d\r\n", strIP.c_str(), nPort);
			std::cout << szBuf;

			//��֤�Ƿ�ͬһ����
			//�Լ��ж�IP
			//���������ض��ṩjson��Ϣ

			//֪ͨ������

			//�����ض�ά������

			//�ظ�һ����ͷ
			ctx.nAnsLen = sizeof(BLUEV_UDPAnsHead);
			BLUEV_UDPAnsHead	*pAns = (BLUEV_UDPAnsHead *)ctx.bnAnsBuf;
			pAns->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENTANS, pAns->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_CONNDEV, pAns->nOpCode);


			return 0;
		}

		int UDPP2PClientService::keepAliveByClient(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			if(pClient->bMainCtrl)
				return -1;

			//�յ���������

			//��֤��������� sessionid

			std::string	strIP;
			int	nPort = 0;
			strIP = inet_ntoa(conn.fromaddr.sin_addr);
			nPort = ntohs(conn.fromaddr.sin_port);

			//parse json
			BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)ctx.bnReqBuf;
			if(!req.isValid())
				return -1;

			std::string	strJson((char *)ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead), ctx.nReqLen - sizeof(BLUEV_UDPReqHead));
			if(strJson.empty())
				return -1;

			Json::Reader	reader;
			Json::Value		root;

			if(!reader.parse(strJson, root))
				return -1;

			Json::Value	&sessionid = root[JSON_SESSIONID];
			if(sessionid.type() != Json::stringValue)
				return -1;

			__int64	llSessionId = _atoi64(sessionid.asCString());
			if(llSessionId <= 0)
				return -1;

			P2PTaskInfo	*pMainCtrl = NULL;
			//////////////////////////////////////////
			pClient->lock.lock();
			std::map<__int64, P2PTaskInfo *>::const_iterator itr;
			itr = pClient->mapTask.find(llSessionId);
			if(itr == pClient->mapTask.end())
			{
				//û�д����񣬲�������������ֱ������
				std::cout << "error��keepAlive���ض�������ʧЧ\r\n";
				pClient->lock.unLock();
				return -1;
			}
			else
			{
				//���д�����ֻ����
				for(itr = pClient->mapTask.begin();
					itr != pClient->mapTask.end();
					itr++)
				{
					P2PTaskInfo	&mainCtrl = *(itr->second);
					if(mainCtrl.getIP() != strIP)
						continue;

					//�������������޸Ķ˿ں�
					/*if(nPort != mainCtrl.getPort())
					{
						mainCtrl.setPort(nPort);
						std::cout << "�˿��Ѹ���\r\n";
					}*/

					//mainCtrl.setConnected(true);
					//���ø���ʱ��

					break;
				}
			}
			pClient->lock.unLock();
			/////////////////////////////////////////////////////

			char	szBuf[100] = {0};
			sprintf(szBuf,  "�յ��������ݰ�sessionid:%lld��ip��%s port:%d\r\n", llSessionId, strIP.c_str(), nPort);
			std::cout << szBuf;

			//�ظ�һ����ͷ
			ctx.nAnsLen = sizeof(BLUEV_UDPAnsHead);
			BLUEV_UDPAnsHead	*pAns = (BLUEV_UDPAnsHead *)ctx.bnAnsBuf;
			pAns->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENTANS, pAns->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_KEEPALIVE, pAns->nOpCode);

			return 0;
		}

		int UDPP2PClientService::digClientAns(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			if(pClient->bMainCtrl)
				return -1;

			//���ض˷����յ��򶴰���

			//��֤�ظ������ sessionid

			std::string	strIP;
			int	nPort = 0;
			strIP = inet_ntoa(conn.fromaddr.sin_addr);
			nPort = ntohs(conn.fromaddr.sin_port);

			///////////////////////////////////////
			pClient->lock.lock();
			P2PTaskInfo	*pTaskInfo = NULL;
			std::map<__int64, P2PTaskInfo *>::const_iterator itr;
			for(itr = pClient->mapTask.begin();
				itr != pClient->mapTask.end();
				itr++)
			{
				pTaskInfo = itr->second;
				if(pTaskInfo->getIP() != strIP)
					continue;

				//�޸Ķ˿ں�
				if(nPort != pTaskInfo->getPort())
				{
					pTaskInfo->setPort(nPort);
					std::cout << "�˿��Ѹ���\r\n";
				}

				pTaskInfo->setConnected(true);
				break;
			}
			pTaskInfo = NULL;
			pClient->lock.unLock();

			//δ�ҵ�ƥ������񣬿�����ʧЧ

			char	szBuf[100] = {0};
			sprintf(szBuf,  "���ض˷����յ������ݰ������ӳɹ�����ip��%s port:%d\r\n", strIP.c_str(), nPort);
			std::cout << szBuf;

			//֪ͨ������

			//�����ض�ά������

			//���ظ�
			ctx.nAnsLen = 0;

			return 0;
		}

		int UDPP2PClientService::connDevDirectAns(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			if(!pClient->bMainCtrl)
				return -1;

			//�豸�����յ�ֱ����Ϣ��

			//��֤�豸��� sessionid/devid

			std::string	strIP;
			int	nPort = 0;
			strIP = inet_ntoa(conn.fromaddr.sin_addr);
			nPort = ntohs(conn.fromaddr.sin_port);

			//////////////////////////////////////
			std::map<__int64, P2PDevInfo *>::const_iterator itr;
			for(itr = pClient->mapDevs.begin();
				itr != pClient->mapDevs.end();
				itr++)
			{
				P2PDevInfo	&dev = *(itr->second);
				if(dev._bSameLan)
				{
					if(dev._strLANIP != strIP)
						continue;
				}
				else
				{
					if(dev.getIP() != strIP)
						continue;
				}

				//�޸Ķ˿ں�
				if(nPort != dev.getPort())
				{
					dev.setPort(nPort);
					std::cout << "�˿��Ѹ���\r\n";
				}

				dev.setConnected(true);
				break;
			}

			char	szBuf[100] = {0};
			sprintf(szBuf,  "�豸�����յ�ֱ�����ݰ�,���ӳɹ�����ip��%s port:%d\r\n", strIP.c_str(), nPort);
			std::cout << szBuf;

			//֪ͨ���������Ժ�ᣬ�������ڱ�������

			//���豸ά������

			//���ظ�
			ctx.nAnsLen = 0;

			return 0;
		}
	
		int UDPP2PClientService::reqProxy(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			__int64		llSessionId = 0;
			//parse json
			BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)ctx.bnReqBuf;
			if(!req.isValid())
				return -1;

			std::string	strJson((char *)ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead), ctx.nReqLen - sizeof(BLUEV_UDPReqHead));
			if(strJson.empty())
				return -1;

			Json::Reader	reader;
			Json::Value		root;

			if(!reader.parse(strJson, root))
				return -1;

			const Json::Value &sessionid = root[JSON_SESSIONID];

			llSessionId = _atoi64(sessionid.asCString());
			
			//base64 to binary
			char	*pData = NULL;
			int		nDataLen = 0;

			try
			{
				if(0 > JsonParser::json2ProxyBinary(root[JSON_PROXYDATA], pData, nDataLen))
					throw std::exception(__FILE__, __LINE__);
			
				BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)pData;

				if(pData)
				{
					delete[] pData;
					pData = NULL;
				}
			}
			catch(std::exception & e)
			{
				if(pData)
				{
					delete[] pData;
					pData = NULL;
				}

				return -1;
			}

			return 0;
		}

		//�յ���������
		int UDPP2PClientService::proxyReq(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			__int64		llSessionId = 0;
			//parse json
			BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)ctx.bnReqBuf;

			std::string	strJson((char *)ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead), ctx.nReqLen - sizeof(BLUEV_UDPReqHead));
			if(strJson.empty())
				return -1;

			std::cout << "receive proxyreq:" << strJson << std::endl;

			Json::Reader	reader;
			Json::Value		root;

			if(!reader.parse(strJson, root))
				return -1;

			const Json::Value &sessionid = root[JSON_SESSIONID];
			const Json::Value &proxydata = root[JSON_PROXYDATA];

			llSessionId = _atoi64(sessionid.asCString());
			std::string strProxyData = proxydata.asCString();

			//����Ӧ��ͷ
			BLUEV_UDPAnsHead &ans = *(BLUEV_UDPAnsHead *)ctx.bnAnsBuf;
			ans.init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, ans.nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_PROXYANS, ans.nOpCode);

			//build json
			{
				Json::Value	root, mainctrl, session;
				char	szBuf[64] = {0};
				sprintf(szBuf, "%lld", llSessionId);
				mainctrl[JSON_SESSIONID] = Json::Value(szBuf);
				mainctrl[JSON_BROADCAST] = Json::Value(1);
				
				sprintf(szBuf, "%lld", pClient->sessionId);
				session[JSON_SESSIONID] = Json::Value(szBuf);
				session[JSON_GUID] = Json::Value(pClient->strGuid);

				root[JSON_MAINCTRL] = mainctrl;
				root[JSON_PROXYDATA] = Json::Value(strProxyData);
				root[JSON_SESSION] = session;

				Json::FastWriter writer;
				std::string strJson = writer.write(root);

				std::cout << "send proxyans json:" << strJson << std::endl;

				memcpy(ctx.bnAnsBuf + sizeof(ans), strJson.c_str(), strJson.size());
				UTIL_CONV::uint2ToChar2(strJson.size(), ans.nDataLen);
				ctx.nAnsLen = strJson.size() + sizeof(ans);
			}
			
			return 0;
		}

		//�յ�������
		int UDPP2PClientService::proxyAns(UDPConnection& conn, UDPServiceParamContext& ctx)
		{
			__int64		llSessionId = 0;
			//parse json
			BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)ctx.bnReqBuf;

			std::string	strJson((char *)ctx.bnReqBuf + sizeof(BLUEV_UDPReqHead), ctx.nReqLen - sizeof(BLUEV_UDPReqHead));
			if(strJson.empty())
				return -1;

			std::cout << "receive proxyans:" << strJson << std::endl;

			Json::Reader	reader;
			Json::Value		root;

			if(!reader.parse(strJson, root))
				return -1;

			const Json::Value &sessionid = root[JSON_SESSIONID];

			llSessionId = _atoi64(sessionid.asCString());

			//base64 to binary
			char	*pData = NULL;
			int		nDataLen = 0;

			try
			{
				if(0 > JsonParser::json2ProxyBinary(root[JSON_PROXYDATA], pData, nDataLen))
					throw std::exception(__FILE__, __LINE__);

				std::string strAns(pData, nDataLen);
				std::cout << strAns << std::endl;

				if(pData)
				{
					delete[] pData;
					pData = NULL;
				}
			}
			catch(std::exception & e)
			{
				e.what();

				if(pData)
				{
					delete[] pData;
					pData = NULL;
				}

				return -1;
			}

			return 0;
		}
	}
}
