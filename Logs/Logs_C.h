///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	������:			Driver EC605
///~~~~~~~~~	������:			��� �������
///~~~~~~~~~	������:			����� ��������������� ��������� (� ��� ��� ��. ����������)
///~~~~~~~~~	����������:	���������� �.�.
///~~~~~~~~~	����:				26.07.2018

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#pragma once

#ifdef LOGS_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif
//#define MYDLL_API __declspec(dllexport)
#define EXTERN_C extern "C"

using namespace System;
using namespace AXONIM::ZipByCppLog;

#include "stdafx.h"
#include "GetZipByCppLog.h"
#include "TrunkBuf.h"

namespace LogsC 
{
	public ref class CLog_C
	{
		// TODO: Add your methods for this class here.
	};
}

namespace Messages
{
	EXTERN_C MYDLL_API const LPSTR GetLastErrorAsLPSTR ();
	EXTERN_C MYDLL_API const char* WinSockErrCodeAsStr ();
	EXTERN_C MYDLL_API char* Format (const char* fmt, ...);

	const char* GetTimeNow ();

	class MYDLL_API CMessages
	{
	protected:
		std::string asFileLog;
		std::string asDirAssembly;
		std::string asDirLog;
		
		bool bDebugOut = false;
		int iSymbolsQuantDebug = 80, iSymbolsQuantRelease = 10;

		char cpBufOut[CTrunkBuf::SIZE_BUF];
		std::string asRet;

		CGetZipByCppLog Zip;

	public:
		const char* cpFileSetups = "BelTransGasDRV.config";

		CMessages () {}
		CMessages (const std::string asPach_Assembly, const std::string asDevName);
		virtual ~CMessages ();

		virtual void RdSetups ();

		// ����� ������ � log-����
		virtual void Out (const char* asMess, ...) = 0;
		//char* Format (const char* fmt, ...);
		virtual std::string CharArrToStrAsInt (char *caBuf, int iSize, int Radix = 16);
		virtual UCHAR* CharArrToCharAsInt (void *vBuf, int iSymbolsQuant, int Radix = 16);
		virtual char* CharArrToCharAsInt (std::string asBuf, int iSize, int Radix = 16);
		//void SetNameDev (std::string asDevName);
		static std::string ExtractDir (std::string asPachFile, std::string& asDirRet);

		std::string DirLog () { return asDirLog; }
		std::string DirAssembly ()
		{
			return asDirAssembly;
		}

	private:
		void SetParam (int i,char *caLine, std::function<void (char*)>);
		void SetFileNameLog (std::string FileNamelog);
		bool DirExists (const std::string& dirName_in);
	};


	class MYDLL_API CMess :
		public CMessages
	{
		//std::string asMessToFl;	

	public:

		CMess () : CMessages ("", "") {}
		CMess (const std::string asPach_Assembly, const std::string asDevName);
		virtual ~CMess () {}
		virtual void Out (const char* asMess, ...);

	private:

		//void AppendMess (const char* caOut, bool bDataTimeShow = true);
		//const char* AddDTtoMessage (bool bDataTimeShow = true); 
		//void FileWr (std::string asFileNmae);
	};

	// ����������� ����� ������ ������ 
	class MYDLL_API CLenOut
	{
	public:
		CLenOut (char* cpBuf, int iLen);
		~CLenOut ();

	private:
		char* cpBuf;
		char cSymSave;
	};

	//EXTERN_C MYDLL_API CMessages::asFileLog;
}