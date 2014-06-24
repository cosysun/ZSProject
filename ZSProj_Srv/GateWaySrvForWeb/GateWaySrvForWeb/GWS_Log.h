#ifndef GWS_LOG_H
#define GWS_LOG_H

#include "GWS_Common.h"

namespace GWS
{
	//////////////////////////////////////////////////////////////////////////
	//��־����

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
	//����

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
	//�ͻ��������־

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
	//Ӧ�÷��������־

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
    //ͳ����Ϣ

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