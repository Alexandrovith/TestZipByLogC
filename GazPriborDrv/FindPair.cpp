	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//~~~~~~~~~	������:			������� ���������� ������ �������� (driver) 
//~~~~~~~~~	������:			���            
//~~~~~~~~~	������:			����������� �������� ��� ���������� �� ������ �����	(as json)
//~~~~~~~~~	����������:	���������� �.�.
//~~~~~~~~~	����:				27.03.2019     

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

// �������� �� ���������� (��� ���) �����
// <param name="sStr">������ ��� ������ ��������</param>
// <param name="iStartFind">������� ������ ������ � ������</param>
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
		while (c != '"' && c != '_' && !IsLetterOrDigit (c)); // ����� ������� ��� �������� 
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

// �������� ��������� ��  �����
// <param name="sKey">����������, ��� ���� (������������ ���������)</param>
string CFindPair::Value (string sKey)
{
	string sRet = "";
	iCurrPos = FindWord (sKey);
	if (iCurrPos > -1)
	{
		iCurrPos += sKey.length ();
		sRet = Value (asTag, iCurrPos);     // ����� �������� �� ���������� (��� ���) �����
	}
	return sRet;
}
//_________________________________________________________________________

// �������� ��������� �� �����, ������� �� ������� ������� � ������
// <param name="sKey">����������, ��� ���� (������������ ���������)</param>
string CFindPair::ValueNext (string sKey)
{
	string sRet = "";
	iCurrPos = FindWord (sKey, iCurrPos);
	if (iCurrPos > -1)
	{
		iCurrPos += sKey.length ();
		sRet = Value (asTag, iCurrPos);     // �������� �� ���������� (��� ���) �����
	}
	return sRet;
}
//_________________________________________________________________________

// ����� ������ ����� � ������ � ������ ������
int CFindPair::FindWord (string sKey)
{
	iCurrPos = asTag.find (sKey);// .IndexOf (sKey);
	return Find (sKey, iCurrPos);
}
//_________________________________________________________________________

// ����� ������ ����� � ������ � �������� �������
int CFindPair::FindWord (string sKey, int iPosNameParam)
{
	iCurrPos = asTag.find (sKey, iPosNameParam + sKey.length ());
	return Find (sKey, iCurrPos);
}
//_________________________________________________________________________

// ����� ������� ������ ����� � ������
int CFindPair::Find (string sKey, int iPosNameParam)
{
	if (iPosNameParam > -1 && asTag.length () > iPosNameParam + sKey.length ())
	{
		// ����, ���� ��������� ��������, ������ sKey, �� ������ �������� ������ ������� �����
		while ((iPosNameParam == 0 && IsLetterOrDigit (asTag[iPosNameParam + sKey.length ()])) ||
			(iPosNameParam != 0) &&
					 (IsLetterOrDigit (asTag[iPosNameParam - 1]) || IsLetterOrDigit (asTag[iPosNameParam + sKey.length ()])))    // ���� ��� ����� �����
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
