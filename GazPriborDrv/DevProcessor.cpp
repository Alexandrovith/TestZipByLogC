// DevProcessor.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "stdio.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <process.h>

#include <FindPair.h>
#include "Common.h"
#include "SfTransaction.h"
#include "DevProcessor.h"
#include "Logs_C.h"
#include "GazPriborDrv.h"
#include "StopStartDev.h"

#define CHECK_TIME
#define FINISHCHECK_TIME(name_c)

using namespace std;

namespace DevProc
{
	HANDLE ghDevicesMutex;
	volatile bool devLoopStarted = TRUE;
	volatile bool bNeedStop = FALSE;

	//< This contains config of the devices
	Json::Value DevicesConfig;
	//< This contains supported device types
	Json::Value DeviceTypesConfig;
	//< This vector contains devices list
	TDevices Devices;
	map<int, TagDescription> tags;

	shared_ptr<CStopStartDev> StopStartDevs = make_shared<CStopStartDev> ();


	//___________________________________________________________________________

	bool isNeedStop () { return bNeedStop; }
	void SetNeedStop () { bNeedStop = true; }
	bool isLoopStarted () { return devLoopStarted; }

	// \brief Parses config from string
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
		MessGeneral->Out ("Configuration. %d values in %s", devicesFound, keyTree.c_str ());

		//int deviceTypesFound = jsonCfg["DeviceTypesList"].size();
		//Mess->Out ("Configuration: %d device types found\n", devicesFound);

		if (devicesFound == 0)
		{
			MessGeneral->Out ("Configuration. Values for %s not found", keyTree.c_str ());
			return FALSE;
		}
		return TRUE;
	}
	//___________________________________________________________________________

	void CloseDevs ()
	{
#ifdef _DEBUG
		MessGeneral->Out ("CloseDevs DESTR");
#endif
		for (CSerialDev* Dev : Devices)
		{
			if (Dev)
			{
				MessGeneral->Out ("Close %s", Dev->GetName ().c_str ());
				delete Dev;
				Dev = nullptr;
			}
		}
#ifdef _DEBUG
		MessGeneral->Out ("Before Devices.clear");
#endif
		Devices.clear ();
#ifdef _DEBUG
		MessGeneral->Out ("Before tags.clear");
#endif
		//		tags.clear ();
		//#ifdef _DEBUG
		//		MessGeneral->Out ("After tags.clear");
		//#endif
	}
	//___________________________________________________________________________

	BOOL processConfigFile (LPCTSTR configName, Json::Value * jsonCfg, string keyTree)
	{
		string json;
		ifstream myfile (configName);
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
	//___________________________________________________________________________

	void lockDevices ()
	{
		::WaitForSingleObject (ghDevicesMutex, INFINITE);  // no time-out interval
	}
	//___________________________________________________________________________

	void unlockDevices ()
	{
		::ReleaseMutex (ghDevicesMutex);
	}
	///___________________________________________________________________________
	// \brief This function creates device
	// \param DeviceConfig config of the device
	// \return pointer to created device if creation was successful
	CSerialDev* CreateDevice (const Json::Value & DeviceConfig)
	{
		bool foundSupportedType = FALSE;
		string port = DeviceConfig["Port"].asString ();

		string asDevName = DeviceConfig["Name"].asString ();

		if (port.empty ())
		{
			MessGeneral->Out ("CreateDevice %s: Port name is empty", asDevName.c_str ());
			return NULL;
		}

		string devType = DeviceConfig["Type"].asString ();
		UINT dtype;

		for (dtype = 0; dtype < DeviceTypesConfig["DeviceTypesList"].size (); dtype++)
		{
			if (DeviceTypesConfig["DeviceTypesList"][dtype]["Type"].asString ().compare (DeviceConfig["Type"].asString ()) == 0)
			{
				MessGeneral->Out ("Подкл.[%s.%s]", asDevName.c_str (), devType.c_str ());
				foundSupportedType = TRUE;
				break;
			}
		}

		if (!foundSupportedType)
		{
			return NULL;
		}

		int baudRate = DeviceConfig["BaudRate"].asInt ();
		int dataBits = DeviceConfig["DataBits"].asInt ();
		int parity = DeviceConfig["Parity"].asInt ();
		int stopBit = DeviceConfig["StopBit"].asInt ();
		int flowCtrl = DeviceConfig["FlowCtrl"].asInt ();

		CSerialDev * serialDev = new CSerialDev (asDevName, port, baudRate, dataBits, parity, stopBit, flowCtrl, devType, DeviceTypesConfig["DeviceTypesList"][dtype]["Protocol"].asString ());
		if (serialDev != NULL)
			Devices.push_back (serialDev);

		return serialDev;
	}
	///_____________________________________________________________________________
	// \brief This function looks for Serial Device 
	// \param deviceName Name of device to find
	// \return pointer to the serial device class object
	CSerialDev * FindSerialDeviceByName (string deviceName)
	{
		// check device existence
		for (UINT i = 0; i < Devices.size (); i++)
		{
			if (Devices[i]->GetName ().compare (deviceName) == 0)
				return Devices[i];
		}
		return 0;
	}
	///_____________________________________________________________________________
	// \brief This function looks for Devs by name
	// \param deviceName device config
	// \return pointer to the serial device class object
	CSerialDev* FindSerialDeviceByName2 (string deviceName, TDevices Devs)
	{
		for (UINT dev = 0; dev < Devs.size (); dev++)
		{
			if (Devs[dev]->GetName ().compare (deviceName) == 0)
			{
				return Devs[dev];
			}
		}
		return 0;
	}
	///_____________________________________________________________________________
	// \brief This function looks for Serial Device 
	// \param DevicePort name of the device port
	// \param DeviceConfig device config
	// \return pointer to the serial device class object
	CSerialDev * FindSerialDevice (string & DevicePort, const Json::Value & DeviceConfig)
	{
		// check device existance
		for (UINT dev = 0; dev < Devices.size (); dev++)
		{
			if (Devices[dev]->GetPort ().compare (DevicePort) == 0)
			{
				return Devices[dev];
			}
		}

		// here we are creating new 
		CSerialDev * dev = CreateDevice (DeviceConfig);
		return dev;
	}
	///_____________________________________________________________________________
	// \brief This function looks for Config of Device 
	// \param asDeviceName name of the device 
	// \return device config
	const Json::Value FindDeviceConfig (string & DeviceName)
	{
		Json::Value devcfg = DevicesConfig["Devices"];

		// find device config with given name
		int size = devcfg.size ();
		for (int i = 0; i < size; i++)
		{
			string name = devcfg[i]["Name"].asString ();
			if (name.compare (DeviceName) == 0)
			{
				Json::Value Device = devcfg[i];
				return Device;
			}
		}
		return Json::Value ();
	}
	///_____________________________________________________________________________
	// \brief This function looks for parameter in command
	// \param ncmd command number
	// \param devTypeConfig device type configuration
	// \return false if not exists
	bool CheckParamInCommand (DWORD ncmd, Json::Value & devTypeConfig, string & param)
	{
		// find command		const Json::Value Requests = devTypeConfig["Requests"];

		// find request with given command
		for (UINT ReqNum = 0; ReqNum < devTypeConfig["Requests"].size (); ReqNum++)
		{
			if (devTypeConfig["Requests"][ReqNum]["Command"].asInt () == ncmd)
			{
				// request found
				const Json::Value paramsIn = devTypeConfig["Requests"][ReqNum]["ParamsInList"];

				// check parameter existance
				for (UINT paramNum = 0; paramNum < paramsIn.size (); paramNum++)
				{
					if (paramsIn[paramNum]["Name"].asString () == param)
					{
						return true;
					}
				}
				return false;
			}
		}
		return false;
	}
	///_____________________________________________________________________________
	// \brief This function looks for reqName in device type
	// \param devTypeConfig config for given device type
	// \param param reqName of the parameter
	// \return number of request
	int FindRequestForReqName (const Json::Value & devTypeConfig, string & reqName)
	{
		// find command
		//const Json::Value Requests = devTypeConfig["Requests"];

		// find request with given command
		for (UINT ReqNum = 0; ReqNum < devTypeConfig["Requests"].size (); ReqNum++)
		{
			if (devTypeConfig["Requests"][ReqNum]["Name"].asString ().compare (reqName) == 0)
			{
				return ReqNum;
			}
		}
		return -1;
	}
	//_____________________________________________________________________________

	const Json::Value FindRequestForReqNameParam (const Json::Value & devTypeConfig, string & reqName, string & param)
	{
		// find command
		//const Json::Value Requests = devTypeConfig["Requests"];
		// find request with given command
		for (UINT ReqNum = 0; ReqNum < devTypeConfig["Requests"].size (); ReqNum++)
		{
			if (devTypeConfig["Requests"][ReqNum]["Name"].asString ().compare (reqName) == 0)
			{
				const Json::Value paramsIn = devTypeConfig["Requests"][ReqNum]["ParamsInList"];
				for (UINT paramNum = 0; paramNum < paramsIn.size (); paramNum++)
				{
					if (paramsIn[paramNum]["Name"].asString ().compare (param) == 0)
					{
				//MessGeneral->Out ("reqName=[%s] param=[%s] Subscribe", reqName.c_str(), param.c_str());
						return devTypeConfig["Requests"][ReqNum];
					}
				}
				//MessGeneral->Out ("reqName=[%s] param=[%s] NOSubscribe", reqName.c_str(), param.c_str());
				return Json::Value ();
			}
		}
				//MessGeneral->Out ("reqName=[%s] param=[%s] NOSubscribe", reqName.c_str(), param.c_str());
		return Json::Value ();
	}
	///_____________________________________________________________________________
	// \brief This function looks for parameter in device type
	// \param devTypeConfig config for given device type
	// \param param name of the parameter
	// \return request number 
	int FindRequestForParameter (const Json::Value & devTypeConfig, string & param)
	{
		// find command
		const Json::Value Requests = devTypeConfig["Requests"];

		// find request with given command
		for (UINT ReqNum = 0; ReqNum < Requests.size (); ReqNum++)
		{
			// request found
			const Json::Value paramsIn = Requests[ReqNum]["ParamsInList"];

			// check parameter existance
			for (UINT paramNum = 0; paramNum < paramsIn.size (); paramNum++)
			{
				if (paramsIn[paramNum]["Name"].asString ().compare (param) == 0)
				{
					return ReqNum;
				}
			}
		}
		return -1;
	}
	///_____________________________________________________________________________
	// \brief This is interface function to SCADA
	// \param tagId to be assigned to value
	// \param address of the value
	// \return True on success
	bool Subscribe (int tagId, string address)
	{
		// HERE TRY to create transaction and if success map parameter with transaction		
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;
		string RunNumber = "";
		string MapID = "";
		UINT Cmd = 0;
		string asDeviceName;

		CHECK_TIME;
		if (reader.parse (address, root))
		{
			Json::Value DeviceConfig;
			Json::Value paramsOut;
			CHECK_TIME;
			asDeviceName = root["DeviceName"].asString ();

			DeviceConfig = FindDeviceConfig (asDeviceName);

			if (DeviceConfig.empty ())
			{
				MessGeneral->Out ("[%s] Subscribe: ERROR: Device was not found", asDeviceName.c_str ());//, asDeviceName.c_str()
				return false;
			}

			string DevicePort = DeviceConfig["Port"].asString ();
			CHECK_TIME;
			lockDevices ();
			FINISHCHECK_TIME ("lockDevices");
			CSerialDev * serdev = FindSerialDevice (DevicePort, DeviceConfig);

			if (!serdev)
			{
				MessGeneral->Out ("[%s]Subscribe: ERROR getting Serial port %s", asDeviceName.c_str (), DevicePort.c_str ());
				unlockDevices ();
				return false;
			}
			//serdev->LockPortRes();

			CHECK_TIME;

			// get protocol config
			UINT devTypeNum;
			for (devTypeNum = 0; devTypeNum < DeviceTypesConfig["DeviceTypesList"].size (); devTypeNum++)
			{
				if (DeviceTypesConfig["DeviceTypesList"][devTypeNum]["Type"].asString ().compare (DeviceConfig["Type"].asString ()) == 0)
				{
					//DevType = DeviceTypesConfig["DeviceTypesList"][devTypeNum];
					break;
				}
			}
			FINISHCHECK_TIME ("DevTypeList");

			//looking for RunNumber param, if it differ for all Transactions.RunNumber we should add new Transaction
			paramsOut = root["ParamsOutList"];
			for (UINT po = 0; po < paramsOut.size (); po++)
			{
				string pname = paramsOut[po]["Name"].asString ();
				if (pname.compare ("RunNumber") == 0)
				{
					RunNumber = paramsOut[po]["Value"].asString ();
					break;
				}
			}

			FINISHCHECK_TIME ("subscribedevice");

			if (serdev->GetDebug () == 0)
			{
				if (!DeviceTypesConfig["DeviceTypesList"][devTypeNum]["Debug"].isNull ())
					serdev->SetDebug (DeviceTypesConfig["DeviceTypesList"][devTypeNum]["Debug"].asUInt ());
			}

			if (serdev->GetTryCounts () < 2)
			{
				if (!DeviceTypesConfig["DeviceTypesList"][devTypeNum]["TryCounts"].isNull ())
					serdev->SetTryCounts (DeviceTypesConfig["DeviceTypesList"][devTypeNum]["TryCounts"].asUInt ());
			}

			// check existing transactions have subscribing parameter
			const TransactionList * pTransactions = serdev->GetTransactionsList ();

			string RequestName = root["RequestName"].asString ();
			string asParamName = root["ParamName"].asString ();
			Json::Value Request;

			CHECK_TIME;

			if (RequestName.empty ())
			{
				unlockDevices ();
				return false;
				//Request = FindRequestForParameter(DeviceTypesConfig["DeviceTypesList"][devTypeNum], asParamName); ///< in case of "RequestName" absence look in all requests for param
			}
			if (RequestName.compare ("Virtual") == 0)
			{
				unlockDevices ();
				tags[tagId].Command = 0; // virtual param code
			}
			else
			{
				Request = FindRequestForReqNameParam (DeviceTypesConfig["DeviceTypesList"][devTypeNum], RequestName, root["ParamName"].asString ());

				if (Request.empty ())
				{
					unlockDevices ();
					return false;
				}

				UINT uiTr = 0;
				//for (Transaction *Transact : serdev->GetTransactions())_MY_
				//{
				//	DWORD ncmd = Transact->GetCommand ();
				//	int rcmd = Request["Command"].asUInt ();
				//	if (rcmd != ncmd)
				//	{
				//		uiTr++;
				//		continue;
				//	}
				//	// check device address on transaction
				//	if (Transact->GetAddr () == DeviceConfig["Address"].asUInt () &&
				//		Transact->GetRunNumber ().compare (RunNumber) == 0)
				//	{
				//		// look for parameter TODO -- set back CheckParam
				//		if (!root.get ("ParamName", "").empty () &&
				//				CheckParamInCommand (ncmd, DeviceTypesConfig["DeviceTypesList"][devTypeNum], root["ParamName"].asString ()))
				//		{
				//			break;
				//		}
				//	}
				//	uiTr++;
				//}

				for (uiTr = 0; uiTr < pTransactions->size (); uiTr++)
				{
					DWORD ncmd = (*pTransactions)[uiTr]->GetCommand ();
					int rcmd = Request["Command"].asUInt ();
					if (rcmd != ncmd)
						continue;
					// check device address on transaction

					if ((*pTransactions)[uiTr]->GetAddr () == DeviceConfig["Address"].asUInt () &&
						(*pTransactions)[uiTr]->GetRunNumber ().compare (RunNumber) == 0)
					{
						// look for parameter TODO -- set back CheckParam
						if (!root.get ("ParamName", "").empty () &&
								CheckParamInCommand (ncmd, DeviceTypesConfig["DeviceTypesList"][devTypeNum], root["ParamName"].asString ()))
						{
							break;
						}
					}
				}

				FINISHCHECK_TIME ("CheckParamInCommand");

				CHECK_TIME;
				if (uiTr != pTransactions->size ())
				{
					Cmd = (*pTransactions)[uiTr]->GetCommand ();
				}
				else
				{// transaction with given parameter was not found, so create new one					
					if (serdev->GetDevProtocol () == "SF")
					{
						SfTransaction * psf;
						//psf = new SfTransaction(asDeviceName, DeviceConfig["Address"].asUInt(), Request, paramsOut);
						psf = new SfTransaction (serdev, DeviceConfig["Address"].asUInt (), Request, paramsOut, root["ParamName"].asString ());
						if (RunNumber != "")
							psf->SetRunNumber (RunNumber);

						Cmd = psf->GetCommand ();
						psf->SetStartArchTime (&DeviceConfig);
						psf->SetTryCount (serdev->GetTryCounts ());
						serdev->AddRequest (psf);
					}
					else
					{
						serdev->Mess->Out ("[%s] Subscribe: ERROR: don't know how to create transactions for protocol %s",
															 root["ParamName"].asString ().c_str (), serdev->GetDevProtocol ().c_str ());
					}
				}
				FINISHCHECK_TIME ("transactions");
				unlockDevices ();
				tags[tagId].Command = Cmd;			// Add parameter to tags list
			}

			tags[tagId].dev = serdev;
			tags[tagId].ParamName = root.get ("ParamName", "").asString ();
			tags[tagId].RunNumber = RunNumber;
			if (tags[tagId].ParamName != "")
				tags[tagId].MapID = tags[tagId].ParamName + RunNumber;
			//#if _DEBUG
			//			if (serdev->GetDebug ())
			//				serdev->Mess->Out ("Subscribe[%lu] @ %d %s. %s %p", tagId, tags[tagId].Command,
			//													 tags[tagId].ParamName.c_str (), tags[tagId].RunNumber.c_str (), tags[tagId].dev);
			//#endif // DEBUG_SUBSCRIBE
		}
		else
		{
			MessGeneral->Out ("[%s] Subscribe ERROR: can not parse address string %s", asDeviceName.c_str (), address.c_str ());
			return false;
		}

		FINISHCHECK_TIME ("subscribe");
		return true;
	}
	///===========================================================================
	// \brief Thread for processing all devices in loop
	// \brief param passed to thread function additional param
	void DevProcessingLoop (void * arg)
	{
		MessGeneral->Out ("----- Waiting to start processing loop ----");
		Sleep (30000);
		MessGeneral->Out ("----- Start processing loop ----");
		size_t uiDevNum = 0;

		while (bNeedStop == false)
		{
			//time_t currenttime;time (&currenttime);
			lockDevices ();
			try
			{
				for (; uiDevNum < Devices.size () && bNeedStop == false; uiDevNum++)
				{
					Devices[uiDevNum]->Process ();
					StopStartDevs->Run ();
				}
				//StopStartDevs->Run ();
				uiDevNum = 0;
			}
			catch (...)
			{
				if (++uiDevNum == Devices.size ())
					uiDevNum = 0;
			}
			unlockDevices ();
			Sleep (20);
		}
		devLoopStarted = false;
		MessGeneral->Out ("SF цикл опроса приборов - выход");
		//while (devLoopStarted)
		//{
		//	time_t currenttime;
		//	time(&currenttime);
		//	lockDevices (); //Mess->Out  ("_MY_ В потоке", 0);
		//	//if(isNeedStop()){
		//	//	devLoopStarted = false;
		//	//	break;
		//	//}
		//	for(UINT devNum = 0; devNum < Devices.size(); devNum++){
		//		Devices[devNum]->Process(currenttime);
		//	}			
		//	unlockDevices();
		//	if (isNeedStop ())
		//	{
		//		devLoopStarted = false;
		//		break;
		//	}
		//	Sleep(20);//20
		//}
		////ExitThread(0);
		//_endthread ();
	}
	///___________________________________________________________________________
	// \brief Get float value from Storage by TagId
	// \param TagId specified in Subscription
	// \param bufToStore pointer to store data to
	// \return size of data
	int GetValue (int tagId, UCHAR * bufToStore)
	{
		if (tags.find (tagId) == tags.end ())
			return 0;
		string paramName = tags[tagId].ParamName;
		//string devName = tags[tagId].asDeviceName;
		string runNumber = tags[tagId].RunNumber;
		TransactionParam tp;
		CSerialDev * dev = tags[tagId].dev;
		if (!dev || !dev->getStorage ())
			return 0;

		if (tags[tagId].Command == 0)
		{// virtual param
			if (paramName.compare ("LinkStatus20") == 0)
			{
				*(float*)bufToStore = dev->GetTransactionStatus (20);
				return sizeof (float);
			}
			else if (paramName.compare ("LinkStatus500") == 0)
			{
				*(float*)bufToStore = dev->GetTransactionStatus (500);
				return sizeof (float);
			}
		}
		if (!dev->getStorage ()->GetValue (tags[tagId].Command, runNumber, paramName, tp))
		{
			return 0;
		}
		vector<UCHAR> data = tp.GetData ();
		int valsize = tp.GetSize ();

		// Convert pressure to MPa
		if (paramName.compare ("StaticPressure") == 0)
		{
			memcpy_s (bufToStore, 256, data.data (), valsize);
			//memcpy (bufToStore, data.data (), valsize);
			float val = *(float*)bufToStore;
			val *= 0.001;
			*(float*)bufToStore = val;
		}
		else if (tp.GetType () == SFPARAM_TYPE_FLOATBCD)
		{
			memcpy (bufToStore, data.data () + 4, 4);
			unsigned long val = *(long*)bufToStore;
			float flpart;
			memcpy ((UCHAR*)&flpart, data.data (), 4);
			//float correction = 1.0;
			//unsigned char km = val >> 28;
			//if (km <= 5)
			//	correction = pow(10.0, (3 - km));
			unsigned long lValue = 0;//val & 0x0f;
			for (int i = 28; i >= 0; i -= 4)
			{
				lValue *= 10;
				lValue += (val >> i) & 0x0f;
			}

			val = lValue;
			*(float*)bufToStore = (float)val + flpart;
			valsize = 4; // sifeof(float)
		}
		else if ((tp.GetType () == SFPARAM_TYPE_BCD) || (tp.GetType () == SFPARAM_TYPE_BCDSCALED))
		{
			memcpy (bufToStore, data.data (), valsize);
			unsigned long val = *(long*)bufToStore;
			unsigned long lValue = 0;//val & 0x0f;
			for (int i = 28; i >= 0; i -= 4)
			{
				lValue *= 10;
				lValue += (val >> i) & 0x0f;
			}
			val = lValue;
			*(float*)bufToStore = (float)val;
			valsize = 4;
		}
		else if (paramName.compare ("DevDate") == 0)
		{
			UCHAR dt[3];
			memcpy (dt, data.data (), sizeof (dt));
			sprintf_s ((char*)bufToStore, 30, "%02d/%02d/%04d",
								 dt[1], dt[0], dt[2] + 2000);
			valsize = strlen ((const char *)bufToStore);
		}
		else if (paramName.compare ("DevTime") == 0)
		{
			UCHAR dt[3];
			memcpy (dt, data.data (), sizeof (dt));
			sprintf_s ((char*)bufToStore, 30, "%02d:%02d:%02d",
								 dt[0], dt[1], dt[2]);
			valsize = strlen ((const char *)bufToStore);
		}
		else
			memcpy (bufToStore, data.data (), valsize);


		return valsize;
	}

	bool Write4Values (const char * address, float values[4])
	{
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;
		Json::Value DeviceConfig;
		Json::Value DevTypeList;
		Json::Value paramsOut;
		string asDeviceName;
		string DevicePort;
		string RunNumber = "";
		string MapID = "";

		//_CrtDumpMemoryLeaks();

		if (reader.parse (address, root))
		{
			asDeviceName = root["DeviceName"].asString ();

			DeviceConfig = FindDeviceConfig (asDeviceName);

			if (DeviceConfig.empty ())
			{
				MessGeneral->Out ("[%s] SetValue: ERROR: Device was not found", asDeviceName.c_str ());
				return false;
			}

			DevicePort = DeviceConfig["Port"].asString ();
			if (!DeviceConfig.get ("Password", "").asString ().empty ())
			{
				for (UINT p = 0; p < root["ParamsOutList"].size (); p++)
				{
					if (root["ParamsOutList"][p]["Name"].asString ().compare ("SuperFlo_Write_Password") == 0)
					{
						root["ParamsOutList"][p]["Value"] = DeviceConfig["Password"];
					}
				}
			}
			lockDevices ();
			CSerialDev * serdev = FindSerialDevice (DevicePort, DeviceConfig);

			if (!serdev)
			{
				unlockDevices ();
				MessGeneral->Out ("[%s] SetValue ERROR, getting Serial port %s", asDeviceName.c_str (), DevicePort.c_str ());
				return false;
			}

			Json::Value DevTypeList = DeviceTypesConfig["DeviceTypesList"];
			Json::Value DevType;

			// get protocol config
			auto asType = DeviceConfig["Type"].asString ();
			for (UINT devTypeNum = 0; devTypeNum < DevTypeList.size (); devTypeNum++)
			{
				if (DevTypeList[devTypeNum]["Type"].asString ().compare (asType) == 0)
				{
					DevType = DevTypeList[devTypeNum];
					break;
				}
			}

			//looking for RunNumber param, if it differ for all Transactions.RunNumber we should add new Transaction
			paramsOut = root["ParamsOutList"];
			for (UINT po = 0; po < paramsOut.size (); po++)
			{
				string pname = paramsOut[po]["Name"].asString ();
				if (pname.compare ("RunNumber") == 0)
				{
					RunNumber = paramsOut[po]["Value"].asString ();
					break;
				}
			}

			// check existing transactions have subscribing parameter
			const TransactionList * pTransactions = serdev->GetTransactionsList ();

			{// transaction with given parameter was not found, so create new one
				string RequestName = root["RequestName"].asString ();
				string asParamName = root["ParamName"].asString ();
				int RequestNum;
				if (RequestName.empty ())
					RequestNum = FindRequestForParameter (DevType, asParamName); ///< in case of "RequestName" absence look in all requests for param
				else
					RequestNum = FindRequestForReqName (DevType, RequestName);

				if (RequestNum < 0)
				{
					unlockDevices ();
					return false;
				}

				bool doneflag = false;
				for (int valindx = 0; valindx < sizeof (values) || doneflag; valindx++)
				{
					Json::Value param;
					string pn = "P" + std::to_string (valindx + 1);

					if (root[pn].isNull ())
						break;

					string pname = root[pn].asString ();

					Json::Value paramsTmp = DevType["Requests"][RequestNum]["ParamsOutList"];

					for (UINT po = 0; po < paramsTmp.size (); po++)
					{
						if (paramsTmp[po]["Name"].asString ().compare (pname) == 0)
						{
							param["Name"] = pname;
							Json::Value fv (values[valindx]);
							param["Value"] = fv;
							paramsOut[paramsOut.size ()] = param;
							break;
						}
					}
				}

				if (serdev->GetDevProtocol () == "SF")
				{
					SfTransaction * psf;
					//psf = new SfTransaction(asDeviceName, DeviceConfig["Address"].asUInt(), DevType["Requests"][RequestNum], paramsOut);
					psf = new SfTransaction (serdev, DeviceConfig["Address"].asUInt (), DevType["Requests"][RequestNum], paramsOut, asParamName);
					if (RunNumber != "")
						psf->SetRunNumber (RunNumber);
					psf->SetStartArchTime (&DeviceConfig);

					// Added special packet processor (for example read from template) for actual device type
					psf->SetOneShot (TRUE);
					psf->SetTryCount (serdev->GetTryCounts ()); //psf->SetTryCount(1);ADD
					serdev->AddRequest (psf);

					if (!DevType["Requests"][RequestNum].get ("AfterResponse", "").asString ().empty ())
					{
						string asAddress = DevProc::MakeAddress (asDeviceName, DevType["Requests"][RequestNum]["AfterResponse"].asString (), "", RunNumber);

						unlockDevices ();
						// Recursion 
						WriteValue (asAddress.c_str (), NULL, 0);

						lockDevices ();
					}
				}
				else
				{
					serdev->Mess->Out ("SetValue: ERROR: do not know how to create transactions for protocol %s", serdev->GetDevProtocol ().c_str ());
				}
			}
			unlockDevices ();
		}
		else
		{
			MessGeneral->Out ("SetValue: ERROR: can not parse address string %s", address);
			return false;
		}
		return true;
	}
	///__________________________________________________________________________
	// \brief Write value to parameter
	// \param TagId ???
	// \param address json request to process
	// \param size bytes count to write
	// \param value new value as string - will be converted in necessary type according to address parameter type 
	bool WriteValue (const char * address, char * value, int size)
	{
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;
		Json::Value DeviceConfig;
		Json::Value DevTypeList;
		Json::Value paramsOut;
		string asDeviceName;
		string DevicePort;
		string RunNumber = "";
		string MapID = "";
		//string asParamName = "TEXT";

		if (reader.parse (address, root))
		{
			asDeviceName = root["DeviceName"].asString ();
			DeviceConfig = FindDeviceConfig (asDeviceName);
			//asParamName = root["ParamsOutList"]["Name"].asString ();

			if (DeviceConfig.empty ())
			{
				MessGeneral->Out ("WriteValue: ERROR: Device [%s] was not found", asDeviceName.c_str ());
				return false;
			}

			DevicePort = DeviceConfig["Port"].asString ();
			if (!DeviceConfig.get ("Password", "").asString ().empty ())	// Запись DT? 
			{
				for (UINT p = 0; p < root["ParamsOutList"].size (); p++)
				{
					if (root["ParamsOutList"][p]["Name"].asString ().compare ("SuperFlo_Write_Password") == 0)
					{
						root["ParamsOutList"][p]["Value"] = DeviceConfig["Password"];
					}
				}
			}
			lockDevices ();
			CSerialDev* serdev = FindSerialDevice (DevicePort, DeviceConfig);

			if (!serdev)
			{
				unlockDevices ();
				MessGeneral->Out ("WriteValue: ERROR getting Serial port %s", DevicePort.c_str ());
				return false;
			}

			Json::Value DevTypeList = DeviceTypesConfig["DeviceTypesList"];
			Json::Value DevType;

			// get protocol config
			for (UINT devTypeNum = 0; devTypeNum < DevTypeList.size (); devTypeNum++)
			{
				if (DevTypeList[devTypeNum]["Type"].asString ().compare (DeviceConfig["Type"].asString ()) == 0)
				{
					DevType = DevTypeList[devTypeNum];
					break;
				}
			}

			//looking for RunNumber param, if it differ for all Transactions.RunNumber we should add new Transaction
			paramsOut = root["ParamsOutList"];
			for (UINT po = 0; po < paramsOut.size (); po++)
			{
				string pname = paramsOut[po]["Name"].asString ();
				if (pname.compare ("RunNumber") == 0)
				{
					string value = paramsOut[po]["Value"].asString ();
					RunNumber = value;
					break;
				}
			}

			// check existing transactions have subscribing parameter
			const TransactionList * pTransactions = serdev->GetTransactionsList ();

			{// transaction with given parameter was not found, so create new one
				string RequestName = root["RequestName"].asString ();
				int RequestNum;
				if (RequestName.empty ())
					RequestNum = FindRequestForParameter (DevType, root["ParamName"].asString ()); ///< in case of "RequestName" absence look in all requests for param
				else
					RequestNum = FindRequestForReqName (DevType, RequestName);

				if (RequestNum < 0)
				{
					unlockDevices ();
					return false;
				}

				if (value)
				{
					Json::Value param;
					int valindx = 0;
					string pn = "P" + std::to_string (valindx + 1);
					string pname;
					if (root[pn].isNull ())
					{ //using ParamName
						string pname = root["ParamName"].asString ();
						Json::Value paramsTmp = DevType["Requests"][RequestNum]["ParamsOutList"];
						for (UINT po = 0; po < paramsTmp.size (); po++)
						{
							if (paramsTmp[po]["Name"].asString ().compare (pname) == 0)
							{
								param["Name"] = pname;
								param["Value"].resize (size);
								for (int valind = 0; valind < size; valind++)
								{
									param["Value"][valind] = value[valind];
								}

								paramsOut[paramsOut.size ()] = param;
								break;
							}
						}
					}
					else
					{ // using P1, P2, ... as names and all of then float types
						for (int valindx = 0; valindx < size / sizeof (float); valindx++)
						{
							Json::Value param;
							pn = "P" + std::to_string (valindx + 1);

							string pname = root[pn].asString ();

							Json::Value paramsTmp = DevType["Requests"][RequestNum]["ParamsOutList"];

							for (UINT po = 0; po < paramsTmp.size (); po++)
							{
								if (paramsTmp[po]["Name"].asString ().compare (pname) == 0)
								{
									param["Name"] = pname;
									float fval = ((float*)value)[valindx];
									Json::Value fv (fval);
									param["Value"] = fv;

									paramsOut[paramsOut.size ()] = param;
									break;
								}
							}
						}
					}
				}

				if (serdev->GetDevProtocol () == "SF")
				{
					SfTransaction * psf;
					if (!DevType["Requests"][RequestNum].get ("AfterResponse", "").asString ().empty ())//ADD
					{
						string address = DevProc::MakeAddress (asDeviceName, DevType["Requests"][RequestNum]["AfterResponse"].asString (), "", RunNumber);
						unlockDevices ();
						// Recursion 
						WriteValue (address.c_str (), NULL, 0);
						lockDevices ();
					}
					psf = new SfTransaction (serdev, DeviceConfig["Address"].asUInt (), DevType["Requests"][RequestNum], paramsOut, root["RequestName"].asString ());
					if (RunNumber.length () > 0)
						psf->SetRunNumber (RunNumber);
					psf->SetStartArchTime (&DeviceConfig);
					psf->SetOneShot (TRUE);
					psf->SetTryCount (serdev->GetTryCounts ());//psf->SetTryCount(1);ADD
					serdev->AddRequest (psf);

					if (!DevType["Requests"][RequestNum].get ("AfterResponse", "").asString ().empty ())
					{
						string address = DevProc::MakeAddress (asDeviceName, DevType["Requests"][RequestNum]["AfterResponse"].asString (), "", RunNumber);
						unlockDevices ();
						// Recursion 
						WriteValue (address.c_str (), NULL, 0);
						lockDevices ();
					}
				}
				//else ADD
				//{
				//	Mess->Out ("SetValue: ERROR: do not know how to create transactions for protocol  %s", serdev->GetDevProtocol());
				//}
			}
			unlockDevices ();
		}
		else
		{
			MessGeneral->Out ("SetValue ERROR: can not parse address string");
			return false;
		}
		return true;
	}
	///__________________________________________________________________________

	string GetArchiveData (string address)
	{
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;
		string asDeviceName;
		//string DevicePort;
		string RunNumber = "";
		string MapID = "";
		UINT Cmd = 0;
		CSerialDev *dev = 0;

		if (reader.parse (address, root))
		{
			asDeviceName = root["DeviceName"].asString ();

			const Json::Value DeviceConfig = FindDeviceConfig (asDeviceName);

			if (DeviceConfig.empty ())
			{
				MessGeneral->Out ("[%s] GetArchiveData: ERROR. Device was not found", asDeviceName.c_str ());
				return string ();
			}

			UINT devTypeNum = 0;
			// get protocol config
			for (devTypeNum = 0; devTypeNum < DeviceTypesConfig["DeviceTypesList"].size (); devTypeNum++)
			{
				if (DeviceTypesConfig["DeviceTypesList"][devTypeNum]["Type"].asString ().compare (DeviceConfig["Type"].asString ()) == 0)
				{
					break;
				}
			}

			for (UINT po = 0; po < root["ParamsOutList"].size (); po++)
			{
				string pname = root["ParamsOutList"][po]["Name"].asString ();
				if (pname.compare ("RunNumber") == 0)
				{
					string value = root["ParamsOutList"][po]["Value"].asString ();
					RunNumber = value;
					break;
				}
			}

			string RequestName = root["RequestName"].asString ();
			int RequestNum;

			RequestNum = FindRequestForReqName (DeviceTypesConfig["DeviceTypesList"][devTypeNum], RequestName);
			if (RequestNum < 0)
			{
				return string ();
			}
			UINT Cmd = DeviceTypesConfig["DeviceTypesList"][devTypeNum]["Requests"][RequestNum]["Command"].asUInt ();
			dev = FindSerialDeviceByName (asDeviceName);
			if (!dev)
			{
				//MessGeneral->Out ("[%s] Request ArchiveData ERROR: No such device", asDeviceName.c_str ());
				return string ();
			}

			return dev->getStorage ()->GetArchiveData (Cmd, RunNumber, dev);
		}
		else
		{
			MessGeneral->Out ("Request ArchiveData ERROR: can't parse address string [%s]", address.c_str ());
			return string ();
		}

		return string ();
	}
	///__________________________________________________________________________
	// \brief This function makes address string from input arguments
	// \param asDeviceName name of the device, must not be empty
	// \param ReqName request name , must not be empty
	// \param paramName parameter name, can be empty
	// \param RunNumber runnumber can be empty 
	// \return address string
	string MakeAddress (string DeviceName, string ReqName, string paramName, string RunNumber)
	{
		stringstream tag;

		// make tag
		tag << "{\"DeviceName\":\""
			<< DeviceName
			<< "\",\"RequestName\":\""
			<< ReqName
			<< "\"";

		if (!paramName.empty ())
		{
			tag << ",\"ParamName\":\""
				<< paramName
				<< "\"";
		}
		tag << ",\"ParamsOutList\":["
			<< "{\"Name\":\"RunNumber\",\"Value\":\""
			<< RunNumber
			<< "\"}]}";

		return tag.str ();
	}
	//_____________________________________________________________________________

	int StopStartDev (char * cpDevConfig, bool bStop)
	{
		return StopStartDevs->Init (cpDevConfig, bStop);
	}
}
//_____________________________________________________________________________
