/*
    libcl2 - common library version 3
    Copyright (C) 2013	Simon Brennecke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef	_include_cl3_system_error_h_
#define	_include_cl3_system_error_h_

#include "os.h"

namespace	cl3
{
	namespace	system
	{
		namespace	error
		{
			class	TCoreException
			{
				private:
					char* message;			//	exception-specific human-readable description message
					void* object;			//	this-pointer of object that raised the exception
					const char* codefile;	//	sourcecode file (from __FILE__ macro)
					const char* classname;	//	name of the class which holds the function that raised the exception (from __CLASS__ macro)
					const char* function;	//	function name in which the exception occurecd (from __FUNCDNAME__ or __PRETTY_FUNCTION__ macro)
					const char* condition;	//	the condition that triggered the exception (from ERROR(condition, excepclass, args))
					TCoreException* inner;	//	inner exception (can be NULL)
					unsigned codeline;		//	sourcecode line number (from __LINE__ macro)

				public:
					CLASS	TCoreException	(const char* format, ...);	//	printf-syle ctor
					CLASS	TCoreException	(const TCoreException&);
					virtual	~TCoreException	();
					void	Set	(void* object, const char* codefile, const char* classname, const char* function, const char* condition, TCoreException* inner, unsigned codeline);
			};

			#if (_CXX == CXX_GNU)
				//	general purpose error macro
				#define	ERROR	(condition, TExceptionClass, ...) do
				{
					if(condition)
					{
						TExceptionClass exception(__VA_ARGS__);
						exception.Set(NULL, __FILE__, __CLASS__, __PRETTY_FUNCTION__, #condition, NULL, __LINE__);
						throw exception;
					}
				} while(false)

				//	use this macro inside non-static member functions - but only if the object is still valid when the exception is caught
				#define	OBJECT_ERROR	(condition, TExceptionClass, ...) do
				{
					if(condition)
					{
						TExceptionClass exception(__VA_ARGS__);
						exception.Set(this, __FILE__, __CLASS__, __PRETTY_FUNCTION__, #condition, NULL, __LINE__);
						throw exception;
					}
				} while(false)

				//	this macro forwards the error - use is if you catch an exception and want to pass if downstream as inner exception
				#define	FORWARD_ERROR	(inner, TExceptionClass, ...) do
				{
					TExceptionClass exception(__VA_ARGS__);
					exception.Set(NULL, __FILE__, __CLASS__, __PRETTY_FUNCTION__, NULL, inner, __LINE__);
					throw exception;
				} while(false)

				//	same as above but for non-static member functions
				#define	FWDOBJ_ERROR	(inner, TExceptionClass, ...) do
				{
					TExceptionClass exception(__VA_ARGS__);
					exception.Set(this, __FILE__, __CLASS__, __PRETTY_FUNCTION__, NULL, inner, __LINE__);
					throw exception;
				} while(false)
			#endif
		}
	}
}

#endif
