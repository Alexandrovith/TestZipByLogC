#pragma once

#include <Windows.h>
#include "DevProcessor.h"

using namespace std;


namespace DevProc
{
	typedef std::vector<CSerialDev*> TDevNames;
	typedef vector<TDevices::iterator> TIterStop;

	class CStopStartDev
	{
		volatile bool bIsInitState = false;

		// ������� ��� �������� 
		TIterStop DevsStop;
		// ������� ��� ������� 
		TDevNames DevsPusk;
		// ������������� ������� 
		TDevNames DevsStopped;

	public:
		CStopStartDev ();
		~CStopStartDev ();

		// �������� ������� ��� �������� / ������� 
		int Init (char * cpDevConfig, bool bStop);
		// ������� / ������ ������ � �������� 
		void Run ();
	};
}
