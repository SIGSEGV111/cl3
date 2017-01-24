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

#ifndef	_include_cl3_core_io_hpp_
#define	_include_cl3_core_io_hpp_

namespace	cl3
{
	namespace system
	{
		namespace task
		{
			namespace synchronization
			{
				struct IWaitable;
				struct IMutex;
				struct ISignal;
			}
		}
	}

	namespace io_v2
	{
		namespace stream
		{
			template<typename TItem> struct IStream;
			template<typename TItem> struct IIn;
			template<typename TItem> struct IOut;
			template<typename TItem> struct ISource;
			template<typename TItem> struct ISink;
			template<typename TItem> struct TPipe;

			static const usys_t INFINITE = (usys_t)-1L;

			template<typename TItem>
			struct IStream
			{
			};

			/*****************************************************/

			template<typename TItem>
			struct IIn : virtual IStream
			{
				virtual system::task::synchronization::IWaitable* InputReady() { return nullptr; }
				virtual usys_t Read(TItem* arr_items, usys_t n_items_max) CL3_WARN_UNUSED_RESULT = 0;
				virtual usys_t WriteOut(IOut<T>& os, usys_t n_items_max = INFINITE) CL3_WARN_UNUSED_RESULT = 0;
			};

			/*****************************************************/

			template<typename TItem>
			struct IOut : virtual IStream
			{
				virtual system::task::synchronization::IWaitable* OutputReady() { return nullptr; }
				virtual void Flush() = 0;
				virtual usys_t Write(const TItem* arr_items, usys_t n_items_max) CL3_WARN_UNUSED_RESULT = 0;
				virtual usys_t ReadIn(IIn<T>& is, usys_t n_items_max = INFINITE) CL3_WARN_UNUSED_RESULT = 0;
			};

			/*****************************************************/
		}

		namespace pipe
		{
			template<typename TItem> struct ISource;
			template<typename TItem> struct ISink;
			template<typename TItem> class TPipe;

			template<typename TItem>
			struct ISource
			{
				private:
					TPipe<TItem>* pipe;
			};

			template<typename TItem>
			struct ISink
			{
				private:
					TPipe<TItem>* pipe;
			};

			template<typename TItem>
			class TPipe
			{
				protected:
					ISource<TItem>* source;
					ISink<TItem>* sink;
			};
		}

		namespace buffer
		{
			template<typename TItem> struct IBuffer;
			template<typename TItem> struct IIterator;
		}

		namespace collection
		{
			template<typename TItem, typename TIndex> struct ICollection;

			template<typename TItem> struct IArray;
			template<typename TItem> struct IChain;
			template<typename TItem> struct IList;
			template<typename TItem> struct IMap;

			template<typename TItem, typename TIndex> struct IComposition;
		}
	}
}

#endif
