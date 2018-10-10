#ifndef _INC__BVP2PSVRLIB_UDPSERVICE_H_201412081035_
#define _INC__BVP2PSVRLIB_UDPSERVICE_H_201412081035_

#include <windows.h>
#include <string>
#include <map>
#include "ILocker.h"
#include "IService.h"

namespace BLUEV
{
	namespace BVP2PSVRLIB
	{
		using BLUEVBASE::UTIL_GUID;
		using BLUEVNET::SocketHelper;

		class P2PUserData
		{
		public:
			__int64		_llSessionId;
			std::string _strNATIP;		//NATip
			int			_nNATPort;		//NAT端口
			std::string _strLANIP;		//局域网内ip
			int			_nLANPort;		//局域网内机器端口
			std::string _strLANMask;	//局域网子网掩码
			std::string _strMAC;		//MAC地址
			std::string	_strGateWay;	//网关地址
			bool		_bMainCtrl;		//是否主控端
			std::string _strDeviceId;	//设备id
			int			_nDeviceType;	//设备类型
			struct tm	_tmCreate;		//创建时间
			struct tm	_tmUpdate;		//更新时间
			std::string _strRemark;		//备注

			SOCKET		_nSocket;
			GUID		_stGuid;		//全球唯一标识

			P2PUserData()
			{
				_llSessionId = 0;
				_nNATPort = 0;
				_nLANPort = 0;
				_bMainCtrl = false;
				_nDeviceType = 0;
				memset(&_tmCreate, 0, sizeof(_tmCreate));
				memset(&_tmUpdate, 0, sizeof(_tmUpdate));

				_nSocket = INVALID_SOCKET;
				memset(&_stGuid, 0, sizeof(_stGuid));
				//memset(this, 0, sizeof(P2PUserData));
				//createGUID();
			}

			P2PUserData& operator=(const P2PUserData& data)
			{
				_llSessionId = data._llSessionId;
				_strNATIP = data._strNATIP;
				_nNATPort = data._nNATPort;
				_strLANIP = data._strLANIP;
				_nLANPort = data._nLANPort;
				_strLANMask = data._strLANMask;
				_strMAC = data._strMAC;
				_strGateWay = data._strGateWay;
				_bMainCtrl = data._bMainCtrl;

				_strDeviceId = data._strDeviceId;
				_nDeviceType = data._nDeviceType;
				_tmCreate = data._tmCreate;
				_tmUpdate = data._tmUpdate;
				_strRemark = data._strRemark;
				_nSocket = data._nSocket;
				_stGuid = data._stGuid;

				return *this;
			}

			int init()
			{
				if(UTIL_GUID::GUID_IsEmpty(_stGuid))
					return createGUID();

				return 0;
			}

			int createGUID()
			{
				return UTIL_GUID::WIN_GUID_Gen(_stGuid);
			}

			static bool isValidIp(const std::string &strIp)
			{
				if(strIp.empty())
					return false;

				//0.0.0.0 - 255.255.255.255
				
				return true;
			}

			/***************判断是否是合法的IPV4*****************************************/  
			bool isValidIP(const std::string &strIP) const
			{
				int				num = 0;
				std::string		ip = strIP;
				
				//判断前三段是否合法，如果存在的话
				for(int i = 0; i < 3; i++)
				{  
					//如果某段为空或不足三段不合法
					int idx = ip.find(".");
					if(idx == std::string::npos || idx == 0)
						return false;

					num = 0;

					for(int j = 0;j < idx; j++)
					{
						//判断具体某一段是否合法
						if(ip[j] < '0' || ip[j] > '9')
							return false;

						num = num * 10 + ip[j] - '0';
					}
					
					if(num > 255)
						return false;
					
					ip.erase(ip.begin(), ip.begin() + idx + 1); 
				}
				
				//判断第四段是否合法
				num = 0;
				
				//若不存在第四段则不合法
				if(ip.length() == 0)
					return false;
				
				for(int k = 0; k < (int)ip.length(); k++)
				{      
					//若存在则判断是否合法
					if(ip[k] < '0' || ip[k] > '9')
						return false;
					
					num = num * 10 + ip[k] - '0';
				}
				
				if(num > 255)
					return false;
				
				//若各个段均合法整个ip才合法
				return true;
			}

			static bool isValidMask(const std::string &strMask)
			{
				if(strMask.empty())
					return false;

				//255.255.0.0 - 255.255.255.255

				return true;
			}

			bool isSameLan(const P2PUserData &data) const
			{
				if(!P2PUserData::isValidIp(_strNATIP) || 
					!P2PUserData::isValidIp(data._strNATIP))
					return false;

				if(0 != _strNATIP.compare(data._strNATIP))
					return false;

				//是否有效的掩码
				if(!P2PUserData::isValidIp(_strLANIP) || 
					!P2PUserData::isValidMask(_strLANMask) ||
					!P2PUserData::isValidIp(data._strLANIP) ||
					!P2PUserData::isValidMask(data._strLANMask))
					return false;

				//获取到网关
				if(!_strGateWay.empty() && !data._strGateWay.empty())
				{
					return (_strGateWay == data._strGateWay);
				}
				//未得到网关

				//子网掩码运算得到网关
				
				//IP	 10.  1.  7. 76 
				//MASK	255.255.255.  0
				//网关	 10.  1.  7.  1

				//127.0.0.1 => 0100007f  反过来存成int

				int		nIP = 0;
				if(0 > SocketHelper::BLUEV_SOCK_NameToIP(_strLANIP.c_str(), &nIP) || 
					nIP == 0)
					return false;

				int		nIP2 = 0;
				if(0 > SocketHelper::BLUEV_SOCK_NameToIP(data._strLANIP.c_str(), &nIP2) || 
					nIP2 == 0)
					return false;

				int		nMask = 0;
				if(0 > SocketHelper::BLUEV_SOCK_NameToIP(_strLANMask.c_str(), &nMask) || 
					nMask == 0)
					return false;

				int		nMask2 = 0;
				if(0 > SocketHelper::BLUEV_SOCK_NameToIP(data._strLANMask.c_str(), &nMask2) || 
					nMask2 == 0)
					return false;

				//得到网关地址
				int		nGateWay = nIP & nMask;
				int		nGateWay2 = nIP2 & nMask2;
				
				if(nGateWay != nGateWay2)
					return false;

				return true;
			}
		};

		typedef std::map<__int64, P2PUserData *> map_users_t;
		typedef std::vector<P2PUserData> vct_users_t;

		using BLUEV::BLUEVBASE::LockerEx;

		//用户连接上下文管理器
		class	CP2PUserCtxMgr
		{
		private:
			map_users_t			_mapUsers;
			__int64				_llSessionId;
			mutable CRITICAL_SECTION	_csSection;

			int		_nSecond;
			__int64	_llChecked;
		public:
			CP2PUserCtxMgr()
			{
				InitializeCriticalSection(&_csSection);
				_llSessionId = 0;
				_nSecond = 20;
				_llChecked = 0;
			}

			~CP2PUserCtxMgr()
			{
				for(map_users_t::iterator itr = _mapUsers.begin();
					itr != _mapUsers.end();
					itr++
					)
				{
					if(itr->second)
						delete itr->second;
				}
				_mapUsers.clear();

				DeleteCriticalSection(&_csSection);
			}

			int setSecond(int nSecond)
			{
				if(nSecond <= 0)
					return -1;

				_nSecond = nSecond;
				return 0;
			}

			__int64 increaseSessionId()
			{
				LockerEx lock(_csSection);

				return ++_llSessionId;
			}

			int login(P2PUserData &userdata)
			{
				LockerEx lock(_csSection);

				P2PUserData	*pUserdata = new P2PUserData;
				if(pUserdata == NULL)
					return -1;

				userdata._llSessionId = increaseSessionId();
				if(0 > userdata.init())
					return -1;

				struct tm	*t = NULL;
				time_t		tt;
				
				time(&tt);
				t = localtime(&tt);
				//printf("%4d年%02d月%02d日 %02d:%02d:%02d\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
				userdata._tmCreate = *t;
				userdata._tmUpdate = *t;

				*pUserdata = userdata;

				_mapUsers[pUserdata->_llSessionId] = pUserdata;

				return 0;
			}

			//用于首次登陆时未收到返回的session + guid
			//考虑维护map<natip, userdata>便于查询
			/*bool isLogin(std::string strNATIP, int nNATPort, std::string strLANIP, int nLANPort, bool bMainCtrl, const std::string &strId, P2PUserData &userData) const
			{
				LockerEx lock(_csSection);

				const P2PUserData	*pUserData = NULL;
				for(map_users_t::const_iterator itr = _mapUsers.begin();
					itr != _mapUsers.end();
					itr++
					)
				{
					//比mac或devid即可
					if(bMainCtrl)
					{
						if(itr->second->_strMAC != strId)
							continue;
					}
					else
					{
						if(itr->second->_strDeviceId != strId)
							continue;
					}

					if(itr->second->_strNATIP != strNATIP 
						|| itr->second->_nNATPort != nNATPort
						|| itr->second->_strLANIP != strLANIP
						|| itr->second->_nLANPort != nLANPort
						)
					{
						continue;
					}

					{
						pUserData = itr->second;
						userData = *pUserData;
						return true;
					}
				}

				return false;
			}*/

			bool isLogin(const P2PUserData &userData, P2PUserData &queryData) const
			{
				LockerEx lock(_csSection);

				const P2PUserData	*pUserData = NULL;
				for(map_users_t::const_iterator itr = _mapUsers.begin();
					itr != _mapUsers.end();
					itr++
					)
				{
					//比mac或devid即可
					if(userData._bMainCtrl)
					{
						if(itr->second->_strMAC != userData._strMAC)
							continue;
					}
					else
					{
						if(itr->second->_strDeviceId != userData._strDeviceId)
							continue;
					}

					if(itr->second->_strNATIP != userData._strNATIP 
						|| itr->second->_nNATPort != userData._nNATPort
						|| itr->second->_strLANIP != userData._strLANIP
						|| itr->second->_nLANPort != userData._nLANPort
						|| itr->second->_strGateWay != userData._strGateWay
						|| itr->second->_bMainCtrl != userData._bMainCtrl
						)
					{
						continue;
					}

					{
						pUserData = itr->second;
						queryData = *pUserData;
						return true;
					}
				}

				return false;
			}

			int logout(const __int64 & llSessionId, const std::string & strGuid)
			{
				LockerEx lock(_csSection);
				map_users_t::iterator itr;
				itr = _mapUsers.find(llSessionId);
				if(itr == _mapUsers.end())
					return -1;

				const P2PUserData	*pData = itr->second;

				/*GUID	stguid;
				if(!UTIL_GUID::GUID_FromString(strGuid, stguid))
					return -1;

				if(!UTIL_GUID::GUID_IsEqual(pData->_stGuid, stguid))
					return -1;*/

				std::string strGuid2;
				UTIL_GUID::GUID_ToString(pData->_stGuid, strGuid2);

				if(strGuid != strGuid2)
					return -1;

				_mapUsers.erase(itr);
				delete pData;

				return 0;
			}

			/*bool isLogin(const __int64 & llSessionId, const std::string & strGuid) const
			{
				LockerEx lock(_csSection);
				map_users_t::const_iterator itr;
				itr = _mapUsers.find(llSessionId);
				if(itr == _mapUsers.end())
					return false;

				std::string strGuid2;
				if(0 > UTIL_GUID::GUID_ToString(itr->second->_stGuid, strGuid2))
					return false;

				return (strGuid == strGuid2) ? true : false;
			}*/

			bool isLogin(const __int64 & llSessionId, const std::string & strGuid, P2PUserData &userdata) const
			{
				LockerEx lock(_csSection);
				map_users_t::const_iterator itr;
				itr = _mapUsers.find(llSessionId);
				if(itr == _mapUsers.end())
					return false;

				/*GUID	stguid;
				if(!UTIL_GUID::GUID_FromString(strGuid, stguid))
				return false;

				return UTIL_GUID::GUID_IsEqual(stguid, itr->second->_stGuid);*/

				std::string strGuid2;
				if(0 > UTIL_GUID::GUID_ToString(itr->second->_stGuid, strGuid2))
					return false;

				bool bLogin = (strGuid == strGuid2) ? true : false;
				if(bLogin)
					userdata = *itr->second;

				return bLogin;
			}

			int query(__int64 llSessionId, P2PUserData &userdata) const
			{
				LockerEx			lock(_csSection);
				const P2PUserData	*pUserData = NULL;
				
				map_users_t::const_iterator	itr;
				itr = _mapUsers.find(llSessionId);
				if(itr == _mapUsers.end())
					return -1;

				pUserData = itr->second;
				if(pUserData == NULL)
					return -1;

				userdata = *pUserData;

				return 0;
			}

		protected:
			int getUsers(map_users_t &mapUsers) const
			{
				LockerEx	lock(_csSection);

				mapUsers.clear();
				
				//map itr->first是const，无法copy
				//std::copy(_mapUsers.begin(), _mapUsers.end(), mapUsers.begin());
				
				mapUsers.insert(_mapUsers.begin(), _mapUsers.end());

				return 0;
			}
		public:
			int getUsers(vct_users_t &vctUsers) const
			{
				LockerEx	lock(_csSection);

				P2PUserData	*pData = NULL;
				map_users_t::const_iterator itr;
				for(itr = _mapUsers.begin();
					itr != _mapUsers.end();
					itr++)
				{
					pData = itr->second;
					if(pData == NULL)
						continue;

					vctUsers.push_back(*pData);
				}

				return 0;
			}

			int keepAlive(__int64 llSessionId)
			{
				LockerEx	lock(_csSection);

				P2PUserData	*pUserData = NULL;
				map_users_t::const_iterator	itr;
				itr = _mapUsers.find(llSessionId);
				if(itr == _mapUsers.end())
					return -1;

				pUserData = itr->second;

				struct tm	*t = NULL;
				time_t		tt;

				time(&tt);
				t = localtime(&tt);
				pUserData->_tmUpdate = *t;

				return 0;
			}

			int checkDead()
			{
				LockerEx lock(_csSection);

				std::vector<__int64> vctDel;
				P2PUserData	*pData = NULL;
				map_users_t::iterator itr;
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
						//逻辑删除 ？ no
						//pData->_bDelete = true;
						delete pData;
						pData = itr->second = NULL;

						//_mapUsers.erase(itr);  can‘t
						vctDel.push_back(itr->first);
					}
				}

				for(int i = 0; i < (int)vctDel.size(); ++i)
				{
					_mapUsers.erase(vctDel[i]);
				}

				return 0;
			}
			
			//llCount		遍历个数
			//retVal		删除个数
			__int64 checkDeadEx(__int64 llCount)
			{
				LockerEx lock(_csSection);

				if(llCount < 0)
					return -1;

				if(llCount > _llSessionId)
					llCount = _llSessionId;

				if(_llChecked >= _llSessionId)
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

				return llDelCout;
			}
		};

	};
};

#endif	// _INC__BVP2PSVRLIB_UDPSERVICE_H_201412081035_