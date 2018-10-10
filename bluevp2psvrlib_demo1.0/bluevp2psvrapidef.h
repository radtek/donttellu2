#ifndef _INC_P2PSVRAPIDEF_H_201412081016_
#define _INC_P2PSVRAPIDEF_H_201412081016_


#ifdef BVP2PSVRLIB_EXPORTS
#define BVP2PSVRLIB_API __declspec(dllexport)
#define BVP2PSVRLIB_VAR __declspec(dllexport)
#else
#define BVP2PSVRLIB_API __declspec(dllimport)
#define BVP2PSVRLIB_VAR __declspec(dllimport)
#endif

#ifdef BVP2PSVRLIB_EXPORTS
#define BVNETLIB_API __declspec(dllexport)
#define BVNETLIB_VAR __declspec(dllexport)
#else
#define BVNETLIB_API __declspec(dllimport)
#define BVNETLIB_VAR __declspec(dllimport)
#endif

#ifdef BVP2PSVRLIB_EXPORTS
#define BVBASELIB_API __declspec(dllexport)
#define BVBASELIB_VAR __declspec(dllexport)
#else
#define BVBASELIB_API __declspec(dllimport)
#define BVBASELIB_VAR __declspec(dllimport)
#endif

#pragma warning(disable:4251)
#pragma warning(disable:4231)

#endif	//_INC_P2PSVRAPIDEF_H_201412081016_