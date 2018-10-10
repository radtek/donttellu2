#ifndef _INC_BLUEVREQANS_H_
#define	_INC_BLUEVREQANS_H_

#include "bluevutil.h"

namespace	BLUEV
{
	namespace	BLUEVNET
	{
#define	BVNRHO_MAGICSTR	"$bluev$"
#define BV_MAJORVER		1
#define BV_MINORVER		1	//1.0 demo�棬 1.1 ����P2P

		using BLUEV::BLUEVBASE::UTIL_CONV;

		// �˽ṹ�Ĵ�СΪ32���ֽڡ�
		class BVNETLIB_API	BLUEV_NetReqHead
		{
		public:
			unsigned char cbSize[2];		// size of this structure
			unsigned char nMajorVer;		// major version, must be BV_MAJORVER
			unsigned char nMinorVer;		// minor version, must be BV_MINORVER
			char	szMagicStr[8];		// must be $bluev$
			unsigned char nOpClass[2];		// operation class, 2 bytes int
			unsigned char nOpCode[2];			// operation code
			unsigned char nDataLen[4];		// data length followed
			unsigned char reuseConnection;	// temp long conn��not suggest to use(iocp server no support)
			unsigned char longConnection;	// true long conn(only serverex suport, iocp server default long conn, needn't set this)
			//22 bytes here
			unsigned char bnRes[10];
			

			BLUEV_NetReqHead()
			{
				init();
			}

			bool isValid()
			{
				if(UTIL_CONV::Char2To_uint2(cbSize) != sizeof(BLUEV_NetReqHead))
					return false;

				if(0 != memcmp(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR)))
					return false;

				if(nMajorVer != BV_MAJORVER)
					return false;

				return true;
			}

			void init()
			{
				memset(this, 0, sizeof(*this));
				UTIL_CONV::uint2ToChar2(sizeof(BLUEV_NetReqHead), cbSize);
				nMajorVer = BV_MAJORVER;
				memcpy(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
			}

		};

		// �˽ṹ�Ĵ�СΪ32���ֽڡ�
		class BVNETLIB_API	BLUEV_NetAnsHead
		{
		public:
			unsigned char cbSize[2];		// size of this structure
			unsigned char nMajorVer;		// major version, must be BV_MAJORVER
			unsigned char nMinorVer;		// minor version, must be BV_MINORVER
			char	szMagicStr[8];		// must be $bluev$
			unsigned char nDataLen[4];		// data length followed 
			unsigned char nRetVal[2];
			unsigned char hasException;		// �Ƿ����˴���
			// 19 bytes here
			unsigned char bnRes[13];		// reserved

			BLUEV_NetAnsHead()
			{
				init();
			}

			void init()
			{
				memset(this, 0, sizeof(*this));
				UTIL_CONV::uint2ToChar2(sizeof(*this), cbSize);
				nMajorVer = BV_MAJORVER;
				memcpy(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
			}

			bool isValid()
			{
				if(UTIL_CONV::Char2To_uint2(cbSize) != sizeof(*this))
					return false;

				if(0 != memcmp(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR)))
					return false;
				
				if(nMajorVer != BV_MAJORVER)
					return false;

				return true;
			}
		};

		// �˽ṹ�Ĵ�СΪ32���ֽڡ�
		class	BVNETLIB_API	BLUEV_UDPReqHead
		{
		public:
			unsigned char	cbSize[2];			// size of this structure, ��ǰΪ32���ֽڡ�
			unsigned char	nMajorVer;			// major version, must be BV_MAJORVER
			unsigned char	nMinorVer;			// minor version, must be BV_MINORVER
			char	szMagicStr[8];		// must be $bluev$
			unsigned char	nOpClass[2];		// operation class, 2 bytes int
			unsigned char	nOpCode[2];			// operation code
			unsigned char	nDataLen[2];		// data length followed max 2^16 = 64k
			// ��������18���ֽڡ� 
			unsigned char	bnRes[14];
			// ��������32���ֽڡ�

			bool	isValid()
			{
				if ( UTIL_CONV::Char2To_uint2(cbSize)!=sizeof(*this) ) return false;
				if ( memcmp(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR))!=0 ) return false;
				if ( nMajorVer != BV_MAJORVER ) return false;
				return	true;
			}

			void	init()
			{
				memset(this, 0, sizeof(*this));
				UTIL_CONV::uint2ToChar2(sizeof(*this), cbSize);
				memcpy(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
				nMajorVer = BV_MAJORVER;
			}

			BLUEV_UDPReqHead()
			{
				init();
			}
		};

		// �˽ṹ�Ĵ�СΪ32���ֽڡ�
		class	BVNETLIB_API	BLUEV_UDPAnsHead
		{
		public:
			unsigned char	cbSize[2];			// size of this structure, ��ǰΪ32���ֽڡ�
			unsigned char	nMajorVer;			// major version, must be BV_MAJORVER
			unsigned char	nMinorVer;			// minor version, must be BV_MINORVER
			char	szMagicStr[8];		// must be $bluev$
			unsigned char	nOpClass[2];		// operation class, 2 bytes int
			unsigned char	nOpCode[2];			// operation code
			unsigned char	nDataLen[2];		// data length followed max 2^16 = 64k
			unsigned char	nRetVal[2];			// ����ֵ
			//20 bytes here
			unsigned char	bnRes[12];

			bool	isValid()
			{
				if ( UTIL_CONV::Char2To_uint2(cbSize)!=sizeof(*this) ) return false;
				if ( memcmp(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR))!=0 ) return false;
				if ( nMajorVer != BV_MAJORVER ) return false;
				return	true;
			}

			void	init()
			{
				memset(this, 0, sizeof(*this));
				UTIL_CONV::uint2ToChar2(sizeof(*this), cbSize);
				memcpy(szMagicStr, BVNRHO_MAGICSTR, strlen(BVNRHO_MAGICSTR));
				nMajorVer = BV_MAJORVER;
			}

			BLUEV_UDPAnsHead()
			{
				init();
			}
		};
	}
}

#endif	//_INC_BLUEVREQANS_H_