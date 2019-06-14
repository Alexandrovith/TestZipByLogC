#pragma once
#include "Transaction.h"
#include "json.h"

class RmgTransaction :
	public Transaction
{
public:
	RmgTransaction(void);
	~RmgTransaction(void);
	RmgTransaction(const string devName, int address, const Json::Value &DeviceRequest, const Json::Value &Params);
	virtual void PrepareToSend(); ///< prepare data on every send
	virtual void SendDone();
	virtual void PrepareToRead();
	virtual void ReadDone();

	virtual bool CreateRequest();
	virtual bool AddParameter(int offset, UINT size, string ptype, string value);
	virtual bool AddRawParameter(int offset, int size, UCHAR* value);
	virtual bool AddByteParameter(int offset, UINT size, UCHAR value);
	virtual bool AddFloatParameter(int offset, UINT size, float value);
	virtual DWORD GetInLength();
	virtual DWORD GetOutLength();
	virtual PROC_RESP_RETVAL ProcessResponse(DWORD size);
	virtual bool PrepareData(); ///< prepare data from template ones for Subscribe or WriteValue

	virtual void SetBadResponse();
	virtual DWORD GetCommand();
	bool CheckResponseCRC();
	static SFPARAM_TYPE GetParamTypeFromString(string type);
	int ParseResponse();
protected:
	Json::Value _params;
	Json::Value _deviceRequest;
};

