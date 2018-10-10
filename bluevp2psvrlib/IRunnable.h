
#ifndef	_INC_IRunnable_h_201412230944
#define _INC_IRunnable_h_201412230944

namespace	BLUEV	{ namespace BLUEVBASE {

/**
 * @brief 线程类的接口。
 * @note 
 */
class BVBASELIB_API IRunnable
{
public:
	virtual	int		run() = 0;
	virtual ~IRunnable(){};
};


}}

#endif	// _INC_IRunnable_h_201412230944
