#include "stdafx.h"
#include "TransactionParam.h"



TransactionParam::TransactionParam(void): _offset(0), _type(SFPARAM_TYPE_BYTE), _size(1), _unit("")
{
}

TransactionParam::~TransactionParam(void)
{
	_data.clear ();
	//delete [] _data;
}

TransactionParam::TransactionParam(int offset, PARAM_TYPE type, UCHAR size, string unit, UCHAR * data):
	_offset(offset), _type(type), _size(size), _unit(unit)
{
	_data.assign(data, data + size);
	//_data = new UCHAR(size);
	//memcpy(_data, data, size);
}


