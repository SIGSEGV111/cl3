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

#ifndef INSIDE_CL3CORE
#error "compiling cl3 source code but macro INSIDE_CL3CORE is not defined"
#endif

#include "io_text_terminal.hpp"
#include "error.hpp"
#include "system_memory.hpp"
#include "system_task.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	terminal
			{
				using namespace error;
				using namespace encoding;
				using namespace stream;

				usys_t	TTerminal::Read		(char* /*arr_items_read*/, usys_t /*n_items_read_max*/, usys_t /*n_items_read_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	TTerminal::Write	(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					return this->os->Write((const byte_t*)arr_items_write, n_items_write_max, n_items_write_min);
				}

				usys_t	TTerminal::Read		(TUTF32* /*arr_items_read*/, usys_t /*n_items_read_max*/, usys_t /*n_items_read_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	TTerminal::Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					return this->encoder->Write(arr_items_write, n_items_write_max, n_items_write_min);
				}

				CLASS	TTerminal::TTerminal	(IOut<byte_t>* os, IIn<byte_t>* is) : os(os), is(is), encoder(CODEC_CXX_CHAR->CreateEncoder())
				{
					CL3_CLASS_ERROR(os == NULL, TException, "os must point to a valid output stream");
					CL3_CLASS_ERROR(is == NULL, TException, "is must point to a valid output stream");
					this->encoder->Sink(os);
				}

				CLASS	TTerminal::~TTerminal	()
				{
				}

				static system::memory::TUniquePtr<TTerminal> terminal;
				static system::memory::TUniquePtr<TTerminal> stderr;

				TTerminal& Terminal	()
				{
					if(terminal == NULL)
					{
						TTerminal* t = new TTerminal(&system::task::TSelfProcess::StdOut(), &system::task::TSelfProcess::StdIn());
						if(terminal.AtomicSwap(NULL, t) != NULL)
							delete t;
					}
					return *terminal.Object();
				}

				TTerminal& StdErr()
				{
					if(stderr == NULL)
					{
						TTerminal* t = new TTerminal(&system::task::TSelfProcess::StdErr(), &system::task::TSelfProcess::StdIn());
						if(stderr.AtomicSwap(NULL, t) != NULL)
							delete t;
					}
					return *stderr.Object();
				}
			}
		}
	}
}
