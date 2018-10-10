#pragma once

#include <string>
// ImageStatic

class CImageStatic : public CStatic
{
	DECLARE_DYNAMIC(CImageStatic)

public:
	CImageStatic();
	virtual ~CImageStatic();

	//CCaptureView	*m_pCaptView;
	//CBTYStatus		m_BtyStatus;
	/*void	SetCaptureViewFgp(CCaptureView* pCaptView, int nBty, int nViewID, int nFgp)
	{
		m_pCaptView = pCaptView;
		m_BtyStatus.SetBtyViewIDFgp(pCaptView, nBty, nViewID, nFgp);
	}*/

	void	SetText(const char *pszView, const char *pszFg)
	{
		m_pszFg = pszFg;
		m_pszView = pszView;
	}

	void	GetImageWndRect();
	void	DrawImage();

	void	SetSelectState(bool bSelectd, bool bRedraw = true);

	void	SetTextFont(CFont *pFont) { m_pTextFont = pFont; };

	void	SetImageInfo(const std::string &strImage, int nWidth, int nHeight);

	void	SetImageInfoAndDraw(const char *pCaptData, int nDataLen, int nWidth, int nHeight)
	{
		std::string temp;
		if (pCaptData != NULL && nDataLen > 0)
			temp.assign(pCaptData, nDataLen);

		if (nWidth < 0 || nHeight < 0)
			nWidth = nHeight = 0;

		SetImageInfo(temp, nWidth, nHeight);
		DrawImage();
	}

	int SaveImage2File(int nType);

	void SetFgp(int nType, int nFgp)
	{
		_nCaptureType = nType;
		_nFgp = nFgp;
	}

	BITMAPINFO		*_pbmInfo;
	//std::string		_strImage;
	char			*_pData;	//_pbmInfo + sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)* 256
private:
	bool	_bValid;	//是否有效的图
	int		_nFgp;		//指位
	int		_nCaptureType;	//类型

	CFont	*m_pTextFont;	//!< 字体

	RECT	m_rtImage;
	double	m_fScale;

	bool	m_bGotRect;			//!< 是否已经得到了图像的显示区域
	bool	m_bIsSelected;		//!< 当前是否选中了该框

	const char*	m_pszFg;		//!< 指位
	const char* m_pszView;

	//!< 根据当前指位是否缺指、必须采集、禁止采集、选中等状态返回相应的边框颜色
	COLORREF	GetFrameColor();

	void	DrawWedgeAndText(CDC *pDC);
	void	DrawScale(HWND hWndImg, CDC *pDC);

	
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRightBtyNotExist();
	afx_msg void OnRightGetBtyFromFile();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

