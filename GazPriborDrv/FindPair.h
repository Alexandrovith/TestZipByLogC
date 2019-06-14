//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//~~~~~~~~~	Проект:			Базовая библиотека опроса приборов (driver)      
//~~~~~~~~~	Прибор:			Все                                     
//~~~~~~~~~	Модуль:			Вычитывание настроек для параметров из строки тегов	(as json)
//~~~~~~~~~	Разработка:	Демешкевич С.А.                                    		  
//~~~~~~~~~	Дата:				27.03.2019                                         		  

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once
#ifdef GAZPRIBORDRV_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif


#include <Windows.h>

using namespace std;

//=============================================================================

class MYDLL_API CFindPair
{
public:
	CFindPair (string Tag);
	CFindPair () {}
	~CFindPair ();

	string operator	[](string asKey)
	{
		return Value (asKey);
	}
	int CurrPos () { return iCurrPos; }
	string Tag () { return asTag; }

	virtual string Value (string sStr, int iStartFind);

private:
	string asTag;
	int iCurrPos;

	int Find (string sKey, int iPosNameParam);
	int FindWord (string sKey, int iPosNameParam);
	int FindWord (string sKey);
	string ValueNext (string sKey);
	string Value (string sKey);

	bool IsLetterOrDigit (char cSym)
	{
		return (isascii (cSym) == 0)/* || (isalnum (cSym) == 0)*/;
	}


	//_________________________________________________________________________

};

