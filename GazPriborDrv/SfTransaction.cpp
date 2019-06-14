#include "stdafx.h"
#include "SfTransaction.h"
#include <iostream>
#include <map>
#include "DevProcessor.h"
#include "common.h"
#include "Logs_C.h"

using namespace std;
using namespace DevProc;

UCHAR auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

UCHAR auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};

//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww

SfTransaction::SfTransaction(void)
{
	
}
//_____________________________________________________________________________
///SfTransaction::SfTransaction(const string devName, int address, const Json::Value &DeviceRequest, const Json::Value &Params) : 
SfTransaction::SfTransaction(CSerialDev* device, int address, const Json::Value &DeviceRequest, const Json::Value &Params, string ParamName) : 
	Transaction(device, ParamName), m_reqIndex(0)
{	
	_deviceName = getSerialDev()->GetName();
	time_t curTime;
	time(&curTime);
	SetStartTime(curTime);
	SetFireTime(curTime);
	_deviceRequest = DeviceRequest;
	_params = Params;
	SetAddr(address);

	_Command = _deviceRequest["Command"].asUInt(); 
	_itoa_s (_Command, caCommand, SIZE_COMM, 10);
	//caCommand[0] = _Command & 0xff;
	//caCommand[1] = _Command >> 8;
	//caCommand[2] = 0;

	_lReqPeriod = _deviceRequest["TimePeriod"].asUInt();
	_rawBufferInSize = _deviceRequest["InSize"].asUInt();
	_rawBufferIn = new UCHAR[_rawBufferInSize];
	memset(_rawBufferIn, 0, _rawBufferInSize);
	_rawBufferOutSize = _deviceRequest["OutSize"].asUInt();
	_rawBufferOut = new UCHAR[_rawBufferOutSize];
	memset(_rawBufferOut, 0, _rawBufferOutSize);
	SetRunNumber("");

	_requestHour = 0; 
	_requestDay = 1; 
	_requestMonth = 1; 
	_requestYear = 15; 

};
//_____________________________________________________________________________

inline CSerialDev* SfTransaction::getSerialDev()
{
	return ((CSerialDev*)_device);
}
//_____________________________________________________________________________

void SfTransaction::SetStartArchTime (void* devConfig) 
{
	string timeParamName;
	switch(_Command){
	case 0x14:
		timeParamName = "TD";
		break;
	case 0x15:
		timeParamName = "TH";
		break;
	case 0x16:
		timeParamName = "TI";
		break;
	case 0x17:
		timeParamName = "TA";
		break;
	default:
		return;
	}
	Json::Value * devCfg = (Json::Value *) devConfig;
	if((*devCfg)[timeParamName].isNull())
		return;
	string timeToSave = (*devCfg)[timeParamName].asString();
	string subTime = timeToSave.substr(0, 2);
	if(subTime.length() != 2)
		return;
	int sday = stoi(subTime);
	_requestDay = sday;
	subTime = timeToSave.substr(2, 2);
	if(subTime.length() != 2)
		return;
	int smonth = stoi(subTime);
	_requestMonth = smonth;
	subTime = timeToSave.substr(4, 4);
	if(subTime.length() != 4)
		return;
	int syear = stoi(subTime);
	getSerialDev ()->Mess->Out ("[%s] arch. start %02d.%02d.%04d", timeParamName.c_str(), /*timeToSave.c_str(), */sday, smonth, syear);
	_requestYear = syear - 2000;
}
//_____________________________________________________________________________

SfTransaction::~SfTransaction(void)
{
	if (_rawBufferIn != NULL)
	{
		delete[] _rawBufferIn;
		_rawBufferIn = nullptr;
	}
	if (_rawBufferOut != NULL)
	{
		delete[] _rawBufferOut;
		_rawBufferOut = nullptr;
	}		
}
//_____________________________________________________________________________

DWORD SfTransaction::GetCommand()
{
	return _Command;
}
//_____________________________________________________________________________

DWORD SfTransaction::GetInLength()
{
	prefix_t * prefix;
	prefix = (prefix_t *) _rawBufferIn;
	return prefix->size;
}
//_____________________________________________________________________________

DWORD SfTransaction::GetOutLength()
{
	prefix_t * prefix;
	prefix = (prefix_t *) _rawBufferOut;
	return prefix->size;
}
//_____________________________________________________________________________

bool SfTransaction::AddRawParameter(int offset, int size, UCHAR* value){
	if(size == 0)
		return FALSE;
	memcpy(_rawBufferOut + offset, value, size);
	return TRUE;
}
//_____________________________________________________________________________

bool SfTransaction::AddParameter(int offset, UINT size, string ptype, string value){
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
//_____________________________________________________________________________

bool SfTransaction::AddByteParameter(int offset, UINT size, UCHAR value)
{
	_rawBufferOut[offset] = value;
	return TRUE;
}
//_____________________________________________________________________________

bool SfTransaction::AddFloatParameter(int offset, UINT size, float value)
{
	*(float*)(_rawBufferOut + offset) = value;
	return TRUE;
}
//_____________________________________________________________________________

PROC_RESP_RETVAL SfTransaction::ProcessResponse(DWORD size)
{
	//DWORD starttick = ::GetTickCount();	
#if DEBUG_RAW_DATA
	if (getSerialDev()->GetDebug() & DEBUG_RAW_DATA){
		char logbuf[1024];
		char const hx[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

		int index, bsize;
		bsize = 0;
		for(index = 0; index < _rawBufferOutSize; index++){
			logbuf[bsize++] = hx[(_rawBufferOut[index]>>4)&0xf];
			logbuf[bsize++] = hx[_rawBufferOut[index]&0x0f];
			logbuf[bsize++] = ' ';
		}
		logbuf[bsize++] = 0;

		Mess->Out ("(%d) OUT: %s", _Command, logbuf);
		bsize = 0;
		for(index = 0; index < _rawBufferInSize; index++){
			logbuf[bsize++] = hx[(_rawBufferIn[index]>>4)&0xf];
			logbuf[bsize++] = hx[_rawBufferIn[index]&0x0f];
			logbuf[bsize++] = ' ';
		}
		logbuf[bsize++] = 0;

		Mess->Out ("(%d) IN : %s", _Command, logbuf);
	}
#endif

	if(size < sizeof(prefix_t))
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
		string asParName = _params[seq_num_param]["Name"].asString();
		if(asParName.compare("RunNumber") == 0) 
		{
			// compare 4 byte in and out buffer in case of presense Run Number in Request and Response
			if(_rawBufferIn[4] != _rawBufferOut[4] && (size > sizeof(_prefixIn)+2)){
				getSerialDev ()->Mess->Out ("[%s][%s]: Resp and Request RunNumber different %d <> %d",
										 asParamName.c_str(), caCommand, _rawBufferIn[4], _rawBufferOut[4]);
				return PROC_RESP_RETVAL_FAIL;
			}
		}	

		if(asParName.compare("Request_Sequence_Number") == 0) 
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
	//Mess->Out ("SfTransaction::ProcessResponse %d msec\n", ::GetTickCount() - starttick);

	return PROC_RESP_RETVAL_SUCCESS;
}
//_____________________________________________________________________________

void SfTransaction::ReadDone()		 
{
	DWORD starttick = ::GetTickCount();

	switch(_prefixIn->cmd){
	case 0xAA: // successful response to command R42 save all binary packet data to simple write generation
		vector<UCHAR> data43(_rawBufferIn+5, _rawBufferIn+(_rawBufferInSize-2-6));
		//getSerialDev()->getStorage()->SavePacket(GetDeviceName()+".R43."+_RunNumber, data);
		getSerialDev()->getStorage()->SavePacket("R43."+_RunNumber, data43);

		//vector<UCHAR> data8(_rawBufferIn+21, _rawBufferIn+37);		 // density, co2, n2, pressure ADD//
		//data8.insert(data8.end(), _rawBufferIn+49, _rawBufferIn+53); // specific energy
		getSerialDev()->getStorage()->SavePacket("R8."+_RunNumber, data43);
		break;
	}
}
//_____________________________________________________________________________

bool SfTransaction::CheckResponseCRC()
{
	return ExtCRC16(_rawBufferIn, FALSE);
}
//_____________________________________________________________________________

/// \brief This function returns type of parameter according to its string name
PARAM_TYPE SfTransaction::GetParamTypeFromString(string stype)
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
	else 
	if(stype.compare("bcd") == 0)
	{
		return SFPARAM_TYPE_BCD;
	}
	else 
	if(stype.compare("bcdscaled") == 0)
	{
		return SFPARAM_TYPE_BCDSCALED;
	}
	else 
	if(stype.compare("floatbcd") == 0)
	{
		return SFPARAM_TYPE_FLOATBCD;
	}

	return SFPARAM_TYPE_STRING;
}
///_____________________________________________________________________________
// \brief Sets dummy parameter to datastorage in case of bad response  
void SfTransaction::SetBadResponse()
{
	TransactionParam tp(0, SFPARAM_TYPE_BYTE, 1, "ConnectionStatusBAD", _rawBufferIn);
	getSerialDev()->getStorage()->SetValue(GetCommand(), _RunNumber, "ConnectionStatusBAD", tp);
	//Storage.SetValue(GetDeviceName(), GetCommand(), _RunNumber, "ConnectionStatusBAD", tp);
}
///_____________________________________________________________________________
// \brief Parse and store parameters to Storage to be ready for scada's High Level 
// \return 0 on simple response, 1 on compex response with continuation
int SfTransaction::ParseResponse()
{
	UCHAR respStatus = 0;
	int paramsInResponse = _deviceRequest["ParamsInList"].size();
	//DWORD ticks_start = ::GetTickCount();
	//int paramsCnt = 0;
	string atype = _deviceRequest["Type"].asString();
	if(atype.compare("Archive") == 0)
	{
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
		int smonth = 0;
		int sday = 0;
		int syear = 0;
		int shour = 0;
		bool resetdate = false;
		for(packIndx; packIndx < packNum; packIndx++) {
			//DataStorage::tStorageParams sp;
			DevStorage::tStorageParams sp;
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
				string unit = "";//_deviceRequest["ParamsInList"][paramNum]["unit"].asString(); // not used

				PARAM_TYPE type = GetParamTypeFromString(stype);
						
				//name += _RunNumber; moved to AddRecord
				TransactionParam tp(offset, type, size, unit, _rawBufferIn + offset + addidtion);	
				sp[name] = tp;
				if(name.compare("Record_Month") == 0){
					smonth = tp.GetData()[0];
					resetdate = true;
				} else
				if(name.compare("Record_Year") == 0){
					syear = tp.GetData()[0];
					resetdate = true;
				} else
				if(name.compare("Record_Day") == 0){
					sday = tp.GetData()[0];
					resetdate = true;
				} else
				if(name.compare("Record_Hour") == 0){
					shour = tp.GetData()[0];
					resetdate = true;
				}
/*				if(name.compare("Data_Field")){
					TransactionParam tp(offset, SFPARAM_TYPE_FLOAT, size, unit, _rawBufferIn + offset + addidtion);	
					sp["Data_Field_Float"] = tp;
				}
*/
				//Storage.AddMultiValue(GetDeviceName(), GetCommand(), name, tp);
			}
			getSerialDev()->getStorage()->AddRecord(GetCommand(), _RunNumber, sp);
			//Storage.AddRecord(GetDeviceName(), GetCommand(), _RunNumber, sp);
			addidtion += recSize;
		}
		if(resetdate){
			_requestHour = shour;
			_requestMonth = smonth;
			_requestYear = syear;
			_requestDay = sday;
			getSerialDev ()->Mess->Out ("[%s][%s] new request time: %02d.%02d.%02d %02d h",
																	asParamName.c_str (), caCommand, _requestDay, _requestMonth, _requestYear, _requestHour);
		}
		// archive processing

		getSerialDev ()->Mess->Out ("[%s][%s]: Archives parsed, %d", asParamName.c_str (), caCommand, packNum);
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
			PARAM_TYPE type = GetParamTypeFromString(stype);
			
			TransactionParam tp(offset, type, size, unit, _rawBufferIn + offset);
			getSerialDev()->getStorage()->SetValue(GetCommand(), _RunNumber, name, tp);
			//Storage.SetValue(GetDeviceName(), GetCommand(), _RunNumber, name, tp);
			//paramsCnt++;
		}
	}

	return respStatus;
}
///____________________________________________________________________________

bool SfTransaction::CreateRequest()
{
	_rawBufferOut[0] = 0xAA;
	_rawBufferOut[1] = _Addr; // addr
	_rawBufferOut[2] = (UCHAR)_rawBufferOutSize; // count
	_rawBufferOut[3] = _Command; // addr
	
	return TRUE;
}
///____________________________________________________________________________

bool SfTransaction::PrepareData()
{
	vector<UCHAR> v;
	switch (_Command)
	{
	case 43: // Write static parameters. Need previous command to read static data.
		v = getSerialDev()->getStorage()->GetPacket("R43."+_RunNumber);
		if(v.size() == 0) 
			return FALSE;
		AddRawParameter(21, v.size(), v.data()); // copy vector at offset 21
		break;
	case 8: // Write static parameters short form. Need previous command to read static data.
		v = getSerialDev()->getStorage()->GetPacket("R8."+_RunNumber);
		v.erase(v.begin(), v.begin() + 16);//ADD
		if(v.size() == 0) 
			return FALSE;
		AddRawParameter(21, 12, v.data()); // copy  3 parameters//ADD
		TransactionParam tp;//ADD:
		bool res = getSerialDev()->getStorage()->GetValue(42, _RunNumber, "Atmospheric_Pressure", tp);
		if(!res || (tp.GetSize() != sizeof(float)))
			return FALSE;
		int seq_num = _params.size();
		_params.resize(seq_num+1);
		_params[seq_num]["Name"] = "Atmospheric_Pressure";
		float valf = *(float*)tp.GetData().data();
		_params[seq_num]["Value"] = valf;

		res = getSerialDev()->getStorage()->GetValue(42, _RunNumber, "Specific_Energy", tp);
		if(!res || (tp.GetSize() != sizeof(float)))
			return FALSE;
		seq_num++;
		_params.resize(seq_num+1);
		_params[seq_num]["Name"] = "Specific_Energy";
		valf = *(float*)tp.GetData().data();
		_params[seq_num]["Value"] = valf;
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
					} else if(_params[po]["Value"].type() == Json::realValue){
						float fval = _params[po]["Value"].asFloat();
						if(fval != fval)
							break;
						AddRawParameter(offset, size, (UCHAR*)(&fval));
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
	switch (_Command)
	{
	case 43: // Write static parameters. Need previous command to read static data.
		memcpy(v.data(), _rawBufferOut + 21, v.size());
		getSerialDev()->getStorage()->SavePacket("R43."+_RunNumber, v);
		AddRawParameter(21, v.size(), v.data()); // copy vector at offset 21
		break;
	}
	
	if(!ExtCRC16(_rawBufferOut, TRUE))
		return FALSE;
	return TRUE;
}
///____________________________________________________________________________

void SfTransaction::PrepareToSend() {
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
				if (!_needContinuation)
				{
					DateTime["Starting_Month"] = _requestMonth;
					DateTime["Starting_Day"] = _requestDay;
					DateTime["Starting_Year"] = _requestYear;
					DateTime["Starting_Hour"] = _requestHour;
					DateTime["Ending_Month"] = tmCurrent.tm_mon+1;
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
		ExtCRC16(_rawBufferOut, TRUE);

		// remember last date
		if(!DateTime.empty()) 
		{
			getSerialDev ()->Mess->Out ("[%s][%s] request time %02d.%02d.%02d %02d h", asParamName.c_str (), 
																	caCommand, _requestDay, _requestMonth, _requestYear, _requestHour);
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
		ExtCRC16(_rawBufferOut, TRUE);
		break;
	}
};
///____________________________________________________________________________

void SfTransaction::SendDone()		 { cout << "SendDone SF\n";};
void SfTransaction::PrepareToRead()	 { cout << "PrepareToRead SF\n";};
///____________________________________________________________________________

bool SfTransaction::ExtCRC16 (UCHAR* extBuf, bool writeflag) {
	unsigned char uchCRCHi = 0xFF ; 
	unsigned char uchCRCLo = 0xFF ; 
	unsigned uIndex ; /* will index into CRC lookup table */
	prefix_t * ppref = (prefix_t *) extBuf;
	int len = ppref->size-2;//bytecnt-2;
	UCHAR * buf = extBuf;
	if(ppref->size < 4)
		return FALSE;
	while (len--) /* pass through message buffer */
	{
		uIndex = uchCRCLo ^ *buf++ ; /* calculate the CRC */
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ;
		uchCRCHi = auchCRCLo[uIndex] ;
	}
	//USHORT crcval = (uchCRCHi << 8 | uchCRCLo) ;
	int index = ppref->size-2;
	if(writeflag) {
		extBuf[index] = uchCRCLo;
		extBuf[index+1] = uchCRCHi;
	} else{
		if((extBuf[index] != uchCRCLo && extBuf[index+1] != uchCRCHi))
			return FALSE; 
	}
	return TRUE;
}