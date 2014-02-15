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

#ifndef	_include_cl3_core_system_types_typeinfo_h_
#define	_include_cl3_core_system_types_typeinfo_h_

#include <typeinfo>
#include "system_types.h"
#include "system_compiler.h"
#include "system_memory.h"

namespace	cl3
{
	namespace	system
	{
		namespace	types
		{
			namespace	typeinfo
			{
				typedef void (*FDestructor)(void*);
				template<class T> static void __generic_dtor(void* object) { reinterpret_cast<T*>(object)->~T(); }

				enum	ETypeClass
				{
					TYPE_CLASS_STRUCT,
					TYPE_CLASS_INTEGER,
					TYPE_CLASS_FLOAT
				};

				struct	TRTTI
				{
					ETypeClass typeclass;
					unsigned
						n_indirections : (sizeof(unsigned)*8-5),
						is_constant : 1,
						is_signed : 1,
						is_pointer : 1,
						is_reference : 1,
						is_array : 1,
						is_trivial_constructable : 1,
						is_trivial_copyable : 1,
						is_trivial_deleteable : 1,
						is_trivial_moveable : 1;
					FDestructor dtor;
					size_t sz_bytes;
					const std::type_info* sys_type_info;

					CL3PUBF	system::memory::TUniquePtr<char[],false> Name() const;
					CL3PUBF	u32 Hash() const;
				};

				template<class T>
				struct	CL3PUBT	TCTTI
				{
					const static ETypeClass typeclass = TYPE_CLASS_STRUCT;
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
					const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<const T>
				{
					const static ETypeClass typeclass = TCTTI<T>::typeclass;
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
					const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<T*>
				{
					const static ETypeClass typeclass = TCTTI<T>::typeclass;
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
					const static TRTTI rtti;
				};

				template<class T>
				struct	CL3PUBT	TCTTI<T&>
				{
					const static ETypeClass typeclass = TCTTI<T>::typeclass;
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
					const static TRTTI rtti;
				};

				template<class T> const FDestructor TCTTI<T>::dtor = &__generic_dtor<T>;
				template<class T> const TRTTI TCTTI<T>::rtti = { typeclass, n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, sizeof(T), &typeid(T) };
				template<class T> const FDestructor TCTTI<const T>::dtor = &__generic_dtor<T>;
				template<class T> const TRTTI TCTTI<const T>::rtti = { typeclass, n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, sizeof(T), &typeid(T) };
				template<class T> const FDestructor TCTTI<T*>::dtor = &__generic_dtor<T>;
				template<class T> const TRTTI TCTTI<T*>::rtti = { typeclass, n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, sizeof(T), &typeid(T) };
				template<class T> const FDestructor TCTTI<T&>::dtor = &__generic_dtor<T>;
				template<class T> const TRTTI TCTTI<T&>::rtti = { typeclass, n_indirections, is_constant, is_signed, is_pointer, is_reference, is_array, is_trivial_constructable, is_trivial_copyable, is_trivial_deleteable, is_trivial_moveable, dtor, sizeof(T), &typeid(T) };
			}
		}
	}
}

#endif
