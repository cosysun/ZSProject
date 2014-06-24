
#ifndef __TOOL_BIND_H__
#define __TOOL_BIND_H__

#include "../config.h"

GV_BEGIN

template <typename F, typename A1, typename A2 = placeholder<0> >
class BASECLASS_API binder
{
	typedef typename function_type<F>::r_type R;

public:
	explicit binder(F f, const A1& a1)
		: m_func(f)
		, m_arg1(a1)
	{}

	explicit binder(F f, const A1& a1, const A2& a2)
		: m_func(f)
		, m_arg1(a1)
		, m_arg2(a2)
	{}

    R operator()()
    {
        return proxy_eval<R>(m_func, m_arg1, m_arg2);
    }
	//
protected:

private:
	F         m_func;
	A1        m_arg1;
	A2        m_arg2;

};


//
class BASECLASS_API binder_ex
{
	//
	template <typename F, typename A1, typename A2 = placeholder<0> >
	class handler
	{
		typedef typename function_type<F>::r_type    R;
		//
	public:
		static Void invoker(Char* param)
		{
			if (param != Null)
			{
				Int pos = 0;
				F f = *(REI_CAST(F*, param + pos));
				pos += sizeof(f);
				A1 a1 = *(REI_CAST(A1*, param + pos));
				pos += sizeof(a1);
				A2 a2 = *(REI_CAST(A2*, param + pos));
				proxy_eval<R>(f, a1, a2);
			}
		}

	};
public:
	binder_ex()
	{
		m_init_time = 0;
		m_bind_data = 0;
		m_invoker = 0;
	}
	//
	~binder_ex() {GV_SAFE_DELETE_ARRAY(m_bind_data);}

	template <typename F, typename A1, typename A2>
	Void init(F f, A1 const & a1, A2 const & a2, Bool once = true)
	{
		if (once && m_init_time > 1)
			return;
		m_init_time++;
		if (m_bind_data)
			GV_SAFE_DELETE_ARRAY(m_bind_data);
		m_bind_data = GV_NEW char[sizeof(f) + sizeof(a1) + sizeof(a2)];
		UInt pos = 0;
		UInt cell_size=0;

		cell_size = sizeof(f);
		MEMORY_COPY(m_bind_data + pos, &f, cell_size);
		pos += cell_size;

		cell_size = sizeof(a1);
		MEMORY_COPY(m_bind_data + pos, &a1, cell_size);
		pos += cell_size;

		cell_size = sizeof(a2);
		MEMORY_COPY(m_bind_data + pos, &a2, cell_size);
		pos += cell_size;
		//
		m_invoker = handler<F, A1, A2>::invoker;
	}

    Bool operator()()
	{
		if (!m_bind_data || !m_invoker)
			return false;

		m_invoker(m_bind_data);
		//
		return true;
	}

protected:

private:
	UInt m_init_time;
	Char*        m_bind_data;
	Void (*m_invoker)(Char*);

};


template <typename F, typename A1>
binder<F, A1>
binder_bind(F f, A1 a1)
{
    return binder<F, A1>(f, a1);
}

template <typename F, typename A1, typename A2>
binder<F, A1, A2>
binder_bind(F f, A1 a1, A2 a2)
{
    return binder<F, A1, A2>(f, a1, a2);
}

//

GV_END

#endif
