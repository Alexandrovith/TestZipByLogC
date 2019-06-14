#include "stdafx.h"
#include "RmgTransaction.h"
#include <iostream>
#include <map>
#include "DevProcessor.h"
#include "common.h"

using namespace std;
using namespace DevProc;

unsigned short CRC(unsigned char *Data, unsigned long length, unsigned short poly, unsigned short init)
{
	unsigned short crc = init;

	for(unsigned long i=0;i<length;i++)
	{
		crc ^= Data[i] << 8;
		for(unsigned long k=0; k<8; k++)
		{
			if(crc & 0x8000){
				crc = crc << 1;
				crc ^= poly;
			} else {
				crc = crc << 1;
			}
		}
	}	
	return crc;
}

RmgTransaction::RmgTransaction(void)
{
}

RmgTransaction::~RmgTransaction(void)
{
}

RmgTransaction::RmgTransaction(const string devName, int address, const Json::Value &DeviceRequest, const Json::Value &Params) : 
	Transaction(devName)
{	
	time_t curTime;
	time(&curTime);
	SetStartTime(curTime);
	SetFireTime(curTime);
	_deviceRequest = DeviceRequest;
	_params = Params;
	SetAddr(address);
	_Command = _deviceRequest["Command"].asUInt(); 
	_lReqPeriod = _deviceRequest["TimePeriod"].asUInt();
	_rawBufferInSize = _deviceRequest["InSize"].asUInt();
	_rawBufferIn = new UCHAR[_rawBufferInSize];
	memset(_rawBufferIn, 0, _rawBufferInSize);
	_rawBufferOutSize = _deviceRequest["OutSize"].asUInt();
	_rawBufferOut = new UCHAR[_rawBufferOutSize];
	memset(_rawBufferOut, 0, _rawBufferOutSize);
	SetRunNumber("");

	//_requestHour = 0; 
	//_requestDay = 1; 
	//_requestMonth = 1; 
	//_requestYear = 14; 
};

/*
RmgTransaction::~RmgTransaction(void)
{
	if(_rawBufferIn != NULL)
		delete [] _rawBufferIn;
	if(_rawBufferOut != NULL)
		delete [] _rawBufferOut;
		
}*/

DWORD RmgTransaction::GetCommand()
{
	return _Command;
}

DWORD RmgTransaction::GetInLength()
{
	prefix_t * prefix;
	prefix = (prefix_t *) _rawBufferIn;
	return prefix->size;
}

DWORD RmgTransaction::GetOutLength()
{
	prefix_t * prefix;
	prefix = (prefix_t *) _rawBufferOut;
	return prefix->size;
}

bool RmgTransaction::AddRawParameter(int offset, int size, UCHAR* value){
	if(size == 0)
		return FALSE;
	memcpy(_rawBufferOut + offset, value, size);
	return TRUE;
}

bool RmgTransaction::AddParameter(int offset, UINT size, string ptype, string value){
	if(ptype.compare("byte") == 0){
		UCHAR val = (UCHAR) stoi(value);
		_rawBufferOut[offset] = val;
	} else if(ptype.compare("integer") == 0){
		long val =  stol(value);
		*(long*)(_rawBufferOut + offset) = val;
	} else if(ptype.compare("float") == 0){
		float val =  stof(value);
		*(float*)(_rawBufferOut + offset) = val;
	} else if(ptype.compare("string") == 0){
		memcpy(_rawBufferOut + offset, value.c_str(), min(size, value.length()));
		if(value.length() < size){
			memset(_rawBufferOut + offset + value.length(), ' ', size - value.length());
		}
	} else
		return FALSE;
	return TRUE;
}

bool RmgTransaction::AddByteParameter(int offset, UINT size, UCHAR value)
{
	_rawBufferOut[offset] = value;
	return TRUE;
}

bool RmgTransaction::AddFloatParameter(int offset, UINT size, float value)
{
	*(float*)(_rawBufferOut + offset) = value;
	return TRUE;
}

PROC_RESP_RETVAL RmgTransaction::ProcessResponse(DWORD size)
{
	//DWORD starttick = ::GetTickCount();	

/*	if(size < sizeof(prefix_t))
		return PROC_RESP_RETVAL_NOTREADY;
	_prefixIn = (prefix_t *) _rawBufferIn;
	if(size < _prefixIn->size)
		return PROC_RESP_RETVAL_NOTREADY;
	if(_prefixIn->syncbyte != 0x55){
		return PROC_RESP_RETVAL_FAIL;
	}
	if(((_prefixIn->cmd) & 0x7f) != _Command){
		return PROC_RESP_RETVAL_FAIL;
	}

	if(!CheckResponseCRC())
		return PROC_RESP_RETVAL_FAIL;


	// find sequence number param
	UINT seq_num_param;
	int seqNum = 0;
	for(seq_num_param=0; seq_num_param<_params.size(); seq_num_param++)
	{
		string pname = _params[seq_num_param]["Name"].asString();
		if(pname.compare("Request_Sequence_Number") == 0) 
		{
			string sn = _params[seq_num_param]["Value"].asString();
			seqNum = stoi(sn);
			break;
		}	
	}
	
	if(ParseResponse() != 0) 
	{ // Need continuation
		
		// sequence not found
		if (seq_num_param == _params.size())
		{
			_params.resize(seq_num_param+1);
			_params[seq_num_param]["Name"] = "Request_Sequence_Number";
			_params[seq_num_param]["Value"] = to_string(0);			
		}

		if(seqNum < 255) 
		{
			seqNum++;
			_params[seq_num_param]["Value"] = to_string(seqNum);
			CreateRequest();
			SetNeedContinuation(TRUE);			
		} 
		else
		{
			_params[seq_num_param]["Value"] = to_string(0);
			SetNeedContinuation(FALSE);
		}
		
	} else 
	{
		// reset request sequence flag
		if(seq_num_param != _params.size())
		{
			_params[seq_num_param]["Value"] = to_string(0);
		}
		SetNeedContinuation(FALSE);
	}

	//LOG("RmgTransaction::ProcessResponse %d msec\n", ::GetTickCount() - starttick);
	*/
	return PROC_RESP_RETVAL_SUCCESS;
}

void RmgTransaction::ReadDone()		 
{
	DWORD starttick = ::GetTickCount();

/*	switch(_prefixIn->cmd){
	case 0xAA: // successful response to command R42 save all binary packet data to simple write generation
		vector<UCHAR> data(_rawBufferIn+5, _rawBufferIn+(_rawBufferInSize-2-6));
		Storage.SavePacket(GetDeviceName()+".R43."+_RunNumber, data);
		break;
	}
*/
	//LOG("RmgTransaction::ReadDone: %d\n", ::GetTickCount() - starttick);
}

bool RmgTransaction::CheckResponseCRC()
{
	//CRC(unsigned char *Data, unsigned long length, unsigned short poly, unsigned short init)
	return TRUE;
}

/// \brief This function returns type of parameter according to its string name
SFPARAM_TYPE RmgTransaction::GetParamTypeFromString(string stype)
{
	if(stype.compare("byte") == 0)
	{
		return SFPARAM_TYPE_BYTE;
	} else 
	if(stype.compare("word") == 0)
	{
		return SFPARAM_TYPE_WORD;
	} 
	else 
	if(stype.compare("integer") == 0)
	{
		return SFPARAM_TYPE_INT;
	} 
	else 
	if(stype.compare("float") == 0)
	{
		return SFPARAM_TYPE_FLOAT;
	} 
	else 
	if(stype.compare("string") == 0)
	{
		return SFPARAM_TYPE_STRING;
	}
	else 
	if(stype.compare("year") == 0)
	{
		return SFPARAM_TYPE_YEAR;
	}
	else 
	if(stype.compare("month") == 0)
	{
		return SFPARAM_TYPE_MONTH;
	}
	else 
	if(stype.compare("day") == 0)
	{
		return SFPARAM_TYPE_DAY;
	}
	else 
	if(stype.compare("hour") == 0)
	{
		return SFPARAM_TYPE_HOUR;
	}
	else 
	if(stype.compare("minute") == 0)
	{
		return SFPARAM_TYPE_MINUTE;
	}
	else 
	if(stype.compare("second") == 0)
	{
		return SFPARAM_TYPE_SECOND;
	}

	return SFPARAM_TYPE_STRING;
}

/// \brief Sets dummy parameter to datastorage in case of bad response  
void RmgTransaction::SetBadResponse()
{
	TransactionParam tp(0, SFPARAM_TYPE_BYTE, 1, "ConnectionStatusBAD", _rawBufferIn);
	Storage.SetValue(GetDeviceName(), GetCommand(), _RunNumber, "ConnectionStatusBAD", tp);
}


/// \brief Parse and store parameters to Storage to be ready for scada's High Level 
/// \return 0 on simple response, 1 on compex response with continuation
int RmgTransaction::ParseResponse()
{
	UCHAR respStatus = 0;
	int paramsInResponse = _deviceRequest["ParamsInList"].size();
	//DWORD ticks_start = ::GetTickCount();

	//int paramsCnt = 0;
	string atype = _deviceRequest["Type"].asString();
	if(atype.compare("Archive") == 0){
		int recOffset = _deviceRequest["RecordOffset"].asUInt();
		int recSize = _deviceRequest["RecordSize"].asUInt();
		UCHAR packIndx = 0, packNum = 0;
		UCHAR addidtion = 0;

		// Looking for RecordsNumber and Status
		for(int paramNum = 0; paramNum < paramsInResponse; paramNum++){
			string name = _deviceRequest["ParamsInList"][paramNum]["Name"].asString();
			if(name == "")
				break;
			int offset = _deviceRequest["ParamsInList"][paramNum]["offset"].asUInt();
			if(name.compare("RecordsNumber") == 0){
				packNum = *(_rawBufferIn + offset);
			}
			if(name.compare("ResponseStatus") == 0){
				respStatus = *(_rawBufferIn + offset);
			}
		}
		if(packNum == 0)
			return respStatus;
		for(packIndx; packIndx < packNum; packIndx++) {
			DataStorage::tStorageParams sp;
			//vector<TransactionParam> tpVector;
			//Storage.AddRecord(GetDeviceName(), GetCommand());

			for(int paramNum = 0; paramNum < paramsInResponse; paramNum++){
				string name = _deviceRequest["ParamsInList"][paramNum]["Name"].asString();
				if(name == "")
					break;
				
				string stype = _deviceRequest["ParamsInList"][paramNum]["type"].asString();
				int offset = _deviceRequest["ParamsInList"][paramNum]["offset"].asUInt();

				if(offset < recOffset) // skip common parameters after second loop
					continue;

				int size = _deviceRequest["ParamsInList"][paramNum]["length"].asUInt();
				string unit = _deviceRequest["ParamsInList"][paramNum]["unit"].asString();

				SFPARAM_TYPE type = GetParamTypeFromString(stype);
						
				//name += _RunNumber; moved to AddRecord
				TransactionParam tp(offset, type, size, unit, _rawBufferIn + offset + addidtion);	
				sp[name] = tp;
				//Storage.AddMultiValue(GetDeviceName(), GetCommand(), name, tp);
			}
			Storage.AddRecord(GetDeviceName(), GetCommand(), _RunNumber, sp);
			addidtion += recSize;
		}
		
		// archive processing

		LOG("Archives parsed, %d\n", packNum);
	} else {
		for(int paramNum = 0; paramNum < paramsInResponse; paramNum++){
			string name = _deviceRequest["ParamsInList"][paramNum]["Name"].asString();
			if(name == "")
				break;
			//name += _RunNumber;
			string stype = _deviceRequest["ParamsInList"][paramNum]["type"].asString();
			int offset = _deviceRequest["ParamsInList"][paramNum]["offset"].asUInt();
			int size = _deviceRequest["ParamsInList"][paramNum]["length"].asUInt();
			string unit = _deviceRequest["ParamsInList"][paramNum]["unit"].asString();
			SFPARAM_TYPE type = GetParamTypeFromString(stype);
			
			TransactionParam tp(offset, type, size, unit, _rawBufferIn + offset);
			Storage.SetValue(GetDeviceName(), GetCommand(), _RunNumber, name, tp);
			//paramsCnt++;
		}
	}


	//LOG("RmgTransaction::ParseResponse took %d ms\n",::GetTickCount() - ticks_start);
	return respStatus;
}

bool RmgTransaction::CreateRequest()
{
	_rawBufferOut[0] = 0xAA;
	_rawBufferOut[1] = _Addr; // addr
	_rawBufferOut[2] = (UCHAR)_rawBufferOutSize; // count
	_rawBufferOut[3] = _Command; // addr

	
	return TRUE;
}

bool RmgTransaction::PrepareData()
{
	switch (_Command)
	{
	case 43: // Write static parameters. Need previous command to read static data.
		vector<UCHAR> v = Storage.GetPacket(_deviceName+".R43."+_RunNumber);
		if(v.size() == 0) 
			return FALSE;
		AddRawParameter(21, v.size(), v.data()); // copy vector at offset
		break;
	}
	//Json::Value paramsOut = _params;
	Json::Value paramsTmp = _deviceRequest["ParamsOutList"];
	if( _params.size() > 0 && paramsTmp.size() > 0)
	{

		for(UINT po=0; po< _params.size(); po++)
		{
			string pname =  _params[po]["Name"].asString();
			
			for(UINT ptmp=0; ptmp<paramsTmp.size(); ptmp++)
			{
				if(paramsTmp[ptmp]["Name"].asString().compare(pname) == 0)
				{
					// add parameter
					int offset = paramsTmp[ptmp]["offset"].asInt();
					int size = paramsTmp[ptmp]["length"].asInt();
					string ptype =  paramsTmp[ptmp]["type"].asString();

					if (_params[po]["Value"].isArray())
					{
						for (UINT i = 0 ;i < _params[po]["Value"].size(); i++)
						{
							char c = _params[po]["Value"][i].asInt();

							AddRawParameter(offset+i, 1, (UCHAR*)&c);
						}
					}
					else
					{
						AddParameter(offset, size, ptype, _params[po]["Value"].asString());
					}			

					break;
				}
			}
		}
	}
	/*if(!SFproto::ExtCRC16(_rawBufferOut, TRUE))
		return FALSE;
		*/
	return TRUE;
}


void RmgTransaction::PrepareToSend() {
	time_t curTime;
	time(&curTime);
	tm tmCurrent;

	localtime_s(&tmCurrent, &curTime);
	map<string, UCHAR> DateTime;
	int t1=0, t2=0;
	int paramsOut = _deviceRequest["ParamsOutList"].size();
	switch(_Command){
	case 0x14:		///< 20, Daily history
	case 0x15:		///< 21, Hourly history
	case 0x16:		///< 22, AUDIT
	case 0x17:		///< 23, Alarms
		for(int paramNum = 0; paramNum < paramsOut; paramNum++)
		{
			string name = _deviceRequest["ParamsOutList"][paramNum]["Name"].asString();
			int offset = _deviceRequest["ParamsOutList"][paramNum]["offset"].asUInt();
			if((name.compare("Starting_Month") == 0) || (name.compare("Ending_Month") == 0))
			{
				/*t1 = *(_rawBufferOut + offset);
				t1 += (int)(*(_rawBufferOut + offset + 1))<<8;
				t1 += (int)(*(_rawBufferOut + offset + 2))<<16;*/
				//if(t1 == 0)
				if (!_needContinuation)
				{
					//tmCurrent.tm_hour = 0;
					//tmCurrent.tm_min = 2;		
					//tmCurrent.tm_sec = 0;

					//time_t timeval = mktime(&tmCurrent);
					//timeval -= 24*60*60; // previous day
					//localtime_s(&tmSet, &timeval);
					/*DateTime["Starting_Month"] = tmSet.tm_mon+1;;
					DateTime["Starting_Day"] = tmSet.tm_mday;
					DateTime["Starting_Year"] = tmSet.tm_year-100;
					DateTime["Starting_Hour"] = tmSet.tm_hour;*/
					/*DateTime["Starting_Month"] = 1;;
					DateTime["Starting_Day"] = 1;
					DateTime["Starting_Year"] = 14;
					DateTime["Starting_Hour"] = 0;*/
/*					DateTime["Starting_Month"] = _requestMonth;
					DateTime["Starting_Day"] = _requestDay;
					DateTime["Starting_Year"] = _requestYear;
					DateTime["Starting_Hour"] = _requestHour;
*/
					DateTime["Ending_Month"] = tmCurrent.tm_mon+1;;
					DateTime["Ending_Day"] = tmCurrent.tm_mday;
					DateTime["Ending_Year"] = tmCurrent.tm_year-100;
					DateTime["Ending_Hour"] = tmCurrent.tm_hour;
				}								
			}
			if(!DateTime.empty()) 
			{
				map<string, UCHAR>::iterator it = DateTime.find(name);		
				if(it != DateTime.end())
				{
					int size = _deviceRequest["ParamsOutList"][paramNum]["length"].asUInt();
					AddByteParameter(offset, size, (UCHAR)((*it).second));
				}
			}
		}
//		SFproto::ExtCRC16(_rawBufferOut, TRUE);

		// remember last date
		if(!DateTime.empty()) 
		{
			LOG("RmgTransaction::PrepareToSend: Req Date has changed");
/*			_requestMonth = DateTime["Ending_Month"];
			_requestDay = DateTime["Ending_Day"];
			_requestYear = DateTime["Ending_Year"];
			_requestHour = DateTime["Ending_Hour"];
			*/
		}
		break;
	case 0x1e:		///< 30, Set Time command
		DateTime["New_Month"] = tmCurrent.tm_mon+1;
		DateTime["New_Day"] = tmCurrent.tm_mday;
		DateTime["New_Year"] = tmCurrent.tm_year-100;
		DateTime["New_Hour"] = tmCurrent.tm_hour;
		DateTime["New_Minute"] = tmCurrent.tm_min;
		DateTime["New_Second"] = tmCurrent.tm_sec;
		
		for(int paramNum = 0; paramNum < paramsOut; paramNum++){
			string name = _deviceRequest["ParamsOutList"][paramNum]["Name"].asString();
			map<string, UCHAR>::iterator it = DateTime.find(name);
			if(it != DateTime.end()){
				int offset = _deviceRequest["ParamsOutList"][paramNum]["offset"].asUInt();
				int size = _deviceRequest["ParamsOutList"][paramNum]["length"].asUInt();
				AddByteParameter(offset, size, (UCHAR)((*it).second));
			}
		}
//		SFproto::ExtCRC16(_rawBufferOut, TRUE);
		break;
	}
};
void RmgTransaction::SendDone()		 { cout << "SendDone SF\n";};
void RmgTransaction::PrepareToRead()	 { cout << "PrepareToRead SF\n";};
