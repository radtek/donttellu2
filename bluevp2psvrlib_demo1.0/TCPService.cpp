#include "bluevp2psvrdef.h"

namespace BLUEV{
	namespace BVP2PSVRLIB{

		using BLUEVBASE::BluevError;
		int TCPService::execute(BLUEVConnection &conn, ServiceParamContext &ctx)
		{
			int retval = -1;

			try
			{
				if((retval = doWork(conn, ctx)) < 0)
					throw BluevError(__FILE__, __LINE__);
			}
			catch(BluevError &e)
			{
				e.print();
				//
			}
			catch(...)
			{

			}

			return retval;				
		}

	}
}