// ImageBadptStat.cpp: implementation of the CImageBadptStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gflsscanertest.h"
#include "ImageBadptStat.h"
#include "GflsScanerTestView.h"
#include "MainFrm.h"
#include "DlgGrayStatistic.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GFLS_ISBADPOINT(diff)			(((diff) > 25 || (diff < -25)) ? 1 : 0)
#define GFLS_BADPTINCENTERAREA(x, y)	(((x) >= 20 && (x) < 620 && (y) >= 20 && (y) < 620) ? 1 : 0)
#define GFLS_ABSOLUTIONVALUE(x)			(((x) >= 0) ? (x) : -(x))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageBadptStat::CImageBadptStat()
{
	m_pTestView = NULL;
	m_pBadInfoData = NULL;
	memset(&m_astBadData, 0, sizeof(m_astBadData));
	m_astBadData.nStep = 128;
	InitBadptInfoData();
}

CImageBadptStat::~CImageBadptStat()
{
	if(m_pBadInfoData)
		free(m_pBadInfoData);
	m_pBadInfoData = NULL;
	FreeBadDataArray();
}

void CImageBadptStat::InitBadptInfoData()
{
	m_nAverage		 = 0;
	m_nSmallGrayCnt	 = 0;
	m_nAllBad1pt	 = 0;
	m_nAllBad2pt	 = 0;
	m_nAllBad3pt	 = 0;
	m_nCnrBad1pt	 = 0;
	m_nCnrBad2pt	 = 0;
	m_nCnrBad3pt	 = 0;	
	ClearOldDataInBadDataArray();
}

void CImageBadptStat::FreeBadDataArray()
{
	if(NULL == m_astBadData.pstBadData)
		return;

	int i;
	GF_BADPOINTDATA* pstPointData = m_astBadData.pstBadData;
	for(i = 0; i < m_astBadData.nCurDataBuf; i++, pstPointData++)
	{
		if(pstPointData->paBadPoint)
			free(pstPointData->paBadPoint);
	}
	free(m_astBadData.pstBadData);
	memset(&m_astBadData, 0, sizeof(m_astBadData));
}

void CImageBadptStat::ClearOldDataInBadDataArray()
{
	if(NULL == m_astBadData.pstBadData)
		return;

	int i;
	GF_BADPOINTDATA* pstPointData = m_astBadData.pstBadData;
	for(i = 0; i < m_astBadData.nCurDataBuf; i++, pstPointData++)
	{
		if(pstPointData->paBadPoint)
			free(pstPointData->paBadPoint);
		memset(pstPointData, 0, sizeof(*pstPointData));
	}
	m_astBadData.nBadData = 0;
}

int CImageBadptStat::AddBadPointData2Array(GF_BADPOINTDATA *pstPointData)
{
	if(m_astBadData.nBadData == m_astBadData.nCurDataBuf)
	{
		int nBuffer = m_astBadData.nCurDataBuf + m_astBadData.nStep;
		GF_BADPOINTDATA *pstNewArray = (GF_BADPOINTDATA*)malloc(nBuffer * sizeof(GF_BADPOINTDATA));
		if(NULL == pstNewArray)
			return -1;
		memset(pstNewArray, 0, sizeof(GF_BADPOINTDATA) * nBuffer);
		if(m_astBadData.pstBadData)
		{
			if(m_astBadData.nBadData > 0)
				memcpy(pstNewArray, m_astBadData.pstBadData, m_astBadData.nBadData * sizeof(GF_BADPOINTDATA));
			free(m_astBadData.pstBadData);
		}
		m_astBadData.pstBadData  = pstNewArray;
		m_astBadData.nCurDataBuf = nBuffer;
	}
	memcpy(m_astBadData.pstBadData + m_astBadData.nBadData, pstPointData, sizeof(*pstPointData));
	m_astBadData.nBadData++;
	return 1;
}

void CImageBadptStat::CheckImageBadPoint()
{
	if(NULL == m_pBadInfoData)
	{
		m_pBadInfoData = (UCHAR*)malloc(m_pTestView->m_stParamInfo.nImgSize);
		if(NULL == m_pBadInfoData)
			return;
	}

	GF_BADPOINTINFO	stBadInfo = {0};
	stBadInfo.paBadPoint = (POINT*)malloc(sizeof(POINT) * m_pTestView->m_stParamInfo.nImgSize);
	if(NULL == stBadInfo.paBadPoint)
		return;
	memset(m_pBadInfoData, 255, m_pTestView->m_stParamInfo.nImgSize);
	
	InitBadptInfoData();
	m_nAverage = (int)(m_pTestView->m_dlgGrayStat.m_staticGrayValue.m_stGrayStat.fAverage + 0.5);

	UCHAR *pSrcData, *pBadpt;
	pSrcData = m_pTestView->m_stParamInfo.pSrcImage;
	pBadpt   = m_pBadInfoData;

	int row, col, ndiff;
	for(row = 0; row < GFLS_SCANNER_IMAGEHEIGHT; row++)
	{
		for(col = 0; col < GFLS_SCANNER_IMAGEWIDTH; col++, pSrcData++, pBadpt++)
		{
			if(0 == *pBadpt)	// 已经是一个直径大于1的疵点了
				continue;
			ndiff = *pSrcData - m_nAverage;
			if(!GFLS_ISBADPOINT(ndiff))
				continue;
			*pBadpt = 0;

			stBadInfo.paBadPoint->x = col;
			stBadInfo.paBadPoint->y = row;
			stBadInfo.bCenter		= 0;
			stBadInfo.nBadptCount	= 1;
			stBadInfo.nIndex		= 0;
			stBadInfo.xRadio		= 1;
			stBadInfo.yRadio		= 1;
			stBadInfo.nRadio		= 1;

			GetBadPointRadia(stBadInfo);
			if(stBadInfo.nRadio >= 3)
			{
				m_nAllBad3pt++;
				m_nAllBad2pt++;
				if(stBadInfo.bCenter)
				{
					m_nCnrBad3pt++;
					m_nCnrBad2pt++;
				}
			}
			else if(stBadInfo.nRadio >= 2)
			{
				m_nAllBad2pt++;
				if(stBadInfo.bCenter)
					m_nCnrBad2pt++;
			}
			else
			{
				m_nAllBad1pt++;
				if(stBadInfo.bCenter)
					m_nCnrBad1pt++;
			}
		}
	}

	free(stBadInfo.paBadPoint);
	m_pTestView->Invalidate(FALSE);
}

int CImageBadptStat::GetBadPointRadia(GF_BADPOINTINFO &stBadInfo)
{
	if(IsRightPointBad(stBadInfo))
	{
		GetBadPointRadia(stBadInfo);
	}
	if(IsLeftBottomPointBad(stBadInfo))
	{
		GetBadPointRadia(stBadInfo);
	}
	if(IsBottomPointBad(stBadInfo))
	{
		GetBadPointRadia(stBadInfo);
	}
	if(IsRightBottomPointBad(stBadInfo))
	{
		GetBadPointRadia(stBadInfo);
	}

	int i, nradio, ntmp;
	POINT *lptmp, *lpFirst;
	lpFirst = stBadInfo.paBadPoint;
	lptmp	= lpFirst + 1;
	for(i = 1; i < stBadInfo.nBadptCount; i++, lptmp++)
	{
		ntmp = lpFirst->x - lptmp->x;
		nradio = GFLS_ABSOLUTIONVALUE(ntmp);
		if(nradio > stBadInfo.nRadio)
			stBadInfo.nRadio = nradio;
		ntmp = lpFirst->y - lptmp->y;
		nradio = GFLS_ABSOLUTIONVALUE(ntmp);
		if(nradio > stBadInfo.nRadio)
			stBadInfo.nRadio = nradio;

		if(!stBadInfo.bCenter)
			stBadInfo.bCenter = GFLS_BADPTINCENTERAREA(lptmp->x, lptmp->y);
	}
	if(!stBadInfo.bCenter)
		stBadInfo.bCenter = GFLS_BADPTINCENTERAREA(lpFirst->x, lpFirst->y);
	
	if(stBadInfo.nBadptCount > 0)
	{
		GF_BADPOINTDATA stPointData = {0};
		stPointData.paBadPoint = (POINT*)malloc(sizeof(POINT) * stBadInfo.nBadptCount);
		if(stPointData.paBadPoint)
		{
			memcpy(stPointData.paBadPoint, stBadInfo.paBadPoint, sizeof(POINT) * stBadInfo.nBadptCount);
			stPointData.nBadptCount = stBadInfo.nBadptCount;
			stPointData.nRadio		= stBadInfo.nRadio;
			stPointData.bCenter		= stBadInfo.bCenter;
			AddBadPointData2Array(&stPointData);
		}
	}

	return stBadInfo.nRadio;
}

int CImageBadptStat::IsRightPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x + 1;	y = lpCur->y;
	if(x >= GFLS_SCANNER_IMAGEWIDTH)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CImageBadptStat::IsLeftBottomPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x -1;	y = lpCur->y + 1;
	if(x < 0 || y >= GFLS_SCANNER_IMAGEHEIGHT)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CImageBadptStat::IsBottomPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x;	y = lpCur->y + 1;
	if(y >= GFLS_SCANNER_IMAGEHEIGHT)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CImageBadptStat::IsRightBottomPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x + 1;	y = lpCur->y + 1;
	if(x >= GFLS_SCANNER_IMAGEWIDTH || y >= GFLS_SCANNER_IMAGEHEIGHT)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CImageBadptStat::AddBadPoint2Array(GF_BADPOINTINFO &stBadInfo, int x, int y)
{
	int nPos, ndiff;
	POINT *lpAdd;

	nPos  = y * GFLS_SCANNER_IMAGEWIDTH + x;
	if(0 == m_pBadInfoData[nPos])
		return 0;

	ndiff = m_pTestView->m_stParamInfo.pSrcImage[nPos] - m_nAverage;
	if(!GFLS_ISBADPOINT(ndiff))
		return 0;
	
	stBadInfo.nIndex++;
	stBadInfo.nBadptCount++;
	lpAdd = stBadInfo.paBadPoint + stBadInfo.nIndex;
	lpAdd->x = x;	lpAdd->y = y;
	m_pBadInfoData[nPos] = 0;

	return 1;
}

void CImageBadptStat::GetBadPointStatInfo(TCHAR *pszInfo)
{
	TCHAR szReturn[] = _T("\r\n");
	int n;

	n  = 0;
	n += _stprintf(pszInfo + n, _T("1、一共有%d个直径为1的疵点，其中%d个在主视场内%s"), m_nAllBad1pt, m_nCnrBad1pt, szReturn);
	n += _stprintf(pszInfo + n, _T("2、一共有%d个直径大于等于2的疵点，其中%d个在主视场内%s"), m_nAllBad2pt, m_nCnrBad2pt, szReturn);
	n += _stprintf(pszInfo + n, _T("3、一共有%d个直径大于等于3的疵点，其中%d个在主视场内%s"), m_nAllBad3pt, m_nCnrBad3pt, szReturn);
//	n += sprintf(pszInfo + n, "疵点要求：主视场范围内不能含有直径大于3个像素点的疵点；直径大于等于2个像素点的疵点在整个图像范围内不能够超过5个；");
//	n += sprintf(pszInfo + n, "直径为1个像素点的疵点在整个图像范围内不能超过50个。");
//	n += sprintf(pszInfo + n, "其中，主视场是指采集仪中心%d像素 x %d像素。", GFLS_SCANNER_CENTERWIDTH, GFLS_SCANNER_CENTERHEIGHT);
}

void CImageBadptStat::DrawBadPointInfo(CDC *pdc, int nw, int nh)
{
//	if(m_astBadData.nBadData < 1 || m_astBadData.nBadData > 50)
	if(m_astBadData.nBadData < 1 || m_nAllBad1pt + m_nAllBad2pt > 50)
		return;

	CRect rtArea;
	CBrush	brush, *poldbrush;
	COLORREF clText, clOld;
	CString strInfo;

	clText = RGB(255, 0, 0);
	clOld = pdc->SetTextColor(clText);

	brush.CreateStockObject(BS_HOLLOW);
	poldbrush = pdc->SelectObject(&brush);
	
	int i, j, nret, nLength;
	GF_BADPOINTDATA *pstPointData;
	POINT *ppt, ptTemp;

	nLength = 3;
	pstPointData = m_astBadData.pstBadData;
	for(i = 0; i < m_astBadData.nBadData; i++, pstPointData++)
	{
		nret = MakeRectIncPoints(pstPointData, rtArea);
		ppt = pstPointData->paBadPoint;
		for(j = 0; j < pstPointData->nBadptCount; j++, ppt++)
			pdc->Ellipse(ppt->x - nLength, ppt->y - nLength, ppt->x + nLength, ppt->y + nLength);

		strInfo.Format(_T("%d"), pstPointData->nBadptCount);
		if(nret == 0)
			pdc->TextOut(pstPointData->paBadPoint->x, pstPointData->paBadPoint->y, strInfo);
//		else if(nret == 1)
		else
		{
			ptTemp.x = (rtArea.left + rtArea.right) >> 1;
			ptTemp.y = (rtArea.top + rtArea.bottom) >> 1;
			pdc->TextOut(ptTemp.x, ptTemp.y, strInfo);
		}
//		else
//		{
//			pdc->DrawText(strInfo, rtArea, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
//		}
	}
	pdc->SetTextColor(clOld);
	pdc->SelectObject(poldbrush);
	brush.DeleteObject();
}

int CImageBadptStat::MakeRectIncPoints(GF_BADPOINTDATA *pstPointData, CRect &rtArea)
{
	if(pstPointData->nBadptCount < 2)
		return 0;

	POINT *ptArray = pstPointData->paBadPoint + 1;
	rtArea.left = rtArea.right = pstPointData->paBadPoint->x;
	rtArea.top = rtArea.bottom = pstPointData->paBadPoint->y;

	for(int i = 1; i < pstPointData->nBadptCount; i++, ptArray++)
	{
		if(rtArea.left > ptArray->x)
			rtArea.left = ptArray->x;
		if(rtArea.right < ptArray->x)
			rtArea.right = ptArray->x;
		if(rtArea.top > ptArray->y)
			rtArea.top = ptArray->y;
		if(rtArea.bottom < ptArray->y)
			rtArea.bottom = ptArray->y;
	}
	if(rtArea.Width() == 0 || rtArea.Height() == 0)
		return 1;
	return 2;
}
