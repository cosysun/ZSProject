#ifndef DSS_ADO_DATA_BUILDER_H
#define DSS_ADO_DATA_BUILDER_H

#include "../BaseClass_V3/include/util/adoDatabase.h"

#include <Windows.h>

GV_USING

struct TADOFieldData
{
	WORD wADOType;			//ADO����������
	WORD wVT;				//_variant_t�е�VT���������ͣ�
	CHAR szColName[50];		//������
	WORD wSize;				//���ݵ�ʵ�ʴ�С
	PCHAR pBuffer;			//ʵ�����ݵ��׵�ַ
};

typedef vector<TADOFieldData> ADOFieldDataArray;

class CADODataBuilder
{
public:
	static DWORD DataBuilder(CADORecordset& recordset, PCHAR pBuffer, DWORD dwSize);
	
	static DWORD DataBuilderByColName(CADORecordset& recordset, PCHAR pBuffer, DWORD dwSize, vector<string>& fieldNameArray);
	
	//pBuffer �������� aADOFieldDataArray ʧЧǰ����ʧЧ������ᷢ���ڴ�����
	static BOOL GetFieldArray(PCHAR pBuffer, DWORD dwSize, ADOFieldDataArray& aADOFieldDataArray);
	
	static BOOL GetVariant(TADOFieldData* pTADOFieldData, _variant_t* pVariant);
};

#endif