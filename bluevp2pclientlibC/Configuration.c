#include "Configuration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct item_t
{
    char *key;
    char *value;
} ITEM;
//***********************
static char *strtrimr(char *pstr)
{
    int i = strlen(pstr) - 1;
    while (isspace(pstr[i]) && (i >= 0))
    {
        pstr[i--] = '\0';
    }
    return pstr;
}
static char *strtriml(char *pstr)
{
    int i = 0,j = strlen(pstr) - 1;
    while (isspace(pstr[i]) && (i <= j))
    {
        ++i;
    }
    if (0 < i)
    {
        strcpy(pstr, &pstr[i]);
    }
    return pstr;
}
static char *strtrim(char *pstr)
{
    char *p;
    p = strtrimr(pstr);
    return strtriml(p);
}
//***********************

int GetPrivateProfileString(const char* lpKeyName,
                            const char* lpDefault,
                            char* lpReturnedString,
                            int nSize,
                            const char* lpFileName)
{
    if(!lpKeyName || !lpDefault || !lpReturnedString || !lpFileName || 0 >= nSize)
    {
        return -1;
    }
    char* p = NULL;
    char* p2 = NULL;
    char buf[1024];
    FILE* f = fopen(lpFileName,"r");
    int nRet = -3;
    do
    {
        if(!f)
        {
            nRet = -2;
            break;
        }
        memset(buf,0x00,sizeof(buf));
        while(fgets(buf,sizeof(buf),f))
        {
            p = strtrim(buf);
            //len = strlen(p);
            if(!p || 0 >= strlen(p) || '#' == p[0])
            {
                continue;
            }
            p2 = strchr(p, '=');
            if(!p2)
            {
                continue;
            }
            *p2++ = '\0';
            if(0 == strncmp(p,lpKeyName,strlen(lpKeyName)))
            {
                strncpy(lpReturnedString,p2,nSize);
                nRet = 0;
                break;
            }
        }
    }
    while(0);
    if(nRet)
    {
        strncpy(lpReturnedString,lpDefault,nSize);
    }
    if(f)
    {
        fclose(f);
    }
    return 0;
}

int GetPrivateProfileInt(const char* lpKeyName,
                         int nDefault,
                         const char* lpFileName)
{
    if(!lpKeyName || !lpFileName)
    {
        return nDefault;
    }
    FILE* f = fopen(lpFileName,"r");
    if(!f)
    {
        return nDefault;
    }
    char buf[1024];
    memset(buf,0x00,sizeof(buf));
    int nRet = nDefault;
    while(fgets(buf,sizeof(buf),f))
    {
        char *p = strtrim(buf);

        if(!p || 0 >= strlen(p) || '#' == p[0])
        {
            continue;
        }
        char* p2 = strchr(p, '=');
        if(!p2)
        {
            continue;
        }
        *p2++ = '\0';
        if(0 == strncmp(p,lpKeyName,strlen(lpKeyName)))
        {
            if(0 < strlen(p2))
            {
                nRet = atoi(p2);
            }
            break;
        }
    }
    if(f)
    {
        fclose(f);
    }
    return nRet;
}

//static int WritePrivateProfileString(const char* lpKeyName,
//                              const char* lpString,
//                              const char* lpFileName)
//{
//    if(!lpKeyName || !lpString || !lpFileName)
//    {
//        return -1;
//    }
//    FILE* f = fopen(lpFileName,"r+");
//    if(!f)
//    {
//        return -2;
//    }
//    char buf[1024];
//    memset(buf,0x00,sizeof(buf));
//    int nRet = -3;
//    while(fgets(buf,sizeof(buf),f))
//    {
//        char *p = strtrim(buf);
//        if(!p || 0 >= strlen(p) || '#' == p[0])
//        {
//            continue;
//        }
//        char* p2 = strchr(p, '=');
//        if(!p2)
//        {
//            continue;
//        }
//        if(0 == strncmp(p,lpKeyName,strlen(lpKeyName)))
//        {
//            nRet = 0;
//            break;
//        }
//    }
//
//    if(f)
//    {
//        fclose(f);
//    }
//    return nRet;
//}
