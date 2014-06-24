#ifndef DSS_ADO_DATA_BUILDER_H
#define DSS_ADO_DATA_BUILDER_H

#include "../BaseClass_V3/include/util/adoDatabase.h"

#include <Windows.h>

GV_USING

struct TADOFieldData
{
	WORD wADOType;			//ADO的数据类型
	WORD wVT;				//_variant_t中的VT（数据类型）
	CHAR szColName[50];		//列名称
	WORD wSize;				//数据的实际大小
	PCHAR pBuffer;			//实际数据的首地址
};

typedef vector<TADOFieldData> ADOFieldDataArray;

class CADODataBuilder
{
public:
	static DWORD DataBuilder(CADORecordset& recordset, PCHAR pBuffer, DWORD dwSize);
	
	static DWORD DataBuilderByColName(CADORecordset& recordset, PCHAR pBuffer, DWORD dwSize, vector<string>& fieldNameArray);
	
	//pBuffer 的数据在 aADOFieldDataArray 失效前不能失效，否则会发生内存问题
	static BOOL GetFieldArray(PCHAR pBuffer, DWORD dwSize, ADOFieldDataArray& aADOFieldDataArray);
	
	static BOOL GetVariant(TADOFieldData* pTADOFieldData, _variant_t* pVariant);
};

#endif