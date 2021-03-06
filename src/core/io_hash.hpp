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

#ifndef	_include_cl3_core_io_hash_hpp_
#define	_include_cl3_core_io_hash_hpp_

#include "io_stream.hpp"
#include "system_types.hpp"

namespace cl3
{
	namespace io
	{
		namespace hash
		{
			using namespace system::types;

			template<class T>
			struct IHash : public stream::IOut<byte_t>
			{
				virtual T Hash() const CL3_GETTER = 0;
			};

			CL3PUBF u64_t JenkinsHash(const byte_t* arr_items, usys_t n_items);

			class CL3PUBT TMurMur32Hash : public IHash<u32_t>
			{
				protected:
					u32_t h;
					u32_t n_partial;
					byte_t arr_partial[4];

				public:
					using IOut<byte_t>::Write;
					CL3PUBF	void	Flush	() final override;
					CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	u32_t	Hash	() const final override CL3_GETTER;
					CL3PUBF	CLASS	TMurMur32Hash(u32_t seed = 0);
			};
		}
	}
}

#endif
