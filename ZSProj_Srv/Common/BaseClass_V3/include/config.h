#ifndef __CONFIG_H__ 
#define __CONFIG_H__

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include <malloc.h>
#include <vector>
#include <list>
#include <sys/stat.h>  
#include <map>


using namespace std;

//
#ifdef WIN32
   #include <winsock2.h>
   #include <process.h>
#endif

#ifdef LINUX
    #include <sys/types.h>
    #include <sys/stat.h>    
    #include <sys/types.h>
    #include <sys/sysinfo.h>
    #include <sys/stat.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <sys/ioctl.h>
    #include <sys/epoll.h>
    #include <pthread.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <semaphore.h>
    #include<netdb.h>
    #include <error.h>
    #include <fcntl.h>
#endif

//
#ifdef WIN32
     #pragma comment(lib,"Ws2_32")     
#endif

//
#ifdef _USEDLL
     
     #ifdef BASECLASS_EXPORTS
        #define BASECLASS_API __declspec(dllexport)
     #else
        #define BASECLASS_API __declspec(dllimport)
     #endif
	 
#endif

#ifdef _USELIB
      #define BASECLASS_API
#endif


/*---------------------------------------
*
---------------------------------------*/
#ifdef WIN32
   #pragma warning(disable:4996)
   #pragma warning(disable:4267)
   #pragma warning(disable:4251)
#endif



/*---------------------------------------
*       base type define
---------------------------------------*/
typedef char                    Char;
typedef unsigned char           UChar;
typedef const char              CChar;
typedef const unsigned char     CUChar;
typedef wchar_t                 Wchar;
typedef const wchar_t           CWchar;
typedef short                   Short;
typedef unsigned short          UShort;
typedef long                    Long;
typedef unsigned long           ULong;
typedef const long              CLong;
typedef const unsigned long     CULong;
typedef int                     Int;
typedef const int               CInt;
typedef unsigned int            UInt;
typedef const unsigned int      CUInt;
typedef size_t                  SizeT;
typedef bool                    Bool;
typedef void                    Void;
typedef const void              CVoid;
typedef signed char             Int8;
typedef unsigned char           UInt8;
typedef signed short            Int16;
typedef unsigned short          UInt16;
typedef signed int              Int32;
typedef unsigned int            UInt32;
typedef signed long long        Int64;
typedef unsigned long long      UInt64;
typedef const unsigned long long CUInt64;
typedef float					Float;
typedef double					Double;


//
#define DO_JOIN(x,y)            x##y

#define Null                    0

//-----------------------------------------
#   define STA_CAST(TYPE, EXPR) static_cast<TYPE>(EXPR)
#   define CON_CAST(TYPE, EXPR) const_cast<TYPE>(EXPR)
#   define REI_CAST(TYPE, EXPR) reinterpret_cast<TYPE>(EXPR)
#   define DYN_CAST(TYPE, EXPR) dynamic_cast<TYPE>(EXPR)
//------------------------------------------------------------------

#define   GV           GAVIN
#define   GV_BEGIN     namespace GV{
#define   GV_END       };
#define   GV_USING     using namespace GV;



#ifdef WIN32
    typedef Int		socklen_t;
#else
    typedef UInt     SOCKET;
	typedef struct sockaddr SOCKADDR;
    typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct hostent*     LPHOSTENT;
	typedef struct in_addr*    LPIN_ADDR;
	typedef Int      HANDLE;
	//
    #define SOCKET_ERROR  -1
	#define INVALID_SOCKET 0
    
#endif

//------------------------------------------------------------------

/*---------------------------------------
*
---------------------------------------*/
GV_BEGIN


//
template <Int I>
class placeholder{};

template <typename T>
struct function_type
{};

template <typename R, typename T>
struct function_type<R (T::*)()>
{
    typedef R r_type;
    typedef T t_type;
};

template <typename R, typename T, typename A1>
struct function_type<R (T::*)(A1)>
{
    typedef R r_type;
    typedef T t_type;
};

template <typename R, typename F, typename A1>
R proxy_eval(F f, A1 a1, placeholder<0>)
{
    return (a1.*f)();
}

template <typename R, typename F, typename A1>
R proxy_eval(F f, A1* a1, placeholder<0>)
{
    return (a1->*f)();
}

template <typename R, typename F, typename A1, typename A2>
R proxy_eval(F f, A1 a1, A2 a2)
{
    return (a1.*f)(a2);
}

template <typename R, typename F, typename A1, typename A2>
R proxy_eval(F f, A1* a1, A2 a2)
{
    return (a1->*f)(a2);
}


#define GV_MALL0C malloc
#define GV_FREE free
#define GV_NEW new 
#define GV_DELETE delete

#ifndef GV_SAFE_FREE
#   define GV_SAFE_FREE(ptr) if ((ptr) != Null) {GV_FREE(ptr); (ptr) = Null;}
#endif

#ifndef GV_SAFE_DELETE
#   define GV_SAFE_DELETE(ptr) if ((ptr) != Null) {GV_DELETE (ptr); (ptr) = Null;}
#endif

#ifndef GV_SAFE_DELETE_ARRAY
#   define GV_SAFE_DELETE_ARRAY(ptr) if ((ptr) != Null) {GV_DELETE [] (ptr); (ptr) = Null;}
#endif

//-------------------------------

#define MEMORY_COPY(target, source, size) memcpy(target, source, size)
#define MEMORY_SET(target, value, size)   memset(target, value, size)
#define MEMORY_MOVE(target,source,size) memmove(target,source,size);

GV_END

#endif