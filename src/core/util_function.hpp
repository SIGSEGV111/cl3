#pragma once

#include "system_memory.hpp"

namespace cl3
{
	namespace util
	{
		namespace function
		{

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
				R (T::*f)(Args...);

				R Call(Args... a) const final override { return (this->obj->*this->f)(system::def::forward<Args>(a)...); }

				ICallable<R, Args...>* Clone() const final override { return new TMemberFunctionCallable<T, R, Args...>(*this); }

				TMemberFunctionCallable(T* obj, R (T::*f)(Args...)) : obj(obj), f(f) {}
				~TMemberFunctionCallable() {}
			};

			template<typename T, typename R, typename... Args>
			struct TStaticFunctionCallable : ICallable<R, Args...>
			{
				R (*f)(Args...);

				R Call(Args... a) const final override { return (this->f)(system::def::forward<Args>(a)...); }

				ICallable<R, Args...>* Clone() const final override { return new TStaticFunctionCallable<T, R, Args...>(*this); }

				TStaticFunctionCallable(R (*f)(Args...)) : f(f) {}
				~TStaticFunctionCallable() {}
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

				TFunction(const TFunction& rhs) : callable(system::memory::MakeUniquePtr<ICallable<R, Args...>>(rhs.callable->Clone())) {}

				TFunction(TFunction&& rhs) : callable(system::def::move(rhs.callable)) {}

				~TFunction() = default;

				template<typename T>
				TFunction(R (*f)(Args...)) : callable(system::memory::MakeUniquePtr<ICallable<R, Args...>>(new TStaticFunctionCallable<R, Args...>(f))) {}

				template<typename T>
				TFunction(T* obj, R (T::*f)(Args...)) : callable(system::memory::MakeUniquePtr<ICallable<R, Args...>>(new TMemberFunctionCallable<T, R, Args...>(obj, f))) {}

				template<typename L>
				TFunction(L l) : callable(system::memory::MakeUniquePtr<ICallable<R, Args...>>(new TLambdaFunctionCallable<L, R, Args...>(l))) {}
			};
		}
	}
}
