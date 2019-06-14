// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GAZPRIBORDRV_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GAZPRIBORDRV_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#include <string>
#include <memory>
#include "Logs_C.h"

//using namespace Messages;

//#ifdef GAZPRIBORDRV_EXPORTS
//#define GAZPRIBORDRV_API __declspec(dllexport)
//#else
//#define GAZPRIBORDRV_API __declspec(dllimport)
//#endif
//#pragma comment (lib, "lib_json.lib");
//#pragma argsused

#define GAZPRIBORDRV_API __declspec(dllexport)

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define VERSION_REVISION "$Revision: 445 $"

EXTERN_C GAZPRIBORDRV_API void Init();

EXTERN_C GAZPRIBORDRV_API void InitConfig(char * jsonConfig);

EXTERN_C GAZPRIBORDRV_API int Subscribe(int tagId, char * address);

EXTERN_C GAZPRIBORDRV_API int WriteValue(char * address, char* value, int size);

GAZPRIBORDRV_API int Write4Values(char * address, float values[4]);

EXTERN_C GAZPRIBORDRV_API int GetValue(int tagId, UCHAR*);

EXTERN_C GAZPRIBORDRV_API int * GetNewArchiveData(char * address);

EXTERN_C GAZPRIBORDRV_API void Close();

EXTERN_C GAZPRIBORDRV_API char* GetName();

EXTERN_C GAZPRIBORDRV_API char* GetVer();

EXTERN_C GAZPRIBORDRV_API int StopStartDev (char * cpDevConfig, bool bStop);

extern std::string asDirAssembly;
extern std::shared_ptr<Messages::CMess> MessGeneral;
