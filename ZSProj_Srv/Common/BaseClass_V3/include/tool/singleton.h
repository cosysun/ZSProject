#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include "AutoLock.h"
#include "../config.h"
/*--------------------------------------------
_  
--------------------------------------------*/

GV_BEGIN


template <class T>
class BASECLASS_API Singleton
{	
public:
	//
	static T* Instance()
	{
		static T  tValue;
		return &tValue;
	};
	//
protected:
	Singleton()
	{
	}
	~Singleton()
	{
	}
private:
	Singleton& operator = (const Singleton&){};
	Singleton(const Singleton&){};

};

//

GV_END

#endif