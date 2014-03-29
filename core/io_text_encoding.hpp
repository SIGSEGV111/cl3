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

#ifndef	_include_cl3_core_io_text_encoding_hpp_
#define	_include_cl3_core_io_text_encoding_hpp_

#include "io_text.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	encoding
			{
				CL3PUBF extern const ICodec* const CODEC_CXX_CHAR;
				CL3PUBF extern const ICodec* const CODEC_CXX_WCHAR;

				class	ACharDecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<char>
				{
					public:
						using stream::IOut<TUTF32>::Write;
						using stream::IOut<TUTF32>::ReadIn;

						CL3PUBF	usys_t	Write	(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<char>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);
				};

				class	AWCharDecoder : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<wchar_t>
				{
					public:
						using stream::IOut<TUTF32>::Write;
						using stream::IOut<TUTF32>::ReadIn;

						CL3PUBF	usys_t	Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
						CL3PUBF	uoff_t	ReadIn	(io::stream::IIn<wchar_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);
				};

				class	ACharEncoder : public virtual stream::IIn<TUTF32>, public virtual stream::IIn<char>
				{
					public:
						using stream::IIn<TUTF32>::Read;
						using stream::IIn<TUTF32>::WriteOut;

						CL3PUBF	usys_t	Read	(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(io::stream::IOut<char>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min = (uoff_t)-1);
				};

				class	AWCharEncoder : public virtual stream::IIn<TUTF32>, public virtual stream::IIn<wchar_t>
				{
					public:
						using stream::IIn<TUTF32>::Read;
						using stream::IIn<TUTF32>::WriteOut;

						CL3PUBF	usys_t	Read	(wchar_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1);
						CL3PUBF	uoff_t	WriteOut(io::stream::IOut<wchar_t>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min = (uoff_t)-1);
				};
			}
		}
	}
}

#endif
