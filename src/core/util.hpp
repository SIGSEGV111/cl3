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

#ifndef	_include_cl3_core_util_hpp_
#define	_include_cl3_core_util_hpp_

#include "system_compiler.hpp"
#include "system_memory.hpp"
#include "system_def.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			class	ITextWriter;
		}
	}

	namespace	util
	{
		CL3PUBF	system::memory::TUniquePtr<char,system::memory::UPTR_ALLOC> mkstrcpy(const char* str, system::memory::IDynamicAllocator* allocator = NULL);

		CL3PUBF	void	Hexdump	(const void* p_mem, system::types::usys_t sz_mem, io::text::ITextWriter&);

		template<typename R, typename... Args>
		struct ICallable
		{
			virtual R Call(Args...) const = 0;
			virtual ICallable* Clone() const = 0;
			virtual ~ICallable() {}
		};

		template<typename T, typename R, typename... Args>
		struct TMemberFunctionCallable : ICallable<R, Args...>
		{
			T* obj;
			R (T::*f)();

			R Call(Args... a) const final override { return (this->obj->*this->f)(system::def::forward<Args>(a)...); }

			ICallable<R, Args...>* Clone() const final override { return new TMemberFunctionCallable<T, R, Args...>(*this); }

			TMemberFunctionCallable(T* obj, R (T::*f)(Args...)) : obj(obj), f(f) {}
			~TMemberFunctionCallable() {}
		};

		template<typename L, typename R, typename... Args>
		struct TLambdaFunctionCallable : ICallable<R, Args...>
		{
			L l;

			R Call(Args... a) const final override { return this->l(system::def::forward<Args>(a)...); }

			ICallable<R, Args...>* Clone() const final override { return new TLambdaFunctionCallable<L, R, Args...>(l); }

			TLambdaFunctionCallable(L l) : l(l) {}
			~TLambdaFunctionCallable() {}
		};

		template<typename R, typename... Args>
		class TFunction
		{
			protected:
				system::memory::TUniquePtr<ICallable<R, Args...>> callable;

			public:
				R operator()(Args... a) const { return this->callable->Call(system::def::forward<Args>(a)...); }

				TFunction(const TFunction& rhs) : callable(system::memory::MakeUniquePtr(rhs.callable->Clone())) {}
				TFunction(TFunction&& rhs) : callable(system::def::move(rhs.callable)) {}
				~TFunction() = default;

				template<typename T>
				TFunction(T* obj, R (T::*f)(Args...)) : callable(system::memory::MakeUniquePtr(new TMemberFunctionCallable<T, R, Args...>(obj, f))) {}

				template<typename L>
				TFunction(L l) : callable(system::memory::MakeUniquePtr(new TLambdaFunctionCallable<L, R, Args...>(l))) {}

		};
	}
}

#endif
