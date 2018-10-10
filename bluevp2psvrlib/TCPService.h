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

		//�������Ѿ���������
		class TCPUserConnInfo
		{
		public:
			std::string	_strId;			//MAC��ַ
			SOCKET		_nSocket;
			bool		_bP2PConn;		//�Ƿ�p2p����
			bool		_bProxyConn;	//�Ƿ���ת����
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
			int			_nNATPort;		//NAT�˿�
			std::string _strLANIP;		//��������ip
			int			_nLANPort;		//�������ڻ����˿�
			std::string _strLANMask;	//��������������
			std::string _strMAC;		//MAC��ַ
			std::string	_strGateWay;	//���ص�ַ
			std::string _strDeviceId;	//�豸id
			struct tm	_tmCreate;		//����ʱ��
			struct tm	_tmUpdate;		//����ʱ��

			bool		_bMainCtrl;		//�Ƿ����ض�	
			volatile bool		_bDeleted;		//�߼�ɾ��
			
			//�����Ѿ�����
			vct_tcpuserconn_t	_vctMCtrl;	//���Ʊ��豸�����ض� 
			vct_tcpuserconn_t	_vctUCtrl;	//�����ض˿��Ƶ��豸 
			
			SOCKET		_nSocket;
			std::string _strSessionId;		//uuid

			std::string	_strUserName;
			std::string _strPassWord;

			std::string	_strUPNPIP;		//ͨ��������������ȡIP��ʱ���ԣ����߱��ض���˫��·���¡���ʱ����UPNP��ȡ·��WANIP	
			int			_nUPNPPort;		//ʹ��UPNPʱ���ڱ��ض�����һ��TCP������������������������ĳ�����
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

			/***************�ж��Ƿ��ǺϷ���IPV4*****************************************/  
			bool isValidIP(const std::string &strIP) const
			{
				int				num = 0;
				std::string		ip = strIP;
				
				//�ж�ǰ�����Ƿ�Ϸ���������ڵĻ�
				for(int i = 0; i < 3; i++)
				{  
					//���ĳ��Ϊ�ջ������β��Ϸ�
					int idx = ip.find(".");
					if(idx == std::string::npos || idx == 0)
						return false;

					num = 0;

					for(int j = 0;j < idx; j++)
					{
						//�жϾ���ĳһ���Ƿ�Ϸ�
						if(ip[j] < '0' || ip[j] > '9')
							return false;

						num = num * 10 + ip[j] - '0';
					}
					
					if(num > 255)
						return false;
					
					ip.erase(ip.begin(), ip.begin() + idx + 1); 
				}
				
				//�жϵ��Ķ��Ƿ�Ϸ�
				num = 0;
				
				//�������ڵ��Ķ��򲻺Ϸ�
				if(ip.length() == 0)
					return false;
				
				for(int k = 0; k < (int)ip.length(); k++)
				{      
					//���������ж��Ƿ�Ϸ�
					if(ip[k] < '0' || ip[k] > '9')
						return false;
					
					num = num * 10 + ip[k] - '0';
				}
				
				if(num > 255)
					return false;
				
				//�������ξ��Ϸ�����ip�źϷ�
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

				//�Ƿ���Ч������
				if(!TCPUserData::isValidIp(_strLANIP) || 
					!TCPUserData::isValidMask(_strLANMask) ||
					!TCPUserData::isValidIp(data._strLANIP) ||
					!TCPUserData::isValidMask(data._strLANMask))
					return false;

				//��ȡ������
				if(!_strGateWay.empty() && !data._strGateWay.empty())
				{
					return (_strGateWay == data._strGateWay);
				}
				//δ�õ�����

				//������������õ�����
				
				//IP	 10.  1.  7. 76 
				//MASK	255.255.255.  0
				//����	 10.  1.  7.  1

				//127.0.0.1 => 0100007f  ���������int

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

				//�õ����ص�ַ
				int		nGateWay = nIP & nMask;
				int		nGateWay2 = nIP2 & nMask2;
				
				if(nGateWay != nGateWay2)
					return false;

				return true;
			}

		};

		//KEY:�˺Ż��豸UUID.����һ���û��ɶ��˵�¼������HASH_MULTIMAP
		typedef stdext::hash_multimap<std::string, TCPUserData *> map_tcpusers_t;
		typedef std::list<TCPUserData *> lst_tcpusers_t;

		using BLUEV::BLUEVBASE::LockerEx;

		//�û����������Ĺ�����
		class	TCPUserCtxMgr
		{
		private:
			map_tcpusers_t		_mapUsers;
			mutable RWLocker	_rwLock;

			int					_nSecond;
			__int64				_llChecked;

			mutable RWLocker	_rwLockForList;
			lst_tcpusers_t		_lstUsers;	//_mapUsers�ĸ���������checkDeadEx
			bool				_bLogicDel;	//�߼�ɾ��		
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

			//llCount		��������
			//retVal		ɾ������
			//���һ��LIST����ԭMAP���߼�ɾ�����ɴ˴�REAL DELETE
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

					//����
					bFound = true;

					UserConnInfo &info = *itrVct;
						
					switch(enumType)
					{
					case CONN_LAN:
						//��Ϊp2p
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
					//����
					UserConnInfo info;
					info._llSessionId = llSessionId;
					switch(enumType)
					{
					case CONN_LAN:
						//��Ϊp2p
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
			struct tm	_tmCreate;		//����ʱ��
			struct tm	_tmUpdate;		//����ʱ��

			std::string	_strUserName;	//�˺�
			std::string _strPassWord;	//32�ֽڣ�MD5��16���Ʊ�ʾ
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
			struct tm	_tmCreate;		//����ʱ��
			struct tm	_tmUpdate;		//����ʱ��

			int			_nStatus;
			std::string	_strDeviceId;	//�豸id
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

		//�û����ݳ־û�������persistence
		//��������ʼ��ʱ�������ݿ������
		//����������ݽṹ������ѯ��
		//���û�ֻ����ֻ��ͬ����������ʱ�����ݿ����
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
			//�˺ű�
			map_dbusers_t	&_mapDbUsers;
			//�豸��
			map_dbdevs_t	&_mapDbDevs;
			//�˺��豸�󶨱�
			map_dbbinds_t	&_mapDbBinds;
			//��־��
			
			//����̫��
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

			//��֤���ض��˺ţ�����
			int checkUser(const std::string &strUserName, const std::string &strPassWord, DbUserData &userData);

			//��֤���ض�devid
			int checkDev(const std::string &strDevId, DbDeviceData &devData);

			//���ض�UUID�뱻�ض�UUID�󶨹�ϵ
			int queryBindsByUser(const std::string &strUserUUID, std::vector<std::string> &vctDevUUID);
			//int queryBindsByDevId(const std::string &strDevId, std::string &strUserName);
		};

		//�û�����ͬ��������
		//��ʱ������ݿ��и��µļ�¼��ͬ����PSTUserCtxMgr����ͨ���������Զ���¼����

		class	SYNCUserCtxMgr
		{
		public:
			//�˺ű�
			map_dbusers_t	_mapDbUsers;
			//�豸��
			map_dbdevs_t	_mapDbDevs;
			//�˺��豸�󶨱�
			map_dbbinds_t	_mapDbBinds;
			//��־��

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

			//ͬ����������
			int sync();

			//���μ�������
			int loadData();

			int Json2BindData(std::string &strJson);

		private:
		};
	}
}

#endif	//_INC_TCPSERVICE_H_201412261337_