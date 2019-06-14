// GazPriborDrv.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "Common.h"
#include "DevProcessor.h"
#include "GazPriborDrv.h"


std::shared_ptr<Messages::CMess> MessGeneral;
string asDirAssembly;
HANDLE hThread;

//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww

BOOL APIENTRY DllMain (HMODULE hModule,
											 DWORD  ul_reason_for_call,
											 LPVOID lpReserved
)
{
	setlocale (LC_ALL, "Russian");

	if (MessGeneral == nullptr)
	{
		char cpModuleFileName[256];
		GetModuleFileNameA ((HMODULE)hModule, cpModuleFileName, sizeof (cpModuleFileName));
		asDirAssembly = cpModuleFileName;
		Messages::CMessages::ExtractDir (asDirAssembly, asDirAssembly);
		MessGeneral = make_shared<Messages::CMess> (asDirAssembly, "SF");
	}
	//switch (ul_reason_for_call)
	//{
	//case DLL_PROCESS_ATTACH:
	//case DLL_THREAD_ATTACH:
	//case DLL_THREAD_DETACH:
	//case DLL_PROCESS_DETACH:
	//	break;
	//}
	return TRUE;
}

///_____________________________________________________________________________
// \brief Parsing config from string
// \param jsonConfig string with config
// \param object for storing device configuration
// \keyTree key to find in configuration as root element
// \return TRUE on success
BOOL processConfig (string jsonConfig, Json::Value * jsonCfg, string keyTree)
{
	Json::Reader reader;
	if (!reader.parse (jsonConfig, *jsonCfg))
		return FALSE;
	int devicesFound = (*jsonCfg)[keyTree].size (); // "Devices"
	MessGeneral->Out ("Configuration: %d values in %s", devicesFound, keyTree.c_str ());

	if (devicesFound == 0)
	{
		MessGeneral->Out ("Configuration: Values for %s not found.", keyTree.c_str ());
		return FALSE;
	}
	return TRUE;
}
//_____________________________________________________________________________

BOOL processConfigFile (char* configName, Json::Value * jsonCfg, string keyTree)
{
	string json;
	ifstream myfile (MessGeneral->DirAssembly () + "\\" + configName);//binDir

	if (myfile.is_open ())
	{
		while (!myfile.eof ())
		{
			string line;
			myfile >> line;
			json += line;
		}
		myfile.close ();
		return processConfig (json, jsonCfg, keyTree);
	}
	return FALSE;
}
//_____________________________________________________________________________

EXTERN_C GAZPRIBORDRV_API void Init ()
{
	InitConfig (NULL);
}
//_____________________________________________________________________________

EXTERN_C GAZPRIBORDRV_API char* GetName ()
{
	return "SuperFlo";
}
//_____________________________________________________________________________

char strver[20];
extern "C" GAZPRIBORDRV_API char* GetVer ()
{
	int rev = atoi (VERSION_REVISION + 10);
	_snprintf_s (strver, sizeof (strver) - 1, "%d.%d.%04d", VERSION_MAJOR, VERSION_MINOR, rev);
	return strver;
}
//_____________________________________________________________________________

bool bBeginThread = false;

/// \brief This function initializes driver and starts main device loop
EXTERN_C GAZPRIBORDRV_API void InitConfig (char* jsonConfig)
{
	if (jsonConfig == NULL)
	{
		if (!processConfigFile ("Devices.config", &DevProc::DevicesConfig, "Devices"))
		{
			cout << "Can't process Devices.config file, terminating..." << endl;
			return;
		};
	}
	else
	{
		if (!processConfig (string (jsonConfig), &DevProc::DevicesConfig, "Devices"))
		{
			cout << "Can't process config from SCADA, terminating..." << endl;
			return;
		}
	}
	if (!processConfigFile ("DeviceTypes.config", &DevProc::DeviceTypesConfig, "DeviceTypesList"))
	{
		cout << "Can't process DeviceTypes.config file, terminating..." << endl;
		return;
	};
	DevProc::ghDevicesMutex = CreateMutex (
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);

	bBeginThread = true;
	hThread = (HANDLE)_beginthread (DevProc::DevProcessingLoop, 0, NULL);
}
///_____________________________________________________________________________
// \brief This function is used to subscribe on parameter given by address string
// \param tagId tag Id to assign for given address 
// \param address string describing parameter
// \return NULL in case of error
EXTERN_C GAZPRIBORDRV_API int Subscribe (int tagId, char * address)
{
	return DevProc::Subscribe (tagId, string (address));
}
///_____________________________________________________________________________
// \brief Save some attributes of parametergiven by address string
// \param tagId tag Id to assigned for given address 
// \param address string describing parameter
// \param value value to write
// \param size bytes count to write
// \return NULL in case of error
EXTERN_C GAZPRIBORDRV_API int WriteValue (char * address, char* value, int size)
{
	return DevProc::WriteValue (address, value, size);
}
///_____________________________________________________________________________
// \brief Save some attributes of parametergiven by address string
// \param tagId tag Id to assigned for given address 
// \param address string describing parameter
// \param value value to write
// \param size bytes count to write
// \return NULL in case of error
GAZPRIBORDRV_API int Write4Values (char * address, float values[4])
{
	return DevProc::Write4Values (address, values);
}
///_____________________________________________________________________________
// \brief Gets value for parameter assigned with tagid
// \param tagId tag Id of parameter
// \return integer value of parameter
EXTERN_C GAZPRIBORDRV_API int GetValue (int tagId, UCHAR* bufToStore)
{
	return DevProc::GetValue (tagId, bufToStore);
}

char archiveTemp[1024 * 1024];///< This array is used to store temporary archive data
///_____________________________________________________________________________
// \brief This function returns archives data for given tagid
// \param tagId tag Id of parameter
// \return pointer to archive data
EXTERN_C GAZPRIBORDRV_API int * GetNewArchiveData (char * address)
{
	string str = DevProc::GetArchiveData (address);
	int strLen = str.length ();

	if (strLen < 14)
	{
		str = "{\"Records\":[]}";
		strLen = str.length ();
	}
	memcpy (archiveTemp, str.c_str (), min (strLen + 1, sizeof (archiveTemp)));
	return (int *)archiveTemp;
}
//_____________________________________________________________________________

EXTERN_C GAZPRIBORDRV_API int StopStartDev (char * cpDevConfig, bool bStop)
{
	return DevProc::StopStartDev (cpDevConfig, bStop);
}
///_____________________________________________________________________________
// \brief This function closes driver
EXTERN_C GAZPRIBORDRV_API void Close ()
{
	if (bBeginThread)
	{
		bBeginThread = false;
		MessGeneral->Out ("Останов Superflo ...");

		DevProc::SetNeedStop ();
		while (DevProc::isLoopStarted ())
			Sleep (0);
		DevProc::CloseDevs ();
		::CloseHandle (DevProc::ghDevicesMutex);

		MessGeneral->Out ("Superflo остановлены");
		//::CloseHandle (hThread);
	}
}

