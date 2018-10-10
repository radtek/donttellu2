#ifndef DATAPOOL_H_INCLUDED
#define DATAPOOL_H_INCLUDED
#include "Common.h"
#include <netinet/in.h>

int InitDataPool(void);
void ReleaseDataPool(void);
//int AddTaskData(LPTASKDATANODE_s pData);
//LPTASKDATANODE_s GetTaskData(void);
int AddTaskData(void);

#endif // DATAPOOL_H_INCLUDED
