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

#ifndef	_include_cl3_core_io_stream_hpp_
#define	_include_cl3_core_io_stream_hpp_

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

			class	CL3PUBT	TIOException : public error::TException
			{
				public:
					CL3PUBF	CLASS	TIOException	();
					CL3PUBF	CLASS	TIOException	(const TIOException&);
					CL3PUBF	CLASS	~TIOException	();
			};

			class	CL3PUBT	TSourceDryException : public TIOException
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

			class	CL3PUBT	TSinkFloodedException : public TIOException
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
				virtual	CL3_SETTER	void		Sink	(IOut<T>* os) = 0;
				virtual	CL3_GETTER	IOut<T>*	Sink	() const = 0;
			};

			template<class T>
			struct	ISink
			{
				virtual	CL3_SETTER	void		Source	(IIn<T>* is) = 0;
				virtual	CL3_GETTER	IIn<T>*		Source	() const = 0;
			};

			template<class T>
			struct	CL3PUBT	AOutPassive : public virtual IOut<T>
			{
				using IOut<T>::Write;

				CL3PUBF	uoff_t	ReadIn	(IIn<T>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1)
				{
					/*
						n_items_ri_max == -1 && n_items_ri_min == -1 => until source runs dry
						n_items_ri_max != -1 && n_items_ri_min != -1 => at least min, but up to max
						n_items_ri_max == -1 && n_items_ri_min != -1 => at least min, but up to source dry
						n_items_ri_max != -1 && n_items_ri_min == -1 => exactly max items
					*/

					if(n_items_ri_min == (uoff_t)-1)
						n_items_ri_min = n_items_ri_max;

					const bool b_srcctl_min = n_items_ri_min == (uoff_t)-1;
					const bool b_srcctl_max = n_items_ri_max == (uoff_t)-1;

					uoff_t n_items_processed = 0;
					const usys_t n_items_buffer = CL3_MAX(1, 256 / sizeof(T));
					T arr_items_buffer[n_items_buffer];

					if(b_srcctl_min)
					{
						uoff_t n_items_now;
						do
						{
							n_items_now = CL3_MIN(n_items_buffer, n_items_ri_min - n_items_processed);
							n_items_now = is.Read(arr_items_buffer, n_items_now, 0);
							this->Write(arr_items_buffer, n_items_now);
							n_items_processed += n_items_now;
						}
						while(n_items_now > 0);
					}
					else
					{
						while(n_items_processed < n_items_ri_min)
						{
							const uoff_t n_items_now = CL3_MIN(n_items_buffer, n_items_ri_min - n_items_processed);
							is.Read(arr_items_buffer, n_items_now);
							this->Write(arr_items_buffer, n_items_now);
							n_items_processed += n_items_now;
						}
					}

					if(b_srcctl_max)
					{
						uoff_t n_items_now;
						do
						{
							n_items_now = CL3_MIN(n_items_buffer, n_items_ri_max - n_items_processed);
							n_items_now = is.Read(arr_items_buffer, n_items_now, 0);
							this->Write(arr_items_buffer, n_items_now);
							n_items_processed += n_items_now;
						}
						while(n_items_now > 0);
					}
					else
					{
						while(n_items_processed < n_items_ri_max)
						{
							uoff_t n_items_now = CL3_MIN(n_items_buffer, n_items_ri_max - n_items_processed);
							n_items_now = is.Read(arr_items_buffer, n_items_now, 0);
							this->Write(arr_items_buffer, n_items_now);
							n_items_processed += n_items_now;
						}
					}

					return n_items_processed;
				}
			};
		}
	}
}

#endif
