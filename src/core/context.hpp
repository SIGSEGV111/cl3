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
		enum	EContext
		{
			CONTEXT_GLOBAL,
			CONTEXT_THREAD,
			CONTEXT_LOCAL
		};

		template<class T>
		struct	IContextVariable
		{
			virtual	T		Value	() const CL3_GETTER = 0;	//	returns CONTEXT_LOCAL is set, otherwise it returns CONTEXT_THREAD if set, otherwise it returns CONTEXT_GLOBAL
			virtual	T		Value	(EContext) const CL3_GETTER = 0;	//	returns the value for the specified context
			virtual	void	Value	(EContext, T) CL3_SETTER = 0;	//	sets the value for the specified context
			virtual	void	Clear	(EContext) = 0;	//	clears the value for the specified context
			inline	T	operator()	() const CL3_GETTER { return Value(); }
		};

		#define	CL3_CONTEXT_VARIABLE_IMPL(type, name, initvalue) \
			struct	CL3_PASTE(TContextVariable_, name) : cl3::context::IContextVariable<type> \
			{ \
				static CL3_THREAD bool value_thread_isset; \
				static CL3_THREAD bool value_local_isset; \
				static type value_global; \
				static CL3_THREAD type value_thread; \
				static CL3_THREAD type value_local; \
 \
				type		Value	() const final override \
				{ \
					if(value_local_isset) \
						return value_local; \
					if(value_thread_isset) \
						return value_thread; \
					return value_global; \
				} \
 \
				type		Value	(cl3::context::EContext c) const final override \
				{ \
					switch(c) \
					{ \
						case cl3::context::CONTEXT_LOCAL: \
							CL3_CLASS_ERROR(!value_local_isset, cl3::error::TException, "local context value is not set"); \
							return value_local; \
						case cl3::context::CONTEXT_THREAD: \
							CL3_CLASS_ERROR(!value_thread_isset, cl3::error::TException, "thread context value is not set"); \
							return value_thread; \
						case cl3::context::CONTEXT_GLOBAL: \
							return value_global; \
					} \
					CL3_CLASS_FAIL(cl3::error::TException, "invalid argumemt value"); \
				} \
 \
				void	Value	(cl3::context::EContext c, type v) final override \
				{ \
					switch(c) \
					{ \
						case cl3::context::CONTEXT_LOCAL: \
							value_local = v; \
							value_local_isset = true; \
							break; \
						case cl3::context::CONTEXT_THREAD: \
							value_thread = v; \
							value_thread_isset = true; \
							break; \
						case cl3::context::CONTEXT_GLOBAL: \
							value_global = v; \
							break; \
					} \
				} \
 \
				void	Clear	(cl3::context::EContext c) final override \
				{ \
					switch(c) \
					{ \
						case cl3::context::CONTEXT_LOCAL: \
							value_local_isset = false; \
							break; \
						case cl3::context::CONTEXT_THREAD: \
							value_thread_isset = false; \
							break; \
						case cl3::context::CONTEXT_GLOBAL: \
							CL3_CLASS_FAIL(cl3::error::TException, "cannon clear global context, as it is the default fallback"); \
							break; \
					} \
				} \
			} CL3_PASTE(__context_variable_, name); \
			CL3_THREAD bool CL3_PASTE(TContextVariable_, name)::value_thread_isset = false; \
			CL3_THREAD bool CL3_PASTE(TContextVariable_, name)::value_local_isset = false; \
			type CL3_PASTE(TContextVariable_, name)::value_global = (initvalue); \
			CL3_THREAD type CL3_PASTE(TContextVariable_, name)::value_thread = (initvalue); \
			CL3_THREAD type CL3_PASTE(TContextVariable_, name)::value_local = (initvalue); \
			cl3::context::IContextVariable<type>& name = CL3_PASTE(__context_variable_, name);

		#define	CL3_CONTEXT_VARIABLE_DECL(type, name)	CL3PUBV	cl3::context::IContextVariable<type>& name;

		template<class T>
		struct	TLocalValueHolder
		{
			IContextVariable<T>* variable;
			T old_value;

			CLASS	TLocalValueHolder	(IContextVariable<T>& variable, T new_value) : variable(&variable), old_value(variable.Value())
			{
				this->variable->Value(CONTEXT_LOCAL, new_value);
			}

			CLASS	TLocalValueHolder	(TLocalValueHolder&& other) : variable(other.variable), old_value(other.old_value)
			{
				other.variable = NULL;
			}

			CLASS	~TLocalValueHolder	()
			{
				if(this->variable)
					this->variable->Value(CONTEXT_LOCAL, old_value);
			}
		};
	}
}

#endif
