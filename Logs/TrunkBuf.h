///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	������:			Driver EC605
///~~~~~~~~~	������:			��� �������
///~~~~~~~~~	������:			��������� �������� ��������� � ���
///~~~~~~~~~	����������:	���������� �.�.
///~~~~~~~~~	����:				26.02.2019

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once

// ��������� ����� ��������� ������� 
class CTrunkBuf
{
public:
#ifdef _DEBUG
	static const int SIZE_BUF = 1024;
#else
	static const int SIZE_BUF = 72;
#endif // _DEBUG
	
	CTrunkBuf (char *cpBufOut, int &iSize, int Radix);
	~CTrunkBuf ();
	void Init (char *cpBufOut, int &iSize, int Radix);

private:
	char* cpBufOut;
	bool bSizeIsTrunk = false;
};

