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

#ifndef INSIDE_CL3CORE
#error "compiling cl3 source code but macro INSIDE_CL3CORE is not defined"
#endif

#include "io_collection_list.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			CLASS	TIndexOutOfBoundsException::TIndexOutOfBoundsException	(ssys_t index, usys_t count) : TException("index out of bounds (index = %zd, count = %zu)", index, count), index(index), count(count)
			{
			}

			CLASS	TIndexOutOfBoundsException::TIndexOutOfBoundsException	(TIndexOutOfBoundsException&& oobe) : TException(static_cast<TException&&>(oobe))
			{
			}

			CLASS	TIndexOutOfBoundsException::~TIndexOutOfBoundsException	()
			{
			}
		}
	}
}