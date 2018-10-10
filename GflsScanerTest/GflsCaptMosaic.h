///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GflsCaptMosaic.h	define const and data struct for capture and mosaic
//	CreateTime : 2006-04-27
//  Creater: Beagle
//

#ifndef	_INC_GFLSCAPTMOSAIC_H_
#define _INC_GFLSCAPTMOSAIC_H_


typedef struct tagGfls_CaptDataMemory
{
	UCHAR	*pData;		// ��ȡ��ָ��ͼ������
	BOOL	bIsFull;	// �Ƿ��Ѿ��õ�ָ��ͼ������
}GFLS_CAPTDATAMEMORY;


typedef struct tagGfls_CaptMosiacParam
{
	GFLS_CAPTDATAMEMORY	stCapMemory[2];		// only for finger capture
	UCHAR				*pBuffer;			// ��ƴ��ʱ���õ��Ļ����������СΪnCapMenSize

	HANDLE			hCaptSemaphore;		// �����ź���
	HANDLE			hMosaicSemaphore;	// ƴ���ź���

	HANDLE			hCaptureThread;
	HANDLE			hMosaicThread;

//	int				nCapMenCnt;			// GFLS_IMGMEMORY�ĸ���
	int				nCapMenSize;		// GFLS_IMGMEMORY��ԱpData�Ĵ�С
	DWORD			dwWaitTime;

	UCHAR			bInitialed;			// �Ƿ��Ѿ���ʼ��
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