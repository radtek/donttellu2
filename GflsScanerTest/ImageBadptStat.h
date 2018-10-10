// ImageBadptStat.h: interface for the CImageBadptStat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEBADPTSTAT_H__29E05839_33EC_4263_BD66_FD5DC4D81CD9__INCLUDED_)
#define AFX_IMAGEBADPTSTAT_H__29E05839_33EC_4263_BD66_FD5DC4D81CD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGflsScanerTestView;

class CImageBadptStat  
{
public:
	CImageBadptStat();
	virtual ~CImageBadptStat();

	UCHAR	*m_pBadInfoData;							// 疵点数据,疵点数据为:如果相应像素为疵点,则值为0(黑色),否则值为255(白色)
	int		m_nAverage, m_nSmallGrayCnt;				// 灰度均值，灰度值小于225的像素个数
	int		m_nAllBad1pt, m_nAllBad2pt, m_nAllBad3pt;	// 整个图像范围内的直径为1,大于等于2,大于等于3的疵点的个数
	int		m_nCnrBad1pt, m_nCnrBad2pt, m_nCnrBad3pt;	// 主视场范围内的直径为1,大于等于2,大于等于3的疵点的个数

	GF_BADPTDATAARRAY	m_astBadData;
	CGflsScanerTestView	*m_pTestView;

	void FreeBadDataArray();
	void ClearOldDataInBadDataArray();
	int	 AddBadPointData2Array(GF_BADPOINTDATA *pstPointData);

	void InitBadptInfoData();

	void CheckImageBadPoint();
	int	 GetBadPointRadia(GF_BADPOINTINFO &stBadInfo);
	int  IsRightPointBad(GF_BADPOINTINFO &stBadInfo);
	int  IsLeftBottomPointBad(GF_BADPOINTINFO &stBadInfo);
	int  IsBottomPointBad(GF_BADPOINTINFO &stBadInfo);
	int  IsRightBottomPointBad(GF_BADPOINTINFO &stBadInfo);
	int  AddBadPoint2Array(GF_BADPOINTINFO &stBadInfo, int x, int y);
	void GetBadPointStatInfo(TCHAR *pszInfo);

	void DrawBadPointInfo(CDC *pdc, int nw, int nh);

	int	MakeRectIncPoints(GF_BADPOINTDATA *pstPointData, CRect &rtArea);
};

#endif // !defined(AFX_IMAGEBADPTSTAT_H__29E05839_33EC_4263_BD66_FD5DC4D81CD9__INCLUDED_)
