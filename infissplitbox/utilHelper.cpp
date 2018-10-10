/**
 * @file	utilHelper.cpp
 * @brief	通用工具类：提供辅助功能
 * @author	北京东方金指科技有限公司
 * @date	2011/02/19 0:34:17
 * @note	
 * @copyright	版权所有(@) 1996-2011
 *				北京东方金指科技有限公司
 */

#include "stdafx.h"
#include "Resource.h"


//!< CBmpResource
void	CBmpResource::InitResourceInfo()
{
	m_hBitmap = NULL;
	m_pbmpInfo = NULL;
	m_pBitData = NULL;
}

void	CBmpResource::FreeResourceInfo()
{
	if ( m_hBitmap ) DeleteObject(m_hBitmap);
	if ( m_pbmpInfo ) free(m_pbmpInfo);
	if ( m_pBitData ) free(m_pBitData);

	InitResourceInfo();
}

BOOL	CBmpResource::LoadBitmapResource(HDC hdc, uint4 nID)
{
	if ( m_pBitData ) return TRUE;

	BITMAPINFOHEADER	*bh;
	BITMAP		bm;
	int			n, i;

	n = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	RGBQUAD			*rgb;

	// Back
	//m_hBitmap = (HBITMAP)GAFISRC_LoadImage(nID, IMAGE_BITMAP, LR_CREATEDIBSECTION);
	if(NULL == m_hBitmap ) return FALSE;

	::GetObject(m_hBitmap, sizeof(BITMAP), &bm);
	m_pbmpInfo = (BITMAPINFO *)malloc(n);
	if(!m_pbmpInfo) return FALSE;

	memset(m_pbmpInfo, 0, n);
	bh = &(m_pbmpInfo->bmiHeader);
	rgb = m_pbmpInfo->bmiColors;
	bh->biBitCount = bm.bmBitsPixel;
	bh->biCompression = BI_RGB;
	bh->biHeight = bm.bmHeight;
	bh->biPlanes = bm.bmPlanes;
	bh->biSize = sizeof(BITMAPINFOHEADER);
	bh->biSizeImage = bm.bmHeight * bm.bmWidth * bm.bmBitsPixel / 8;
	bh->biWidth = bm.bmWidth;
	bh->biXPelsPerMeter = 20000;
	bh->biYPelsPerMeter = 20000;
	m_pBitData = (UCHAR*)malloc(bh->biHeight * bh->biWidth * bh->biBitCount / 8);
	if(!m_pBitData) return FALSE;

	for(i = 0; i < 256; i++)
	{
		rgb[i].rgbBlue = rgb[i].rgbGreen = rgb[i].rgbRed = (BYTE)i;
	}

	::GetDIBits(hdc, m_hBitmap, 0, bh->biHeight, (LPVOID)m_pBitData, m_pbmpInfo, DIB_RGB_COLORS);
	DeleteObject(m_hBitmap);
	m_hBitmap = NULL;

	return TRUE;
}

bool	CBmpResource::Draw(CDC *pDC, LPRECT prtDes)
{
	if ( !pDC || !prtDes || !m_pBitData ) return false;

	::StretchDIBits(pDC->GetSafeHdc(), 
		prtDes->left, prtDes->top, prtDes->right - prtDes->left, prtDes->bottom - prtDes->top,
		0, 0, m_pbmpInfo->bmiHeader.biWidth, abs(m_pbmpInfo->bmiHeader.biHeight),
		m_pBitData, m_pbmpInfo,
		DIB_RGB_COLORS, SRCCOPY);

	return true;
}

CBmpResource	CUtilHelper::m_bmResBtnNormal;
CBmpResource	CUtilHelper::m_bmResBtnPress;

//!< 贵州新界面
CBmpResource	CUtilHelper::m_bmResBtnNormalNew;
CBmpResource	CUtilHelper::m_bmResBtnPressNew;
CBmpResource	CUtilHelper::m_bmResRadioNormal;
CBmpResource	CUtilHelper::m_bmResRadioPress;

COLORREF	CUtilHelper::m_crBackground = RGB(245, 249, 255);
COLORREF	CUtilHelper::m_crFrame = RGB(157, 181, 214);
COLORREF	CUtilHelper::m_crFill = RGB(202, 239, 255);
COLORREF	CUtilHelper::m_crHandDescText = RGB(51, 102, 153);
COLORREF	CUtilHelper::m_crTipInfo = RGB(144, 120, 225);

char	CUtilHelper::m_szNotExist[32] = {0};
char	CUtilHelper::m_szReGather[32] = {0};
char	CUtilHelper::m_szRollFinger[32] = {0};
char	CUtilHelper::m_szFlatFinger[32] = {0};
char	CUtilHelper::m_szFingerName[10][32] = {0};
char	CUtilHelper::m_szPalmName[4][32] = {0};
char	CUtilHelper::m_szSlap[3][32] = {0};

void CUtilHelper::GetStringResource()
{
//	GAFISRC_LoadString(IDS_GYTHCAPT_FINGER_NOEXIST, m_szNotExist, sizeof(m_szNotExist));
//	GAFISRC_LoadString(IDS_GYTHCAPT_FINGER_REGATHER, m_szReGather, sizeof(m_szReGather));
//	GAFISRC_LoadString(IDS_GYTHCAPT_FINGER_ROLL, m_szRollFinger, sizeof(m_szRollFinger));
//	GAFISRC_LoadString(IDS_GYTHCAPT_FINGER_FLAT, m_szFlatFinger, sizeof(m_szFlatFinger));

	for ( int i = 0; i < 10; ++i )
	{
	//	GAFISRC_LoadString(IDS_GYTHCAPT_FINGER_NAME_R1 + i, m_szFingerName[i], sizeof(m_szFingerName[i]));
		char	szBuf[32] = { 0 };
		sprintf(szBuf, "%d", i + 1);
		strncpy(m_szFingerName[i], szBuf, sizeof(m_szFingerName[i]));
	}

	for ( int i = 0; i < PALM_CNT; ++i )
	{
//		GAFISRC_LoadString(IDS_GYTHCAPT_PALM_NAME_RSUB +i, m_szPalmName[i], sizeof(m_szPalmName[i]));
	}
	for ( int i = 0; i < SLAP_CNT; ++i )
	{
//		GAFISRC_LoadString(IDS_GYTHCAPT_FG4THUMB_NAME_RFOUR +i, m_szSlap[i], sizeof(m_szSlap[i]));
	}
}

CUtilHelper::CUtilHelper()
{
	GYTHHELPER_Init256BitmapInfo(&m_stbi, 0, 0, BPP_8);
}

UCHAR	CUtilHelper::CprMethod_UserDefindToG6(int nVendorId, int nProductId)
{
	switch(nVendorId)
	{
	case	VENDORID_BY:
		return GAIMG_CPRMETHOD_BUPT;
	case	VENDORID_HX_COGENT:
		return GAIMG_CPRMETHOD_COGENT;
	case	VENDORID_NEC:
		return GAIMG_CPRMETHOD_NEC;
	case	VENDORID_PKUHT:
		return GAIMG_CPRMETHOD_PKU;
	case	VENDORID_EGF:
		return GAIMG_CPRMETHOD_GFS;
	case	VENDORID_QH:
		return GAIMG_CPRMETHOD_TSINGHUA;
	default:
		return	0;
	}
}

UCHAR	CUtilHelper::CprMethod_GAFIS7To6(int nG7Method, int nVendorId /* = VENDORID_UNKNOWN */, int nProductId /* = 0 */)
{
	switch( nG7Method )
	{
	case CPRCODE_NOCOMPRESS:
	default:
		return GAIMG_CPRMETHOD_DEFAULT;
	case CPRCODE_XGW:
		return GAIMG_CPRMETHOD_XGW;
	case CPRCODE_XGW_EZW:
		return GAIMG_CPRMETHOD_XGW_EZW;
	case CPRCODE_USERDEFINED:
		return CprMethod_UserDefindToG6(nVendorId, nProductId);
	case CPRCODE_WSQ20:
		return GAIMG_CPRMETHOD_WSQ;
	case CPRCODE_JPEGB:
	case CPRCODE_JPEGL:
	case CPRCODE_JP2000:
	case CPRCODE_JP2000LS:
		return GAIMG_CPRMETHOD_JPG;
	}
}

UCHAR	CUtilHelper::CprMethod_GAFIS6To7(int nG6Method, int &nVendorId, int &nProductId)
{
	nVendorId = VENDORID_UNKNOWN;
	nProductId = 0;

	switch ( nG6Method )
	{
	default:
	case GAIMG_CPRMETHOD_DEFAULT:
	case GAIMG_CPRMETHOD_XGW:
		return CPRCODE_XGW;
	case GAIMG_CPRMETHOD_XGW_EZW:
		return CPRCODE_XGW_EZW;
	case GAIMG_CPRMETHOD_WSQ:
		return CPRCODE_WSQ20;
	case GAIMG_CPRMETHOD_JPG:
		return CPRCODE_JPEGB;
	case GAIMG_CPRMETHOD_BUPT:
	case GAIMG_CPRMETHOD_COGENT:
	case GAIMG_CPRMETHOD_NEC:
	case GAIMG_CPRMETHOD_TSINGHUA:
	case GAIMG_CPRMETHOD_GFS:
	case GAIMG_CPRMETHOD_PKU:
		CprMethod_G6ToVendorProductID(nG6Method, nVendorId, nProductId);
		return CPRCODE_USERDEFINED;
	}
}

void	CUtilHelper::CprMethod_G6ToVendorProductID(int nG6Method, int &nVendorId, int &nProductId)
{
	nProductId = 0;

	switch(nG6Method)
	{
	case GAIMG_CPRMETHOD_BUPT:
		nVendorId = VENDORID_BY;
		break;
	case GAIMG_CPRMETHOD_COGENT:
		nVendorId = VENDORID_HX_COGENT;
		break;
	case GAIMG_CPRMETHOD_NEC:
		nVendorId = VENDORID_NEC;
		break;
	case GAIMG_CPRMETHOD_TSINGHUA:
		nVendorId = VENDORID_QH;
		break;
	case GAIMG_CPRMETHOD_GFS:
		nVendorId = VENDORID_EGF;
		break;
	case GAIMG_CPRMETHOD_PKU:
		nVendorId = VENDORID_PKUHT;
		break;
	default:
		nVendorId = VENDORID_UNKNOWN;
	}
}


int CUtilHelper::PalmFGPToIndex(int nFGP)
{
	switch (nFGP )
	{
	case GAFIS7::CodeData::R_PALM_Q:
	case GAFIS7::CodeData::L_PALM_Q:
		return nFGP - GAFIS7::CodeData::R_PALM_Q;
	case GAFIS7::CodeData::R_PALM:
	case GAFIS7::CodeData::L_PALM:
		return nFGP - GAFIS7::CodeData::R_PALM;
	case GAFIS7::CodeData::R_WRITER:
	case GAFIS7::CodeData::L_WRITER:
		return nFGP - GAFIS7::CodeData::R_WRITER + 2;
	case GAFIS7::CodeData::R_FOUR:
	case GAFIS7::CodeData::L_FOUR:
		return nFGP - GAFIS7::CodeData::R_FOUR;
	case GAFIS7::CodeData::RL_THUMB:
		return SLAP_THUMB;
	default:
		return -1;
	}
}

int	CUtilHelper::PalmIndexToFGP(int nIndex, bool bSlap)
{
	if ( nIndex < 0 ) return GAFIS7::CodeData::UNKNOWN;
	else if ( nIndex < 2 ) 
	{
		if ( bSlap ) 
			return GAFIS7::CodeData::R_FOUR + nIndex;
		else 
		{
#ifdef GA7_USE6HEAD	//贵州 ZONE_GUIZHOU
			return GAFIS7::CodeData::R_PALM + nIndex;
#endif
			return GAFIS7::CodeData::R_PALM_Q + nIndex;
		}
	}
	else 
	{
		if ( bSlap )
		{
			if ( nIndex == 2 ) return GAFIS7::CodeData::RL_THUMB;
			else return GAFIS7::CodeData::UNKNOWN;
		}
		else if ( nIndex < 4 ) return GAFIS7::CodeData::R_WRITER + nIndex - 2;
		else return GAFIS7::CodeData::UNKNOWN;
	}
}

int CUtilHelper::UTIL_BuildCaptImageData(GYTH_CAPTIMAGEDATA &stBTYCaptData, int nDesWidth, int nDesHeight, int nDesBPP,
										 UCHAR *pbnSrcData, int nSrcWidth, int nSrcHeight, int ncbSrcLine, int nSrcBPP)
{
	UCHAR	*pBuffer, *pBitData;
	int		nImgBuffer, nCurBuffer, ncbDesLine;

	ncbDesLine = GetBmpImgLine(nDesWidth, nDesBPP);
	nImgBuffer = ncbDesLine * nDesHeight + sizeof(GYTH_256BITMAPINFO);
	CUtilHelper::GetBmpImgBuffer(stBTYCaptData.pbmInfo, pBuffer, nCurBuffer, sizeof(GYTH_256BITMAPINFO));
	GYTHHELPER_ReMallocBufferEx(&pBuffer, &nCurBuffer, nImgBuffer, 255);
	if ( !pBuffer ) return 0;

	GYTH_256BITMAPINFO	*p256bmInfo = (GYTH_256BITMAPINFO*)pBuffer;
	pBitData = pBuffer + sizeof(GYTH_256BITMAPINFO);

	if ( (ncbDesLine==ncbSrcLine) && (nDesHeight==nSrcHeight) )
	{
		memcpy(pBitData, pbnSrcData, ncbDesLine * nDesHeight);
	}
	else
	{
		UCHAR	*pbnTempDes, *pbnTempSrc;
		int		nOffset, ncbCopyLine, nCopyHeight;

		pbnTempDes = pBitData;
		pbnTempSrc = pbnSrcData;
		if ( nDesHeight <= nSrcHeight )
		{
			nOffset = (nSrcHeight - nDesHeight) / 2;
			pbnTempSrc += nOffset * ncbSrcLine;
			nCopyHeight = nDesHeight;
		}
		else
		{
			nOffset = (nDesHeight - nSrcHeight) / 2;
			pbnTempDes += nOffset * ncbDesLine;
			nCopyHeight = nSrcHeight;
		}
		if ( nDesWidth <= nSrcWidth )
		{
			nOffset = (nSrcWidth - nDesWidth) / 2;
			pbnTempSrc += GetImageLine(nOffset, nSrcBPP);
			ncbCopyLine = ncbDesLine;
		}
		else if ( nDesWidth > nSrcWidth )
		{
			nOffset = (nDesWidth - nSrcWidth) / 2;
			pbnTempDes += GetImageLine(nOffset, nDesBPP);
			ncbCopyLine = ncbSrcLine;
		}
		for ( int i = 0; i < nCopyHeight; ++i, pbnTempDes += ncbDesLine, pbnTempSrc += ncbSrcLine )
		{
			memcpy(pbnTempDes, pbnTempSrc, ncbCopyLine);
		}	
	}

	stBTYCaptData.pbmInfo  = (BITMAPINFO*)pBuffer;
	stBTYCaptData.pBitData = pBitData;
	stBTYCaptData.bDataIsValid = 1;
	stBTYCaptData.bDrawInFrame = 1;
	memcpy(p256bmInfo, &m_stbi, sizeof(GYTH_256BITMAPINFO));
	p256bmInfo->bmiHeader.biBitCount  = nDesBPP;
	p256bmInfo->bmiHeader.biWidth	  = nDesWidth;
	p256bmInfo->bmiHeader.biHeight	  = -nDesHeight;
	p256bmInfo->bmiHeader.biSizeImage = GetImageSize(nDesWidth, nDesHeight, nDesBPP);

	return 1;
}

void	UTIL_DrawNoScaleData(CDC *pDC, CRect rtFrame, BITMAPINFO *pbmInfo, UCHAR *pData, CPoint *pptImgOff)
{
	if ( !pbmInfo || !pData ) return;

	//CBrush	brush(RGB(224, 224, 224));
	//pDC->FillRect(&rtFrame, &brush);

	int		nwImg, nhImg, nwFrm, nhFrm, nw, nh;
	CPoint	ptImgOff(0, 0), ptDispOff;

	nwImg = pbmInfo->bmiHeader.biWidth;
	nhImg = abs(pbmInfo->bmiHeader.biHeight);
	nwFrm = rtFrame.Width();
	nhFrm = rtFrame.Height();

	if ( ( (nwImg > nwFrm) || (nhImg > nhFrm) ) && pptImgOff )
	{
		ptImgOff = *pptImgOff;
	}

	if ( nwImg > nwFrm ) nw = nwFrm;
	else nw = nwImg;
	if ( nhImg > nhFrm ) nh = nhFrm;
	else nh = nhImg;

	ptDispOff.x = (nwFrm - nwImg) / 2;
	ptDispOff.y = (nhFrm - nhImg) / 2;
	if ( ptDispOff.x < 0 ) 
	{
		ptImgOff.x = -ptDispOff.x;
		ptDispOff.x = 0;
	}
	if ( ptDispOff.y < 0 ) 
	{
		ptImgOff.y = -ptDispOff.y;
		ptDispOff.y = 0;
	}

	if(ptDispOff.y > CUtilHelper::VITICAL_DISP ) ptDispOff.y = CUtilHelper::VITICAL_DISP;

	if ( pbmInfo->bmiHeader.biHeight > 0 ) ptImgOff.y = ( nhImg - ptImgOff.y);

	::SetDIBitsToDevice(pDC->GetSafeHdc(), rtFrame.left + ptDispOff.x, rtFrame.top + ptDispOff.y, 
		nw, nh, ptImgOff.x, ptImgOff.y, 0, nhImg, pData, pbmInfo, DIB_RGB_COLORS);

	//brush.DeleteObject();
}

void	UTIL_GetSplitedMatchPlainData(UCHAR *pbnSrcData, UCHAR *pbnSplitData, REGIONRECT &stRegion, 
									  int nwSrc, int nhSrc, int &nwDes, int &nhDes)
{
	nwDes = stRegion.nWidth / 4 * 4;
	nhDes = stRegion.nHeight / 4 * 4;

	RECT rtCut= {stRegion.y, stRegion.x, stRegion.y + nwDes, stRegion.x + nhDes};
	SIZE szSrc = {nwSrc, nhSrc};

	Gfp_ImgCut(pbnSplitData, pbnSrcData, rtCut, szSrc, 255);
}

int		UTIL_MntStd2MntDispInfo(void* pMntData, FINGERFEATQLEV* pstFeat, int bPlain)
{
#if 0
	GFALGORITHMHEAD			stAlgHead = {0};
	GFALG_MNTDISPINITINFO	stDispIni = {0};
	GFALG_MNTDISPANDMNTSTD  stAlgStdDisp = {0};
	MNTDISPSTRUCT			stDisp = {0};
	int nret;

	stAlgHead.nCurAlgorithm = GAFIS_MATCHMETHOD_XGWNEW;
	stDispIni.bLatent		= 0;
	stDispIni.bPalm			= 0;
	stDispIni.nResolution	= CUtilHelper::PPI_DEFAULT;
	stDispIni.pMntDisp		= &stDisp;
	GfAlg_MntDispMntInitial(&stAlgHead, &stDispIni);

	stAlgStdDisp.bFinger	= 1;
	stAlgStdDisp.bLatent	= 0;
	stAlgStdDisp.pstMntDisp = &stDisp;
	stAlgStdDisp.pstMntStd	= pMntData;

	stDisp.FeatQlev.bIsPlain = bPlain ? 1 : 0;

	nret = GfAlg_MntStd2MntDisp(&stAlgHead, &stAlgStdDisp);
	GfAlg_FreeMntDispInnerData(stAlgHead.nCurAlgorithm, &stDisp);

	memcpy(pstFeat, &stDisp.FeatQlev, sizeof(*pstFeat));

	return nret;	
#else
	return -1;
#endif
}

//!< 图像截取
void	UTIL_ImageIntercept(UCHAR *pSrc, UCHAR *pDes, int nwSrc, int nhSrc, int nwDes, int nhDes)
{
	SIZE sizeSrc = {nwSrc, nhSrc};
	RECT rtCut;

	rtCut.left	 = (nwSrc - nwDes) / 2;
	rtCut.top	 = (nhSrc - nhDes) / 2;
	rtCut.right  = rtCut.left + nwDes;
	rtCut.bottom = rtCut.top + nhDes;

	Gfp_ImgCut(pDes, pSrc, rtCut, sizeSrc, 255);
}

void UTIL_DrawImageDataInArea(CDC* pDC, CRect rt, BITMAPINFO *pbmInfo, UCHAR *pData)
{
	if ( !pbmInfo || !pData ) return;

	CRect	rtArea;
	int		nwArea, nhArea, nwImg, nhImg, xOff, yOff;
	double	fScale, ftmp;

	rt.DeflateRect(1, 1);
	nwArea = rt.Width();	nhArea = rt.Height();
	nwImg  = pbmInfo->bmiHeader.biWidth;	nhImg = pbmInfo->bmiHeader.biHeight; 
	if(nhImg < 0) nhImg = -nhImg;

	ftmp   = 1.0 * nwArea / (1.0 * nwImg);
	fScale = 1.0 * nhArea / (1.0 * nhImg);
	if(ftmp < fScale)
	{
		fScale = ftmp;
		nhArea = (int)(fScale * nhImg);
	}
	else nwArea = (int)(fScale * nwImg);
	nwArea = nwArea / 4 * 4;

	xOff = rt.left + (rt.Width()  - nwArea) / 2;
	yOff = rt.top  + (rt.Height() - nhArea) / 2;

	::SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);

	::StretchDIBits(pDC->GetSafeHdc(), xOff, yOff, nwArea, nhArea, 0, 0, 
		nwImg, nhImg /*pbmInfo->bmiHeader.biHeight*/, pData, pbmInfo, DIB_RGB_COLORS, SRCCOPY);
}