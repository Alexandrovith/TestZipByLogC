///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

///~~~~~~~~~	������:			Driver Superflo
///~~~~~~~~~	������:			��� �������
///~~~~~~~~~	������:			������� � �������������� ������ � ��������
///~~~~~~~~~	����������:	���������� �.�.
///~~~~~~~~~	����:				28.03.2019

///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "stdafx.h"
#include "StopStartDev.h"
#include "FindPair.h"
#include "SerialDev.h"
#include "DevProcessor.h"

namespace DevProc
{
	CStopStartDev::CStopStartDev ()
	{}
	//_____________________________________________________________________________

	CStopStartDev::~CStopStartDev ()
	{}
	//_____________________________________________________________________________

	int GetPosSym (char* cpStr, char cSym, int iMaxLen)
	{
		for (int i = 0; i < iMaxLen; i++)
		{
			if (cpStr[i] == cSym || cpStr[i] == 0)
				return i;
		}
		return 0;
	}
	//_____________________________________________________________________________

	int CStopStartDev::Init (char * cpDevConfig, bool bStop)
	{
		string asDevName = string (cpDevConfig, GetPosSym (cpDevConfig, ',', 64));
		try
		{
			if (asDevName.length () == 0)
			{
				MessGeneral->Out ("[%s] ������� ������ � �������� �� ���������: �������� ������� ������������ [%s]", asDevName.c_str (), cpDevConfig);
				return 1;
			}
			bIsInitState = true;
			//shared_ptr<CFindPair> FindPair = make_shared<CFindPair> (string (cpDevConfig));
			//asDevName = FindPair->operator[] ("Name");

			TDevices::iterator Iter;

			if (bStop)
			{
				for (Iter = Devices.begin (); Iter != Devices.end (); Iter++)
				{
					if ((*Iter)->GetName () == asDevName)
					{
						for (auto dev : DevsStop)						// �������� ������� ����������� ����� ������� 
						{
							if ((*dev)->GetName () == asDevName)
							{
								bIsInitState = false;
								return 0;
							}
						}
						DevsStop.push_back (Iter);
						DevsStopped.push_back (*Iter);
						bIsInitState = false;
						return 0;
					}
				}
				MessGeneral->Out ("[%s] ������� ������ � �������� �� ���������: ������ �� ��� ���������������", asDevName.c_str ());
			}
			else
			{
				for (auto dev : DevsPusk)								// �������� ������� � ������ ����������� ����� ������� 
				{
					if (dev->GetName () == asDevName)			// ���� ������ ��� ���� � ������ �� ������ 
					{
						bIsInitState = false;
						return 0;
					}
				}
				for (Iter = DevsStopped.begin (); Iter != DevsStopped.end (); Iter++)
				{
					if ((*Iter)->GetName () == asDevName)
					{
						DevsPusk.push_back (*Iter);				// ��������� � ������ ��� ������� 
						DevsStopped.erase (Iter);					// � ������ ������������� �� ������ �� �������� 
						bIsInitState = false;
						return 0;
					}
				}
				bIsInitState = false;
				MessGeneral->Out ("[%s] ���������� ������ � �������� �� ���������: ������ �� ������ � ������ �������������", asDevName.c_str ());
				return 1;
			}
		}
		catch (const std::exception& exc)
		{
			if (asDevName.length () > 0)
				MessGeneral->Out ("[%s] %s", asDevName.c_str (), exc.what ());
		}
		bIsInitState = false;
		return 1;
	}
	//_____________________________________________________________________________

	void CStopStartDev::Run ()
	{
		if (bIsInitState)
			return;

		TDevNames::const_iterator IterPusk;
		for (IterPusk = DevsPusk.begin (); IterPusk != DevsPusk.end (); IterPusk++)
		{
			(*IterPusk)->Init ();
			//Devices.push_back (*IterPusk);
			MessGeneral->Out ("[%s] ����� � �������� ��������˨�", (*IterPusk)->GetName ().c_str ());
		}
		DevsPusk.clear ();

		TIterStop::const_iterator IterStop;
		for (IterStop = DevsStop.begin (); IterStop != DevsStop.end (); IterStop++)
		{
			(*(*IterStop))->Disconnect ();
			MessGeneral->Out ("[%s] ����� � �������� �������������", (*(*IterStop))->GetName ().c_str ());
			//Devices.erase (*IterStop);
		}
		DevsStop.clear ();
	}
	//_____________________________________________________________________________
}