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

#ifndef	_include_cl3_core_system_types_typeinfo_hpp_
#define	_include_cl3_core_system_types_typeinfo_hpp_

#include <typeinfo>
#include "system_types.hpp"
#include "system_compiler.hpp"
#include "system_memory.hpp"






/*
#include <iostream>
#include <vector>

template<typename T>
class is_default_constructible
{
	private:
		typedef struct { char arr[sizeof(T)+0]; } yes;
		typedef struct { char arr[sizeof(T)+1]; } no;

		template<typename U>
		static decltype(new U(), yes()) test(int);

		template<typename>
		static no test(...);

	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(yes);
};

template<typename T>
class is_copy_constructible
{
	private:
		typedef struct { char arr[sizeof(T)+0]; } yes;
		typedef struct { char arr[sizeof(T)+1]; } no;

		template<typename U>
		static decltype(new U(*reinterpret_cast<const U*>(NULL)), yes()) test(int);

		template<typename>
		static no test(...);

	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(yes);
};

template<typename T>
class is_destructible
{
	private:
		typedef struct { char arr[sizeof(T)+0]; } yes;
		typedef struct { char arr[sizeof(T)+1]; } no;

		template<typename U>
		static decltype((delete reinterpret_cast<U*>(NULL)), yes()) test(int);

		template<typename>
		static no test(...);

	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(yes);
};

struct foo
{
	private:
// 	public:
		foo() {}
	private:
// 	public:
		foo(int) {}
	private:
// 	public:
		foo(const foo&) {}
	private:
// 	public:
		~foo() {}
};

int main()
{
	std::cout << is_default_constructible<foo>::value << '\n'		// 0
			  << is_default_constructible<std::vector<int> >::value << '\n';  // 1

	std::cout << is_copy_constructible<foo>::value << '\n'		// 0
			  << is_copy_constructible<std::vector<int> >::value << '\n';  // 1

	std::cout << is_destructible<foo>::value << '\n'		// 0
			  << is_destructible<std::vector<int> >::value << '\n';  // 1
}
*/












namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	string
			{
				class	TUString;
				typedef	system::memory::TUniquePtr<TUString>	TUStringUPtr;
			}
		}
	}

	namespace	system
	{
		namespace	types
		{
			namespace	typeinfo
			{
				//	constructors & destructor
				typedef void (*FDestructor)(void*);
				typedef void (*FStandardConstructor)(void*);
				typedef void (*FCopyConstructor)(void*, const void*);

				//	converts object value to human-readable string
				typedef	io::text::string::TUStringUPtr (*FStringify)(const void*);

				namespace	_
				{
					CL3PUBF io::text::string::TUStringUPtr generic_stringify(const void* object, size_t sz_object, io::text::string::TUStringUPtr type_name, bool is_reference, unsigned n_indirections);

					template<class T> static void generic_dtor(void* object) { reinterpret_cast<T*>(object)->~T(); }
					template<class T> static void generic_ctor(void* object) { new (object) T(); }
					template<class T> static void generic_copyctor(void* object, const void* ref) { new (object) T(*reinterpret_cast<const T*>(ref)); }

					template<class T, bool>
					struct	TImpl;

					template<class T>
					struct TImpl<T, false>
					{
						static FDestructor dtor;
						static FStandardConstructor ctor;
						static FCopyConstructor copyctor;
						static FStringify stringify;
					};

					template<class T>
					struct TImpl<T, true>
					{
						static FDestructor dtor;
						static FStandardConstructor ctor;
						static FCopyConstructor copyctor;
						static FStringify stringify;
					};

					template<class T> FDestructor TImpl<T, false>::dtor = NULL;
					template<class T> FDestructor TImpl<T, true >::dtor = &generic_dtor<T>;

					template<class T> FStandardConstructor TImpl<T, false>::ctor = NULL;
					template<class T> FStandardConstructor TImpl<T, true >::ctor = &generic_ctor<T>;

					template<class T> FCopyConstructor TImpl<T, false>::copyctor = NULL;
					template<class T> FCopyConstructor TImpl<T, true >::copyctor = &generic_copyctor<T>;

					template<class T> FStringify TImpl<T, false>::stringify = NULL;
					template<class T> FStringify TImpl<T, true >::stringify = NULL;

					struct Yes { char value[1]; };
					struct No  { char value[sizeof(Yes)+1]; };

					template<class T>
					struct	TDetectStandardConstructor
					{
						template<class U> static Yes	test(char*[sizeof(new U())]) { return Yes(); }
						template<class U> static No		test(...) { return No(); }

						static const bool has_feature = sizeof(test<T>(0)) == sizeof(Yes);
					};

					template<class T>
					struct	TDetectCopyConstructor
					{
						template<class U> static Yes	test(char*[sizeof(new U(*reinterpret_cast<const U*>(NULL)))]) { return Yes(); }
						template<class U> static No		test(...) { return No(); }

						static const bool has_feature = sizeof(test<T>(0)) == sizeof(Yes);
					};

					template<class T>
					struct	TDetectDestructor
					{
						template<class U> static Yes	test(int) { delete reinterpret_cast<U*>(NULL); return Yes(); }
						template<class U> static No		test(...) { return No(); }

						static const bool has_feature = sizeof(test<T>(0)) == sizeof(Yes);
					};
				}

				struct	TRTTI
				{
					unsigned
						n_indirections : (sizeof(unsigned)*8-9),	//	number of indirections (references/pointers), specifies how often you would have to dereference to receive the actual value
						is_constant : 1,	//	constant?
						is_signed : 1,	//	signed numeric type? (NOTE: floats are also usually signed)
						is_pointer : 1,	//	pointer?
						is_reference : 1,	//	reference?
						is_array : 1,	//	standard c-style array?
						is_trivial_constructable : 1,	//	malloc() + memset() will do
						is_trivial_copyable : 1,		//	malloc() + memcpy() will do
						is_trivial_deleteable : 1,		//	free() will do
						is_trivial_moveable : 1;		//	memmove() will do (but you must no longer use the old object, only the new one)

					FDestructor dtor;	//	pointer to destructor function (if available, NULL otherwise)
					FStandardConstructor ctor;	//	pointer to standard constructor (if available, NULL otherwise)
					FCopyConstructor copyctor;	//	pointer to copy constructor (if available, NULL otherwise)
					FStringify stringify;	//	pointer to stringify function (always available - but might be less useful if not specialized)

					size_t sz_bytes;	//	size of the datatype in bytes
					const std::type_info* sys_type_info;	//	pointer to the type-info structure provided by C++ typeid()

					CL3PUBF	GETTER	io::text::string::TUStringUPtr Name() const;	//	platform independet human-readable type-name
					CL3PUBF	GETTER	u32 Hash() const;	//	32bit hashcode based on type-name
				};

				template<class T>
				struct	TCTTI
				{
					const static bool is_constant = false;
					const static bool is_signed = false;
					const static bool is_pointer = false;
					const static bool is_reference = false;
					const static bool is_array = false;
					const static bool is_trivial_constructable = false;
					const static bool is_trivial_copyable = false;
					const static bool is_trivial_deleteable = false;
					const static bool is_trivial_moveable = true;	//	NOTE: most classes are trivial moveable, this is why this is true by default
					const static unsigned n_indirections = 0;

					const static FDestructor dtor;
					const static FStandardConstructor ctor;
					const static FCopyConstructor copyctor;
					const static FStringify stringify;

					const static TRTTI rtti;
				};

				template<class T>
				struct	TCTTI<const T>
				{
					const static bool is_constant = true;
					const static bool is_signed = TCTTI<T>::is_signed;
					const static bool is_pointer = TCTTI<T>::is_pointer;
					const static bool is_reference = TCTTI<T>::is_reference;
					const static bool is_array = TCTTI<T>::is_array;
					const static bool is_trivial_constructable = TCTTI<T>::is_trivial_constructable;
					const static bool is_trivial_copyable = TCTTI<T>::is_trivial_copyable;
					const static bool is_trivial_deleteable = TCTTI<T>::is_trivial_deleteable;
					const static bool is_trivial_moveable = TCTTI<T>::is_trivial_moveable;
					const static unsigned n_indirections = TCTTI<T>::n_indirections;

					const static FDestructor dtor;
					const static FStandardConstructor ctor;
					const static FCopyConstructor copyctor;
					const static FStringify stringify;

					const static TRTTI rtti;
				};

				template<class T>
				struct	TCTTI<T*>
				{
					const static bool is_constant = TCTTI<T>::is_constant;
					const static bool is_signed = TCTTI<T>::is_signed;
					const static bool is_pointer = true;
					const static bool is_reference = TCTTI<T>::is_reference;
					const static bool is_array = TCTTI<T>::is_array;
					const static bool is_trivial_constructable = true;
					const static bool is_trivial_copyable = true;
					const static bool is_trivial_deleteable = true;
					const static bool is_trivial_moveable = true;
					const static unsigned n_indirections = TCTTI<T>::n_indirections + 1;

					const static FDestructor dtor;
					const static FStandardConstructor ctor;
					const static FCopyConstructor copyctor;
					const static FStringify stringify;

					const static TRTTI rtti;
				};

				template<class T>
				struct	TCTTI<T&>
				{
					const static bool is_constant = TCTTI<T>::is_constant;
					const static bool is_signed = TCTTI<T>::is_signed;
					const static bool is_pointer = TCTTI<T>::is_pointer;
					const static bool is_reference = true;
					const static bool is_array = TCTTI<T>::is_array;
					const static bool is_trivial_constructable = TCTTI<T>::is_trivial_constructable;
					const static bool is_trivial_copyable = TCTTI<T>::is_trivial_copyable;
					const static bool is_trivial_deleteable = TCTTI<T>::is_trivial_deleteable;
					const static bool is_trivial_moveable = TCTTI<T>::is_trivial_moveable;
					const static unsigned n_indirections = TCTTI<T>::n_indirections + 1;

					const static FDestructor dtor;
					const static FStandardConstructor ctor;
					const static FCopyConstructor copyctor;
					const static FStringify stringify;

					const static TRTTI rtti;
				};

				template<class T> const FDestructor				TCTTI<T>::dtor		= _::TImpl<T, _::TDetectDestructor<T>::has_feature>::dtor;
				template<class T> const FStandardConstructor	TCTTI<T>::ctor		= _::TImpl<T, _::TDetectStandardConstructor<T>::has_feature>::ctor;
				template<class T> const FCopyConstructor		TCTTI<T>::copyctor	= _::TImpl<T, _::TDetectCopyConstructor<T>::has_feature>::copyctor;
				template<class T> const FStringify				TCTTI<T>::stringify	= NULL;
				template<class T> const TRTTI					TCTTI<T>::rtti		= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, stringify, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<const T>::dtor		= _::TImpl<T, _::TDetectDestructor<T>::has_feature>::dtor;
				template<class T> const FStandardConstructor	TCTTI<const T>::ctor		= _::TImpl<T, _::TDetectStandardConstructor<T>::has_feature>::ctor;
				template<class T> const FCopyConstructor		TCTTI<const T>::copyctor	= _::TImpl<T, _::TDetectCopyConstructor<T>::has_feature>::copyctor;
				template<class T> const FStringify				TCTTI<const T>::stringify	= NULL;
				template<class T> const TRTTI					TCTTI<const T>::rtti		= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, stringify, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<T*>::dtor			= _::TImpl<T*, true>::dtor;
				template<class T> const FStandardConstructor	TCTTI<T*>::ctor			= _::TImpl<T*, true>::ctor;
				template<class T> const FCopyConstructor		TCTTI<T*>::copyctor		= _::TImpl<T*, true>::copyctor;
				template<class T> const FStringify				TCTTI<T*>::stringify	= NULL;
				template<class T> const TRTTI					TCTTI<T*>::rtti			= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, stringify, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<T&>::dtor			= NULL;
				template<class T> const FStandardConstructor	TCTTI<T&>::ctor			= NULL;
				template<class T> const FCopyConstructor		TCTTI<T&>::copyctor		= NULL;
				template<class T> const FStringify				TCTTI<T&>::stringify	= NULL;
				template<class T> const TRTTI TCTTI<T&>::rtti = { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, stringify, sizeof(T), &typeid(T) };
			}
		}
	}
}

#endif
