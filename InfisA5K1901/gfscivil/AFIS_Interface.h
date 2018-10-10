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
// 生成识别引擎
	AFIS_RV SetArgu(AFIS_API_Arguments argu);
// 设置参数，参数定义参见userdefinition.h	
	
	AFIS_RV GetArgu(AFIS_API_Arguments &argu);
// 获取参数，设置参数之前应该先调用此函数获取当前参数	
	int		GetMaxFeatureSize();
// 获取最大特征大小，在提取特征前需要分配的最大空间
	int		GetFtrSize();
// 获取当前特征大小，提取特征后可根据此特征大小写入特征文件
	int		GetRegSampleNum();
// 获取注册指纹数
	int		GetFingerQlt();
// 获取当前指纹质量，提取特征后调用该函数获取当前指纹图像质量，0～99，数值越大越好
	AFIS_RV Extract(unsigned char *pInImg, unsigned char *pFeatureData);
// 提取特征函数，提取之前需保证已生成特征提取引擎
	AFIS_RV Register(unsigned char *pMultiFeatureData);
	AFIS_RV Match(unsigned char *pFeatureOld, unsigned char *pFeatureNew, int *nGrade);
// 特征匹配函数，匹配之前需保证已生成特征匹配引擎	
};

#endif

