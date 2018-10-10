#ifndef NETSOCKET_H_INCLUDED
#define NETSOCKET_H_INCLUDED
#include "Common.h"

#define _NETSOCKET_DEBUG_
//**** Enum ****
//**** Public Function ****
typedef void (*pfuncDoNetSocket)(LPTASKDATANODE_s pNode);
int InitUdpServer(void);
int StartUdpServer(pfuncDoNetSocket pfunc);
void StopUdpServer(void);
//**** Send Cmd ****
void ReplyClient(const LPTASKDATANODE_s pNode);
void ReplyAllClient(void);
//int ReplyCtrlTD(const LPTASKDATANODE_s pNode);
//int ReplyTDState(const LPTASKDATANODE_s pNode);
#endif // NETSOCKET_H_INCLUDED
