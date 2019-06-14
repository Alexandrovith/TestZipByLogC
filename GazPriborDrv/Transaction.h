#pragma once
#include <windows.h>
#include <time.h> 
#include "TransactionParam.h"

typedef enum _PROC_RESP_RETVAL{
	PROC_RESP_RETVAL_SUCCESS = 0,
	PROC_RESP_RETVAL_FAIL,
	PROC_RESP_RETVAL_NOTREADY
} PROC_RESP_RETVAL;

class Transaction
{
public:
	
	void SetPeriod(long);
	Transaction(void);
	Transaction(void* device, string ParamName);
	Transaction(int);
	Transaction(int, long);
	virtual ~Transaction(void);
	time_t SetFireTime(time_t);
	time_t ResetFireTime(time_t);
	time_t SetStartTime(time_t);
	bool isFire(time_t);
	//int SendRequest(OVERLAPPED *);
	UCHAR* GetOutBuffer() { return _rawBufferOut;};
	UCHAR* GetInBuffer() { return _rawBufferIn;};
	void SetAddr(UINT addr) {_Addr = addr;};
	UINT GetAddr() {return _Addr;};
	void SetOneShot(bool oneShot) { _oneShot = oneShot;};
	bool isOneShot() { return _oneShot;};
	virtual void PrepareToSend()=0;
	virtual void SendDone()=0;
	virtual void PrepareToRead()=0;
	virtual void ReadDone()=0;
	virtual bool CreateRequest()=0;
	virtual bool AddParameter(int offset, UINT size, string ptype, string value)=0;
	virtual bool AddRawParameter(int offset, int size, UCHAR* value)=0;
	virtual bool AddByteParameter(int offset, UINT size, UCHAR value)=0;
	virtual bool AddFloatParameter(int offset, UINT size, float value)=0;
	virtual DWORD GetInLength()=0;
	virtual DWORD GetOutLength()=0;
	virtual DWORD GetCommand()=0;
	virtual bool PrepareData()=0;
	
	void SetNeedContinuation(bool continuation) { _needContinuation = continuation;};
	bool isNeedContinuation() { return _needContinuation;};

	virtual PROC_RESP_RETVAL ProcessResponse(DWORD size)=0;
	virtual void SetBadResponse()=0;
	virtual void SetStartArchTime(void* devConfig)=0;
	DWORD GetInSize() { return _rawBufferInSize;};
	DWORD GetOutSize() { return _rawBufferOutSize;};
	const string GetRunNumber() {return _RunNumber;};
	void SetRunNumber(string rn) {_RunNumber = rn;}
	string GetPassword() { return _password;};
	string SetPassword(string & pass) { _password = pass;};
	void SetTryCount(int count);
	int GetTryCount();
	int OneMoreTry();
	string GetParamName () { return asParamName; }
	char* GetCommName () { return caCommand; }

protected:
	const static int SIZE_COMM = 8;

	UINT _Addr;
	bool _needContinuation;
	string _password;			///< Password for SF
	bool _oneShot;				///< flag for Transaction that should fire only ones by request
	string _RunNumber;			///< RunNumber to store parameter of request  // TODO only used for SF
	string _deviceName;			///< Name of Device current transaction belongs to
	UCHAR* _rawBufferOut;		///< Raw Buffer for request
	UCHAR* _rawBufferIn;		///< Raw Buffer for response
	DWORD _rawBufferInSize;		///< Buffer Size for response
	DWORD _rawBufferOutSize;	///< Buffer Size for request
	int   _Command;
	void* _device;			///< serial device transaction belongs to
	int _tryCount;			///< how many times to try request in case of errors
	int _initTryCount;

	long  _lReqPeriod;
	string GetDeviceName() {return _deviceName;};	
	time_t _startTime;
	time_t _fireTime;

	string asParamName;
	char caCommand[SIZE_COMM];
};

