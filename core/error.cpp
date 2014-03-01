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
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include "error.hpp"
#include "util.hpp"
#include "system_types_typeinfo.hpp"

namespace	cl3
{
	namespace	error
	{
		CLASS	TCoreException::TCoreException	(const char* format, ...) : message(NULL), object(NULL), codefile(NULL), function(NULL), expression(NULL), inner(NULL), codeline(0)
		{
			va_list list;
			va_start(list, format);
			int l = vsnprintf(NULL, 0, format, list) + 1;
			va_end(list);
			if(l <= 0) throw "TCoreException: printf format error (ctor)";

			va_start(list, format);
			message = (char*)malloc(l);
			if(message == NULL) { va_end(list); throw "TCoreException: out of memory (ctor)"; }
			vsnprintf(message, l, format, list);
			va_end(list);
		}

		CLASS	TCoreException::TCoreException	(const TCoreException& e) : message(util::mkstrcpy(e.message).Claim()), object(e.object), codefile(e.codefile), function(e.function), expression(e.expression), inner(e.inner), codeline(e.codeline)
		{}

		CLASS	TCoreException::~TCoreException	()
		{
			free(message);
		}

		void	TCoreException::Set	(const void* object, const char* codefile, const char* function, const char* expression, TCoreException* inner, unsigned codeline)
		{
			this->object    = object;
			this->codefile  = codefile;
			this->function  = function;
			this->expression = expression;
			this->inner     = inner;
			this->codeline  = codeline;
		}

		CLASS	TCoreArgumentException::TCoreArgumentException	(const system::types::typeinfo::TRTTI* argrtti, const char* argname, const char* argvalue) :
				TCoreException("function call argument or parameter is invalid (type: \"%s\", name: \"%s\", value: \"%s\")", argrtti != NULL ? argrtti->Name().Array() : "<?>", argname, argvalue),
				argrtti(argrtti), argname(argname), argvalue(util::mkstrcpy(argvalue).Claim())
		{
		}

		CLASS	TCoreArgumentException::TCoreArgumentException	(const TCoreArgumentException& ae) : TCoreException(ae), argrtti(ae.argrtti), argname(ae.argname), argvalue(util::mkstrcpy(ae.argvalue).Claim())
		{
		}

		CLASS	TCoreArgumentException::~TCoreArgumentException	()
		{
			free(argvalue);
		}
	}
}
