// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT


//#include "gafis7base/gafis7basedef.h"
#include "gafis7egfstd/gafis7egfstddef.h"
#include "gafis7egfstd/egfstdutil.h"

#include "gafis7loc/gafis7locdef.h"
#include "ga7locstd/ga7locstddef.h"

#include "gloclib/glocdef.h"
#include "gafis7lob/gafis7lobdef.h"
#include "gafis7loc/gafis7tp.h"

#include "gafis7cpr/gafis7cprdef.h"

#include "gutillib/gmd5.h"

//#include "gafis67/tools.h"
#include "gafis67/gafis67.h"

#include "GfpUtility/gfpubpro.h"
#include "gythCapture/gythdef.h"
#include "gaxmllib/gaxmlutl.h"
#include "gutillib/gbase64.h"

#include "gafis7xgw/gafis7xgw.h"
#include "gafis7xgw/mntconv.h"
#include "ga7match/ga7matchdef.h"

#include "ga7lsn/ga7lsndef.h"

#include "xgwAlgorithm/HandObjTypedef.h"
#include "gythCapture/gythUtility.h"

#include "utilHelper.h"

#include "infissplitboxdef.h"