#include "stdafx.h"
#include "DataStorage.h"
#include <iostream>
#include <string>
#include "Common.h"

#pragma warning(disable : 4503)

DataStorage::DataStorage(void)
{
	InitLock();
}

DataStorage::~DataStorage(void)
{
	DelLock();
}

/*
int DataStorage::GetVectorSize(string device, int reqNumber){
	LOCK;
	tStorageDataIt it = _storageData.find(device);
	if(it == _storageData.end()){
		UNLOCK;
		return 0;
	}
	tReqDataIt reqIt = _storageData[device].find(reqNumber);
	if(reqIt == _storageData[device].end()){
		UNLOCK;
		return 0;
	}

	UNLOCK;
	return _storageData[device][reqNumber].size();
}*/

/// \brief add new record for device storage
/// \param device name of device 
/// \param reqNumber request number
void DataStorage::AddRecord(string device, int reqNumber, string runNumber, DataStorage::tStorageParams sp)
{
	LOCK;

	tStorageDataIt it = _storageData.find(device);
	if(it == _storageData.end()){
		//create new device entrance
		//storageParams 
		auto res = _storageData.emplace(device, tReqData());
		if(res.second == FALSE){
			UNLOCK;
			return ;
		}
		it = res.first;
	}
	
	int runNumberInt = reqNumber;
	if(runNumber.size())
		runNumberInt += 256*stoi(runNumber);  // 15-8 bits as runNumber, 7-0 bits as reqNumber

	tReqDataIt reqIt = _storageData[device].find(runNumberInt);
	if(reqIt == _storageData[device].end()){
		vector<tStorageParams> vec;
		auto res = _storageData[device].emplace(runNumberInt, vec);
		if(res.second == FALSE){
			UNLOCK;
			return ;
		}
		reqIt = res.first;
	}

	_storageData[device][runNumberInt].push_back(sp);

	UNLOCK;
}


/// \brief add parameter and value to storage
/// \param device name of device 
/// \param reqNumber request number
/// \param paramName name of parameter to store
/// \param param TransactionParam object
int DataStorage::AddMultiValue(string device, int reqNumber, string paramName, TransactionParam &param)
{
	//tStorageParams *storageParams;
	LOCK;
	//tStorageParams sp = _storageData[device][reqNumber][0];
	_storageData[device][reqNumber].back()[paramName] = param;
	UNLOCK;
	return 0;
}


/// \brief save parameter and value to storage
/// \param device name of device 
/// \param paramName name of parameter to store
/// \param param TransactionParam object
int DataStorage::SetValue(string device, int reqNumber, string runNumber, string paramName, TransactionParam &param)
{
	//tStorageParams *storageParams;
	LOCK;
	tStorageDataIt it = _storageData.find(device);
	if(it == _storageData.end()){
		//create new device entrance
		//storageParams 
		auto res = _storageData.emplace(device, tReqData());
		if(res.second == FALSE){
			UNLOCK;
			return 1;
		}
		it = res.first;
	}

	int runNumberInt = reqNumber;
	if(runNumber.size())
		runNumberInt += 256*stoi(runNumber);  // 15-8 bits as runNumber, 7-0 bits as reqNumber

	tReqDataIt reqIt = _storageData[device].find(runNumberInt);
	if(reqIt == _storageData[device].end()){
		vector<tStorageParams> vec;
		auto res = _storageData[device].emplace(runNumberInt, vec);
		if(res.second == FALSE){
			UNLOCK;
			return 1;
		}
		reqIt = res.first;
	}
	if(_storageData[device][runNumberInt].size() == 0){
		tStorageParams pr;
		_storageData[device][runNumberInt].push_back(pr);
	}

	//tStorageParams sp = _storageData[device][reqNumber][0];
	if(paramName.compare("ConnectionStatusBAD") == 0){
		_storageData[device][runNumberInt][0][paramName] = param;
	} else {
		tStorageParamsIt paramit_dummy = ((*reqIt).second)[0].find("ConnectionStatusBAD");
		if(paramit_dummy != ((*reqIt).second)[0].end()){
			_storageData[device][runNumberInt][0].erase(paramit_dummy);
		}
		_storageData[device][runNumberInt][0][paramName] = param;
	}
	UNLOCK;
	return 0;
}

/// \brief save binary packet
/// \param keyValue name of key to store Ex: "Brest.R42.1"
/// \param runNumber for which runNumber store binary packet
int DataStorage::SavePacket(string keyValue, vector<UCHAR> data)
{
	LOCK;
	tStoragePackIt it = _storagePack.find(keyValue);
	if(it == _storagePack.end()){
		//create new device entrance
		auto res = _storagePack.emplace(keyValue, data);
		UNLOCK;
		if(res.second == FALSE){
			return 1;
		}
		return 0;
	}

	_storagePack[keyValue] = data;
	UNLOCK;
	return 0;
}

vector<UCHAR> DataStorage::GetPacket(string keyValue)
{
	LOCK;
	vector<UCHAR> data = _storagePack[keyValue];
	UNLOCK;
	return data;
}

bool DataStorage::GetValue(string device, int reqNumber, string runNumber, string paramName, TransactionParam &param)
{
	LOCK;
	tStorageDataIt it = _storageData.find(device);
	if(it == _storageData.end()){
		UNLOCK;
		return FALSE;
	}
	//tReqData req = ((*it).second);
	int runNumberInt = reqNumber;
	if(runNumber.size())
		runNumberInt += 256*stoi(runNumber);  // 15-8 bits as runNumber, 7-0 bits as reqNumber

	tReqDataIt reqIt = _storageData[device].find(runNumberInt);
	if(reqIt == _storageData[device].end()){
		UNLOCK;
		return FALSE;
	}
	//vector<tStorageParams> vec = ((*reqIt).second);

	//tStorageParams params = vec[0];
	// Check for dummy parameter "ConnectionStatusBAD" presence
	tStorageParamsIt paramit_dummy = ((*reqIt).second)[0].find("ConnectionStatusBAD");
	if(paramit_dummy != ((*reqIt).second)[0].end()){
		UNLOCK;
		return FALSE;
	}

	tStorageParamsIt paramit = ((*reqIt).second)[0].find(paramName);
	if(paramit == ((*reqIt).second)[0].end()){
		UNLOCK;
		return FALSE;
	}
	param = (*paramit).second;
	UNLOCK;
	return TRUE;
}

/// \brief This function converts storage param data to archive param
Json::Value DataStorage::ConvertStorageParamToArchiveParam(tStorageParamsIt & recIt)
{
	int valsize = recIt->second.GetSize();
	int tp = recIt->second.GetType();
	int val;
	char tmpData[256];
	vector<UCHAR> data = recIt->second.GetData(); 
	Json::Value param;


//			string tmpStr(data.begin(), data.begin()+valsize);
/*			for(int i=0; i<valsize; i++){}
		sprintf(tmpData+i*2, "%02X", data.data());
	}
	*/

	// save name
	param["Name"] = recIt->first;		

	switch(tp){
	case SFPARAM_TYPE_BYTE:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%02X", data[0]);		
		param["Type"] = "byte";
		break;
	case SFPARAM_TYPE_YEAR:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%02X", data[0]);		
		param["Type"] = "year";
		break;
	case SFPARAM_TYPE_MONTH:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%02X", data[0]);		
		param["Type"] = "month";
		break;
	case SFPARAM_TYPE_DAY:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%02X", data[0]);		
		param["Type"] = "day";
		break;
	case SFPARAM_TYPE_HOUR:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%02X", data[0]);
		param["Value"] = tmpData;
		param["Type"] = "hour";
		break;
	case SFPARAM_TYPE_MINUTE:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%02X", data[0]);		
		param["Type"] = "minute";
		break;
	case SFPARAM_TYPE_SECOND:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%02X", data[0]);		
		param["Type"] = "second";
		break;
	case SFPARAM_TYPE_FLOAT:			
		sprintf_s(tmpData, sizeof(tmpData)-1, "%08X", *(UINT*)(data.data()));		
		param["Type"] = "float";
		break;
	case SFPARAM_TYPE_INT:		
		sprintf_s(tmpData, sizeof(tmpData)-1, "%08X", *(UINT*)(data.data()));
		param["Type"] = "integer";
		break;
	case SFPARAM_TYPE_BCD:		
		//sprintf_s(tmpData, sizeof(tmpData)-1, "%08X", *(UINT*)(data.data()));
		for(int i=0; i<4; i++){
			sprintf_s(&tmpData[i*2], sizeof(tmpData)-1-i*2, "%02X", data[i]); 
		}
		param["Type"] = "bcd";
		break;
	default:
		for(int i=0; i<valsize; i++){
			sprintf_s(&tmpData[i*2], sizeof(tmpData)-1-i*2, "%02X", data[i]);  // TODO fix size
		}				
		param["Type"] = "string";
	}

	// save generated value
	param["Value"] = tmpData;

	return param;
}

string DataStorage::GetArchiveData(string device, int reqNumber, string runNumber){
	LOCK;
	tStorageDataIt it = _storageData.find(device);
	if(it == _storageData.end()){
		UNLOCK;
		return string("");
	}
	//tReqData req = ((*it).second);
	
	int runNumberInt = reqNumber;
	if(runNumber.size())
		runNumberInt += 256*stoi(runNumber);  // 15-8 bits as runNumber, 7-0 bits as reqNumber
	tReqDataIt reqIt = _storageData[device].find(runNumberInt);
	if(reqIt == _storageData[device].end()){
		UNLOCK;
		return string("");
	}
	vector<tStorageParams> vec = ((*reqIt).second);
	
	Json::Value root;
	Json::Value records(Json::arrayValue);  
	Json::Value record;
	Json::Value params(Json::arrayValue);
	Json::Value param;
	bool saveFloat;
	int paramNumber = 0;
	for(UINT indx = 0; indx < vec.size(); indx++){
		saveFloat = false;
		for(tStorageParamsIt recParIt = vec[indx].begin(); recParIt != vec[indx].end(); recParIt++)
		{		
			
			param = ConvertStorageParamToArchiveParam(recParIt);
			if(reqNumber == 22  && 
				recParIt->first.compare("Parameter_Number") == 0){
				paramNumber = (*recParIt).second.GetData()[0];
			}
			if(reqNumber == 23  && 
				recParIt->first.compare("Data_Field") == 0 &&
				saveFloat) {
					param["Type"] = "float";
			}
			params.append(param);
			if(reqNumber == 23 ) {
				if(param["Name"].compare("Alarm_Code") == 0) {
					int acode = (*recParIt).second.GetData()[0];
					switch(acode){
					//case 0:
					//case 3:
					//case 6:
					case 8:
					case 10:
					case 131:
					//case 134:
					case 136:
					case 138:
						saveFloat = true;
					}
				}
			}
		}
		if(reqNumber == 22  && 
			(paramNumber == 128 || paramNumber == 129)) {
				for(int i = 0; i<params.size(); i++){
					string fname = params[i]["Name"].asString();
					if(fname.compare("Old_Value") == 0 || fname.compare("New_Value") == 0){
						if(paramNumber == 128)
							params[i]["Type"] = "time";
						else
							params[i]["Type"] = "date";
					} 
				}
		}
		record["Params"] = params;
		params.clear();
		records.append(record);
	}
	if(vec.size())
		LOG("Archive[%d.%s]: %d records", reqNumber, runNumber.c_str(), vec.size());

	root["Records"] = records;
	Json::FastWriter writer;
	string outputString = writer.write(root);
	((*reqIt).second).clear();
	UNLOCK;
	return 	outputString;
}