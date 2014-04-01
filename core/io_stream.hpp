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
					uoff_t n_items_requested_read_max;
					uoff_t n_items_requested_read_min;
					uoff_t n_items_actually_read;
					uoff_t n_items_still_available;

					CL3PUBF	CLASS	TSourceDryException	(uoff_t n_items_requested_read_max, uoff_t n_items_requested_read_min, uoff_t n_items_actually_read, uoff_t n_items_still_available);
					CL3PUBF	CLASS	TSourceDryException	(const TSourceDryException&);
					CL3PUBF	CLASS	~TSourceDryException();
			};

			class	TSinkFloodedException : public TIOException
			{
				public:
					uoff_t n_items_requested_write_max;
					uoff_t n_items_requested_write_min;
					uoff_t n_items_actually_written;
					uoff_t n_items_still_space;

					CL3PUBF	CLASS	TSinkFloodedException	(uoff_t n_items_requested_write_max, uoff_t n_items_requested_write_min, uoff_t n_items_actually_written, uoff_t n_items_still_space);
					CL3PUBF	CLASS	TSinkFloodedException	(const TSinkFloodedException&);
					CL3PUBF	CLASS	~TSinkFloodedException	();
			};

			template<class T>
			struct	IStream
			{
			};

			template<class T>
			struct	IIn : virtual IStream<T>
			{
				virtual	usys_t	Read	(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1) = 0;	//	reads at least "n_items_read_min" and at most "n_items_read_max" items from the stream into "arr_items_read", returns the amount of items actually read
				virtual	uoff_t	WriteOut(IOut<T>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min = (uoff_t)-1) = 0;
			};

			template<class T>
			struct	IOut : virtual IStream<T>
			{
				virtual	usys_t	Write	(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1) = 0;
				virtual	uoff_t	ReadIn	(IIn<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1) = 0;
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
