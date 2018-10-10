#pragma once

#include <exception>

namespace BLUEV{
	namespace BLUEVBASE{

#define ERRLOG_PATH	"c:\\temp\\errlog.dat"

	class BVBASELIB_API BluevError : public std::exception
	{
	public:
		enum
		{
			ERR_PARSEREQ = -1,	//解析请求信息错误
			ERR_BUILDANS = -2,	//构造返回信息错误
			ERR_LOGIN = -3,	//登陆错误（内存不足）
			ERR_LOGOUT = -4,	//登出错误（未登录，重复登出，guid不符）
			ERR_MCTRLID = -5,	//主控端身份验证失败（未登录，查询登陆信息失败，非主控端）
			ERR_GETDEVINFO = -6,	//获取设备列表失败
			ERR_KEEPALIVE = -7,	//保持心跳失败（未登录）
			ERR_UCTRLID = -8,	//被控端身份验证失败（未登录，查询登陆信息失败，非被控端）
			ERR_CONNDEV = -9,	//协助打洞失败
			ERR_PROXYREQ = -10,	//代理请求失败
			ERR_PROXYANS = -11,	//代理应答失败
			ERR_BUILDREQ = -12,	//构造请求信息错误
			ERR_PARSEANS = -13,	//构造请求信息错误
		};
	private:
		std::string	_strFileName;
		int			_nLine;
		std::string	_strLine;
		int			_nErrNum;

		static __int64	_sllErrCount;
	public:
		BluevError(void) : exception()
		{
			_nLine = 0;
			_sllErrCount++;
			_nErrNum = 0;
		}

		explicit BluevError(const char * const & pszErr) : exception(pszErr)
		{
			_nLine = 0;
			_sllErrCount++;
			_nErrNum = 0;
		}

		BluevError(const char * const & pszFileName, int nLine) : 
		_strFileName(pszFileName), _nLine(nLine)
		{
			char	szBuf[32] = {0};
			sprintf(szBuf, " line:%d", _nLine);
			_strLine = szBuf;
			_sllErrCount++;
			_nErrNum = 0;
		}

		BluevError(const char * const & pszFileName, int nLine, const char * const & pszErr) :
		exception(pszErr), _strFileName(pszFileName), _nLine(nLine)
		{
			char	szBuf[32] = {0};
			sprintf(szBuf, " line:%d", _nLine);
			_strLine = szBuf;
			_sllErrCount++;
			_nErrNum = 0;
		}

		BluevError(const char * const & pszFileName, int nLine, int nErrNum) :
		_strFileName(pszFileName), _nLine(nLine), _nErrNum(nErrNum)
		{
			char	szBuf[32] = {0};
			sprintf(szBuf, " line:%d", _nLine);
			_strLine = szBuf;
			_sllErrCount++;
		}

		int getLine()
		{
			return _nLine;
		}

		std::string getLineName()
		{
			return _strLine;
		}

		std::string getFileName()
		{
			return _strFileName;
		}

		__int64 getErrCount()
		{
			return _sllErrCount;
		}

		int getErrNum()
		{
			return _nErrNum;
		}

		int print(std::string strErrAppend = "")
		{
			std::string	strPrint;
			strPrint.append("\r\n");

			char	szBuf[32] = {0};
			if(_nErrNum != 0)
			{
				sprintf(szBuf, "err num:%d ", _nErrNum);
				strPrint.append(szBuf);
			}

			strPrint.append("file name:");
			strPrint.append(_strFileName);
			//strPrint.append(" line:");
			strPrint.append(_strLine);
			strPrint.append("\r\n");
			strPrint.append(this->what());
			strPrint.append("\r\n");

			if(!strErrAppend.empty())
			{
				strPrint.append(strErrAppend);
				strPrint.append("\r\n");
			}

			std::cout << strPrint;

			return 0;
		}
	};
	
	}
}
