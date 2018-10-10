
#ifndef	_INC_GBASE64_H_
#define	_INC_GBASE64_H_
#include <stdlib.h>
#include <memory.h>


#define	BASE_UTIL_ALIGN(a, at)	((((a)+(at)-1)/(at))*(at))

extern	int		BASE64_IsBase64Char(char cCode);
extern
int		BASE64_Encode(char *pData,	/* [IN] data to be encoded*/
					   int nDataLen,	/* [IN] */
					   char **ppCode,	/* [OUT] code*/
					   int *pnCodeLen,	/* [OUT] */
					   int nOption
					  );

extern
int		BASE64_Decode(const char *pCode,		// IN code.
					   int nCodeLen,	// including terminating '=' sign.
					   char **ppData,	/* [OUT] data decoded*/
					   int *pnDataLen,	/* [OUT] */
					   int nOption
					  );

extern
int		BASE64_DecodeEx(const char *pCode,		// IN code.
					   int nCodeLen,	// including terminating '=' sign.
					   char **ppData,	/* [OUT] data decoded*/
					   int *pnDataLen,	/* [OUT] */
					   int nOption
					   );


#endif// _INC_GBASE64_H_

