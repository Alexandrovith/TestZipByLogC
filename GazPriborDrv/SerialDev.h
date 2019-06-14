#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "Transaction.h"
#include "DevStorage.h"
#include "Logs_C.h"

//#define DEBUG_RAW_DATA		4
//#define DEBUG_SUBSCRIBE		8

using namespace std;
/// \brief This enum defines states of serial device
typedef enum SerialDevState
{
	SD_INIT = 0,
	SD_NEXT_REQUEST,
	SD_WAIT_REQUEST_FINISH,
};

typedef std::vector<Transaction *> TransactionList;

/// \brief This class is interface to serial device
class CSerialDev
{
public:
	std::shared_ptr<Messages::CMess> Mess;

	CSerialDev(void);
	CSerialDev(string & name, string & port, int baudRate, int dataBits, int parity, int stopBit, int flowCtrl, string & devType, string & devProtocol);
	~CSerialDev(void);

	void Close ();

	int AddRequest(Transaction * rq);
	//int AddRequest(int  reqId, long time);	
	int Process(/*time_t procTime*/);
	//int LogRequestCB(void);
	void SetConnectRepeatTimeout(long seconds);
	void InvalidateHandle(HANDLE m_hThreadStarted) { m_hThreadStarted = INVALID_HANDLE_VALUE; }
	CRITICAL_SECTION m_csLock;
	inline void	LockPortRes() { EnterCriticalSection(&m_csLock);}	
	inline void	UnlockPortRes()	{ LeaveCriticalSection(&m_csLock); }
	inline void	InitLock() { InitializeCriticalSection(&m_csLock);}
	inline void	DelLock() { DeleteCriticalSection(&m_csLock);}
	void NextRequest(time_t tCurTime, bool isSuccess);
	//int ProcessResponse(char * rdbytes, DWORD size);
	int SendTransaction (Transaction* Tr);
	int RecvTransaction();
	void SetDebug(unsigned long debug){
		_debug = debug;
	};
	unsigned long GetDebug(){
		return _debug;
	};
	void SetTryCounts(unsigned long trycounts){
		_trycounts = trycounts;
	};
	unsigned long GetTryCounts(){
		return _trycounts;
	};
	void SetTransactionTimeout(DWORD timeout){_dwTransactionTimeout = timeout;};
	DevStorage* getStorage();
	//void setStorage(DevStorage* storage);

	string GetName(){ return _devName;}; 
	string GetPort(){ return _port;}; 
	string GetDevProtocol(){return _devProtocol;};
	string GetDeviceType(){return _deviceType;};

	const TransactionList * GetTransactionsList() {return &_transactions;};
	const TransactionList GetTransactions() {return _transactions;};
	void SetTransactionStatus(char statusCmd);
	float CSerialDev::GetTransactionStatus(int count);

	int Init();
	void Disconnect ();

private:
	//xxxxxxxxxxxxxxxxxxxx       Ï Å Ð Å Ì Å Í Í Û Å       xxxxxxxxxxxxxxxxxxxxxx

	string		_devName;				///< device name
	HANDLE		_hComm;					///< serial port handle
	string		_port;					///< port Name
	int			_baudRate;				///< Baud Rate
	int			_dataBits;				///< Data Bits
	int			_parity;				///< parity
	int			_stopBit;				///< stop bits 0- One stopbit, 1- 1.5 , 2- 2
	int			_flowCtrl;				///< flow control 0- disableDTR, 1- enableDTR
	string		_devProtocol;				///< Protocol of device
	string		_deviceType;				///< Type of device
	long		_lConnectRepeatTimeout; ///< reconnect period
	//WCHAR		_wcPortName[20];		///< serial port name
	int			_sendState;				///< state of send
	time_t		_tRepeatTime;			///< when to start com port processing(Open)
	HANDLE		_hThread;				///< thread handle
	//time_t		_startTime;
	BOOL		_bStart;
	SerialDevState _state;				///< this defines serial device processor state
	OVERLAPPED	_ovWrite;				///< OVERLAPPED structure to process write request
	OVERLAPPED	_ovRead;				///< OVERLAPPED structure to process read request
	//TransactionList::iterator 
	UINT		_activeTransactionIdx;  ///< Index of current processing transaction
	TransactionList _transactions;
	BOOL		_bWaitingOnRead;		///< Flag showing current read operation state
	DWORD		_dwBytesRead;			///< Number of bytes received from serial port
	time_t		_tTransactionStartTime; ///< Start time for current transaction to process timeout
	//DWORD		_tTransactionStartTick; ///< Start system tick for current transaction to get transaction time
	DWORD		_dwTransactionTimeout;
	DevStorage*	_localStorage;			///< Storage for all parameters for current serial device
	unsigned long _debug;				///< debug level to save logs to file 
	long _trycounts;
	char	_transactionStatus1[500];	///< status of 500 last transactions
	int		_tsindex1;
	int iCountNoRespose = 0;

	Transaction * GetTransaction();
	bool ActionNoResponse (int& iCountNoConnect);

};

