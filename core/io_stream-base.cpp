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

#include "io_stream-base.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	stream
		{
			CLASS	TIOException::TIOException	() : TException("I/O error occured")
			{
			}

			CLASS	TIOException::TIOException	(const TIOException& e) : TException(e)
			{
			}

			CLASS	TIOException::~TIOException	()
			{
			}

			CLASS	TSourceDryException::TSourceDryException	(size_t n_items_read_max, size_t n_items_read_min, size_t n_items_read) : n_items_read_max(n_items_read_max), n_items_read_min(n_items_read_min), n_items_read(n_items_read)
			{
			}

			CLASS	TSourceDryException::TSourceDryException	(const TSourceDryException& e) : TIOException(e)
			{
			}

			CLASS	TSourceDryException::~TSourceDryException	()
			{
			}

			CLASS	TSinkFloodedException::TSinkFloodedException	(size_t n_items_write_max, size_t n_items_write_min, size_t n_items_written) : n_items_write_max(n_items_write_max), n_items_write_min(n_items_write_min), n_items_written(n_items_written)
			{
			}

			CLASS	TSinkFloodedException::TSinkFloodedException	(const TSinkFloodedException& e) : TIOException(e)
			{
			}

			CLASS	TSinkFloodedException::~TSinkFloodedException	()
			{
			}
		}
	}
}
