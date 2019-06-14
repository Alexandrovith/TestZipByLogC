#pragma once
#include "windows.h"
#include <string>
#include <vector>
#include <map>


using namespace std;

/// \brief Thus enum defines parameter types
typedef enum PARAM_TYPE {
	SFPARAM_TYPE_BYTE = 0, 
	SFPARAM_TYPE_WORD,
	SFPARAM_TYPE_INT,
	SFPARAM_TYPE_FLOAT,
	SFPARAM_TYPE_STRING,
	SFPARAM_TYPE_YEAR,
	SFPARAM_TYPE_DAY,
	SFPARAM_TYPE_MONTH,
	SFPARAM_TYPE_HOUR,
	SFPARAM_TYPE_MINUTE,
	SFPARAM_TYPE_SECOND,
	SFPARAM_TYPE_BCD,
	SFPARAM_TYPE_FLOATBCD,
	SFPARAM_TYPE_BCDSCALED,
};

class TransactionParam{
public:

	static enum SET_PARAMETER {
		SET_OUTPUT_PARAMETER = 0,
		SET_INPUT_PARAMETER,
	};
	TransactionParam();
	TransactionParam(int offset, PARAM_TYPE type, UCHAR size, string unit, UCHAR * data);
	~TransactionParam();
	vector<UCHAR> GetData () { return _data; }
	int GetOffset () { return _offset; }
	int GetSize () { return _size; }
	const string GetUnit () { return _unit; }
	int GetType () { return _type; }
//	void Set(UCHAR*, UCHAR*);

private:
	UCHAR _offset;
	UCHAR _type;
	UCHAR _size;
	string _unit;
	vector <UCHAR> _data;
};

