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

#ifndef	_include_cl3_core_io_buffer_hpp_
#define	_include_cl3_core_io_buffer_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "io_stream.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	buffer
		{
			template<class T>
			struct	IBufferedStream : public virtual stream::IIn<T>, public virtual stream::IOut<T>
			{
				virtual	void	Index	(uoff_t) = 0;
				virtual	uoff_t	Index	() const = 0;
			};

			template<class T>
			struct	IBuffer
			{
				virtual	void	Read	(uoff_t index, T* arr_items_read, usys_t n_items_read) = 0;
				virtual	void	WriteOut(uoff_t index, stream::IOut<T>& os, usys_t n_items_wo) = 0;
				virtual	void	Write	(uoff_t index, const T* arr_items_write, usys_t n_items_write) = 0;
				virtual	void	ReadIn	(uoff_t index, stream::IIn<T>& is, usys_t n_items_ri) = 0;
				virtual	system::memory::TUniquePtr<IBufferedStream<T> >	CreateStream	() CL3_WARN_UNUSED_RESULT = 0;
			};
		}
	}
}

#endif
