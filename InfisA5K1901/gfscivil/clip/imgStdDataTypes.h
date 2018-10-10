/*-------------------------------------------------------------------------------------------------
FILE NAME:		ImgStdDataTypes.h
DESCRIPTION:	Declaration of standard data types

AUTHOR:			Wang Shuguang
DATE:			Aug/07/2006
TYPE:			API, For Static	Library
REVISION:		Aug/07/2006, by	Wang Shuguang.
VERSION:		3.0

ALL	RIGHTS RESERVED.
-------------------------------------------------------------------------------------------------*/
#pragma once
#ifndef _ImgStdDataType_h_
#define _ImgStdDataType_h_

namespace imgStd
{
    // Type definitions
    typedef unsigned char		    uint8;
    typedef	  signed char		    sint8;
    typedef	         char		    int8;
    typedef unsigned short		    uint16;
    typedef	  signed short		    sint16;
    typedef	         short		    int16;
    typedef unsigned int		    uint32;
    typedef	  signed int		    sint32;
    typedef	         int		    int32;
    typedef unsigned __int64	    uint64;
    typedef   signed __int64	    sint64;

    typedef const unsigned char		cuint8;
    typedef	  const signed char		csint8;
    typedef	         const char		cint8;
    typedef const unsigned short	cuint16;
    typedef	  const signed short	csint16;
    typedef	         const short	cint16;
    typedef const unsigned int		cuint32;
    typedef	  const signed int		csint32;
    typedef	         const int		cint32;
    typedef const unsigned __int64	cuint64;
    typedef   const signed __int64	csint64;
    typedef          const __int64	cint64;
}

#endif
