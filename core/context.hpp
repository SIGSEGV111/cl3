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

#ifndef	_include_cl3_core_context_hpp_
#define	_include_cl3_core_context_hpp_

#include "system_compiler.hpp"

namespace	cl3
{
	namespace	context
	{
		template<class T>
		class	TParameterStack
		{
			private:
				T* current_value;

			public:
				typedef T TValue;
				T*		Current	() { return reinterpret_cast<T*>(current_value); }
				void	Current	(T* new_value) { current_value = reinterpret_cast<void*>(new_value); }
		};

		template<class T, TParameterStack<T>& stack>
		class	TParameterHolder
		{
			private:
				T* old_value;

				CLASS	TParameterHolder	(const TParameterHolder&);

			public:
				CLASS	TParameterHolder	(T* new_value) throw() : old_value(stack.Current()) { stack.Current(new_value); }
				CLASS	~TParameterHolder	() { stack.Current(old_value); }
		};

		#define	CONTEXT_PARAMETER(stack, new_value) cl3::context::TParameterHolder<decltype(stack)::TValue> CL3_PASTE(__context_parameter_, __COUNTER__)(stack, new_value)
	}
}

#endif
