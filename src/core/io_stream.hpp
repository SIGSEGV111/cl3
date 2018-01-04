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
#include "error.hpp"
#include "system_task_async.hpp"

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

			struct	CL3PUBT	TIOException : error::TException
			{
				CL3PUBF	CLASS	TIOException	();
				CL3PUBF	CLASS	TIOException	(TIOException&&);
				CL3PUBF	CLASS	~TIOException	();
			};

			struct	CL3PUBT	TSourceDryException : TIOException
			{
				uoff_t n_items_requested_read_max;
				uoff_t n_items_requested_read_min;
				uoff_t n_items_actually_read;
				uoff_t n_items_still_available;

				CL3PUBF	CLASS	TSourceDryException	(uoff_t n_items_requested_read_max, uoff_t n_items_requested_read_min, uoff_t n_items_actually_read, uoff_t n_items_still_available);
				CL3PUBF	CLASS	TSourceDryException	(TSourceDryException&&);
				CL3PUBF	CLASS	~TSourceDryException();
			};

			struct	CL3PUBT	TSinkFloodedException : TIOException
			{
				uoff_t n_items_requested_write_max;
				uoff_t n_items_requested_write_min;
				uoff_t n_items_actually_written;
				uoff_t n_items_still_space;

				CL3PUBF	CLASS	TSinkFloodedException	(uoff_t n_items_requested_write_max, uoff_t n_items_requested_write_min, uoff_t n_items_actually_written, uoff_t n_items_still_space);
				CL3PUBF	CLASS	TSinkFloodedException	(TSinkFloodedException&&);
				CL3PUBF	CLASS	~TSinkFloodedException	();
			};

			template<class T>
			struct	IStream
			{
			};

			template<class T>
			struct	IIn : virtual IStream<T>
			{
				virtual	usys_t	Remaining	() const CL3_GETTER { return (usys_t)-1; }	//	number of items which are at least left for reading (-1 if infinite, 0 if the source is completly empty/dry and can never ever provide any more items)
				virtual	usys_t	Read		(T* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) CL3_WARN_UNUSED_RESULT = 0;	//	reads at least "n_items_read_min" and at most "n_items_read_max" items from the stream into "arr_items_read", returns the amount of items actually read
				inline	void	Read		(T* arr_items_read, usys_t n_items_read)			{ CL3_CLASS_LOGIC_ERROR(this->Read(arr_items_read, n_items_read, n_items_read) != n_items_read); }
				inline	bool	IsDry		() const CL3_GETTER { return this->Left() == 0UL; }

				//	pipe data until either source.IsDry() or sink.IsFlooded()
				virtual	usys_t	WriteOut	(IOut<T>& os)
				{
					return this->WriteOut(os, (usys_t)-1, 0);
				}

				//	pipe data until either source.IsDry() or sink.IsFlooded(), but at least n_items_wo_min, and at most n_items_wo_max
				virtual	usys_t	WriteOut	(IOut<T>& os, usys_t n_items_wo_max, usys_t n_items_wo_min) CL3_WARN_UNUSED_RESULT
				{
					const usys_t n_items_buffer = CL3_MAX(1, 1024 / sizeof(T));
					usys_t n_items_wo = 0;
					T buffer[n_items_buffer];

					CL3_CLASS_ERROR(this->Remaining() < n_items_wo_min, TSourceDryException, n_items_wo_max, n_items_wo_min, 0, this->Remaining());
					CL3_CLASS_ERROR(os.Space() < n_items_wo_min, TSinkFloodedException, n_items_wo_max, n_items_wo_min, 0, os.Space());

					while(n_items_wo < n_items_wo_min)
					{
						const usys_t n_items_work = CL3_MIN(n_items_wo_min - n_items_wo, n_items_buffer);
						this->Read(buffer, n_items_work);
						os.Write(buffer, n_items_work);
						n_items_wo += n_items_work;
					}

					while(n_items_wo < n_items_wo_max)
					{
						const usys_t n_items_request = CL3_MIN(n_items_wo_max - n_items_wo, CL3_MIN(n_items_buffer, CL3_MIN(this->Remaining(), os.Space())));
						if(n_items_request == 0) break;
						const usys_t n_items_work = this->Read(buffer, n_items_request, 0);
						if(n_items_work == 0) break;	//	this should *in theory, in a perfect world* never happen
						os.Write(buffer, n_items_work);
						n_items_wo += n_items_work;
					}

					return n_items_wo;
				}

				inline	void	WriteOut	(IOut<T>& os, usys_t n_items_wo) { CL3_CLASS_LOGIC_ERROR(this->WriteOut(os, n_items_wo, n_items_wo) != n_items_wo); }
			};

			template<class T>
			struct	IOut : virtual IStream<T>
			{
				virtual	void	Flush		() {}	//	flush buffers, and recursively call Flush() on all downstream sinks
				virtual	usys_t	Space		() const CL3_GETTER { return (usys_t)-1; }	//	return the amount of items which at least still can be written to the sink (-1 if infinite, 0 if the sink can ever ever accept more items)
				virtual	usys_t	Write		(const T* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT = 0;
				inline	void	Write		(const T* arr_items_write, usys_t n_items_write)	{ CL3_CLASS_LOGIC_ERROR(this->Write(arr_items_write, n_items_write, n_items_write) != n_items_write); }
				inline	bool	IsFlooded	() const CL3_GETTER { return this->Space() == 0UL; }
			};

			template<class T>
			struct	ISource
			{
				virtual	void		Sink	(IOut<T>* os) CL3_SETTER = 0;
				virtual	IOut<T>*	Sink	() const CL3_GETTER = 0;
			};

			template<class T>
			struct	ISink
			{
				virtual	void		Source	(IIn<T>* is) CL3_SETTER = 0;
				virtual	IIn<T>*		Source	() const CL3_GETTER = 0;
			};

			namespace v2
			{
				enum class EMode
				{
					PASSIVE, ACTIVE, INDIFFERENT
				};

				template<typename T>
				class ISource;

				template<typename T>
				class ISink;

				template<typename T>
				class ISource :
						private system::task::async::TAsyncEventProcessor::TCallback::IReceiver,
						private system::task::async::TAsyncEventProcessor::TCallback,
						public stream::IIn<T>
				{
					private:
						CL3PUBF void AsyncCallback(system::task::async::TAsyncEventProcessor*, system::task::synchronization::IWaitable*) final override;

					protected:
						ISink<T>* sink;

						CL3PUBF CLASS explicit ISource(system::task::async::TAsyncEventProcessor* aep)
						{
							this->aep = aep;
						}

						virtual void OnSinkChange(ISink<T>*) {}

					public:
						CL3PUBF void Sink(ISink<T>* new_sink) CL3_SETTER
						{
							if(this->sink != new_sink)
							{
								this->OnSinkChange(new_sink);
								this->sink = new_sink;
								new_sink->Source(this);
							}
						}

						CL3PUBF ISink<T>* Sink() const CL3_GETTER { return this->sink; }

						virtual system::task::synchronization::IWaitable* OnSourceReady() { return NULL; }
				};

				template<typename T>
				class ISink
				{
					protected:
						ISource<T>* source;

						//	called before the source is changed
						virtual void OnSourceChange(ISource<T>*) {}

					public:
						CL3PUBF void Source(ISource<T>*) CL3_SETTER;
						CL3PUBF ISource<T>* Source() const CL3_GETTER;

						//	waitable which can be used to determine if the sink can accept more data right now
						//	NULL means the sink is always ready
						virtual system::task::synchronization::IWaitable* OnSinkReady() { return NULL; }

						//	returns the preferred call-mode
						//	PASSIVE means that sources should use the Write() function
						//	ACTIVE means that sources should use the ReadIn() function
						//	INDIFFERENT means that neither implementation provides benefits
						virtual EMode PreferredSinkMode() const CL3_GETTER { return EMode::INDIFFERENT; }

						//	returns the amount of free space within the sink
						//	-1 means infinite
						virtual usys_t Space() const CL3_GETTER { return (usys_t)-1; }

						//	instructs the sink to read data from its source
						//	calls to ReadIn() must never block
						virtual usys_t ReadIn(usys_t n_items_ri_max) CL3_WARN_UNUSED_RESULT = 0;

						//	write actual data to the sink
						//	calls to Write() must never block
						virtual usys_t Write(const T* arr_items_write, usys_t n_items_write_max) CL3_WARN_UNUSED_RESULT = 0;
				};

				template<typename T>
				void ISink<T>::Source(ISource<T>* new_source)
				{
					if(this->source != new_source)
					{
						this->OnSourceChange(new_source);
						this->source = new_source;
						new_source->Sink(this);
					}
				}

				template<typename T>
				ISource<T>* ISink<T>::Source() const
				{
					return this->source;
				}

				static const usys_t SZ_ACTIVE_BUFFER = 4096;

				//	implements the Readin() function based on Space() and Write()
				//	this class requires that the Space() functions returns an accurate count
				template<typename T>
				struct AActiveSink : public virtual ISink<T>
				{
					using ISink<T>::Space;
					using ISink<T>::Write;

					CL3PUBF EMode PreferredSinkMode() const final override CL3_GETTER { return EMode::PASSIVE; }

					CL3PUBF usys_t ReadIn(usys_t n_items_ri_max) final override CL3_WARN_UNUSED_RESULT
					{
						CL3_CLASS_ERROR(this->source == NULL, error::TException, "sink must be bound to a source before calling ReadIn()");
						usys_t n_read = 0;

						const usys_t n_items_buffer = CL3_MAX(1, SZ_ACTIVE_BUFFER / sizeof(T));
						T arr_items_buffer[n_items_buffer];

						while(n_read < n_items_ri_max && this->source->Remaining() > 0)
						{
							const usys_t n_read_now = CL3_MIN(n_items_buffer, this->Space());
							const usys_t n = this->source->Read(arr_items_buffer, n_read_now);
							if(n == 0)
								break;
							CL3_CLASS_LOGIC_ERROR(this->Write(arr_items_buffer, n) != n);
							n_read += n;
						}

						return n_read;
					}
				};
			}
		}
	}
}

#endif
