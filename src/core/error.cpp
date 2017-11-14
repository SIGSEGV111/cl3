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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include <stdio.h>
#include <stdarg.h>
#include "error.hpp"
#include "util.hpp"
#include "system_memory.hpp"
#include "io_text.hpp"
#include "io_text_terminal.hpp"

namespace	cl3
{
	namespace	error
	{
		using namespace system::memory;

		CLASS	TException::TException	(const char* format, ...) : message(NULL), object(NULL), codefile(NULL), function(NULL), expression(NULL), inner(NULL), codeline(0)
		{
			CL3_CONTEXT_VARIABLE_PUSH(allocator_generic, allocator_exception.Value());

			if(format)
			{
				va_list list;
				va_start(list, format);
				int l = vsnprintf(NULL, 0, format, list) + 1;
				va_end(list);
				if(l <= 0) throw "TException: printf format error (ctor)";

				va_start(list, format);
				message = (char*)Alloc(l, NULL);
 				if(message == NULL) { va_end(list); throw "TException: out of memory (ctor)"; }
				vsnprintf(message, l, format, list);
				va_end(list);
			}
		}

		CLASS	TException::TException	(TException&& e) : message(e.message), object(e.object), codefile(e.codefile), function(e.function), expression(e.expression), inner(e.inner), codeline(e.codeline)
		{
			e.message = NULL;
		}

		CLASS	TException::TException	(const TException& e) : message(NULL), object(e.object), codefile(e.codefile), function(e.function), expression(e.expression), inner(e.inner), codeline(e.codeline)
		{
			CL3_CONTEXT_VARIABLE_PUSH(allocator_generic, allocator_exception.Value());
			this->message = util::mkstrcpy(e.message).Claim();
		}

		CLASS	TException::~TException	()
		{
			Free(message);
		}

		void	TException::Set		(const void* object, const char* codefile, const char* function, const char* expression, TException* inner, unsigned codeline)
		{
			this->object    = object;
			this->codefile  = codefile;
			this->function  = function;
			this->expression = expression;
			this->inner     = inner;
			this->codeline  = codeline;

			#ifdef CL3_DEBUG
				this->Print();
			#endif
		}

		void	TException::Print	() const
		{
			io::text::terminal::StdErr()<<*this;
		}

		io::text::ITextWriter&	operator<<	(io::text::ITextWriter& tw, const TException& ex)
		{
			tw<<"ERROR:"
					<<"\n\tmessage: "<<ex.message
					<<"\n\tfile: "<<ex.codefile<<":"<<ex.codeline
					<<"\n\texpression: "<<(ex.expression == NULL ? "<none>" : ex.expression)
					<<"\n";

			if(ex.inner)
				tw<<"\n\tINNER "<<(*ex.inner);

			return tw;
		}

		CLASS	TNotImplementedException::TNotImplementedException	() : TException("function or feature not implemented yet")
		{
		}

		CLASS	TNotImplementedException::TNotImplementedException	(TNotImplementedException&& nie) : TException(static_cast<TException&&>(nie))
		{
		}

		CLASS	TNotImplementedException::~TNotImplementedException	()
		{
		}

		CLASS	TLogicException::TLogicException	() : TException("programming error: the actual program logic violated the expectations of the programmer")
		{
		}

		CLASS	TLogicException::TLogicException	(TLogicException&& le) : TException(static_cast<TException&&>(le))
		{
		}

		CLASS	TLogicException::~TLogicException	()
		{
		}

		CLASS TInvalidArgumentException::TInvalidArgumentException	(const char* argname) : TException("an invalid value was passed as argument"), argname(util::mkstrcpy(argname).Claim())
		{
		}

		CLASS TInvalidArgumentException::~TInvalidArgumentException	()
		{
			Free(this->argname);
		}
	}
}
