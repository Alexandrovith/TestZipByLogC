#pragma once
#include <windows.h>
#include "TransactionParam.h"
#include <string>
#include <map>

using namespace std;

typedef struct {
	UCHAR syncbyte;
	UCHAR addr;
	UCHAR size;
	UCHAR cmd;
}prefix_t;


class SFproto
{
public:
	prefix_t *m_prefix;
	UCHAR *m_rawData;
	SFproto(void);
	SFproto(UCHAR* pData);
	~SFproto(void);
	bool CRC16 (bool writeflag);
	static bool ExtCRC16 (UCHAR*ExtBuf, bool writeflag);
	bool Add(UCHAR * buf, int cnt);
	void Clear();
	
private:
	short crc;
	int bytecnt;
	static UCHAR auchCRCHi[256];
	static UCHAR auchCRCLo[256];
};

