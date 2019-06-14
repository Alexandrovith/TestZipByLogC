#include "stdafx.h"
#include "DevStorage.h"
#include <iostream>
#include <Process.h>
#include "Common.h"
#include <wchar.h>
#include "Logs_C.h"
#include "GazPriborDrv.h"
#include "SerialDev.h"
#include "ErrSock.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;
using namespace Messages;

#ifdef CHECKTIMES
#define CHECK_TIME { DWORD st1, st2; st1=::GetTickCount();
#define FINISHCHECK_TIME(name_c) st2 = ::GetTickCount(); if ((st2-st1)>1) {MessGeneral->Out ("Checktime %s = %d\n", name_c, st2-st1);} }
#else
#define CHECK_TIME
#define FINISHCHECK_TIME(name_c) 
#endif

CSerialDev::CSerialDev (void) :
	_bStart (false),
	_sendState (0),
	_hComm (NULL),
	_lConnectRepeatTimeout (30),
	_state (SD_INIT),
	_bWaitingOnRead (FALSE),
	_dwTransactionTimeout (3 * 4),
	_localStorage (0),
	_debug (0)
	, _trycounts (1)//ADD
{
	_activeTransactionIdx = 0;
	time (&_tRepeatTime);
	InitLock ();
}

CSerialDev::CSerialDev (string & name, string & port, int baudRate, int dataBits, int parity, int stopBit, int flowCtrl, string & devType, string & devProtocol) :
	_devName (name),
	_bStart (false),
	_sendState (0),
	_hComm (NULL),
	_lConnectRepeatTimeout (30),
	_state (SD_INIT),
	_bWaitingOnRead (FALSE),
	_dwTransactionTimeout (5 * 6),
	_port (port),
	_baudRate (baudRate),
	_dataBits (dataBits),
	_parity (parity),
	_stopBit (stopBit),
	_flowCtrl (flowCtrl),
	_deviceType (devType),
	_devProtocol (devProtocol),
	_localStorage (0),
	_debug (0)
	, _trycounts (1)//ADD
{
	// Вывод в Log 
	//string asDirAssembly = cpModuleFileName;
	//CMessages::ExtractDir (asDirAssembly, asDirAssembly);
	Mess = make_shared<CMess> (asDirAssembly, name);

	_activeTransactionIdx = 0;
	time (&_tRepeatTime);
	InitLock ();
	if (_localStorage == 0)
	{
		_localStorage = new DevStorage ();
	}
	memset (_transactionStatus1, 0, sizeof (_transactionStatus1));
	_tsindex1 = 0;
}
///____________________________________________________________________________

CSerialDev::~CSerialDev (void)
{
	Close ();
}
///____________________________________________________________________________

void CSerialDev::Close ()
{
	Disconnect ();

	TransactionList::iterator ItTr;
	for (ItTr = _transactions.begin (); ItTr != _transactions.end (); ItTr++)
	{
		Transaction* Tr = *ItTr;
		if (Tr)
			delete Tr;
	}
	////for (auto Tr : _transactions)
	////{
	////	delete Tr;
	////}
	//_transactions.clear ();
	_localStorage->RemovePackets ();
	delete _localStorage;
}
///____________________________________________________________________________

void CSerialDev::SetTransactionStatus (char statusCmd)
{
	_transactionStatus1[_tsindex1] = statusCmd;
	_tsindex1++;
	if (statusCmd < 0)
	{//ADD
//Transaction * Tr = _transactions[_activeTransactionIdx];
//Tr->SetBadResponse();            this method only for link quallity check
		_localStorage->RemovePackets ();
	}
	if (_tsindex1 > sizeof (_transactionStatus1))
		_tsindex1 = 0;
}
//_____________________________________________________________________________

float CSerialDev::GetTransactionStatus (int count)
{
	if (count > sizeof (_transactionStatus1))
		count = sizeof (_transactionStatus1);
	int indx = (_tsindex1 - count + sizeof (_transactionStatus1)) % sizeof (_transactionStatus1);
	int goodtr = 0;
	int alltr = count;
	for (int i = 0; i < count; i++)
	{

		if (_transactionStatus1[indx] > 0)
			goodtr++;
		if (_transactionStatus1[indx] == 0)
			alltr--;
	}
	return (alltr > 0) ? (float)goodtr * 100 / alltr : 0;
}
//_____________________________________________________________________________

DevStorage* CSerialDev::getStorage ()
{
	return _localStorage;
}
//_____________________________________________________________________________

void CSerialDev::SetConnectRepeatTimeout (long seconds)
{
	_lConnectRepeatTimeout = seconds;
}
//_____________________________________________________________________________

BOOL CSerialDev::AddRequest (Transaction * rq)
{
	if (NULL == rq)
		return FALSE;
	if (!rq->CreateRequest ())
		return FALSE;
	//LockPortRes();
	_transactions.push_back (rq);
	//UnlockPortRes();
	return TRUE;
}
//_____________________________________________________________________________

void CSerialDev::NextRequest (time_t tCurTime, bool isSuccess)
{
	//time_t tCurTime;
	//time(&tCurTime);
	//LockPortRes();
	Transaction* tr = _transactions[_activeTransactionIdx];

	if (isSuccess)
	{
		//trycount = 0;
		if (tr->isOneShot ())
		{
			Mess->Out ("[%s.%s] del_TrAct", tr->GetParamName ().c_str (), tr->GetCommName ());
			delete tr;
			_transactions.erase (_transactions.begin () + _activeTransactionIdx);
			return;
		}
		tr->ResetFireTime (tCurTime);
	}
	else
	{
		int trycount = tr->GetTryCount ();
		if (trycount == 0)
		{
			tr->SetBadResponse ();
			tr->ResetFireTime (tCurTime);
			ActionNoResponse (iCountNoRespose);
		}
		else
		{
			tr->OneMoreTry ();
		}
	}
	_activeTransactionIdx++;
	//UnlockPortRes();
}
///____________________________________________________________________________
// Переподключение прибора при достижении MAX_COUNT_NO_CONNECT неответов и незапросов 
bool CSerialDev::ActionNoResponse (int& iCountNoConnect)
{
	const int MAX_TIME_NO_CONNECT = 12;

	if (++iCountNoConnect == MAX_TIME_NO_CONNECT)
	{
		iCountNoConnect = 0;
		Disconnect ();
		Mess->Out ("Ответу нету (%s). Ожидание...", _port.c_str());
		Sleep (70000);
		Init ();
		return true;
	}
	return false;
}
///____________________________________________________________________________

Transaction* CSerialDev::GetTransaction ()
{
	//LockPortRes();
	if (_activeTransactionIdx != _transactions.size ())
	{
		Transaction* tp = _transactions[_activeTransactionIdx];
		//_transactions[_activeTransactionIdx];
		//UnlockPortRes();
		return tp;
	}
	//UnlockPortRes();
	return NULL;
}
///____________________________________________________________________________

std::wstring s2ws (const std::string & s)
{
	int len;
	int slength = (int)s.length () + 1;
	len = MultiByteToWideChar (CP_ACP, 0, s.c_str (), slength, 0, 0);
	std::wstring r (len, L'\0');
	MultiByteToWideChar (CP_ACP, 0, s.c_str (), slength, &r[0], len);
	return r;
}
///____________________________________________________________________________
// \brief This function initializes com port class object
// \return nonzero in case of error
int CSerialDev::Init ()
{
	time_t tCurTime;
	time (&tCurTime);
	wstring comname = L"\\\\.\\" + s2ws (_port);// StringToWString(string("\\\\.\\")+_port).c_str()
	_hComm = CreateFile (comname.c_str (), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);// | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, 0);

	if (_hComm == INVALID_HANDLE_VALUE)
	{
		Mess->Out ("Can't open port (%s): %s", _port.c_str (), GetLastErrorAsLPSTR ());

		_tRepeatTime = tCurTime + _lConnectRepeatTimeout;
		return -1;
	}

	DWORD dwCommEvents = EV_RXCHAR | 0;
	//if (! ::SetCommMask(hComm, EV_RXCHAR|EV_TXEMPTY) )
	if (!::SetCommMask (_hComm, dwCommEvents))
	{
		Mess->Out ("Can't set port (%s) parameters: %s", _port.c_str (), GetLastErrorAsLPSTR ());
		return -1;
	}

	DCB dcb = { 0 };
	dcb.DCBlength = sizeof (DCB);

	if (!::GetCommState (_hComm, &dcb))
	{
		Mess->Out ("Failed to Get Comm State: %s", GetLastErrorAsLPSTR ());
		return E_FAIL;
	}

	dcb.BaudRate = _baudRate;	// 9600;
	dcb.ByteSize = _dataBits;	//8;
	dcb.Parity = _parity;		//0;
	dcb.StopBits = _stopBit * 2 - 2;		//ONESTOPBIT;

	dcb.fDsrSensitivity = 0;
	dcb.fDtrControl = (_flowCtrl == 0) ? DTR_CONTROL_DISABLE : DTR_CONTROL_ENABLE;// ENABLE;
	dcb.fOutxDsrFlow = 0;

	if (!::SetCommState (_hComm, &dcb))
	{
		Mess->Out ("Failed to Set Comm State (%s): %s", _port.c_str (), GetLastErrorAsLPSTR ());
		return E_FAIL;
	}

	COMMTIMEOUTS timeouts;
	/*timeouts.ReadIntervalTimeout			= 1000;//MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier		= 1000;
	timeouts.ReadTotalTimeoutConstant		= 1000;//MAXDWORD;
	timeouts.WriteTotalTimeoutMultiplier	= 1000;
	timeouts.WriteTotalTimeoutConstant		= 1000;//MAXDWORD;*/
	timeouts.ReadIntervalTimeout = 300 * 4;//MAXDWORD; 
	timeouts.ReadTotalTimeoutMultiplier = 5 * 4;
	timeouts.ReadTotalTimeoutConstant = 5 * 4;//MAXDWORD;
	timeouts.WriteTotalTimeoutMultiplier = 5 * 4;
	timeouts.WriteTotalTimeoutConstant = 1000 * 4;

	if (!SetCommTimeouts (_hComm, &timeouts))
	{
		Mess->Out ("Error setting time-outs (%s): %s", _port.c_str (), GetLastErrorAsLPSTR);
		return -1;
	}
	PurgeComm (_hComm, PURGE_RXABORT | PURGE_RXCLEAR);
	//LockPortRes();
	for (std::vector<Transaction*>::iterator it = _transactions.begin (); it != _transactions.end (); ++it)
	{
		(*it)->SetStartTime (tCurTime);
		(*it)->SetFireTime (tCurTime);
	}
	//UnlockPortRes();
	_activeTransactionIdx = 0;

	memset (&_ovWrite, 0, sizeof (_ovWrite));
	memset (&_ovRead, 0, sizeof (_ovRead));

	_ovWrite.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	_ovRead.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);

	if (_ovWrite.hEvent == NULL || _ovRead.hEvent == NULL)
	{	// Error creating overlapped event handles. 
		Mess->Out ("Error creating overlapped events (%s)", _port.c_str ());
		return -1;
	}
	return 0;
}
///____________________________________________________________________________

void CSerialDev::Disconnect ()
{
	if (_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle (_hComm);
		_hComm = INVALID_HANDLE_VALUE;
	}
}
///____________________________________________________________________________

int CSerialDev::SendTransaction (Transaction * Tr)
{
	DWORD dwWritten;

	CHECK_TIME

		if (!Tr->PrepareData ())
		{
			return -1;
		}
	Tr->PrepareToSend ();
	PurgeComm (_hComm, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
	ResetEvent (_ovRead.hEvent);
	if (!WriteFile (_hComm, Tr->GetOutBuffer (), Tr->GetOutLength (), &dwWritten, &_ovWrite))
	{
		if (GetLastError () != ERROR_IO_PENDING)
		{
			// WriteFile failed, but it isn't delayed. Report error and abort.  
			return -1;
		}
		else
		{
			DWORD bytesWritten;
			//set wait for send state
			if (!GetOverlappedResult (_hComm, &_ovWrite, &bytesWritten, TRUE))
			{
				Mess->Out ("[%s.%s] SerialDev:SendTransaction:GetOverlappedResult FAILED", Tr->GetParamName ().c_str (), Tr->GetCommName ());
			}
			else
			{
				Mess->Out ("[%s.%s] TX: (%d)", Tr->GetParamName ().c_str (), Tr->GetCommName (), bytesWritten);
			}
		}
	}
	else
	{
		Mess->Out ("[%s.%s] TX: (%d)", Tr->GetParamName ().c_str (), Tr->GetCommName (), Tr->GetOutLength ()/*, _port.c_str ()*/);
		// WriteFile completed immediately. 
	}

	FINISHCHECK_TIME ("SendTransaction");	//::Sleep(150);	
	return 0;
}
///____________________________________________________________________________

int cntRead = 0;
COMSTAT comStat;
DWORD   dwErrors;

///____________________________________________________________________________

int CSerialDev::RecvTransaction ()
{
	Transaction* Tr = _transactions[_activeTransactionIdx];
	UCHAR* addr = Tr->GetInBuffer ();
	int size = Tr->GetInSize ();
	int cnt = 0;

	_ovRead.InternalHigh = 0;
	_ovRead.OffsetHigh = 0;
	_ovRead.Offset = 0;

	if (!ClearCommError (_hComm, &dwErrors, &comStat))
		return 1;				// Report error in ClearCommError. 

	//while (_ovRead.InternalHigh == 0)
	{
		if (!ReadFile (_hComm, addr, size, &_dwBytesRead, &_ovRead))
		{
			if (GetLastError () != ERROR_IO_PENDING)
			{			// read not delayed? 
						// Error in communications; report it.
				Mess->Out ("[%s.%s] RX:(%d) Err: %s", Tr->GetParamName ().c_str (), Tr->GetCommName (), _dwBytesRead, GetLastErrorAsLPSTR ());
				return 1;
			}
			//else {ADD	Mess->Out ("Pending ReadFile: %d bytes", _dwBytesRead);}
		}
		else
		{
			Mess->Out ("[%s.%s] RX:(%d)", Tr->GetParamName ().c_str (), Tr->GetCommName (), _dwBytesRead);
			if (_dwBytesRead > 0)
			{
				cntRead += _dwBytesRead;
			}
		}
		cnt++;
	}
	return 0;
}
///____________________________________________________________________________

int CSerialDev::Process (/*time_t procTime*/)
{
	time_t tCurTime;
	DWORD dwRes;
	time (&tCurTime);

	switch (_state)
	{
	case SD_INIT:
		if (difftime (tCurTime, _tRepeatTime) < 0)
		{
			return 1;
		}

		if (Init () == 0)
		{
			_state = SD_NEXT_REQUEST;
		}
		break;
	case SD_NEXT_REQUEST:
		// wrap around vector
		//LockPortRes();
		if (_activeTransactionIdx == _transactions.size ())
			_activeTransactionIdx = 0;

		// find transaction to send
		for (_activeTransactionIdx; _activeTransactionIdx != _transactions.size (); ++_activeTransactionIdx)
		{
			auto Tr = GetTransaction ();

			if (_transactions[_activeTransactionIdx]->isFire (tCurTime))
			{
				// SendRequest		
				if (SendTransaction (Tr))
				{
					Mess->Out ("[%s.%s] Can't send request", Tr->GetParamName ().c_str (), Tr->GetCommName ());
					SetTransactionStatus (-(char)Tr->GetCommand ());//ADD
					NextRequest (tCurTime, false);					//UnlockPortRes();
					return 1;
				}
				_tTransactionStartTime = tCurTime;				//_tTransactionStartTick = ::GetTickCount ();				//LogRequestCB ();
				SetLastError (ERROR_SUCCESS);
				if (RecvTransaction ())
				{
					Mess->Out ("[%s.%s] Can't Recv response", Tr->GetParamName ().c_str (), Tr->GetCommName ());
					_state = SD_NEXT_REQUEST;
					NextRequest (tCurTime, false); // free resource on current com port
					return 1;
				}
				_state = SD_WAIT_REQUEST_FINISH;				//UnlockPortRes();
				return 0;
			}
		}
		break;
	case SD_WAIT_REQUEST_FINISH:
		dwRes = WaitForSingleObject (_ovRead.hEvent, 0);		//FINISHCHECK_TIME("WaitForSingleObject WAIT REQUEST");

		switch (dwRes)
		{
		case WAIT_OBJECT_0:
		{
			CHECK_TIME
			Transaction* Tr = _transactions[_activeTransactionIdx];

			if (!GetOverlappedResult (_hComm, &_ovRead, &_dwBytesRead, FALSE))
			{
				//LockPortRes();
				Mess->Out ("[%s.%s] Can't Recv Delayed response", Tr->GetParamName ().c_str (), Tr->GetCommName ());
				SetTransactionStatus (-(char)GetTransaction ()->GetCommand ());//ADD
				_state = SD_NEXT_REQUEST;
				NextRequest (tCurTime, false);					// free resource on current com port				//UnlockPortRes();
				return 1;
			}
			else
			{
				if (_dwBytesRead > 0)
				{						//LockPortRes();
					switch (Tr->ProcessResponse (_dwBytesRead))
					{
					case PROC_RESP_RETVAL_SUCCESS:
						Tr->ReadDone ();		// special processing for actual protocol on successful input packet 
						// Successfully processed
						Mess->Out ("[%s.%s](%s) Done, %d bytes", Tr->GetParamName ().c_str (), Tr->GetCommName (), _port.c_str (), _dwBytesRead);
						SetTransactionStatus (GetTransaction ()->GetCommand ()); // set good status;
						_state = SD_NEXT_REQUEST;
						if (Tr->isNeedContinuation ())
						{ // repeat the same packet to get all archive data
							return 0;
						}
						NextRequest (tCurTime, true);
						return 0;
					case PROC_RESP_RETVAL_FAIL:
						Mess->Out ("[%s.%s] Failed to process response (read %d of %d)", Tr->GetParamName ().c_str (), Tr->GetCommName (),
							_dwBytesRead, Tr->GetInSize ());
						SetTransactionStatus (-(char)GetTransaction ()->GetCommand ()); // set bad status;
						_state = SD_NEXT_REQUEST;
						NextRequest (tCurTime, false);
						//UnlockPortRes();
						return 1;
					case PROC_RESP_RETVAL_NOTREADY:
						Mess->Out ("[%s.%s] Waiting for full buffer (read %d of %d)", Tr->GetParamName ().c_str (), Tr->GetCommName (),
							_dwBytesRead, Tr->GetInSize ());
						SetTransactionStatus (-(char)GetTransaction ()->GetCommand ()); // set bad status;
						_state = SD_NEXT_REQUEST;
						NextRequest (0, false);
						break;
					}
				}
				else
				{ // wrong behaviour
					if (!ClearCommError (_hComm, &dwErrors, &comStat))
						// Report error in ClearCommError. 
						return 1;
					if (dwErrors == 0 && comStat.cbInQue != 0)
					{
						if (RecvTransaction ())
						{
							Mess->Out ("[%s.%s] Can't Recv response", Tr->GetParamName ().c_str (), Tr->GetCommName ());
							SetTransactionStatus (-(char)GetTransaction ()->GetCommand ()); // set bad status;
							_state = SD_NEXT_REQUEST;
							NextRequest (tCurTime, false); // free resource on current com port
							return 1;
						}
					}
				}
			}
			if (tCurTime > (_tTransactionStartTime + _dwTransactionTimeout))
			{
				Mess->Out ("[%s.%s] Timeout off, read bytes: %d", Tr->GetParamName ().c_str (), Tr->GetCommName (),
					_dwBytesRead);
				SetTransactionStatus (-(char)GetTransaction ()->GetCommand ()); // set bad status;
				_state = SD_NEXT_REQUEST;
				//Transaction * Tr = _transactions[_activeTransactionIdx];
				Tr->SetBadResponse ();
				NextRequest (tCurTime, false); // free resource on current com port
				return 1;
			}
			FINISHCHECK_TIME ("GetOverlappedResult WAIT REQUEST");
			break;
		}
		case WAIT_TIMEOUT:
			break;
		default:
			Transaction* Tr = _transactions[_activeTransactionIdx];
			Mess->Out ("[%s.%s] Error on GetOverlappedResult: %s", Tr->GetParamName ().c_str (), Tr->GetCommName (),
				GetLastErrorAsLPSTR ());//err
			break;
		}

		break;
	default:
		Transaction* Tr = _transactions[_activeTransactionIdx];
		Mess->Out ("[%s.%s] SerialDev::Process: UNKNOWN STATE (%s)", Tr->GetParamName ().c_str (), Tr->GetCommName (), GetLastErrorAsLPSTR ());
		break;
	}
	return 0;
}
