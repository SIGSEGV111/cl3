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

#pragma once

#include "system_endian.hpp"
#include "system_time.hpp"
#include "system_memory.hpp"
#include "io_serialization.hpp"
#include "io_collection_list.hpp"
#include "io_file.hpp"

namespace cl3
{
	namespace atom
	{
		using namespace system::types;

		class IAtom;

		/*********************************************************************/

		union atom_id_t
		{
			struct
			{
				#ifdef CL3_IS_LITTLE_ENDIAN
					u64_t flag : 1;
					u64_t id : 63;
				#else
					u64_t id : 63;
					u64_t flag : 1;
				#endif
			};
			u64_t u64;

			void Serialize(io::serialization::ISerializer& s) const
			{
				s.Push("id", this->u64);
			}

			void Deserialize(io::serialization::IDeserializer& ds)
			{
				ds.Pop("id", this->u64);
			}

			CLASS atom_id_t(u64_t v) : u64(v) {}

			#ifdef CL3_IS_LITTLE_ENDIAN
				CLASS atom_id_t(u64_t id, bool flag) : flag(flag ? 1 : 0), id(id) {}
			#else
				CLASS atom_id_t(u64_t id, bool flag) : id(id), flag(flag ? 1 : 0) {}
			#endif
		};

		/*********************************************************************/

		struct IManager
		{
			//	a manager must be multi-thread safe
			virtual IAtom* Load(atom_id_t, bool fork = false) = 0;	//	loads the atom referenced by ID; if it is already loaded it returns the ptr to the loaded instance; if fork is set to true, no overriding is performed and a *COPY* (instead of a new version) of the original atom is returned
			virtual void Save(const IAtom*) = 0;		//	saves the atom to the store and assigns a new ID
			virtual void New(IAtom*) = 0;				//	registers the specified atom with this manager and assigns an ID
			virtual void Delete(atom_id_t) = 0;			//	deletes the atom from the store; can be a NO-OP
			virtual atom_id_t ParentOf(atom_id_t) = 0;	//	returns the parent atom of the specified atom - if any
			virtual system::time::TTime BirthdateOf(atom_id_t) = 0;	//	return the time the atom was saved to the store, if the atom if currently in memory it returns TTime::TTime(0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF)
// 			virtual void Unload(IAtom*) = 0;			//	deletes the specified atom from memory (calls operator delete())	//	FIXME: that should be handled by the memory management (ref-counting)
		};

		/*********************************************************************/

		class CL3PUBT TManager : public IManager
		{
			protected:
				io::file::TFile* index_file;
				io::collection::list::TList<io::file::TFile*> data_files;

				struct TIndexEntry
				{
					atom_id_t id;			//	id of the atom
					atom_id_t idx_override;	//	flag == 0: index of the next version of the atom; flag == 1: ID of the next version (in memory)
					atom_id_t idx_parent;	//	index of the parent atom (flag == 0 means copy/fork; flag == 1 means override)
					u64_t offset_data;		//	file-offset of the serialized data for this atom
				};

				io::file::TMapping map_index;	//	maps atom-ids to file-offsets; ordered asc by ID
				io::collection::list::TList<IAtom*> loaded_atoms;	//	list of atoms currently loaded in memory; ordered asc by ID

				u64_t next_id;	//	used to generate new atom-ids

			public:
				CL3PUBF IAtom* Load(atom_id_t, bool fork = false) final override;
				CL3PUBF void Save(const IAtom*) final override;
				CL3PUBF void New(IAtom*) final override;
				CL3PUBF void Delete(atom_id_t) final override;
				CL3PUBF atom_id_t ParentOf(atom_id_t) final override;
				CL3PUBF system::time::TTime BirthdateOf(atom_id_t) final override;

				CL3PUBF CLASS  TManager(const io::collection::IStaticCollection<io::file::TFile*>& data_files, io::file::TFile* index_file);
				CL3PUBF CLASS  TManager(const TManager&) = delete;
				CL3PUBF CLASS  TManager(TManager&&) = delete;
				CL3PUBF CLASS ~TManager();
		};

		/*********************************************************************/

		class CL3PUBT IAtom :
			protected virtual io::serialization::ISerializable,
			public system::memory::IManaged
		{
			friend struct IManager;
			private:
				IManager* const manager;
				mutable atom_id_t id;
				mutable atom_id_t id_parent;	//	flag == 0 means fork (copy), flag == 1 means override (new version)

			protected:
				CL3PUBF virtual void AtomSerialize(io::serialization::ISerializer&) const;

			public:
				void Serialize(io::serialization::ISerializer&) const final override;

				CL3PUBF bool BeforeDispose() override;

				CL3PUBF atom_id_t AtomID() const CL3_GETTER;
				CL3PUBF void Save() const;

				CL3PUBF CLASS IAtom(IAtom&&) = delete;	//	atoms cannot be moved as their memory address must not change
				CL3PUBF CLASS explicit IAtom(IManager*);	//	creates a new atom
				CL3PUBF CLASS explicit IAtom(io::serialization::IDeserializer&);	//	loads the atom from a stream
				CL3PUBF CLASS IAtom(const IAtom&);	//	creates a new atom based uppon the referenced atom (fork/copy)
				CL3PUBF CLASS virtual ~IAtom();
		};

		/*********************************************************************/

		template<typename T>
		class TAtomPtr
		{
			protected:
				IManager* manager;
				union
				{
					T* obj;
					atom_id_t id;
				};

			public:
				inline T& operator* () CL3_GETTER { this->Load(); return *this->obj; }
				inline T* operator->() CL3_GETTER { this->Load(); return  this->obj; }

				inline bool IsLoaded() const CL3_GETTER { return this->id.flag == 0; }
				inline bool IsBound() const CL3_GETTER { return this->id.id != 0; }
				void Load(bool fork = false);	//	loads the atom into memory
				void Unload();	//	allows the atom to be unloaded if all other references are also released

				TAtomPtr& operator=(atom_id_t);
				TAtomPtr& operator=(T*);
				TAtomPtr& operator=(const TAtomPtr&);

				inline IManager* Manager() const CL3_GETTER { return this->manager; }

				CLASS TAtomPtr();
				CLASS explicit TAtomPtr(IManager*, atom_id_t);
				CLASS explicit TAtomPtr(T*);
				CLASS ~TAtomPtr();
		};

		template<typename T>
		void TAtomPtr<T>::Load(bool fork)
		{
			if(!this->IsLoaded())
			{
				CL3_CLASS_ERROR(!this->IsBound(), error::TException, "this TAtomPtr was not bound to any atom");
				CL3_CLASS_LOGIC_ERROR(this->manager == NULL);
				this->obj = dynamic_cast<T*>(this->manager->Load(this->id, fork));
				//this->obj->IncRef();	//	the atom manager already incremented the ref-count
			}
		}

		template<typename T>
		void TAtomPtr<T>::Unload()
		{
			if(this->IsLoaded())
			{
				const IAtom* const a = this->obj;
				this->id = a->AtomID();
				this->id.flag = 1;
			}
		}

		template<typename T>
		TAtomPtr<T>& TAtomPtr<T>::operator=(atom_id_t id)
		{
			if(this->id.id != id.id)
			{
				this->Unload();
				this->id = id;
				this->id.flag = 1;
			}
			return *this;
		}

		template<typename T>
		TAtomPtr<T>& TAtomPtr<T>::operator=(T* obj)
		{
			if(this->obj != obj)
			{
				this->Unload();
				this->id.id = 0;
				this->id.flag = 0;
				this->manager = obj->Manager();
				this->obj = obj;
				this->obj->IncRef();
			}
			return *this;
		}

		template<typename T>
		TAtomPtr<T>& TAtomPtr<T>::operator=(const TAtomPtr& ptr)
		{
			this->Unload();
			this->manager = ptr.manager;
			this->id = ptr.id;
			if(this->IsLoaded())
				this->obj->IncRef();
			return *this;
		}

		template<typename T>
		CLASS TAtomPtr<T>::TAtomPtr() : manager(NULL)
		{
			this->id.id = 0;
			this->id.flag = 1;
		}

		template<typename T>
		CLASS TAtomPtr<T>::TAtomPtr(IManager* manager, atom_id_t id) : manager(manager)
		{
			this->id = id;
			this->id.flag = 1;
		}

		template<typename T>
		CLASS TAtomPtr<T>::TAtomPtr(T* obj) : manager(manager)
		{
			this->id.id = 0;
			this->id.flag = 0;
			this->obj = obj;
		}

		template<typename T>
		CLASS TAtomPtr<T>::~TAtomPtr()
		{
			this->Unload();
		}
	}
}
