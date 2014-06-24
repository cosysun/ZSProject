#include "ADODataBuilder.h"

//////////////////////////////////////////////////////////////////////////
#pragma pack(1)

//数据协议：TDataHead + TDataInfo(1) + 列名(1) + 数据(1) + TDataInfo(2) + 列名(2) + 数据(2) + TDataInfo(3) + 列名(3) + 数据(3) + ......

struct TDataHead
{
	WORD wFieldCount;
};

struct TDataInfo
{
	WORD wADOType;		//ADO的数据类型
	WORD wVT;			//_variant_t中的VT（数据类型）
	WORD wSize;			//数据的实际大小
	WORD wColNameLen;	//列名称长度（不包含结尾的 \0 结束符）
};

#pragma pack()

//////////////////////////////////////////////////////////////////////////

DWORD CADODataBuilder::DataBuilder(CADORecordset& recordset, PCHAR pBuffer, DWORD dwSize)
{	    
	if (recordset.IsEof() || !pBuffer || 0 == dwSize)
	{
		return 0;
	}

	PCHAR pTemp = pBuffer;
	DWORD dwRealSize = 0;
	int nIndex = 0;
	int nFieldCount = (int)recordset.GetFieldCount();

	if (0 >= nFieldCount)
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	//写入字段数
	
	dwRealSize += sizeof(TDataHead);
	
	if (dwSize < dwRealSize)
	{
		return 0;
	}
	
	TDataHead aDataHead = {0};
	aDataHead.wFieldCount = (WORD)nFieldCount;
	memcpy(pTemp, &aDataHead, sizeof(TDataHead));
	pTemp += sizeof(TDataHead);

	//////////////////////////////////////////////////////////////////////////
	//构造数据内容描述

	TDataInfo aDataInfo = {0};
	CADOFieldInfo aFileInfo = {0};
	_variant_t aVar;

	for (nIndex = 0; nIndex < nFieldCount; ++nIndex)
	{
		dwRealSize += sizeof(TDataInfo);
		
		if (dwSize < dwRealSize)
		{
			return 0;
		}
	
		if (!recordset.GetFieldInfo(nIndex, &aFileInfo))
		{
			return 0;
		}

		if (!recordset.GetFieldValue(nIndex, aVar))
		{
			return 0;
		}
		
		aDataInfo.wADOType = aFileInfo.m_nType;
		aDataInfo.wVT = (WORD)aVar.vt;
		
		aDataInfo.wColNameLen = (WORD)strlen(aFileInfo.m_strName);
		dwRealSize += aDataInfo.wColNameLen;

		if (dwSize < dwRealSize)
		{
			return 0;
		}
		
		switch (aVar.vt)
		{
		case VT_I2:
			{
				aDataInfo.wSize = sizeof(short);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;
			
				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;
			
				short aValue = (short)aVar;
				
				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_I4:
			{
				aDataInfo.wSize = sizeof(long);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				long aValue = (long)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_R4:
			{
				aDataInfo.wSize = sizeof(float);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				float aValue = (float)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_R8:
			{
				aDataInfo.wSize = sizeof(double);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				double aValue = (double)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_CY:
			{
				aDataInfo.wSize = sizeof(CY);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				CY aValue = (CY)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_DATE:
			{
				aDataInfo.wSize = sizeof(DATE);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				DATE aValue = (DATE)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_BSTR:
			{
				_bstr_t strTemp = (_bstr_t)aVar;
				
				LONG lSize = strTemp.length() * sizeof(WCHAR);

				if (lSize > 65535)
				{
					return 0;
				}

				aDataInfo.wSize = (WORD)lSize;
				
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				memcpy(pTemp, strTemp.GetBSTR(), aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}	
			break;			
		case VT_BOOL:
			{
				aDataInfo.wSize = sizeof(bool);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				bool aValue = (bool)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_DECIMAL:
			{
				aDataInfo.wSize = sizeof(DECIMAL);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				DECIMAL aValue = (DECIMAL)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_UI1:
			{
				aDataInfo.wSize = sizeof(BYTE);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				BYTE aValue = (BYTE)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case (VT_ARRAY | VT_UI1)://8209
			{
				if (aFileInfo.m_lSize > 65535)
				{
					return 0;
				}
			
				aDataInfo.wSize = (WORD)aFileInfo.m_lSize;
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;
				
				char *pBuf = NULL;
				
				SafeArrayAccessData(aVar.parray, (void **)&pBuf);

				memcpy(pTemp, pBuf, aDataInfo.wSize);

				SafeArrayUnaccessData(aVar.parray);
			}
			break;
		case VT_NULL:
			{
				aDataInfo.wSize = 0;
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}
				
				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;
			}
			break;
		default:
			{
				return 0;
			}
		}
	}
	
	return dwRealSize;
}

DWORD CADODataBuilder::DataBuilderByColName(CADORecordset& recordset, PCHAR pBuffer, DWORD dwSize, vector<string>& fieldNameArray)
{
	if (recordset.IsEof() || !pBuffer || 0 == dwSize)
	{
		return 0;
	}

	PCHAR pTemp = pBuffer;
	DWORD dwRealSize = 0;
	int nIndex = 0;
	int nFieldCount = (int)fieldNameArray.size();

	if (0 >= nFieldCount)
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	//写入字段数
	
	dwRealSize += sizeof(TDataHead);
	
	if (dwSize < dwRealSize)
	{
		return 0;
	}
	
	TDataHead aDataHead = {0};
	aDataHead.wFieldCount = (WORD)nFieldCount;
	memcpy(pTemp, &aDataHead, sizeof(TDataHead));
	pTemp += sizeof(TDataHead);

	//////////////////////////////////////////////////////////////////////////
	//构造数据内容描述

	TDataInfo aDataInfo = {0};
	CADOFieldInfo aFileInfo = {0};
	_variant_t aVar;

	for (nIndex = 0; nIndex < nFieldCount; ++nIndex)
	{
		dwRealSize += sizeof(TDataInfo);
		
		if (dwSize < dwRealSize)
		{
			return 0;
		}
	
		if (!recordset.GetFieldInfo(fieldNameArray[nIndex].c_str(), &aFileInfo))
		{
			return 0;
		}

		if (!recordset.GetFieldValue(fieldNameArray[nIndex].c_str(), aVar))
		{
			return 0;
		}

		aDataInfo.wADOType = aFileInfo.m_nType;
		aDataInfo.wVT = (WORD)aVar.vt;

		aDataInfo.wColNameLen = (WORD)strlen(aFileInfo.m_strName);
		dwRealSize += aDataInfo.wColNameLen;

		if (dwSize < dwRealSize)
		{
			return 0;
		}

		switch (aVar.vt)
		{
		case VT_I2:
			{
				aDataInfo.wSize = sizeof(short);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				short aValue = (short)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_I4:
			{
				aDataInfo.wSize = sizeof(long);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				long aValue = (long)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_R4:
			{
				aDataInfo.wSize = sizeof(float);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				float aValue = (float)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_R8:
			{
				aDataInfo.wSize = sizeof(double);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				double aValue = (double)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_CY:
			{
				aDataInfo.wSize = sizeof(CY);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				CY aValue = (CY)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_DATE:
			{
				aDataInfo.wSize = sizeof(DATE);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				DATE aValue = (DATE)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_BSTR:
			{
				_bstr_t strTemp = (_bstr_t)aVar;

				LONG lSize = strTemp.length() * sizeof(WCHAR);

				if (lSize > 65535)
				{
					return 0;
				}

				aDataInfo.wSize = (WORD)lSize;

				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				memcpy(pTemp, strTemp.GetBSTR(), aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}	
			break;			
		case VT_BOOL:
			{
				aDataInfo.wSize = sizeof(bool);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				bool aValue = (bool)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_DECIMAL:
			{
				aDataInfo.wSize = sizeof(DECIMAL);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				DECIMAL aValue = (DECIMAL)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case VT_UI1:
			{
				aDataInfo.wSize = sizeof(BYTE);
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				BYTE aValue = (BYTE)aVar;

				memcpy(pTemp, &aValue, aDataInfo.wSize);
				pTemp += aDataInfo.wSize;
			}
			break;
		case (VT_ARRAY | VT_UI1)://8209
			{
				if (aFileInfo.m_lSize > 65535)
				{
					return 0;
				}

				aDataInfo.wSize = (WORD)aFileInfo.m_lSize;
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;

				char *pBuf = NULL;

				SafeArrayAccessData(aVar.parray, (void **)&pBuf);

				memcpy(pTemp, pBuf, aDataInfo.wSize);

				SafeArrayUnaccessData(aVar.parray);
			}
			break;
		case VT_NULL:
			{
				aDataInfo.wSize = 0;
				memcpy(pTemp, &aDataInfo, sizeof(TDataInfo));
				pTemp += sizeof(TDataInfo);

				dwRealSize += aDataInfo.wSize;

				if (dwSize < dwRealSize)
				{
					return 0;
				}

				memcpy(pTemp, aFileInfo.m_strName, aDataInfo.wColNameLen);
				pTemp += aDataInfo.wColNameLen;
			}
			break;
		default:
			{
				return 0;
			}
		}
	}

	return dwRealSize;
}

BOOL CADODataBuilder::GetFieldArray(PCHAR pBuffer, DWORD dwSize, ADOFieldDataArray& aADOFieldDataArray)
{
	if (!pBuffer || 0 == dwSize)
	{
		return FALSE;
	}
	
	if (dwSize < sizeof(TDataHead))
	{
		return FALSE;
	}
	
	aADOFieldDataArray.clear();
	
	TDataHead* pTDataHead = (TDataHead*)pBuffer;
	
	if (0 == pTDataHead->wFieldCount)
	{
		return TRUE;
	}

	if (dwSize < (DWORD)sizeof(TDataInfo) * (DWORD)pTDataHead->wFieldCount)
	{
		return FALSE;
	}
	
	DWORD dwRealSize = sizeof(TDataHead);
	
	TDataInfo* pTDataInfo = (TDataInfo*)(pBuffer + sizeof(TDataHead));
	
	WORD wIndex = 0;
	
	for (wIndex = 0; wIndex < pTDataHead->wFieldCount; ++wIndex)
	{
		dwRealSize += sizeof(TDataInfo);
		
		if (dwSize < dwRealSize)
		{
			return FALSE;
		}
		
		dwRealSize += pTDataInfo->wColNameLen;

		if (dwSize < dwRealSize)
		{
			return FALSE;
		}
		
		dwRealSize += pTDataInfo->wSize;
		
		if (dwSize < dwRealSize)
		{
			return FALSE;
		}
		
		pTDataInfo = (TDataInfo*)((PCHAR)pTDataInfo + sizeof(TDataInfo) + pTDataInfo->wColNameLen + pTDataInfo->wSize);
	}
	
	//////////////////////////////////////////////////////////////////////////
	//解析数据
	
	pTDataInfo = (TDataInfo*)(pBuffer + sizeof(TDataHead));
	
	aADOFieldDataArray.resize(pTDataHead->wFieldCount);
	
	for (wIndex = 0; wIndex < pTDataHead->wFieldCount; ++wIndex)
	{
		TADOFieldData& aTADOFieldData = aADOFieldDataArray[wIndex];
		
		aTADOFieldData.wADOType = pTDataInfo->wADOType;
		aTADOFieldData.wVT = pTDataInfo->wVT;
		
		memcpy(aTADOFieldData.szColName, (PCHAR)pTDataInfo + sizeof(TDataInfo), 
			pTDataInfo->wColNameLen > 49 ? 49 : pTDataInfo->wColNameLen);
		
		aTADOFieldData.szColName[49] = NULL;
		
		aTADOFieldData.wSize = pTDataInfo->wSize;
		aTADOFieldData.pBuffer = (PCHAR)pTDataInfo + sizeof(TDataInfo) + pTDataInfo->wColNameLen;
		
		pTDataInfo = (TDataInfo*)((PCHAR)pTDataInfo + sizeof(TDataInfo) + pTDataInfo->wColNameLen + pTDataInfo->wSize);
	}
	
	return TRUE;
}

BOOL CADODataBuilder::GetVariant(TADOFieldData* pTADOFieldData, _variant_t* pVariant)
{
	if (!pTADOFieldData || !pVariant)
	{
		return FALSE;
	}

	switch (pTADOFieldData->wVT)
	{
	case VT_I2:
		{
			if (pTADOFieldData->wSize != sizeof(short))
			{
				return FALSE;
			}

			short* pValue = (short*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_I4:
		{
			if (pTADOFieldData->wSize != sizeof(long))
			{
				return FALSE;
			}

			long* pValue = (long*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_R4:
		{		
			if (pTADOFieldData->wSize != sizeof(float))
			{
				return FALSE;
			}

			float* pValue = (float*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_R8:
		{
			if (pTADOFieldData->wSize != sizeof(double))
			{
				return FALSE;
			}

			double* pValue = (double*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_CY:
		{
			if (pTADOFieldData->wSize != sizeof(CY))
			{
				return FALSE;
			}

			CY* pValue = (CY*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_DATE:
		{		
			if (pTADOFieldData->wSize != sizeof(DATE))
			{
				return FALSE;
			}

			DATE* pValue = (DATE*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_BSTR:
		{
			WCHAR wzStrTemp[10240] = {0};

			memcpy(wzStrTemp, pTADOFieldData->pBuffer, pTADOFieldData->wSize > 10240 ? 10239 : pTADOFieldData->wSize);

			_bstr_t strTemp = wzStrTemp;

			*pVariant = strTemp;
		}	
		break;			
	case VT_BOOL:
		{
			if (pTADOFieldData->wSize != sizeof(bool))
			{
				return FALSE;
			}

			bool* pValue = (bool*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_DECIMAL:
		{		
			if (pTADOFieldData->wSize != sizeof(DECIMAL))
			{
				return FALSE;
			}

			DECIMAL* pValue = (DECIMAL*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case VT_UI1:
		{			
			if (pTADOFieldData->wSize != sizeof(BYTE))
			{
				return FALSE;
			}

			BYTE* pValue = (BYTE*)pTADOFieldData->pBuffer;

			*pVariant = *pValue;
		}
		break;
	case (VT_ARRAY | VT_UI1)://8209
		{
			long lngOffset = 0;
			UCHAR chData;
			SAFEARRAY FAR *psa = NULL;
			SAFEARRAYBOUND rgsabound[1];

			//Create a safe array to store the array of BYTES 
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = pTADOFieldData->wSize;
			psa = SafeArrayCreate(VT_UI1,1,rgsabound);
			if (!psa)
			{
				return FALSE;
			}

			while(lngOffset < (long)pTADOFieldData->wSize)
			{
				chData = ((UCHAR*)pTADOFieldData->pBuffer)[lngOffset];
				SafeArrayPutElement(psa, &lngOffset, &chData);
				lngOffset++;
			}

			//Assign the Safe array  to a variant. 
			pVariant->parray = psa;
		}
		break;
	case VT_NULL:
		{

		}
		break;
	default:
		{
			return FALSE;
		}
	}

	pVariant->vt = pTADOFieldData->wVT;

	return TRUE;
}