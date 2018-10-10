/**
 * gfsmatch.h : header file for civil match dll
 * Created by beagle on Nov. 24, 2008
 * Copyright(c) EGFS 1998-2008
 */


#ifndef _INC_GFSMATCH_H
#define _INC_GFSMATCH_H

#ifdef GFSMATCH_EXPORTS
#define GFSMATCHSDK_API __declspec(dllexport)
#else
#define GFSMATCHSDK_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define  GFSMATCHSDK_OPERATE_OK						0       //  successful

#define GFSMATCHSDK_ERROR_NOTINIT					-1		// not initial
#define	GFSMATCHSDK_ERROR_PARAM						-2		//  invalid parameter

#define GFSMATCHSDK_DOG_NOTFIND						-10      // not find the Dongle
#define GFSMATCHSDK_DOG_VERIFYPASSWORDFALSE			-20      // get password failed
#define GFSMATCHSDK_DOG_READPRODUCTFALSE			-30      // get product info failed
#define GFSMATCHSDK_DOG_VISITLICENSEFALSE			-40      // license expired
	
#define	GFSMATCHSDK_FINGERIMG_NULL					-101	//¡¡the image data is null
#define GFSMATCHSDK_FINGERIMG_SMALL					-102	//¡¡the image data is too small
#define GFSMATCHSDK_FINGERIMG_BIG					-103    //¡¡the image data is too big
#define GFSMATCHSDK_FINGERIMG_DATAERROR				-104	//  error image data

#define GFSMATCHSDK_FINGEREXTRACT_POORQUALITY		-201	//  poor quality of the image data
#define	GFSMATCHSDK_FINGEREXTRACT_POSERROR			-202    //  the finger data is not in the middle of the image
#define GFSMATCHSDK_FINGEREXTRACT_WHITEIMG			-203	//  a blank image
#define GFSMATCHSDK_FINGEREXTRACT_NOENOUGHMINUTIAE	-204	//  not enough minutiae
#define GFSMATCHSDK_FINGEREXTRACT_MORE_DATA			-205	//  the buffer is too small

#define GFSMATCHSDK_RV_DATAERROR					-301	//	invalid feature data
#define	GFSMATCHSDK_INITIAL_CREATERROR				-302	//	create match engine failed 
#define GFSMATCHSDK_INITIAL_SETGETARUERROR			-303	//	get/set argument failed
#define	GFSMATCHSDK_INITIAL_GETFEATURESIZEERROR		-304	//	get the max size of feature data failed
#define	GFSMATCHSDK_RV_MALLOCERROR					-305	//	alloc memory failed
#define GFSMATCHSDK_RV_IOERROR						-306	//	IO Operation failed


/**
 * GFSMatch_Startup
 * Description: initiates use of this dll by a process or dll, must be called first
 * Parameters: null
 * Return Value: GAFIS_OPERATE_OK - successful; otherwise failed;
 */
GFSMATCHSDK_API int __stdcall GFSMatch_Startup();

/**
 * GFSMatch_Cleanup
 * Description: terminates use of this dll, When it has completed the use of this dll, the application or DLL must call GFSMatch_Cleanup 
 * Parameters: null
 * Return Value: GAFIS_OPERATE_OK - successful; otherwise failed;
 */
GFSMATCHSDK_API int __stdcall GFSMatch_Cleanup();

/**
 * GFSMatch_Extrace
 * Description: Extract feature from image data
 * Parameters:
 *   pImageData[in]: Image data for feature extracting, it must be a grayscale(256 gray levels) image and with 500ppi resolution
 *   nWidth[in]:	 The width of the image data, must be no more than 512 pixels
 *   nHeight[in]:	 The height of the image data, must be no more than 512 pixels
 *   pFeature[in,out]:	Pointer to a buffer for extracted features
 *   pnSize[in, out]:	Pointer to a variable that specifies the size of the buffer pointed to by the pFeature parameter, in bytes.
 *                      When the function returns, this variable contains the size of the data saved to pFeature. 
 *                      If the buffer specified by pFeature parameter is not large enough to hold the data, 
 *                      the function returns GFSMATCHSDK_FINGEREXTRACT_MORE_DATA and stores the required buffer size 
 *                      in the variable pointed to by pnSize.
 *                      In general, the buffer specified by pFeature should more than 1k size
 * Return Value: GAFIS_OPERATE_OK - successful; otherwise failed;
 */
GFSMATCHSDK_API int __stdcall GFSMatch_Extract(
											   unsigned char *pImageData, 
											   int nWidth, 
											   int nHeight, 
											   unsigned char *pFeature,
											   int *pnSize
											  );


/**
 * GFSMatch_Match
 * Description: match between two features
 * Parameters:
 *   pSrcFeature[in]: The source feature info for match
 *   pDesFeature[in]: The destination feature info for match
 *   pnScore[out]:	  The score after matched
 * Return Value: GAFIS_OPERATE_OK - successful; otherwise failed;
 */
GFSMATCHSDK_API int __stdcall GFSMatch_Match(
											 unsigned char *pSrcFeature,
											 unsigned char *pDesFeature,
											 int *pnScore
											);


#ifdef __cplusplus
}
#endif

#endif	// _INC_GFSMATCH_H