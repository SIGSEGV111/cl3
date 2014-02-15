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

#include "system_context.h"
#include "system_types.h"

namespace	cl3
{
	namespace	system
	{
		namespace	context
		{
			using namespace types;

			//	FIXME: port this code to thread-local object interface (TODO)
			static CL3_THREAD byte __buffer_optionstack_dynamicallocator[sizeof(TOptionStack<memory::IDynamicAllocator>)];

			template<>	TOptionStack<memory::IDynamicAllocator>&	OptionStack<memory::IDynamicAllocator>	()
			{
				static CL3_THREAD TOptionStack<memory::IDynamicAllocator>* instance = NULL;
				if(instance == NULL)
					instance = new (__buffer_optionstack_dynamicallocator) TOptionStack<memory::IDynamicAllocator>(&memory::TDefaultAllocator::instance);
				return *instance;
			}
		}
	}
}
