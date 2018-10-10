#include "bluevp2pclientdef.h"
#include <process.h>

namespace	BLUEV
{
	namespace	BVP2PCLIENTLIB
	{
		using BLUEVNET::SocketHelper;
		using BLUEVNET::BLUEV_UDPReqHead;
		using BLUEVNET::BLUEV_UDPAnsHead;
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::UTIL_GUID;
		using BLUEVBASE::UTIL_NET;
		using BLUEVBASE::AdapterInfo;
		using BLUEVBASE::UTIL_BASE64;
		using BVP2PSVRLIB::JsonBuilder;
		using BVP2PSVRLIB::JsonParser;
		using BVP2PSVRLIB::ServiceHelper;
		using BVP2PSVRLIB::UserLogin;

		int	UDPP2PClient::login()
		{
			char	bnReqBuf[1024] = {0};
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, pReq->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_LOGIN, pReq->nOpCode);
			
			std::string	strLanIp = "127.0.0.1";
			int			nLanPort = port;
			std::string	strMask = "255.255.0.0";
			std::string	strGateWay = "255.255.0.0";
			std::string	strMAC;
			std::string	strDevId;

			//不好，因为重复连接时guid会变。。。还是用线程id吧。。
			if(0)
			{
				//暂时用guid代替
				GUID	stguid;
				UTIL_GUID::WIN_GUID_Gen(stguid);
				UTIL_GUID::GUID_ToString(stguid, strDevId);
			}
			else
			{
				char	szBuf[100] = {0};
				//sprintf(szBuf, "threadid:%d_curtime:%d", GetCurrentThreadId(), GetCurrentTime());
				sprintf(szBuf, "threadid:%d", GetCurrentThreadId());
				strDevId = szBuf;
			}
			
			std::vector<AdapterInfo>	vctNet;
			UTIL_NET::GetMacAddress(vctNet);
			for(int i = 0; i < (int)vctNet.size(); i++)
			{
				if(vctNet[i].strIP == "0.0.0.0")
					continue;

				if(vctNet[i].strGateWayIP.empty() || 
					vctNet[i].strGateWayIP == "0.0.0.0")
					continue;

				strMAC = vctNet[i].strMac;
				strLanIp = vctNet[i].strIP;
				strMask = vctNet[i].strMask;
				strGateWay = vctNet[i].strGateWayIP;
			}

			UserLogin	user;
			user.strLanIp = strLanIp;
			user.nLanPort = nLanPort;
			user.strMask = strMask;
			user.strGateWay = strGateWay;
			user.bMainCtrl = bMainCtrl;
			user.strId = bMainCtrl ? strMAC : strDevId;

			std::string	strJson;
			if(0 > JsonBuilder::userData2Json(user, strJson))
				return -1;

			std::cout << "req login json : " << strJson << std::endl;
			
			memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
			int	nReqLen = sizeof(BLUEV_UDPReqHead) + strJson.size();
			
			return sendToSvr(bnReqBuf, nReqLen);
		}

		int	UDPP2PClient::logout()
		{
			char	bnReqBuf[1024] = {0};
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, pReq->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_LOGOUT, pReq->nOpCode);

			std::string	strJson;
			if(0 > JsonBuilder::session2Json(sessionId, strGuid, strJson))
				return -1;

			std::cout << "req logout json : " << strJson << std::endl;

			memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
			int	nReqLen = sizeof(BLUEV_UDPReqHead) + strJson.size();

			return sendToSvr(bnReqBuf, nReqLen);
		}

		int	UDPP2PClient::getDevInfo()
		{
			char	bnReqBuf[1024] = {0};
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, pReq->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_GETDEVINFO, pReq->nOpCode);

			std::string	strJson;
			if(0 > JsonBuilder::session2Json(sessionId, strGuid, strJson))
				return -1;

			std::cout << "req getdevinfo json : " << strJson << std::endl;

			memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
			int	nReqLen = sizeof(BLUEV_UDPReqHead) + strJson.size();

			return sendToSvr(bnReqBuf, nReqLen);
		}

		int	UDPP2PClient::connDevWithSvr(const P2PDevInfo &dev)
		{
			char	bnReqBuf[1024] = {0};
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, pReq->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_CONNDEV, pReq->nOpCode);

			Json::Value	root, session, devinfo;
			Json::FastWriter	writer;

			if(0 > JsonBuilder::session2Json(sessionId, strGuid, session))
				return -1;

			if(0 > JsonBuilder::devData2Json(dev._llSessionId, dev._strDevId, dev._bSameLan, devinfo))
				return -1;
			
			root[JSON_DEVINFO] = devinfo;
			root[JSON_SESSION] = session;
			
			std::string	strJson = writer.write(root);
			std::cout << "req connDev json : " << strJson << std::endl;

			memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
			int	nReqLen = sizeof(BLUEV_UDPReqHead) + strJson.size();

			return sendToSvr(bnReqBuf, nReqLen);
		}

		int	UDPP2PClient::connDevDirect(const P2PDevInfo &dev)
		{
			char	bnReqBuf[1024] = {0};
			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PCLIENT, pReq->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_CONNDEV, pReq->nOpCode);

			char	szBuf[100] = {0};
			sprintf(szBuf, "%lld", sessionId);

			Json::Value	root;
			Json::FastWriter	writer;
			root[JSON_SESSIONID] = Json::Value(szBuf);
			
			std::string	strJson = writer.write(root);
			if(strJson.empty())
				return -1;

			memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
			int	nReqLen = sizeof(BLUEV_UDPReqHead) + strJson.size();

			std::string	strIp;
			int			nPort = 0;

			//同一内网直连
			if(dev._bSameLan)
			{
				strIp = dev._strLANIP;
				nPort = dev._nLANPort;
			}
			else
			{
				strIp = dev.getIP();
				nPort = dev.getPort();
			}

			int	nRange = this->nRange;
			int nPortSrc = nPort;

			//同一内网不用猜测端口
			if(dev._bSameLan)
				nRange = 0;

			for(nPort = nPortSrc - nRange; nPort <= nPortSrc + nRange; nPort++)
			{
				if(nPort <= 0)
					continue;

				if(dev.getConnected())
					break;

				//连接
				struct sockaddr_in	devSin = {0};  
				int			nAddrLen = 0;
				/*SOCKET		devSocket;
				devSocket = SocketHelper::BLUEV_SOCKUDP_ClientOpen();
				if(devSocket == INVALID_SOCKET)
				return -1;*/

				nAddrLen = sizeof(devSin);

				if(0 > ServiceHelper::rebuildConn(strIp, nPort, devSin, nAddrLen))
					return -1;

				if(0 > sendto(clientSocket, (char *)pReq, nReqLen, 0, (struct sockaddr *)&devSin, nAddrLen))
					return -1;

				//身份证明，发送自己的sessionid或对方的sessionid
			}

			nPort = nPortSrc;

			memset(szBuf, 0, sizeof(szBuf));
			if(dev._bSameLan)
				sprintf(szBuf, "同一内网，直接连接设备:%s:%d\r\n", strIp.c_str(), nPort);
			else
			{
				if(nPort != dev.getPort())
					sprintf(szBuf, "停止主动连接设备:%s:%d，因为发现端口已更新为:%d\r\n", strIp.c_str(), nPort, dev.getPort());
				else
					sprintf(szBuf, "请求协助后主动连接设备:%s:%d\r\n", strIp.c_str(), nPort);
			}

			std::cout << szBuf;

			return 0;
		}

		int UDPP2PClient::getServerInfo(std::string& strSvrIP, int& nSvrPort)
		{
			strSvrIP = this->strSvrAddr;
			nSvrPort = this->svrPort;

			return	0;
		}

		int	UDPP2PClient::sendToSvr(const char *pReq, const int &nReqLen)
		{
			if(pReq == NULL || nReqLen < sizeof(BLUEV_UDPReqHead))
				return -1;

			BLUEV_UDPReqHead	&req = *(BLUEV_UDPReqHead *)pReq;
			if(!req.isValid())
				return -1;

			std::string strSvrIP;
			int			nSvrPort = 0;

			struct sockaddr_in	svrSin = {0};  
			int			nAddrLen = 0;

			nAddrLen = sizeof(svrSin);

			if(0 > getServerInfo(strSvrIP, nSvrPort) ||
				strSvrIP.empty() ||
				nSvrPort <= 0)
				return -1;

			if(0 > ServiceHelper::rebuildConn(strSvrIP, nSvrPort, svrSin, nAddrLen))
				return -1;

			if(0 > sendto(clientSocket, (char *)pReq, nReqLen, 0, (struct sockaddr *)&svrSin, nAddrLen))
				return -1;

			return 0;
		}
		/*
		int UDPP2PClient::proxyConn(const P2PDevInfo &dev, int nOpCode, const std::string &strData)
		{
			if(strData.size() >= 1024 - sizeof(BLUEV_UDPReqHead) || strData.empty() || nOpCode < 0)
				return -1;

			char	bnReqBuf[1024] = {0};
			char	bnProxyBuf[1024] = {0};

			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, pReq->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_PROXY, pReq->nOpCode);

			BLUEV_UDPReqHead	*pPxy = (BLUEV_UDPReqHead *)bnProxyBuf;
			pPxy->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2PPROXY, pPxy->nOpClass);
			UTIL_CONV::uint2ToChar2(nOpCode, pPxy->nOpCode);

			//构造json
			Json::Value	root, session, devinfo;
			Json::FastWriter	writer;

			if(0 > JsonBuilder::session2Json(sessionId, strGuid, session))
				return -1;

			if(0 > JsonBuilder::devData2Json(dev._llSessionId, dev._strDevId, devinfo))
				return -1;
			
			root[JSON_DEVINFO] = devinfo;
			root[JSON_SESSION] = session;

			//构造代理数据
			memcpy(bnProxyBuf + sizeof(BLUEV_UDPReqHead), strData.c_str(), strData.size());
			UTIL_CONV::uint2ToChar2(strData.size(), pPxy->nDataLen);

			char *pData = bnProxyBuf;
			if(0 > JsonBuilder::proxyBinary2json(pData, sizeof(*pPxy) + strData.size(), root))
				return -1;

			std::string	strJson = writer.write(root);
			if(strJson.size() >= sizeof(bnReqBuf) - sizeof(BLUEV_UDPReqHead))
				return -1;

			std::cout << "req proxy json : " << strJson << std::endl;

			memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
			int	nReqLen = sizeof(BLUEV_UDPReqHead) + strJson.size();
			if(nReqLen >= sizeof(bnReqBuf))
				return -1;

			return sendToSvr(bnReqBuf, nReqLen);
		}
		*/
		int UDPP2PClient::proxyRequest(const P2PDevInfo &dev, const std::string &strData)
		{
			if(strData.size() >= 1024 - sizeof(BLUEV_UDPReqHead) || strData.empty())
				return -1;

			char	bnReqBuf[1024] = {0};

			BLUEV_UDPReqHead	*pReq = (BLUEV_UDPReqHead *)bnReqBuf;
			pReq->init();

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_CLASS_P2P, pReq->nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_P2P_PROXY, pReq->nOpCode);

			//构造json
			Json::Value	root, session, devinfo;
			Json::FastWriter	writer;

			if(0 > JsonBuilder::session2Json(sessionId, strGuid, session))
				return -1;

			if(0 > JsonBuilder::devData2Json(dev._llSessionId, dev._strDevId, devinfo))
				return -1;

			root[JSON_DEVINFO] = devinfo;
			root[JSON_SESSION] = session;

			//构造代理数据
			if(0 > JsonBuilder::proxyBinary2json(strData.c_str(), strData.size(), root))
				return -1;

			std::string	strJson = writer.write(root);
			if(strJson.size() >= sizeof(bnReqBuf) - sizeof(BLUEV_UDPReqHead))
				return -1;

			std::cout << "req proxy json : " << strJson << std::endl;

			memcpy(bnReqBuf + sizeof(BLUEV_UDPReqHead), strJson.c_str(), strJson.size());
			UTIL_CONV::uint2ToChar2(strJson.size(), pReq->nDataLen);
			int	nReqLen = sizeof(BLUEV_UDPReqHead) + strJson.size();
			if(nReqLen >= sizeof(bnReqBuf))
				return -1;

			return sendToSvr(bnReqBuf, nReqLen);
		}
	}
}