//
// sproxy.exe generated file
// do not modify this file
//
// Created: 08/08/2013@16:18:27
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace LocationData
{

struct LocationDataInfo
{
	BSTR DEVICE_CODE;
	int USER_TYPE;
	BSTR USER_CODE;
	BSTR GPS_DATETIME;
	BSTR GPSAV;
	BSTR LONG_WE;
	double LONGITUDE;
	BSTR LAT_NS;
	double LATITUDE;
	int SPEED;
	int DIRECTION;
	BSTR LOCATION;
	BSTR STATE;
};

struct DeviceInfoClass
{
	int DEVICE_ID;
	BSTR DEVICE_CODE;
	BSTR DEPT_NO;
	BSTR PUBLIC_CODE;
	BSTR SPECIAL_CODE;
	BSTR PRODUCER;
	BSTR DEVICE_TYPE;
	BSTR STATE;
	BSTR ONLINE_STATE;
	BSTR VALIDE_DATE;
	BSTR USER_TYPE;
	BSTR USER_CODE;
	BSTR REMARK;
};

struct LocationSpecialDataInfo
{
	BSTR SPECIAL_CODE;
	BSTR GPS_DATETIME;
	BSTR GPSAV;
	BSTR LONG_WE;
	double LONGITUDE;
	BSTR LAT_NS;
	double LATITUDE;
	int SPEED;
	int DIRECTION;
	BSTR LOCATION;
	BSTR STATE;
};

template <typename TClient = CSoapSocketClientT<> >
class CLocationDataT : 
	public TClient, 
	public CSoapRootHandler
{
protected:

	const _soapmap ** GetFunctionMap();
	const _soapmap ** GetHeaderMap();
	void * GetHeaderValue();
	const wchar_t * GetNamespaceUri();
	const char * GetServiceName();
	const char * GetNamespaceUriA();
	HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem);
	HRESULT GetClientReader(ISAXXMLReader **ppReader);

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	CLocationDataT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://192.168.1.139/HytDbWs/LocationData.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CLocationDataT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT HYTPGIS_DynamicGroup_QueryByGID(
		int GID, 
		BSTR* HYTPGIS_DynamicGroup_QueryByGIDResult
	);

	HRESULT HYTPGIS_AlarmDevice_Add(
		int AlarmID, 
		BSTR szDeviceCode, 
		BSTR szChunnel, 
		bool* HYTPGIS_AlarmDevice_AddResult
	);

	HRESULT HYTPGIS_DeviceQuery_BySpecialCode(
		BSTR szSpecialCode, 
		bool* HYTPGIS_DeviceQuery_BySpecialCodeResult, 
		DeviceInfoClass* deviceClass
	);

	HRESULT HYTPGIS_DeviceTable_Query(
		BSTR szDeviceCode, 
		bool* HYTPGIS_DeviceTable_QueryResult, 
		DeviceInfoClass* deviceClass
	);

	HRESULT HYTPGIS_LocateSpecialDataAdd(
		LocationSpecialDataInfo locationSpecialDataInfo, 
		int* HYTPGIS_LocateSpecialDataAddResult
	);

	HRESULT HYTPGIS_DeviceModify_OnLineStatus(
		BSTR DEVICE_CODE, 
		BSTR OnLineStatus, 
		bool* HYTPGIS_DeviceModify_OnLineStatusResult
	);

	HRESULT HYTPGIS_DynamicGroup_QueryMember(
		BSTR szDeviceCode, 
		BSTR* HYTPGIS_DynamicGroup_QueryMemberResult
	);

	HRESULT HYTPGIS_Group_QueryDeviceGroup(
		BSTR szDeviceCode, 
		BSTR* HYTPGIS_Group_QueryDeviceGroupResult
	);

	HRESULT HYTPGIS_Group_ModifyDeviceState(
		BSTR szDeviceCode, 
		int state, 
		bool* HYTPGIS_Group_ModifyDeviceStateResult
	);

	HRESULT HYTPGIS_Test_Dataset(
		BSTR* HYTPGIS_Test_DatasetResult
	);

	HRESULT HYTPGIS_PatroAreaUser_QueryAllDevice(
		BSTR* HYTPGIS_PatroAreaUser_QueryAllDeviceResult
	);

	HRESULT HYTPGIS_DeviceUpdate_OffLine(
		bool* HYTPGIS_DeviceUpdate_OffLineResult
	);

	HRESULT HYTPGIS_LocateDataAdd(
		LocationDataInfo locationDataInfo, 
		int* HYTPGIS_LocateDataAddResult
	);

	HRESULT HelloWorld(
		BSTR* HelloWorldResult
	);

	HRESULT HYTPGIS_LocationData_ModifyByLocation(
		__int64 GpsID, 
		BSTR szDeviceCode, 
		BSTR szLocation, 
		bool* HYTPGIS_LocationData_ModifyByLocationResult
	);
};

typedef CLocationDataT<> CLocationData;

__if_not_exists(__LocationDataInfo_entries)
{
extern __declspec(selectany) const _soapmapentry __LocationDataInfo_entries[] =
{
	{ 
		0xD15BF5EA, 
		"DEVICE_CODE", 
		L"DEVICE_CODE", 
		sizeof("DEVICE_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, DEVICE_CODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCC499AC0, 
		"USER_TYPE", 
		L"USER_TYPE", 
		sizeof("USER_TYPE")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(LocationDataInfo, USER_TYPE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCC401C39, 
		"USER_CODE", 
		L"USER_CODE", 
		sizeof("USER_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, USER_CODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE019C036, 
		"GPS_DATETIME", 
		L"GPS_DATETIME", 
		sizeof("GPS_DATETIME")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, GPS_DATETIME),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x053213E1, 
		"GPSAV", 
		L"GPSAV", 
		sizeof("GPSAV")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, GPSAV),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x983D98CB, 
		"LONG_WE", 
		L"LONG_WE", 
		sizeof("LONG_WE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, LONG_WE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9C77846B, 
		"LONGITUDE", 
		L"LONGITUDE", 
		sizeof("LONGITUDE")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_FIELD, 
		offsetof(LocationDataInfo, LONGITUDE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB60FF6E1, 
		"LAT_NS", 
		L"LAT_NS", 
		sizeof("LAT_NS")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, LAT_NS),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x785E73DC, 
		"LATITUDE", 
		L"LATITUDE", 
		sizeof("LATITUDE")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_FIELD, 
		offsetof(LocationDataInfo, LATITUDE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x060AFED1, 
		"SPEED", 
		L"SPEED", 
		sizeof("SPEED")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(LocationDataInfo, SPEED),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9DDADC41, 
		"DIRECTION", 
		L"DIRECTION", 
		sizeof("DIRECTION")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(LocationDataInfo, DIRECTION),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x85D51E19, 
		"LOCATION", 
		L"LOCATION", 
		sizeof("LOCATION")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, LOCATION),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x060D2141, 
		"STATE", 
		L"STATE", 
		sizeof("STATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationDataInfo, STATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __LocationDataInfo_map =
{
	0x19DC07DF,
	"LocationDataInfo",
	L"LocationDataInfo",
	sizeof("LocationDataInfo")-1,
	sizeof("LocationDataInfo")-1,
	SOAPMAP_STRUCT,
	__LocationDataInfo_entries,
	sizeof(LocationDataInfo),
	13,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__DeviceInfoClass_entries)
{
extern __declspec(selectany) const _soapmapentry __DeviceInfoClass_entries[] =
{
	{ 
		0x2A09415C, 
		"DEVICE_ID", 
		L"DEVICE_ID", 
		sizeof("DEVICE_ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(DeviceInfoClass, DEVICE_ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD15BF5EA, 
		"DEVICE_CODE", 
		L"DEVICE_CODE", 
		sizeof("DEVICE_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, DEVICE_CODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x19436D49, 
		"DEPT_NO", 
		L"DEPT_NO", 
		sizeof("DEPT_NO")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, DEPT_NO),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC69D4CD9, 
		"PUBLIC_CODE", 
		L"PUBLIC_CODE", 
		sizeof("PUBLIC_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, PUBLIC_CODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC1BEFB3B, 
		"SPECIAL_CODE", 
		L"SPECIAL_CODE", 
		sizeof("SPECIAL_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, SPECIAL_CODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x39FFCA84, 
		"PRODUCER", 
		L"PRODUCER", 
		sizeof("PRODUCER")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, PRODUCER),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD1657471, 
		"DEVICE_TYPE", 
		L"DEVICE_TYPE", 
		sizeof("DEVICE_TYPE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, DEVICE_TYPE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x060D2141, 
		"STATE", 
		L"STATE", 
		sizeof("STATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, STATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC720CB65, 
		"ONLINE_STATE", 
		L"ONLINE_STATE", 
		sizeof("ONLINE_STATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, ONLINE_STATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE1D49592, 
		"VALIDE_DATE", 
		L"VALIDE_DATE", 
		sizeof("VALIDE_DATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, VALIDE_DATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCC499AC0, 
		"USER_TYPE", 
		L"USER_TYPE", 
		sizeof("USER_TYPE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, USER_TYPE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCC401C39, 
		"USER_CODE", 
		L"USER_CODE", 
		sizeof("USER_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, USER_CODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC452F6E2, 
		"REMARK", 
		L"REMARK", 
		sizeof("REMARK")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeviceInfoClass, REMARK),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DeviceInfoClass_map =
{
	0x0354BF72,
	"DeviceInfoClass",
	L"DeviceInfoClass",
	sizeof("DeviceInfoClass")-1,
	sizeof("DeviceInfoClass")-1,
	SOAPMAP_STRUCT,
	__DeviceInfoClass_entries,
	sizeof(DeviceInfoClass),
	13,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

__if_not_exists(__LocationSpecialDataInfo_entries)
{
extern __declspec(selectany) const _soapmapentry __LocationSpecialDataInfo_entries[] =
{
	{ 
		0xC1BEFB3B, 
		"SPECIAL_CODE", 
		L"SPECIAL_CODE", 
		sizeof("SPECIAL_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationSpecialDataInfo, SPECIAL_CODE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE019C036, 
		"GPS_DATETIME", 
		L"GPS_DATETIME", 
		sizeof("GPS_DATETIME")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationSpecialDataInfo, GPS_DATETIME),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x053213E1, 
		"GPSAV", 
		L"GPSAV", 
		sizeof("GPSAV")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationSpecialDataInfo, GPSAV),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x983D98CB, 
		"LONG_WE", 
		L"LONG_WE", 
		sizeof("LONG_WE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationSpecialDataInfo, LONG_WE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9C77846B, 
		"LONGITUDE", 
		L"LONGITUDE", 
		sizeof("LONGITUDE")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_FIELD, 
		offsetof(LocationSpecialDataInfo, LONGITUDE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB60FF6E1, 
		"LAT_NS", 
		L"LAT_NS", 
		sizeof("LAT_NS")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationSpecialDataInfo, LAT_NS),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x785E73DC, 
		"LATITUDE", 
		L"LATITUDE", 
		sizeof("LATITUDE")-1, 
		SOAPTYPE_DOUBLE, 
		SOAPFLAG_FIELD, 
		offsetof(LocationSpecialDataInfo, LATITUDE),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x060AFED1, 
		"SPEED", 
		L"SPEED", 
		sizeof("SPEED")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(LocationSpecialDataInfo, SPEED),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9DDADC41, 
		"DIRECTION", 
		L"DIRECTION", 
		sizeof("DIRECTION")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(LocationSpecialDataInfo, DIRECTION),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x85D51E19, 
		"LOCATION", 
		L"LOCATION", 
		sizeof("LOCATION")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationSpecialDataInfo, LOCATION),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x060D2141, 
		"STATE", 
		L"STATE", 
		sizeof("STATE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LocationSpecialDataInfo, STATE),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __LocationSpecialDataInfo_map =
{
	0x0D245FC0,
	"LocationSpecialDataInfo",
	L"LocationSpecialDataInfo",
	sizeof("LocationSpecialDataInfo")-1,
	sizeof("LocationSpecialDataInfo")-1,
	SOAPMAP_STRUCT,
	__LocationSpecialDataInfo_entries,
	sizeof(LocationSpecialDataInfo),
	11,
	-1,
	SOAPFLAG_NONE,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};
}

struct __CLocationData_HYTPGIS_DynamicGroup_QueryByGID_struct
{
	int GID;
	BSTR HYTPGIS_DynamicGroup_QueryByGIDResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_DynamicGroup_QueryByGID_entries[] =
{

	{
		0x000137B4, 
		"GID", 
		L"GID", 
		sizeof("GID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_struct, GID),
		NULL,
		NULL,
		-1,
	},
	{
		0x1B43315C, 
		"HYTPGIS_DynamicGroup_QueryByGIDResult", 
		L"HYTPGIS_DynamicGroup_QueryByGIDResult", 
		sizeof("HYTPGIS_DynamicGroup_QueryByGIDResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_struct, HYTPGIS_DynamicGroup_QueryByGIDResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_DynamicGroup_QueryByGID_map =
{
	0xF87C18AC,
	"HYTPGIS_DynamicGroup_QueryByGID",
	L"HYTPGIS_DynamicGroup_QueryByGIDResponse",
	sizeof("HYTPGIS_DynamicGroup_QueryByGID")-1,
	sizeof("HYTPGIS_DynamicGroup_QueryByGIDResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_entries,
	sizeof(__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_AlarmDevice_Add_struct
{
	int AlarmID;
	BSTR szDeviceCode;
	BSTR szChunnel;
	bool HYTPGIS_AlarmDevice_AddResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_AlarmDevice_Add_entries[] =
{

	{
		0x8E92365A, 
		"AlarmID", 
		L"AlarmID", 
		sizeof("AlarmID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_AlarmDevice_Add_struct, AlarmID),
		NULL,
		NULL,
		-1,
	},
	{
		0xFA9C8498, 
		"szDeviceCode", 
		L"szDeviceCode", 
		sizeof("szDeviceCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_AlarmDevice_Add_struct, szDeviceCode),
		NULL,
		NULL,
		-1,
	},
	{
		0x0BE0049A, 
		"szChunnel", 
		L"szChunnel", 
		sizeof("szChunnel")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_AlarmDevice_Add_struct, szChunnel),
		NULL,
		NULL,
		-1,
	},
	{
		0x7DBBADEB, 
		"HYTPGIS_AlarmDevice_AddResult", 
		L"HYTPGIS_AlarmDevice_AddResult", 
		sizeof("HYTPGIS_AlarmDevice_AddResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_AlarmDevice_Add_struct, HYTPGIS_AlarmDevice_AddResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_AlarmDevice_Add_map =
{
	0xDB05F4FB,
	"HYTPGIS_AlarmDevice_Add",
	L"HYTPGIS_AlarmDevice_AddResponse",
	sizeof("HYTPGIS_AlarmDevice_Add")-1,
	sizeof("HYTPGIS_AlarmDevice_AddResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_AlarmDevice_Add_entries,
	sizeof(__CLocationData_HYTPGIS_AlarmDevice_Add_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_struct
{
	BSTR szSpecialCode;
	bool HYTPGIS_DeviceQuery_BySpecialCodeResult;
	DeviceInfoClass deviceClass;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_entries[] =
{

	{
		0xCC983989, 
		"szSpecialCode", 
		L"szSpecialCode", 
		sizeof("szSpecialCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_struct, szSpecialCode),
		NULL,
		NULL,
		-1,
	},
	{
		0x92A20822, 
		"HYTPGIS_DeviceQuery_BySpecialCodeResult", 
		L"HYTPGIS_DeviceQuery_BySpecialCodeResult", 
		sizeof("HYTPGIS_DeviceQuery_BySpecialCodeResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_struct, HYTPGIS_DeviceQuery_BySpecialCodeResult),
		NULL,
		NULL,
		-1,
	},
	{
		0xFE164AE6, 
		"deviceClass", 
		L"deviceClass", 
		sizeof("deviceClass")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_struct, deviceClass),
		NULL,
		&__DeviceInfoClass_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_map =
{
	0xC2EBB8F2,
	"HYTPGIS_DeviceQuery_BySpecialCode",
	L"HYTPGIS_DeviceQuery_BySpecialCodeResponse",
	sizeof("HYTPGIS_DeviceQuery_BySpecialCode")-1,
	sizeof("HYTPGIS_DeviceQuery_BySpecialCodeResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_entries,
	sizeof(__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_DeviceTable_Query_struct
{
	BSTR szDeviceCode;
	bool HYTPGIS_DeviceTable_QueryResult;
	DeviceInfoClass deviceClass;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_DeviceTable_Query_entries[] =
{

	{
		0xFA9C8498, 
		"szDeviceCode", 
		L"szDeviceCode", 
		sizeof("szDeviceCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_DeviceTable_Query_struct, szDeviceCode),
		NULL,
		NULL,
		-1,
	},
	{
		0x7C70CA33, 
		"HYTPGIS_DeviceTable_QueryResult", 
		L"HYTPGIS_DeviceTable_QueryResult", 
		sizeof("HYTPGIS_DeviceTable_QueryResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_DeviceTable_Query_struct, HYTPGIS_DeviceTable_QueryResult),
		NULL,
		NULL,
		-1,
	},
	{
		0xFE164AE6, 
		"deviceClass", 
		L"deviceClass", 
		sizeof("deviceClass")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_DeviceTable_Query_struct, deviceClass),
		NULL,
		&__DeviceInfoClass_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_DeviceTable_Query_map =
{
	0x5B734343,
	"HYTPGIS_DeviceTable_Query",
	L"HYTPGIS_DeviceTable_QueryResponse",
	sizeof("HYTPGIS_DeviceTable_Query")-1,
	sizeof("HYTPGIS_DeviceTable_QueryResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_DeviceTable_Query_entries,
	sizeof(__CLocationData_HYTPGIS_DeviceTable_Query_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_LocateSpecialDataAdd_struct
{
	LocationSpecialDataInfo locationSpecialDataInfo;
	int HYTPGIS_LocateSpecialDataAddResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_LocateSpecialDataAdd_entries[] =
{

	{
		0x13D837E0, 
		"locationSpecialDataInfo", 
		L"locationSpecialDataInfo", 
		sizeof("locationSpecialDataInfo")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_LocateSpecialDataAdd_struct, locationSpecialDataInfo),
		NULL,
		&__LocationSpecialDataInfo_map,
		-1,
	},
	{
		0x279E2C42, 
		"HYTPGIS_LocateSpecialDataAddResult", 
		L"HYTPGIS_LocateSpecialDataAddResult", 
		sizeof("HYTPGIS_LocateSpecialDataAddResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_LocateSpecialDataAdd_struct, HYTPGIS_LocateSpecialDataAddResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_LocateSpecialDataAdd_map =
{
	0x87816512,
	"HYTPGIS_LocateSpecialDataAdd",
	L"HYTPGIS_LocateSpecialDataAddResponse",
	sizeof("HYTPGIS_LocateSpecialDataAdd")-1,
	sizeof("HYTPGIS_LocateSpecialDataAddResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_LocateSpecialDataAdd_entries,
	sizeof(__CLocationData_HYTPGIS_LocateSpecialDataAdd_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_DeviceModify_OnLineStatus_struct
{
	BSTR DEVICE_CODE;
	BSTR OnLineStatus;
	bool HYTPGIS_DeviceModify_OnLineStatusResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_DeviceModify_OnLineStatus_entries[] =
{

	{
		0xD15BF5EA, 
		"DEVICE_CODE", 
		L"DEVICE_CODE", 
		sizeof("DEVICE_CODE")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_struct, DEVICE_CODE),
		NULL,
		NULL,
		-1,
	},
	{
		0x10270749, 
		"OnLineStatus", 
		L"OnLineStatus", 
		sizeof("OnLineStatus")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_struct, OnLineStatus),
		NULL,
		NULL,
		-1,
	},
	{
		0xADD99726, 
		"HYTPGIS_DeviceModify_OnLineStatusResult", 
		L"HYTPGIS_DeviceModify_OnLineStatusResult", 
		sizeof("HYTPGIS_DeviceModify_OnLineStatusResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_struct, HYTPGIS_DeviceModify_OnLineStatusResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_DeviceModify_OnLineStatus_map =
{
	0x8A4318F6,
	"HYTPGIS_DeviceModify_OnLineStatus",
	L"HYTPGIS_DeviceModify_OnLineStatusResponse",
	sizeof("HYTPGIS_DeviceModify_OnLineStatus")-1,
	sizeof("HYTPGIS_DeviceModify_OnLineStatusResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_entries,
	sizeof(__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_DynamicGroup_QueryMember_struct
{
	BSTR szDeviceCode;
	BSTR HYTPGIS_DynamicGroup_QueryMemberResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_DynamicGroup_QueryMember_entries[] =
{

	{
		0xFA9C8498, 
		"szDeviceCode", 
		L"szDeviceCode", 
		sizeof("szDeviceCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_DynamicGroup_QueryMember_struct, szDeviceCode),
		NULL,
		NULL,
		-1,
	},
	{
		0x130C8605, 
		"HYTPGIS_DynamicGroup_QueryMemberResult", 
		L"HYTPGIS_DynamicGroup_QueryMemberResult", 
		sizeof("HYTPGIS_DynamicGroup_QueryMemberResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_DynamicGroup_QueryMember_struct, HYTPGIS_DynamicGroup_QueryMemberResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_DynamicGroup_QueryMember_map =
{
	0x07ED3B95,
	"HYTPGIS_DynamicGroup_QueryMember",
	L"HYTPGIS_DynamicGroup_QueryMemberResponse",
	sizeof("HYTPGIS_DynamicGroup_QueryMember")-1,
	sizeof("HYTPGIS_DynamicGroup_QueryMemberResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_DynamicGroup_QueryMember_entries,
	sizeof(__CLocationData_HYTPGIS_DynamicGroup_QueryMember_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_Group_QueryDeviceGroup_struct
{
	BSTR szDeviceCode;
	BSTR HYTPGIS_Group_QueryDeviceGroupResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_Group_QueryDeviceGroup_entries[] =
{

	{
		0xFA9C8498, 
		"szDeviceCode", 
		L"szDeviceCode", 
		sizeof("szDeviceCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_Group_QueryDeviceGroup_struct, szDeviceCode),
		NULL,
		NULL,
		-1,
	},
	{
		0xCA019DA5, 
		"HYTPGIS_Group_QueryDeviceGroupResult", 
		L"HYTPGIS_Group_QueryDeviceGroupResult", 
		sizeof("HYTPGIS_Group_QueryDeviceGroupResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_Group_QueryDeviceGroup_struct, HYTPGIS_Group_QueryDeviceGroupResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_Group_QueryDeviceGroup_map =
{
	0x5086BB35,
	"HYTPGIS_Group_QueryDeviceGroup",
	L"HYTPGIS_Group_QueryDeviceGroupResponse",
	sizeof("HYTPGIS_Group_QueryDeviceGroup")-1,
	sizeof("HYTPGIS_Group_QueryDeviceGroupResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_Group_QueryDeviceGroup_entries,
	sizeof(__CLocationData_HYTPGIS_Group_QueryDeviceGroup_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_Group_ModifyDeviceState_struct
{
	BSTR szDeviceCode;
	int state;
	bool HYTPGIS_Group_ModifyDeviceStateResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_Group_ModifyDeviceState_entries[] =
{

	{
		0xFA9C8498, 
		"szDeviceCode", 
		L"szDeviceCode", 
		sizeof("szDeviceCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_Group_ModifyDeviceState_struct, szDeviceCode),
		NULL,
		NULL,
		-1,
	},
	{
		0x086249E1, 
		"state", 
		L"state", 
		sizeof("state")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_Group_ModifyDeviceState_struct, state),
		NULL,
		NULL,
		-1,
	},
	{
		0x021DB0EB, 
		"HYTPGIS_Group_ModifyDeviceStateResult", 
		L"HYTPGIS_Group_ModifyDeviceStateResult", 
		sizeof("HYTPGIS_Group_ModifyDeviceStateResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_Group_ModifyDeviceState_struct, HYTPGIS_Group_ModifyDeviceStateResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_Group_ModifyDeviceState_map =
{
	0xFFF4B7FB,
	"HYTPGIS_Group_ModifyDeviceState",
	L"HYTPGIS_Group_ModifyDeviceStateResponse",
	sizeof("HYTPGIS_Group_ModifyDeviceState")-1,
	sizeof("HYTPGIS_Group_ModifyDeviceStateResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_Group_ModifyDeviceState_entries,
	sizeof(__CLocationData_HYTPGIS_Group_ModifyDeviceState_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_Test_Dataset_struct
{
	BSTR HYTPGIS_Test_DatasetResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_Test_Dataset_entries[] =
{

	{
		0x9583840B, 
		"HYTPGIS_Test_DatasetResult", 
		L"HYTPGIS_Test_DatasetResult", 
		sizeof("HYTPGIS_Test_DatasetResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_Test_Dataset_struct, HYTPGIS_Test_DatasetResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_Test_Dataset_map =
{
	0x041BD31B,
	"HYTPGIS_Test_Dataset",
	L"HYTPGIS_Test_DatasetResponse",
	sizeof("HYTPGIS_Test_Dataset")-1,
	sizeof("HYTPGIS_Test_DatasetResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_Test_Dataset_entries,
	sizeof(__CLocationData_HYTPGIS_Test_Dataset_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_struct
{
	BSTR HYTPGIS_PatroAreaUser_QueryAllDeviceResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_entries[] =
{

	{
		0xB8109442, 
		"HYTPGIS_PatroAreaUser_QueryAllDeviceResult", 
		L"HYTPGIS_PatroAreaUser_QueryAllDeviceResult", 
		sizeof("HYTPGIS_PatroAreaUser_QueryAllDeviceResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_struct, HYTPGIS_PatroAreaUser_QueryAllDeviceResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_map =
{
	0xFE2DCD12,
	"HYTPGIS_PatroAreaUser_QueryAllDevice",
	L"HYTPGIS_PatroAreaUser_QueryAllDeviceResponse",
	sizeof("HYTPGIS_PatroAreaUser_QueryAllDevice")-1,
	sizeof("HYTPGIS_PatroAreaUser_QueryAllDeviceResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_entries,
	sizeof(__CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_DeviceUpdate_OffLine_struct
{
	bool HYTPGIS_DeviceUpdate_OffLineResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_DeviceUpdate_OffLine_entries[] =
{

	{
		0x03B1AA3B, 
		"HYTPGIS_DeviceUpdate_OffLineResult", 
		L"HYTPGIS_DeviceUpdate_OffLineResult", 
		sizeof("HYTPGIS_DeviceUpdate_OffLineResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_DeviceUpdate_OffLine_struct, HYTPGIS_DeviceUpdate_OffLineResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_DeviceUpdate_OffLine_map =
{
	0xB66C454B,
	"HYTPGIS_DeviceUpdate_OffLine",
	L"HYTPGIS_DeviceUpdate_OffLineResponse",
	sizeof("HYTPGIS_DeviceUpdate_OffLine")-1,
	sizeof("HYTPGIS_DeviceUpdate_OffLineResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_DeviceUpdate_OffLine_entries,
	sizeof(__CLocationData_HYTPGIS_DeviceUpdate_OffLine_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_LocateDataAdd_struct
{
	LocationDataInfo locationDataInfo;
	int HYTPGIS_LocateDataAddResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_LocateDataAdd_entries[] =
{

	{
		0xA080C3FF, 
		"locationDataInfo", 
		L"locationDataInfo", 
		sizeof("locationDataInfo")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_LocateDataAdd_struct, locationDataInfo),
		NULL,
		&__LocationDataInfo_map,
		-1,
	},
	{
		0x128BAA81, 
		"HYTPGIS_LocateDataAddResult", 
		L"HYTPGIS_LocateDataAddResult", 
		sizeof("HYTPGIS_LocateDataAddResult")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_LocateDataAdd_struct, HYTPGIS_LocateDataAddResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_LocateDataAdd_map =
{
	0xE3C76F11,
	"HYTPGIS_LocateDataAdd",
	L"HYTPGIS_LocateDataAddResponse",
	sizeof("HYTPGIS_LocateDataAdd")-1,
	sizeof("HYTPGIS_LocateDataAddResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_LocateDataAdd_entries,
	sizeof(__CLocationData_HYTPGIS_LocateDataAdd_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HelloWorld_struct
{
	BSTR HelloWorldResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HelloWorld_entries[] =
{

	{
		0x0588E51B, 
		"HelloWorldResult", 
		L"HelloWorldResult", 
		sizeof("HelloWorldResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HelloWorld_struct, HelloWorldResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HelloWorld_map =
{
	0x8AFDB82B,
	"HelloWorld",
	L"HelloWorldResponse",
	sizeof("HelloWorld")-1,
	sizeof("HelloWorldResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HelloWorld_entries,
	sizeof(__CLocationData_HelloWorld_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};


struct __CLocationData_HYTPGIS_LocationData_ModifyByLocation_struct
{
	__int64 GpsID;
	BSTR szDeviceCode;
	BSTR szLocation;
	bool HYTPGIS_LocationData_ModifyByLocationResult;
};

extern __declspec(selectany) const _soapmapentry __CLocationData_HYTPGIS_LocationData_ModifyByLocation_entries[] =
{

	{
		0x05442917, 
		"GpsID", 
		L"GpsID", 
		sizeof("GpsID")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_LocationData_ModifyByLocation_struct, GpsID),
		NULL,
		NULL,
		-1,
	},
	{
		0xFA9C8498, 
		"szDeviceCode", 
		L"szDeviceCode", 
		sizeof("szDeviceCode")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_LocationData_ModifyByLocation_struct, szDeviceCode),
		NULL,
		NULL,
		-1,
	},
	{
		0xC623FF46, 
		"szLocation", 
		L"szLocation", 
		sizeof("szLocation")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CLocationData_HYTPGIS_LocationData_ModifyByLocation_struct, szLocation),
		NULL,
		NULL,
		-1,
	},
	{
		0x6AAC8AB4, 
		"HYTPGIS_LocationData_ModifyByLocationResult", 
		L"HYTPGIS_LocationData_ModifyByLocationResult", 
		sizeof("HYTPGIS_LocationData_ModifyByLocationResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CLocationData_HYTPGIS_LocationData_ModifyByLocation_struct, HYTPGIS_LocationData_ModifyByLocationResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CLocationData_HYTPGIS_LocationData_ModifyByLocation_map =
{
	0xC7A12804,
	"HYTPGIS_LocationData_ModifyByLocation",
	L"HYTPGIS_LocationData_ModifyByLocationResponse",
	sizeof("HYTPGIS_LocationData_ModifyByLocation")-1,
	sizeof("HYTPGIS_LocationData_ModifyByLocationResponse")-1,
	SOAPMAP_FUNC,
	__CLocationData_HYTPGIS_LocationData_ModifyByLocation_entries,
	sizeof(__CLocationData_HYTPGIS_LocationData_ModifyByLocation_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};

extern __declspec(selectany) const _soapmap * __CLocationData_funcs[] =
{
	&__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_map,
	&__CLocationData_HYTPGIS_AlarmDevice_Add_map,
	&__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_map,
	&__CLocationData_HYTPGIS_DeviceTable_Query_map,
	&__CLocationData_HYTPGIS_LocateSpecialDataAdd_map,
	&__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_map,
	&__CLocationData_HYTPGIS_DynamicGroup_QueryMember_map,
	&__CLocationData_HYTPGIS_Group_QueryDeviceGroup_map,
	&__CLocationData_HYTPGIS_Group_ModifyDeviceState_map,
	&__CLocationData_HYTPGIS_Test_Dataset_map,
	&__CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_map,
	&__CLocationData_HYTPGIS_DeviceUpdate_OffLine_map,
	&__CLocationData_HYTPGIS_LocateDataAdd_map,
	&__CLocationData_HelloWorld_map,
	&__CLocationData_HYTPGIS_LocationData_ModifyByLocation_map,
	NULL
};

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_DynamicGroup_QueryByGID(
		int GID, 
		BSTR* HYTPGIS_DynamicGroup_QueryByGIDResult
	)
{
    if ( HYTPGIS_DynamicGroup_QueryByGIDResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.GID = GID;

	__atlsoap_hr = SetClientStruct(&__params, 0);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_DynamicGroup_QueryByGID\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_DynamicGroup_QueryByGIDResult = __params.HYTPGIS_DynamicGroup_QueryByGIDResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_AlarmDevice_Add(
		int AlarmID, 
		BSTR szDeviceCode, 
		BSTR szChunnel, 
		bool* HYTPGIS_AlarmDevice_AddResult
	)
{
    if ( HYTPGIS_AlarmDevice_AddResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_AlarmDevice_Add_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.AlarmID = AlarmID;
	__params.szDeviceCode = szDeviceCode;
	__params.szChunnel = szChunnel;

	__atlsoap_hr = SetClientStruct(&__params, 1);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_AlarmDevice_Add\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_AlarmDevice_AddResult = __params.HYTPGIS_AlarmDevice_AddResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_DeviceQuery_BySpecialCode(
		BSTR szSpecialCode, 
		bool* HYTPGIS_DeviceQuery_BySpecialCodeResult, 
		DeviceInfoClass* deviceClass
	)
{
    if ( HYTPGIS_DeviceQuery_BySpecialCodeResult == NULL )
		return E_POINTER;
    if ( deviceClass == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.szSpecialCode = szSpecialCode;

	__atlsoap_hr = SetClientStruct(&__params, 2);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_DeviceQuery_BySpecialCode\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_DeviceQuery_BySpecialCodeResult = __params.HYTPGIS_DeviceQuery_BySpecialCodeResult;
	*deviceClass = __params.deviceClass;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_DeviceTable_Query(
		BSTR szDeviceCode, 
		bool* HYTPGIS_DeviceTable_QueryResult, 
		DeviceInfoClass* deviceClass
	)
{
    if ( HYTPGIS_DeviceTable_QueryResult == NULL )
		return E_POINTER;
    if ( deviceClass == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_DeviceTable_Query_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.szDeviceCode = szDeviceCode;

	__atlsoap_hr = SetClientStruct(&__params, 3);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_DeviceTable_Query\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_DeviceTable_QueryResult = __params.HYTPGIS_DeviceTable_QueryResult;
	*deviceClass = __params.deviceClass;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_LocateSpecialDataAdd(
		LocationSpecialDataInfo locationSpecialDataInfo, 
		int* HYTPGIS_LocateSpecialDataAddResult
	)
{
    if ( HYTPGIS_LocateSpecialDataAddResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_LocateSpecialDataAdd_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.locationSpecialDataInfo = locationSpecialDataInfo;

	__atlsoap_hr = SetClientStruct(&__params, 4);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_LocateSpecialDataAdd\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_LocateSpecialDataAddResult = __params.HYTPGIS_LocateSpecialDataAddResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_DeviceModify_OnLineStatus(
		BSTR DEVICE_CODE, 
		BSTR OnLineStatus, 
		bool* HYTPGIS_DeviceModify_OnLineStatusResult
	)
{
    if ( HYTPGIS_DeviceModify_OnLineStatusResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.DEVICE_CODE = DEVICE_CODE;
	__params.OnLineStatus = OnLineStatus;

	__atlsoap_hr = SetClientStruct(&__params, 5);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_DeviceModify_OnLineStatus\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_DeviceModify_OnLineStatusResult = __params.HYTPGIS_DeviceModify_OnLineStatusResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_DynamicGroup_QueryMember(
		BSTR szDeviceCode, 
		BSTR* HYTPGIS_DynamicGroup_QueryMemberResult
	)
{
    if ( HYTPGIS_DynamicGroup_QueryMemberResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_DynamicGroup_QueryMember_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.szDeviceCode = szDeviceCode;

	__atlsoap_hr = SetClientStruct(&__params, 6);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_DynamicGroup_QueryMember\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_DynamicGroup_QueryMemberResult = __params.HYTPGIS_DynamicGroup_QueryMemberResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_Group_QueryDeviceGroup(
		BSTR szDeviceCode, 
		BSTR* HYTPGIS_Group_QueryDeviceGroupResult
	)
{
    if ( HYTPGIS_Group_QueryDeviceGroupResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_Group_QueryDeviceGroup_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.szDeviceCode = szDeviceCode;

	__atlsoap_hr = SetClientStruct(&__params, 7);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_Group_QueryDeviceGroup\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_Group_QueryDeviceGroupResult = __params.HYTPGIS_Group_QueryDeviceGroupResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_Group_ModifyDeviceState(
		BSTR szDeviceCode, 
		int state, 
		bool* HYTPGIS_Group_ModifyDeviceStateResult
	)
{
    if ( HYTPGIS_Group_ModifyDeviceStateResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_Group_ModifyDeviceState_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.szDeviceCode = szDeviceCode;
	__params.state = state;

	__atlsoap_hr = SetClientStruct(&__params, 8);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_Group_ModifyDeviceState\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_Group_ModifyDeviceStateResult = __params.HYTPGIS_Group_ModifyDeviceStateResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_Test_Dataset(
		BSTR* HYTPGIS_Test_DatasetResult
	)
{
    if ( HYTPGIS_Test_DatasetResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_Test_Dataset_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 9);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_Test_Dataset\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_Test_DatasetResult = __params.HYTPGIS_Test_DatasetResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_PatroAreaUser_QueryAllDevice(
		BSTR* HYTPGIS_PatroAreaUser_QueryAllDeviceResult
	)
{
    if ( HYTPGIS_PatroAreaUser_QueryAllDeviceResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 10);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_PatroAreaUser_QueryAllDevice\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_PatroAreaUser_QueryAllDeviceResult = __params.HYTPGIS_PatroAreaUser_QueryAllDeviceResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_DeviceUpdate_OffLine(
		bool* HYTPGIS_DeviceUpdate_OffLineResult
	)
{
    if ( HYTPGIS_DeviceUpdate_OffLineResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_DeviceUpdate_OffLine_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 11);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_DeviceUpdate_OffLine\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_DeviceUpdate_OffLineResult = __params.HYTPGIS_DeviceUpdate_OffLineResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_LocateDataAdd(
		LocationDataInfo locationDataInfo, 
		int* HYTPGIS_LocateDataAddResult
	)
{
    if ( HYTPGIS_LocateDataAddResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_LocateDataAdd_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.locationDataInfo = locationDataInfo;

	__atlsoap_hr = SetClientStruct(&__params, 12);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_LocateDataAdd\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_LocateDataAddResult = __params.HYTPGIS_LocateDataAddResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HelloWorld(
		BSTR* HelloWorldResult
	)
{
    if ( HelloWorldResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HelloWorld_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 13);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HelloWorld\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HelloWorldResult = __params.HelloWorldResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CLocationDataT<TClient>::HYTPGIS_LocationData_ModifyByLocation(
		__int64 GpsID, 
		BSTR szDeviceCode, 
		BSTR szLocation, 
		bool* HYTPGIS_LocationData_ModifyByLocationResult
	)
{
    if ( HYTPGIS_LocationData_ModifyByLocationResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CLocationData_HYTPGIS_LocationData_ModifyByLocation_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.GpsID = GpsID;
	__params.szDeviceCode = szDeviceCode;
	__params.szLocation = szLocation;

	__atlsoap_hr = SetClientStruct(&__params, 14);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/HYTPGIS_LocationData_ModifyByLocation\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*HYTPGIS_LocationData_ModifyByLocationResult = __params.HYTPGIS_LocationData_ModifyByLocationResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CLocationDataT<TClient>::GetFunctionMap()
{
	return __CLocationData_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CLocationDataT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CLocationData_HYTPGIS_DynamicGroup_QueryByGID_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_DynamicGroup_QueryByGID_atlsoapheader_map = 
	{
		0xF87C18AC,
		"HYTPGIS_DynamicGroup_QueryByGID",
		L"HYTPGIS_DynamicGroup_QueryByGIDResponse",
		sizeof("HYTPGIS_DynamicGroup_QueryByGID")-1,
		sizeof("HYTPGIS_DynamicGroup_QueryByGIDResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_AlarmDevice_Add_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_AlarmDevice_Add_atlsoapheader_map = 
	{
		0xDB05F4FB,
		"HYTPGIS_AlarmDevice_Add",
		L"HYTPGIS_AlarmDevice_AddResponse",
		sizeof("HYTPGIS_AlarmDevice_Add")-1,
		sizeof("HYTPGIS_AlarmDevice_AddResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_AlarmDevice_Add_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_atlsoapheader_map = 
	{
		0xC2EBB8F2,
		"HYTPGIS_DeviceQuery_BySpecialCode",
		L"HYTPGIS_DeviceQuery_BySpecialCodeResponse",
		sizeof("HYTPGIS_DeviceQuery_BySpecialCode")-1,
		sizeof("HYTPGIS_DeviceQuery_BySpecialCodeResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_DeviceTable_Query_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_DeviceTable_Query_atlsoapheader_map = 
	{
		0x5B734343,
		"HYTPGIS_DeviceTable_Query",
		L"HYTPGIS_DeviceTable_QueryResponse",
		sizeof("HYTPGIS_DeviceTable_Query")-1,
		sizeof("HYTPGIS_DeviceTable_QueryResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_DeviceTable_Query_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_LocateSpecialDataAdd_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_LocateSpecialDataAdd_atlsoapheader_map = 
	{
		0x87816512,
		"HYTPGIS_LocateSpecialDataAdd",
		L"HYTPGIS_LocateSpecialDataAddResponse",
		sizeof("HYTPGIS_LocateSpecialDataAdd")-1,
		sizeof("HYTPGIS_LocateSpecialDataAddResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_LocateSpecialDataAdd_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_DeviceModify_OnLineStatus_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_DeviceModify_OnLineStatus_atlsoapheader_map = 
	{
		0x8A4318F6,
		"HYTPGIS_DeviceModify_OnLineStatus",
		L"HYTPGIS_DeviceModify_OnLineStatusResponse",
		sizeof("HYTPGIS_DeviceModify_OnLineStatus")-1,
		sizeof("HYTPGIS_DeviceModify_OnLineStatusResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_DynamicGroup_QueryMember_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_DynamicGroup_QueryMember_atlsoapheader_map = 
	{
		0x07ED3B95,
		"HYTPGIS_DynamicGroup_QueryMember",
		L"HYTPGIS_DynamicGroup_QueryMemberResponse",
		sizeof("HYTPGIS_DynamicGroup_QueryMember")-1,
		sizeof("HYTPGIS_DynamicGroup_QueryMemberResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_DynamicGroup_QueryMember_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_Group_QueryDeviceGroup_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_Group_QueryDeviceGroup_atlsoapheader_map = 
	{
		0x5086BB35,
		"HYTPGIS_Group_QueryDeviceGroup",
		L"HYTPGIS_Group_QueryDeviceGroupResponse",
		sizeof("HYTPGIS_Group_QueryDeviceGroup")-1,
		sizeof("HYTPGIS_Group_QueryDeviceGroupResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_Group_QueryDeviceGroup_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_Group_ModifyDeviceState_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_Group_ModifyDeviceState_atlsoapheader_map = 
	{
		0xFFF4B7FB,
		"HYTPGIS_Group_ModifyDeviceState",
		L"HYTPGIS_Group_ModifyDeviceStateResponse",
		sizeof("HYTPGIS_Group_ModifyDeviceState")-1,
		sizeof("HYTPGIS_Group_ModifyDeviceStateResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_Group_ModifyDeviceState_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_Test_Dataset_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_Test_Dataset_atlsoapheader_map = 
	{
		0x041BD31B,
		"HYTPGIS_Test_Dataset",
		L"HYTPGIS_Test_DatasetResponse",
		sizeof("HYTPGIS_Test_Dataset")-1,
		sizeof("HYTPGIS_Test_DatasetResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_Test_Dataset_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_atlsoapheader_map = 
	{
		0xFE2DCD12,
		"HYTPGIS_PatroAreaUser_QueryAllDevice",
		L"HYTPGIS_PatroAreaUser_QueryAllDeviceResponse",
		sizeof("HYTPGIS_PatroAreaUser_QueryAllDevice")-1,
		sizeof("HYTPGIS_PatroAreaUser_QueryAllDeviceResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_DeviceUpdate_OffLine_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_DeviceUpdate_OffLine_atlsoapheader_map = 
	{
		0xB66C454B,
		"HYTPGIS_DeviceUpdate_OffLine",
		L"HYTPGIS_DeviceUpdate_OffLineResponse",
		sizeof("HYTPGIS_DeviceUpdate_OffLine")-1,
		sizeof("HYTPGIS_DeviceUpdate_OffLineResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_DeviceUpdate_OffLine_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_LocateDataAdd_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_LocateDataAdd_atlsoapheader_map = 
	{
		0xE3C76F11,
		"HYTPGIS_LocateDataAdd",
		L"HYTPGIS_LocateDataAddResponse",
		sizeof("HYTPGIS_LocateDataAdd")-1,
		sizeof("HYTPGIS_LocateDataAddResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_LocateDataAdd_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HelloWorld_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HelloWorld_atlsoapheader_map = 
	{
		0x8AFDB82B,
		"HelloWorld",
		L"HelloWorldResponse",
		sizeof("HelloWorld")-1,
		sizeof("HelloWorldResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HelloWorld_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};

	static const _soapmapentry __CLocationData_HYTPGIS_LocationData_ModifyByLocation_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CLocationData_HYTPGIS_LocationData_ModifyByLocation_atlsoapheader_map = 
	{
		0xC7A12804,
		"HYTPGIS_LocationData_ModifyByLocation",
		L"HYTPGIS_LocationData_ModifyByLocationResponse",
		sizeof("HYTPGIS_LocationData_ModifyByLocation")-1,
		sizeof("HYTPGIS_LocationData_ModifyByLocationResponse")-1,
		SOAPMAP_HEADER,
		__CLocationData_HYTPGIS_LocationData_ModifyByLocation_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};


	static const _soapmap * __CLocationData_headers[] =
	{
		&__CLocationData_HYTPGIS_DynamicGroup_QueryByGID_atlsoapheader_map,
		&__CLocationData_HYTPGIS_AlarmDevice_Add_atlsoapheader_map,
		&__CLocationData_HYTPGIS_DeviceQuery_BySpecialCode_atlsoapheader_map,
		&__CLocationData_HYTPGIS_DeviceTable_Query_atlsoapheader_map,
		&__CLocationData_HYTPGIS_LocateSpecialDataAdd_atlsoapheader_map,
		&__CLocationData_HYTPGIS_DeviceModify_OnLineStatus_atlsoapheader_map,
		&__CLocationData_HYTPGIS_DynamicGroup_QueryMember_atlsoapheader_map,
		&__CLocationData_HYTPGIS_Group_QueryDeviceGroup_atlsoapheader_map,
		&__CLocationData_HYTPGIS_Group_ModifyDeviceState_atlsoapheader_map,
		&__CLocationData_HYTPGIS_Test_Dataset_atlsoapheader_map,
		&__CLocationData_HYTPGIS_PatroAreaUser_QueryAllDevice_atlsoapheader_map,
		&__CLocationData_HYTPGIS_DeviceUpdate_OffLine_atlsoapheader_map,
		&__CLocationData_HYTPGIS_LocateDataAdd_atlsoapheader_map,
		&__CLocationData_HelloWorld_atlsoapheader_map,
		&__CLocationData_HYTPGIS_LocationData_ModifyByLocation_atlsoapheader_map,
		NULL
	};
	
	return __CLocationData_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CLocationDataT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CLocationDataT<TClient>::GetNamespaceUri()
{
	return L"http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CLocationDataT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CLocationDataT<TClient>::GetNamespaceUriA()
{
	return "http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CLocationDataT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CLocationDataT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
{
	if (ppReader == NULL)
	{
		return E_INVALIDARG;
	}
	
	CComPtr<ISAXXMLReader> spReader = GetReader();
	if (spReader.p != NULL)
	{
		*ppReader = spReader.Detach();
		return S_OK;
	}
	return TClient::GetClientReader(ppReader);
}

} // namespace LocationData

template<>
inline HRESULT AtlCleanupValue<LocationData::LocationDataInfo>(LocationData::LocationDataInfo *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->DEVICE_CODE);
	AtlCleanupValue(&pVal->USER_TYPE);
	AtlCleanupValue(&pVal->USER_CODE);
	AtlCleanupValue(&pVal->GPS_DATETIME);
	AtlCleanupValue(&pVal->GPSAV);
	AtlCleanupValue(&pVal->LONG_WE);
	AtlCleanupValue(&pVal->LONGITUDE);
	AtlCleanupValue(&pVal->LAT_NS);
	AtlCleanupValue(&pVal->LATITUDE);
	AtlCleanupValue(&pVal->SPEED);
	AtlCleanupValue(&pVal->DIRECTION);
	AtlCleanupValue(&pVal->LOCATION);
	AtlCleanupValue(&pVal->STATE);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<LocationData::LocationDataInfo>(LocationData::LocationDataInfo *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->DEVICE_CODE, pMemMgr);
	AtlCleanupValueEx(&pVal->USER_TYPE, pMemMgr);
	AtlCleanupValueEx(&pVal->USER_CODE, pMemMgr);
	AtlCleanupValueEx(&pVal->GPS_DATETIME, pMemMgr);
	AtlCleanupValueEx(&pVal->GPSAV, pMemMgr);
	AtlCleanupValueEx(&pVal->LONG_WE, pMemMgr);
	AtlCleanupValueEx(&pVal->LONGITUDE, pMemMgr);
	AtlCleanupValueEx(&pVal->LAT_NS, pMemMgr);
	AtlCleanupValueEx(&pVal->LATITUDE, pMemMgr);
	AtlCleanupValueEx(&pVal->SPEED, pMemMgr);
	AtlCleanupValueEx(&pVal->DIRECTION, pMemMgr);
	AtlCleanupValueEx(&pVal->LOCATION, pMemMgr);
	AtlCleanupValueEx(&pVal->STATE, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<LocationData::DeviceInfoClass>(LocationData::DeviceInfoClass *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->DEVICE_ID);
	AtlCleanupValue(&pVal->DEVICE_CODE);
	AtlCleanupValue(&pVal->DEPT_NO);
	AtlCleanupValue(&pVal->PUBLIC_CODE);
	AtlCleanupValue(&pVal->SPECIAL_CODE);
	AtlCleanupValue(&pVal->PRODUCER);
	AtlCleanupValue(&pVal->DEVICE_TYPE);
	AtlCleanupValue(&pVal->STATE);
	AtlCleanupValue(&pVal->ONLINE_STATE);
	AtlCleanupValue(&pVal->VALIDE_DATE);
	AtlCleanupValue(&pVal->USER_TYPE);
	AtlCleanupValue(&pVal->USER_CODE);
	AtlCleanupValue(&pVal->REMARK);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<LocationData::DeviceInfoClass>(LocationData::DeviceInfoClass *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->DEVICE_ID, pMemMgr);
	AtlCleanupValueEx(&pVal->DEVICE_CODE, pMemMgr);
	AtlCleanupValueEx(&pVal->DEPT_NO, pMemMgr);
	AtlCleanupValueEx(&pVal->PUBLIC_CODE, pMemMgr);
	AtlCleanupValueEx(&pVal->SPECIAL_CODE, pMemMgr);
	AtlCleanupValueEx(&pVal->PRODUCER, pMemMgr);
	AtlCleanupValueEx(&pVal->DEVICE_TYPE, pMemMgr);
	AtlCleanupValueEx(&pVal->STATE, pMemMgr);
	AtlCleanupValueEx(&pVal->ONLINE_STATE, pMemMgr);
	AtlCleanupValueEx(&pVal->VALIDE_DATE, pMemMgr);
	AtlCleanupValueEx(&pVal->USER_TYPE, pMemMgr);
	AtlCleanupValueEx(&pVal->USER_CODE, pMemMgr);
	AtlCleanupValueEx(&pVal->REMARK, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<LocationData::LocationSpecialDataInfo>(LocationData::LocationSpecialDataInfo *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->SPECIAL_CODE);
	AtlCleanupValue(&pVal->GPS_DATETIME);
	AtlCleanupValue(&pVal->GPSAV);
	AtlCleanupValue(&pVal->LONG_WE);
	AtlCleanupValue(&pVal->LONGITUDE);
	AtlCleanupValue(&pVal->LAT_NS);
	AtlCleanupValue(&pVal->LATITUDE);
	AtlCleanupValue(&pVal->SPEED);
	AtlCleanupValue(&pVal->DIRECTION);
	AtlCleanupValue(&pVal->LOCATION);
	AtlCleanupValue(&pVal->STATE);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<LocationData::LocationSpecialDataInfo>(LocationData::LocationSpecialDataInfo *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->SPECIAL_CODE, pMemMgr);
	AtlCleanupValueEx(&pVal->GPS_DATETIME, pMemMgr);
	AtlCleanupValueEx(&pVal->GPSAV, pMemMgr);
	AtlCleanupValueEx(&pVal->LONG_WE, pMemMgr);
	AtlCleanupValueEx(&pVal->LONGITUDE, pMemMgr);
	AtlCleanupValueEx(&pVal->LAT_NS, pMemMgr);
	AtlCleanupValueEx(&pVal->LATITUDE, pMemMgr);
	AtlCleanupValueEx(&pVal->SPEED, pMemMgr);
	AtlCleanupValueEx(&pVal->DIRECTION, pMemMgr);
	AtlCleanupValueEx(&pVal->LOCATION, pMemMgr);
	AtlCleanupValueEx(&pVal->STATE, pMemMgr);
	return S_OK;
}
