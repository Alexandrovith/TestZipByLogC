///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Driver by cpp
///~~~~~~~~~	Прибор:			Все приборы
///~~~~~~~~~	Модуль:			Подключение библиотеки C#
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				13.05.2019

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once

//#include <msclr\auto_gcroot.h>

class CGetZipByCppLogPrivate;
//{
//public:
//	msclr::auto_gcroot<CZipByCppLog^> ZipByCppLog;
//};
//=============================================================================

class __declspec(dllexport) CGetZipByCppLog
{
private:
	CGetZipByCppLogPrivate* _private;

public:
	CGetZipByCppLog ();
	//{
	//	_private = new GetZipByCppLogPrivate ();
	//	_private->ZipByCppLog = gcnew CZipByCppLog ();
	//}
	//_____________________________________________________________________________

	~CGetZipByCppLog () { delete _private; }

	void Pack ();
	//{
	//	_private->ZipByCppLog->Pack ();
	//}
};
