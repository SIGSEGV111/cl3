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

#ifndef	_include_cl3_core_io_stream_util_hpp_
#define	_include_cl3_core_io_stream_util_hpp_

#include "io_stream-base.hpp"
#include "error-ext.hpp"

#include <alloca.h>

namespace	cl3
{
	namespace	io
	{
		namespace	stream
		{
			template<class T>
			struct	AWriteOut : IIn<T>
			{
				using IIn<T>::Left;
				using IIn<T>::Read;

				off64_t	WriteOut	(IOut<T>& os, off64_t n_items_wo_max, off64_t n_items_wo_min = (off64_t)-1)
				{
					/*
						a) max -1, min -1: until source runs dry
						b) max -1, min N: at least N, but more if source and sink can handle it
						c) max N, min M, up to N, but at least M
						-) max N, min -1: exactly N items (special case of c)
					*/

					//	will use at most 16KiB (unless T is bigger than 16KiB, then sizeof(T) bytes are used)
					const off64_t n_buffer_items = CL3_MIN(n_items_wo_max, (off64_t)CL3_MAX(16384UL / sizeof(T), 1));
					T* buffer = reinterpret_cast<T*>(::alloca(sizeof(T) * n_buffer_items));

					off64_t n_already_transfered = 0;
					off64_t n_transfer_now;

					if(n_items_wo_min == (off64_t)-1)
						n_items_wo_min = n_items_wo_max;

					CL3_CLASS_ARGUMENT_ERROR(n_items_wo_max < n_items_wo_min, n_items_wo_max, "n_items_wo_max cannot be less than n_items_wo_min. (n_items_wo_min = "<<n_items_wo_min<<")");

					if(n_items_wo_min != (off64_t)-1)
					{
						//	b & c (min N case)

						while(n_already_transfered < n_items_wo_min)
						{
							n_transfer_now = CL3_MIN(n_buffer_items, n_items_wo_min - n_already_transfered);
							CL3_CLASS_LOGIC_ERROR(Read(buffer, n_transfer_now) != n_transfer_now);
							CL3_CLASS_LOGIC_ERROR(os.Write(buffer, n_transfer_now) != n_transfer_now);
							n_already_transfered += n_transfer_now;
						}
					}
					else
					{
						//	a (min -1 case)

						CL3_NOT_IMPLEMENTED;
					}


					//	a & b (max -1 case) & c (max N case)

					while(n_already_transfered < n_items_wo_max)
					{
						const off64_t n_space = os.Space();
						const off64_t n_left = Left();

						if(n_space == (off64_t)-1 || n_left == (off64_t)-1 || n_space == 0 || n_left == 0)
							break;	//	sink and/or source can't, or don't know if they can, handle more data => take no risks and abort here

						//	take the minimum amounts from all limitors
						n_transfer_now = CL3_MIN(n_buffer_items, CL3_MIN(n_items_wo_max - n_already_transfered, CL3_MIN(n_space, n_left)));

						Read(buffer, n_transfer_now);
						os.Write(buffer, n_transfer_now);
						n_already_transfered += n_transfer_now;
					}

					return n_already_transfered;
				}
			};
		}
	}
}

#endif
