#include "bluevp2psvrdef.h"
#include <iostream>
#include <sstream>

namespace BLUEV{
	namespace BLUEVNET{

		using BLUEVBASE::BluevError;

		int MainService::doService()
		{
			int			retval	= -1;
			std::ostringstream	oss;

			ServiceParamContext	svcParam;
			BLUEVConnection		conn;
			BLUEV_NetReqHead	&req = conn.req;
			BLUEV_NetAnsHead	&ans = conn.ans;

			// �����û�����
			conn.nSockfd = _nSocket;

			try
			{
				int	nSecond = _param.nServerRecvTimeOut * 1000;	//ms
				setsockopt(_nSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nSecond, sizeof(int));

				do
				{
					//��ȡһ��ͷ
					SocketHelper::BLUEV_SOCK_Recv(_nSocket, &req, sizeof(req));

					// У���û�����
					if(!req.isValid())
						throw BluevError("�Ƿ�ͷ����Ϣ");

					// Ȼ����ò�ͬ�Ĵ��������д���
					int			opClass = UTIL_CONV::Char2To_uint2(req.nOpClass);
					IService	*service = _svcMgr.lookup(opClass);
					if(service == NULL)
					{
						oss.str("");
						oss << "��Ч���û�����opClass=" << opClass;
						throw BluevError(oss.str().c_str());
					}

					int dataLen = UTIL_CONV::Char4To_uint4(req.nDataLen);
					if(dataLen < 0 || dataLen > 10*1024*1024) //10M
					{
						oss.str("");
						oss << "��Ч�����ݳ��ȣ�length=" << dataLen;
						throw BluevError(oss.str().c_str());
					}

					// ��������ʱ��Ҫ�ͷ��ڴ�
					if(svcParam.pReq)
					{
						delete[] svcParam.pReq;
						svcParam.pReq = NULL;
					}
					if(svcParam.pAns)
					{
						delete[] svcParam.pAns;
						svcParam.pAns = NULL;
					}
					
					svcParam.nAnsLen = 0;
					svcParam.nReqLen = dataLen;
					svcParam.nPort = ntohs(_nPort);
					//UTIL_CONV::uint4ToChar4(_nIPAddr, svcParam.bnIP);
					struct in_addr	addr;
					addr.S_un.S_addr = _nIPAddr;
					char	*pIp = inet_ntoa(addr);
					memcpy(svcParam.bnIP, pIp, strlen(pIp));

					if(dataLen > 0)
					{
						// ��ȡʣ������ݡ�
						svcParam.pReq = new unsigned char[dataLen];
						if(svcParam.pReq == NULL)
							throw BluevError(__FILE__, __LINE__);

						if(dataLen > SocketHelper::BLUEV_SOCK_Recv(_nSocket, svcParam.pReq, dataLen))
							throw BluevError(__FILE__, __LINE__);
					}

					// ִ���û�����
					retval = service->execute(conn, svcParam);

					// �ѽ�����͸��ͻ��ˡ�
					UTIL_CONV::uint2ToChar2(retval, ans.nRetVal);
					UTIL_CONV::uint4ToChar4(svcParam.nAnsLen, ans.nDataLen);

					// ����ִ�н��
					if(sizeof(ans) > SocketHelper::BLUEV_SOCK_Send(_nSocket, &ans, sizeof(ans)))
						throw BluevError(__FILE__, __LINE__);
					if(svcParam.pAns != NULL && svcParam.nAnsLen > 0)
					{
						if(svcParam.nAnsLen > SocketHelper::BLUEV_SOCK_Send(_nSocket, svcParam.pAns, svcParam.nAnsLen))
							throw BluevError(__FILE__, __LINE__);
					}

					// �ͷ��ڴ�
					if(svcParam.pReq)
					{
						delete[] svcParam.pReq;
						svcParam.pReq = NULL;
					}
					if(svcParam.pAns)
					{
						delete[] svcParam.pAns;
						svcParam.pAns = NULL;
					}
				
					//������
				} while(req.reuseConnection != 0);
			}
			catch(BluevError& e)
			{
				e.print();
			}
			catch(...)
			{				
			}

			// �ͷ��ڴ�
			if(svcParam.pReq)
			{
				delete[] svcParam.pReq;
				svcParam.pReq = NULL;
			}
			if(svcParam.pAns)
			{
				delete[] svcParam.pAns;
				svcParam.pAns = NULL;
			}

			// �ر�����
			closesocket(_nSocket);
			return	retval;
		}

	}
}