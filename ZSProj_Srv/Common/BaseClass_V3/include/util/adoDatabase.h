/*******************************************************
*    ado 
*
*
********************************************************/
#ifndef __ADODATABASE_H__
#define __ADODATABASE_H__

#include <math.h>
#include <atlconv.h>
#include "icrsint.h"
#include "AdoHelper.h"
#include "../config.h"

#pragma warning (disable: 4146)
#pragma warning (default: 4146)

#import "C:\Program Files\Common Files\System\ADO\msado15.dll" rename("EOF", "EndOfFile")
using namespace ADODB;
//
GV_BEGIN
//
class CADOCommand;

struct CADOFieldInfo
{
	Char  m_strName[30]; 
	Short m_nType;
	Long m_lSize; 
	Long m_lDefinedSize;
	Long m_lAttributes;
	Short m_nOrdinalPosition;
	Bool m_bRequired;   
	Bool m_bAllowZeroLength; 
	Long m_lCollatingOrder;  
};
//
/*---------------------------------------------------
*
---------------------------------------------------*/
class CADODatabase
{
public:
	enum cadoConnectModeEnum
    {	
		connectModeUnknown = adModeUnknown,
		connectModeRead = adModeRead,
		connectModeWrite = adModeWrite,
		connectModeReadWrite = adModeReadWrite,
		connectModeShareDenyRead = adModeShareDenyRead,
		connectModeShareDenyWrite = adModeShareDenyWrite,
		connectModeShareExclusive = adModeShareExclusive,
		connectModeShareDenyNone = adModeShareDenyNone
    };

	CADODatabase();
	virtual ~CADODatabase();
	//
	Bool Open(CChar* pszConnection, CChar* pszUserID = "", CChar* pszPassword = "");
	//connect for security  add by gavin on 2013.4.16
	Bool Open_Security(LPCSTR pszConnection, LPCSTR pszUserID = "", LPCSTR pszPassword = "");
	//
	_ConnectionPtr GetActiveConnection() {return m_pConnection;};
	//
	Bool Execute(CChar* pszExec);
	//
	Int GetRecordsAffected() {return m_nRecordsAffected;};
	//
	ULong GetRecordCount(_RecordsetPtr m_pRs);
	//
	Long BeginTransaction()  {return m_pConnection->BeginTrans();};
	//
	Long CommitTransaction() {return m_pConnection->CommitTrans();};
	//
	Long RollbackTransaction() {return m_pConnection->RollbackTrans();};
	//
	Bool IsOpen();
	//
	Void Close();
	//
	Void SetConnectionMode(cadoConnectModeEnum nMode) {m_pConnection->PutMode((enum ConnectModeEnum)nMode);};
	//
	Void SetConnectionString(CChar* pszConnection);
	//
	Void GetConnectionString(Char*   pszConnect,Int  nMaxLen);
	//
	Void GetLastErrorString(Char*   pszError,Int  nMaxLen);
	//
	ULong GetLastError() { return m_ulLastError; }
	//
	Void GetErrorDescription(Char*   pszError,Int  nMaxLen);
	//
	Void SetConnectionTimeout(Long nConnectionTimeout = 30) { m_nConnectionTimeout = nConnectionTimeout; }
protected:
	Void dump_com_error(_com_error &e);
public:
	_ConnectionPtr m_pConnection;
protected:
	string m_strConnection;
	string m_strLastError;
	string m_strErrorDescription;
	ULong m_ulLastError;
	Int m_nRecordsAffected;
	Long m_nConnectionTimeout;
};

/*---------------------------------------------------
*
---------------------------------------------------*/
class CADORecordset
{
public:
	Bool Clone(CADORecordset& pRs);
	
	enum cadoOpenEnum
	{
		openUnknown = 0,
		openQuery = 1,
		openTable = 2,
		openStoredProc = 3
	};

	enum cadoEditEnum
	{
		dbEditNone = 0,
		dbEditNew = 1,
		dbEdit = 2
	};
	
	enum cadoPositionEnum
	{
	
		positionUnknown = -1,
		positionBOF = -2,
		positionEOF = -3
	};
	
	enum cadoSearchEnum
	{	
		searchForward = 1,
		searchBackward = -1
	};

	enum cadoDataType
	{
		typeEmpty = ADODB::adEmpty,
		typeTinyInt = ADODB::adTinyInt,
		typeSmallInt = ADODB::adSmallInt,
		typeInteger = ADODB::adInteger,
		typeBigInt = ADODB::adBigInt,
		typeUnsignedTinyInt = ADODB::adUnsignedTinyInt,
		typeUnsignedSmallInt = ADODB::adUnsignedSmallInt,
		typeUnsignedInt = ADODB::adUnsignedInt,
		typeUnsignedBigInt = ADODB::adUnsignedBigInt,
		typeSingle = ADODB::adSingle,
		typeDouble = ADODB::adDouble,
		typeCurrency = ADODB::adCurrency,
		typeDecimal = ADODB::adDecimal,
		typeNumeric = ADODB::adNumeric,
		typeBoolean = ADODB::adBoolean,
		typeError = ADODB::adError,
		typeUserDefined = ADODB::adUserDefined,
		typeVariant = ADODB::adVariant,
		typeIDispatch = ADODB::adIDispatch,
		typeIUnknown = ADODB::adIUnknown,
		typeGUID = ADODB::adGUID,
		typeDate = ADODB::adDate,
		typeDBDate = ADODB::adDBDate,
		typeDBTime = ADODB::adDBTime,
		typeDBTimeStamp = ADODB::adDBTimeStamp,
		typeBSTR = ADODB::adBSTR,
		typeChar = ADODB::adChar,
		typeVarChar = ADODB::adVarChar,
		typeLongVarChar = ADODB::adLongVarChar,
		typeWChar = ADODB::adWChar,
		typeVarWChar = ADODB::adVarWChar,
		typeLongVarWChar = ADODB::adLongVarWChar,
		typeBinary = ADODB::adBinary,
		typeVarBinary = ADODB::adVarBinary,
		typeLongVarBinary = ADODB::adLongVarBinary,
		typeChapter = ADODB::adChapter,
		typeFileTime = ADODB::adFileTime,
		typePropVariant = ADODB::adPropVariant,
		typeVarNumeric = ADODB::adVarNumeric,
		typeArray = ADODB::adVariant
	};
	
	enum cadoSchemaType 
	{
		schemaSpecific = adSchemaProviderSpecific,	
		schemaAsserts = adSchemaAsserts,
		schemaCatalog = adSchemaCatalogs,
		schemaCharacterSet = adSchemaCharacterSets,
		schemaCollections = adSchemaCollations,
		schemaColumns = adSchemaColumns,
		schemaConstraints = adSchemaCheckConstraints,
		schemaConstraintColumnUsage = adSchemaConstraintColumnUsage,
		schemaConstraintTableUsage  = adSchemaConstraintTableUsage,
		shemaKeyColumnUsage = adSchemaKeyColumnUsage,
		schemaTableConstraints = adSchemaTableConstraints,
		schemaColumnsDomainUsage = adSchemaColumnsDomainUsage,
		schemaIndexes = adSchemaIndexes,
		schemaColumnPrivileges = adSchemaColumnPrivileges,
		schemaTablePrivileges = adSchemaTablePrivileges,
		schemaUsagePrivileges = adSchemaUsagePrivileges,
		schemaProcedures = adSchemaProcedures,
		schemaTables =adSchemaTables,
		schemaProviderTypes = adSchemaProviderTypes,
		schemaViews = adSchemaViews,
		schemaProcedureParameters = adSchemaProcedureParameters,
		schemaForeignKeys = adSchemaForeignKeys,
		schemaPrimaryKeys = adSchemaPrimaryKeys,
		schemaProcedureColumns = adSchemaProcedureColumns,
		schemaDBInfoKeywords = adSchemaDBInfoKeywords,
		schemaDBInfoLiterals = adSchemaDBInfoLiterals,
		schemaCubes = adSchemaCubes,
		schemaDimensions = adSchemaDimensions,
		schemaHierarchies  = adSchemaHierarchies, 
		schemaLevels = adSchemaLevels,
		schemaMeasures = adSchemaMeasures,
		schemaProperties = adSchemaProperties,
		schemaMembers = adSchemaMembers,
	}; 
	
	//
	CADORecordset();
	//
	CADORecordset(CADODatabase* pAdoDatabase);
	//
	virtual ~CADORecordset();
	//
	template <typename T> Bool SetFieldValue(Int index, T value);
	//
	template <typename T> Bool SetFieldValue(CChar* szName, T value);
	//	
	template <typename T> Bool GetFieldValue(Int index, T& value);
	//
	template <typename T> Bool GetFieldValue(CChar* szName, T& value);	
	//----string ------------
	Bool SetFieldValue(Int nIndex, CChar* pszValue);
	//
	Bool SetFieldValue(CChar* pszFieldName, CChar* pszValue);
	//
	Bool GetFieldValue(CChar*  pszFieldName, Char*  pszValue,Int nMaxLen);
	//
	Bool GetFieldValue(Int nIndex, Char*  pszValue,Int nMaxLen);
	//
	Bool SetFieldValue(Int nIndex, _variant_t vtValue);
	//
	Bool SetFieldValue(CChar* pszFieldName, _variant_t vtValue);
	//
	Bool GetFieldValue(Int nIndex, _variant_t& vtValue);
	//
	Bool GetFieldValue(CChar*  pszFieldName, _variant_t& vtValue);
	//
	Bool SetFieldEmpty(Int nIndex);
	//
	Bool SetFieldEmpty(CChar* pszFieldName);
	//
	Void CancelUpdate();
	//
	Bool Update();
	//
	Void Edit();
	//
	Bool AddNew();
	//
	Bool AddNew(CADORecordBinding &pAdoRecordBinding);
	//
	Bool Find(CChar* pszFind, Int nSearchDirection = CADORecordset::searchForward);
	//
	Bool FindFirst(CChar* pszFind);
	//
	Bool FindNext();
	//
	Void GetQuery(Char*   pszQuery,Long  nMaxLen);
	//
	Void SetQuery(CChar*  pszQuery);
	//
	Bool RecordBinding(CADORecordBinding &pAdoRecordBinding);
	//
	ULong GetRecordCount();
	//
	Bool IsOpen();
	//
	Void Close();
	//
	Bool Open(_ConnectionPtr mpdb, CChar* pszExec= "", Int nOption = CADORecordset::openUnknown);
	//
	Bool Open(CChar* pszExec= "", Int nOption = CADORecordset::openUnknown);
	//
	Bool OpenSchema(Int nSchema, CChar*  pszSchema = "");
	///
	Long GetFieldCount() {return m_pRecordset->Fields->GetCount();};
	//	
	Bool IsFieldNull(CChar*  pszFieldName);
	Bool IsFieldNull(Int nIndex);
	Bool IsFieldEmpty(CChar*  pszFieldName);
	Bool IsFieldEmpty(Int nIndex);	

	Bool IsEof() {return m_pRecordset->EndOfFile == VARIANT_TRUE;}
	Bool IsEOF() {return m_pRecordset->EndOfFile == VARIANT_TRUE;}
	Bool IsBof() {return m_pRecordset->BOF == VARIANT_TRUE;}
	Bool IsBOF() {return m_pRecordset->BOF == VARIANT_TRUE;}

	Void MoveFirst() {m_pRecordset->MoveFirst();}
	Void MoveNext()  {m_pRecordset->MoveNext();}
	Void MovePrevious() {m_pRecordset->MovePrevious();}
	Void MoveLast() {m_pRecordset->MoveLast();}

	Long GetAbsolutePage() {return m_pRecordset->GetAbsolutePage();}
	Void SetAbsolutePage(Int nPage) {m_pRecordset->PutAbsolutePage((enum PositionEnum)nPage);}

	Long GetPageCount() {return m_pRecordset->GetPageCount();}
	Long GetPageSize() {return m_pRecordset->GetPageSize();}
	Void SetPageSize(Int nSize) {m_pRecordset->PutPageSize(nSize);}

	Long GetAbsolutePosition() {return m_pRecordset->GetAbsolutePosition();}
	Void SetAbsolutePosition(Int nPosition) {m_pRecordset->PutAbsolutePosition((enum PositionEnum)nPosition);}

	Bool GetFieldInfo(CChar*  pszFieldName, CADOFieldInfo* fldInfo);
	Bool GetFieldInfo(Int nIndex, CADOFieldInfo* fldInfo);

	Bool AppendChunk(CChar*  pszFieldName, Void* lpData, UInt nBytes);
	Bool AppendChunk(Int nIndex, Void* lpData, UInt nBytes);

	Bool GetChunk(CChar*  pszFieldName, Char*  pszValue,Int nMaxLen);
	Bool GetChunk(Int nIndex, Char*  pszValue,Int nMaxLen);
	Bool GetChunk(CChar*  pszFieldName, Void* pData);
	//add by gavin on 2006.8.1
	Bool GetChunk(FieldPtr pField, Void* lpData,Long &nReturnDataLen);
	Bool GetChunk(CChar*  pszFieldName, Void* lpData,Long &nReturnDataLen);
	//add by gavin on 2006.8.1 over
	Bool GetChunk(Int nIndex, Void* pData);

	Void GetString(CChar* pszCols, CChar* pszRows, CChar* pszNull,Char*  pszValue,Int nMaxLen, Long numRows = 0);
	Void GetLastErrorString(Char*   pszError,Int  nMaxLen) 
	{
		if(NULL == pszError || nMaxLen <=0)
			return;
		memcpy(pszError,m_strLastError.c_str(),min((Int)m_strLastError.size(),nMaxLen));
	};
	ULong GetLastError() {return m_dwLastError;};

	Void GetBookmark() {m_varBookmark = m_pRecordset->Bookmark;};
	Bool SetBookmark();

	Bool Delete();
	Bool IsConnectionOpen() {return m_pConnection != NULL && m_pConnection->GetState() != adStateClosed;};
	_RecordsetPtr GetRecordset() {return m_pRecordset;}
	_ConnectionPtr GetActiveConnection() {return m_pConnection;}

	Bool SetFilter(CChar*   pszFileter);
	Bool SetSort(CChar*   pszCriteria);
	Bool SaveAsXML(CChar*   pszXMLFile);
	Bool OpenXML(CChar*   pszXMLFile);
	Bool Execute(CADOCommand* pCommand);
public:
	
	_RecordsetPtr m_pRecordset;
	_CommandPtr m_pCmd;
	
protected:
	_ConnectionPtr m_pConnection;
	Int m_nSearchDirection;
	string m_strFind;
	_variant_t m_varBookFind;
	_variant_t m_varBookmark;
	Int m_nEditStatus;
	string m_strLastError;
	ULong m_dwLastError;
	Void dump_com_error(_com_error &e);
	IADORecordBinding *m_pRecBinding;
	string m_strQuery;
protected:
	Bool GetFieldInfo(FieldPtr pField, CADOFieldInfo* fldInfo);
	Bool GetChunk(FieldPtr pField, Char*  pszValue,Int nMaxLen);
	Bool GetChunk(FieldPtr pField, Void* lpData);
	Bool AppendChunk(FieldPtr pField, Void* lpData, UInt nBytes);
		
};

//
template <typename T>
Bool CADORecordset::SetFieldValue(Int index, T value) 
{
	_variant_t vt;
	if( !T2Variant(value, vt) )
		return false;
	return SetFieldValue(index, vt);
}

template <typename T>
Bool CADORecordset::SetFieldValue(CChar* szName, T value) 
{
	_variant_t vt;
	if( !T2Variant(value, vt) )
		return false;
	return SetFieldValue(szName, vt);
}

template <typename T>
Bool CADORecordset::GetFieldValue(Int index, T& value)
{
	_variant_t vt;
	if( !GetFieldValue(index, vt) )
		return false;
	return Variant2T(vt, value);
}

template <typename T>
Bool CADORecordset::GetFieldValue(CChar* szName, T& value)
{
	_variant_t vt;
	if( !GetFieldValue(szName, vt) )
		return false;
	return Variant2T(vt, value);
}

/*---------------------------------------------------
*
---------------------------------------------------*/
class CADOParameter
{
public:

	enum cadoParameterDirection
	{
		paramUnknown = adParamUnknown,
		paramInput = adParamInput,
		paramOutput = adParamOutput,
		paramInputOutput = adParamInputOutput,
		paramReturnValue = adParamReturnValue 
	};

	CADOParameter(Int nType, Long lSize = 0, Int nDirection = paramInput, CChar*  pszName = "");
	
	virtual ~CADOParameter();

	template <typename T> Bool SetValue(T value)
	{	
		_variant_t vt;
		if( !T2Variant(value, vt) )
			return false;
		return SetValue(vt);
	}

	template <typename T> Bool GetValue(T& value)
	{
		_variant_t vt;
		if( !GetValue(vt) )
			return false;
		return Variant2T(vt, value);
	}

	Bool GetValue(Char*  pszValue,Int  nMaxLen);

	Bool SetValue(_variant_t vtValue);
	Bool GetValue(_variant_t& vtValue);

	Void SetPrecision(Int nPrecision) {m_pParameter->PutPrecision(nPrecision);};
	Void SetScale(Int nScale) {m_pParameter->PutNumericScale(nScale);};

	Void SetName(CChar*   pszName);
	Void GetName(Char*   pszValue,Int nMaxLen);
	Int GetType() {return m_nType;};
	_ParameterPtr GetParameter() {return m_pParameter;};

protected:
	Void dump_com_error(_com_error &e);
protected:
	_ParameterPtr m_pParameter;
	string m_strName;
	Int m_nType;
	string m_strLastError;
	ULong m_dwLastError;
};

/*---------------------------------------------------
*
---------------------------------------------------*/
class CADOCommand
{
public:
	enum cadoCommandType
	{
		typeCmdText = adCmdText,
		typeCmdTable = adCmdTable,
		typeCmdTableDirect = adCmdTableDirect,
		typeCmdStoredProc = adCmdStoredProc,
		typeCmdUnknown = adCmdUnknown,
		typeCmdFile = adCmdFile
	};
	
	CADOCommand(CADODatabase* pAdoDatabase, CChar*  pszCommandText = "", Int nCommandType = typeCmdStoredProc);
		
	virtual ~CADOCommand();

	Void SetTimeout(Long nTimeOut) {m_pCommand->PutCommandTimeout(nTimeOut);};

	Void SetText(CChar*  pszCommandText);
	Void GetText(Char*   pszValue,Int nMaxLen);

	Void SetType(Int nCommandType);
	Int GetType() {return m_nCommandType;};

	Bool AddParameter(CADOParameter* pAdoParameter);
	Bool AddParameter(CChar*  pszName, Int nType, Int nDirection, Long lSize, Int nValue);
	Bool AddParameter(CChar*  pszName, Int nType, Int nDirection, Long lSize, Long lValue);
	Bool AddParameter(CChar*  pszName, Int nType, Int nDirection, Long lSize, double dblValue, Int nPrecision = 0, Int nScale = 0);
	Bool AddParameter(CChar*  pszName, Int nType, Int nDirection, Long lSize, string strValue);
	Bool AddParameter(CChar*  pszName, Int nType, Int nDirection, Long lSize, time_t time);
	Bool AddParameter(CChar*  pszName, Int nType, Int nDirection, Long lSize, _variant_t vtValue, Int nPrecision = 0, Int nScale = 0);

	Bool Execute();
	Int GetRecordsAffected() {return m_nRecordsAffected;};
	_CommandPtr GetCommand() {return m_pCommand;};
	
	Void GetLastErrorString(Char*  pszValue,Int nMaxLen);
protected:
	Void dump_com_error(_com_error &e);

protected:
	_CommandPtr m_pCommand;
	Int m_nCommandType;
	Int m_nRecordsAffected;
	string m_strCommandText;
	string m_strLastError;
	ULong m_dwLastError;
};


GV_END

#endif 
