#ifndef GWS_LOG_H
#define GWS_LOG_H

#include "GWS_Common.h"

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
	//日志基类

	class CLog
	{
	public:

		CLog();

		virtual ~CLog();

		virtual void Init() = 0;

		void Write(LEVEL_LOG logLevel, LPCSTR lpFormat, ...);

		void WriteEx(LEVEL_LOG logLevel, LPCSTR lpFormat, ...);

		void SetLogLevel(LEVEL_LOG logLevel);

	protected:

		CLogFile m_CLogFile;
	};

	//////////////////////////////////////////////////////////////////////////
	//程序

	class CSystemLog : public CLog, public CSingletonT<CSystemLog>
	{
	public:

		virtual ~CSystemLog();

		virtual void Init();

		friend class CSingletonT<CSystemLog>;

	private:

		CSystemLog();

	};

	//////////////////////////////////////////////////////////////////////////
	//客户端相关日志

	class CClientLog : public CLog, public CSingletonT<CClientLog>
	{
	public:

		virtual ~CClientLog();

		virtual void Init();

		friend class CSingletonT<CClientLog>;

	private:

		CClientLog();
	};

	//////////////////////////////////////////////////////////////////////////
	//应用服务相关日志

	class CAppSrvLog : public CLog, public CSingletonT<CAppSrvLog>
	{
	public:

		virtual ~CAppSrvLog();

		virtual void Init();

		friend class CSingletonT<CAppSrvLog>;

	private:

		CAppSrvLog();
	};

    //////////////////////////////////////////////////////////////////////////
    //统计信息

    class CSnmpLog : public CLog, public CSingletonT<CSnmpLog>
    {
    public:

        CSnmpLog();

        virtual ~CSnmpLog();

        virtual void Init();

        BOOL Init(LPCSTR lpDirName);

        friend class CSingletonT<CSnmpLog>;
    };
}

#endif