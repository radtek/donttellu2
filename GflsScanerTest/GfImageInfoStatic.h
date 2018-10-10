#if !defined(AFX_GFIMAGEINFOSTATIC_H__E541A502_EBDE_44FE_92E1_D52FE7A64E03__INCLUDED_)
#define AFX_GFIMAGEINFOSTATIC_H__E541A502_EBDE_44FE_92E1_D52FE7A64E03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GfImageInfoStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGfImageInfoStatic window

class CGfImageInfoStatic : public CStatic
{
// Construction
public:
	CGfImageInfoStatic();

// Attributes
public:
	UCHAR	*m_pImageData, *m_pBadInfoData;		// 图像数据和疵点数据,疵点数据为:如果相应像素为疵点,则值为0(黑色),否则值为255(白色)
	int		m_nDataSize;
	
	int		m_nAverage, m_nSmallGrayCnt;				// 灰度均值，灰度值小于225的像素个数
	int		m_nAllBad1pt, m_nAllBad2pt, m_nAllBad3pt;	// 整个图像范围内的直径为1,大于等于2,大于等于3的疵点的个数
	int		m_nCnrBad1pt, m_nCnrBad2pt, m_nCnrBad3pt;	// 主视场范围内的直径为1,大于等于2,大于等于3的疵点的个数

	UCHAR	m_bDrawBadptInfo;

// Operations
public:
	void InitialBufData();
	void FreeBufferData();
	void InitBadptInfoData();
	void DrawImageOrBadptData(CDC* pDC);
	void DrawBadptPlace(CDC* pDC);
	void SetImageData2BadptStat(UCHAR* pImageData);
	void GetImageDataAverage(UCHAR* pImageData);
	int  GetBadPointRadia(GF_BADPOINTINFO &stBadInfo); 
	int  IsRightPointBad(GF_BADPOINTINFO &stBadInfo);
	int  IsLeftBottomPointBad(GF_BADPOINTINFO &stBadInfo);
	int  IsBottomPointBad(GF_BADPOINTINFO &stBadInfo);
	int  IsRightBottomPointBad(GF_BADPOINTINFO &stBadInfo);
	int  AddBadPoint2Array(GF_BADPOINTINFO &stBadInfo, int x, int y);
	void GetBadPointStatInfo(TCHAR *pszInfo);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGfImageInfoStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGfImageInfoStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGfImageInfoStatic)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GFIMAGEINFOSTATIC_H__E541A502_EBDE_44FE_92E1_D52FE7A64E03__INCLUDED_)
