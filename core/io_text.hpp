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

#ifndef	_include_cl3_core_io_text_hpp_
#define	_include_cl3_core_io_text_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "io_stream.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	text
		{
			namespace	string
			{
				struct	IString;
			}

			namespace	token
			{
				struct	ITokenizer;
			};

			namespace	encoding
			{
				class	TDecodeException;
				class	TEncodeException;
				struct	ICodec;
				struct	IEncoder;
				struct	IDecoder;
			};

			struct	TUTF32
			{
				u32 code;

				CL3PUBF	CLASS	TUTF32	(char chr);
				CL3PUBF	CLASS	TUTF32	(wchar_t wchr);
				inline	CLASS	TUTF32	(u32 code) throw() : code(code) {}
			};

			struct	ITextReader : public stream::IIn<TUTF32>
			{
				CL3PUBF	ITextReader&	operator>>	(TUTF32&);
				CL3PUBF	ITextReader&	operator>>	(s8&);
				CL3PUBF	ITextReader&	operator>>	(u8&);
				CL3PUBF	ITextReader&	operator>>	(s16&);
				CL3PUBF	ITextReader&	operator>>	(u16&);
				CL3PUBF	ITextReader&	operator>>	(s32&);
				CL3PUBF	ITextReader&	operator>>	(u32&);
				CL3PUBF	ITextReader&	operator>>	(s64&);
				CL3PUBF	ITextReader&	operator>>	(u64&);
				CL3PUBF	ITextReader&	operator>>	(f32&);
				CL3PUBF	ITextReader&	operator>>	(f64&);
				CL3PUBF	ITextReader&	operator>>	(string::IString&);
				CL3PUBF	ITextReader&	operator>>	(token::ITokenizer&);
			};

			struct	ITextWriter : public stream::IOut<TUTF32>
			{
				CL3PUBF	ITextWriter&	operator<<	(TUTF32);
				CL3PUBF	ITextWriter&	operator<<	(s8);
				CL3PUBF	ITextWriter&	operator<<	(u8);
				CL3PUBF	ITextWriter&	operator<<	(s16);
				CL3PUBF	ITextWriter&	operator<<	(u16);
				CL3PUBF	ITextWriter&	operator<<	(s32);
				CL3PUBF	ITextWriter&	operator<<	(u32);
				CL3PUBF	ITextWriter&	operator<<	(s64);
				CL3PUBF	ITextWriter&	operator<<	(u64);
				CL3PUBF	ITextWriter&	operator<<	(f32);
				CL3PUBF	ITextWriter&	operator<<	(f64);
				CL3PUBF	ITextWriter&	operator<<	(const string::IString&);
				CL3PUBF	ITextWriter&	operator<<	(const char*);
				CL3PUBF	ITextWriter&	operator<<	(const wchar_t*);
			};

			struct	ITextStream : ITextReader, ITextWriter
			{
				using ITextReader::operator>>;
				using ITextWriter::operator<<;
			};
		}
	}
}

#endif
