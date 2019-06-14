#pragma once
#include "GazPriborDrv.h"
#include <map>
#include "TransactionParam.h"
#include "json.h"

using namespace std;
#ifdef DEBUG_LOG
#define LOCK LOG("Lock: %s\n", __FUNCTION__);LockData()
#define UNLOCK LOG("Unlock: %s\n", __FUNCTION__);UnlockData()
#else
#define LOCK LockData()
#define UNLOCK UnlockData()
#endif

class DataStorage
{
public:
	typedef map<string, TransactionParam> tStorageParams;
	typedef tStorageParams::iterator tStorageParamsIt;

	typedef map<int, vector<tStorageParams>> tReqData;
	typedef tReqData::iterator tReqDataIt;

	typedef map<string, tReqData> tStorageData;
	typedef tStorageData::iterator tStorageDataIt;

	typedef map<string, vector<UCHAR>> tStoragePack;
	typedef tStoragePack::iterator tStoragePackIt;


	DataStorage(void);
	~DataStorage(void);

	//int GetVectorSize(string device, int reqNumber);
	int SetValue(string device, int reqNumber, string runNumber, string paramName, TransactionParam &param);
	void AddRecord(string device, int reqNumber, string runNumber, DataStorage::tStorageParams sp);
	int AddMultiValue(string device, int reqNumber, string paramName, TransactionParam &param);
	bool GetValue(string device, int reqNumber, string runNumber, string paramName, TransactionParam &param);
	string GetArchiveData(string device, int reqNumber, string runNumber);
	int SavePacket(string keyValue, vector<UCHAR>);
	vector<UCHAR> GetPacket(string keyValue);

private:
	inline void	LockData() { EnterCriticalSection(&_dataLock);};
	inline void	UnlockData() { LeaveCriticalSection(&_dataLock);};
	inline void	InitLock() { InitializeCriticalSection(&_dataLock);};
	inline void	DelLock() { DeleteCriticalSection(&_dataLock);};

	/// \brief This function converts storage param data to archive param
	Json::Value ConvertStorageParamToArchiveParam(tStorageParamsIt & recIt);

	CRITICAL_SECTION _dataLock;
	tStorageData _storageData;
	tStoragePack _storagePack;
};

