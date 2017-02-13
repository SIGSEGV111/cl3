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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "atom.hpp"

namespace cl3
{
	namespace atom
	{
		void IAtom::Serialize(io::serialization::ISerializer& s) const
		{
			s.Push("id", id.u64);
			s.Push("id_parent", id_parent.u64);
			s.Push("ts_birthdate", system::time::TTime::Now(system::time::TIME_CLOCK_REALTIME));
		}

		void IAtom::IncRef() const
		{
			system::compiler::AtomicAdd(this->n_refs, (usys_t)1);
		}

		void IAtom::DecRef() const
		{
			if(system::compiler::AtomicSub(this->n_refs, (usys_t)1) == 0)
			{
				if(this->id.id != 0)
					this->Save();
				delete this;
			}
		}

		atom_id_t IAtom::AtomID() const
		{
			if(this->id.id == 0)
				this->manager->New(const_cast<IAtom*>(this));
			return this->id;
		}

		void IAtom::Save() const
		{
			this->manager->Save(const_cast<IAtom*>(this));
		}

		CLASS IAtom::IAtom(IManager* manager) : manager(manager), id(0), id_parent(0), n_refs(0)
		{
			CL3_CLASS_ERROR(manager == NULL, error::TInvalidArgumentException, "manager");
		}

		CLASS IAtom::IAtom(io::serialization::IDeserializer& ds) : manager(NULL), id(0), id_parent(0), n_refs(0)
		{
			ds.Pop("id", id);
			ds.Pop("id_parent", id_parent);

			system::time::TTime ts_birthdate;
			ds.Pop("ts_birthdate", ts_birthdate);
		}

		CLASS IAtom::IAtom(const IAtom& other) : manager(other.manager), id(0), id_parent(other.AtomID()), n_refs(0)
		{
		}

		CLASS IAtom::~IAtom()
		{
		}

		/*********************************************************************/

		IAtom* TManager::Load(atom_id_t, bool /*copy_orig*/)
		{
			CL3_NOT_IMPLEMENTED;
		}

		void TManager::Save(IAtom*)
		{
			CL3_NOT_IMPLEMENTED;
		}

		void TManager::New(IAtom*)
		{
			CL3_NOT_IMPLEMENTED;
		}

		void TManager::Delete(atom_id_t)
		{
			CL3_NOT_IMPLEMENTED;
		}

		atom_id_t TManager::ParentOf(atom_id_t)
		{
			CL3_NOT_IMPLEMENTED;
		}

		system::time::TTime TManager::BirthdateOf(atom_id_t)
		{
			CL3_NOT_IMPLEMENTED;
		}

		CLASS TManager::TManager(const io::collection::IStaticCollection<io::file::TFile*>& data_files, io::file::TFile* index_file) : index_file(index_file), data_files(data_files), map_index(index_file), loaded_atoms(), next_id(0)
		{
			CL3_NOT_IMPLEMENTED;
		}

		CLASS TManager::~TManager()
		{
			//CL3_NOT_IMPLEMENTED;
		}
	}
}
