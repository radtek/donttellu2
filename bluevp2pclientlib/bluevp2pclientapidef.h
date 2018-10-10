#ifndef _INC_P2PCLIENTAPIDEF_H_201412121537_
#define _INC_P2PCLIENTAPIDEF_H_201412121537_


#ifdef BVP2PCLIENTLIB_EXPORTS
#define BVP2PCLIENTLIB_API __declspec(dllexport)
#define BVP2PCLIENTLIB_VAR __declspec(dllexport)
#else
#define BVP2PCLIENTLIB_API __declspec(dllimport)
#define BVP2PCLIENTLIB_VAR __declspec(dllimport)
#endif

#pragma warning(disable:4251)
#pragma warning(disable:4231)

#endif	//_INC_P2PCLIENTAPIDEF_H_201412121537_