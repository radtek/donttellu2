///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GflsCaptMosaic.h	define const and data struct for capture and mosaic
//	CreateTime : 2006-04-27
//  Creater: Beagle
//

#ifndef	_INC_GFLSCAPTMOSAIC_H_
#define _INC_GFLSCAPTMOSAIC_H_


typedef struct tagGfls_CaptDataMemory
{
	UCHAR	*pData;		// 获取的指纹图像数据
	BOOL	bIsFull;	// 是否已经得到指纹图像数据
}GFLS_CAPTDATAMEMORY;


typedef struct tagGfls_CaptMosiacParam
{
	GFLS_CAPTDATAMEMORY	stCapMemory[2];		// only for finger capture
	UCHAR				*pBuffer;			// 在拼接时最后得到的缓冲区，其大小为nCapMenSize

	HANDLE			hCaptSemaphore;		// 捕获信号量
	HANDLE			hMosaicSemaphore;	// 拼接信号量

	HANDLE			hCaptureThread;
	HANDLE			hMosaicThread;

//	int				nCapMenCnt;			// GFLS_IMGMEMORY的个数
	int				nCapMenSize;		// GFLS_IMGMEMORY成员pData的大小
	DWORD			dwWaitTime;

	UCHAR			bInitialed;			// 是否已经初始化
	UCHAR			nReserved[4];
}GFLS_CAPTMOSIACPARAM;

extern GFLS_CAPTMOSIACPARAM	g_stCaptMosiac;

int Gfls_Thread_InitParam();
int Gfls_Thread_Finish();
void Gfls_Thread_ResetThread();
void Gfls_Thread_ReleaseSemaphore();
void Gfls_Thread_CloseSemaphore();

extern DWORD WINAPI Gfls_Thread_CaptImage( LPVOID pParam );
extern DWORD WINAPI Gfls_Thread_MosaicImage( LPVOID pParam );
extern DWORD WINAPI Gfls_Thread_CaptAndMosaicImage( LPVOID pParam );

void StartFingerCaptureThread(LPVOID pParam, int bRollFinger);
void TerminateCaptureThread();
 
#endif	// _INC_GFLSCAPTMOSAIC_H_