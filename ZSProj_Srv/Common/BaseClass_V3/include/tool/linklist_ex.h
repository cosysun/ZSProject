

#ifndef __GV_LINKLISTEX_H__
#define __GV_LINKLISTEX_H__

#include "../config.h"

GV_BEGIN

#define LIST_LINKER(link_name) \
    class link_name \
    { \
    public: \
        link_name(Void) {DO_JOIN(clear_, link_name)();} \
        \
        link_name* DO_JOIN(get_next_, link_name)(Void) {return DO_JOIN(m_next_, link_name);} \
        Void DO_JOIN(set_next_, link_name)(link_name* next) {DO_JOIN(m_next_, link_name) = next;} \
        link_name* DO_JOIN(get_prev_, link_name)(Void) {return DO_JOIN(m_prev_, link_name);} \
        Void DO_JOIN(set_prev_, link_name)(link_name* prev) {DO_JOIN(m_prev_, link_name) = prev;} \
        Void* DO_JOIN(get_list_, link_name)(Void) {return DO_JOIN(m_list_, link_name);} \
        Void DO_JOIN(set_list_, link_name)(Void* list_pointer) {DO_JOIN(m_list_, link_name) = list_pointer;} \
        \
        Void DO_JOIN(clear_, link_name)(Void) {DO_JOIN(m_next_, link_name) = Null; DO_JOIN(m_prev_, link_name) = Null; DO_JOIN(m_list_, link_name) = Null;} \
        \
        virtual Void DO_JOIN(initial_, link_name)(Void) {} \
        virtual Void DO_JOIN(final_, link_name)(Void) {} \
        virtual Void DO_JOIN(join_list_, link_name)(Void* /*list*/) {} \
        virtual Void DO_JOIN(exit_list_, link_name)(Void* /*list*/) {} \
        \
    private: \
        link_name* DO_JOIN(m_next_, link_name); \
        link_name* DO_JOIN(m_prev_, link_name); \
		Void* DO_JOIN(m_list_, link_name); \
        \
    };

#define LIST(list_name, link_name) \
    class list_name \
    { \
    public: \
        list_name() {init();} \
        ~list_name() \
		{ \
		} \
        \
        Void init(Void) {m_head = Null; m_tail = Null; m_num = 0;} \
        \
        Void clear(Void) \
        { \
            link_name* cur = m_head; \
            link_name* temp; \
            while (cur) \
            { \
                temp = cur; \
                cur = cur->DO_JOIN(get_next_, link_name)(); \
                temp->DO_JOIN(clear_, link_name)(); \
                temp->DO_JOIN(exit_list_, link_name)(this); \
            } \
            m_head = m_tail = Null; \
            m_num = 0; \
        } \
        \
        Long size(Void) const {return m_num;} \
        Bool empty(Void) const {return m_num == 0;} \
        link_name* front(Void) {return m_head;} \
        link_name* end(Void) {return m_tail;} \
        \
        Bool push_front(link_name* val) \
        { \
            if (val == Null) \
                return false; \
            val->DO_JOIN(set_prev_, link_name)(Null); \
            val->DO_JOIN(set_next_, link_name)(m_head); \
			val->DO_JOIN(set_list_, link_name)(this); \
            if (m_head) \
                m_head->DO_JOIN(set_prev_, link_name)(val); \
            else \
                m_tail = val; \
            m_head = val; \
            m_num++; \
            val->DO_JOIN(join_list_, link_name)(this); \
            return true; \
        } \
        \
        Bool push_back(link_name* val) \
        { \
            if (val == Null) \
                return false; \
            val->DO_JOIN(set_prev_, link_name)(m_tail); \
            val->DO_JOIN(set_next_, link_name)(Null); \
			val->DO_JOIN(set_list_, link_name)(this); \
            if (m_tail) \
                m_tail->DO_JOIN(set_next_, link_name)(val); \
            else \
                m_head = val; \
            m_tail = val; \
            m_num++; \
            val->DO_JOIN(join_list_, link_name)(this); \
            return true; \
        } \
		\
        Bool push_back_list(link_name* val) \
        { \
            if (val == Null) \
                return false; \
            val->DO_JOIN(set_prev_, link_name)(m_tail); \
			val->DO_JOIN(set_list_, link_name)(this); \
            if (m_tail) \
                m_tail->DO_JOIN(set_next_, link_name)(val); \
            else \
                m_head = val; \
            m_num++; \
            val->DO_JOIN(join_list_, link_name)(this); \
			while (val->DO_JOIN(get_next_, link_name)()) \
			{ \
				val = val->DO_JOIN(get_next_, link_name)(); \
				m_num++; \
				val->DO_JOIN(set_list_, link_name)(this); \
				val->DO_JOIN(join_list_, link_name)(this); \
			} \
            m_tail = val; \
            return true; \
        } \
        \
        Bool pop_front(Void) \
        { \
            if (!m_head) \
                return false; \
            link_name* temp = m_head; \
            m_head = m_head->DO_JOIN(get_next_, link_name)(); \
            if (m_head) \
                m_head->DO_JOIN(set_prev_, link_name)(Null); \
            else \
                m_tail = Null; \
            temp->DO_JOIN(clear_, link_name)(); \
            m_num--; \
            temp->DO_JOIN(exit_list_, link_name)(this); \
            return true; \
        } \
        \
        Bool pop_back(Void) \
        { \
            if (!m_tail) \
                return false; \
            link_name* temp = m_tail; \
            m_tail = m_tail->DO_JOIN(get_prev_, link_name)(); \
            if (m_tail) \
                m_tail->DO_JOIN(set_next_, link_name)(Null); \
            else \
                m_head = Null; \
            temp->DO_JOIN(clear_, link_name)(); \
            m_num--; \
            temp->DO_JOIN(exit_list_, link_name)(this); \
            return true; \
        } \
        \
        Bool find(link_name* val) \
        { \
            if (val == Null) \
                return false; \
			if (val->DO_JOIN(get_list_, link_name)() != REI_CAST(Void*, this)) \
				return false; \
            link_name* temp = m_head; \
            while (temp) \
            { \
                if (temp == val) \
                    return true; \
                temp = temp->DO_JOIN(get_next_, link_name)(); \
            } \
            return false; \
        } \
        \
        Bool erase(link_name* val) \
        { \
            if (val == Null) \
                return false; \
			if (val->DO_JOIN(get_list_, link_name)() != REI_CAST(Void*, this)) \
				return false; \
           if ((m_head == Null) || (m_tail == Null)) \
                return false; \
            if (val->DO_JOIN(get_prev_, link_name)()) \
			{\
			    if(val->DO_JOIN(get_prev_,link_name)()->DO_JOIN(get_next_,link_name)() != val) \
				   return false;\
                val->DO_JOIN(get_prev_, link_name)()->DO_JOIN(set_next_, link_name)(val->DO_JOIN(get_next_, link_name)()); \
			}\
            if (val->DO_JOIN(get_next_, link_name)()) \
			{\
			    if(val->DO_JOIN(get_next_,link_name)()->DO_JOIN(get_prev_,link_name)() != val) \
				   return false;\
                val->DO_JOIN(get_next_, link_name)()->DO_JOIN(set_prev_, link_name)(val->DO_JOIN(get_prev_, link_name)()); \
			}\
            if (val == m_head) \
                if ((m_head = val->DO_JOIN(get_next_, link_name)()) != Null) \
                    m_head->DO_JOIN(set_prev_, link_name)(Null); \
            if (val == m_tail) \
                if ((m_tail = val->DO_JOIN(get_prev_, link_name)()) != Null) \
                    m_tail->DO_JOIN(set_next_, link_name)(Null); \
            m_num--; \
            val->DO_JOIN(clear_, link_name)(); \
            val->DO_JOIN(exit_list_, link_name)(this); \
            return true; \
        } \
        \
        Bool insert(link_name* whe, link_name* val, Bool ahead_whe = true) \
        { \
            if (whe == Null || val == Null) \
                return false; \
            if (m_head == Null || m_tail == Null) \
                return false; \
            if (ahead_whe) \
            { \
                val->DO_JOIN(set_next_, link_name)(whe); \
                if (whe->DO_JOIN(get_prev_, link_name)()) \
                { \
                    whe->DO_JOIN(get_prev_, link_name)()->DO_JOIN(set_next_, link_name)(val); \
                    val->DO_JOIN(set_prev_, link_name)(whe->DO_JOIN(get_prev_, link_name)()); \
                } \
                else \
                { \
                    m_head = val; \
                    val->DO_JOIN(set_prev_, link_name)(Null); \
                } \
                whe->DO_JOIN(set_prev_, link_name)(val); \
            } \
            else \
            { \
                val->DO_JOIN(set_prev_, link_name)(whe); \
                if (whe->DO_JOIN(get_next_, link_name)()) \
                { \
                    whe->DO_JOIN(get_next_, link_name)()->DO_JOIN(set_prev_, link_name)(val); \
                    val->DO_JOIN(set_next_, link_name)(whe->DO_JOIN(get_next_, link_name)()); \
                } \
                else \
                { \
                    m_tail = val; \
                    val->DO_JOIN(set_next_, link_name)(Null); \
                } \
                whe->DO_JOIN(set_next_, link_name)(val); \
            } \
            val->DO_JOIN(set_list_, link_name)(this); \
            val->DO_JOIN(join_list_, link_name)(this); \
            return true; \
        } \
        \
        link_name* list_begin(Void) {m_search = m_head;return m_search;} \
        link_name* list_next(Void) \
        { \
            link_name* pRet = Null; \
            if (m_search) \
			{\
			   m_search = m_search->DO_JOIN(get_next_, link_name)(); \
			   pRet = m_search;\
		    }\
            return pRet; \
        } \
        \
    private: \
        link_name* m_head; \
        link_name* m_tail; \
        link_name* m_search; \
        ULong m_num; \
        \
    };

GV_END

#endif
