#ifndef _INC_ISERVERARGS_H_201504131141_
#define _INC_ISERVERARGS_H_201504131141_

namespace BLUEV{
	namespace BVP2PSVRLIB{

		class IServerArgs
		{
		public:
			std::wstring	_wstrProcName;
			int				_nPort;

			IServerArgs()
			{
				_nPort = 0;
			}

			virtual int parseArgs(int argc, wchar_t **argv) = 0;
		};

#define UDPSERVER_PORT	14000
#define TCPSERVER_PORT	14100

		class UDPServerArgs : public IServerArgs
		{
		public:
			enum tagUDPServerType
			{
				UDPSERVER,
				UDPSERVEREX

			};

			int	_nType;

			UDPServerArgs()
			{
				_nType = UDPSERVER;
				_nPort = UDPSERVER_PORT;
			}

			int parseArgs(int argc, wchar_t **argv)
			{
				if(argc < 1 || argv == NULL)
					return -1;

				_wstrProcName = argv[0];

				if(argc > 1)
					_nPort = _wtoi(argv[1]);

				if(_nPort <= 0)
					return -1;

				return 0;
			}
		};

		class TCPServerArgs : public IServerArgs
		{
		public:
			enum tagTCPServerType
			{
				TCPSERVER,
				TCPSERVEREX,
				IOCPSERVER

			};

			int		_nType;
			bool	_bPreFork;

			TCPServerArgs()
			{
				_nType = TCPSERVER;
				_bPreFork = false;
				_nPort = TCPSERVER_PORT;
			}

			int parseArgs(int argc, wchar_t **argv)
			{
				if(argc < 1 || argv == NULL)
					return -1;

				_wstrProcName = argv[0];

				if(argc > 2)
					_nPort = _wtoi(argv[2]);

				if(_nPort <= 0)
					return -1;

				if(argc > 3)
					_bPreFork = _wtoi(argv[3]) ? true : false;

				return 0;
			}
		};

	}}


#endif	//_INC_ISERVERARGS_H_201504131141_