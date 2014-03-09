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

#ifndef	_include_cl3_core_io_stream_base_hpp_
#define	_include_cl3_core_io_stream_base_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "system_task_synchronization.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	stream
		{
			template<class T>	struct	IStream;
			template<class T>	struct	IIn;
			template<class T>	struct	IOut;
			template<class T>	struct	ISource;
			template<class T>	struct	ISink;

			class	TIOException : public error::TException
			{
				public:
					CL3PUBF	CLASS	TIOException	();
					CL3PUBF	CLASS	TIOException	(const TIOException&);
					CL3PUBF	CLASS	~TIOException	();
			};

			class	TSourceDryException : public TIOException
			{
				public:
					size_t n_items_read_max;	//	maximum number of items to read
					size_t n_items_read_min;	//	minimum number of items to read
					size_t n_items_read;		//	number of items actually read

					CL3PUBF	CLASS	TSourceDryException	(size_t n_items_read_max, size_t n_items_read_min, size_t n_items_read);
					CL3PUBF	CLASS	TSourceDryException	(const TSourceDryException&);
					CL3PUBF	CLASS	~TSourceDryException();
			};

			class	TSinkFloodedException : public TIOException
			{
				public:
					size_t n_items_write_max;	//	maximum number of items to write
					size_t n_items_write_min;	//	minimum number of items to write
					size_t n_items_written;		//	number of items actually written

					CL3PUBF	CLASS	TSinkFloodedException	(size_t n_items_write_max, size_t n_items_write_min, size_t n_items_written);
					CL3PUBF	CLASS	TSinkFloodedException	(const TSinkFloodedException&);
					CL3PUBF	CLASS	~TSinkFloodedException	();
			};

			template<class T>
			struct	IStream
			{
			};

			template<class T>
			struct	IIn : virtual IStream<T>, system::task::synchronization::IInterlocked
			{
				virtual	size_t	Read	(T* arr_items_read, size_t n_items_read_max, size_t n_items_read_min = (size_t)-1) = 0;	//	reads at least "n_items_read_min" and at most "n_items_read_max" items from the stream into "arr_items_read", returns the amount of items actually read
				virtual	off64_t	WriteOut(IOut<T>& os, off64_t n_items_wo_max, off64_t n_items_wo_min = (off64_t)-1) = 0;
				virtual	GETTER	off64_t	Left	(size_t sz_unit = sizeof(T)) const = 0;	//	returns the number of items at least available for reading, returns -1 if exact amount cannot be determined
			};

			template<class T>
			struct	IOut : virtual IStream<T>, system::task::synchronization::IInterlocked
			{
				virtual	size_t	Write	(const T* arr_items_write, size_t n_items_write_max, size_t n_items_write_min = (size_t)-1) = 0;
				virtual	off64_t	ReadIn	(IIn<T>& is, off64_t n_items_ri_max, off64_t n_items_ri_min = (off64_t)-1) = 0;
				virtual	GETTER	off64_t	Space	(size_t sz_unit = sizeof(T)) const = 0;	//	returns the number of items for which at least is space available, returns -1 if exact amount cannot be determined
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
