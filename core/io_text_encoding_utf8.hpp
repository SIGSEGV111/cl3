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

#ifndef	_include_cl3_core_io_text_encoding_utf8_hpp_
#define	_include_cl3_core_io_text_encoding_utf8_hpp_

#include "io_text_encoding.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	encoding
			{
				namespace	utf8
				{
					class	TUTF8Encoder : public IEncoder
					{
						protected:
							stream::IOut<byte_t>* sink;

						public:
							//	from IEncoder
							CL3PUBF	void	Reset	();
							CL3PUBF	void	Write	(const TUTF32* arr_items_write, usys_t n_items_write);

							//	from ISource<byte_t>
							CL3PUBF	void	Sink	(stream::IOut<byte_t>* os) CL3_SETTER;
							CL3PUBF	stream::IOut<byte_t>*	Sink	() const CL3_GETTER;

							//	from TUTF8Encoder
							CL3PUBF	CLASS	TUTF8Encoder	();
					};

					class	TUTF8Decoder : public IDecoder
					{
						protected:
							stream::IOut<TUTF32>* sink;
							int shift;
							u32_t state;

						public:
							//	from IDecoder
							CL3PUBF	void	Reset	();
							CL3PUBF	void	Write	(const byte_t* arr_items_write, usys_t n_items_write);

							//	from ISource<TUTF32>
							CL3PUBF	void	Sink	(stream::IOut<TUTF32>* os) CL3_SETTER;
							CL3PUBF	stream::IOut<TUTF32>*	Sink	() const CL3_GETTER;

							//	from TUTF8Decoder
							CL3PUBF	CLASS	TUTF8Decoder	();
							CL3PUBF	CLASS	~TUTF8Decoder	();
					};

					class	TUTF8Codec : public ICodec
					{
						public:
							//	from ICodec
							CL3PUBF	string::TString	Name	() const;
							CL3PUBF	system::memory::TUniquePtr<IEncoder>	CreateEncoder	() const;
							CL3PUBF	system::memory::TUniquePtr<IDecoder>	CreateDecoder	() const;
							CLASS	TUTF8Codec	() {}
					};

					CL3PUBF extern const TUTF8Codec* const CODEC_UTF8;
				}
			}
		}
	}
}

#endif
