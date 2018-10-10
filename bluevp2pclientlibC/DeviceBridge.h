#ifndef DEVICEBRIDGE_H_INCLUDED
#define DEVICEBRIDGE_H_INCLUDED
#include "Common.h"

int OpenDevice(const char* szName);
int CloseDevice(void);
int ShutOnLight(void);
int ShutOffLight(void);
int AutoRecall(struct timeval* ptmval);
int GetLightState(void);
void WatchComm(int* nRun);
//****Private Function
//static int DataReader(char buf[],int nLen);
//static int DataWriter(const char buf[],const int nLen);
#endif // DEVICEBRIDGE_H_INCLUDED
