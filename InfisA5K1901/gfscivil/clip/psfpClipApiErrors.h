/*-------------------------------------------------------------------------------------------------
FILE NAME:		psfpApiErrors.h
DESCRIPTION:	Definition of return values	of this	system.
				This file will be open to application programmers.

AUTHOR:			Wang Shuguang
DATE:			Aug/07/2006
TYPE:			API, For Static	Library
REVISION:		Aug/07/2006, by	Wang Shuguang.
VERSION:		3.0

ALL	RIGHTS RESERVED.
-------------------------------------------------------------------------------------------------*/
#pragma once
#ifndef	_psfpClipApiErrors_h_
#define	_psfpClipApiErrors_h_

#include <string>

namespace psfpClipApi
{
	// Return code of all functions
	enum EPsfpClipRet
	{
		PR_CLIP_OK = 0,				// OK, for all functions if	successful

		// Quality check
		PR_CLIP_NULL_IMAGE,			// Null	Image
		PR_CLIP_DATA_ERROR,
		PR_CLIP_TOTAL
	};

    // Error messages
    using std::string;
    extern const string g_strPsfpClipRet[PR_CLIP_TOTAL];
}

#endif