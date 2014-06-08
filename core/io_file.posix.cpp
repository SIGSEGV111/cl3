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

#include "io_file.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	file
		{
			void	TStream::Index		(uoff_t new_index)
			{
				this->index = new_index;
			}

			uoff_t	TStream::Index		() const
			{
				return index;
			}

			usys_t	TStream::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			uoff_t	TStream::WriteOut	(IOut<byte_t>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t	TStream::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			uoff_t	TStream::ReadIn		(IIn<byte_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TStream::TStream	(TFile* file)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TStream::TStream	(const TStream& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TStream::~TStream	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			/**********************************************************************/


			CLASS	TFile::TFile	()
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TFile::TFile	(const text::string::TString& name, int access, ECreate create)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TFile::TFile	(const TFile& other)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS	TFile::~TFile	()
			{
				CL3_NOT_IMPLEMENTED;
			}
		}
	}
}
