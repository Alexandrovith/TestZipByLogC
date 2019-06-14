	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//~~~~~~~~~	Проект:			Базовая библиотека опроса приборов (driver) 
//~~~~~~~~~	Прибор:			Все            
//~~~~~~~~~	Модуль:			Вычитывание настроек для параметров из строки тегов	(as json)
//~~~~~~~~~	Разработка:	Демешкевич С.А.
//~~~~~~~~~	Дата:				27.03.2019     

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "stdafx.h"
#include "FindPair.h"


CFindPair::CFindPair (string Tag)
{
	asTag = Tag;
}


CFindPair::~CFindPair ()
{}

//_________________________________________________________________________"parname":12"parname2":"321"

// Значение по найденному (или нет) ключу
// <param name="sStr">Строка для поиска значения</param>
// <param name="iStartFind">Позиция начала поиска в строке</param>
string CFindPair::Value (string sStr, int iStartFind)
{
	string sRet = "";
	int iEnd = sStr.length ();
	for (; iStartFind < iEnd; iStartFind++)
	{
		char c = sStr[iStartFind];
		if (c != ':')
			continue;
		do
			c = sStr[++iStartFind];
		while (c != '"' && c != '_' && !IsLetterOrDigit (c)); // Нашли кавычки или значение 
		if (c == '"')
		{
			if (sStr[iStartFind + 1] == '"')
				return "";
			c = ' ';
		}
		else c = sStr[--iStartFind];
		//while (c != '"' && IsLetterOrDigit (c) == false)   //c != '"' || 
		while (c != /*'_' && */IsLetterOrDigit (c) == false && c != '"')
		{
			if (++iStartFind >= iEnd)
				return sRet;
			c = sStr[iStartFind];
		}
		while (c != '"' && c != ',')// && (IsLetterOrDigit (c) || c == ' ' || c == '-' || c == '+' || c == '.' || c == '_'))
		{
			sRet += c;
			if (++iStartFind >= iEnd)
				return sRet;
			c = sStr[iStartFind];
		}
		break;
	}
	return sRet;
}
//_________________________________________________________________________

// Значение параметра по  ключу
// <param name="sKey">Собственно, сам ключ (наименование параметра)</param>
string CFindPair::Value (string sKey)
{
	string sRet = "";
	iCurrPos = FindWord (sKey);
	if (iCurrPos > -1)
	{
		iCurrPos += sKey.length ();
		sRet = Value (asTag, iCurrPos);     // Поиск значения по найденному (или нет) ключу
	}
	return sRet;
}
//_________________________________________________________________________

// Значение параметра по ключу, начиная от текущей позиции в строке
// <param name="sKey">Собственно, сам ключ (наименование параметра)</param>
string CFindPair::ValueNext (string sKey)
{
	string sRet = "";
	iCurrPos = FindWord (sKey, iCurrPos);
	if (iCurrPos > -1)
	{
		iCurrPos += sKey.length ();
		sRet = Value (asTag, iCurrPos);     // Значение по найденному (или нет) ключу
	}
	return sRet;
}
//_________________________________________________________________________

// Поиск целого слова в строке с начала строки
int CFindPair::FindWord (string sKey)
{
	iCurrPos = asTag.find (sKey);// .IndexOf (sKey);
	return Find (sKey, iCurrPos);
}
//_________________________________________________________________________

// Поиск целого слова в строке с заданной позиции
int CFindPair::FindWord (string sKey, int iPosNameParam)
{
	iCurrPos = asTag.find (sKey, iPosNameParam + sKey.length ());
	return Find (sKey, iCurrPos);
}
//_________________________________________________________________________

// Поиск позиции целого слова в строке
int CFindPair::Find (string sKey, int iPosNameParam)
{
	if (iPosNameParam > -1 && asTag.length () > iPosNameParam + sKey.length ())
	{
		// Ищем, пока найденное значение, равное sKey, не станет являться частью другого слова
		while ((iPosNameParam == 0 && IsLetterOrDigit (asTag[iPosNameParam + sKey.length ()])) ||
			(iPosNameParam != 0) &&
					 (IsLetterOrDigit (asTag[iPosNameParam - 1]) || IsLetterOrDigit (asTag[iPosNameParam + sKey.length ()])))    // Если это часть слова
		{
			iPosNameParam = asTag.find (sKey, iPosNameParam + sKey.length ());
			if (iPosNameParam == -1)
				return iPosNameParam;
		}
		iCurrPos = iPosNameParam;
		return iPosNameParam;
	}
	return -1;
}
//_________________________________________________________________________
