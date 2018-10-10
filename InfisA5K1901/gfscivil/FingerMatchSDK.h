/**
 * 文件名  ：	FingerMatchSDK.h
 * 公司名称：	北京东方金指科技有限公司
 * 描述    ：	定义使用的宏、结构体,声名接口
 * 版本号  ：	 6.2.1.1
 * 版权    ：	版权所有　(C)　2000－2008  北京东方金指科技有限公司
*/

#ifndef _INC_FINGERMATCHSDK_H
#define _INC_FINGERMATCHSDK_H

#ifdef FINGERMATCHSDK_EXPORTS
#define FINGERMATCHSDK_API __declspec(dllexport)
#else
#define FINGERMATCHSDK_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define  GAFIS_OPERATE_OK					0       //  操作成功

#define  GAIFS_DOG_NOTFIND                  -1      //  没有找到加密狗
#define  GAFIS_DOG_VERIFYPASSWORDFALSE	    -2      //  加密狗获取用户密码失败
#define  GAFIS_DOG_READPRODUCTFALSE         -3      //  加密狗获取产品信息失败
#define  GAFIS_DOG_VISITLICENSEFALSE        -4      //  访问许可证失败受权过期
	
#define	 GAFIS_FINGERIMG_NULL				-5		//　指纹图像数据为空
#define  GAFIS_FINGERIMG_SMALL				-6  	//　指纹图像太小
#define  GAFIS_FINGERIMG_BIG				-7      //　指纹图像太大
#define GAFIS_FINGER_NOTBMPFORMAT			-8		//  非BMP格式图像数据
#define GAFIS_FINGER_NOT24BITCOUNT			-9		//  非24位图
#define GAFIS_FINGER_COMPRESS				-10		//  图像为压缩图
#define GAFIS_FINGER_PELSSMALL				-11		//  分辨率不够
#define GAFIS_FINGER_PLANES					-12		//  目标设备的级别不为1
#define GAFIS_FINGER_BMPDATAERROR			-13		//  错误的图像数据
#define GAFIS_FINGERIMG_NONOMAL				-14		//	非768*576大小的数据

#define  GAFIS_FINGERIMG_POORQUALITY		-15		//  图像质量太差
#define	 GAFIS_FINGERPOS_ERROR				-16     //  采集到的指纹图像不居中
#define  GAFIS_FINGERIMG_WHITEIMG			-17		//  图像为空白图
#define  GAFIS_FINGERIMG_NOENOUGHMINUATIAE  -18		//  提取的指纹特征过少
#define  GAFIS_FINGERIMG_BITCOUNTERROR		-19		//	图像位数设置错误
#define	 GAFIS_OPERATE_ERROR_PARAM			-20	    //  参数设置错误

#define  GAFIS_RV_DATAERROR					-21		//	非法特征数据
#define	 GAFIS_INITIAL_CREATERROR			-22		//	生成识别引擎失败
#define  GAFIS_INITIAL_SETGETARUERROR		-23		//	获取或者设置参数失败
#define	 GAFIS_INITIAL_GETFEATURESIZEERROR  -24		//	获取特征数据最大值失败
#define	 GAFIS_RV_MALLOCERROR				-25		//	分配内存失败
#define  GAFIS_RV_SETMNTDATAERROR			-26		//	设置指纹特征失败
#define  GAFIS_RV_IOERROR					-27		//	IO操作失败
#define	 GAFIS_INITIAL_BEYONDDATE			-28		//  已经过了授权日期
#define  GAFIS_OPERATE_ROATEERROR			-29     //  旋转指纹图像180度不成功
#define FEATURE_MAXSIZE						656		//指纹特征数据最大值

typedef struct tagFingerFeatureData
{
	unsigned char	szFeatureData[FEATURE_MAXSIZE];
	int			    nDataLength;
}FINGERFEATUREDATA;


/******************************************************************************
函数名称: GAFIS_InitialSystem
函数描述: 生成识别引擎函数.调用其它接口前要先调用此函数 
返  回值: 参照返回值说明
参数说明: 无
******************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_InitialSystem();


/*************************************************************************************
函数名称: GAFIS_MatchEx
函数描述: 特征匹配函数.调用此函数前请先调用GAFIS_SetMNTFromDB进行库存特征设置
返  回值: 参照返回值说明
参数说明: 
	参数1: *pInImg					指纹图片文件数据
	参数2：*pnScore					返回的最高得分
***************************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_MatchEx(unsigned char *pInImg, 
											   int *pnScore
											  );


/***************************************************************************************
函数名称: GAFIS_SetMNTFromDB
函数描述: 库存特征设置函数
返  回值: 参照返回值说明
参数说明:
    参数1: *pFeatureOld 			指纹特征数据;
	参数2: nCount					指纹个数;
****************************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_SetMNTFromDB(FINGERFEATUREDATA *pFeatureOld,
													int nCount
													);


/*************************************************************************************
函数名称: GAFIS_ExtractMnt
函数描述: 特征提取函数.
返  回值: 参照返回值说明
参数说明: 
	参数1: *pInImg					指纹图片文件数据，格式与GAFIS_MatchEx中的pInImg一致
	参数2：*pFeature				返回的特征数据，其中的nDataLength表示了szFeatureData特征数据的长度
***************************************************************************************/
FINGERMATCHSDK_API int __stdcall GAFIS_ExtractMnt(unsigned char *pInImg, 
												  FINGERFEATUREDATA *pFeature
												  );


#ifdef __cplusplus
}
#endif

#endif