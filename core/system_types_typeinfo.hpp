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

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			struct	ITextWriter;
			struct	ITextReader;

			//	converts object value to human-readable string
			typedef	void (*FPrint)(ITextWriter&, const void*);

			namespace	string
			{
				class	TUString;
				typedef	system::memory::TUniquePtr<TUString>	TUStringUPtr;

				CL3PUBF	TUStringUPtr	Stringify	(FPrint print, const void* object);

				template<class T>
				TUStringUPtr			Stringify	(const T& object);
			}
		}

		namespace	serialization
		{
			struct	ISerializer;
			struct	IDeserializer;
			struct	ISerializable;
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
					template<class T> void generic_dtor(void* object) { reinterpret_cast<T*>(object)->~T(); }
					template<class T> void generic_stdctor(void* object) { new (object) T(); }
					template<class T> void generic_copyctor(void* object, const void* ref) { new (object) T(*reinterpret_cast<const T*>(ref)); }

					template<class T> void generic_print(io::text::ITextWriter& w, const void* object) { w<<(*reinterpret_cast<const T*>(object)); }

					template<class T> void generic_serialize(io::serialization::ISerializer& s, void* object) { reinterpret_cast<const T*>(object)->Serialize(s); }
					template<class T> void generic_deserctor(io::serialization::IDeserializer& ds, void* object) { new (object) T(ds); }
					template<class T> void generic_deserinst(io::serialization::IDeserializer& ds, void* object) { reinterpret_cast<T*>(object)->Deserialize(ds); }


					template<class T, bool>
					struct	TImpl;

					template<class T>
					struct TImpl<T, false>
					{
						static FDestructor dtor;
						static FStandardConstructor ctor;
						static FCopyConstructor copyctor;
						static io::text::FPrint print;
						static FSerialize serialize;
						static FDeserialize deserctor;
						static FDeserialize deserinst;
					};

					template<class T>
					struct TImpl<T, true>
					{
						static FDestructor dtor;
						static FStandardConstructor ctor;
						static FCopyConstructor copyctor;
						static io::text::FPrint print;
						static FSerialize serialize;
						static FDeserialize deserctor;
						static FDeserialize deserinst;
					};

					template<class T> FDestructor TImpl<T, false>::dtor = NULL;
					template<class T> FDestructor TImpl<T, true >::dtor = &generic_dtor<T>;

					template<class T> FStandardConstructor TImpl<T, false>::ctor = NULL;
					template<class T> FStandardConstructor TImpl<T, true >::ctor = &generic_stdctor<T>;

					template<class T> FCopyConstructor TImpl<T, false>::copyctor = NULL;
					template<class T> FCopyConstructor TImpl<T, true >::copyctor = &generic_copyctor<T>;

					template<class T> io::text::FPrint TImpl<T, false>::print = NULL;
					template<class T> io::text::FPrint TImpl<T, true >::print = &generic_print<T>;

					template<class T> FSerialize TImpl<T, false>::serialize = NULL;
					template<class T> FSerialize TImpl<T, true >::serialize = &generic_serialize<T>;

					template<class T> FDeserialize TImpl<T, false>::deserctor = NULL;
					template<class T> FDeserialize TImpl<T, true >::deserctor = &generic_deserctor<T>;

					template<class T> FDeserialize TImpl<T, false>::deserinst = NULL;
					template<class T> FDeserialize TImpl<T, true >::deserinst = &generic_deserinst<T>;

					struct yes { char value[1]; };
					struct no  { char value[sizeof(yes)+1]; };

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
					class is_copy_constructible
					{
						private:
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
							template<typename U>
							static decltype((delete reinterpret_cast<U*>(NULL)), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					class is_serializable
					{
						private:
							template<typename U>
							static decltype((reinterpret_cast<const U*>(NULL)->Serialize(*reinterpret_cast<io::serialization::ISerializer*>(NULL))), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					class is_deserializable_instance
					{
						private:
							template<typename U>
							static decltype((reinterpret_cast<U*>(NULL)->Deserialize(*reinterpret_cast<io::serialization::IDeserializer*>(NULL))), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					class is_deserializable_ctor
					{
						private:
							template<typename U>
							static decltype(U(*reinterpret_cast<io::serialization::IDeserializer*>(NULL)), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};

					template<typename T>
					class is_printable
					{
						private:
							template<typename U>
							static decltype( ((*reinterpret_cast<io::text::ITextWriter*>(NULL))<<(*reinterpret_cast<const U*>(NULL))), yes()) test(int);

							template<typename>
							static no test(...);

						public:
							static const bool value = sizeof(test<T>(0)) == sizeof(yes);
					};
				}

				struct	TRTTI
				{
					unsigned
						n_indirections : (sizeof(unsigned)*8-9),	//	number of indirections (references/pointers), specifies how often you would have to dereference to receive the actual value
						is_constant : 1,	//	constant?
						is_signed : 1,	//	signed numeric type? (NOTE: floats are usually signed)
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
					io::text::FPrint print;	//	pointer to print function (always available - but might be less useful if not specialized)
					FSerialize serialize;	//	serializes an instance to a stream
					FDeserialize deserialize_instance;	//	overwrites an existing instance with deserialized data
					FDeserialize deserialize_ctor;	//	constructs a new instance from a stream by deserialization

					usys_t sz_bytes;	//	size of the datatype in byte_ts
					const std::type_info* sys_type_info;	//	pointer to the type-info structure provided by C++ typeid()

					CL3PUBF	GETTER	io::text::string::TUStringUPtr Name() const;	//	platform independet human-readable type-name
					CL3PUBF	GETTER	u32_t Hash() const;	//	32bit hashcode based on type-name
				};

				template<class T>
				struct	CL3PUBT	TCTTI
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
					const static io::text::FPrint print;
					const static FSerialize serialize;
					const static FDeserialize deserialize_instance;
					const static FDeserialize deserialize_ctor;

					const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<const T>
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
					const static io::text::FPrint print;
					const static FSerialize serialize;
					const static FDeserialize deserialize_instance;
					const static FDeserialize deserialize_ctor;

					const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<T*>
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
					const static io::text::FPrint print;
					const static FSerialize serialize;
					const static FDeserialize deserialize_instance;
					const static FDeserialize deserialize_ctor;

					const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<T&>
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
					const static io::text::FPrint print;
					const static FSerialize serialize;
					const static FDeserialize deserialize_instance;
					const static FDeserialize deserialize_ctor;

					const static TRTTI rtti;
				};

				template<class T> const FDestructor				TCTTI<T>::dtor					= _::TImpl<T, _::is_destructible<T>::value>::dtor;
				template<class T> const FStandardConstructor	TCTTI<T>::ctor					= _::TImpl<T, _::is_default_constructible<T>::value>::ctor;
				template<class T> const FCopyConstructor		TCTTI<T>::copyctor				= _::TImpl<T, _::is_copy_constructible<T>::value>::copyctor;
				template<class T> const io::text::FPrint		TCTTI<T>::print					= _::TImpl<T, _::is_printable<T>::value>::print;
				template<class T> const FSerialize				TCTTI<T>::serialize				= _::TImpl<T, _::is_serializable<T>::value>::serialize;
				template<class T> const FDeserialize			TCTTI<T>::deserialize_instance	= _::TImpl<T, _::is_deserializable_instance<T>::value>::deserinst;
				template<class T> const FDeserialize			TCTTI<T>::deserialize_ctor		= _::TImpl<T, _::is_deserializable_ctor<T>::value>::deserctor;
				template<class T> const TRTTI					TCTTI<T>::rtti		= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<const T>::dtor					= _::TImpl<T, _::is_destructible<T>::value>::dtor;
				template<class T> const FStandardConstructor	TCTTI<const T>::ctor					= _::TImpl<T, _::is_default_constructible<T>::value>::ctor;
				template<class T> const FCopyConstructor		TCTTI<const T>::copyctor				= _::TImpl<T, _::is_copy_constructible<T>::value>::copyctor;
				template<class T> const io::text::FPrint		TCTTI<const T>::print					= _::TImpl<T, _::is_printable<T>::value>::print;
				template<class T> const FSerialize				TCTTI<const T>::serialize				= _::TImpl<T, _::is_serializable<T>::value>::serialize;
				template<class T> const FDeserialize			TCTTI<const T>::deserialize_instance	= NULL;
				template<class T> const FDeserialize			TCTTI<const T>::deserialize_ctor		= _::TImpl<T, _::is_deserializable_ctor<T>::value>::deserctor;
				template<class T> const TRTTI					TCTTI<const T>::rtti		= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<T*>::dtor					= NULL;
				template<class T> const FStandardConstructor	TCTTI<T*>::ctor					= NULL;
				template<class T> const FCopyConstructor		TCTTI<T*>::copyctor				= NULL;
				template<class T> const io::text::FPrint		TCTTI<T*>::print				= _::TImpl<T*, _::is_printable<T*>::value>::print;
				template<class T> const FSerialize				TCTTI<T*>::serialize			= NULL;
				template<class T> const FDeserialize			TCTTI<T*>::deserialize_instance	= NULL;
				template<class T> const FDeserialize			TCTTI<T*>::deserialize_ctor		= NULL;
				template<class T> const TRTTI					TCTTI<T*>::rtti			= { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };

				template<class T> const FDestructor				TCTTI<T&>::dtor					= NULL;
				template<class T> const FStandardConstructor	TCTTI<T&>::ctor					= NULL;
				template<class T> const FCopyConstructor		TCTTI<T&>::copyctor				= NULL;
				template<class T> const io::text::FPrint		TCTTI<T&>::print				= _::TImpl<T&, _::is_printable<T&>::value>::print;
				template<class T> const FSerialize				TCTTI<T&>::serialize			= NULL;
				template<class T> const FDeserialize			TCTTI<T&>::deserialize_instance	= NULL;
				template<class T> const FDeserialize			TCTTI<T&>::deserialize_ctor		= NULL;
				template<class T> const TRTTI TCTTI<T&>::rtti = { n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, ctor, copyctor, print, serialize, deserialize_instance, deserialize_ctor, sizeof(T), &typeid(T) };
			}
		}
	}

	namespace	io
	{
		namespace	text
		{
			namespace	string
			{
				template<class T>
				TUStringUPtr	Stringify	(const T& object)
				{
					return Stringify(system::types::typeinfo::TCTTI<T>::print, &object);
				}
			}
		}
	}

	namespace	system
	{
		namespace	memory
		{
			template<class T>	T*	Alloc	(usys_t n_items)				{ return reinterpret_cast<T*>(Alloc(n_items, &typeinfo::TCTTI<T>::rtti)); }
			template<class T>	T*	Realloc	(T* p_mem, usys_t n_items_new)	{ return reinterpret_cast<T*>(Realloc(p_mem, n_items_new, &typeinfo::TCTTI<T>::rtti)); }
		}
	}
}

#endif
