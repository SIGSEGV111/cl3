/*
 l ibcl3 - common library version 3 *
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

extern "C" void hashlittle2(const void *key, size_t length, u32_t *pc, u32_t *pb);

namespace cl3
{
	namespace io
	{
		namespace hash
		{
			static u64_t JenkinsHash(const byte_t* arr_items, usys_t n_items)
			{
				union
				{
					struct
					{
						u32_t l,h;
					};
					u64_t qw;
				};

				::hashlittle2(arr_items, n_items, &l, &h);

				return qw;
			}

			class TJenkinsHash : public IIn<byte_t>
			{
				
			};
		}
	}
}

#endif
