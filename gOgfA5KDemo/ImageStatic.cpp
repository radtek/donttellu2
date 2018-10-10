
// ImageStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "gOgfA5KDemo.h"
#include "ImageStatic.h"
#include "../infissplitbox/infissplitboxdef.h"
#include "CaptureDlg.h"
#include "ImageShow.h"
// CImageStatic

IMPLEMENT_DYNAMIC(CImageStatic, CStatic)

CImageStatic::CImageStatic()
{
//	m_pCaptView = NULL;
	m_bGotRect = false;
	m_bIsSelected = false;

	memset(&m_rtImage, 0, sizeof(m_rtImage));
	m_fScale = 1.0;

	m_pszFg = NULL;
	m_pszView = NULL;

	m_pTextFont = NULL;

	_pbmInfo = NULL;
	_pData = NULL;

	_bValid = false;
	_nCaptureType = CCaptureDlg::CAPTURE_SLAP;
	_nFgp = 1;
}

CImageStatic::~CImageStatic()
{
	if (_pbmInfo)
	{
		delete _pbmInfo;
		_pbmInfo = NULL;
	}
}


BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//ON_WM_CONTEXTMENU()
	//ON_COMMAND(IDM_RIGHT_BTYNOTEXIST, &CImageStatic::OnRightBtyNotExist)
	//ON_COMMAND(IDM_RIGHT_GETBTYFROMFILE, &CImageStatic::OnRightGetBtyFromFile)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CImageStatic message handlers

void CImageStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	DrawImage();
}

void CImageStatic::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

BOOL CImageStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}

//void CImageStatic::OnContextMenu(CWnd* pWnd, CPoint point)
//{
//	// TODO: Add your message handler code here
//	//!< 禁止采集
//	if (m_BtyStatus.pnCaptOption && (*m_BtyStatus.pnCaptOption == CConfigInfo::FGCAPT_DISABLE)) return;
//	if (!m_BtyStatus.pstCaptImage) return;
//
//	//!< 非指纹采集是否需要显示右键菜单?
//	//if ( m_BtyStatus.nBty != GAFIS7::CodeData::BTY_FINGER ) return;
//
//	//!< 重采时，非重采标记的不显示右键菜单。
//	if (!m_pCaptView->m_pCtrl->m_vecRegather.empty() && m_BtyStatus.pbReGather && !*m_BtyStatus.pbReGather)
//	{
//		//由于缺指操作会清空当前m_BtyStatus.pbReGather，因此误缺指后，想改回来，就需要验证是否真是重采指位。
//		bool	bReGather = false;
//		for (unsigned int i = 0; i < m_pCaptView->m_pCtrl->m_vecRegather.size(); ++i)
//		{
//			const GAFIS7LOB_HEADSTRUCT	&head = m_pCaptView->m_pCtrl->m_vecRegather[i];
//			if (head.nBty == (UCHAR)m_BtyStatus.nBty &&
//				head.nFgp == (UCHAR)m_BtyStatus.Fgp &&
//				head.nViewId == (UCHAR)m_BtyStatus.ViewID)
//				bReGather = true;
//
//			//目前是缺指状态，准备取消缺指：可以判断相反viewid是否真是重采。
//			if (m_BtyStatus.pbNotExist && *m_BtyStatus.pbNotExist)
//			{
//				//同一指位，相反viewid如果是重采，也可弹出菜单。前提是当前已经是缺指状态。
//				if (head.nBty == (UCHAR)m_BtyStatus.nBty &&
//					head.nFgp == (UCHAR)m_BtyStatus.Fgp &&
//					head.nViewId == (UCHAR)!m_BtyStatus.ViewID)
//					bReGather = true;
//			}
//		}
//
//		if (!bReGather)
//			return;
//	}
//
//	CMenu menu, *pSub;
//
//	if (!menu.LoadMenu(IDR_MENU_RIGHT)) return;
//	pSub = menu.GetSubMenu(0);
//
//	if (!pSub)
//	{
//		menu.DestroyMenu();
//		return;
//	}
//
//	if (m_BtyStatus.pbNotExist)
//	{
//		if (*m_BtyStatus.pbNotExist)
//		{
//			pSub->CheckMenuItem(IDM_RIGHT_BTYNOTEXIST, MF_BYCOMMAND | MF_CHECKED);
//			pSub->EnableMenuItem(IDM_RIGHT_GETBTYFROMFILE, MF_BYCOMMAND | MF_GRAYED);
//		}
//	}
//	else pSub->EnableMenuItem(IDM_RIGHT_BTYNOTEXIST, MF_BYCOMMAND | MF_GRAYED);
//
//	pSub->TrackPopupMenu(TPM_LEFTBUTTON | TPM_LEFTALIGN | TPM_VERTICAL, point.x, point.y, this);
//
//	menu.DestroyMenu();
//}

void CImageStatic::SetImageInfo(const std::string &strImage, int nWidth, int nHeight)
{
	if (_pbmInfo)
	{
		delete _pbmInfo;
		_pbmInfo = NULL;
	}

	int		nImageLen = strImage.size();
	if (strImage.empty())
	{
		_bValid = false;
		nImageLen = nWidth * nHeight;
	}
	else
		_bValid = true;
	
	char	*mmbuf = new char[sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)* 256 + nImageLen];
	memset(mmbuf, 255, sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)* 256 + nImageLen);

	//Create Bitmapinfo header
	BITMAPINFOHEADER bmi;
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biWidth = nWidth;
	bmi.biHeight = -abs(nHeight);
	bmi.biPlanes = 1;
	bmi.biBitCount = 8;
	bmi.biCompression = 0;
	bmi.biSizeImage = nWidth * abs(nHeight);
	bmi.biXPelsPerMeter = 0;
	bmi.biYPelsPerMeter = 0;
	bmi.biClrUsed = 256;
	bmi.biClrImportant = 0;

	//load infoheader
	memcpy(mmbuf, &bmi, sizeof(BITMAPINFOHEADER));;
	//create palatte,Bitmap palette: grey scale, 256 levels
	
	char *argbq = mmbuf + sizeof(BITMAPINFOHEADER);
	for (int i = 0; i < 256; i++)
	{
		*argbq++ = i; *argbq++ = i; *argbq++ = i; *argbq++ = 0;
	}

	_pData = mmbuf + sizeof(BITMAPINFOHEADER)+256 * sizeof(RGBQUAD);
	if (!strImage.empty())
		memcpy(_pData, strImage.c_str(), nImageLen);;

	_pbmInfo = (BITMAPINFO *)mmbuf;
}

void CImageStatic::GetImageWndRect()
{
	if (m_bGotRect) return;
	if (!IsWindow(GetSafeHwnd())) return;
	if (!_pData || !_pbmInfo) return;

	RECT	rtWnd;

	int		nWidth, nHeight;

	nWidth = _pbmInfo->bmiHeader.biWidth;
	nHeight = abs(_pbmInfo->bmiHeader.biHeight);
	if (nWidth < 1 || nHeight < 1) return;

	GetClientRect(&rtWnd);
	double	fx = (rtWnd.right - rtWnd.left) * 1.0 / nWidth;
	double	fy = (rtWnd.bottom - rtWnd.top) * 1.0 / nHeight;
	int		nwDisp, nhDisp, ntmpsize, nxDeflate, nyDeflate;

	if (fx < fy)
	{
		nwDisp = (rtWnd.right - rtWnd.left) / 4 * 4;
		ntmpsize = (int)(nHeight * fx);
		nhDisp = ntmpsize / 4 * 4;
		m_fScale = fx;
	}
	else
	{
		nhDisp = (rtWnd.bottom - rtWnd.top) / 4 * 4;
		ntmpsize = (int)(nWidth * fy);
		nwDisp = ntmpsize / 4 * 4;
		m_fScale = fy;
	}
	nxDeflate = ((rtWnd.right - rtWnd.left) - nwDisp) / 2;
	nyDeflate = ((rtWnd.bottom - rtWnd.top) - nhDisp) / 2;
	::InflateRect(&rtWnd, -nxDeflate, -nyDeflate);

	memcpy(&m_rtImage, &rtWnd, sizeof(rtWnd));
	m_bGotRect = true;
}

void CImageStatic::DrawImage()
{
	HWND	hWndImg = GetSafeHwnd();

	if (!hWndImg || !IsWindow(hWndImg)) return;	//|| ( m_nWidth * m_nHeight ) < 2 ) return;

	CDC		*pDC = NULL;

	pDC = GetDC();
	if (NULL == pDC) return;

	DrawScale(hWndImg, pDC);

	ReleaseDC(pDC);
}

void CImageStatic::DrawScale(HWND hWndImg, CDC *pDC)
{
	RECT	rtClient;
	int		nDispWidth = 0, nDispHeight = 0;
	int		nImgWidth = 0, nImgHeight = 0;
	COLORREF clrText;

	GetImageWndRect();

	::GetClientRect(hWndImg, &rtClient);
	//nDispWidth = (rtClient.right - rtClient.left) / 4 * 4;
	nDispWidth = rtClient.right - rtClient.left;
	nDispHeight = (rtClient.bottom - rtClient.top);
	rtClient.right = nDispWidth;

	nImgWidth = m_rtImage.right - m_rtImage.left;
	nImgHeight = m_rtImage.bottom - m_rtImage.top;
	/*if (_pbmInfo)
	{
		nImgWidth = _pbmInfo->bmiHeader.biWidth;
		nImgHeight = abs(_pbmInfo->bmiHeader.biHeight);
	}*/

	CDC		dc;
	CBitmap	bmp, *pOldbmp;
	CBrush	brush(RGB(245, 249, 255)/*CUtilHelper::m_crBackground*/);
	bool	bExist;

	dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, nDispWidth, nDispHeight);
	pOldbmp = dc.SelectObject(&bmp);

	//clrText = dc.SetTextColor(GetFrameColor() == RGB(192, 192, 192) ? RGB(192, 192, 192) : CUtilHelper::m_crHandDescText);//如果不可用则字体变灰
	dc.SetBkMode(TRANSPARENT);

	dc.FillRect(&rtClient, &brush);

	//!< 重采是否显示图像
	//if ( m_BtyStatus.pbReGather && *m_BtyStatus.pbReGather )	bExist = false;
	//if (bExist && CConfigInfo::IsCapturedDataValid(m_BtyStatus.pstCaptImage))
	if (_pbmInfo)
	{
		int		nRealWidth, nRealHeight;
		BITMAPINFO	*pbmpInfo = _pbmInfo;

		nRealWidth = pbmpInfo->bmiHeader.biWidth;
		nRealHeight = abs(pbmpInfo->bmiHeader.biHeight);

		::SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
		::StretchDIBits(dc.GetSafeHdc(),
			m_rtImage.left, m_rtImage.top, nImgWidth, nImgHeight,
			0, 0, nRealWidth, nRealHeight, _pData, pbmpInfo, DIB_RGB_COLORS, SRCCOPY);
	}

	DrawWedgeAndText(&dc);

	pDC->BitBlt(rtClient.left, rtClient.top, nDispWidth, nDispHeight, &dc, 0, 0, SRCCOPY);

	//dc.SetTextColor(clrText);
	if (pOldbmp) dc.SelectObject(pOldbmp);

	brush.DeleteObject();
	bmp.DeleteObject();
	//dc.DeleteDC();
}

void CImageStatic::DrawWedgeAndText(CDC *pDC)
{
	CPen	pen, penRed, *poldpen;
	CBrush	brush, *poldbrush;
	CRect	rtClietn, rt1, rt2, rt3;
	COLORREF cr;
	UINT	nFormat;
	CFont	*pCurrentFont, *pOldFont;

	//if (m_pTextFont) pCurrentFont = m_pTextFont;
	//else pCurrentFont = GetFont();
	//pOldFont = pDC->SelectObject(pCurrentFont);

	GetClientRect(rtClietn);
	rt1 = rtClietn;
	rt2 = rtClietn;
	rt3 = rtClietn;

	if (m_pszView || m_pszFg)
	{
		//if ((m_BtyStatus.pbNotExist && *m_BtyStatus.pbNotExist) || m_BtyStatus.pbReGather && *m_BtyStatus.pbReGather)
		if (1)
		{
			rt1.bottom = rtClietn.top + rtClietn.Height() * 3 / 10;
			rt2.top = rt1.bottom;	rt2.bottom = rt2.top + rt1.Height();
			rt3.top = rt2.bottom;
		}
		else
		{
			rt1.bottom = rtClietn.top + rtClietn.Height() / 2;
			rt2.top = rt1.bottom;
		}

		if (!m_pszView || !m_pszFg)
		{
			rt1.bottom = rt2.bottom;
			rt2 = rt1;
		}
	}
	nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

	//if (m_pszView) pDC->DrawText(m_pszView, _tcslen(m_pszView), &rt1, nFormat);
	//if (m_pszFg) pDC->DrawText(m_pszFg, _tcslen(m_pszFg), &rt2, nFormat);
#if 0
	//!< 缺指优先于重采。 缺指不会打回重采，重采可以选择缺指
	if (m_BtyStatus.pbNotExist && *m_BtyStatus.pbNotExist)
	{
		pDC->DrawText(CUtilHelper::m_szNotExist, ga_strlen(CUtilHelper::m_szNotExist), &rt3, nFormat);
	}
	else if (m_BtyStatus.pbReGather && *m_BtyStatus.pbReGather)
	{
		COLORREF clrOld = pDC->SetTextColor(RGB(231, 76, 60));
		pDC->DrawText(CUtilHelper::m_szReGather, ga_strlen(CUtilHelper::m_szReGather), &rt3, nFormat);
		pDC->SetTextColor(clrOld);
	}
#endif
	cr = GetFrameColor();
	pen.CreatePen(PS_SOLID, 1, cr);
	brush.CreateStockObject(NULL_BRUSH);
	poldpen = pDC->SelectObject(&pen);
	poldbrush = pDC->SelectObject(&brush);

	pDC->Rectangle(&rtClietn);

	pDC->SelectObject(poldpen);
	pDC->SelectObject(poldbrush);
//	pDC->SelectObject(pOldFont);

	pen.DeleteObject();
	brush.DeleteObject();
	penRed.DeleteObject();

}

COLORREF CImageStatic::GetFrameColor()
{
	return RGB(64, 64, 64);
#if 0
	//!< 禁止采集
	if (m_BtyStatus.pnCaptOption && (*m_BtyStatus.pnCaptOption == CConfigInfo::FGCAPT_DISABLE))
	{
		return RGB(192, 192, 192);
	}

	//!< 缺指
	if (m_BtyStatus.pbNotExist && *m_BtyStatus.pbNotExist)
	{
		return RGB(64, 64, 64);
	}

	//!< 重采
	if (!m_bIsSelected && m_BtyStatus.pbReGather && *m_BtyStatus.pbReGather)
	{
		return RGB(0, 0, 255);
	}

	//!< 选中
	if (m_bIsSelected) return RGB(255, 0, 0);
	else return CUtilHelper::m_crFrame;
#endif
}

void CImageStatic::OnRightBtyNotExist()
{
#if 0
	// TODO: Add your command handler code here
	if (!m_BtyStatus.pbNotExist) return;
	if (!m_pCaptView || !m_pCaptView->m_pCtrl) return;

	//!< 重采时，非重采标记的不允许缺指操作。  OnContextMenu处理过了
	//if(!m_pCaptView->m_pCtrl->m_vecRegather.empty() && m_BtyStatus.pbReGather && !*m_BtyStatus.pbReGather)
	//	return;

	if (!*m_BtyStatus.pbNotExist && m_BtyStatus.nBty == GAFIS7::CodeData::BTY_FINGER)
	{
		if (m_BtyStatus.ViewID == GAFIS7::CodeData::FINGER_VIEWID_ROLL)
		{
			//在滚指点缺指，判断平面是否已采
			if (m_pCaptView->m_pCtrl->m_ConfigInfo.FingerHasCaptured(m_pCaptView->m_pCtrl->m_stPlainData + CUtilHelper::FingerFGPToIndex(m_BtyStatus.Fgp), m_BtyStatus.Fgp, GAFIS7::CodeData::FINGER_VIEWID_FLAT))
			{
				GFPUTILITY_INFOSTRUCT	stUtilityInfo = { 0 };

				stUtilityInfo.fInfoType = GFP_UTILITY_INFOTYPE_ASKYESNO;
				stUtilityInfo.szInformation = "此指位平面指纹已采，请确认是否缺指";
				stUtilityInfo.pParentWnd = m_pCaptView;
				stUtilityInfo.szTitle = theApp.m_stUtilityInfo.szTitle;

				/*int	retval = Gfp_UtilityDisplayInfo(&stUtilityInfo);
				if(retval == IDNO)
				return;*/

				CDlgYesNo dlg(this);

				dlg.m_strTitle = stUtilityInfo.szTitle;
				dlg.m_strInfo = _T("警告");
				dlg.m_strInfo2 = stUtilityInfo.szInformation;
				dlg.m_bShowCancel = (stUtilityInfo.fInfoType == GFP_UTILITY_INFOTYPE_ASKYESCANCEL);

				dlg.DoModal();

				if (dlg.m_nRetValue == IDNO)
					return;
			}
		}
		else
		{
			//在平面点缺指，判断滚动是否已采
			if (m_pCaptView->m_pCtrl->m_ConfigInfo.FingerHasCaptured(m_pCaptView->m_pCtrl->m_stRollData + CUtilHelper::FingerFGPToIndex(m_BtyStatus.Fgp), m_BtyStatus.Fgp, GAFIS7::CodeData::FINGER_VIEWID_ROLL))
			{
				GFPUTILITY_INFOSTRUCT	stUtilityInfo = { 0 };

				stUtilityInfo.fInfoType = GFP_UTILITY_INFOTYPE_ASKYESNO;
				stUtilityInfo.szInformation = "此指位滚动指纹已采，请确认是否缺指";
				stUtilityInfo.pParentWnd = m_pCaptView;
				stUtilityInfo.szTitle = theApp.m_stUtilityInfo.szTitle;

				/*int	retval = Gfp_UtilityDisplayInfo(&stUtilityInfo);
				if(retval == IDNO)
				return;*/

				CDlgYesNo dlg(this);

				dlg.m_strTitle = stUtilityInfo.szTitle;
				dlg.m_strInfo = _T("警告");
				dlg.m_strInfo2 = stUtilityInfo.szInformation;
				dlg.m_bShowCancel = (stUtilityInfo.fInfoType == GFP_UTILITY_INFOTYPE_ASKYESCANCEL);

				dlg.DoModal();

				if (dlg.m_nRetValue == IDNO)
					return;
			}
		}
	}

	bool	bRegather = false;
	bool	bSameFingerRegather = false;

	*m_BtyStatus.pbNotExist = !(*m_BtyStatus.pbNotExist);
	if (*m_BtyStatus.pbNotExist)
	{
		CCatchedBlob *pBfv_Blob;

		pBfv_Blob = m_pCaptView->m_pCtrl->GetPointedCatchedBlob(m_BtyStatus.nBty, m_BtyStatus.Fgp, m_BtyStatus.ViewID);
		if (pBfv_Blob) pBfv_Blob->DeleteBlob();
		if (m_BtyStatus.pstCaptImage) m_BtyStatus.pstCaptImage->bDataIsValid = 0;

		if (m_pCaptView->m_dlgCaptArea.GetCurSelectedImageStatic() == this)
		{
			m_pCaptView->m_dlgFrameArea.m_staticFrame.SetBTYCaptData(NULL);
			m_pCaptView->m_dlgFrameArea.m_FingerEditBlock.SetMntAndImage(NULL, NULL);

#if 1
			//清空特征
			bool	bNeedExtract = false;
			UTIL_EXTRACTDATA	*pExtracData = m_pCaptView->m_pCtrl->GetPointedExtractData(m_BtyStatus.nBty, m_BtyStatus.Fgp, m_BtyStatus.ViewID, bNeedExtract);
			if (pExtracData)
			{
				AFIS_FREE(pExtracData->pstMntData);
			}
#endif
		}
		//清除重采标记
		bRegather = false;
		//滚指缺指应同时清除同一指位的平面的重采标记，反之亦然
		bSameFingerRegather = false;

		//清除质量得分
		m_BtyStatus.nScore = -1;
	}
	else
	{

		//由于缺指操作会清空当前m_BtyStatus.pbReGather，因此误缺指后，想改回来，就需要验证是否真是重采指位。
		for (unsigned int i = 0; i < m_pCaptView->m_pCtrl->m_vecRegather.size(); ++i)
		{
			const GAFIS7LOB_HEADSTRUCT	&head = m_pCaptView->m_pCtrl->m_vecRegather[i];
			if (head.nBty == (UCHAR)m_BtyStatus.nBty &&
				head.nFgp == (UCHAR)m_BtyStatus.Fgp &&
				head.nViewId == (UCHAR)m_BtyStatus.ViewID)
				bRegather = true;	//恢复重采标记

			if (head.nBty == (UCHAR)m_BtyStatus.nBty &&
				head.nFgp == (UCHAR)m_BtyStatus.Fgp &&
				head.nViewId == (UCHAR)!m_BtyStatus.ViewID)
				bSameFingerRegather = true;	//恢复同一指位，相反viewid的重采标记
		}
	}

	{
		//!< 去除或恢复重采标记
		if (m_BtyStatus.pbReGather)
			*m_BtyStatus.pbReGather = bRegather;

		//!< 同时去除或恢复平指、滚指的重采。
		if (m_BtyStatus.nBty == GAFIS7::CodeData::BTY_FINGER)
		{
			//滚指缺指，则同时清除平面的重采标记。恢复时也要判断是否真的是重采。
			if (m_BtyStatus.ViewID == GAFIS7::CodeData::FINGER_VIEWID_ROLL)
			{
				m_pCaptView->m_pCtrl->m_ConfigInfo.m_bPlainFingerReGather[CUtilHelper::FingerFGPToIndex(m_BtyStatus.Fgp)] = bSameFingerRegather;
			}
			else
			{
				m_pCaptView->m_pCtrl->m_ConfigInfo.m_bRollFingerReGather[CUtilHelper::FingerFGPToIndex(m_BtyStatus.Fgp)] = bSameFingerRegather;
			}
		}
		else if (m_BtyStatus.nBty == GAFIS7::CodeData::BTY_PALM)
		{
			if (m_BtyStatus.Fgp == GAFIS7::CodeData::R_PALM_Q || m_BtyStatus.Fgp == GAFIS7::CodeData::L_PALM_Q ||
				m_BtyStatus.Fgp == GAFIS7::CodeData::R_PALM || m_BtyStatus.Fgp == GAFIS7::CodeData::L_PALM)
				m_pCaptView->m_pCtrl->m_ConfigInfo.m_bPalmReGather[CUtilHelper::PalmFGPToIndex(m_BtyStatus.Fgp)] = bRegather;
			else if (m_BtyStatus.Fgp == GAFIS7::CodeData::L_FOUR ||
				m_BtyStatus.Fgp == GAFIS7::CodeData::R_FOUR ||
				m_BtyStatus.Fgp == GAFIS7::CodeData::RL_THUMB
				)
				m_pCaptView->m_pCtrl->m_ConfigInfo.m_bSlapReGather[CUtilHelper::PalmFGPToIndex(m_BtyStatus.Fgp)] = bRegather;
		}
	}

	m_pCaptView->m_pCtrl->SendCaptureProgressEvent();
	m_pCaptView->m_pCtrl->SendModifiedEvent();

	DrawImage();
#endif
}

void CImageStatic::OnRightGetBtyFromFile()
{
#if 0
	// TODO: Add your command handler code here
	if (!m_pCaptView || !m_pCaptView->m_pCtrl || !m_BtyStatus.pstCaptImage) return;

	//!< 重采时，非重采标记的不允许从图片导入操作。  OnContextMenu处理过了
	//if(!m_pCaptView->m_pCtrl->m_vecRegather.empty() && m_BtyStatus.pbReGather && !*m_BtyStatus.pbReGather)
	//	return;

	m_pCaptView->m_pCtrl->TerminateScanThread(true);

	char	szImgFileInfo[256] = { 0 };
	GAFISRC_LoadString(IDS_GFLVSCAN_IMGFILE_FILTER, szImgFileInfo, sizeof(szImgFileInfo));

	CFileDialog dlg(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT, szImgFileInfo, this);
	int		retval;

	retval = dlg.DoModal();
	if (retval != IDOK) return;

	char szFileName[512];
	GF_IMAGEINFOSTRUCT stImageInfo = { 0 };

	strncpy(szFileName, (const char*)(LPCTSTR)dlg.GetPathName(), sizeof(szFileName));
	stImageInfo.bmpHead.biBitCount = CUtilHelper::BPP_8;
	retval = GfImage_LoadImageFile(szFileName, &stImageInfo, 1);
	if (retval != GFIMAGEFORMAT_ERROR_SUCCESS)
	{
		GfImage_FreeImageInfo(&stImageInfo);

		char	szErrorInfo[128];
		CString	strInfo;

		GAFISRC_LoadString(IDS_IMAGEFROMFILE_ERROR, szErrorInfo, sizeof(szErrorInfo));
		strInfo = szErrorInfo;
		m_pCaptView->m_pCtrl->FireBTYCaptureEvent(m_BtyStatus.nBty, m_BtyStatus.Fgp, m_BtyStatus.ViewID,
			GFS_CAPTSTATUS_DOCAPTURE, strInfo);
		return;
	}

	if (stImageInfo.bmpHead.biHeight < 0)
	{
		stImageInfo.bmpHead.biHeight = -stImageInfo.bmpHead.biHeight;
		Gfp_OperatorImageDataEx(stImageInfo.pImgData, stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biHeight,
			stImageInfo.bmpHead.biBitCount, GFP_OPERATORIMAGEDATA_TYPE_VMIRROR);
	}

	int		nWidth, nHeight, nBPP, nSrcLine;

	if (!m_BtyStatus.pstCaptImage->pbmInfo)
	{
		nWidth = m_pCaptView->m_pCtrl->m_CaptCard.GetBTYDefWidth(m_BtyStatus.nBty, m_BtyStatus.Fgp);
		nHeight = m_pCaptView->m_pCtrl->m_CaptCard.GetBTYDefHeight(m_BtyStatus.nBty, m_BtyStatus.Fgp);
		nBPP = m_pCaptView->m_utilHelper.m_stbi.bmiHeader.biBitCount;
	}
	else
	{
		nWidth = m_BtyStatus.pstCaptImage->pbmInfo->bmiHeader.biWidth;
		nHeight = abs(m_BtyStatus.pstCaptImage->pbmInfo->bmiHeader.biHeight);
		nBPP = m_BtyStatus.pstCaptImage->pbmInfo->bmiHeader.biBitCount;
	}
	if (stImageInfo.bIsPacketDIB)
	{
		nSrcLine = CUtilHelper::GetBmpImgLine(stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biBitCount);
	}
	else nSrcLine = CUtilHelper::GetImageLine(stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biBitCount);

	retval = m_pCaptView->m_utilHelper.UTIL_BuildCaptImageData(*m_BtyStatus.pstCaptImage, nWidth, nHeight, nBPP,
		stImageInfo.pImgData, stImageInfo.bmpHead.biWidth, stImageInfo.bmpHead.biHeight, nSrcLine, stImageInfo.bmpHead.biBitCount);
	GfImage_FreeImageInfo(&stImageInfo);
	if (retval < 1) return;

	m_pCaptView->m_dlgFrameArea.ShowImageDataInFrame(this);
	DrawImage();

	if (m_BtyStatus.nBty == GAFIS7::CodeData::BTY_FINGER) m_pCaptView->ShiftNextFinger(m_BtyStatus.pstCaptImage->pBitData, m_BtyStatus);
	else
	{
		m_BtyStatus.AddCaptImage2ExtractQueue(m_pCaptView->m_pCtrl);
		if (m_BtyStatus.bSlap) m_pCaptView->SplitFlatFinger(this);
	}
#endif
}

void CImageStatic::SetSelectState(bool bSelectd, bool bRedraw)
{
#if 0
	if (bSelectd == m_bIsSelected)
	{
		//最后一个指位不会更改select为false状态，导致不能算分。
		if (m_pCaptView->m_pCtrl->m_enumZone == CgythFingerPalmCtrl::ZONE_GUIZHOU)
		{
			//检查当前viewid是否还有未重采的指位。如果有，则当前不是最后一个。
			bool	lastRegather = true;
			for (int i = 0; i < 10; ++i)
			{
				if (m_BtyStatus.ViewID == GAFIS7::CodeData::FINGER_VIEWID_FLAT)
				{
					if (m_pCaptView->m_pCtrl->m_ConfigInfo.m_bPlainFingerReGather[i])
						lastRegather = false;
				}
				else
				{
					if (m_pCaptView->m_pCtrl->m_ConfigInfo.m_bRollFingerReGather[i])
						lastRegather = false;
				}
			}

			if (lastRegather &&
				m_BtyStatus.pstCaptImage->bDataIsValid &&
				m_BtyStatus.nBty == GAFIS7::CodeData::BTY_FINGER
				)
			{
				//入提特征队列
				if (m_pCaptView->m_pCtrl->m_pSvr)
				{
					m_pCaptView->m_pCtrl->m_pSvr->addTask(m_BtyStatus);
				}
			}
		}

		return;
	}
	m_bIsSelected = bSelectd;
	if (bRedraw)	DrawImage();

	//!< 播放语音
	if (m_pCaptView == NULL)	return;
	if (*m_BtyStatus.pbNotExist)	return;

	//!< 贵州边采边提特征
	if (m_pCaptView->m_pCtrl->m_enumZone == CgythFingerPalmCtrl::ZONE_GUIZHOU)
	{
		if (bSelectd == false &&
			m_BtyStatus.pstCaptImage->bDataIsValid &&
			m_BtyStatus.nBty == GAFIS7::CodeData::BTY_FINGER
			)
		{
			//入提特征队列
			if (m_pCaptView->m_pCtrl->m_pSvr)
			{
				m_pCaptView->m_pCtrl->m_pSvr->addTask(m_BtyStatus);
			}
		}
	}

	//!< 只播放一次
	if (m_BtyStatus.pstCaptImage->bDataIsValid)	return;

	if (m_pCaptView->m_pCtrl->m_pSoundPlayer && bSelectd)
	{
		int		nEnum = -1;
		if (m_BtyStatus.nBty == GAFIS7::CodeData::BTY_FINGER)
		{
			int	nBaseFgp = -1;
			if (m_BtyStatus.ViewID == GAFIS7::CodeData::FINGER_VIEWID_ROLL)
			{
				nBaseFgp = 0;
			}
			else if (m_BtyStatus.ViewID == GAFIS7::CodeData::FINGER_VIEWID_FLAT)
			{
				nBaseFgp = GAFIS7::CodeData::FINGER_L_LITTLE;
			}

			nEnum = m_BtyStatus.Fgp - GAFIS7::CodeData::FINGER_R_THUMB + nBaseFgp + FingerPalmSoundHelper::SOUND_FINGER_R_THUMB_ROLL;
		}
		else if (m_BtyStatus.nBty == GAFIS7::CodeData::BTY_PALM)
		{
			switch (m_BtyStatus.Fgp)
			{
			case GAFIS7::CodeData::L_PALM_Q:
			case GAFIS7::CodeData::L_PALM:
				nEnum = FingerPalmSoundHelper::SOUND_PALM_L_PALM;
				break;
			case GAFIS7::CodeData::R_PALM_Q:
			case GAFIS7::CodeData::R_PALM:
				nEnum = FingerPalmSoundHelper::SOUND_PALM_R_PALM;
				break;
			case GAFIS7::CodeData::L_WRITER:
				nEnum = FingerPalmSoundHelper::SOUND_PALM_L_WRITER;
				break;
			case GAFIS7::CodeData::R_WRITER:
				nEnum = FingerPalmSoundHelper::SOUND_PALM_R_WRITER;
				break;
			case GAFIS7::CodeData::R_FOUR:
				nEnum = FingerPalmSoundHelper::SOUND_PALM_R_FOUR;
				break;
			case GAFIS7::CodeData::L_FOUR:
				nEnum = FingerPalmSoundHelper::SOUND_PALM_L_FOUR;
				break;
			case GAFIS7::CodeData::RL_THUMB:
				nEnum = FingerPalmSoundHelper::SOUND_PALM_RL_THUMB;
				break;
			default:
				break;
			}
		}

		//!< 未进行干湿指测试，并且支持测试
		if (false == m_pCaptView->m_pCtrl->m_ConfigInfo.m_bDryWetTestOK &&
			m_pCaptView->m_pCtrl->m_CaptCard.FingerIsSupportHumidityTest())
		{
			std::string	strFileName;
			nEnum = FingerPalmSoundHelper::SOUND_FINGER_NEEDDRYTEST;
			m_pCaptView->m_pCtrl->m_pSoundPlayer->queryFileName(nEnum, strFileName);
			m_pCaptView->m_pCtrl->m_pSoundPlayer->playSound(strFileName, 1);

			return;
		}

		std::string	strFileName;
		m_pCaptView->m_pCtrl->m_pSoundPlayer->queryFileName(nEnum, strFileName);
		m_pCaptView->m_pCtrl->m_pSoundPlayer->playSound(strFileName, 1);
	}
#endif
}

int CImageStatic::SaveImage2File(int nType)
{
	if (_bValid == false || !_pData || !_pbmInfo)
		return -1;

	char	szBuf[MAX_PATH] = { 0 };
	char	*pData = NULL;
	int		nDataLen = 0;
	int		nWidth = _pbmInfo->bmiHeader.biWidth;
	int		nHeight = abs(_pbmInfo->bmiHeader.biHeight);

	switch (nType)
	{
	case IMAGE_TYPE_BMP:

		sprintf(szBuf, "%d_%d.bmp", _nCaptureType, _nFgp);
		return INFIS_SaveDataAsBMP(szBuf, (UCHAR *)_pData, nWidth, nHeight, 8);
		break;
	case IMAGE_TYPE_WSQ:

		sprintf(szBuf, "%d_%d.wsq", _nCaptureType, _nFgp);
		if (0 > INFIS_CompressByWSQ((UCHAR *)_pData, nWidth, nHeight, 10, (UCHAR **)&pData, &nDataLen) ||
			pData == NULL || nDataLen <= 0)
			return -1;

		{
			FILE	*fp = fopen(szBuf, "wb");
			if (fp != NULL)
			{
				fwrite(pData, nDataLen, 1, fp);
				fclose(fp);
			}
		}
		break;
	default:
		return -1;
	}

	INFIS_FreeImageWSQ((UCHAR *)pData);
	return 0;
}



void CImageStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (_pbmInfo == NULL)
		return;

	CImageShow	dlg;
	
	int nWidth = _pbmInfo->bmiHeader.biWidth;
	int nHeight = abs(_pbmInfo->bmiHeader.biHeight);
	char	*mmbuf = (char *)_pbmInfo;
	char	*pData = mmbuf + sizeof(BITMAPINFOHEADER)+256 * sizeof(RGBQUAD);
	std::string	image(pData, nWidth * nHeight);
	dlg._stImage.SetImageInfo(image, nWidth, nHeight);
	
	dlg.DoModal();

	CStatic::OnLButtonDblClk(nFlags, point);
}
