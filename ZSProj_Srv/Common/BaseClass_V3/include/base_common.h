#ifndef __BASE_COMMON_H__
#define __BASE_COMMON_H__

#include "config.h"

GV_BEGIN

/*-----------------------------------
-   list is debug info
--------------------------------------*/

#define __DEBUG_DELETESOCKET_SAFE__         //
#define __DEBUG_CLOSESOCKET__                  //
#define __DEBUG_CLOSESOCKET_EXCEPTION__          //
#define __USE_LOG__                        //
#define __DEBUG_PERFORMANCE__             //debug performance add by gavin on 2012.12.28
//#define __TRACKER_PERFORMANCE__                //
//#define __USE_TRACKER_PACK__                   //
//#define __USE_DELETESOCKET_SAFE__           //
//#define __TRACKER_DIELOCK_                    //

const Short             g_sTime_Performance = 16;//max timeout value
/*-----------------------------------
-   debug info
--------------------------------------*/
/*
extern Char                         g_szLogFileName[260];//log file name 
extern Char                         g_szLogFileName_PackTracker[260];//pack tracker file name
extern Char                         g_szLogFileName_DeadLock[260];//deadlock file name
extern Char                         g_szLogFileName_Performance[260];//performance file name
*/

/*-----------------------------------
-   
--------------------------------------*/
extern Void  Tracker_Performance_Log(CChar* pszLogFileName,CChar*   pszFormat,...);

extern Void  Tracker_DeadLock_Log(CChar*   pszFormat,...);

GV_END

#endif