#ifndef _INC_BLUEVP2PSVRMAIN_H_201412081031_
#define _INC_BLUEVP2PSVRMAIN_H_201412081031_

#include "IServiceStatus.h"

namespace BLUEV
{
	namespace BVP2PSVRLIB
	{
		using BLUEV::BLUEVBASE::IServiceStatus;

		extern BVP2PSVRLIB_API
			int BLUEVP2P_SvrMain(int argc, wchar_t **argv, IServiceStatus& svcStatus);
	};
};

#endif	//_INC_BLUEVP2PSVRMAIN_H_201412081031_