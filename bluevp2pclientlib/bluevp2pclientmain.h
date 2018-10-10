#ifndef _INC_BLUEVP2PCLIENTMAIN_H_201412151626_
#define _INC_BLUEVP2PCLIENTMAIN_H_201412151626_

#include "UDPP2PClient.h"
#include "TCPClient.h"

namespace BLUEV
{
	namespace BVP2PCLIENTLIB
	{
		using BLUEV::BLUEVBASE::IServiceStatus;
		using BLUEV::BVP2PCLIENTLIB::UDPP2PClient;
		using BLUEV::BVP2PCLIENTLIB::TCPClient;

		extern BVP2PCLIENTLIB_API
			int BLUEVP2P_ClientMain(int argc, wchar_t **argv, IServiceStatus& svcStatus);

		extern  BVP2PCLIENTLIB_VAR 
			UDPP2PClient	*g_pUdpClient;

		extern BVP2PCLIENTLIB_VAR 
			TCPClient	*g_pTcpClient;

		/*	UDP	Function	*/

		extern BVP2PCLIENTLIB_API
			int login();

		extern BVP2PCLIENTLIB_API
			int logout();

		extern BVP2PCLIENTLIB_API
			int getDevInfo();

		extern BVP2PCLIENTLIB_API
			int connDev(int nIndex);

		extern BVP2PCLIENTLIB_API
			int keepAlive(int nSecond);

		extern BVP2PCLIENTLIB_API
			int proxyConn(int nIndex);



		/*	TCP	Function	*/
		extern BVP2PCLIENTLIB_API
			int chatMirror(std::string &strSend);

		extern BVP2PCLIENTLIB_API 
			int connDevTCP(int nIndex);

		extern BVP2PCLIENTLIB_API
			int keepAliveTCP(int nSecond);

		extern BVP2PCLIENTLIB_API 
			int proxyReqTCP(int nIndex);
	};
};

#endif	//_INC_BLUEVP2PCLIENTMAIN_H_201412151626_