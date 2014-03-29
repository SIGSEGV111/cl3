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

#include "io_text_encoding.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	encoding
			{
				const ICodec* const CODEC_CXX_CHAR = NULL;
				const ICodec* const CODEC_CXX_WCHAR = NULL;

				usys_t	ACharDecoder::Write		(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				uoff_t	ACharDecoder::ReadIn	(io::stream::IIn<char>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	AWCharDecoder::Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				uoff_t	AWCharDecoder::ReadIn	(io::stream::IIn<wchar_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	ACharEncoder::Read		(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				uoff_t	ACharEncoder::WriteOut	(io::stream::IOut<char>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	AWCharEncoder::Read		(wchar_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				uoff_t	AWCharEncoder::WriteOut	(io::stream::IOut<wchar_t>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min)
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}
