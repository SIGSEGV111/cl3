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
					class	CL3PUBT	TUTF8Encoder : public virtual IEncoder
					{
						protected:
							stream::IOut<byte_t>* sink;
							unsigned shift;
							byte_t state[4];

						public:
							//	from IXCoder
							CL3PUBF	const ICodec*	Codec	() const final override CL3_GETTER;
							CL3PUBF	void			Reset	() final override;
							CL3PUBF	bool			IsDirty	() const final override CL3_GETTER;

							//	from IOut<TUTF32>
							using IOut<TUTF32>::Write;
							CL3PUBF	usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

							//	from ISource<byte_t>
							CL3PUBF	void	Sink	(stream::IOut<byte_t>* os) final override CL3_SETTER;
							CL3PUBF	stream::IOut<byte_t>*	Sink	() const final override CL3_GETTER;

							//	from TUTF8Encoder
							CL3PUBF	CLASS	TUTF8Encoder	();
// 							CL3PUBF	CLASS	~TUTF8Encoder	();
					};

					class	CL3PUBT	TUTF8Decoder : public virtual IDecoder
					{
						protected:
							stream::IOut<TUTF32>* sink;
							unsigned shift;
							u32_t state;

						public:
							//	from IXCoder
							CL3PUBF	const ICodec*	Codec	() const final override CL3_GETTER;
							CL3PUBF	void			Reset	() final override;
							CL3PUBF	bool			IsDirty	() const final override CL3_GETTER;

							//	from IOut<byte_t>
							using IOut<byte_t>::Write;
							CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT;

							//	from ISource<TUTF32>
							CL3PUBF	void	Sink	(stream::IOut<TUTF32>* os) final override CL3_SETTER;
							CL3PUBF	stream::IOut<TUTF32>*	Sink	() const final override CL3_GETTER;

							//	from TUTF8Decoder
							CL3PUBF	CLASS	TUTF8Decoder	();
// 							CL3PUBF	CLASS	~TUTF8Decoder	();
					};

					class	CL3PUBT	TUTF8Codec : public ICodec
					{
						public:
							//	from ICodec
							CL3PUBF	string::TString	Name	() const final override CL3_GETTER;
							CL3PUBF	system::memory::TUniquePtr<IEncoder>	CreateEncoder	() const final override CL3_WARN_UNUSED_RESULT;
							CL3PUBF	system::memory::TUniquePtr<IDecoder>	CreateDecoder	() const final override CL3_WARN_UNUSED_RESULT;
							CLASS	TUTF8Codec	() {}
					};

					CL3PUBF extern const TUTF8Codec* const CODEC_UTF8;
				}
			}
		}
	}
}

#endif
