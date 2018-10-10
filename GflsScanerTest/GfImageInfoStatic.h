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
	UCHAR	*m_pImageData, *m_pBadInfoData;		// ͼ�����ݺʹõ�����,�õ�����Ϊ:�����Ӧ����Ϊ�õ�,��ֵΪ0(��ɫ),����ֵΪ255(��ɫ)
	int		m_nDataSize;
	
	int		m_nAverage, m_nSmallGrayCnt;				// �ҶȾ�ֵ���Ҷ�ֵС��225�����ظ���
	int		m_nAllBad1pt, m_nAllBad2pt, m_nAllBad3pt;	// ����ͼ��Χ�ڵ�ֱ��Ϊ1,���ڵ���2,���ڵ���3�Ĵõ�ĸ���
	int		m_nCnrBad1pt, m_nCnrBad2pt, m_nCnrBad3pt;	// ���ӳ���Χ�ڵ�ֱ��Ϊ1,���ڵ���2,���ڵ���3�Ĵõ�ĸ���

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
