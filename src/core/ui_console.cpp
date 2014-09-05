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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "ui_console.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	ui
	{
		namespace	console
		{
			using namespace io::text::encoding;

			TConsole stdconsole(0, 1);

			usys_t	TConsole::Read	(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				return this->fds_in.Read((byte_t*)arr_items_read, n_items_read_max, n_items_read_min);
			}

			usys_t	TConsole::Write	(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				return this->fds_out.Write((const byte_t*)arr_items_write, n_items_write_max, n_items_write_min);
			}

			usys_t	TConsole::Read	(wchar_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t	TConsole::Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t	TConsole::Read	(io::text::TUTF32* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
			{
				CL3_NOT_IMPLEMENTED;
			}

			usys_t	TConsole::Write	(const io::text::TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				return this->enc_char->Write(arr_items_write, n_items_write_max, n_items_write_min);
			}

			CLASS	TConsole::TConsole	(fd_t fd_in, fd_t fd_out) : fds_in(fd_in), fds_out(fd_out), enc_char(CODEC_CXX_CHAR->CreateEncoder()), dec_char(CODEC_CXX_CHAR->CreateDecoder())
			{
				enc_char->Sink(&this->fds_out);
			}

			IConsole* console = &stdconsole;
		}
	}
}
