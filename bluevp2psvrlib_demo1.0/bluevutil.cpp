#include "bluevp2psvrdef.h"

namespace BLUEV{
	namespace BLUEVBASE{

		#define	UTIL_ALIGN(a, at)	((((a)+(at)-1)/(at))*(at))

		// pData is 3 bytes long, and pCode will 4 bytes long.
		int UTIL_BASE64::UTIL_Encode(unsigned char *pData, char *pCode)
		{
			static	char pszBase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			if(pData == NULL || pCode == NULL)
				return -1;

			*pCode = pszBase[pData[0]>>2];
			pCode++;
			*pCode = pszBase[ ((pData[0] & 0x3)<<4) | ( pData[1]>>4) ];
			pCode++;
			*pCode = pszBase[ ((pData[1] & 0xF)<<2) | ( pData[2]>>6) ];
			pCode++;
			*pCode = pszBase[(pData[2] & 0x3F )];

			return 0;
		}

		int UTIL_BASE64::UTIL_Code2Value(char cCode)
		{
			if ( cCode>='A' && cCode<='Z' ) return	cCode-'A';
			if ( cCode>='a' && cCode<='z' ) return	cCode-'a'+26;
			if ( cCode>='0' && cCode<='9' ) return	cCode-'0'+52;
			if ( cCode=='+' ) return	62;
			if ( cCode=='/' ) return	63;
			return	-1;	// bad value.
		}

		bool UTIL_BASE64::isBase64Char(char cCode)
		{
			if((cCode>='A' && cCode<='Z') ||
				(cCode>='a' && cCode<='z') ||
				(cCode>='0' && cCode<='9') ||
				(cCode=='+' || cCode=='/'))
				return true;
			
			if(cCode == '=')
				return true;

			return	false;
		}

		// pCode is 4 bytes long, and pData is 3 bytes long data. chech pCode range out of this scope.
		int UTIL_BASE64::UTIL_Decode(const char *pCode, unsigned char *pData)
		{
			int		n0 = 0, n1 = 0, n2 = 0, n3 = 0;

			if(pCode == NULL || pData == NULL)
				return -1;

			n0 = UTIL_Code2Value(pCode[0]);
			n1 = UTIL_Code2Value(pCode[1]);
			n2 = UTIL_Code2Value(pCode[2]);
			n3 = UTIL_Code2Value(pCode[3]);
			*pData = (unsigned char)(n0<<2 | (n1 & 0x30)>>4 );
			pData++;
			*pData = (unsigned char)(((n1 & 0xF )<<4) | ((n2 & 0x3C)>>2));
			pData++;
			*pData = (unsigned char)(((n2 & 0x3)<<6) | n3);

			return 0;
		}


		int UTIL_BASE64::encode(char *pData,	/* [IN] data to be encoded*/
							   int nDataLen,	/* [IN] */
							   char **ppCode,	/* [OUT] code*/
							   int *pnCodeLen,	/* [OUT] */
							   int nOption
							  )
		{
			char			*pOut = NULL;
			unsigned char	bnBuf[3] = {0};
			int				retval = -1;
			int				i = 0, k = 0, ntemp = 0;

			if(pData == NULL || nDataLen <= 0 || ppCode == NULL || pnCodeLen == NULL)
				return retval;

			*pnCodeLen = 0;
			ntemp = nDataLen;
			ntemp = UTIL_ALIGN(ntemp, 3);
			ntemp = ntemp*4/3;	// exact # of bytes needed.
			ntemp += 100;	// extra 100 bytes.
			pOut = new char[ntemp];
			if(pOut == NULL)
				return retval;
			ntemp = (nDataLen/3)*3;
			for(i=k=0; i<ntemp; i+=3, k+=4 ) {
				UTIL_Encode((unsigned char *)pData+i, pOut+k);	// encode.
			}
			ntemp = nDataLen%3;
			if ( ntemp>0 ) {
				memset(bnBuf, 0, sizeof(bnBuf));
				memcpy(bnBuf, pData+i, ntemp);
				UTIL_Encode(bnBuf, pOut+k);
				k += 4;
				pOut[k-1] = '=';
				if ( ntemp==1 ) {
					pOut[k-2] = pOut[k-1];
				}
			}

			pOut[k] = '\0';

			*ppCode = pOut;
			*pnCodeLen = k;

			retval = k;

			if ( retval<0 ) {
				if(pOut)
					delete[] pOut;
			}
			return	retval;
		}

		// the pCode should not contain extra characters, else will return error.
		int UTIL_BASE64::decode(const char *pCode,		// IN code.
								int nCodeLen,	// including terminating '=' sign.
								char **ppData,	/* [OUT] data decoded*/
								int *pnDataLen,	/* [OUT] */
								int nOption
								)
		{
			int				retval = -1;
			int				i = 0, ntemp = 0, k = 0;
			unsigned char	*pData = NULL;
			char			szBuf[4] = {0};

			if(pCode == NULL || nCodeLen <= 0 || ppData == NULL || pnDataLen == NULL)
				return retval;

			*ppData = NULL;
			*pnDataLen = 0;
			if ( nCodeLen%4!=0 ) {
				// must be multiple of 4 bytes long.
				return retval;
			}
			ntemp = (nCodeLen/4)*3;
			pData = new unsigned char[ntemp];
			if(pData == NULL)
				return retval;
			for(i=k=0; i<nCodeLen-4; i+=4, k+=3) {
				UTIL_Decode(pCode+i, pData+k);	// 
			}
			pCode += i;
			memcpy(szBuf, pCode, 4);
			ntemp = 0;
			if ( szBuf[2]=='=' ) {
				szBuf[2] = 'A';
				ntemp++;
			}
			if ( szBuf[3]=='=' ) {
				szBuf[3] = 'A';
				ntemp++;
			}
			UTIL_Decode(szBuf, pData+k);
	
			k += 3-ntemp;
	
			*ppData = (char *)pData;
			*pnDataLen = k;
			retval = k;	// ok.

			if ( retval<0 ) {
				if(pData)
					delete[] pData;
			}
			return	retval;
		}


		int UTIL_BASE64::decodeEx(const char *pCode,		// IN code.
							   int nCodeLen,	// including terminating '=' sign.
							   char **ppData,	/* [OUT] data decoded*/
							   int *pnDataLen,	/* [OUT] */
							   int nOption
							   )
		{
			char	*pNewCode = NULL;
			int		nNewCodeLen = 0;

			if(pCode == NULL || nCodeLen <= 0 || ppData == NULL || pnDataLen == NULL)
				return -1;

			pNewCode = new char[nCodeLen];
			if(pNewCode == NULL)
				return -1;
	
			for(int i = 0; i < nCodeLen; ++i, ++pCode)
			{
				if(*pCode=='\r' || *pCode=='\n')
					continue;

				pNewCode[nNewCodeLen++] = *pCode;
			}

			nNewCodeLen = decode(pNewCode, nNewCodeLen, ppData, pnDataLen, 0);

			if(pNewCode)
				delete[] pNewCode;

			return nNewCodeLen;
		}

	}
}