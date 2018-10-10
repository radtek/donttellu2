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
			ERR_PARSEREQ = -1,	//����������Ϣ����
			ERR_BUILDANS = -2,	//���췵����Ϣ����
			ERR_LOGIN = -3,	//��½�����ڴ治�㣩
			ERR_LOGOUT = -4,	//�ǳ�����δ��¼���ظ��ǳ���guid������
			ERR_MCTRLID = -5,	//���ض������֤ʧ�ܣ�δ��¼����ѯ��½��Ϣʧ�ܣ������ضˣ�
			ERR_GETDEVINFO = -6,	//��ȡ�豸�б�ʧ��
			ERR_KEEPALIVE = -7,	//��������ʧ�ܣ�δ��¼��
			ERR_UCTRLID = -8,	//���ض������֤ʧ�ܣ�δ��¼����ѯ��½��Ϣʧ�ܣ��Ǳ��ضˣ�
			ERR_CONNDEV = -9,	//Э����ʧ��
			ERR_PROXYREQ = -10,	//��������ʧ��
			ERR_PROXYANS = -11,	//����Ӧ��ʧ��
			ERR_BUILDREQ = -12,	//����������Ϣ����
			ERR_PARSEANS = -13,	//����������Ϣ����
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
