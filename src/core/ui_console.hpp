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

#ifndef	_include_cl3_core_ui_terminal_hpp_
#define	_include_cl3_core_ui_terminal_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "io_text.hpp"
#include "io_text_encoding.hpp"
#include "io_stream_fd.hpp"
#include "context.hpp"
#include "system_memory.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	ui
	{
		namespace	console
		{
			using namespace system::types;

			struct	CL3PUBT	IConsole : virtual io::text::ITextStream
			{
			};

			class	CL3PUBT	TConsole : public virtual IConsole
			{
				CLASS		TConsole	(const TConsole&) = delete;
				TConsole&	operator=	(const TConsole&) = delete;

				protected:
					io::stream::fd::TFDStream fds_in;
					io::stream::fd::TFDStream fds_out;

					system::memory::TUniquePtr<io::text::encoding::IEncoder> enc_char;	//	UTF-32 to char
					system::memory::TUniquePtr<io::text::encoding::IDecoder> dec_char;	//	char to UTF-32

				public:
					CL3PUBF	usys_t	Read	(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	usys_t	Write	(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	usys_t	Read	(wchar_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	usys_t	Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	usys_t	Read	(io::text::TUTF32* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	usys_t	Write	(const io::text::TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TConsole	(fd_t fd_in, fd_t fd_out);
			};

			CL3PUBV IConsole* console;
		}
	}
}

#endif
