//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GflsCaptMosaic.cpp 拼接功能
//
//

#include "StdAfx.h"
#include "GflsScanerTest.h"
#include "GflsScanerTestView.h"

GFLS_CAPTMOSIACPARAM	g_stCaptMosiac;

int Gfls_Thread_InitParam()
{
//	g_stCaptMosiac.nCapMenCnt	= 2;
	g_stCaptMosiac.nCapMenSize	= GFLS_SCANNER_IMAGEWIDTH * GFLS_SCANNER_IMAGEHEIGHT;
	
	UCHAR *pData;
	int i, nret;

	nret = -1;
	for(i = 0; i < 2; i++)
	{
		pData = (UCHAR*)malloc(g_stCaptMosiac.nCapMenSize);
		if(!pData)
			goto Finish_Exit;
		memset(pData, 255, g_stCaptMosiac.nCapMenSize);
		g_stCaptMosiac.stCapMemory[i].pData   = pData;
		g_stCaptMosiac.stCapMemory[i].bIsFull = FALSE;
	}

	pData = (UCHAR*)malloc(g_stCaptMosiac.nCapMenSize);
	if(!pData)
		goto Finish_Exit;
	memset(pData, 255, g_stCaptMosiac.nCapMenSize);
	g_stCaptMosiac.pBuffer = pData;
	g_stCaptMosiac.bInitialed = 1;

	g_stCaptMosiac.dwWaitTime = INFINITE;
	
	nret = 1;

Finish_Exit:
	if(nret < 1)
		Gfls_Thread_Finish();
	return nret;
}

int  Gfls_Thread_Finish()
{
	int i;

	for(i = 0; i < 2; i++)
	{
		if(g_stCaptMosiac.stCapMemory[i].pData)
			free(g_stCaptMosiac.stCapMemory[i].pData);
	}
	memset(g_stCaptMosiac.stCapMemory, 0, sizeof(g_stCaptMosiac.stCapMemory));


	Gfls_Thread_CloseSemaphore();
	if(g_stCaptMosiac.pBuffer)
		free(g_stCaptMosiac.pBuffer);
	g_stCaptMosiac.pBuffer = NULL;

	g_stCaptMosiac.bInitialed = 0;

	return 1;
}

void Gfls_Thread_ResetThread()
{
	if(!g_stCaptMosiac.bInitialed)
		return;

	Gfls_Thread_CloseSemaphore();

	int i;

	for(i = 0; i < 2; i++)
	{
		memset(g_stCaptMosiac.stCapMemory[i].pData, 255, g_stCaptMosiac.nCapMenSize);
		g_stCaptMosiac.stCapMemory[i].bIsFull = FALSE;
	}

	g_stCaptMosiac.hCaptSemaphore   = CreateSemaphore(NULL, 2, 2, NULL);
	g_stCaptMosiac.hMosaicSemaphore = CreateSemaphore(NULL, 0, 2, NULL);
}

void Gfls_Thread_ReleaseSemaphore()
{
	if(g_stCaptMosiac.hCaptSemaphore)
	{
		ReleaseSemaphore(g_stCaptMosiac.hCaptSemaphore, 1, NULL);
	}

	if(g_stCaptMosiac.hMosaicSemaphore)
	{
		ReleaseSemaphore(g_stCaptMosiac.hMosaicSemaphore, 1, NULL);
	}
}

void Gfls_Thread_CloseSemaphore()
{
	if(g_stCaptMosiac.hCaptSemaphore)
	{
		CloseHandle(g_stCaptMosiac.hCaptSemaphore);
		g_stCaptMosiac.hCaptSemaphore = NULL;
	}
	
	if(g_stCaptMosiac.hMosaicSemaphore) 
	{
		CloseHandle(g_stCaptMosiac.hMosaicSemaphore);
		g_stCaptMosiac.hMosaicSemaphore = NULL;
	}
}

DWORD WINAPI Gfls_Thread_CaptImage(LPVOID pParam)
{
	char strFileName[512] = {0};
	strcpy(strFileName,"caiji.log");
	int m_bWriten = 0,m_bW = 0;
	CString temp,temp1;
	CFile mFile;
	CFileException e;
	if(!g_stCaptMosiac.bInitialed)
		return 0;
	CGflsScanerTestView *pView = (CGflsScanerTestView*)pParam;
	if(!pView)
		return 0;

	int i;
	GFLS_CAPTDATAMEMORY	*pstMemory;
	DWORD dwSign;
	int nret;
	if (reType == 4)
{
	nret = g_stLiveCaptDll.pfnLiveScan_BeginCapture(g_stScannerInfo.nCurChannel);

}
else
{
	nret = g_stCaptDll.pfnScan_BeginCapture(g_stScannerInfo.nCurChannel);

}
	if(nret != 1)
	{
		ShowScannerErrorInfo(nret, pView, 1);
		return 0;
	}

	// 开始采集
	while(!g_stScannerInfo.bExitMosaic)
	{
		pstMemory = g_stCaptMosiac.stCapMemory;
		for(i = 0; i < 2; i++, pstMemory++)
		{
			if(g_stScannerInfo.bExitMosaic)
				break;
			dwSign = WaitForSingleObject(g_stCaptMosiac.hCaptSemaphore, g_stCaptMosiac.dwWaitTime);
			if(g_stScannerInfo.bExitMosaic)
				break;
			if(dwSign != WAIT_OBJECT_0)
				continue;
			if (reType == 4)
{
						if(!pstMemory->bIsFull)
			{
				memset(pstMemory->pData, 255, g_stCaptMosiac.nCapMenSize);
				if(g_stLiveCaptDll.pfnLiveScan_GetFPRawData(g_stScannerInfo.nCurChannel, pstMemory->pData) > 0)
				{
					pstMemory->bIsFull = TRUE;
				}
//				nret =g_stMosaicDll.pfnMosaic_IsFinger(pstMemory->pData,640,640);			
			}

}
else
{
					if(!pstMemory->bIsFull)
			{
				memset(pstMemory->pData, 255, g_stCaptMosiac.nCapMenSize);
				if(g_stCaptDll.pfnScan_GetFPRawData(g_stScannerInfo.nCurChannel, pstMemory->pData) > 0)
				{
					pstMemory->bIsFull = TRUE;
				}
//				nret =g_stMosaicDll.pfnMosaic_IsFinger(pstMemory->pData,640,640);			

}
			}
			ReleaseSemaphore(g_stCaptMosiac.hMosaicSemaphore, 1, NULL);
		}
	}
	if (reType == 4)
{
				g_stLiveCaptDll.pfnLiveScan_EndCapture(g_stScannerInfo.nCurChannel);

}
else
{
			g_stCaptDll.pfnScan_EndCapture(g_stScannerInfo.nCurChannel);
}

	
	return 1;
}

DWORD WINAPI Gfls_Thread_MosaicImage(LPVOID pParam)
{
	
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
	int m_bWriten = 0,n,m_bW = 0;
	CStringA temp,temp1;
	CFile mFile;
	CFileException e;
    DWORD dwPos;
	if(!g_stCaptMosiac.bInitialed)
		return 0;
	CGflsScanerTestView *pView = (CGflsScanerTestView*)pParam;
	if(!pView)
		return 0;

	int i, nret, bSupport;
	GFLS_CAPTDATAMEMORY	*pstMemory;
	DWORD dwSign;

	bSupport = g_stMosaicDll.pfnMosaic_IsSupportIdentifyFinger();

	g_stScannerInfo.nCaptFrames = 0;
	while(!g_stScannerInfo.bExitMosaic)
	{
		pstMemory = g_stCaptMosiac.stCapMemory;
		for(i = 0; i < 2; i++, pstMemory++)
		{
			if(g_stScannerInfo.bExitMosaic)
				break;
			dwSign = WaitForSingleObject(g_stCaptMosiac.hMosaicSemaphore, g_stCaptMosiac.dwWaitTime);
			if(g_stScannerInfo.bExitMosaic)
				break;
			if(dwSign != WAIT_OBJECT_0)
				continue;
			if(pstMemory->bIsFull && pstMemory->pData)
			{
				memcpy(g_stCaptMosiac.pBuffer, pstMemory->pData, g_stCaptMosiac.nCapMenSize);
			}
			pstMemory->bIsFull = FALSE;
			ReleaseSemaphore(g_stCaptMosiac.hCaptSemaphore, 1, NULL);

			// 进行拼接
			nret = 1;
//			if(bSupport && g_stScannerInfo.nCaptFrames == 0)
//				nret = g_stMosaicDll.pfnMosaic_IsFinger(g_stCaptMosiac.pBuffer, pView->m_stParamInfo.nImgWidth, pView->m_stParamInfo.nImgHeight);
			
			if(nret == 1)
			{
				if(g_stScannerInfo.nCaptFrames == 0)
				{
					n = g_stMosaicDll.pfnMosaic_Start(pView->m_stParamInfo.pSrcImage, pView->m_stParamInfo.nImgWidth, pView->m_stParamInfo.nImgHeight);
				
					temp1.Format("%d",n);
					if (n == 1)
					{
						temp += "测试\"5.9初始化拼接过程\"成功 返回值="+temp1+"\r\n";
					}
					else
					{
						temp += "测试\"5.9初始化拼接过程\"失败 返回值="+temp1+"\r\n";
						continue;
					}
				}
				nret = g_stMosaicDll.pfnMosaic_DoMosaic(g_stCaptMosiac.pBuffer, pView->m_stParamInfo.nImgWidth, pView->m_stParamInfo.nImgHeight);		
				if(m_bW == 0)
				{
					temp1.Format("%d",nret);
					if (nret == 1) 
					{
						temp += "测试\"5.10拼接过程\"成功 返回值="+temp1+"\r\n";
					}
					else
					{
						temp += "测试\"5.10拼接过程\"失败 返回值="+temp1+"\r\n";
					}
					m_bW = 1;
				}
		
				CFileStatus status;
				if(mFile.GetStatus(_T("pinjie.log"),status))
				{
					if(m_bWriten == 0)
					{
						mFile.Open(_T("pinjie.log"), CFile::modeWrite,&e);
						mFile.SeekToEnd();
						dwPos = (DWORD)mFile.GetPosition();
						mFile.LockRange(dwPos, 1);
						mFile.Write(temp,temp.GetLength());
						mFile.UnlockRange(dwPos,1);
						mFile.Flush();
						mFile.Close(); 
						m_bWriten = 1;
					}
				}
				g_stScannerInfo.nCaptFrames++;
			
				pView->ZoomImageWithStyle();
				pView->SendStatImageDataMessage(0, 1);
			}

			if(nret == 1)
				continue;
			else
			{
				if(g_stScannerInfo.nCaptFrames > 0)	
				{
					n = g_stMosaicDll.pfnMosaic_Stop();	
					temp1.Format("%d",n);
					if (n == 1) 
					{
						temp += "测试\"5.11结束拼接\"成功 返回值="+temp1+"\r\n";
					}	
					else
					{
						temp += "测试\"5.11结束拼接\"失败 返回值="+temp1+"\r\n";
					}
					PutInfo2Log(strFileName,temp);
				}
				g_stScannerInfo.nCaptFrames = 0;
			}
			
/*			switch(nret)
			{
			case IM_EN_IDLE:
				Sleep(100);					
				break;

			case IM_EN_TOOQUICK:
			case IM_EN_ROLLBACK:
			case IM_EN_OVERTIME:
			case IM_EN_END:
				n = g_stMosaicDll.pfnMosaic_Stop();	
				temp1.Format("%d",n);
				if (n == 1) 
				{
					temp += "测试\"结束拼接\"成功 返回值="+temp1+"\r\n";
				}	
				else
				{
					temp += "测试\"结束拼接\"失败 返回值="+temp1+"\r\n";
				}
				PutInfo2Log(strFileName,temp);
				g_stScannerInfo.nCaptFrames = 0;
				break;
			}
*/
		}
	}
	
	return 1;
}

DWORD WINAPI Gfls_Thread_CaptAndMosaicImage( LPVOID pParam )
{
	CStringA temp,temp1;
	CFile mFile;
	CFileException e;
	int m_bwriten = 0;
	char strFileName[512] = {0};
	strcpy(strFileName,"pinjie.log");
	if(!g_stCaptMosiac.bInitialed)
		return 0;
	CGflsScanerTestView *pView = (CGflsScanerTestView*)pParam;
	if(!pView)
		return 0;
	int nret, bSupport, nsuccCount;
	UCHAR *pTempData = g_stCaptMosiac.stCapMemory[0].pData;

	bSupport = g_stMosaicDll.pfnMosaic_IsSupportIdentifyFinger();
	temp1.Format("%d",bSupport);
	if (bSupport < 0) 
	{
		temp += "测试\"5.3拼接接口是否提供判断图像为指纹的函数\"失败 返回值= "+temp1+" \r\n";
	}
	else if(bSupport == 1||bSupport == 0)
	{
		temp += "测试\"5.3拼接接口是否提供判断图像为指纹的函数\"成功 返回值= "+temp1+"\r\n";
	}
	while(!g_stScannerInfo.bExitMosaic)
	{
		while(!g_stScannerInfo.bExitMosaic)
		{
		if (reType == 4)
{
			nret = g_stLiveCaptDll.pfnLiveScan_BeginCapture(g_stScannerInfo.nCurChannel);
			if(nret != 1)
			{
				ShowScannerErrorInfo(nret, pView, 1);
				return 0;
			}
			memset(pTempData, 255, g_stCaptMosiac.nCapMenSize);
			nret = g_stLiveCaptDll.pfnLiveScan_GetFPRawData(g_stScannerInfo.nCurChannel, pTempData);
			if(nret != 1)
			{
				ShowScannerErrorInfo(nret, pView, 1);
				return 0;
			}
			nret = g_stLiveCaptDll.pfnLiveScan_EndCapture(g_stScannerInfo.nCurChannel);
			temp1.Format("%d",nret);

			
}
else
{
					nret = g_stCaptDll.pfnScan_BeginCapture(g_stScannerInfo.nCurChannel);
			if(nret != 1)
			{
				ShowScannerErrorInfo(nret, pView, 1);
				return 0;
			}
			memset(pTempData, 255, g_stCaptMosiac.nCapMenSize);
			nret = g_stCaptDll.pfnScan_GetFPRawData(g_stScannerInfo.nCurChannel, pTempData);
			if(nret != 1)
			{
				ShowScannerErrorInfo(nret, pView, 1);
				return 0;
			}
			nret = g_stCaptDll.pfnScan_EndCapture(g_stScannerInfo.nCurChannel);
			temp1.Format("%d",nret);

}
	
			if(bSupport)
			{
				nret = g_stMosaicDll.pfnMosaic_IsFinger(pTempData, pView->m_stParamInfo.nImgWidth, pView->m_stParamInfo.nImgHeight);
				if (nret < 0) 
				{temp += "测试\"判断图像为指纹\"失败 返回值= "+temp1+" \r\n";}
				else if(nret == 1||nret == 0)
				{
					temp += "测试\"判断图像为指纹\"成功 返回值= "+temp1+"\r\n";
				}
			}
			else
				nret = 1;
			if(nret == 1)
				break;
		}
		if(g_stScannerInfo.bExitMosaic)
			return 0;
		memcpy(g_stCaptMosiac.pBuffer, pTempData, g_stCaptMosiac.nCapMenSize);
//		memcpy(pView->m_stParamInfo.pSrcImage, pTempData, g_stCaptMosiac.nCapMenSize);
		nret = g_stMosaicDll.pfnMosaic_Start(pView->m_stParamInfo.pSrcImage, pView->m_stParamInfo.nImgWidth, pView->m_stParamInfo.nImgHeight);
		temp1.Format("%d",nret);
		if (nret == 1)
		{
			temp += "测试\"5.9初始化拼接过程\"成功 返回值="+temp1+"\r\n";
			nsuccCount = 0;
		}
		else
		{
			temp += "测试\"5.9初始化拼接过程\"失败 返回值="+temp1+"\r\n";
		}
		if(nret != 1)
		{
			ShowScannerErrorInfo(nret, pView, 1);
			return 0;
		}
		while(nret == 1)
		{
			if (reType == 4)
{
						g_stLiveCaptDll.pfnLiveScan_BeginCapture(g_stScannerInfo.nCurChannel);
			g_stLiveCaptDll.pfnLiveScan_GetFPRawData(g_stScannerInfo.nCurChannel, g_stCaptMosiac.pBuffer);
			g_stLiveCaptDll.pfnLiveScan_EndCapture(g_stScannerInfo.nCurChannel);

}
else
{
					g_stCaptDll.pfnScan_BeginCapture(g_stScannerInfo.nCurChannel);
			g_stCaptDll.pfnScan_GetFPRawData(g_stScannerInfo.nCurChannel, g_stCaptMosiac.pBuffer);
			g_stCaptDll.pfnScan_EndCapture(g_stScannerInfo.nCurChannel);

}
			nret = g_stMosaicDll.pfnMosaic_DoMosaic(g_stCaptMosiac.pBuffer, pView->m_stParamInfo.nImgWidth, pView->m_stParamInfo.nImgHeight);
			if(nret == 1)
			{
				nsuccCount++;
				TRACE("nsuccCount = %d\n", nsuccCount);
			}
			if(nret == -500)
			{
				nret = 1;	
				continue;
			}

			if(nret == -1000)
			{
//				nret = 1;
//				if(1 > 0)
				{
					static int bCanSave = 0;
					char szName[128] = {0};
					sprintf(szName, "d:\\xgw\\gfls%03d.bmp", bCanSave);
					GFLS_IMAGEINFOSTRUCT stImageInfo = {0};
					stImageInfo.pImgData = g_stCaptMosiac.pBuffer;
					stImageInfo.prgbQuad = pView->m_stbmpInfo.bmiColors;
					memcpy(&stImageInfo.bmpHead, &pView->m_stbmpInfo.bmiHeader, sizeof(stImageInfo.bmpHead));
					stImageInfo.bmpHead.biWidth  =pView->m_stParamInfo.nImgWidth;
					stImageInfo.bmpHead.biHeight = pView->m_stParamInfo.nImgHeight;
					stImageInfo.bmpHead.biSizeImage = pView->m_stParamInfo.nImgSize;
					Gfls_SaveBmpImgFile(szName, &stImageInfo);
					bCanSave++;
					if(bCanSave > 1000)
						bCanSave = 0;
				}
			}

			if(m_bwriten== 0)
			{
				temp1.Format("%d",nret);
				if (nret == 1) 
				{
					temp += "测试\"5.10拼接过程\"成功 返回值="+temp1+"\r\n";
				}
				else
				{
					temp += "测试\"5.10拼接过程\"失败 返回值="+temp1+"\r\n";
				}
				m_bwriten = 1;
				
			}

			pView->ZoomImageWithStyle();
			pView->SendStatImageDataMessage(0, 1);
		}
		nret = g_stMosaicDll.pfnMosaic_Stop();
		temp1.Format("%d",nret);
		if (nret == 1) 
		{
			temp += "测试\"5.11结束拼接\"成功 返回值="+temp1+"\r\n";
		}	
		else
		{
			temp += "测试\"5.11结束拼接\"失败 返回值="+temp1+"\r\n";
		}
		PutInfo2Log(strFileName,temp);
	}
	return 1;
}

void StartFingerCaptureThread(LPVOID pParam, int bRollFinger)
{
	CStringA temp,temp1;
	CFile mFile;
	CFileException e;
	int m_bwriten = 0;
	char strFileName[512] = {0};
	char szInfo1[1024] = {0};
	strcpy(strFileName,"pinjie.log");
	int n;
	if(g_stScannerInfo.bHasMosaicThrd && (bRollFinger == g_stMosaicDll.bMosaicRoll))
	{
		StopCaptureThread();
		return;		
	}

	if(g_stScannerInfo.hCaptThread)
	
		StopCaptureThread();


	
	g_stScannerInfo.nCaptFrames = 0;


//	if(n != bRollFinger)
//	{
//			MessageBox(NULL,"返回值不等于修改前的采集方式值!", "活体采集测试程序", MB_OK);
//			return;
	/*}*/

	n = g_stMosaicDll.pfnMosaic_SetRollMode(bRollFinger);
 	temp1.Format("%d",n);
	if (n < 0) 
	{
		temp += "测试\"5.8选择拼接方式的函数\"失败 返回值="+temp1+"\r\n ";
	}
	else
	{
		temp += "测试\"5.8选择拼接方式的函数\"成功 返回值="+temp1+"\r\n ";
	}	
	PutInfo2Log(strFileName,temp);

	g_stMosaicDll.bMosaicRoll = bRollFinger;
	if(g_stScannerInfo.bHasMosaicThrd)
	{
		return;
	}

	DWORD threadID = 0;

	if(g_stCaptMosiac.hCaptureThread)
		CloseHandle(g_stCaptMosiac.hCaptureThread);
	if(g_stCaptMosiac.hMosaicThread)
		CloseHandle(g_stCaptMosiac.hMosaicThread);

	Gfls_Thread_ResetThread();

	g_stScannerInfo.bExitMosaic = 0;
	
	//g_stCaptMosiac.hCaptureThread = CreateThread(NULL, 0, Gfls_Thread_CaptImage,   pParam, 0, &threadID);
//	g_stCaptMosiac.hMosaicThread  = CreateThread(NULL, 0, Gfls_Thread_MosaicImage, pParam, 0, &threadID);
	g_stCaptMosiac.hMosaicThread  = CreateThread(NULL, 0, Gfls_Thread_CaptAndMosaicImage, pParam, 0, &threadID);

	g_stScannerInfo.bHasMosaicThrd = 1;
	
	
}

void TerminateCaptureThread()
{
//	g_stScannerInfo.bExitMosaic = 1;
}

