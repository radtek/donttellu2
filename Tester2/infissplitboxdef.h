#pragma once

#ifdef	INFIS_SPLITBOX_IMPL
#define	INFIS_SPLITBOX_DECL	__declspec(dllexport)
#else
#define INFIS_SPLITBOX_DECL	__declspec(dllimport)
#endif

typedef enum CaptureType
{
	CAPTURE_TYPE_FLATFINGER = 0,	//one flat finger(not support)
	CAPTURE_TYPE_RFOUR = 1,			//right four fingers
	CAPTURE_TYPE_LFOUR = 2,			//left four fingers
	CAPTURE_TYPE_RLTHUMB = 3,		//right and left thumb
} CaptureType;

typedef enum ImageType
{
	IMAGE_TYPE_BMP = 0,		//bmp
	IMAGE_TYPE_WSQ = 1,		//wsq, compress ratio:10
	
} ImageType;

/*
	param:
		nCaptureType	type of captured image(look the definition of enum CaptureType)
		imagebuff		point to the image to be splited
		nImgWidth		width of the image to be splited
		nImgHeight		height of the image to be splited
		pWnd			point to the parent window(CWnd)
		nImageType		compress type of image to store(look the definition of enum ImageType)

	retval:	
		0		success
		-1		fail
		-5		no license
		-100	user clicked cancel button
*/
extern "C"
INFIS_SPLITBOX_DECL int INFIS_CutImage(int nCaptureType, unsigned char* imagebuff, int nImgWidth, int nImgHeight, void *pWnd, int nImageType);

extern "C"
INFIS_SPLITBOX_DECL int INFIS_SaveDataAsBMP(char *filename, unsigned char *imagebuff, int nImgWidth, int nImgHeight, int nBit);

/*
	notice: 
		Should call the "INFIS_FreeImageWSQ" to free the buffer(*ppDest), in case of the difference of CRT LIB between two MFC dll
*/
extern "C"
INFIS_SPLITBOX_DECL int INFIS_CompressByWSQ(unsigned char *pSrc, int nw, int nh, int nCprRatio, unsigned char **ppDest, int *pDataLen);

extern "C"
INFIS_SPLITBOX_DECL void INFIS_FreeImageWSQ(unsigned char *pWSQ);