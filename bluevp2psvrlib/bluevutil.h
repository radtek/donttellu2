#ifndef	_INC_GADATCNV_H_20141201819_
#define	_INC_GADATCNV_H_20141201819_

#include <objbase.h>
#include <string>
#include <IPTypes.h>
#include <IPHlpApi.h>

namespace BLUEV
{
	namespace BLUEVBASE
	{
		using std::string;

		class BVBASELIB_API UTIL_CONV
		{
		public:
			//错误用法:
			//int nRet = Char2To_uint2(xx);
			//当结果为负数时，nRet只会得到正数。由ushort提升为int
			//正确用法:
			//int nRet = (short)Char2To_uint2(xx);
			//或使用Char2To_int2
			static unsigned short Char2To_uint2(const UCHAR *ch)
			{
				return	(unsigned short)(ch[0]*256+ch[1]);
			}

			static void	uint2ToChar2(unsigned short w, unsigned char *ch)
			{
				ch[0] = (unsigned char)(w/256);
				ch[1] = (unsigned char)(w & (0xFF));
			}

			//考虑到负数时，ushort提升为int的情况
			//提供以下函数来避免。
			static void	int2ToChar2(short w, unsigned char *ch)
			{
				ch[0] = (unsigned char)(w >> 8);	//不同于/256，移位不同于带符号除法
				ch[1] = (unsigned char)(w & (0xFF));
			}

			static short Char2To_int2(const UCHAR *ch)
			{
				return	(short)(ch[0]*256+ch[1]);
			}

			static unsigned int Char4To_uint4(const UCHAR *ch)
			{
				return	(unsigned int)(ch[0]*256*256*256+ch[1]*256*256+ch[2]*256+ch[3]);
			}

			static void	uint4ToChar4(unsigned int dw, unsigned char *ch)
			{
				ch[0] = (unsigned char)(dw/(256*256*256));
				ch[1] = (unsigned char)((dw/(256*256)) & (0xFF));
				ch[2] = (unsigned char)((dw/256) & (0xFF));
				ch[3] = (unsigned char)(dw & (0xFF));
			}
		};
		
		class BVBASELIB_API	UTIL_GUID
		{
		public:
			static int WIN_GUID_Gen(GUID &stguid)
			{
				HRESULT	h = CoCreateGuid(&stguid);
				
				if(FAILED(h)) 
					return -1;

				return 0;
			}

			static bool GUID_IsEmpty(const GUID &stguid)
			{
				GUID	temp;
				memset(&temp, 0, sizeof(temp));

				return	memcmp(&temp, &stguid, sizeof(GUID)) == 0 ? true : false;
			}

			static bool GUID_IsEqual(const GUID &stguid, const GUID &stguid2)
			{
				return	memcmp(&stguid, &stguid2, sizeof(GUID)) == 0 ? true : false;
			}
			
			static int GUID_ToString(const GUID &stguid, std::string &strGuid)
			{
				/*char	szBuf[100] = {0};
				HRESULT	h = StringFromCLSID(stguid, (LPOLESTR *)szBuf);

				if(FAILED(h)) 
				return -1;
				
				strGuid = szBuf;

				return 0;*/
				char buf[100] = {0};

				_snprintf(
					buf,
					sizeof(buf),
					"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
					stguid.Data1, stguid.Data2, stguid.Data3,
					stguid.Data4[0], stguid.Data4[1],
					stguid.Data4[2], stguid.Data4[3],
					stguid.Data4[4], stguid.Data4[5],
					stguid.Data4[6], stguid.Data4[7]);

				strGuid = buf;
				
				return 0;
			}

			static int GUID_FromString(const std::string &strGuid, GUID &stguid)
			{
				HRESULT	h = CLSIDFromString((LPOLESTR)strGuid.c_str(), &stguid);

				if(FAILED(h)) 
					return -1;

				return 0;
			}
		};
		
		class BVBASELIB_API AdapterInfo
		{
		public:
			std::string strIP;
			std::string strMask;
			std::string	strMac;
			std::string	strDesc;
			std::string	strName;
			std::string	strGateWayIP;
			std::string	strGateWayMask;
		};

		class BVBASELIB_API UTIL_NET
		{
		public:
			static int GetMacAddress(std::vector<AdapterInfo> &vctNet)
			{
				IP_ADAPTER_INFO	*pstInfo = NULL, *pstTemp = NULL;
				IP_ADAPTER_INFO	stInfo = {0};
				ULONG			uSize = 0;

				uSize = sizeof(stInfo);
				pstInfo = &stInfo;
				
				DWORD	nret = GetAdaptersInfo(pstInfo, &uSize);
				if(nret == ERROR_BUFFER_OVERFLOW) 
				{
					// need alloc memory.
					pstInfo = (IP_ADAPTER_INFO *)new char[uSize];
					if(pstInfo == NULL)
						return -1;

					// get info. too.
					nret = GetAdaptersInfo(pstInfo, &uSize);
				}

				if(nret == ERROR_NO_DATA) 
				{
					// no data
					if(pstInfo != &stInfo)
					{
						delete[] pstInfo;
						pstInfo = NULL;
					}
					return -1;
				}

				if(nret != ERROR_SUCCESS)
				{
					// error occurred.
					if(pstInfo != &stInfo)
					{
						delete[] pstInfo;
						pstInfo = NULL;
					}
					return -1;
				}

				vctNet.clear();

				// ok got all.
				pstTemp = pstInfo;
				while(pstTemp)
				{
					// get mac address.
					char	szBuf[100] = {0};
					for(int i = 0, n = 0; i < (int)pstTemp->AddressLength; i++)
					{
						if(i > 0)
							szBuf[n++] = '-';
						
						sprintf(szBuf + n, "%02X", pstTemp->Address[i]);
						n += 2;
					}
					
					AdapterInfo		net;
					net.strMac = szBuf;
					net.strIP = pstTemp->IpAddressList.IpAddress.String;
					net.strMask = pstTemp->IpAddressList.IpMask.String;
					net.strDesc = pstTemp->Description;
					net.strName = pstTemp->AdapterName;

					net.strGateWayIP = pstTemp->GatewayList.IpAddress.String;
					net.strGateWayMask = pstTemp->GatewayList.IpMask.String;
					
					vctNet.push_back(net);

					pstTemp = pstTemp->Next;
				}
				
				if(pstInfo != &stInfo)
				{
					delete[] pstInfo;
					pstInfo = NULL;
				}

				return	0;
			}
		};
		
		class BVBASELIB_API UTIL_TICK
		{
		public:
			static __int64 getTickCount()
			{
				LARGE_INTEGER	tl, nl;

				if ( QueryPerformanceFrequency(&tl) && QueryPerformanceCounter(&nl) )
				{
					return nl.QuadPart;
				}

				//return GetTickCount64();
				return GetTickCount();
			}

			static double tickToSecond(__int64 llTimeUsed)
			{
				double	fTime;
				LARGE_INTEGER	tl, nl;
				if(QueryPerformanceFrequency(&tl) && QueryPerformanceCounter(&nl))
					fTime = (double)tl.QuadPart;
				else
					fTime = CLOCKS_PER_SEC;

				return	llTimeUsed / fTime;
			}

		};

		template<typename T>
		class BVBASELIB_API DataQue
		{
		public:
			std::deque<T *>	queData;

			HANDLE	hAddDataEvent;	// que event.
			HANDLE	hGetDataEvent;

			bool	bNoWait;
			bool	bStop;
			int		nTotalItemCnt;	//最大个数default is 100

			CRITICAL_SECTION	stCrit;

			DataQue() throw(...)
			{
				if(0 > init())
					throw BluevError(__FILE__, __LINE__);
			}

			~DataQue()
			{
				unInit();
			}

			int init()
			{
				InitializeCriticalSection(&stCrit);
				hAddDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if(hAddDataEvent == NULL)
					return -1;
				
				hGetDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if(hGetDataEvent == NULL)
					return -1;

				nTotalItemCnt = 100;
				bNoWait = false;
				bStop = false;

				return 0;
			}

			int unInit()
			{
				stop();

				//清理内存队列
				EnterCriticalSection(&stCrit);

				std::deque<T *>::const_iterator itr;
				for(itr = queData.begin();
					itr != queData.end();
					itr++
					)
				{
					if(*itr)
						delete *itr;
				}

				queData.clear();

				LeaveCriticalSection(&stCrit);

				DeleteCriticalSection(&stCrit);

				return 0;
			}

			int stop()
			{
				bStop = true;

				if(hGetDataEvent)
				{
					//唤醒
					SetEvent(hGetDataEvent);
					CloseHandle(hGetDataEvent);
					hGetDataEvent = NULL;
				}
				if(hAddDataEvent)
				{
					//唤醒
					SetEvent(hAddDataEvent);
					CloseHandle(hAddDataEvent);
					hAddDataEvent = NULL;
				}

				return 0;
			}

			int addData(const T *pData)
			{
				bool	bNeedWait = false;

				do
				{
					if(bNeedWait)
					{
						WaitForSingleObject(hGetDataEvent, 1000);//不能是INFINITE
						if(bStop)
							return -1;

						bNeedWait = false;
					}

					EnterCriticalSection(&stCrit);
					if((int)queData.size() >= nTotalItemCnt)
					{
						bNeedWait = true;
						LeaveCriticalSection(&stCrit);
					}

				}while(bNeedWait);

				queData.push_back((T *)pData);
				
				LeaveCriticalSection(&stCrit);

				SetEvent(hAddDataEvent);

				return 0;
			}

			int getData(T * &pData)
			{
				bool	bNeedWait = false;

				do
				{
					if(bNeedWait)
					{
						if(bNoWait)
							return -1;

						WaitForSingleObject(hAddDataEvent, 1000);//must not be infinite
						if(bStop)
							return -1;

						bNeedWait = false;
					}

					EnterCriticalSection(&stCrit);

					if(queData.empty())
					{
						bNeedWait = true;

						LeaveCriticalSection(&stCrit);
						if(bNoWait)
							return -1;
					}

				}while(bNeedWait);

				pData = queData.front();
				queData.pop_front();
				
				LeaveCriticalSection(&stCrit);

				SetEvent(hGetDataEvent);

				return 0;
			}
		};
		
		class BVBASELIB_API UTIL_BASE64
		{
		public:
			static bool isBase64Char(char cCode);

			//ppCode返回内部new的内存地址，需要外部主动delete[] 释放
			static int encode(char *pData,	/* [IN] data to be encoded*/
				int nDataLen,	/* [IN] */
				char **ppCode,	/* [OUT] code*/
				int *pnCodeLen,	/* [OUT] */
				int nOption
				);

			//ppData返回内部new的内存地址，需要外部主动delete[] 释放
			static int decodeEx(const char *pCode,		// IN code.
				int nCodeLen,	// including terminating '=' sign.
				char **ppData,	/* [OUT] data decoded*/
				int *pnDataLen,	/* [OUT] */
				int nOption
				);
		protected:
			static int decode(const char *pCode,		// IN code.
				int nCodeLen,	// including terminating '=' sign.
				char **ppData,	/* [OUT] data decoded*/
				int *pnDataLen,	/* [OUT] */
				int nOption
				);
		private:
			static int UTIL_Encode(unsigned char *pData, char *pCode);
			static int UTIL_Code2Value(char cCode);
			static int UTIL_Decode(const char *pCode, unsigned char *pData);
		};

		//using BLUEVBASE::RWLocker;

		template<typename T>
		class BVBASELIB_API MemPool
		{
		private:
			char				*_pMem;
			int					_nMemLen;

			CRITICAL_SECTION	_stCrit;
			//RWLocker			_lockRW;

			T					*_pObj;
		public:
			MemPool()
			{
				_pMem = NULL;
				_nMemLen = 0;
				_pObj = NULL;

				InitializeCriticalSection(&_stCrit);
			}

			~MemPool()
			{
				releaseAll();
				freeObj();

				DeleteCriticalSection(&_stCrit);
			}

			int alloc(int nObjNum) throw()
			{
				if(_pMem || nObjNum <= 0)
					return -1;

				int nMemLen = nObjNum * sizeof(T);
				try
				{
					_pMem = new char[nMemLen];
					if(_pMem == NULL)
						throw BluevError(__FILE__, __LINE__);

					memset(_pMem, 0, nMemLen);

					_nMemLen = nMemLen;

					_pObj = reinterpret_cast<T *>(_pMem);
				}
				catch(BluevError &e)
				{
					e.print();
					return -1;
				}
				
				return 0;
			}

			int freeObj()
			{
				if(_pMem)
				{
					delete[] _pMem;
					_pMem = 0;
					_nMemLen = 0;

					_pObj = NULL;
				}

				return 0;
			}

			int getObj(T * & pObj) throw()
			{
				LockerEx lock(_stCrit);

				if(_pObj == NULL || _pMem == NULL)
					return -1;

				try
				{
					/*if(typeid(pObj) != typeid(Mem *))
					return -1;*/

					//是否Mem的派生类, pObj为空时无效
					//if(!dynamic_cast<Mem *>(pObj))
					//	throw BluevError(__FILE__, __LINE__);

					//travel to search
					bool	bGot = false;
					for(unsigned int i = 0; i < _nMemLen / sizeof(T); ++i)
					{
						pObj = _pObj + i;

						if(pObj->isUsed())
							continue;

						try
						{
#ifdef BVP2PSVRLIB_EXPORTS

//这里不检查内存泄漏，因此先undef。再恢复。
#ifdef new
	#undef new
							pObj = new(pObj)T();
	#ifdef _DEBUG
		#define new DEBUG_CLIENTBLOCK
	#endif

#else
							pObj = new(pObj)T();
#endif

#else
							pObj = new(pObj)T();
#endif
							if(pObj == NULL)
								throw BluevError(__FILE__, __LINE__);
						
							//pObj->bUsed = true;
							bGot = true;
							break;
						}
						catch(BluevError &e)
						{
							e.print();
							continue;
						}
					}
				
					//all is used
					if(!bGot)
					{
						pObj = NULL;
						throw BluevError(__FILE__, __LINE__);
					}
				}
				catch(BluevError &e)
				{
					e.print();
					return -1;
				}
				
				return 0;
			}

			int releaseObj(T * const &pObj) throw()
			{
				LockerEx lock(_stCrit);

				if(pObj == NULL || _pObj == NULL || _pMem == NULL)
					return -1;

				if(pObj < reinterpret_cast<T *>(_pMem) || pObj > reinterpret_cast<T *>(_pMem + _nMemLen - sizeof(T)))
					return -1;

				try
				{
					/*if(typeid(pObj) != typeid(Mem *))
						return -1;*/
				
					//是否Mem的派生类,pObj为空时无效
					//if(!dynamic_cast<Mem *>(pObj))
					//	throw BluevError(__FILE__, __LINE__);

					pObj->~T();
					memset(pObj, 0, sizeof(T));
				}
				catch(BluevError &e)
				{
					e.print();
					return -1;
				}

				return 0;
			}

			int releaseAll() throw()
			{
				LockerEx lock(_stCrit);

				if(_pObj == NULL || _pMem == NULL)
					return -1;

				try
				{
					T	*pObj = NULL;
					//travel to search
					for(unsigned int i = 0; i < _nMemLen / sizeof(T); ++i)
					{
						pObj = _pObj + i;

						if(!pObj->isUsed())
							continue;

						pObj->~T();
					}
				}
				catch(BluevError &e)
				{
					e.print();
					return -1;
				}

				memset(_pMem, 0, _nMemLen);

				return 0;
			}
		};
	}
}


#endif	//_INC_GADATCNV_H_20141201819_