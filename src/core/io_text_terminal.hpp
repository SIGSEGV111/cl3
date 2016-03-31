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

#ifndef	_include_cl3_core_io_text_terminal_hpp_
#define	_include_cl3_core_io_text_terminal_hpp_

#include "io_text_encoding.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	terminal
			{
				class	TTerminal : public virtual ITextStream,
									public virtual encoding::AWCharDecoder,
									public virtual encoding::AWCharEncoder
				{
					private:
						CLASS	TTerminal	(const TTerminal&) = delete;

					protected:
						stream::IOut<byte_t>* os;
						stream::IIn<byte_t>* is;
						system::memory::TUniquePtr<encoding::IEncoder> encoder;

					public:
						using IIn<wchar_t>::Read;
						using IOut<wchar_t>::Write;

						CL3PUBF	usys_t	Read	(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF	usys_t	Write	(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						CL3PUBF	usys_t	Read	(TUTF32* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF	usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

						CL3PUBF	CLASS	TTerminal	(stream::IOut<byte_t>* os, stream::IIn<byte_t>* is);
						CL3PUBF	virtual	~TTerminal	();
				};

				CL3PUBF TTerminal& Terminal	() CL3_GETTER;
			}
		}
	}
}

#endif
