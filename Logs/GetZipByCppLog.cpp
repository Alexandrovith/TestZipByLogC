///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Driver by cpp
///~~~~~~~~~	Прибор:			Все приборы
///~~~~~~~~~	Модуль:			Подключение библиотеки C#
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				13.05.2019

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


#include "pch.h"
#include <msclr\auto_gcroot.h>
#include "GetZipByCppLog.h"

using namespace AXONIM::ZipByCppLog;

class CGetZipByCppLogPrivate
{
public:
	msclr::auto_gcroot<CZipByCppLog^> ZipByCppLog;
};
//=============================================================================

CGetZipByCppLog::CGetZipByCppLog ()
{
	_private = new CGetZipByCppLogPrivate ();
	_private->ZipByCppLog = gcnew CZipByCppLog ();
}
///____________________________________________________________________________

void CGetZipByCppLog::Pack ()
{
	_private->ZipByCppLog->Pack ();
}
