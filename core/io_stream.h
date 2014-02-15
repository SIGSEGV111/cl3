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

#ifndef	_include_cl3_core_io_stream_h_
#define	_include_cl3_core_io_stream_h_

#include "system_compiler.h"

namespace	cl3
{
	namespace	io
	{
		namespace	stream
		{
			template<class T>	struct	IStream;
			template<class T>	struct	IIn;
			template<class T>	struct	IOut;
			template<class T>	struct	ISource;
			template<class T>	struct	ISink;

			template<class T>
			struct	IStream
			{
			};

			template<class T>
			struct	IIn : virtual IStream<T>
			{
				virtual	size_t	Read	(T* arr_items_read, size_t n_items_read_max, size_t n_items_read_min = (size_t)-1) = 0;	//	reads at least "n_items_read_min" and at most "n_items_read_max" items from the stream into "arr_items_read", returns the amount of items actually read
				virtual	size_t	WriteOut(IOut<T>& os, size_t n_items_wo_max, size_t n_items_wo_min = (size_t)-1) = 0;
			};

			template<class T>
			struct	IOut : virtual IStream<T>
			{
				virtual	size_t	Write	(const T* arr_items_write, size_t n_items_write_max, size_t n_items_write_min = (size_t)-1) = 0;
				virtual	size_t	ReadIn	(IIn<T>& is, size_t n_items_ri_max, size_t n_items_ri_min = (size_t)-1) = 0;
			};

			template<class T>
			struct	IIO : IIn<T>, IOut<T>
			{
			};

			template<class T>
			struct	ISource
			{
				virtual	SETTER	void		Sink	(IOut<T>* os) = 0;
				virtual	GETTER	IOut<T>*	Sink	() const = 0;
			};

			template<class T>
			struct	ISink
			{
				virtual	SETTER	void		Source	(IIn<T>* is) = 0;
				virtual	GETTER	IIn<T>*		Source	() const = 0;
			};
		}
	}
}

#endif
