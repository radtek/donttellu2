#ifndef _INC_NETREQANS_H_201412081716_
#define _INC_NETREQANS_H_201412081716_

namespace BLUEV
{
	namespace BLUEVNET
	{
		class  BVNETLIB_API	BLUEVOpClass
		{
		public:
			enum tagOPCLASS
			{
				OP_CLASS_P2P = 1,	//����
				OP_CLASS_P2PANS,	//Ӧ��
				OP_CLASS_P2PCLIENT,	//�ͻ���֮������
				OP_CLASS_P2PCLIENTANS,	//�ͻ���֮��Ӧ��
				OP_CLASS_P2PPROXY,	//������������ת����
				OP_CLASS_P2PPROXYANS,	//������������תӦ��
				OP_CLASS_USER,
			};

			enum tagOPCODE
			{
				OP_P2P_LOGIN = 1,
				OP_P2P_LOGOUT = 2,
				OP_P2P_GETDEVINFO = 3,
				OP_P2P_CONNDEV = 4,
				OP_P2P_DIG = 5,
				OP_P2P_DOWNLOAD = 6,
				OP_P2P_KEEPALIVE = 7,
				OP_P2P_UPLOAD = 8,
				OP_P2P_PROXY = 9,
				OP_P2P_PROXYANS = 10,
			};

			/*enum tagOPCLASS_CLIENT
			{
				
			};

			enum tagOPCODE_CLIENT
			{
				
			};*/

			enum tagERRCODE
			{
				ERR_THREADFULL = 1,
				ERR_SERVICENOTFOUND,
				ERR_MEMORY,
				ERR_EXCEPTION
			};
		};
	};
};


#endif	//_INC_NETREQANS_H_201412081716_