/*---------------------------------------------------------------------

  File Name:	AFIS_Interface.h
  Description:	Definition of CAFIS_Interface.

  Author:		Wang Shuguang
  Date:			10/03/2001
  Type:			Civil verification usage
  Revised:		06-27-2002 by WangShuguang
  Version:		3.0

  All rights reserved.

---------------------------------------------------------------------*/
#ifndef _AFIS_Interface_H_
#define _AFIS_Interface_H_
#include "AFIS_UserDefinition.h"

#ifdef AFIS_DLL_EXPORTS
#define AFIS_DLL_API __declspec(dllexport)
#else
#define AFIS_DLL_API __declspec(dllimport)
#endif

/*********************************************************************
//						 Interface Class		                    //
/********************************************************************/
class CAFIS_MinutiaExtraction;
class CAFIS_MinutiaMatch;
struct AFIS_FH;
class AFIS_DLL_API CAFIS_Interface {
public:
	CAFIS_MinutiaExtraction   *m_pExtraction;
	CAFIS_MinutiaMatch        *m_pMatch;
private:
	AFIS_API_Arguments		  m_stArgu;
	int						  m_nFtrSize;
	int						  m_nFingerQlt;

	AFIS_RV SetDefaultArgu();
	AFIS_RV SetMeArgu(AFIS_ME_Arguments argu);
	AFIS_RV SetErArgu(AFIS_ER_Arguments argu);
	AFIS_RV SetFmArgu(AFIS_FM_Arguments argu);
	AFIS_RV FMREnCode(unsigned char* pFMRData, AFIS_FH& FeatureData);
	AFIS_RV FMRDeCode(AFIS_FH& FeatureData, unsigned char* pFMRData);

public:
	CAFIS_Interface();
	~CAFIS_Interface();

	//  Application Program Interface
	AFIS_RV Create(bool bIfCreateExtractEngine = true,
		       bool bIfCreateMatchEngine = true);
// ����ʶ������
	AFIS_RV SetArgu(AFIS_API_Arguments argu);
// ���ò�������������μ�userdefinition.h	
	
	AFIS_RV GetArgu(AFIS_API_Arguments &argu);
// ��ȡ���������ò���֮ǰӦ���ȵ��ô˺�����ȡ��ǰ����	
	int		GetMaxFeatureSize();
// ��ȡ���������С������ȡ����ǰ��Ҫ��������ռ�
	int		GetFtrSize();
// ��ȡ��ǰ������С����ȡ������ɸ��ݴ�������Сд�������ļ�
	int		GetRegSampleNum();
// ��ȡע��ָ����
	int		GetFingerQlt();
// ��ȡ��ǰָ����������ȡ��������øú�����ȡ��ǰָ��ͼ��������0��99����ֵԽ��Խ��
	AFIS_RV Extract(unsigned char *pInImg, unsigned char *pFeatureData);
// ��ȡ������������ȡ֮ǰ�豣֤������������ȡ����
	AFIS_RV Register(unsigned char *pMultiFeatureData);
	AFIS_RV Match(unsigned char *pFeatureOld, unsigned char *pFeatureNew, int *nGrade);
// ����ƥ�亯����ƥ��֮ǰ�豣֤����������ƥ������	
};

#endif

