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
	bool	_bValid;	//�Ƿ���Ч��ͼ
	int		_nFgp;		//ָλ
	int		_nCaptureType;	//����

	CFont	*m_pTextFont;	//!< ����

	RECT	m_rtImage;
	double	m_fScale;

	bool	m_bGotRect;			//!< �Ƿ��Ѿ��õ���ͼ�����ʾ����
	bool	m_bIsSelected;		//!< ��ǰ�Ƿ�ѡ���˸ÿ�

	const char*	m_pszFg;		//!< ָλ
	const char* m_pszView;

	//!< ���ݵ�ǰָλ�Ƿ�ȱָ������ɼ�����ֹ�ɼ���ѡ�е�״̬������Ӧ�ı߿���ɫ
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

