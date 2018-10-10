#include "bluevp2psvrdef.h"

namespace BLUEV{
	namespace BVP2PSVRLIB{

		using BLUEVBASE::BluevError;
		int TCPService::execute(BLUEVConnection &conn, ServiceParamContext &ctx)
		{
			int retval = -1;

			try
			{
				if((retval = doWork(conn, ctx)) < 0)
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				e.print();
			}

			return retval;				
		}


		int TCPUserCtxMgr::login(TCPUserData &userdata)
		{
			//内存池
			TCPUserData	*pUserdata = new TCPUserData;
			if(pUserdata == NULL)
				return -1;

			struct tm	*t = NULL;
			time_t		tt;
				
			time(&tt);
			t = localtime(&tt);

			userdata._tmCreate = *t;
			userdata._tmUpdate = *t;

			*pUserdata = userdata;

			{
				RWLockerEx lock(_rwLock, false);
				//_mapUsers[pUserdata->_strSessionId] = pUserdata;
				_mapUsers.insert(std::make_pair(pUserdata->_strSessionId, pUserdata));
			}

			if(_bLogicDel)
			{
				RWLockerEx lock(_rwLockForList, false);
				_lstUsers.push_back(pUserdata);
			}

			return 0;
		}

		bool TCPUserCtxMgr::isLogin(const TCPUserData &userData, TCPUserData &queryData) const
		{
			bool	bLogin = false;
			if(userData._bMainCtrl)
				bLogin = isUserLogin(userData._strSessionId, userData._nSocket, queryData);
			else
			{
				bLogin = isDevLogin(userData._strSessionId, queryData);
				if(bLogin)
				{
					if(queryData._nSocket != userData._nSocket)
						bLogin = false;
				}
			}

			return bLogin;
		}

		int TCPUserCtxMgr::logout(const std::string & strSessionId, const SOCKET &nSocket)
		{
			RWLockerEx	lock(_rwLock, false);

			TCPUserData	*pUserData = NULL;

			//map_tcpusers_t::size_type cnt;
			//cnt = _mapUsers.count(strSessionId);
			//if(cnt == 0)
			//	return -1;

			//map_tcpusers_t::iterator	itr;
			//itr = _mapUsers.find(strSessionId);
			//if(itr == _mapUsers.end())
			//	return -1;

			//for(;cnt > 0; cnt--, itr++)
			//{
			//	if(itr->second->_nSocket != nSocket)
			//		continue;

			//	pUserData = itr->second;
			//	break;
			//}

			std::pair<map_tcpusers_t::iterator, map_tcpusers_t::iterator>	pair1;
			pair1 = _mapUsers.equal_range(strSessionId); 
			if(pair1.first == pair1.second)
				return -1;

			bool						bFound = false;
			map_tcpusers_t::iterator	itr;
			for(itr = pair1.first; itr != pair1.second; ++itr) 
			{
				pUserData = itr->second;

				if(pUserData == NULL)
					continue;

				if(pUserData->_nSocket != nSocket)
					continue;

				bFound = true;
				break;
			}

			if(pUserData == NULL || !bFound)
				return -1;
			
			//删除互联关系, 太麻烦不处理
			/*
			if(pData->_bMainCtrl)
			{
				std::list<UserConnInfo>::iterator itrList;
				for(itrList = pData->_listUCtrl.begin();
					itrList != pData->_listUCtrl.end();
					itrList++)
				{
					map_tcpusers_t::iterator itrMap;
					itrMap = _mapUsers.find(itrList->_llSessionId);
					if(itrMap == _mapUsers.end())
						continue;

					itrMap->second->_listMCtrl
				}
			}
			else
				pData->_listMCtrl;
				*/

			_mapUsers.erase(itr);

			//closesocket(pUserData->_nSocket);
			pUserData->_nSocket = INVALID_SOCKET;

			if(_bLogicDel)
				pUserData->_bDeleted = true;
			else
				delete pUserData;

			return 0;
		}

		//主控端可共用一个账号
		bool TCPUserCtxMgr::isUserLogin(const std::string & strSessionId, const SOCKET &nSocket, TCPUserData &userdata) const
		{
			return this->queryUser(strSessionId, nSocket, userdata) < 0 ? false : true;
		}

		//主控端可共用一个账号
		bool TCPUserCtxMgr::isDevLogin(const std::string & strSessionId, TCPUserData &userdata) const
		{
			return this->queryDev(strSessionId, userdata) < 0 ? false : true;
		}

		int TCPUserCtxMgr::queryUser(const std::string & strSessionId, const SOCKET &nSocket, TCPUserData &userdata) const
		{
			RWLockerEx			lock(_rwLock);
			const TCPUserData	*pUserData = NULL;

			//map_tcpusers_t::size_type cnt;
			//cnt = _mapUsers.count(strSessionId);
			//if(cnt == 0)
			//	return -1;

			//map_tcpusers_t::const_iterator	itr;
			//itr = _mapUsers.find(strSessionId);
			//if(itr == _mapUsers.end())
			//	return -1;

			//for(;cnt > 0; cnt--, itr++)
			//{
			//	if(itr->second->_nSocket != nSocket)
			//		continue;

			//	pUserData = itr->second;
			//	break;
			//}
			
			std::pair<map_tcpusers_t::const_iterator, map_tcpusers_t::const_iterator>	pair1;
			pair1 = _mapUsers.equal_range(strSessionId); 
			if(pair1.first == pair1.second)
				return -1;

			bool							bFound = false;
			map_tcpusers_t::const_iterator	itr;
			for(itr = pair1.first; itr != pair1.second; ++itr) 
			{
				pUserData = itr->second;

				if(pUserData == NULL)
					continue;

				if(pUserData->_nSocket != nSocket)
					continue;

				bFound = true;
				break;
			}

			if(pUserData == NULL || !bFound)
				return -1;

			userdata = *pUserData;

			return 0;
		}

		int TCPUserCtxMgr::queryDev(const std::string & strSessionId, TCPUserData &userdata) const
		{
			RWLockerEx			lock(_rwLock);
			const TCPUserData	*pUserData = NULL;

			//设备不同于账号可以同时登录，因此只有一个
			map_tcpusers_t::const_iterator	itr;
			itr = _mapUsers.find(strSessionId);
			if(itr == _mapUsers.end())
				return -1;

			pUserData = itr->second;

			if(pUserData == NULL)
				throw BluevError(__FILE__, __LINE__);

			userdata = *pUserData;

			return 0;
		}

		int TCPUserCtxMgr::keepAlive(const std::string & strSessionId, const SOCKET &nSocket)
		{
			RWLockerEx	lock(_rwLock, false);

			TCPUserData	*pUserData = NULL;
			std::pair<map_tcpusers_t::iterator, map_tcpusers_t::iterator>	pair1;
			pair1 = _mapUsers.equal_range(strSessionId); 
			if(pair1.first == pair1.second)
				return -1;

			bool	bFound = false;
			map_tcpusers_t::iterator	itr;
			for(itr = pair1.first; itr != pair1.second; ++itr) 
			{
				pUserData = itr->second;
				if(pUserData == NULL)
					continue;

				if(pUserData->_nSocket != nSocket)
					continue;

				bFound = true;
				break;
			}

			if(!bFound)
				return -1;

			struct tm	*t = NULL;
			time_t		tt;

			time(&tt);
			t = localtime(&tt);
			pUserData->_tmUpdate = *t;

			return 0;
		}

		int TCPUserCtxMgr::checkDead()
		{
			RWLockerEx lock(_rwLock, false);

			map_tcpusers_t	mapDels;

			TCPUserData	*pData = NULL;
			map_tcpusers_t::iterator itr;
			for(itr = _mapUsers.begin();
				itr != _mapUsers.end();
				itr++)
			{
				pData = itr->second;
				if(pData == NULL)
					continue;

				//与当前时间间隔
				time_t	tt;
				time(&tt);
				time_t	tt2 = mktime(&pData->_tmUpdate);

				if((tt - tt2) > _nSecond)
				{
					if(1)
					{
						//直接关闭连接
						//只要IOCP不close，这里的描述符就不会复用。
						closesocket(pData->_nSocket);
						pData->_nSocket = INVALID_SOCKET;
					}
					else
					{
						//加读锁
						//find pServer->_memPool.releaseObj((IOCPContext *)pPerHandle);
						//PostQueuedCompletionStatus(_hIocp, 0xffffffff, pPerHandle, NULL);
						//去读锁

						//由IOCP关闭socket，并释放pPerHandle
						//太麻烦，不可行。且pPerHandle随时可能再唤醒。
					}
					

					//逻辑删除
					if(_bLogicDel)
						pData->_bDeleted = true;
					else
						delete pData;
					
					pData = itr->second = NULL;

					//_mapUsers.erase(itr);  can‘t
					mapDels.insert(std::make_pair(itr->first, itr->second));
				}
			}

			map_tcpusers_t::const_iterator	itrDel;
			for(itrDel = mapDels.begin(); itrDel != mapDels.end(); ++itrDel)
			{
				std::pair<map_tcpusers_t::const_iterator, map_tcpusers_t::const_iterator>	pair1;
				pair1 = _mapUsers.equal_range(itrDel->first); 
				if(pair1.first == pair1.second)
					continue;

				map_tcpusers_t::const_iterator	itrUsr;
				for(itrUsr = pair1.first; itrUsr != pair1.second; ++itrUsr) 
				{
					if(itrUsr->second != (itrDel->second))
						continue;

					_mapUsers.erase(itrUsr);
					break;
				}
			}

			return mapDels.size();
		}

		__int64 TCPUserCtxMgr::checkDeadEx(__int64 llCount)
		{
			return 0;
			/*
			//必须支持逻辑删除
			if(!_bLogicDel)
				return -1;

			RWLockerEx lock(_rwLockForList, false);

			int nSize = _lstUsers.size();

			if(llCount < 0)
				return -1;

			if(llCount > nSize)
				llCount = nSize;

			if(_llChecked >= nSize)
			{
				_llChecked = 0;
				//return 0;
			}
			
			P2PUserData	*pData = NULL;
			map_users_t::const_iterator itr;
			__int64		llDelCout = 0;
			char		szBuf[100] = {0};

			do
			{
				itr = _mapUsers.find(_llChecked);
				if(itr == _mapUsers.end())
					continue;

				pData = itr->second;
				//与当前时间间隔
				time_t	tt;
				time(&tt);
				time_t	tt2 = mktime(&pData->_tmUpdate);

				if((tt - tt2) > _nSecond)
				{
					sprintf(szBuf, "客户%lld is dead\r\n", pData->_llSessionId);
					std::cout << szBuf;

					delete pData;
					_mapUsers.erase(itr);
					llDelCout++;
				}
			}
			while(++_llChecked <= _llSessionId && llCount--);

			if(_llChecked > _llSessionId)
				_llChecked--;

			return llDelCout;*/
		}
	


		/////////////////////////////////////////////
		int PSTUserCtxMgr::checkUser(const std::string &strUserName, const std::string &strPassWord, DbUserData &userData)
		{
			if(strUserName.empty() || strPassWord.empty())
				return ERR_ERRORPARAM;

			{
				RWLockerEx lock(_rwLock);

				map_dbusers_t::const_iterator	itr;
				itr = _mapDbUsers.find(strUserName);
				if(itr == _mapDbUsers.end())
					return ERR_NULLUSERNAM;

				DbUserData	*pUserData = itr->second;
				if(pUserData == NULL)
					return ERR_NULLUSERNAM;

				if(pUserData->_strPassWord != strPassWord)
					return ERR_ERRORPASSWORD;

				userData = *(pUserData);
			}
			
			return 0;
		}

		int PSTUserCtxMgr::checkDev(const std::string &strDevId, DbDeviceData &devData)
		{
			if(strDevId.empty())
				return ERR_ERRORPARAM;

			{
				RWLockerEx lock(_rwLock);

				map_dbdevs_t::const_iterator	itr;
				itr = _mapDbDevs.find(strDevId);
				if(itr == _mapDbDevs.end())
					return ERR_NULLDEVID;

				DbDeviceData	*pDevData = itr->second;
				if(pDevData == NULL)
					return ERR_NULLDEVID;

				devData = *(pDevData);
			}

			return 0;
		}

		int PSTUserCtxMgr::queryBindsByUser(const std::string &strUserUUID, std::vector<std::string> &vctDevUUID)
		{
			if(strUserUUID.empty())
				return -1;

			vctDevUUID.clear();

			{
				RWLockerEx lock(_rwLock);

				/*map_dbbinds_t::const_iterator	itr;
				itr = _mapDbBinds.find(strUserUUID);
				if(itr == _mapDbBinds.end())
					return ERR_NULLBINDSDEV;

				map_dbbinds_t::size_type cnt;
				cnt = _mapDbBinds.count(strUserUUID);
				
				for(;cnt > 0; cnt--, itr++)
				{
					vctDevUUID.push_back(itr->second);
				}*/
				
				std::pair<map_dbbinds_t::const_iterator, map_dbbinds_t::const_iterator>	pair1;
				pair1 = _mapDbBinds.equal_range(strUserUUID); 
				if(pair1.first == pair1.second)
					return ERR_NULLBINDSDEV;

				for (; pair1.first != pair1.second; ++pair1.first) 
					vctDevUUID.push_back(pair1.first->second);

			}

			return 0;
		}


		////////////////////////////////
		int SYNCUserCtxMgr::sync()
		{
			//获取数据库更新
			//1.连接池获取连接
			//2.sql
			//3.放入内存
			//4.将连接放回连接池

			{
				RWLockerEx lock(_rwLock, false);
				//更新本地数据
			}

			return 0;
		}
#define BINDDATA_FILENAME	"c:\\temp\\binddata.txt"

		int SYNCUserCtxMgr::loadData()
		{
			//获取数据库全部数据
			//1.连接池获取连接
			//2.sql
			//3.放入内存
			//4.将连接放回连接池

			{
				RWLockerEx lock(_rwLock, false);
				//更新本地数据

				//改为从本地配置文件读取
				struct	_stati64 st;
				if(0 <= _stati64(BINDDATA_FILENAME, &st))
				{
					int		nFileLen = (int)st.st_size;
					char	*pData = new char[nFileLen];
					FILE	*fp = fopen(BINDDATA_FILENAME, "rt");
					if(fp)
					{
						fread(pData, nFileLen, 1, fp);
					}

					std::string	strJson(pData, nFileLen);
					Json2BindData(strJson);
				}
				else
				{
					DbUserData	*pUserData = new DbUserData;
					DbUserData	*pUserData2 = new DbUserData;

					DbDeviceData	*pDevData = new DbDeviceData;
					DbDeviceData	*pDevData2 = new DbDeviceData;

					pUserData->_strUserName = "admin1";
					pUserData->_strPassWord = "admin1";
					pUserData->_strUUID = "BDA806AE-209E-4460-B6FA-9921C3B17E55";

					pUserData2->_strUserName = "admin2";
					pUserData2->_strPassWord = "admin2";
					pUserData2->_strUUID = "5C019039-DC92-43D8-AC46-57A5D49A2E5C";

					pDevData->_strDeviceId = "deviceid1";
					pDevData->_strUUID = "38C99E01-BA70-463B-B75C-6041F9DB7E89";

					pDevData2->_strDeviceId = "deviceid2";
					pDevData2->_strUUID ="4656BE2B-B0BF-46BE-A46C-B6BEFEC67DAF";

					_mapDbUsers.insert(std::make_pair(pUserData->_strUserName, pUserData));
					_mapDbUsers.insert(std::make_pair(pUserData2->_strUserName, pUserData2));

					_mapDbDevs.insert(std::make_pair(pDevData->_strDeviceId, pDevData));
					_mapDbDevs.insert(std::make_pair(pDevData2->_strDeviceId, pDevData2));

					_mapDbBinds.insert(std::make_pair(pUserData->_strUUID, pDevData->_strUUID));
					_mapDbBinds.insert(std::make_pair(pUserData2->_strUUID, pDevData2->_strUUID));
				}
			}

			_bLoad = true;

			return 0;
		}

		int SYNCUserCtxMgr::Json2BindData(std::string &strJson)
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
}
}