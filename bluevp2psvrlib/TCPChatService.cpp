#include "bluevp2psvrdef.h"

namespace BLUEV{
	namespace BVP2PSVRLIB{
		
		using BLUEVNET::BLUEV_NetReqHead;
		using BLUEVNET::BLUEV_NetAnsHead;
		using BLUEVNET::BLUEVOpClass;
		using BLUEVBASE::UTIL_CONV;
		using BLUEVBASE::BluevError;

		int TCPChatService::doWork(BLUEVConnection &conn, ServiceParamContext &ctx)
		{
			BLUEV_NetReqHead	&req = conn.req;
			BLUEV_NetAnsHead	&ans = conn.ans;

			int	retval = -1;
			
			int	opCode = UTIL_CONV::Char2To_uint2(req.nOpCode);

			switch(opCode)
			{
			case BLUEVOpClass::OP_CHAT_MIRROR:
				retval = chatMirror(conn, ctx);
				break;
			default:
				return -1;
			}

			return retval;
		}

		int TCPChatService::chatMirror(BLUEVConnection& conn, ServiceParamContext& ctx)
		{
			if(ctx.pReq == NULL || ctx.nReqLen <= 0)
				return -1;

			ctx.pAns = new unsigned char[ctx.nReqLen];
			if(ctx.pAns == NULL)
				return -1;

			memcpy(ctx.pAns, ctx.pReq, ctx.nReqLen);
			ctx.nAnsLen = ctx.nReqLen;

			std::string strLog = "ÓÐtcp¿Í»§chat mirror:";
			strLog.append((char *)ctx.pAns, ctx.nAnsLen);
			strLog.append("\r\n");

			std::cout << strLog;

			return 0;
		}
	}
}