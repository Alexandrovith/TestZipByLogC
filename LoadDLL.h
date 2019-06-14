#pragma once
#include <Windows.h>

class CLoadDLL
{
	HINSTANCE hDLL;

public:
	typedef void (CALLBACK* DInitConfig)(char*);
	typedef int (CALLBACK* DSubscribe)(int, char *);
	typedef int (CALLBACK* DWriteValue)(char * address, char* value, int size);
	typedef void (CALLBACK* DClose)();
	typedef void (CALLBACK* DStopStartDev)(char*, bool);
	typedef void (CALLBACK* DZipByCppLog)();

	CLoadDLL (LPCWSTR lpLibFileName);
	~CLoadDLL ();
	void* Load (LPCSTR FuncName);
	void Close ();
};

