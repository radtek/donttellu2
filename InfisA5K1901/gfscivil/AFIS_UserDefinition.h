/*******************************************************************
|	AFIS_UserDefinition.h: Header file for user definition.
|
|   Author: Wang Shuguang
|   Date:   1998.09.06
|   All Rights Reserved.
********************************************************************/

#ifndef __AFIS_UserDefinition_H__
#define __AFIS_UserDefinition_H__

#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------
//	1. Define
//-----------------------------------------------------------------------------
#define		IN					// Input argument
#define		OUT					// Output argument
#define		IO					// Input/Output argument
#define     UNKNOWN_VALUE 0xFF  // Unknown value

//-----------------------------------------------------------------------------
//	2. Constants
//-----------------------------------------------------------------------------
//	2.1 Return Value of all functions
enum AFIS_RV
{
	AFIS_RV_OK = 0,				// OK, for all stages
	
	// Quality check
	AFIS_RV_NULLIMAGE,			// Null Image
	AFIS_RV_TOOSMALLAREA,		// Small area
	AFIS_RV_POORQUALITY,		// Poor quality

	AFIS_RV_LEFTSHIFT,			// need left shift
	AFIS_RV_RIGHTSHIFT,			// need right shift
	AFIS_RV_TOPSHIFT,			// need top shift
	AFIS_RV_BOTTOMSHIFT,		// need bottom shift

	AFIS_RV_NOCORE,				// No Core
	AFIS_RV_NOTKNOWNTYPE,		// Not-known type
	AFIS_RV_NOENOUGHMINUATIAE,	// No enough minutiae

	// Register
	AFIS_RV_REGISTERSUCCESS,	// register successful
	AFIS_RV_REGISTERFAIL,		// register fail
		
	// Match
	AFIS_RV_DIFFERENTFINGER,	// different
	AFIS_RV_NOTSURE,			// not sure
	AFIS_RV_SAMEFINGER,			// same finger
	AFIS_RV_ALLMATCHED,			// all finger in a ffr have been matched
		
	// Others
	AFIS_RV_MALLOCERROR,		// Memory allocation error
	AFIS_RV_IOERROR,			// IO error
	AFIS_RV_ARGUERROR,			// Arguments error
	AFIS_RV_DATAERROR,			// Data error
	AFIS_RV_TOTAL
};

//	2.2 Data Type, represents different database or sensor
enum AFIS_DATATYPE
{
	AFIS_DT_SENSOR_OPTICAL = 0,	// Sensor type
	AFIS_DT_SENSOR_CMOS,
	AFIS_DT_SENSOR_ULTRASONIC,
	AFIS_DT_SENSOR_PAPER,

	AFIS_DT_TOTAL
};

//	2.3 Finger Position
enum AFIS_FINGER_POSITION 
{
	AFIS_FP_UNKNOWN_FINGER = 0,
	AFIS_FP_RIGHT_THUMB,
	AFIS_FP_RIGHT_INDEX,
	AFIS_FP_RIGHT_MIDDLE,
	AFIS_FP_RIGHT_RING,
	AFIS_FP_RIGHT_LITTLE,
	AFIS_FP_LEFT_THUMB,
	AFIS_FP_LEFT_INDEX,
	AFIS_FP_LEFT_MIDDLE,
	AFIS_FP_LEFT_RING,
	AFIS_FP_LEFT_LITTLE,
	AFIS_FP_PLAIN_RIGHT_THUMB,
	AFIS_FP_PLAIN_LEFT_THUMB,
	AFIS_FP_PLAIN_RIGHT_FOUR,
	AFIS_FP_PLAIN_LEFT_FOUR,

	AFIS_FP_TOTAL
};

// 2.4 Impression Type
enum AFIS_COMPANY_CODE
{
	AFIS_CP_JINZHI = 0,
	AFIS_CP_HANWANG,
	AFIS_CP_UNIC,
	
	AFIS_CP_TOTAL
};

//	2.5 Match Option
enum AFIS_MATCH_OPTION 
{
	AFIS_MO_ADAPT_TPL = 0x0001,		// Auto apdat templat according to match result
	AFIS_MO_SHOW_RST  = 0x0002,		// Will show matched minutiae pairs

	AFIS_MO_VERIFY    = 0x0008,		// Verify, only check if same finger
	AFIS_MO_ENROLL    = 0x0011,		// Template enroll, default with adapt
};

//-----------------------------------------------------------------------------
//	3. Argu Structure
//-----------------------------------------------------------------------------
//	3.1 Structure of Minutiae extraction arguments
struct AFIS_ME_Arguments
{	
	int nFingerPos;				// Finger position
	int nDataType;				// Sensor or database type
	int nCompany;				// Company code
	int nImageWidth;			// Input image width
	int nImageHeight;			// Input image height
	int nQualityControl;		// Quality control threshold 0~100
	int nMaxMinuNum;			// Maximum minutiae number, < 256
	int nMinMinuNum;			// Minimum minutiae number, > 0
};

//	3.2 Structure of fingerprint enrollment arguments
struct AFIS_ER_Arguments
{	
	int nSampleNum;				// The number of samples needed to enroll a finger
	int nThreshold;				// Threshold of cross-match in enrollment
};

//	3.3 Structure of fingerprint matching arguments
struct AFIS_FM_Arguments
{	
	int nThreshold;				// Match threshold
	int nMaxRotation;			// Maximum rotation
	int nMaxXTranslation;		// Maximum translation in X axis
	int nMaxYTranslation;		// Maximum translation in Y axis
	int nMaxMinuNum;			// Maximum minutiae number for template expand
	int nMinMinuNum;			// Minimum minutiae number for template shrink
	int nMatchOption;		    // Match option
};

//	3.4 Structure of API arguments, integration of ME, ER and FM for easier usage.
struct AFIS_API_Arguments 
{
	struct AFIS_ME_Arguments meArgu;	// Minutia extraction arguments
	struct AFIS_ER_Arguments erArgu;	// Enroll arguments
	struct AFIS_FM_Arguments fmArgu;	// Matching arguments
};

#ifdef __cplusplus
}
#endif

#endif	// End of ifndef _AFIS_UserDefinition_h_


