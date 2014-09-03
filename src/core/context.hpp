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

#include "system_os.hpp"
#include "system_compiler.hpp"

namespace	cl3
{
	namespace	context
	{
		#if (CL3_OS == CL3_OS_POSIX)
			template<class T>
			struct	THolder
			{
				T& current_value;
				T old_value;
				inline THolder(T& current_value, T new_value) : current_value(current_value), old_value(current_value) { current_value = new_value; }
				inline ~THolder() { current_value = old_value; }
			};

			#define	CL3_PARAMETER_STACK_DECL(type, name)	CL3PUBF extern CL3_THREAD type name##__stack_variable
			#define	CL3_PARAMETER_STACK_IMPL(type, name, value)	CL3_THREAD type name##__stack_variable = (value);
			#define	CL3_PARAMETER_STACK_PUSH(name, value)	cl3::context::THolder<decltype(name##__stack_variable)> CL3_PASTE(__stack_holder_, __COUNTER__)(name##__stack_variable, value)
			#define	CL3_PARAMETER_STACK_VALUE(name)			(static_cast<const decltype(name##__stack_variable)>(name##__stack_variable))
		#else
			template<class T, T (*get)(), void (*set)(T), T new_value>
			struct	THolder
			{
				T old_value;
				inline THolder() : old_value(get()) { set(new_value); }
				inline ~THolder() { set(old_value); }
			};

			#define	CL3_PARAMETER_STACK_DECL(type, name)	\
				typedef type name##__typedef; \
				CL3PUBF void name##__stack_setter(type); \
				CL3PUBF type name##__stack_getter()

			#define	CL3_PARAMETER_STACK_IMPL(type, name, value)	\
				static CL3_THREAD type __stack_variable_#name = (value); \
				void name##__stack_setter(type value) { __stack_variable_#name = value; } \
				type name##__stack_getter() { return __stack_variable_#name }

			#define	CL3_PARAMETER_STACK_PUSH(name, value)	cl3::context::THolder<name##__typedef, name##__stack_getter, name##__stack_setter, value> CL3_PASTE(__stack_holder_, __COUNTER__)
			#define	CL3_PARAMETER_STACK_VALUE(name)			(name##__stack_getter())
		#endif
	}
}

#endif
