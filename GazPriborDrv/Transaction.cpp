#include "stdafx.h"
#include "Transaction.h"
#include <iostream>
#include <limits>

using namespace std;

Transaction::Transaction(void)
{

}

Transaction::Transaction(int nReg):
	_rawBufferOut(NULL),
	_rawBufferIn(NULL),
	_rawBufferInSize(0),
	_rawBufferOutSize(0), 
	_needContinuation(FALSE),
	_initTryCount(0),
	_tryCount(0)
{
	_Command = nReg;
}

Transaction::Transaction(void* device, string ParamName):
	_rawBufferOut(NULL),
	_rawBufferIn(NULL),
	_rawBufferInSize(0),
	_rawBufferOutSize(0), 
	_needContinuation(FALSE),
	_initTryCount(0),
	_tryCount(0)
	, asParamName(ParamName)
{
	_device=device;
	//_deviceName = device->GetName();
	_oneShot = FALSE;
}
	
Transaction::Transaction(int nReg, long period):
	_rawBufferOut(NULL),
	_rawBufferIn(NULL),
	_rawBufferInSize(0),
	_rawBufferOutSize(0), 
	_needContinuation(FALSE),
	_initTryCount(0),
	_tryCount(0)
{
	_Command = nReg;
	_lReqPeriod = period;
}

void Transaction::SetTryCount(int count)
{
	_tryCount = count;
	_initTryCount = count;

}

int Transaction::GetTryCount()
{
	return _tryCount;
}

int Transaction::OneMoreTry()
{
	if(_tryCount > 0) _tryCount--;
	return _tryCount;
}

void Transaction::SetPeriod(long period)
{
	_lReqPeriod = period;
}

Transaction::~Transaction(void)
{

}

time_t Transaction::SetFireTime(time_t tRefTime)
{
	_fireTime = tRefTime;
	_tryCount = _initTryCount;
	return _fireTime;
}

time_t Transaction::SetStartTime(time_t tRefTime)
{
	_startTime = tRefTime;
	return _startTime;
}

time_t Transaction::ResetFireTime(time_t tRefTime)
{
	_tryCount = _initTryCount;           // reset try count for new request
	if(_lReqPeriod == 0)
		return (_fireTime += 24*60*60);  // temp hardcode 
	time_t newFire = _fireTime + _lReqPeriod;
	if(newFire < tRefTime){
		UINT64 nperiod = (tRefTime - _startTime) / _lReqPeriod;
		newFire = _startTime + (nperiod + 1) * _lReqPeriod;
	}
	_fireTime = newFire;
	return _fireTime;
}

bool Transaction::isFire(time_t tRefTime)
{
	return (tRefTime >= _fireTime);
}
