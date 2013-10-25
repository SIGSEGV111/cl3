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

#include <stdio.h>
#include "error.h"

namespace	cl3
{
	namespace	system
	{
		namespace	error
		{
			static char* mkstrcpy(const char* str)
			{
				size_t l = strlen(str) + 1;
				char* cpy = (char*)malloc(l);
				if(cpy == NULL) throw "TCoreException: out of memory (mkstrcpy)";
				memcpy(cpy, str, l);
				return cpy;
			}

			CLASS	TCoreException::TCoreException	(const char* format, ...) : message(NULL), object(NULL), codefile(NULL), classname(NULL), function(NULL), condition(NULL), inner(NULL), codeline(0)
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

			CLASS	TCoreException::TCoreException	(const TCoreException& e) : message(mkstrcpy(e.message)), object(e.object), codefile(e.codefile), classname(e.classname), function(e.function), condition(e.condition), inner(e.inner), codeline(e.codeline)
			{}

			CLASS	TCoreException::~TCoreException	()
			{
				free(message);
			}

			void	TCoreException::Set	(void* object, const char* codefile, const char* classname, const char* function, const char* condition, TCoreException* inner, unsigned codeline)
			{
				this->object    = object;
				this->codefile  = codefile;
				this->classname = classname;
				this->function  = function;
				this->condition = condition;
				this->inner     = inner;
				this->codeline  = codeline;
			}
		}
	}
}
