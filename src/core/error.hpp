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

#ifndef	_include_cl3_core_error_base_hpp_
#define	_include_cl3_core_error_base_hpp_

#include "system_os.hpp"
#include "system_compiler.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			class	ITextWriter;
		}
	}

	namespace	error
	{
		class	CL3PUBT	TException
		{
			private:
				TException& operator=(const TException&);	//	not available

			public:
				char* message;			//	exception-specific human-readable description message
				const void* object;		//	this-pointer of object that raised the exception
				const char* codefile;	//	sourcecode file (from __FILE__ macro)
				const char* function;	//	function name in which the exception occurecd (from __FUNCDNAME__ or __PRETTY_FUNCTION__ macro)
				const char* expression;	//	the expression that triggered the exception (from ERROR(expression, excepclass, args))
				TException* inner;		//	inner exception (can be NULL)
				unsigned codeline;		//	sourcecode line number (from __LINE__ macro)

				CL3PUBF	CLASS	TException	(const char* format, ...);	//	printf-syle ctor
				CL3PUBF	CLASS	TException	(TException&&);
				CL3PUBF	CLASS	TException	(const TException&);
				virtual	CLASS	~TException	();

				CL3PUBF	void	Set			(const void* object, const char* codefile, const char* function, const char* expression, TException* inner, unsigned codeline);
				CL3PUBF	void	Print		() const;
		};

		CL3PUBF	io::text::ITextWriter&	operator<<	(io::text::ITextWriter&, const TException&);

		class	CL3PUBT	TSyscallException : public TException
		{
			public:
				#if (CL3_OS == CL3_OS_POSIX)
					int err_no;	//	changed to err_no, because "errno" might be a macro and the preprocessor would do evil things if we would use the same name
				#elif (CL3_OS == CL3_OS_WINDOWS)
					DWORD err_no;	//	changed to err_no, because "errno" might be a macro and the preprocessor would do evil things if we would use the same name
				#endif
				char message_buffer[256];	//	hopefully no system error message will be longer than this

			public:
				CL3PUBF	CLASS	TSyscallException	();
				CL3PUBF	CLASS	TSyscallException	(int err_no);
				CL3PUBF	CLASS	TSyscallException	(TSyscallException&&);
				CL3PUBF	virtual	~TSyscallException	();
		};

		class	CL3PUBT	TNotImplementedException : public TException
		{
			public:
				CL3PUBF	CLASS	TNotImplementedException	();
				CL3PUBF	CLASS	TNotImplementedException	(TNotImplementedException&&);
				CL3PUBF	virtual	~TNotImplementedException	();
		};

		class	CL3PUBT	TLogicException : public TException
		{
			public:
				CL3PUBF	CLASS	TLogicException	();
				CL3PUBF	CLASS	TLogicException	(TLogicException&&);
				CL3PUBF	virtual	~TLogicException();
		};

		class CL3PUBT TInvalidArgumentException : public TException
		{
			public:
				char* const argname;

				CL3PUBF	CLASS	TInvalidArgumentException	(const char* argname);
				CLASS	TInvalidArgumentException	(TInvalidArgumentException&&) = default;
				CL3PUBF	virtual	~TInvalidArgumentException	();
		};

		#define	CL3_CLASS_LOGIC_ERROR(expression)	do \
			{ \
				if(CL3_UNLIKELY( (expression) )) \
				{ \
					cl3::error::TLogicException le; \
					le.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw le; \
				} \
			} while(false)

		#define	CL3_NONCLASS_LOGIC_ERROR(expression)	do \
			{ \
				if(CL3_UNLIKELY( (expression) )) \
				{ \
					cl3::error::TLogicException le; \
					le.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw le; \
				} \
			} while(false)

		#define	CL3_NOT_IMPLEMENTED	do \
			{ \
				cl3::error::TNotImplementedException nie; \
				nie.Set(NULL, __FILE__, __PRETTY_FUNCTION__, NULL, NULL, __LINE__); \
				throw nie; \
			} while(false)

		#if (CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_CLANG)
			//	general purpose error macro
			#define	CL3_NONCLASS_ERROR(expression, TExceptionClass, ...) do \
			{ \
				if(CL3_UNLIKELY( (expression) )) \
				{ \
					TExceptionClass exception(__VA_ARGS__); \
					exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			//	use this macro inside non-static member functions - but only if the object is still valid when the exception is caught
			#define	CL3_CLASS_ERROR(expression, TExceptionClass, ...) do \
			{ \
				if(CL3_UNLIKELY( (expression) )) \
				{ \
					TExceptionClass exception(__VA_ARGS__); \
					exception.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			#define	CL3_CLASS_ERROR_NOARGS(expression, TExceptionClass) do \
			{ \
				if(CL3_UNLIKELY( (expression) )) \
				{ \
					TExceptionClass exception; \
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
				if( CL3_UNLIKELY((long)(expression) == -1L) ) \
				{ \
					cl3::error::TSyscallException exception; \
					exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			#define	CL3_CLASS_SYSERR(expression) do \
			{ \
				if( CL3_UNLIKELY((long)(expression) == -1L) ) \
				{ \
					cl3::error::TSyscallException exception; \
					exception.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			#define	CL3_NONCLASS_PTHREAD_ERROR(expression) do \
			{ \
				int err_no; \
				if( CL3_UNLIKELY( (err_no = (expression)) != 0 ) ) \
				{ \
					cl3::error::TSyscallException exception(err_no); \
					exception.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)

			#define	CL3_CLASS_PTHREAD_ERROR(expression) do \
			{ \
				int err_no; \
				if( CL3_UNLIKELY( (err_no = (expression)) != 0 ) ) \
				{ \
					cl3::error::TSyscallException exception(err_no); \
					exception.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					throw exception; \
				} \
			} while(false)
		#endif
	}
}

#endif
