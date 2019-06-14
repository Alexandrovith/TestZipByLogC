#pragma once

#include <exception>

//EXTERN_C MYDLL_API const char* WinSockErrCodeAsStr ();
//EXTERN_C MYDLL_API const LPSTR GetLastErrorAsLPSTR ();

namespace Messages
{
	class CErrSock : public std::exception
	{
	public:
		CErrSock (const char* cpMess);
		~CErrSock ();

	};

	class CErrMess : public std::exception
	{
	public:
		CErrMess () {}
		CErrMess (const char* cpMess);
		~CErrMess ();
	};

}