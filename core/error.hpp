/*
    libcl3 - common library version 3
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

#ifndef	_include_cl3_core_error_hpp_
#define	_include_cl3_core_error_hpp_

#include "system_os.hpp"
#include "system_compiler.hpp"

namespace	cl3
{
	//	forward declaration for TRTTI
	namespace system { namespace types { namespace typeinfo { struct TRTTI; } } }

	namespace	error
	{
		class	CL3PUBT	TException
		{
			private:
				TException& operator=(const TException&);	//	not available

			protected:
				char* message;			//	exception-specific human-readable description message
				const void* object;		//	this-pointer of object that raised the exception
				const char* codefile;	//	sourcecode file (from __FILE__ macro)
				const char* function;	//	function name in which the exception occurecd (from __FUNCDNAME__ or __PRETTY_FUNCTION__ macro)
				const char* expression;	//	the expression that triggered the exception (from ERROR(expression, excepclass, args))
				TException* inner;	//	inner exception (can be NULL)
				unsigned codeline;		//	sourcecode line number (from __LINE__ macro)

			public:
				CL3PUBF	CLASS	TException	(const char* format, ...);	//	printf-syle ctor
				CL3PUBF	CLASS	TException	(const TException&);
				CL3PUBF	virtual	~TException	();
				CL3PUBF	void	Set	(const void* object, const char* codefile, const char* function, const char* expression, TException* inner, unsigned codeline);
		};

		class	CL3PUBT	TCoreArgumentException : public TException
		{
			protected:
				const system::types::typeinfo::TRTTI* argrtti;	//	datatype of the argument expressed as Run-Time-Type-Info object (can be NULL)
				const char* argname;	//	name of the argument that has an invalid value
				char* argvalue;	//	value of the argument (as human-readable string)

			public:
				CL3PUBF	CLASS	TCoreArgumentException	(const system::types::typeinfo::TRTTI* argrtti, const char* argname, const char* argvalue);
				CL3PUBF	CLASS	TCoreArgumentException	(const TCoreArgumentException&);
				CL3PUBF	virtual	~TCoreArgumentException	();
		};

		class	CL3PUBT	TSyscallException : public TException
		{
			protected:
				#if (CL3_OS == CL3_OS_POSIX)
					int err_no;	//	changed to err_no, because "errno" might be a macro and the preprocessor would do evil things if we would use the same name
				#elif (CL3_OS == CL3_OS_WINDOWS)
					DWORD err_no;	//	changed to err_no, because "errno" might be a macro and the preprocessor would do evil things if we would use the same name
				#endif
				char message_buffer[256];	//	hopefully no system error message will be longer than this

			public:
				CL3PUBF	CLASS	TSyscallException	();
				CL3PUBF	CLASS	TSyscallException	(const TSyscallException&);
				CL3PUBF	virtual	~TSyscallException	();
		};

		#if (CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_LLVM)
			//	general purpose error macro
			#define	CL3_NONCLASS_ERROR(expression, TExceptionClass, ...) do \
			{ \
				if(expression) \
				{ \
					TExceptionClass exception(__VA_ARGS__); \
					exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			//	use this macro inside non-static member functions - but only if the object is still valid when the exception is caught
			#define	CL3_CLASS_ERROR(expression, TExceptionClass, ...) do \
			{ \
				if(expression) \
				{ \
					TExceptionClass exception(__VA_ARGS__); \
					exception.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			//	this macro forwards the error - use is if you catch an exception and want to pass if downstream as inner exception
			#define	CL3_NONCLASS_FORWARD_ERROR(inner, TExceptionClass, ...) do \
			{ \
				TExceptionClass exception(__VA_ARGS__); \
				exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, NULL, inner, __LINE__); \
				throw exception; \
			} while(false)

			//	same as above but for non-static member functions
			#define	CL3_CLASS_FORWARD_ERROR(inner, TExceptionClass, ...) do \
			{ \
				TExceptionClass exception(__VA_ARGS__); \
				exception.Set(this, __FILE__, __PRETTY_FUNCTION__, NULL, inner, __LINE__); \
				throw exception; \
			} while(false)

			//	just throw without check for an expression to be true
			#define CL3_NONCLASS_FAIL(TExceptionClass, ...) do \
			{ \
				TExceptionClass exception(__VA_ARGS__); \
				exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, NULL, NULL, __LINE__); \
				throw exception; \
			} while(false)

		//	just throw without check for an expression to be true
			#define CL3_CLASS_FAIL(TExceptionClass, ...) do \
			{ \
				TExceptionClass exception(__VA_ARGS__); \
				exception.Set(this, __FILE__, __PRETTY_FUNCTION__, NULL, NULL, __LINE__); \
				throw exception; \
			} while(false)
		#endif

		#if (CL3_OS == CL3_OS_POSIX)
			#define	CL3_NONCLASS_SYSERR(expression) do \
			{ \
				if( (long)(expression) == -1L ) \
				{ \
					cl3::error::TSyscallException exception; \
					exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			#define	CL3_CLASS_SYSERR(expression) do \
			{ \
				if( (long)(expression) == -1L ) \
				{ \
					cl3::error::TSyscallException exception; \
					exception.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			#define	CL3_NONCLASS_PTHREAD_ERROR(expression) do \
			{ \
				if( (expression) != 0 ) \
				{ \
					cl3::error::TSyscallException exception; \
					exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			#define	CL3_CLASS_PTHREAD_ERROR(expression) do \
			{ \
				if( (expression) != 0 ) \
				{ \
					cl3::error::TSyscallException exception; \
					exception.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)
		#endif
		#if (CL3_OS == CL3_OS_WINDOWS)
			#define	CL3_SYSERR(expression)
		#endif

	}
}

#endif
