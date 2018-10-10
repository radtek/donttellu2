#ifndef _INC_BLUEVP2PSVRDEF_H_201412081027_
#define _INC_BLUEVP2PSVRDEF_H_201412081027_


#ifdef BVP2PSVRLIB_EXPORTS

#ifdef _DEBUG
	#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
	#define DEBUG_CLIENTBLOCK
#endif

	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#include <assert.h>

#ifdef _DEBUG
	#define new DEBUG_CLIENTBLOCK
#endif


#endif



#ifndef	FD_SETSIZE
	#define FD_SETSIZE      1024
#endif

//������windows.h֮ǰ ��������
#include <WinSock2.h>
#include <windows.h>

#include "bluevp2psvrapidef.h"
#include "bluevp2psvrmain.h"
#include "json/json.h"
#include "bluevreqans.h"
#include "ServiceMgr.h"
#include "BLUEVConnection.h"
#include "bluevutil.h"
#include "ILocker.h"
#include "IService.h"
#include "IServiceMgr.h"
#include "IServiceStatus.h"
#include "netreqans.h"
#include "SocketHelper.h"
#include "UDPP2PService.h"
#include "udpserver.h"
#include "UDPService.h"

#include "IRunnable.h"
#include "TCPServer.h"
#include "TCPServerEx.h"
#include "MainService.h"
#include "TCPService.h"
#include "TCPP2PService.h"
#include "TCPChatService.h"

#include "UDPServerEx.h"
#include "BluevError.h"

#include "IOCPServer.h"
//#include "IOCPServerEx.h"

#include "IServerArgs.h"
#include "IServerWrapper.h"
#include "TCPServerWrapper.h"
#include "UDPServerWrapper.h"

#endif	//_INC_BLUEVP2PSVRDEF_H_201412081027_