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

#include <cl3/core/io_stream.hpp>

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

			CLASS	TIOException::TIOException	(TIOException&& e) : TException(static_cast<TException&&>(e))
			{
			}

			CLASS	TIOException::~TIOException	()
			{
			}

			CLASS	TSourceDryException::TSourceDryException	(uoff_t n_items_requested_read_max, uoff_t n_items_requested_read_min, uoff_t n_items_actually_read, uoff_t n_items_still_available) :
					n_items_requested_read_max(n_items_requested_read_max),
					n_items_requested_read_min(n_items_requested_read_min),
					n_items_actually_read(n_items_actually_read),
					n_items_still_available(n_items_still_available)
			{
			}

			CLASS	TSourceDryException::TSourceDryException	(TSourceDryException&& e)
				: TIOException(static_cast<TIOException&&>(e)),
					n_items_requested_read_max(e.n_items_requested_read_max),
					n_items_requested_read_min(e.n_items_requested_read_min),
					n_items_actually_read(e.n_items_actually_read),
					n_items_still_available(e.n_items_still_available)
			{
			}

			CLASS	TSourceDryException::~TSourceDryException	()
			{
			}

			CLASS	TSinkFloodedException::TSinkFloodedException	(uoff_t n_items_requested_write_max, uoff_t n_items_requested_write_min, uoff_t n_items_actually_written, uoff_t n_items_still_space) :
					n_items_requested_write_max(n_items_requested_write_max),
					n_items_requested_write_min(n_items_requested_write_min),
					n_items_actually_written(n_items_actually_written),
					n_items_still_space(n_items_still_space)
			{
			}

			CLASS	TSinkFloodedException::TSinkFloodedException	(TSinkFloodedException&& e)
				: TIOException(static_cast<TIOException&&>(e)),
					n_items_requested_write_max(e.n_items_requested_write_max),
					n_items_requested_write_min(e.n_items_requested_write_min),
					n_items_actually_written(e.n_items_actually_written),
					n_items_still_space(e.n_items_still_space)
			{
			}

			CLASS	TSinkFloodedException::~TSinkFloodedException	()
			{
			}
		}
	}
}
