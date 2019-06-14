///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	������:			Driver by cpp
///~~~~~~~~~	������:			��� �������
///~~~~~~~~~	������:			����������� ���������� C#
///~~~~~~~~~	����������:	���������� �.�.
///~~~~~~~~~	����:				13.05.2019

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
