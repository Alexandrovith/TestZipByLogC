#include "pch.h"
#include "LoadDLL.h"


CLoadDLL::CLoadDLL (LPCWSTR lpLibFileName)
{
	hDLL = LoadLibrary (lpLibFileName);
}
//_____________________________________________________________________________

CLoadDLL::~CLoadDLL ()
{
	Close ();
}
//_____________________________________________________________________________

void* CLoadDLL::Load (LPCSTR FuncName)
{              // Handle to DLL  
	void* lpfnDllFunc = nullptr;    // Function pointer  
	if (hDLL != nullptr)
	{
		lpfnDllFunc = GetProcAddress (hDLL, FuncName);
		if (!lpfnDllFunc)
		{
			Close ();
			return nullptr;
		}
	}
	return lpfnDllFunc;
}
//_____________________________________________________________________________

void CLoadDLL::Close ()
{
	if (hDLL != nullptr)
	{
		FreeLibrary (hDLL);
		hDLL = nullptr;
	}
}
