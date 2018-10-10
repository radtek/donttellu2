/*-------------------------------------------------------------------------------------------------
FILE NAME:      psfpApiClass.h
DESCRIPTION:    Declaration of psfpClipApi class.

AUTHOR:         Wang Shuguang
DATE:           Aug/07/2006
TYPE:           API, For Static Library
REVISION:       Aug/07/2006, by Wang Shuguang.
VERSION:        3.0

ALL RIGHTS RESERVED.
-------------------------------------------------------------------------------------------------*/
#pragma once
#ifndef _psfpClipApiClass_h_
#define _psfpClipApiClass_h_

#include "Config.h"

#include <memory>
#include "imgStdDataTypes.h"
#include "psfpClipApiErrors.h"

namespace psfpClipApi
{
	// modified
    using std::auto_ptr;
	using namespace imgStd;

	class PSFPDLL_API CClipApi
	{
		//  Application Program Interface
	public:
		//	Constructor and destructor
		CClipApi();
		~CClipApi();

		//	Create engine
		EPsfpClipRet Create();

		EPsfpClipRet ImgClipping(cuint8 *const pInImg, int32 nOldHeight, int32 nOldWidth, 
							 uint8 *const pOutImg, int32 nNewHeight, int32 nNewWidth);

		EPsfpClipRet GetFingerRotAgl(cuint8 *const pInImg, int32 nRow, int32 nCol, 
							 int32 nMaxRotAgl, int32 &nRealAgl);

		EPsfpClipRet GetFingerPosRotAgl(cuint8 *const pInImg, int32 nRow, int32 nCol, 
			int32 nNewRow, int32 nNewCol, int32 nMaxAngle,
			int32 &nTop, int32 &nLft, int32 &nAngle);


	public://private:
        //  Data members
        struct   SImpl;
        auto_ptr<SImpl>    m_pImpl;
	};
}

#endif