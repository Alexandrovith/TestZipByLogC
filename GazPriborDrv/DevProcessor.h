#pragma once
#include "stdafx.h"
#include "SerialDev.h"
#include "json.h"

using namespace std;

namespace DevProc
{
	typedef vector<CSerialDev*> TDevices;
	typedef struct STagDescription
	{
		string ParamName;   ///< parameter name
		string RunNumber;	///< additional param to separate the same params with different RunNumber 
		string MapID;		///< parameter name
		UINT Command;		///< command identifier
		CSerialDev* dev;
	} TagDescription;
	extern HANDLE ghDevicesMutex;

	bool isNeedStop ();
	void SetNeedStop ();
	bool isLoopStarted ();
	extern Json::Value DevicesConfig;		///< This contains config of the devices
	extern Json::Value DeviceTypesConfig;	///< This contains supported device types
	extern TDevices Devices; ///< This vector contains devices list

	/// \brief This struct defines data describing tag request
	extern map<int, TagDescription> tags;

	BOOL processConfig (string jsonConfig, Json::Value * jsonCfg, string keyTree);
	BOOL processConfigFile (LPCTSTR configName, Json::Value * jsonCfg, string keyTree);
	void lockDevices ();
	void unlockDevices ();
	CSerialDev* CreateDevice (const Json::Value & DeviceConfig);
	CSerialDev* FindSerialDevice (string & DevicePort, const Json::Value & DeviceConfig);
	CSerialDev* FindSerialDeviceByName (string deviceName);
	CSerialDev* FindSerialDeviceByName2 (string deviceName, TDevices Devs);
	const Json::Value FindDeviceConfig (string & DeviceName);
	//const Json::Value FindRequestForReqName(const Json::Value & devTypeConfig, string & reqName);
	int FindRequestForReqName (const Json::Value & devTypeConfig, string & reqName);
	const Json::Value FindRequestForReqNameParam (const Json::Value & devTypeConfig, string & reqName, string & param);
	bool CheckParamInCommand (DWORD ncmd, Json::Value & devTypeConfig, string & param);
	//const Json::Value FindRequestForParameter(const Json::Value & devTypeConfig, string & param);
	int FindRequestForParameter (const Json::Value & devTypeConfig, string & param);
	bool Subscribe (int tagId, string address);
	void DevProcessingLoop (void * arg);
	bool WriteValue (const char * address, char * value, int size);
	bool Write4Values (const char * address, float values[4]);
	int GetValue (int tagId, UCHAR * bufToStore);
	string GetArchiveData (string address);
	string MakeAddress (string DeviceName, string ReqName, string paramName, string RunNumber);
	void CloseDevs ();
	int StopStartDev (char * cpDevConfig, bool bStop);
}

//void FileLogMessage (string message);