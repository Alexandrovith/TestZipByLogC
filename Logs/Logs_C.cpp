//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//~~~~~~~~~	������:			Drivers C++
//~~~~~~~~~	������:			��� ������� �� C++
//~~~~~~~~~	������:			����� ��������������� ��������� (� ���)
//~~~~~~~~~	����������:	���������� �.�.
//~~~~~~~~~	����:				26.07.2018

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "stdafx.h"
#include "pch.h"

#include "Logs_C.h"
#include "ErrSock.h"

using namespace std::placeholders;    // adds visibility of _1, _2, _3,...

namespace Messages
{
	const std::string DIR_LOG = "\\logs";
	const std::string EXT_LOG = ".css";

	CMessages::CMessages (const std::string asPach_Assembly, const std::string asDevName) :
		asDirLog (asPach_Assembly + DIR_LOG),
		asDirAssembly (asPach_Assembly)
	{
		SetFileNameLog (asDevName);

		if (asFileLog.length () == 0)
			asFileLog = asPach_Assembly + DIR_LOG + asDevName + EXT_LOG;

		if (asPach_Assembly.length () < 1)
			throw CErrMess ((const char*)"������ ����� �������� ������");

		if (DirExists (asDirLog) == false)
		{
			_mkdir (asDirLog.c_str ());
		}
		RdSetups ();
	}
	//_____________________________________________________________________________

	std::string CMessages::ExtractDir (std::string asPachFile, std::string& asDirRet)
	{
		const static std::string asEndPach = "\\";
		std::string::size_type llPos = asPachFile.find_last_of (asEndPach);
		asDirRet = asPachFile.substr (0, llPos);
		return asDirRet;
	}
	//_____________________________________________________________________________

	bool CMessages::DirExists (const std::string& dirName_in)
	{
		DWORD attribs = GetFileAttributesA (dirName_in.c_str ());
		if (attribs == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!

		return (attribs & FILE_ATTRIBUTE_DIRECTORY);
	}
	//_____________________________________________________________________________

	CMessages::~CMessages ()
	{}
	//_____________________________________________________________________________

	void CMessages::SetFileNameLog (std::string FileNameLog)
	{
		asFileLog = asDirLog + "\\" + asFileLog.substr (0, asFileLog.find_last_of ("\\", 512, 2) + 1) + /*DIR_LOG + */FileNameLog + EXT_LOG;
	}
	//_____________________________________________________________________________

	//char* CMessages::Format (const char* fmt, ...)
	//{
	//	static const int SIZE = 6000;
	//	static char caRet[SIZE];

	//	va_list ap;
	//	va_start (ap, fmt);
	//	int n = vsnprintf_s ((char*)caRet, SIZE, SIZE, fmt, ap);
	//	va_end (ap);
	//	if (n > -1 && n < SIZE)
	//	{
	//		return (char*)caRet;
	//	}
	//	return (char*)"NONE";
	//}
	//_____________________________________________________________________________

	std::string CMessages::CharArrToStrAsInt (char *caBuf, int iSize, int Radix)
	{
		asRet = "";
		for (size_t i = 0; i < iSize; i++)
		{
			char Buf[32];
			asRet += _itoa_s ((unsigned char)(*caBuf), Buf, Radix);
			asRet += " ";
			caBuf++;
		}
		return asRet;
	}
	//_____________________________________________________________________________
	void CMessages::SetParam (int i, char *caLine, std::function<void (char*)> Setup)
	{
		//const int QUANT_PAR = 3;
		//const char const caParams[QUANT_PAR][30] = { "[bDebugOut]", "[iSymbolsQuantDebug]", "[iSymbolsQuantRelease]" };
		////void ((*Setup[QUANT_PAR]))(int) = { [=] (int val) {bDebugOut = val; } };
		//std::function<void (char*)> Setup[] = { [=] (char* caLine) {if (strcmp (caLine, "true") == 0) bDebugOut = "true"; else bDebugOut = false; },
		//	[=](char* caLine) { iSymbolsQuantDebug = atoi (caLine); },
		//	[=](char* caLine) { iSymbolsQuantRelease = atoi (caLine);	} };

		//if (strcmp (caLine, caParams[i]) == 0)
		//{
		//	IF >> caLine;
		//	Setup[0] (caLine);
		//	break;
		//}
	}
	//_____________________________________________________________________________

	void CMessages::RdSetups ()
	{
		//std::string asPath = asDirAssembly + "\\" + cpFileSetups;
		//std::ifstream IF (asPath.c_str ()/*, std::ios::app*/);

		//for (int i = 0; i < QUANT_PAR; ++i)
		//{
		//	char caLine[64];
		//	do
		//	{
		//		IF.getline (caLine, sizeof (caLine));
		//		if (strcmp (caLine, caParams[0]) == 0)
		//		{
		//			IF >> caLine;
		//			Setup[0] (caLine);
		//			break;
		//		}

		//	} while (strlen (caLine) > 0);
		//}
	}
	//_____________________________________________________________________________
	// ���� iSymbolsQuant == 0, �� ����� �� ����� 0 
	UCHAR* CMessages::CharArrToCharAsInt (void *vBuf, int iSymbolsQuant, int Radix)
	{
			cpBufOut[0] = 0;
		if (iSymbolsQuant <= 0)
		{
			//cpBufOut[0] = 0;
			return (UCHAR*)cpBufOut;
		}
		if (bDebugOut)
			iSymbolsQuant = iSymbolsQuantDebug;
		else iSymbolsQuant = iSymbolsQuantRelease;

		//std::unique_ptr<CTrunkBuf> BufOut (new CTrunkBuf (cpBufOut, iSymbolsQuant, Radix));

		char* caBuf = (char*)vBuf;
		if (iSymbolsQuant)
		{
			for (size_t i = 0; i < iSymbolsQuant && *caBuf; i++)
			{
				char Buf[16];
				_itoa_s ((unsigned char)(*caBuf), Buf, Radix);
				strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, Buf, 2);
				strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ", 1);
				caBuf++;
			}
			strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, "\0", 1);
		}
		else
		{
			int iDiap = 0;
			while (*caBuf)
			{
				char Buf[16];
				_itoa_s ((unsigned char)(*caBuf), Buf, Radix);
				strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, Buf, 2);
				strncat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ", 1);
				caBuf++;
				if (++iDiap == CTrunkBuf::SIZE_BUF)
					break;
			}
		}
		return (UCHAR*)cpBufOut;
	}
	//_____________________________________________________________________________

	char* CMessages::CharArrToCharAsInt (std::string asBuf, int iSize, int Radix)
	{
		cpBufOut[0] = 0;
		for (size_t i = 0; i < iSize; i++)
		{
			char Buf[16];
			_itoa_s ((unsigned char)(asBuf[i]), Buf, Radix);
			strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, Buf);
			strcat_s (cpBufOut, CTrunkBuf::SIZE_BUF, " ");
		}
		return cpBufOut;
	}
	//_____________________________________________________________________________

	const char* GetTimeNow ()
	{
		SYSTEMTIME st;
		GetLocalTime (&st);
		static char caBuf[48];
		sprintf_s (caBuf, "%02d%02d%02d %02d:%02d:%02d.%03d ", st.wDay, st.wMonth,
							 st.wYear - 2000, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		return caBuf;
	}


	//=============================================================================

	CMess::CMess (const std::string asPach_Assembly, const std::string asDevName) :
		CMessages (asPach_Assembly, asDevName)
	{
		if (asFileLog.length () == 0)
			asFileLog = asDirLog + asDevName + EXT_LOG;
	}
	//_____________________________________________________________________________
	// ����� ������ � log-����
	void CMess::Out (const char* cpMess, ...)
	{
		static const int SIZE = 600;
		char caOut[SIZE + 1];

		try
		{
			va_list ap;
			va_start (ap, cpMess);
			int n = vsnprintf_s ((char*)caOut, SIZE, _TRUNCATE, cpMess, ap);
			va_end (ap);
			if (n > -1/* && n < SIZE*/)
			{
		/*		AppendMess (caOut);
				if (asMessToFl.length() >= 1024)
				{
					FileWr (asFileLog);
				}*/
				std::ofstream F (asFileLog, std::ios::app);
				F << GetTimeNow () << caOut << std::endl;
				F.close ();
				Zip.Pack ();
			}
		}
		catch (const std::exception& exc)
		{
		}
	}
	//_________________________________________________________________________

	//void CMess::AppendMess (const char* caOut, bool bDataTimeShow)
	//{
		//asMessToFl.append (AddDTtoMessage (bDataTimeShow));	//bDataTimeShow asMessToFl.append (caOut);
		//asMessToFl.append (caOut);
		//asMessToFl.append ("\r\n");
	//}
	//_________________________________________________________________________
	
	//const char* CMess::AddDTtoMessage (bool bDataTimeShow)
	//{
	//	if (bDataTimeShow)
	//	{
	//		return GetTimeNow ();
	//	}
	//	return "";
	//}
	//_____________________________________________________________________________

	//void CMess::FileWr (std::string asFileNmae)
	//{
		//std::ofstream F (asFileNmae, std::ios::app);
		//F << asMessToFl.c_str() << std::endl;
		//F.close ();
		//asMessToFl.clear ();
	//}
	//_____________________________________________________________________________

	char* Format (const char* fmt, ...)
	{
		static const int SIZE = 600;
		static char caRet[SIZE + 1];

		va_list ap;
		va_start (ap, fmt);
		int n = vsnprintf_s ((char*)caRet, SIZE, _TRUNCATE, fmt, ap);
		va_end (ap);
		if (n > -1/* && n < SIZE*/)
		{
			return (char*)caRet;
		}
		return (char*)"";
	}

	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	CLenOut::CLenOut (char* cpBuf, int iLen)
	{
		this->cpBuf = &cpBuf[iLen];
		cSymSave = cpBuf[iLen];
		cpBuf[iLen] = 0;
	}

	CLenOut::~CLenOut ()
	{
		*cpBuf = cSymSave;
	}
}