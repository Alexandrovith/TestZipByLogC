///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	Проект:			Driver by cpp
///~~~~~~~~~	Прибор:			Все приборы
///~~~~~~~~~	Модуль:			Обрезание длинного сообщения в лог
///~~~~~~~~~	Разработка:	Демешкевич С.А.
///~~~~~~~~~	Дата:				26.02.2019

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "pch.h"
#include "TrunkBuf.h"

//Обрезание длинного сообщения в лог
CTrunkBuf::CTrunkBuf (char *BufOut, int &iSize, int Radix)
{
	Init (BufOut, iSize, Radix);
}
///____________________________________________________________________________

void CTrunkBuf::Init (char *BufOut, int &iSize, int Radix)
{
	cpBufOut = BufOut;
	memset (cpBufOut, 0, SIZE_BUF);
	int iMaxQuantSym = Radix == 10 ? 4 : 3;
	if (iSize >= SIZE_BUF / iMaxQuantSym)
	{
		iSize = SIZE_BUF / iMaxQuantSym - 4;
		bSizeIsTrunk = true;
	}
}
///____________________________________________________________________________

CTrunkBuf::~CTrunkBuf ()
{
	if (bSizeIsTrunk)
	{
		strcat_s (cpBufOut, SIZE_BUF, "...");
	}
}
