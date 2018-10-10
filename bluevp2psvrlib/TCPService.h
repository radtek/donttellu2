#ifndef _INC_TCPSERVICE_H_201412261337_
#define _INC_TCPSERVICE_H_201412261337_

#include <hash_map>

namespace BLUEV{
	namespace BVP2PSVRLIB{

		using BLUEVNET::IService;
		using BLUEVNET::TCPServer;
		using BLUEVNET::BLUEVConnection;
		using BLUEVNET::ServiceParamContext;

		using BLUEVBASE::UTIL_GUID;
		using BLUEVNET::SocketHelper;
		using BLUEVBASE::RWLocker;
		using BLUEVBASE::RWLockerEx;
		using BLUEVBASE::BluevError;

		class TCPUserCtxMgr;
		class PSTUserCtxMgr;

		class TCPService : public IService
		{
		protected:
			TCPServer		*_pServer;
		public:
			TCPService(
				TCPServer	*pServer) 
				: 
				_pServer(pServer)
			{
				if(_pServer == NULL)
					throw BluevError(__FILE__, __LINE__);
			}

			int execute(BLUEVConnection &conn, ServiceParamContext &ctx);
			virtual int doWork(BLUEVConnection &conn, ServiceParamContext &ctx) = 0;
		};

		//必须是已经互相连接
		class TCPUserConnInfo
		{
		public:
			std::string	_strId;			//MAC地址
			SOCKET		_nSocket;
			bool		_bP2PConn;		//是否p2p连接
			bool		_bProxyConn;	//是否中转连接
			bool		_bUPNP;

			TCPUserConnInfo()
			{
				memset(this, 0, sizeof(*this));
				_nSocket = INVALID_SOCKET;
			}

			bool operator==(TCPUserConnInfo &data)
			{
				return _nSocket == data._nSocket;
			}
		};

		typedef std::vector<TCPUserConnInfo>	vct_tcpuserconn_t;
		
		class TCPUserData
		{
		public:
			std::string _strNATIP;		//NATip
			int			_nNATPort;		//NAT端口
			std::string _strLANIP;		//局域网内ip
			int			_nLANPort;		//局域网内机器端口
			std::string _strLANMask;	//局域网子网掩码
			std::string _strMAC;		//MAC地址
			std::string	_strGateWay;	//网关地址
			std::string _strDeviceId;	//设备id
			struct tm	_tmCreate;		//创建时间
			struct tm	_tmUpdate;		//更新时间

			bool		_bMainCtrl;		//是否主控端	
			volatile bool		_bDeleted;		//逻辑删除
			
			//必须已经互联
			vct_tcpuserconn_t	_vctMCtrl;	//控制本设备的主控端 
			vct_tcpuserconn_t	_vctUCtrl;	//本主控端控制的设备 
			
			SOCKET		_nSocket;
			std::string _strSessionId;		//uuid

			std::string	_strUserName;
			std::string _strPassWord;

			std::string	_strUPNPIP;		//通过外网服务器获取IP有时不对，或者被控端在双层路由下。这时可用UPNP获取路由WANIP	
			int			_nUPNPPort;		//使用UPNP时，在被控端另起一个TCP服务器，用来区别与服务器的长连接
			bool		_bUPNP;

			TCPUserData()
			{
				_nNATPort = 0;
				_nLANPort = 0;
				_bMainCtrl = false;
				_bDeleted = false;

				memset(&_tmCreate, 0, sizeof(_tmCreate));
				memset(&_tmUpdate, 0, sizeof(_tmUpdate));
				_nSocket = INVALID_SOCKET;

				_bUPNP = false;
				_nUPNPPort = 0;
			}

			TCPUserData& operator=(const TCPUserData& data)
			{
				_strNATIP = data._strNATIP;
				_nNATPort = data._nNATPort;
				_strLANIP = data._strLANIP;
				_nLANPort = data._nLANPort;
				_strLANMask = data._strLANMask;
				_strMAC = data._strMAC;
				_strGateWay = data._strGateWay;
				_bMainCtrl = data._bMainCtrl;
				_bDeleted = data._bDeleted;

				_strDeviceId = data._strDeviceId;
				_tmCreate = data._tmCreate;
				_tmUpdate = data._tmUpdate;
				_nSocket = data._nSocket;
				_strSessionId = data._strSessionId;

				_vctUCtrl = data._vctUCtrl;
				_vctMCtrl = data._vctMCtrl;

				_strUserName = data._strUserName;
				_strPassWord = data._strPassWord;

				_strUPNPIP = data._strUPNPIP;
				_nUPNPPort = data._nUPNPPort;
				_bUPNP = data._bUPNP;

				return *this;
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

			bool isSameLan(const TCPUserData &data) const
			{
				if(!TCPUserData::isValidIp(_strNATIP) || 
					!TCPUserData::isValidIp(data._strNATIP))
					return false;

				if(0 != _strNATIP.compare(data._strNATIP))
					return false;

				//是否有效的掩码
				if(!TCPUserData::isValidIp(_strLANIP) || 
					!TCPUserData::isValidMask(_strLANMask) ||
					!TCPUserData::isValidIp(data._strLANIP) ||
					!TCPUserData::isValidMask(data._strLANMask))
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

		//KEY:账号或设备UUID.考虑一个用户可多人登录，采用HASH_MULTIMAP
		typedef stdext::hash_multimap<std::string, TCPUserData *> map_tcpusers_t;
		typedef std::list<TCPUserData *> lst_tcpusers_t;

		using BLUEV::BLUEVBASE::LockerEx;

		//用户连接上下文管理器
		class	TCPUserCtxMgr
		{
		private:
			map_tcpusers_t		_mapUsers;
			mutable RWLocker	_rwLock;

			int					_nSecond;
			__int64				_llChecked;

			mutable RWLocker	_rwLockForList;
			lst_tcpusers_t		_lstUsers;	//_mapUsers的副本，用于checkDeadEx
			bool				_bLogicDel;	//逻辑删除		
		public:
			TCPUserCtxMgr()
			{
				_nSecond = 20;
				_llChecked = 0;

				_bLogicDel = false;
			}

			~TCPUserCtxMgr()
			{
				if(_bLogicDel)
				{
					lst_tcpusers_t::const_iterator itr;
					for(itr = _lstUsers.begin();
						itr != _lstUsers.end();
						itr++
						)
					{
						if(*itr)
							delete *itr;
					}
				}
				else
				{
					map_tcpusers_t::const_iterator itr;
					for(itr = _mapUsers.begin();
						itr != _mapUsers.end();
						itr++
						)
					{
						if(itr->second)
							delete itr->second;
					}
				}
				
				_mapUsers.clear();
				_lstUsers.clear();
			}

			int setSecond(int nSecond)
			{
				if(nSecond <= 0)
					return -1;

				_nSecond = nSecond;
				return 0;
			}

			int login(TCPUserData &userdata);

			bool isLogin(const TCPUserData &userData, TCPUserData &queryData) const;

			int logout(const std::string & strSessionId, const SOCKET & nSocket);

			bool isUserLogin(const std::string & strSessionId, const SOCKET & nSocket, TCPUserData &userdata) const;
		
			bool isDevLogin(const std::string & strSessionId, TCPUserData &userdata) const;

			int queryUser(const std::string & strSessionId, const SOCKET &nSocket, TCPUserData &userdata) const;

			int queryDev(const std::string & strSessionId, TCPUserData &userdata) const;

			int keepAlive(const std::string & strSessionId, const SOCKET &nSocket);

			int checkDead();

			//llCount		遍历个数
			//retVal		删除个数
			//另存一份LIST，在原MAP中逻辑删除，由此处REAL DELETE
			__int64 checkDeadEx(__int64 llCount);

			typedef enum tagConnType
			{
				CONN_P2P = 1,	
				CONN_PROXY = 2,
				CONN_LAN = 3
			} ConnType;
/*
			int addUpdateConnInfo(__int64 llSessionId, __int64 llDevSessionId, ConnType enumType)
			{
				RWLockerEx lock(_rwLock, false);

				if(llSessionId <= 0 || llDevSessionId <= 0)
					return -1;

				map_tcpusers_t::iterator itr, itrDev;
				itr = _mapUsers.find(llSessionId);
				if(itr == _mapUsers.end())
					return -1;

				itrDev = _mapUsers.find(llDevSessionId);
				if(itrDev == _mapUsers.end())
					return -1;

				TCPUserData & data = *itr->second;
				TCPUserData & devData = *itrDev->second;
				if(!data._bMainCtrl || devData._bMainCtrl)
					return -1;

				if(0 > addUpdateVct(data, llDevSessionId, enumType))
					return -1;
				
				if(0 > addUpdateVct(devData, llSessionId, enumType))
					return -1;

				return 0;
			}

		private:
			int addUpdateVct(TCPUserData & data, __int64 llSessionId, ConnType enumType)
			{
				if(llSessionId <= 0)
					return -1;

				std::vector<UserConnInfo>	*pVct;
				if(data._bMainCtrl)
				{
					pVct = &data._vctUCtrl;
				}
				else
				{
					pVct = &data._vctMCtrl;
				}

				bool	bFound = false;
				std::vector<UserConnInfo>::iterator itrVct;
				for(itrVct = pVct->begin();
					itrVct != pVct->end();
					itrVct++)
				{
					if(itrVct->_llSessionId != llSessionId)
						continue;

					//更新
					bFound = true;

					UserConnInfo &info = *itrVct;
						
					switch(enumType)
					{
					case CONN_LAN:
						//视为p2p
					case CONN_P2P:
						info._bP2PConn = true;
						break;
					case CONN_PROXY:
						info._bProxyConn = true;
						break;
					default:
						return -1;
					}

					break;
				}

				if(!bFound)
				{
					//新增
					UserConnInfo info;
					info._llSessionId = llSessionId;
					switch(enumType)
					{
					case CONN_LAN:
						//视为p2p
					case CONN_P2P:
						info._bP2PConn = true;
						break;
					case CONN_PROXY:
						info._bProxyConn = true;
						break;
					default:
						return -1;
					}
					
					pVct->push_back(info);
				}
				
				return 0;
			}
*/
		};

		class DbUserData
		{
		public:
			struct tm	_tmCreate;		//创建时间
			struct tm	_tmUpdate;		//更新时间

			std::string	_strUserName;	//账号
			std::string _strPassWord;	//32字节，MD5的16进制表示
			std::string _strUUID;

			DbUserData()
			{
				memset(&_tmCreate, 0, sizeof(_tmCreate));
				memset(&_tmUpdate, 0, sizeof(_tmUpdate));
			}

			DbUserData & operator=(const DbUserData &data)
			{
				_tmCreate = data._tmCreate;
				_tmUpdate = data._tmUpdate;
				_strUserName = data._strUserName;
				_strPassWord = data._strPassWord;
				_strUUID = data._strUUID;

				return *this;
			}
		};

		class DbDeviceData
		{
		public:
			struct tm	_tmCreate;		//创建时间
			struct tm	_tmUpdate;		//更新时间

			int			_nStatus;
			std::string	_strDeviceId;	//设备id
			std::string _strUUID;

			DbDeviceData()
			{
				memset(&_tmCreate, 0, sizeof(_tmCreate));
				memset(&_tmUpdate, 0, sizeof(_tmUpdate));
				_nStatus = 0;
			}

			DbDeviceData & operator=(const DbDeviceData &data)
			{
				_tmCreate = data._tmCreate;
				_tmUpdate = data._tmUpdate;
				_nStatus = data._nStatus;
				_strDeviceId = data._strDeviceId;
				_strUUID = data._strUUID;

				return *this;
			}
		};

		//用户数据持久化管理器persistence
		//服务器初始化时，从数据库读出。
		//仅保存简单数据结构，供查询。
		//对用户只读。只有同步管理器定时从数据库更新
		//
		typedef std::hash_map<std::string, DbUserData *>		map_dbusers_t;	//key:username
		typedef std::hash_map<std::string, DbDeviceData *>		map_dbdevs_t;	//key::devid
		typedef std::hash_multimap<std::string, std::string>	map_dbbinds_t;	//key::usr uuid value:dev uuid

		/*
		typedef std::map<std::string, DbUserData>		map_dbusers_t;	//key:username
		typedef std::map<std::string, DbDeviceData>		map_dbdevs_t;	//key::devid
		typedef std::multimap<std::string, std::string>	map_dbbinds_t;	//key::username value:devid
		*/
		
		class	PSTUserCtxMgr
		{
			enum
			{
				ERR_NULLBINDSDEV = -5,
				ERR_NULLDEVID = -4,
				ERR_ERRORPASSWORD = -3,
				ERR_NULLUSERNAM = -2,
				ERR_ERRORPARAM = -1
				
			};
		private:
			//账号表
			map_dbusers_t	&_mapDbUsers;
			//设备表
			map_dbdevs_t	&_mapDbDevs;
			//账号设备绑定表
			map_dbbinds_t	&_mapDbBinds;
			//日志表
			
			//粒度太大
			RWLocker		&_rwLock;
		public:
			PSTUserCtxMgr(
				map_dbusers_t	&mapDbUsers, 
				map_dbdevs_t	&mapDbDevs, 
				map_dbbinds_t	&mapDbBinds, 
				RWLocker		&rwLock
				) : 
				_mapDbUsers(mapDbUsers), 
				_mapDbDevs(mapDbDevs), 
				_mapDbBinds(mapDbBinds), 
				_rwLock(rwLock)
			{

			}

			~PSTUserCtxMgr()
			{

			}

			//验证主控端账号，密码
			int checkUser(const std::string &strUserName, const std::string &strPassWord, DbUserData &userData);

			//验证被控端devid
			int checkDev(const std::string &strDevId, DbDeviceData &devData);

			//主控端UUID与被控端UUID绑定关系
			int queryBindsByUser(const std::string &strUserUUID, std::vector<std::string> &vctDevUUID);
			//int queryBindsByDevId(const std::string &strDevId, std::string &strUserName);
		};

		//用户数据同步管理器
		//定时检查数据库中更新的记录，同步到PSTUserCtxMgr。可通过触发器自动记录更新

		class	SYNCUserCtxMgr
		{
		public:
			//账号表
			map_dbusers_t	_mapDbUsers;
			//设备表
			map_dbdevs_t	_mapDbDevs;
			//账号设备绑定表
			map_dbbinds_t	_mapDbBinds;
			//日志表

			RWLocker		_rwLock;

			volatile bool	_bLoad;

			SYNCUserCtxMgr()
			{
				_bLoad = false;

				//test
				loadData();
			}

			~SYNCUserCtxMgr()
			{
				map_dbusers_t::const_iterator itr;
				for(itr = _mapDbUsers.begin();
					itr != _mapDbUsers.end();
					itr++
					)
				{
					if(itr->second)
						delete itr->second;
				}

				map_dbdevs_t::const_iterator itr2;
				for(itr2 = _mapDbDevs.begin();
					itr2 != _mapDbDevs.end();
					itr2++
					)
				{
					if(itr2->second)
						delete itr2->second;
				}

				_mapDbUsers.clear();
				_mapDbDevs.clear();
				_mapDbBinds.clear();
			}

			//同步更新数据
			int sync();

			//初次加载数据
			int loadData();

			int Json2BindData(std::string &strJson);

		private:
		};
	}
}

#endif	//_INC_TCPSERVICE_H_201412261337_