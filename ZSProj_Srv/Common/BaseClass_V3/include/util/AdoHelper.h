#ifndef _ADO_HELPER_H_H
#define _ADO_HELPER_H_H

#include "../config.h"
#include <string>
#include <sstream>
#include <time.h>
#include <comutil.h>
using namespace std;

//template <Bool b> struct cpp_static_assert {
//	virtual void static_assert_failed() = 0;
//};
//template <> struct cpp_static_assert<true> {};
//#define STATIC_ASSERT2(x, l, c) struct cpp_static_assert_##l##_##c { cpp_static_assert<x> cpp_assert; }
//#define STATIC_ASSERT(x) STATIC_ASSERT2(x, __LINE__,__COUNTER__)

#define ADO_MAX_TIME64_T 0x7915ECBFFi64 //time_t

struct TimeHolder
{
	TimeHolder():time(0) {}
	TimeHolder(time_t t):time(t) {}
	TimeHolder(const TimeHolder& rhs)
	{
		time = rhs.time;
	}

	TimeHolder& operator=(const TimeHolder& rhs) 
	{
		time = rhs.time;
		return *this;
	}
	time_t time;
};

inline string StringTrim(const string& str)
{
	string::size_type nBegin = str.find_first_not_of(' ');
	string::size_type nEnd = str.find_last_not_of(' ');
	return str.substr(nBegin, nEnd - nBegin + 1);	
}

template <typename T>
inline Bool Variant2T(_variant_t& value, T& ret)
{
	//STATIC_ASSERT(false); //not support
	return false;
}

template <typename T>
inline Bool T2Variant(T& value, _variant_t& ret)
{
	//STATIC_ASSERT(false); //not support
	return false;
}

//----------------------------------------------------------------
//Char or byte
template <> inline Bool Variant2T<Char>(_variant_t& value, Char& ret)
{
	switch( value.vt )
	{
	case VT_BOOL:
		ret = (Char)value.boolVal;
	case VT_I1:
		ret = (Char)value.cVal;
		break;
	case VT_UI1:
		ret = (Char)value.bVal;
		break;
	default:
		return false;
	}
	return true;
}

template <> inline Bool Variant2T<UChar>(_variant_t& value, UChar& ret)
{
	return Variant2T<Char>(value,(Char&)ret);
}

template <> inline Bool T2Variant<Char>(Char& value, _variant_t& ret)
{
	ret.vt = VT_I1;
	ret.cVal = value;
	return true;
}

template <> inline Bool T2Variant<UChar>(UChar& value, _variant_t& ret)
{
	ret.vt = VT_UI1;
	ret.bVal = value;
	return true;
}

//----------------------------------------------------------------
//Short
template <> inline Bool Variant2T<Short>(_variant_t& value, Short& ret)
{
	switch( value.vt )
	{
	case VT_I2:
		ret = (Short)value.iVal; 
		break;
	case VT_UI2:
		ret = (Short)value.uiVal;
		break;
	default:
		{
			Char v = 0;
			Bool b = Variant2T<Char>(value, v);
			if( b )
				ret = v;
			return b;
		}
	}
	return true;
}

template <> inline Bool Variant2T<UShort>(_variant_t& value, UShort& ret)
{
	return Variant2T<Short>(value,(Short&)ret);
}

template <> inline Bool T2Variant<Short>(Short& value, _variant_t& ret)
{
	ret.vt = VT_I2;
	ret.iVal = value;
	return true;
}

template <> inline Bool T2Variant<UShort>(UShort& value, _variant_t& ret)
{
	ret.vt = VT_UI2;
	ret.uiVal = value;
	return true;
}

//----------------------------------------------------------------
//Int
template <> inline Bool Variant2T<Int>(_variant_t& value, Int& ret)
{
	switch( value.vt )
	{
	case VT_I4:
		ret = value.lVal;
		break;
	case VT_INT:
		ret = value.intVal;
		break;
	case VT_UI4:
		ret = (Int)value.ulVal;
		break;
	case VT_UINT:
		ret = (Int)value.uintVal;
		break;
	default:
		{
			Short v = 0;
			Bool b = Variant2T<Short>(value, v);
			if( b )
				ret = v;
			return b;
		}
	}
	return true;
}

template <> inline Bool Variant2T<UInt>(_variant_t& value, UInt& ret)
{
	return Variant2T<Int>(value,(Int&)ret);
}

template <> inline Bool T2Variant<Int>(Int& value, _variant_t& ret)
{
	ret.vt = VT_INT;
	ret.intVal = value;
	return true;
}

template <> inline Bool T2Variant<UInt>(UInt& value, _variant_t& ret)
{
	ret.vt = VT_UINT;
	ret.uintVal = value;
	return true;
}

//----------------------------------------------------------------
//Long
template <> inline Bool Variant2T<Long>(_variant_t& value, Long& ret)
{
	return Variant2T<Int>(value, (Int&)ret);
}

template <> inline Bool Variant2T<ULong>(_variant_t& value, ULong& ret)
{
	return Variant2T<Int>(value, (Int&)ret);
}

template <> inline Bool T2Variant<Long>(Long& value, _variant_t& ret)
{
	ret.vt = VT_I4;
	ret.lVal = value;
	return true;
}

template <> inline Bool T2Variant<ULong>(ULong& value, _variant_t& ret)
{
	ret.vt = VT_UI4;
	ret.ulVal = value;
	return true;
}

//----------------------------------------------------------------
//int64
template <> inline Bool Variant2T<Int64>(_variant_t& value, Int64& ret)
{
	switch( value.vt )
	{
	case VT_I8:
		ret = value.llVal;
		break;
	case VT_UI8:
		ret = (Int64)value.ullVal;
		break;
	case VT_DECIMAL:
		ret = value.decVal.Lo64;
		ret *= (value.decVal.sign == 128)? -1 : 1;
		break;
	default:
		{
			Int v = 0;
			Bool b = Variant2T<Int>(value, v);
			if( b )
				ret = v;
			return b;
		}
	}
	return true;
}

template <> inline Bool Variant2T<UInt64>(_variant_t& value, UInt64& ret)
{
	return Variant2T<Int64>(value,(Int64&)ret);
}

template <> inline Bool T2Variant<Int64>(Int64& value, _variant_t& ret)
{
	ret.vt = VT_I8;
	ret.llVal = value;
	return true;
}

template <> inline Bool T2Variant<UInt64>(UInt64& value, _variant_t& ret)
{
	ret.vt = VT_UI8;
	ret.ullVal = value;
	return true;
}

//----------------------------------------------------------------
//Float
template <> inline Bool Variant2T<Float>(_variant_t& value, Float& ret)
{
	switch( value.vt )
	{
	case VT_R4:
		ret = value.fltVal;
		break;
	default:
		return false;
	}
	return true;
}

template <> inline Bool T2Variant<Float>(Float& value, _variant_t& ret)
{
	ret.vt = VT_R4;
	ret.fltVal = value;
	return true;
}

//----------------------------------------------------------------
//Double
template <> inline Bool Variant2T<Double>(_variant_t& value, Double& ret)
{
	switch( value.vt )
	{
	case VT_R8:
		ret = value.dblVal;
		break;
	case VT_DECIMAL:
		{
			ret = value.decVal.Lo32;
			ret *= (value.decVal.sign == 128)? -1 : 1;
			ret /= pow(10.0, (Int)value.decVal.scale); 
		}
		break;
	default:
		{
			Float v = 0.f;
			Bool b = Variant2T<Float>(value, v);
			if( b )
				ret = v;
			return b;
		}
	}
	return true;
}

template <> inline Bool T2Variant<Double>(Double& value, _variant_t& ret)
{
	ret.vt = VT_R8;
	ret.dblVal = value;
	return true;
}

//----------------------------------------------------------------
//time
template <> inline Bool Variant2T<TimeHolder>(_variant_t& value, TimeHolder& ret)
{
	switch( value.vt )
	{
	case VT_DATE:
		{
			SYSTEMTIME st = { 0 };
			if( !VariantTimeToSystemTime(value.date, &st) )
				return false;
			struct tm tmDest = { 0 };
			tmDest.tm_sec = st.wSecond;
			tmDest.tm_min = st.wMinute;
			tmDest.tm_hour = st.wHour;
			tmDest.tm_mday = st.wDay;
			tmDest.tm_mon = st.wMonth - 1;
			tmDest.tm_year = st.wYear - 1900;
			tmDest.tm_wday = st.wDayOfWeek;
			tmDest.tm_isdst = -1;   // Force DST checking

			if( tmDest.tm_year >= 1101 )
				return false;
			ret.time = mktime(&tmDest);    // Normalize
			if( ret.time < 0 )
				ret.time = 0;
		}
		break;
	case VT_NULL:
	case VT_EMPTY:
		{
			ret.time = 0;
		}
		break;
	default:
		return false;
	}
	return true;
}

template <> inline Bool T2Variant<TimeHolder>(TimeHolder& value, _variant_t& ret)
{
	ret.vt = VT_DATE;
	ret.date = 0;

	SYSTEMTIME st = { 0 };
	struct tm tmp = { 0 };
	//vs2005下时间超出范围会有bug，这里处理下
	if( (Int64)value.time < 0 )
		value.time = 0;
	if( value.time > ADO_MAX_TIME64_T )
		value.time = ADO_MAX_TIME64_T;

	if( localtime_s(&tmp, &value.time) )
		return false;
	st.wYear = tmp.tm_year+1900;
	st.wMonth = tmp.tm_mon + 1;
    st.wDay = tmp.tm_mday;
	st.wHour = tmp.tm_hour;
	st.wMinute = tmp.tm_min;
	st.wSecond = tmp.tm_sec;
	Double t = 0;
	if( !SystemTimeToVariantTime(&st, &t))
		return false;

	ret.date = t;
	return true;
}


//---------------------
//string
template <> inline Bool Variant2T<string>(_variant_t& value, string& ret)
{
	switch(value.vt) 
		{
		case VT_R4:
			{
				ostringstream ss;
				ss<<value.fltVal;
				ret = ss.str();
			}
			break;
		case VT_R8:
			{
				ostringstream ss;
				ss<<value.dblVal;
				ret = ss.str();
			}
			break;
		case VT_BSTR:
			{
				_bstr_t  bstrTemp(value.bstrVal);
				ret.assign((Char*)bstrTemp);
			}
			break;
		case VT_I1:
			{
				ostringstream ss;
				ss<<value.cVal;
				ret = ss.str();
			}
			break;
		case VT_UI1:
			{
				ostringstream ss;
				ss<<value.bVal;
				ret = ss.str();
			}
			break;
		case VT_I2:
			{
				ostringstream ss;
				ss<<value.iVal;
				ret = ss.str();
			}
			break;
		case VT_UI2:
			{
				ostringstream ss;
				ss<<value.uiVal;
				ret = ss.str();
			}
			break;
		case VT_I4:
			{
				ostringstream ss;
				ss<<value.lVal;
				ret = ss.str();
			}
			break;
		case VT_UI4:
			{
				ostringstream ss;
				ss<<value.ulVal;
				ret = ss.str();
			}
			break;
		case VT_INT:
			{
				ostringstream ss;
				ss<<value.intVal;
				ret = ss.str();
			}
			break;
		case VT_UINT:
			{
				ostringstream ss;
				ss<<value.uintVal;
				ret = ss.str();
			}
			break;
		case VT_I8:
			{
				ostringstream ss;
				ss<<value.llVal;
				ret = ss.str();
			}
			break;
		case VT_UI8:
			{
				ostringstream ss;
				ss<<value.ullVal;
				ret = ss.str();
			}
			break;
		case VT_DECIMAL:
			{
				Double val = value.decVal.Lo32;
				val *= (value.decVal.sign == 128)? -1 : 1;
				val /= pow((Double)10, (Int)value.decVal.scale); 
				ostringstream ss;
				ss<<val;
				ret = ss.str();
			}
			break;
		case VT_DATE:
			{
				TimeHolder th;
				if( !Variant2T<TimeHolder>(value, th) )
					return false;
				
				Char szBuf[256] = { 0 };
				struct tm t;
				localtime_s(&t, &th.time);
				if( asctime_s(szBuf, 255, &t) )
					return false;
				ret = szBuf;
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			ret="";
			break;
		case VT_BOOL:
			ret = value.boolVal == VARIANT_TRUE? "TRUE":"FALSE";
			break;
		default:
			ret="";
			return false;
		}
	return true;
}

template <> inline Bool T2Variant<string>(string& value, _variant_t& ret)
{
	_bstr_t bst(value.c_str());
	ret = bst;
	return true;
}


//---------------------
//wstring
template <> inline Bool Variant2T<wstring>(_variant_t& value, wstring& ret)
{
	switch(value.vt) 
		{
		case VT_R4:
			{
				wostringstream ss;
				ss<<value.fltVal;
				ret = ss.str();
			}
			break;
		case VT_R8:
			{
				wostringstream ss;
				ss<<value.dblVal;
				ret = ss.str();
			}
			break;
		case VT_BSTR:
			{
				_bstr_t  bstrTemp(value.bstrVal);
				ret.assign((wchar_t*)bstrTemp);
			}
			break;
		case VT_I1:
			{
				wostringstream ss;
				ss<<value.cVal;
				ret = ss.str();
			}
			break;
		case VT_UI1:
			{
				wostringstream ss;
				ss<<value.bVal;
				ret = ss.str();
			}
			break;
		case VT_I2:
			{
				wostringstream ss;
				ss<<value.iVal;
				ret = ss.str();
			}
			break;
		case VT_UI2:
			{
				wostringstream ss;
				ss<<value.uiVal;
				ret = ss.str();
			}
			break;
		case VT_I4:
			{
				wostringstream ss;
				ss<<value.lVal;
				ret = ss.str();
			}
			break;
		case VT_UI4:
			{
				wostringstream ss;
				ss<<value.ulVal;
				ret = ss.str();
			}
			break;
		case VT_INT:
			{
				wostringstream ss;
				ss<<value.intVal;
				ret = ss.str();
			}
			break;
		case VT_UINT:
			{
				wostringstream ss;
				ss<<value.uintVal;
				ret = ss.str();
			}
			break;
		case VT_I8:
			{
				wostringstream ss;
				ss<<value.llVal;
				ret = ss.str();
			}
			break;
		case VT_UI8:
			{
				wostringstream ss;
				ss<<value.ullVal;
				ret = ss.str();
			}
			break;
		case VT_DECIMAL:
			{
				Double val = value.decVal.Lo32;
				val *= (value.decVal.sign == 128)? -1 : 1;
				val /= pow((Double)10, (Int)value.decVal.scale); 
				wostringstream ss;
				ss<<val;
				ret = ss.str();
			}
			break;
		case VT_DATE:
			{
				TimeHolder th;
				if( !Variant2T<TimeHolder>(value, th) )
					return false;
				
				wchar_t wszBuf[256] = { 0 };
				struct tm t;
				localtime_s(&t, &th.time);
				if( _wasctime_s(wszBuf, 255, &t) )
					return false;
				ret = wszBuf;
			}
			break;
		case VT_EMPTY:
		case VT_NULL:
			ret=L"";
			break;
		case VT_BOOL:
			ret = value.boolVal == VARIANT_TRUE? L"TRUE":L"FALSE";
			break;
		default:
			ret=L"";
			return false;
		}
	return true;
}

template <> inline Bool T2Variant<wstring>(wstring& value, _variant_t& ret)
{
	_bstr_t bst(value.c_str());
	ret = bst;
	return true;
}

//------------------
//make binary
inline _variant_t MakeBinary(Char* pData, Int nDataLen)
{
	_variant_t vtVal;

	vtVal.vt = VT_NULL;
	if( pData != NULL && nDataLen  > 0 )
	{
		do
		{
			Long lngOffset = 0;
			UCHAR chData;
			SAFEARRAY FAR *psa = NULL;
			SAFEARRAYBOUND rgsabound[1];
			//Create a safe array to store the array of BYTES 
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = nDataLen;
			psa = SafeArrayCreate(VT_UI1,1,rgsabound);
			if( psa == NULL )
				break;

			while(lngOffset < (Long)nDataLen)
			{
				chData	= ((UCHAR*)pData)[lngOffset];
				SafeArrayPutElement(psa, &lngOffset, &chData);
				lngOffset++;
			}

			//Assign the Safe array  to a variant. 
			vtVal.vt = VT_ARRAY|VT_UI1;
			vtVal.parray = psa;
		}while(false);
	}
	return vtVal;
}

inline Int GetBinary(_variant_t& vtVal, Char* pDataOut, Int nDataLenOut)
{
	if( pDataOut == NULL || nDataLenOut <= 0 )
		return -1;
	Int bytes = 0;
	if( vtVal.vt == (VT_ARRAY | VT_UI1) )
	{
		Char *pBuf = NULL;
		SafeArrayAccessData(vtVal.parray,(void **)&pBuf); 
		if( vtVal.parray->cbElements != 1 )
		{
			SafeArrayUnaccessData (vtVal.parray);
			return -1; 
		}
		bytes = (Int)vtVal.parray->rgsabound[0].cElements;

		memcpy(pDataOut,pBuf, min(bytes, nDataLenOut));

		SafeArrayUnaccessData (vtVal.parray);
		return bytes;
	}
	return 0;
}

#endif //_ADO_HELPER_H_H