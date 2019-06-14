#pragma once
#include "GazPriborDrv.h"
#include "Transaction.h"
#include "TransactionParam.h"
#include "SFproto.h"
#include <map>
#include "json.h"
#include "SerialDev.h"


class SfTransaction :
	public Transaction
{
public:
	UCHAR m_reqIndex;
	SfTransaction(void);
	SfTransaction(CSerialDev* device, int address, const Json::Value &DeviceRequest, const Json::Value &Params, string ParamName);
	virtual ~SfTransaction(void);
	/// \brief This function returns type of parameter according to its string name
	static PARAM_TYPE GetParamTypeFromString(string type);
	
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
	virtual void SetStartArchTime(void* devConfig);
	virtual DWORD GetCommand();
	bool CheckResponseCRC();
	int ParseResponse();
	bool ExtCRC16 (UCHAR* extBuf, bool writeflag);
	inline CSerialDev* getSerialDev();

private:
	Json::Value _params;
	Json::Value _deviceRequest;
	prefix_t * _prefixIn;

	int _requestHour; ///< if req has out param hour -- save last value
	int _requestDay; ///< if req has out param Day -- save last value
	int _requestMonth; ///< if req has out param Month -- save last value
	int _requestYear; ///< if req has out param Year -- save last value
};

