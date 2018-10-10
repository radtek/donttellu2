// GfImageInfoStatic.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "GfImageInfoStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGfImageInfoStatic

CGfImageInfoStatic::CGfImageInfoStatic()
{
	m_pBadInfoData = NULL;
	m_pBadInfoData = NULL;
	m_nDataSize      = 0;
	m_bDrawBadptInfo = 0;
	InitBadptInfoData();
}

CGfImageInfoStatic::~CGfImageInfoStatic()
{
}


BEGIN_MESSAGE_MAP(CGfImageInfoStatic, CStatic)
	//{{AFX_MSG_MAP(CGfImageInfoStatic)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGfImageInfoStatic message handlers

void CGfImageInfoStatic::OnDestroy() 
{
	FreeBufferData();
	CStatic::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CGfImageInfoStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawImageOrBadptData(&dc);
	
	// Do not call CStatic::OnPaint() for painting messages
}

void CGfImageInfoStatic::InitBadptInfoData()
{
//	m_bDrawBadptInfo = 0;
	m_nAverage		 = 0;
	m_nSmallGrayCnt	 = 0;
	m_nAllBad1pt	 = 0;
	m_nAllBad2pt	 = 0;
	m_nAllBad3pt	 = 0;
	m_nCnrBad1pt	 = 0;
	m_nCnrBad2pt	 = 0;
	m_nCnrBad3pt	 = 0;
}

void CGfImageInfoStatic::InitialBufData()
{
	m_nDataSize = GFLS_SCANNER_IMAGEWIDTH * GFLS_SCANNER_IMAGEHEIGHT;
	m_pImageData = (UCHAR*)malloc(m_nDataSize);
	if(m_pImageData)
		memset(m_pImageData, 255, m_nDataSize);
	m_pBadInfoData = (UCHAR*)malloc(m_nDataSize);
	if(m_pBadInfoData)
		memset(m_pBadInfoData, 255, m_nDataSize);
}

void CGfImageInfoStatic::FreeBufferData()
{
	if(m_pBadInfoData)
		free(m_pBadInfoData);
	if(m_pImageData)
		free(m_pImageData);
	m_pBadInfoData = NULL;
	m_pImageData   = NULL;
}

void CGfImageInfoStatic::DrawImageOrBadptData(CDC* pDC)
{
	CRect rtBack;
//	GetClientRect(rtBack);
	rtBack.SetRect(0, 0, GFLS_SCANNER_IMAGEWIDTH, GFLS_SCANNER_IMAGEHEIGHT);
	
	CDC dc;
	CBitmap bitmap, *poldbmp;
	dc.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC, rtBack.right, rtBack.bottom);
	poldbmp = dc.SelectObject(&bitmap);

	CBrush bhBack(RGB(255, 255, 255));
	dc.FillRect(&rtBack, &bhBack);

	UCHAR *pDrawData;
	if(m_bDrawBadptInfo)
		pDrawData = m_pBadInfoData;
	else
		pDrawData = m_pImageData;
	
	BITMAPINFO* pbmpInfo = (BITMAPINFO*)&theApp.m_stbmpInfo;
	pbmpInfo->bmiHeader.biWidth		= GFLS_SCANNER_IMAGEWIDTH;
	pbmpInfo->bmiHeader.biHeight	= -GFLS_SCANNER_IMAGEHEIGHT;
	pbmpInfo->bmiHeader.biSizeImage	= m_nDataSize;
	pbmpInfo->bmiHeader.biXPelsPerMeter = 20000;
	pbmpInfo->bmiHeader.biYPelsPerMeter = 20000;

	if(pDrawData)
		::SetDIBitsToDevice(dc.GetSafeHdc(), 0, 0, rtBack.right, rtBack.bottom, 0, 0, 0, rtBack.bottom, pDrawData, pbmpInfo, DIB_RGB_COLORS);
	DrawBadptPlace(&dc);
	pDC->BitBlt(0, 0, rtBack.right, rtBack.bottom, &dc, 0, 0, SRCCOPY);

	bhBack.DeleteObject();
	dc.SelectObject(poldbmp);
}

void CGfImageInfoStatic::DrawBadptPlace(CDC* pDC)
{
	if(!m_bDrawBadptInfo)
		return;
}


#define GFLS_ISBADPOINT(diff)			(((diff) > 25 || (diff < -25)) ? 1 : 0)
#define GFLS_BADPTINCENTERAREA(x, y)	(((x) >= 20 && (x) < 620 && (y) >= 20 && (y) < 620) ? 1 : 0)
#define GFLS_ABSOLUTIONVALUE(x)			(((x) >= 0) ? (x) : -(x))

void CGfImageInfoStatic::SetImageData2BadptStat(UCHAR* pImageData)
{
	if(NULL == pImageData || !m_pImageData || !m_pBadInfoData)
		return;
	GF_BADPOINTINFO	stBadInfo = {0};
	stBadInfo.paBadPoint = (POINT*)malloc(sizeof(POINT) * m_nDataSize);
	if(NULL == stBadInfo.paBadPoint)
		return;

	memcpy(m_pImageData, pImageData, m_nDataSize);
	memset(m_pBadInfoData, 255, m_nDataSize);
	
	InitBadptInfoData();
	GetImageDataAverage(pImageData);

	UCHAR *pSrcData, *pBadpt;
	pSrcData = pImageData;
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
					m_nCnrBad1pt;
			}
		}
	}

	free(stBadInfo.paBadPoint);

	Invalidate(FALSE);
}

void CGfImageInfoStatic::GetImageDataAverage(UCHAR* pImageData)
{
	UCHAR *pTemp;
	int i, nSum;
	double fAve;

	nSum = 0;
	pTemp = pImageData;
	m_nSmallGrayCnt = 0;
	for(i = 0; i < m_nDataSize; i++, pTemp++)
	{
		if(*pTemp < 225)
			m_nSmallGrayCnt++;
		nSum += (*pTemp);
	}

	fAve = (double)nSum / (double)m_nDataSize;
	m_nAverage = (int)(fAve + 0.5);
}

int CGfImageInfoStatic::GetBadPointRadia(GF_BADPOINTINFO &stBadInfo)
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

	return stBadInfo.nRadio;
}

int CGfImageInfoStatic::IsRightPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x + 1;	y = lpCur->y;
	if(x >= GFLS_SCANNER_IMAGEWIDTH)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CGfImageInfoStatic::IsLeftBottomPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x -1;	y = lpCur->y + 1;
	if(x < 0 || y >= GFLS_SCANNER_IMAGEHEIGHT)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CGfImageInfoStatic::IsBottomPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x;	y = lpCur->y + 1;
	if(y >= GFLS_SCANNER_IMAGEHEIGHT)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CGfImageInfoStatic::IsRightBottomPointBad(GF_BADPOINTINFO &stBadInfo)
{
	POINT* lpCur = stBadInfo.paBadPoint + stBadInfo.nIndex;
	int x, y;

	x = lpCur->x + 1;	y = lpCur->y + 1;
	if(x >= GFLS_SCANNER_IMAGEWIDTH || y >= GFLS_SCANNER_IMAGEHEIGHT)
		return 0;

	return AddBadPoint2Array(stBadInfo, x, y);
}

int CGfImageInfoStatic::AddBadPoint2Array(GF_BADPOINTINFO &stBadInfo, int x, int y)
{
	int nPos, ndiff;
	POINT *lpAdd;

	nPos  = y * GFLS_SCANNER_IMAGEWIDTH + x;
	if(0 == m_pBadInfoData[nPos])
		return 0;

	ndiff = m_pImageData[nPos] - m_nAverage;
	if(!GFLS_ISBADPOINT(ndiff))
		return 0;
	
	stBadInfo.nIndex++;
	stBadInfo.nBadptCount++;
	lpAdd = stBadInfo.paBadPoint + stBadInfo.nIndex;
	lpAdd->x = x;	lpAdd->y = y;
	m_pBadInfoData[nPos] = 0;

	return 1;
}

void CGfImageInfoStatic::GetBadPointStatInfo(TCHAR *pszInfo)
{
	TCHAR szReturn[] = _T("\r\n");
	int n;

	n  = 0;
	n += _stprintf(pszInfo + n, _T("图像疵点检测结果：%s"), szReturn);
	n += _stprintf(pszInfo + n, _T("1、一共有%d个直径为1的疵点，其中%d个在主视场内%s"), m_nAllBad1pt, m_nCnrBad1pt, szReturn);
	n += _stprintf(pszInfo + n, _T("2、一共有%d个直径大于等于2的疵点，其中%d个在主视场内%s"), m_nAllBad2pt, m_nCnrBad2pt, szReturn);
	n += _stprintf(pszInfo + n, _T("3、一共有%d个直径大于等于3的疵点，其中%d个在主视场内%s"), m_nAllBad3pt, m_nCnrBad3pt, szReturn);
	n += _stprintf(pszInfo + n, _T("4、背景图像的灰度均值：%d%s"), m_nAverage, szReturn);
	n += _stprintf(pszInfo + n, _T("5、背景图像灰度值小于225的像素个数：%d%s"), m_nSmallGrayCnt, szReturn);
	n += _stprintf(pszInfo + n, _T("其中，主视场是指采集仪中心%d像素 x %d像素"), GFLS_SCANNER_CENTERWIDTH, GFLS_SCANNER_CENTERHEIGHT);

//	CString strTemp;
//	strInfo = "图像疵点检测结果：" + pszReturn;
//	strTemp.Format("1、一共有%d个直径为1的疵点，其中%d个在主视场内%s", m_nAllBad1pt, m_nCnrBad1pt, szReturn);
//	strInfo += strTemp;
//	strTemp.Format("2、一共有%d个直径大于等于2的疵点，其中%d个在主视场内%s", m_nAllBad2pt, m_nCnrBad2pt, szReturn);
//	strInfo += strTemp;
//	strTemp.Format("3、一共有%d个直径大于等于3的疵点，其中%d个在主视场内%s", m_nAllBad3pt, m_nCnrBad3pt, szReturn);
//	strInfo += strTemp;
//	strTemp.Format("4、背景图像灰度值小于225的像素个数：%d%s", m_nSmallGrayCnt, szReturn);
//	strInfo += strTemp;
}
