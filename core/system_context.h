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

#ifndef	_include_cl3_core_system_context_h_
#define	_include_cl3_core_system_context_h_

#include "system_compiler.h"
#include "system_memory.h"
#include "util.h"

namespace	cl3
{
	namespace	system
	{
		namespace	context
		{
			template<class T>
			class	TOptionStack
			{
				protected:
					util::TCoreList<T*> stack;

				public:
					T*		Current	()
					{
						return stack.Last();
					}

					void	Push	(T* item)
					{
						stack.Push(item);
					}

					T*		Pop		()
					{
						return stack.Pop();
					}

					CLASS	TOptionStack	(T* _default = NULL)
					{
						stack.Push(_default);
					}
			};

			template<class T>	CL3PUBF	TOptionStack<T>&	OptionStack	()
			{
				static CL3_THREAD TOptionStack<T>* instance = NULL;
				if(instance == NULL)
					instance = new TOptionStack<T>();
				return *instance;
			}

			//	specialized version, which will provide an default allocator (which is obviously required)
			template<>	CL3PUBF	TOptionStack<memory::IDynamicAllocator>&	OptionStack<memory::IDynamicAllocator>	();
		}
	}
}

#endif
