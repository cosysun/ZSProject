

#ifndef __TOOL_FUNCTION_EX_H__
#define __TOOL_FUNCTION_EX_H__

#include "../config.h"

GV_BEGIN

template <Int i>
class placeholder_ex{};

template <typename F,typename A1,typename A2>
Void proxy_eval_ex(F f,A1 a1,A2 a2)
{
	return (a1.*f)();
}

template <typename F,typename A1,typename A2>
Void proxy_eval_ex(F f,A1* a1,A2 a2)
{
	return (a1->*f)();
}

//
class function_ex
{
	typedef Void (*function_invoker)(Char*);
	//
	template <typename F,typename Arg1,typename Arg2>
	class function_binder
	{
	public:
		static Void binder(Char*  param)
		{
			if (param != Null)
			{
				Int pos = 0;
				F f = *(REI_CAST(F*, param + pos));
				pos += sizeof(f);
				Arg1 a1 = *(REI_CAST(Arg1*, param + pos));
				pos += sizeof(a1);
				Arg2 a2 = *(REI_CAST(Arg2*, param + pos));
				proxy_eval_ex(f, a1, a2);
			}
		}
	};
	//	
public:
	function_ex(){m_bind_data = 0;m_nInitNum=0;m_invoker=0;}
	//
	template <typename F,typename Arg1,typename Arg2>
	Void bind(F f,Arg1& a1,Arg2& a2,Bool  once = true)
	{
		if (once && m_nInitNum > 1)
			return;
		m_nInitNum++;
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
		m_invoker = function_binder<F, Arg1, Arg2>::binder;
	}
	//
	Bool operator()()
	{
		if (!m_bind_data || !m_invoker)
			return false;

		m_invoker(m_bind_data);
		//
		return true;
	}
private:
	Char*               m_bind_data;
	Int                 m_nInitNum;
	function_invoker    m_invoker;
	
};


GV_END


#endif 
