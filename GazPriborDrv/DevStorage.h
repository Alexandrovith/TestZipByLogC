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

class DevStorage
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


	DevStorage(void);
	~DevStorage(void);

	//int GetVectorSize(string device, int reqNumber);
	int SetValue(int reqNumber, string runNumber, string paramName, TransactionParam &param);
	void AddRecord(int reqNumber, string runNumber, DevStorage::tStorageParams sp);
	int AddMultiValue(int reqNumber, string paramName, TransactionParam &param);
	bool GetValue(int reqNumber, string runNumber, string paramName, TransactionParam &param);
	string GetArchiveData(int reqNumber, string runNumber, void* Dev);
	int SavePacket(string keyValue, vector<UCHAR>);
	vector<UCHAR> GetPacket(string keyValue);
	void RemovePackets();//ADD

private:
	inline void	LockData() { EnterCriticalSection(&_dataLock);};
	inline void	UnlockData() { LeaveCriticalSection(&_dataLock);};
	inline void	InitLock() { InitializeCriticalSection(&_dataLock);};
	inline void	DelLock() { DeleteCriticalSection(&_dataLock);};

	/// \brief This function converts storage param data to archive param
	Json::Value ConvertStorageParamToArchiveParam(tStorageParamsIt & recIt);

	CRITICAL_SECTION _dataLock;
	tReqData _storageDataRecords;
	//tStorageData _storageData;
	tStoragePack _storagePack;
};

