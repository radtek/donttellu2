// infissplitbox.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "infissplitbox.h"
#include "dlgsplitflat.h"
#include "gbaselib/gaimgop.h"
#include "infissplitboxdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CinfissplitboxApp

BEGIN_MESSAGE_MAP(CinfissplitboxApp, CWinApp)
END_MESSAGE_MAP()


// CinfissplitboxApp 构造

CinfissplitboxApp::CinfissplitboxApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CinfissplitboxApp 对象

CinfissplitboxApp theApp;


// CinfissplitboxApp 初始化

BOOL CinfissplitboxApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

enum	CAPTWNDINFO_ENUM
{
	CAPTWND_FINGER = 1, CAPTWND_SLAP = 2,
	CAPTWND_PALM = 3, CAPTWND_WRITER = 4, CAPTWND_HAND = 5, CAPTWND_COUNT = 5,
	FINGER_WIDTH = 640, FINGER_HEIGHT = 640,
	SLAP_WIDTH = 1600, SLAP_HEIGHT = 1500,
	PALM_WIDHT = 2304, PALM_HEIGHT = 2304,
	WRITER_WIDTH = 1024, WRITER_HEIGHT = 1968,
	HAND_WIDTH = 2304, HAND_HEIGHT = 3584,
	CAPTWND_INVALIDE = 0
};


CUtilHelper		m_utilHelper;

//int PASCAL	GfImage_FreeImageInfo(GF_IMAGEINFOSTRUCT *pstImageInfo)
//{
//	if (pstImageInfo)
//	{
//		if (pstImageInfo->pImgData) free(pstImageInfo->pImgData);
//		if (pstImageInfo->prgbQuad) free(pstImageInfo->prgbQuad);
//
//		pstImageInfo->prgbQuad = NULL;
//		pstImageInfo->pImgData = NULL;
//	}
//
//	return GFIMAGEFORMAT_ERROR_SUCCESS;
//}

void GetImage(CBTYStatus &btyStatus, GF_IMAGEINFOSTRUCT &stImageInfo)
{
	int		nWidth, nHeight, nBPP, nSrcLine;

	if (!btyStatus.pstCaptImage->pbmInfo)
	{
		/*nWidth = m_pCaptView->m_pCtrl->m_CaptCard.GetBTYDefWidth(m_BtyStatus.nBty, m_BtyStatus.Fgp);
		nHeight = m_pCaptView->m_pCtrl->m_CaptCard.GetBTYDefHeight(m_BtyStatus.nBty, m_BtyStatus.Fgp);
		nBPP = m_pCaptView->m_utilHelper.m_stbi.bmiHeader.biBitCount;*/
		nWidth = SLAP_WIDTH;
		nHeight = SLAP_HEIGHT;
		nBPP = 8;
	}
	else
	{
		nWidth = btyStatus.pstCaptImage->pbmInfo->bmiHeader.biWidth;
		nHeight = abs(btyStatus.pstCaptImage->pbmInfo->bmiHeader.biHeight);
		nBPP = btyStatus.pstCaptImage->pbmInfo->bmiHeader.biBitCount;
	}
	if (stImageInfo.bIsPacketDIB)
	{
		nSrcLine = CUtilHelper::GetBmpImgLine(stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biBitCount);
	}
	else nSrcLine = CUtilHelper::GetImageLine(stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biBitCount);

	int retval = m_utilHelper.UTIL_BuildCaptImageData(*btyStatus.pstCaptImage, nWidth, nHeight, nBPP,
		stImageInfo.pImgData, stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biHeight, nSrcLine, stImageInfo.bmpHead.biBitCount);

	GfImage_FreeImageInfo(&stImageInfo);
	if (retval < 1) return;
}

void GetImageFromFile(const CStringA &filename, GF_IMAGEINFOSTRUCT &st)
{
	GF_IMAGEINFOSTRUCT stImageInfo = { 0 };

	stImageInfo.bmpHead.biBitCount = CUtilHelper::BPP_8;
	int retval = GfImage_LoadImageFile(filename.GetString(), &stImageInfo, 1);
	if (retval != GFIMAGEFORMAT_ERROR_SUCCESS)
	{
		GfImage_FreeImageInfo(&stImageInfo);
		return;
	}

	if (stImageInfo.bmpHead.biHeight < 0)
	{
		stImageInfo.bmpHead.biHeight = -stImageInfo.bmpHead.biHeight;
		Gfp_OperatorImageDataEx(stImageInfo.pImgData, stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biHeight,
			stImageInfo.bmpHead.biBitCount, GFP_OPERATORIMAGEDATA_TYPE_VMIRROR);
	}

	memcpy(&st, &stImageInfo, sizeof(st));
}

void GetImageFromMem(unsigned char *imagebuff, int nw, int nh, GF_IMAGEINFOSTRUCT &st)
{
	if (imagebuff == NULL)
		return;

	GF_IMAGEINFOSTRUCT stImageInfo = { 0 };

	stImageInfo.bmpHead.biBitCount = CUtilHelper::BPP_8;
	stImageInfo.bmpHead.biWidth = nw;
	stImageInfo.bmpHead.biHeight = nh;
	stImageInfo.bmpHead.biXPelsPerMeter = 19700;
	stImageInfo.bmpHead.biYPelsPerMeter = 19700;
	stImageInfo.bmpHead.biSize = sizeof(BITMAPINFOHEADER);
	stImageInfo.bmpHead.biSizeImage = nw * nh;

	stImageInfo.pImgData = (UCHAR *)GAFIS_MALLOC(nw * nh);
	if (stImageInfo.pImgData == NULL)
		return;

	memcpy(stImageInfo.pImgData, imagebuff, nw * nh);

	if (stImageInfo.bmpHead.biHeight < 0)
	{
		stImageInfo.bmpHead.biHeight = -stImageInfo.bmpHead.biHeight;
		Gfp_OperatorImageDataEx(stImageInfo.pImgData, stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biHeight,
			stImageInfo.bmpHead.biBitCount, GFP_OPERATORIMAGEDATA_TYPE_VMIRROR);
	}

	memcpy(&st, &stImageInfo, sizeof(st));
}

#ifdef NIRVANA_TIMESTAMP
#define	DEADLINE_YEAR	2017	//2015	//2015
#define	DEADLINE_MON	6	//12	//11
#define	DEADLINE_DAY	1	//1	//10
#endif

INFIS_SPLITBOX_DECL int INFIS_CutImage(int nCaptureType, UCHAR * imagebuff, int nImgWidth, int nImgHeight, void *pWnd, int nImageType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef NIRVANA_TIMESTAMP
	//struct tm	*t = NULL;
	time_t		ttCur, ttEnd;
	struct tm	tmStamp;

	time(&ttCur);
	//t = localtime(&tt);

	memset(&tmStamp, 0, sizeof(tmStamp));
	//2016.11.16
	tmStamp.tm_year = DEADLINE_YEAR - 1900;
	tmStamp.tm_mon = DEADLINE_MON - 1;
	tmStamp.tm_mday = DEADLINE_DAY;

	ttEnd = mktime(&tmStamp);

	if (ttCur >= ttEnd)
	{
		return -5;
	}

#endif
	if (imagebuff == NULL || pWnd == NULL || nImgWidth <= 0 || nImgHeight <= 0)
		return -1;

	CUtilHelper::GetStringResource();

	// TODO: 在此添加控件通知处理程序代码
	REGIONRECT	stSrcObj = { 0 };
	REGIONRECT	stDesObj[4] = { 0 };
	int		nExistObj = 0, nDesObj = 0;
	int		retval = 0;

	CBTYStatus		btyStatus;
	switch (nCaptureType)
	{
	/*case CAPTURE_TYPE_FLATFINGER:
		btyStatus.nBty = GAFIS7::CodeData::BTY_FINGER;
		btyStatus.Fgp = GAFIS7::CodeData::FINGER_R_THUMB;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;*/
	case CAPTURE_TYPE_RFOUR:
		btyStatus.nBty = GAFIS7::CodeData::BTY_PALM;
		btyStatus.Fgp = GAFIS7::CodeData::R_FOUR;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;
	case CAPTURE_TYPE_LFOUR:
		btyStatus.nBty = GAFIS7::CodeData::BTY_PALM;
		btyStatus.Fgp = GAFIS7::CodeData::L_FOUR;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;
	case CAPTURE_TYPE_RLTHUMB:
	default:
		btyStatus.nBty = GAFIS7::CodeData::BTY_PALM;
		btyStatus.Fgp = GAFIS7::CodeData::RL_THUMB;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;
	}

	GYTH_CAPTIMAGEDATA	stCaptImage = { 0 };
	btyStatus.pstCaptImage = &stCaptImage;

	GF_IMAGEINFOSTRUCT stImageInfo = { 0 };

	//GetImageFromFile("d:\\rfour.bmp", stImageInfo);
	GetImageFromMem(imagebuff, nImgWidth, nImgHeight, stImageInfo);

	GetImage(btyStatus, stImageInfo);

	GYTH_CAPTIMAGEDATA	*pstPalmData = btyStatus.pstCaptImage;

	stSrcObj.nWidth = CUtilHelper::GetBmpImgLine(pstPalmData->pbmInfo->bmiHeader.biWidth, pstPalmData->pbmInfo->bmiHeader.biBitCount);
	stSrcObj.nHeight = abs(pstPalmData->pbmInfo->bmiHeader.biHeight);

	if (CUtilHelper::SlapIsRLThumb(btyStatus.Fgp))
	{
		stSrcObj.nObjectType = HANDSPLITE_OBJTYPE_RLTHUMB;
		//if (!m_pCtrl->m_ConfigInfo.m_bFingerNotExist[0])
		{
			stDesObj[nDesObj].nObjectType = HANDSPLITE_OBJTYPE_FLAT_RTHUMB;
			++nDesObj;
			//			if (m_pCtrl->m_ConfigInfo.IsCapturedDataValid(&m_pCtrl->m_stPlainData[0])) ++nExistObj;
		}
		//if (!m_pCtrl->m_ConfigInfo.m_bFingerNotExist[5])
		{
			stDesObj[nDesObj].nObjectType = HANDSPLITE_OBJTYPE_FLAT_LTHUMB;
			++nDesObj;
			//			if (m_pCtrl->m_ConfigInfo.IsCapturedDataValid(&m_pCtrl->m_stPlainData[5])) ++nExistObj;
		}
	}
	else
	{
		int		nObjType, nStartIdx;
		//		bool	*pbFingerNotExist;

		if (CUtilHelper::SlapIsRFour(btyStatus.Fgp))
		{
			stSrcObj.nObjectType = HANDSPLITE_OBJTYPE_RPLAINFOUR;
			nObjType = HANDSPLITE_OBJTYPE_FLAT_RINDEX;
			nStartIdx = 1;
			//			pbFingerNotExist = m_pCtrl->m_ConfigInfo.m_bFingerNotExist + 1;
		}
		else
		{
			stSrcObj.nObjectType = HANDSPLITE_OBJTYPE_LPLAINFOUR;
			nObjType = HANDSPLITE_OBJTYPE_FLAT_LINDEX;
			nStartIdx = 6;
			//			pbFingerNotExist = m_pCtrl->m_ConfigInfo.m_bFingerNotExist + 6;
		}

		for (int i = 0; i < 4; ++i, ++nObjType, ++nStartIdx)
		{
			//if (m_pCtrl->m_ConfigInfo.m_bFingerNotExist[nStartIdx]) continue;
			//			if ( pbFingerNotExist[i] ) continue;
			stDesObj[nDesObj].nObjectType = nObjType;
			++nDesObj;
			//if (m_pCtrl->m_ConfigInfo.IsCapturedDataValid(&m_pCtrl->m_stPlainData[nStartIdx])) ++nExistObj;
		}
	}

	if (nDesObj < 1) return -1;

	retval = GAFIS_SpliteImageNew(pstPalmData->pBitData, stSrcObj.nWidth, stSrcObj.nHeight, &stSrcObj, stDesObj, nDesObj);

	if (retval < 0) 
		return retval;

	CDlgSplitFlat	dlg((CWnd *)pWnd);

	dlg.m_staticSplit.SetSplitFlatInfo(&btyStatus, &stSrcObj, stDesObj, nDesObj);
	if (dlg.DoModal() != IDOK)
		return -100;

	dlg.m_staticSplit.GetSplittedFlatData(true, nImageType);

	return 0;
}

INFIS_SPLITBOX_DECL int INFIS_CutImage2Mem(int nCaptureType, unsigned char* imagebuff, int nImgWidth, int nImgHeight, void *pWnd, Infis_Image **ppstImg, int *pnCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef NIRVANA_TIMESTAMP
	//struct tm	*t = NULL;
	time_t		ttCur, ttEnd;
	struct tm	tmStamp;

	time(&ttCur);
	//t = localtime(&tt);

	memset(&tmStamp, 0, sizeof(tmStamp));
	//2016.11.16
	tmStamp.tm_year = DEADLINE_YEAR - 1900;
	tmStamp.tm_mon = DEADLINE_MON - 1;
	tmStamp.tm_mday = DEADLINE_DAY;

	ttEnd = mktime(&tmStamp);

	if (ttCur >= ttEnd)
	{
		return -5;
	}

#endif
	if (imagebuff == NULL || pWnd == NULL || nImgWidth <= 0 || nImgHeight <= 0)
		return -1;

	CUtilHelper::GetStringResource();

	// TODO: 在此添加控件通知处理程序代码
	REGIONRECT	stSrcObj = { 0 };
	REGIONRECT	stDesObj[4] = { 0 };
	int		nExistObj = 0, nDesObj = 0;
	int		retval = 0;

	CBTYStatus		btyStatus;
	switch (nCaptureType)
	{
	/*case CAPTURE_TYPE_FLATFINGER:
		btyStatus.nBty = GAFIS7::CodeData::BTY_FINGER;
		btyStatus.Fgp = GAFIS7::CodeData::FINGER_R_THUMB;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;*/
	case CAPTURE_TYPE_RFOUR:
		btyStatus.nBty = GAFIS7::CodeData::BTY_PALM;
		btyStatus.Fgp = GAFIS7::CodeData::R_FOUR;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;
	case CAPTURE_TYPE_LFOUR:
		btyStatus.nBty = GAFIS7::CodeData::BTY_PALM;
		btyStatus.Fgp = GAFIS7::CodeData::L_FOUR;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;
	case CAPTURE_TYPE_RLTHUMB:
	default:
		btyStatus.nBty = GAFIS7::CodeData::BTY_PALM;
		btyStatus.Fgp = GAFIS7::CodeData::RL_THUMB;
		btyStatus.ViewID = GAFIS7::CodeData::PALM_VIEWID_FLAT;
		btyStatus.bSlap = true;

		break;
	}

	GYTH_CAPTIMAGEDATA	stCaptImage = { 0 };
	btyStatus.pstCaptImage = &stCaptImage;

	GF_IMAGEINFOSTRUCT stImageInfo = { 0 };

	//GetImageFromFile("d:\\rfour.bmp", stImageInfo);
	GetImageFromMem(imagebuff, nImgWidth, nImgHeight, stImageInfo);

	GetImage(btyStatus, stImageInfo);

	GYTH_CAPTIMAGEDATA	*pstPalmData = btyStatus.pstCaptImage;

	stSrcObj.nWidth = CUtilHelper::GetBmpImgLine(pstPalmData->pbmInfo->bmiHeader.biWidth, pstPalmData->pbmInfo->bmiHeader.biBitCount);
	stSrcObj.nHeight = abs(pstPalmData->pbmInfo->bmiHeader.biHeight);

	if (CUtilHelper::SlapIsRLThumb(btyStatus.Fgp))
	{
		stSrcObj.nObjectType = HANDSPLITE_OBJTYPE_RLTHUMB;
		//if (!m_pCtrl->m_ConfigInfo.m_bFingerNotExist[0])
		{
			stDesObj[nDesObj].nObjectType = HANDSPLITE_OBJTYPE_FLAT_RTHUMB;
			++nDesObj;
			//			if (m_pCtrl->m_ConfigInfo.IsCapturedDataValid(&m_pCtrl->m_stPlainData[0])) ++nExistObj;
		}
		//if (!m_pCtrl->m_ConfigInfo.m_bFingerNotExist[5])
		{
			stDesObj[nDesObj].nObjectType = HANDSPLITE_OBJTYPE_FLAT_LTHUMB;
			++nDesObj;
			//			if (m_pCtrl->m_ConfigInfo.IsCapturedDataValid(&m_pCtrl->m_stPlainData[5])) ++nExistObj;
		}
	}
	else
	{
		int		nObjType, nStartIdx;
		//		bool	*pbFingerNotExist;

		if (CUtilHelper::SlapIsRFour(btyStatus.Fgp))
		{
			stSrcObj.nObjectType = HANDSPLITE_OBJTYPE_RPLAINFOUR;
			nObjType = HANDSPLITE_OBJTYPE_FLAT_RINDEX;
			nStartIdx = 1;
			//			pbFingerNotExist = m_pCtrl->m_ConfigInfo.m_bFingerNotExist + 1;
		}
		else
		{
			stSrcObj.nObjectType = HANDSPLITE_OBJTYPE_LPLAINFOUR;
			nObjType = HANDSPLITE_OBJTYPE_FLAT_LINDEX;
			nStartIdx = 6;
			//			pbFingerNotExist = m_pCtrl->m_ConfigInfo.m_bFingerNotExist + 6;
		}

		for (int i = 0; i < 4; ++i, ++nObjType, ++nStartIdx)
		{
			//if (m_pCtrl->m_ConfigInfo.m_bFingerNotExist[nStartIdx]) continue;
			//			if ( pbFingerNotExist[i] ) continue;
			stDesObj[nDesObj].nObjectType = nObjType;
			++nDesObj;
			//if (m_pCtrl->m_ConfigInfo.IsCapturedDataValid(&m_pCtrl->m_stPlainData[nStartIdx])) ++nExistObj;
		}
	}

	if (nDesObj < 1) return -1;

	retval = GAFIS_SpliteImageNew(pstPalmData->pBitData, stSrcObj.nWidth, stSrcObj.nHeight, &stSrcObj, stDesObj, nDesObj);

	if (retval < 0)
		return retval;

	CDlgSplitFlat	dlg((CWnd *)pWnd);

	dlg.m_staticSplit.SetSplitFlatInfo(&btyStatus, &stSrcObj, stDesObj, nDesObj);
	if (dlg.DoModal() != IDOK)
		return -100;

	dlg.m_staticSplit.GetSplittedFlatData(false, 0);

	Infis_Image	*pstImage = NULL;

	int	nCount = dlg.m_staticSplit._mapSplitedImg.size();
	if (nCount > 0)
	{
		pstImage = (Infis_Image *)GAFIS_MALLOC(sizeof(Infis_Image)* nCount);

		Infis_Image	*pstTemp = pstImage;

		std::map<int, std::string>::iterator	itr;
		for (itr = dlg.m_staticSplit._mapSplitedImg.begin();
			itr != dlg.m_staticSplit._mapSplitedImg.end();
			itr++)
		{
			const std::string	&img = itr->second;
			pstTemp->nFgp = itr->first;
			pstTemp->nImageLen = img.size();

			pstTemp->pImage = (char *)GAFIS_MALLOC(img.size());
			if (pstTemp->pImage == NULL)
				continue;

			memcpy(pstTemp->pImage, img.c_str(), img.size());
			pstTemp++;
		}
	}

	*ppstImg = pstImage;
	*pnCount = nCount;

	return 0;
}

INFIS_SPLITBOX_DECL void INFIS_FreeCutImage(Infis_Image *pstImg, int nCount)
{
	if (pstImg == NULL || nCount <= 0)
		return;

	Infis_Image *pTemp = pstImg;
	for (int i = 0; i < nCount; ++i)
	{
		AFIS_FREE(pTemp->pImage);
		pTemp++;
	}

	AFIS_FREE(pstImg);
}

INFIS_SPLITBOX_DECL int INFIS_SaveDataAsBMP(char *filename, unsigned char *imagebuff, int nImgWidth, int nImgHeight, int nBit)
{
	return GAFIS_SaveDataAsBMP(filename, imagebuff, nImgWidth, nImgHeight, nBit);
}

INFIS_SPLITBOX_DECL int INFIS_CompressByWSQ(unsigned char *pSrc, int nw, int nh, int nCprRatio, unsigned char **ppDest, int *pDataLen)
{
	if (pSrc == NULL || nw <= 0 || nh <= 0 || nCprRatio <= 0 || ppDest == NULL || pDataLen == NULL)
		return -1;

	int		nret, ndpi, nsize;
	float	fratio = 0.0f;
	UCHAR	*compress_buffer = NULL;

	ndpi = 500;
	nsize = 0;

	nret = gfimglib_wsq_encode(pSrc, nw, nh, ndpi, nCprRatio, &compress_buffer, &nsize, &fratio);

	if (nret != GFIMGLIB_ERROR_SUCCESS)
		return nret;

	if (compress_buffer != NULL)
	{
		*ppDest = compress_buffer;
		*pDataLen = nsize;
	}
	
	return 0;
}

INFIS_SPLITBOX_DECL void INFIS_FreeImageWSQ(unsigned char *pWSQ)
{
	if (pWSQ)
		gfimglib_free((void **)&pWSQ);
}


#if 0

int ExtractFingerMntEx(CBTYStatus &btyStatus, int& nErrorNo, char* pszErrorInfo, int ncbErrorInfo,
	int &nImgQlev, int &nExtracted)
{
	nErrorNo = 1;
	nImgQlev = 1;
	nExtracted = 0;

	nImgQlev = 0;
	nExtracted = 1;

	CUTIL_ExtractMNT_LV	ExfLV(nErrorNo);
	GYTH_CAPTIMAGEDATA	*pstFingData;
	int		nw, nh, nFingIdx, nImgSize, nbits;
	UCHAR	*pBitData;

	pBitData = NULL;
	nw = nh = 0;
	nFingIdx = m_utilHelper.FingerFGPToIndex(btyStatus.Fgp);

	if (m_utilHelper.IsFlatFinger(btyStatus.ViewID))
	{
		pstFingData = m_pCtrl->m_stPlainData + nFingIdx;
		ExfLV.bPlain = 1;
		ExfLV.pstFeat = m_FingerQual.m_stPlainFeat + nFingIdx;
		ExfLV.pMntData = m_FingerQual.GetMntBuffer(nFingIdx, 0);
		ExfLV.pstImage = m_FingerQual.m_pstPlainImage;
	}
	else
	{
		pstFingData = m_pCtrl->m_stRollData + nFingIdx;
		ExfLV.bPlain = 0;
		ExfLV.pstFeat = m_FingerQual.m_stRollFeat + nFingIdx;
		ExfLV.pMntData = m_FingerQual.GetMntBuffer(nFingIdx, 1);
		ExfLV.pstImage = m_FingerQual.m_pstRollImage;
	}

	if (!pstFingData->bDataIsValid || !ExfLV.pstImage) return nImgQlev;

	nbits = pstFingData->pbmInfo->bmiHeader.biBitCount;
	nw = pstFingData->pbmInfo->bmiHeader.biWidth;
	nh = pstFingData->pbmInfo->bmiHeader.biHeight;
	if (nh < 0) nh = -nh;
	pBitData = pstFingData->pBitData;
	if (!pBitData || !ExfLV.pMntData || nw < 1 || nh < 1) return nImgQlev;

	ExfLV.pstImage->stHead.nFingerIndex = nFingIdx + 1;
	uint2ToChar2(nw, ExfLV.pstImage->stHead.nWidth);
	uint2ToChar2(nh, ExfLV.pstImage->stHead.nHeight);
	nImgSize = nw * nh;
	uint4ToChar4(nImgSize, ExfLV.pstImage->stHead.nImgSize);
	memset(ExfLV.pstImage->bnData, 255, nImgSize);
	GYTHHELPER_CopyImageData(ExfLV.pstImage->bnData, pBitData, nw, CUtilHelper::GetBmpImgLine(nw, nbits), nh);
	//	memcpy(pstImage->bnData, pBitData, nImgSize);
	if (m_FingerQual.m_pstBinData)
	{
		memset(m_FingerQual.m_pstBinData, 0, m_FingerQual.m_nBinDataLen + sizeof(GAFISIMAGEHEADSTRUCT));
		memcpy(&m_FingerQual.m_pstBinData->stHead, &ExfLV.pstImage->stHead, sizeof(m_FingerQual.m_pstBinData->stHead));
		uint4ToChar4(/*m_FingerQual.m_nBinDataLen*/0, m_FingerQual.m_pstBinData->stHead.nImgSize);
	}
	ExfLV.pstBinData = m_FingerQual.m_pstBinData;
	ExfLV.nThreshold = m_pCtrl->m_ConfigInfo.m_nQualThreshold;
	ExfLV.pszErrorInfo = pszErrorInfo;
	ExfLV.ncbErrorInfo = ncbErrorInfo;

	if (m_pCtrl->m_EQVThread.IsInited()) m_pCtrl->m_EQVThread.UTIL_ExtractMNT_LV(ExfLV);
	else
	{
		nErrorNo = GAFIS_ExtractMNT_LV(ExfLV.pstImage, ExfLV.pMntData, ExfLV.pstBinData, ExfLV.nThreshold, pszErrorInfo);

		UTIL_MntStd2MntDispInfo(ExfLV.pMntData, ExfLV.pstFeat, ExfLV.bPlain);
	}

	nImgQlev = (int)(ExfLV.pstFeat->ImgQlev);
	return nImgQlev;
}

#endif