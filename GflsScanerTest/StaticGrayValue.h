#if !defined(AFX_STATICGRAYVALUE_H__4DA8ED79_1CC5_4A91_874C_202E8C8B7404__INCLUDED_)
#define AFX_STATICGRAYVALUE_H__4DA8ED79_1CC5_4A91_874C_202E8C8B7404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticGrayValue.h : header file
//

class CDlgImageInfo;
class CDlgGrayStatistic;

/////////////////////////////////////////////////////////////////////////////
// CStaticGrayValue window

class CStaticGrayValue : public CStatic
{
// Construction
public:
	CStaticGrayValue();

// Attributes
public:
	CWnd				*m_pParentWnd;
	CDlgImageInfo		*m_pDlgImgInfo;
	CDlgGrayStatistic	*m_pDlgGrayStat;

	int	m_bHaveGrayInfo;									// 是否有灰度统计信息
	int	m_bLBtnDown;

	GF_IMAGEGRAYSTATSTRUCT	m_stGrayStat;

	int	m_nGrayDataCount[256];								// 0-255灰度值的个数
	int	m_nYStart[256];										// 画直方图时纵坐标的起始点

	int m_nGrayValue1, m_nGrayValue2, m_nGrayCntInRang;		// 色阶范围（灰度值范围）、及该灰度范围的像素个数
	double m_fPercent;										// 上述灰度范围内的像素个数占总像素数的百分比

	CRect m_rtArea;

// Operations
public:
	void InitialGrayStatInfo();
	void SetStatGrayData(UCHAR* pImageData, int nw, int nh, CRect rtArea);
	void GetGrayStatInfo();
	void GetSelectGrayStatInfo();
	void DrawGrayStatData(CDC* pDC);
	void SetValueInRange(int &nValue) { if(nValue < 0) nValue = 0; if(nValue > 255) nValue = 255;}

	void SetImageStatisticInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticGrayValue)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticGrayValue();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticGrayValue)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICGRAYVALUE_H__4DA8ED79_1CC5_4A91_874C_202E8C8B7404__INCLUDED_)
