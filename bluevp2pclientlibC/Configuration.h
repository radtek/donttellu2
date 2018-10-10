#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED

int GetPrivateProfileString(const char* lpKeyName,
                            const char* lpDefault,
                            char* lpReturnedString,
                            int nSize,
                            const char* lpFileName);
int GetPrivateProfileInt(const char* lpKeyName,
                         int nDefault,
                         const char* lpFileName);

//int WritePrivateProfileString(const char* lpKeyName,
//                              const char* lpString,
//                              const char* lpFileName);
#endif // CONFIGURATION_H_INCLUDED

