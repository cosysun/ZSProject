#ifndef GWS_DATA_PROCESS_H_
#define GWS_DATA_PROCESS_H_

#include "GWS_Common.h"

namespace GWS
{
    class CLogDataProcess
    {
    public:
        CLogDataProcess();
        ~CLogDataProcess();

    public:
        BOOL ConnectDB();

    public:
        BOOL WriteOmasLogInfo();

    private:
        CADODatabase    m_DBConn;
    };
}
#endif