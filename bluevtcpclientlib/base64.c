#include "base64.h"
// pData is 3 bytes long, and pCode will 4 bytes long.
void	BASE64_UTIL_Encode(char *pData, char *pCode)
{
	static	char pszBase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	*pCode = pszBase[pData[0]>>2];
	pCode++;
	*pCode = pszBase[ ((pData[0] & 0x3)<<4) | ( pData[1]>>4) ];
	pCode++;
	*pCode = pszBase[ ((pData[1] & 0xF)<<2) | ( pData[2]>>6) ];
	pCode++;
	*pCode = pszBase[(pData[2] & 0x3F )];
}

int		BASE64_UTIL_Code2Value(char cCode)
{
	if ( cCode>='A' && cCode<='Z' ) return	cCode-'A';
	if ( cCode>='a' && cCode<='z' ) return	cCode-'a'+26;
	if ( cCode>='0' && cCode<='9' ) return	cCode-'0'+52;
	if ( cCode=='+' ) return	62;
	if ( cCode=='/' ) return	63;
	return	-1;	// bad value.
}

int		BASE64_IsBase64Char(char cCode)
{
	if ( (cCode>='A' && cCode<='Z') ||
		 (cCode>='a' && cCode<='z') ||
		 (cCode>='0' && cCode<='9') ||
		 (cCode=='+' || cCode=='/') ) return	1;
	if ( cCode=='=' ) return 1;
	return	0;
}

// pCode is 4 bytes long, and pData is 3 bytes long data. chech pCode range out of this scope.
void	BASE64_UTIL_Decode(const char *pCode, char *pData)
{
	int		n0, n1, n2, n3;

	n0 = BASE64_UTIL_Code2Value(pCode[0]);
	n1 = BASE64_UTIL_Code2Value(pCode[1]);
	n2 = BASE64_UTIL_Code2Value(pCode[2]);
	n3 = BASE64_UTIL_Code2Value(pCode[3]);
	*pData = (n0<<2 | (n1 & 0x30)>>4 );
	pData++;
	*pData = (((n1 & 0xF )<<4) | ((n2 & 0x3C)>>2));
	pData++;
	*pData = (((n2 & 0x3)<<6) | n3);
}


int		BASE64_Encode(char *pData,	/* [IN] data to be encoded*/
					   int nDataLen,	/* [IN] */
					   char **ppCode,	/* [OUT] code*/
					   int *pnCodeLen,	/* [OUT] */
					   int nOption
					  )
{
	char	*pOut;
	char	bnBuf[3];
	int		retval;
	int		i, k, ntemp;

	retval = -1;
	pOut = NULL;
	*pnCodeLen = 0;
	if ( nDataLen<=0 ) return	0;	// nothing to encode.
	ntemp = nDataLen;
	ntemp = BASE_UTIL_ALIGN(ntemp, 3);
	ntemp = ntemp*4/3;	// exact # of bytes needed.
	ntemp += 100;	// extra 100 bytes.
	pOut = (char *)malloc(ntemp);
	if(pOut == NULL)
		return -1;
	memset(pOut, 0, ntemp);
	ntemp = (nDataLen/3)*3;
	for(i=k=0; i<ntemp; i+=3, k+=4 ) {
		BASE64_UTIL_Encode(pData+i, pOut+k);	// encode.
	}
	ntemp = nDataLen%3;
	if ( ntemp>0 ) {
		memset(bnBuf, 0, sizeof(bnBuf));
		memcpy(bnBuf, pData+i, ntemp);
		BASE64_UTIL_Encode(bnBuf, pOut+k);
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

	return	retval;
}

// the pCode should not contain extra characters, else will return error.
int		BASE64_Decode(const char *pCode,		// IN code.
					   int nCodeLen,	// including terminating '=' sign.
					   char **ppData,	/* [OUT] data decoded*/
					   int *pnDataLen,	/* [OUT] */
					   int nOption
					  )
{
	int		retval;
	int		i, ntemp, k;
	char	*pData;
	char	szBuf[4];

	retval = -1;
	*ppData = NULL;
	*pnDataLen = 0;
	pData = NULL;
	if ( nCodeLen<=0 ) return	0;
	if ( nCodeLen%4!=0 ) {
		// must be multiple of 4 bytes long

		return retval;
	}
	ntemp = (nCodeLen/4)*3;
	pData = (char *)malloc(ntemp);
	if(pData == NULL)
		return retval;
	memset(pData, 0, ntemp);
	for(i=k=0; i<nCodeLen-4; i+=4, k+=3) {
		BASE64_UTIL_Decode(pCode+i, pData+k);	//
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
	BASE64_UTIL_Decode(szBuf, pData+k);

	k += 3-ntemp;

	*ppData = pData;
	*pnDataLen = k;
	retval = k;	// ok.

	if ( retval<0 ) {
		if(pData)
		{
			free(pData);
			pData = NULL;
		}
	}
	return	retval;
}


int		BASE64_DecodeEx(const char *pCode,		// IN code.
					   int nCodeLen,	// including terminating '=' sign.
					   char **ppData,	/* [OUT] data decoded*/
					   int *pnDataLen,	/* [OUT] */
					   int nOption
					   )
{
	char* pNewCode = NULL;

	int	i, nNewCodeLen = 0;

	if(pCode==NULL || nCodeLen<1)
	{
		return -1;
	}

	pNewCode = (char *)malloc(nCodeLen);
	if(pNewCode == NULL)
		return -1;

	memset(pNewCode, 0, nCodeLen);

	for(i = 0; i < nCodeLen; ++i, ++pCode)
	{
		if(*pCode=='\r' || *pCode=='\n')	continue;
		pNewCode[nNewCodeLen++] = *pCode;
	}

	nNewCodeLen = BASE64_Decode(pNewCode, nNewCodeLen, ppData, pnDataLen, 0);

	if(pNewCode);
	{
		free(pNewCode);
		pNewCode = NULL;
	}

	return nNewCodeLen;
}


