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
#include <type_traits>
#include "system_types.hpp"
#include "system_compiler.hpp"
#include "system_memory.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			class	ITextWriter;
			class	ITextReader;

			//	converts object value to human-readable string
			typedef	void (*FPrint)(ITextWriter&, const void*);

			namespace	string
			{
				class TString;
			}
		}

		namespace	serialization
		{
			class	ISerializer;
			class	IDeserializer;
			class	ISerializable;

			class	IArraySerializer;
			class	IArrayDeserializer;
			class	IArraySerializable;
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
				typedef void (*FDeserialize)(io::serialization::IDeserializer&, void*);
				typedef void (*FCopyConstructor)(void*, const void*);

				//	serializes/deserializes an object to/from a stream
				typedef void (*FSerialize)(io::serialization::ISerializer&, const void*);
				typedef void (*FDeserialize)(io::serialization::IDeserializer&, void*);

				namespace	_
				{
					static void* const DUMMY = (void*)0x1;

					template<class T> CL3PUBF void generic_dtor(void* object) { reinterpret_cast<T*>(object)->~T(); }
					template<class T> CL3PUBF void generic_stdctor(void* object) { new (object) T(); }
					template<class T> CL3PUBF void generic_copyctor(void* object, const void* ref) { new (object) T(*reinterpret_cast<const T*>(ref)); }

					template<class T> CL3PUBF void generic_print(io::text::ITextWriter& w, const void* object) { w<<(*reinterpret_cast<const T*>(object)); }

					template<class T> CL3PUBF void generic_serialize(io::serialization::ISerializer& s, const void* object) { reinterpret_cast<const T*>(object)->Serialize(s); }
					template<class T> CL3PUBF void generic_deserctor(io::serialization::IDeserializer& ds, void* object) { new (object) T(ds); }
					template<class T> CL3PUBF void generic_deserinst(io::serialization::IDeserializer& ds, void* object) { reinterpret_cast<T*>(object)->Deserialize(ds); }


					template<class T, bool>
					struct	CL3PUBT	TImpl;

					template<class T>
					struct CL3PUBT	TImpl<T, false>
					{
						CL3PUBF	const static FDestructor dtor;
						CL3PUBF	const static FStandardConstructor ctor;
						CL3PUBF	const static FCopyConstructor copyctor;
						CL3PUBF	const static io::text::FPrint print;
						CL3PUBF	const static FSerialize serialize;
						CL3PUBF	const static FDeserialize deserctor;
						CL3PUBF	const static FDeserialize deserinst;
					};

					template<class T>
					struct CL3PUBT	TImpl<T, true>
					{
						CL3PUBF	const static FDestructor dtor;
						CL3PUBF	const static FStandardConstructor ctor;
						CL3PUBF	const static FCopyConstructor copyctor;
						CL3PUBF	const static io::text::FPrint print;
						CL3PUBF	const static FSerialize serialize;
						CL3PUBF	const static FDeserialize deserctor;
						CL3PUBF	const static FDeserialize deserinst;
					};

					template<class T> const FDestructor TImpl<T, false>::dtor = NULL;
					template<class T> const FDestructor TImpl<T, true >::dtor = &generic_dtor<T>;

					template<class T> const FStandardConstructor TImpl<T, false>::ctor = NULL;
					template<class T> const FStandardConstructor TImpl<T, true >::ctor = &generic_stdctor<T>;

					template<class T> const FCopyConstructor TImpl<T, false>::copyctor = NULL;
					template<class T> const FCopyConstructor TImpl<T, true >::copyctor = &generic_copyctor<T>;

					template<class T> const io::text::FPrint TImpl<T, false>::print = NULL;
					template<class T> const io::text::FPrint TImpl<T, true >::print = &generic_print<T>;

					template<class T> const FSerialize TImpl<T, false>::serialize = NULL;
					template<class T> const FSerialize TImpl<T, true >::serialize = &generic_serialize<T>;

					template<class T> const FDeserialize TImpl<T, false>::deserctor = NULL;
					template<class T> const FDeserialize TImpl<T, true >::deserctor = &generic_deserctor<T>;

					template<class T> const FDeserialize TImpl<T, false>::deserinst = NULL;
					template<class T> const FDeserialize TImpl<T, true >::deserinst = &generic_deserinst<T>;

					template<typename T, bool>
					struct CL3PUBT	is_boolean_impl;

					template<typename T>
					struct	is_boolean_impl<T,true>
					{
						static const bool value = ((T)2) == ((T)1);
					};

					template<typename T>
					struct	is_boolean_impl<T,false>
					{
						static const bool value = false;
					};
				}

				namespace	features
				{
					template<class T> struct remove_ref      { typedef T type; };
					template<class T> struct remove_ref<T&>  { typedef T type; };
					template<class T> struct remove_ref<T&&> { typedef T type; };
					template<class T> typename remove_ref<T>::type&& move(T&& value) { return static_cast<typename remove_ref<T>::type&&>(value); }

					template<class T> constexpr T& forward( typename remove_ref<T>::type& t ) noexcept { return static_cast<T&>(t); }
					template<class T> constexpr T&& forward( typename remove_ref<T>::type&& t ) noexcept { return static_cast<T&&>(t); }


					template<class T> struct remove_const             { typedef T type; };
					template<class T> struct remove_const<const T>    { typedef T type; };

					template<bool, typename T1, typename T2>
					struct type_iif
					{
						using type = T2;
					};

					template<typename T1, typename T2>
					struct type_iif<true, T1, T2>
					{
						using type = T1;
					};

					template<bool, typename T>
					struct ref_if
					{
						using type = T;
					};

					template<typename T>
					struct ref_if<true, T>
					{
						using type = const T&;
					};

					template<bool b, typename T>
					struct inherit_if;

					template<typename T>
					struct inherit_if<false, T> {};

					template<typename T>
					struct inherit_if<true, T> : T {};

					struct yes { char value[1]; };
					struct no  { char value[sizeof(yes)+1]; };

					template<typename T>
					class is_class
					{
						private:
							//	test if we can define a member-function pointer
							template<typename U>
							static decltype( ((((U*)NULL)->*((void (U::*)())NULL))()), yes() ) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_class<T>::value;

					template<typename T>
					class is_default_constructible
					{
						private:
							template<typename U>
							static decltype(new U(), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_default_constructible<T>::value;

					template<typename T>
					class is_copy_constructible
					{
						private:
							template<typename U>
							static decltype(new U(*reinterpret_cast<const U*>(_::DUMMY)), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_copy_constructible<T>::value;

					template<typename T>
					class is_move_constructible
					{
					private:
						template<typename U>
						static decltype(new U(system::def::move(*reinterpret_cast<U*>(_::DUMMY))), yes()) test(int);

						template<typename>
						static no test(...);

					public:
						static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_move_constructible<T>::value;

					template<typename T>
					class is_destructible
					{
						private:
							template<typename U>
							static decltype((reinterpret_cast<U*>(_::DUMMY)->~U()), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_destructible<T>::value;

					template<typename T>
					class is_serializable
					{
						private:
							template<typename U>
							static decltype((reinterpret_cast<const U*>(_::DUMMY)->Serialize(*reinterpret_cast<io::serialization::ISerializer*>(_::DUMMY))), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_serializable<T>::value;

					template<typename T>
					class is_deserializable_instance
					{
						private:
							template<typename U>
							static decltype((reinterpret_cast<U*>(_::DUMMY)->Deserialize(*reinterpret_cast<io::serialization::IDeserializer*>(_::DUMMY))), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_deserializable_instance<T>::value;

					template<typename T>
					class is_deserializable_ctor
					{
						private:
							template<typename U>
							static decltype(U(*reinterpret_cast<io::serialization::IDeserializer*>(_::DUMMY)), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_deserializable_ctor<T>::value;

					template<typename T>
					class is_printable
					{
						private:
							template<typename U>
							static decltype( ((*reinterpret_cast<io::text::ITextWriter*>(_::DUMMY))<<(*reinterpret_cast<const U*>(_::DUMMY))), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_printable<T>::value;

					template<typename T>
					class is_comparable_lessthan
					{
						private:
							template<typename U>
							static decltype(( (*reinterpret_cast<const U*>(_::DUMMY)) < (*reinterpret_cast<const U*>(_::DUMMY)) ? 1 : 2 ), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_comparable_lessthan<T>::value;

					template<typename T>
					class is_comparable_biggerthan
					{
						private:
							template<typename U>
							static decltype(( (*reinterpret_cast<const U*>(_::DUMMY)) > (*reinterpret_cast<const U*>(_::DUMMY)) ? 1 : 2 ), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_comparable_biggerthan<T>::value;

					template<typename T>
					class is_comparable_equal
					{
						private:
							template<typename U>
							static decltype(( (*reinterpret_cast<const U*>(_::DUMMY)) == (*reinterpret_cast<const U*>(_::DUMMY)) ? 1 : 2 ), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_comparable_equal<T>::value;

					template<typename T>
					class is_comparable
					{
						public:
							static const bool value = is_comparable_biggerthan<T>::value && is_comparable_lessthan<T>::value && is_comparable_equal<T>::value;
					};

					template<typename T>
					const bool is_comparable<T>::value;

					template<typename T>
					class is_assignable
					{
						private:
							template<typename U>
							static decltype(( (*reinterpret_cast<U*>(_::DUMMY)) = (*reinterpret_cast<const U*>(_::DUMMY)) ), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					const bool is_assignable<T>::value;

					template<typename T>
					class is_boolean
					{
						public:
							static const bool value = _::is_boolean_impl<T, std::is_integral<T>::value && is_comparable_equal<T>::value>::value;
					};

					template<typename T>
					const bool is_boolean<T>::value;
				}

				struct	CL3PUBT	TRTTI
				{
					unsigned
						n_indirections : (sizeof(unsigned)*8-12),	//	number of indirections (references/pointers), specifies how often you would have to dereference to receive the actual value
						is_constant : 1,	//	constant?
						is_signed : 1,		//	signed numeric type? (NOTE: floats are usually signed)
						is_pointer : 1,		//	pointer?
						is_reference : 1,	//	reference?
						is_array : 1,		//	standard c-style array?
						is_integer : 1,		//	regular integer?
						is_float : 1,		//	is float?
						is_bool : 1,		//	is boolean?
						is_trivial_constructable : 1,	//	malloc() + memset() will do
						is_trivial_copyable : 1,		//	malloc() + memcpy() will do
						is_trivial_deleteable : 1,		//	free() will do
						is_trivial_moveable : 1;		//	memmove() will do (but you must no longer use the old object, only the new one)

					FDestructor dtor;	//	pointer to destructor function (if available, NULL otherwise)
					FStandardConstructor ctor;	//	pointer to standard constructor (if available, NULL otherwise)
					FCopyConstructor copyctor;	//	pointer to copy constructor (if available, NULL otherwise)
					io::text::FPrint print;	//	pointer to print function (always available - but might be less useful if not specialized)
					FSerialize serialize;	//	serializes an instance to a stream
					FDeserialize deserialize_instance;	//	overwrites an existing instance with deserialized data
					FDeserialize deserialize_ctor;	//	constructs a new instance from a stream by deserialization

					usys_t sz_bytes;	//	size of the datatype in byte_ts
					const std::type_info* sys_type_info;	//	pointer to the type-info structure provided by C++ typeid()

					CL3PUBF	CL3_GETTER	system::memory::TUniquePtr<io::text::string::TString> Name() const;	//	platform independet human-readable type-name
					CL3PUBF	CL3_GETTER	u32_t Hash() const;	//	32bit hashcode based on type-name
				};

				template<class T>
				struct	CL3PUBT	TCTTI
				{
					CL3PUBF	const static bool is_constant = false;
					CL3PUBF	const static bool is_signed = false;
					CL3PUBF	const static bool is_pointer = false;
					CL3PUBF	const static bool is_reference = false;
					CL3PUBF	const static bool is_array = false;
					CL3PUBF	const static bool is_integer = std::is_integral<T>::value;
					CL3PUBF	const static bool is_float = std::is_floating_point<T>::value;
					CL3PUBF	const static bool is_bool = features::is_boolean<T>::value;
					CL3PUBF	const static bool is_trivial_constructable = false;
					CL3PUBF	const static bool is_trivial_copyable = false;
					CL3PUBF	const static bool is_trivial_deleteable = false;
					CL3PUBF	const static bool is_trivial_moveable = true;	//	NOTE: most classes are trivial moveable, this is why this is true by default
					CL3PUBF	const static unsigned n_indirections = 0;

					CL3PUBF	const static FDestructor dtor;
					CL3PUBF	const static FStandardConstructor ctor;
					CL3PUBF	const static FCopyConstructor copyctor;
					CL3PUBF	const static io::text::FPrint print;
					CL3PUBF	const static FSerialize serialize;
					CL3PUBF	const static FDeserialize deserialize_instance;
					CL3PUBF	const static FDeserialize deserialize_ctor;

					CL3PUBF	const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<const T>
				{
					CL3PUBF	const static bool is_constant = true;
					CL3PUBF	const static bool is_signed = TCTTI<T>::is_signed;
					CL3PUBF	const static bool is_pointer = TCTTI<T>::is_pointer;
					CL3PUBF	const static bool is_reference = TCTTI<T>::is_reference;
					CL3PUBF	const static bool is_array = TCTTI<T>::is_array;
					CL3PUBF	const static bool is_integer = TCTTI<T>::is_integer;
					CL3PUBF	const static bool is_float = TCTTI<T>::is_float;
					CL3PUBF	const static bool is_bool = TCTTI<T>::is_bool;
					CL3PUBF	const static bool is_trivial_constructable = TCTTI<T>::is_trivial_constructable;
					CL3PUBF	const static bool is_trivial_copyable = TCTTI<T>::is_trivial_copyable;
					CL3PUBF	const static bool is_trivial_deleteable = TCTTI<T>::is_trivial_deleteable;
					CL3PUBF	const static bool is_trivial_moveable = TCTTI<T>::is_trivial_moveable;
					CL3PUBF	const static unsigned n_indirections = TCTTI<T>::n_indirections;

					CL3PUBF	const static FDestructor dtor;
					CL3PUBF	const static FStandardConstructor ctor;
					CL3PUBF	const static FCopyConstructor copyctor;
					CL3PUBF	const static io::text::FPrint print;
					CL3PUBF	const static FSerialize serialize;
					CL3PUBF	const static FDeserialize deserialize_instance;
					CL3PUBF	const static FDeserialize deserialize_ctor;

					CL3PUBF	const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<T*>
				{
					CL3PUBF	const static bool is_constant = TCTTI<T>::is_constant;
					CL3PUBF	const static bool is_signed = TCTTI<T>::is_signed;
					CL3PUBF	const static bool is_pointer = true;
					CL3PUBF	const static bool is_reference = TCTTI<T>::is_reference;
					CL3PUBF	const static bool is_array = TCTTI<T>::is_array;
					CL3PUBF	const static bool is_integer = TCTTI<T>::is_integer;
					CL3PUBF	const static bool is_float = TCTTI<T>::is_float;
					CL3PUBF	const static bool is_bool = TCTTI<T>::is_bool;
					CL3PUBF	const static bool is_trivial_constructable = true;
					CL3PUBF	const static bool is_trivial_copyable = true;
					CL3PUBF	const static bool is_trivial_deleteable = true;
					CL3PUBF	const static bool is_trivial_moveable = true;
					CL3PUBF	const static unsigned n_indirections = TCTTI<T>::n_indirections + 1;

					CL3PUBF	const static FDestructor dtor;
					CL3PUBF	const static FStandardConstructor ctor;
					CL3PUBF	const static FCopyConstructor copyctor;
					CL3PUBF	const static io::text::FPrint print;
					CL3PUBF	const static FSerialize serialize;
					CL3PUBF	const static FDeserialize deserialize_instance;
					CL3PUBF	const static FDeserialize deserialize_ctor;

					CL3PUBF	const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<T&>
				{
					CL3PUBF	const static bool is_constant = TCTTI<T>::is_constant;
					CL3PUBF	const static bool is_signed = TCTTI<T>::is_signed;
					CL3PUBF	const static bool is_pointer = TCTTI<T>::is_pointer;
					CL3PUBF	const static bool is_reference = true;
					CL3PUBF	const static bool is_array = TCTTI<T>::is_array;
					CL3PUBF	const static bool is_integer = TCTTI<T>::is_integer;
					CL3PUBF	const static bool is_float = TCTTI<T>::is_float;
					CL3PUBF	const static bool is_bool = TCTTI<T>::is_bool;
					CL3PUBF	const static bool is_trivial_constructable = TCTTI<T>::is_trivial_constructable;
					CL3PUBF	const static bool is_trivial_copyable = TCTTI<T>::is_trivial_copyable;
					CL3PUBF	const static bool is_trivial_deleteable = TCTTI<T>::is_trivial_deleteable;
					CL3PUBF	const static bool is_trivial_moveable = TCTTI<T>::is_trivial_moveable;
					CL3PUBF	const static unsigned n_indirections = TCTTI<T>::n_indirections + 1;

					CL3PUBF	const static FDestructor dtor;
					CL3PUBF	const static FStandardConstructor ctor;
					CL3PUBF	const static FCopyConstructor copyctor;
					CL3PUBF	const static io::text::FPrint print;
					CL3PUBF	const static FSerialize serialize;
					CL3PUBF	const static FDeserialize deserialize_instance;
					CL3PUBF	const static FDeserialize deserialize_ctor;

					CL3PUBF	const static TRTTI rtti;
				};

				template<class T> const FDestructor				TCTTI<T>::dtor					= _::TImpl<T, features::is_destructible<T>::value>::dtor;
				template<class T> const FStandardConstructor	TCTTI<T>::ctor					= _::TImpl<T, features::is_default_constructible<T>::value>::ctor;
				template<class T> const FCopyConstructor		TCTTI<T>::copyctor				= _::TImpl<T, features::is_copy_constructible<T>::value>::copyctor;
				template<class T> const io::text::FPrint		TCTTI<T>::print					= _::TImpl<T, features::is_printable<T>::value>::print;
				template<class T> const FSerialize				TCTTI<T>::serialize				= _::TImpl<T, features::is_serializable<T>::value>::serialize;
				template<class T> const FDeserialize			TCTTI<T>::deserialize_instance	= _::TImpl<T, features::is_deserializable_instance<T>::value>::deserinst;
				template<class T> const FDeserialize			TCTTI<T>::deserialize_ctor		= _::TImpl<T, features::is_deserializable_ctor<T>::value>::deserctor;
				template<class T> const TRTTI					TCTTI<T>::rtti		= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_integer, is_float, is_bool, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<const T>::dtor					= _::TImpl<T, features::is_destructible<T>::value>::dtor;
				template<class T> const FStandardConstructor	TCTTI<const T>::ctor					= _::TImpl<T, features::is_default_constructible<T>::value>::ctor;
				template<class T> const FCopyConstructor		TCTTI<const T>::copyctor				= _::TImpl<T, features::is_copy_constructible<T>::value>::copyctor;
				template<class T> const io::text::FPrint		TCTTI<const T>::print					= _::TImpl<T, features::is_printable<T>::value>::print;
				template<class T> const FSerialize				TCTTI<const T>::serialize				= _::TImpl<T, features::is_serializable<T>::value>::serialize;
				template<class T> const FDeserialize			TCTTI<const T>::deserialize_instance	= NULL;
				template<class T> const FDeserialize			TCTTI<const T>::deserialize_ctor		= _::TImpl<T, features::is_deserializable_ctor<T>::value>::deserctor;
				template<class T> const TRTTI					TCTTI<const T>::rtti		= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_integer, is_float, is_bool, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<T*>::dtor					= NULL;
				template<class T> const FStandardConstructor	TCTTI<T*>::ctor					= NULL;
				template<class T> const FCopyConstructor		TCTTI<T*>::copyctor				= NULL;
				template<class T> const io::text::FPrint		TCTTI<T*>::print				= _::TImpl<T*, features::is_printable<T*>::value>::print;
				template<class T> const FSerialize				TCTTI<T*>::serialize			= NULL;
				template<class T> const FDeserialize			TCTTI<T*>::deserialize_instance	= NULL;
				template<class T> const FDeserialize			TCTTI<T*>::deserialize_ctor		= NULL;
				template<class T> const TRTTI					TCTTI<T*>::rtti			= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_integer, is_float, is_bool, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<T&>::dtor					= NULL;
				template<class T> const FStandardConstructor	TCTTI<T&>::ctor					= NULL;
				template<class T> const FCopyConstructor		TCTTI<T&>::copyctor				= NULL;
				template<class T> const io::text::FPrint		TCTTI<T&>::print				= _::TImpl<T&, features::is_printable<T&>::value>::print;
				template<class T> const FSerialize				TCTTI<T&>::serialize			= NULL;
				template<class T> const FDeserialize			TCTTI<T&>::deserialize_instance	= NULL;
				template<class T> const FDeserialize			TCTTI<T&>::deserialize_ctor		= NULL;
				template<class T> const TRTTI TCTTI<T&>::rtti = { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_integer, is_float, is_bool, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };
			}
		}
	}
}

#endif
