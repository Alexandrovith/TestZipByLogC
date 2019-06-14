// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
string binDir;



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if (binDir.empty()) {
		char moduleFileName[256];
		GetModuleFileNameA(hModule, moduleFileName, sizeof(moduleFileName));
		string strfile(moduleFileName);
		string::size_type pos = strfile.find_last_of( "\\/" );
		binDir = strfile.substr(0, pos);
		printf("Using driver %s\n", moduleFileName);
	}
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

