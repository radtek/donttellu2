// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
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