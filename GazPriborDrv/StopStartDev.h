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

		// Приборы для останова 
		TIterStop DevsStop;
		// Приборы для запуска 
		TDevNames DevsPusk;
		// Остановленные приборы 
		TDevNames DevsStopped;

	public:
		CStopStartDev ();
		~CStopStartDev ();

		// Фиксация прибора для останова / запуска 
		int Init (char * cpDevConfig, bool bStop);
		// Останов / запуск обмена с прибрами 
		void Run ();
	};
}
