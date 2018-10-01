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

#include "atom.hpp"

namespace cl3
{
	namespace atom
	{
		bool IAtom::BeforeDispose()
		{
			if(this->id.id != 0)
				this->Save();
			return true;
		}

		void IAtom::Serialize(io::serialization::ISerializer& s) const
		{
			//	this the serializer which gets called when sombody wants to serialize a atom
			//	thus we only store the ID here
			s.Push("id", this->AtomID().id);
		}

		atom_id_t IAtom::AtomID() const
		{
			if(this->id.id == 0)
				this->manager->New(const_cast<IAtom*>(this));
			return this->id;
		}

		void IAtom::Save() const
		{
			this->manager->Save(this);
		}

		CLASS IAtom::IAtom(IManager* manager) : manager(manager), id(0), id_parent(0)
		{
			CL3_CLASS_ERROR(manager == NULL, error::TInvalidArgumentException, "manager");
		}

		void IAtom::AtomSerialize(io::serialization::ISerializer& s) const
		{
			s.Push("id", this->AtomID().id);
			s.Push("id_parent", id_parent.id);
// 			s.Push("ts_save", system::time::TTime::Now(system::time::EClock::REALTIME));

			CL3_NOT_IMPLEMENTED;
		}

		CLASS IAtom::IAtom(io::serialization::IDeserializer& ds) : manager(NULL), id(0), id_parent(0)
		{
			//	this is the deserializer which gets called when an atom was loaded from disk it has to match AtomSerialize()
			//	it gets called by the derived classes constructor with a derserializer for the actual data portion

			ds.Pop("id", id.u64);
			ds.Pop("id_parent", id_parent.u64);

			system::time::TTime ts_save;
// 			ds.Pop("ts_save", ts_save);

			CL3_NOT_IMPLEMENTED;
		}

		CLASS IAtom::IAtom(const IAtom& other) : manager(other.manager), id(0), id_parent(other.AtomID())
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

		void TManager::Save(const IAtom*)
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
