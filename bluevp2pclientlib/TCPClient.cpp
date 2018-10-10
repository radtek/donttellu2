#include "bluevp2pclientdef.h"

#include <MSTcpIP.h>

namespace	BLUEV
{
	namespace	BVP2PCLIENTLIB
	{
		int TCPClient::start()
		{
			int nIP;
			if(0 > SocketHelper::BLUEV_SOCK_NameToIP((char*)_strSvrAddr.c_str(), &nIP))
				return -1;

			SocketHelper::BLUEV_SOCK_SetAddrN(&_sinAddr, nIP, _nSvrPort);

			int	nSecond = 10000;	//ms
			setsockopt(_nSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nSecond, sizeof(int));
			setsockopt(_nSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nSecond, sizeof(int));

			if(0)
			{
				/* keepalive */
				int bKeepAlive = 1;
				setsockopt(_nSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&bKeepAlive, sizeof(int));

				struct tcp_keepalive keepin;
				struct tcp_keepalive keepout;

				keepin.keepaliveinterval = 10000;	//10s 每10S发送1包探测报文，发5次没有回应，就断开
				keepin.keepalivetime = 3000 * 10;	//30s 超过30S没有数据，就发送探测包
				keepin.onoff = 1;

				int	nBytes = 0;
				WSAIoctl(_nSocket, SIO_KEEPALIVE_VALS, &keepin, 
					sizeof(keepin), &keepout, sizeof(keepout),
					(LPDWORD)&nBytes, NULL, NULL);
			}

			//阻塞
			int nRet = connect(_nSocket, (struct sockaddr *)&_sinAddr, sizeof(_sinAddr));
			if(nRet < 0)
				return nRet;

			_bPrepared = true;

			return 0;
		}

		int	TCPClient::getDevInfo()
		{
			LockerEx	lock(_stCrit);

			BLUEV_NetReqHead	req;

			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCPCLASS_P2P, req.nOpClass);
			UTIL_CONV::uint2ToChar2(BLUEVOpClass::OP_TCP_GETDEVINFO, req.nOpCode);
			//req.reuseConnection = 1;
			req.longConnection = 1;

			std::string	strJson;
			if(0 > TCPJsonBuilder::session2Json(_strSessionId, strJson))
				return -1;

			std::cout << "req getdevinfo json : " << strJson << std::endl;

			UTIL_CONV::uint4ToChar4(strJson.size(), req.nDataLen);

			if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, &req, sizeof(req)))
				return -1;

			if(0 > SocketHelper::BLUEV_SOCK_Send(_nSocket, (void *)strJson.c_str(), strJson.size()))
				return -1;

			BLUEV_NetAnsHead	ans;
			if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, &ans, sizeof(ans)))
				return -1;

			if(!ans.isValid())
				return -1;

			int nRetVal = UTIL_CONV::Char2To_int2(ans.nRetVal);
			std::cout << "getDevInfo ret:" << nRetVal << std::endl;
			if(nRetVal < 0)
				return nRetVal;

			int nDataLen = UTIL_CONV::Char4To_uint4(ans.nDataLen);
			if(nDataLen <= 0 || nDataLen >= 10*1024*1024)	//10M
				return -1;

			char	*pData = new char[nDataLen];
			if(pData == NULL)
				return -1;

			if(0 > SocketHelper::BLUEV_SOCK_Recv(_nSocket, pData, nDataLen))
			{
				delete[] pData;
				return -1;
			}

			strJson.assign(pData, nDataLen);
			if(strJson.empty())
			{
				delete[] pData;
				return -1;
			}

			std::cout << "receive getDevInfo json:" << strJson << std::endl;

			Json::Value		root;
			Json::Reader	reader;

			if(false == reader.parse(strJson, root))
				return -1;

			Json::Value	&devs = root[JSON_DEVS];
			if(!devs.isArray() || devs.size() < 1)
				return -1;

			map_devs_t mapDevs;
			for(int i = 0; i < (int)devs.size(); ++i)
			{
				TCPDevInfo	*pDev = NULL;
				try
				{
					pDev = new TCPDevInfo;
					if(pDev == NULL)
						throw std::exception(__FILE__, __LINE__);

					if(0 > TCPjson2DevInfo(*pDev, devs[i]))
						throw std::exception(__FILE__, __LINE__);;

					//修正ＩＰ
					if(!pDev->_bSameLan)
					{
						if(pDev->_strIP == "127.0.0.1")
							pDev->_strIP = (_strSvrAddr);
					}

					mapDevs[pDev->_strSessionId] = pDev;
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

			{
				//RWLockerEx	lock(this->_rwLock, false);
				_mapDevs.swap(mapDevs);
			}
			
			map_devs_t::const_iterator itr;
			for(itr = mapDevs.begin();
				itr != mapDevs.end();
				itr++)
			{
				if(itr->second)
					delete itr->second;
			}

			mapDevs.clear();

			std::cout << "获取设备信息成功，个数:" << devs.size() <<std::endl;

			return 0;
		}

		int TCPClient::TCPjson2DevInfo(TCPDevInfo &devInfo, const Json::Value &dev) throw()
		{
			try
			{
				if(!dev.isObject())
					throw std::exception(__FILE__, __LINE__);

				const Json::Value	&sessionid = dev[JSON_SESSIONID];
				if(sessionid.type() != Json::stringValue)
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

				const Json::Value	&bupnp = dev[JSON_ISUPNP];
				if(bupnp.type() != Json::intValue)
					throw std::exception(__FILE__, __LINE__);

				devInfo._bUPNP = bupnp.asInt() ? true : false;

				if(devInfo._bUPNP)
				{
					const Json::Value	&upnpip = dev[JSON_UPNPIP];
					if(upnpip.type() != Json::stringValue)
						throw std::exception(__FILE__, __LINE__);

					const Json::Value	&upnpport = dev[JSON_UPNPPORT];
					if(upnpport.type() != Json::intValue)
						throw std::exception(__FILE__, __LINE__);

					devInfo._strUPNPIP = upnpip.asCString();
					devInfo._nUPNPPort = upnpport.asInt();
				}

				std::string	strSessionId = sessionid.asCString();
				if(strSessionId.empty())
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

				devInfo._strSessionId = strSessionId;
				devInfo._strIP = (strIP);
				devInfo._nPort = (nPort);

			}
			catch(std::exception &e)
			{
				e.what();
				return -1;
			}

			return 0;
		}
	}
}