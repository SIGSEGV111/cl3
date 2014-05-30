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

#include "io_text_encoding_utf8.hpp"
#include "io_text_string.hpp"

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
					using namespace stream;
					using namespace error;

					void			TUTF8Encoder::Reset	()
					{
						//	nothing to do here
					}

					usys_t			TUTF8Encoder::Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t)
					{
						CL3_CLASS_ERROR(this->sink == NULL, TException, "Sink() must point to a valid sink");
						usys_t n_out = 0;
						for(usys_t i = 0; i < n_items_write_max; i++)
						{
							const u32_t u = arr_items_write[i].code;
							byte_t arr_out[4];
							byte_t length;

							if(u < 128U)	//	1-byte-codes
							{
								arr_out[0] = (byte_t)u;
								length = 1;
							}
							else if(u < 2048U)	//	2-byte-codes
							{
								arr_out[0] = ((u & 0x7C0) >> 6) | 0xC0;
								arr_out[1] = (u & 0x3F) | 0x80;
								length = 2;
							}
							else if(u < 65536U)	//	3-byte-codes
							{
								arr_out[0] = ((u & 0xF000) >> 12) | 0xE0;
								arr_out[1] = ((u & 0xFC0) >> 6) | 0x80;
								arr_out[2] = (u & 0x3F) | 0x80;
								length = 3;
							}
							else if(u < 2097152U)	//	4-byte-codes
							{
								arr_out[0] = ((u & 0x1C0000) >> 18) | 0xF0;
								arr_out[1] = ((u & 0x3F000) >> 12) | 0x80;
								arr_out[2] = ((u & 0xFC0) >> 6) | 0x80;
								arr_out[3] = (u & 0x3F) | 0x80;
								length = 4;
							}
							else	//	errors...
							{
								TTranscodeException e(CODEC_UTF8, DIRECTION_ENCODE, REASON_INVALID, i, n_out);
								on_error.Raise(*this, e);
								switch(e.action)
								{
									case ERRORACTION_CONTINUE:
										continue;
									case ERRORACTION_ABORT:
										CL3_CLASS_FAIL(TTranscodeException, CODEC_UTF8, DIRECTION_ENCODE, REASON_INVALID, i, n_out);
										break;
								}
							}

							this->sink->Write(arr_out, length);
							n_out += length;
						}

						return n_items_write_max;
					}

					void			TUTF8Encoder::Sink	(IOut<byte_t>* os)
					{
						sink = os;
					}

					IOut<byte_t>*	TUTF8Encoder::Sink	() const
					{
						return sink;
					}

					CLASS	TUTF8Encoder::TUTF8Encoder	() : sink(NULL) {}

					void			TUTF8Decoder::Reset	()
					{
						this->shift = 0;
						this->state = 0U;
					}

					usys_t			TUTF8Decoder::Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t)
					{
						CL3_CLASS_ERROR(this->sink == NULL, TException, "Sink() must point to a valid sink");

						usys_t n_out = 0;
						for(usys_t i = 0; i < n_items_write_max; i++)
						{
							const u32_t b = arr_items_write[i];

							gt_again:;
							if(this->shift == 0)
							{
								if((b & 0xF8) == 0xF0)
								{
									this->shift = 3;
									this->state = (b & 0x07);
								}
								else if((b & 0xF0) == 0xE0)
								{
									this->shift = 2;
									this->state = (b & 0x0F);
								}
								else if((b & 0xE0) == 0xC0)
								{
									this->shift = 1;
									this->state = (b & 0x1F);
								}
								else if((b & 0x80) == 0x00)
								{
									this->shift = 0;
									this->state = b;
								}
								else	//	errors...
								{
									TTranscodeException e(CODEC_UTF8, DIRECTION_DECODE, REASON_INVALID, i, n_out);
									on_error.Raise(*this, e);
									switch(e.action)
									{
										case ERRORACTION_CONTINUE:
											continue;
										case ERRORACTION_ABORT:
											CL3_CLASS_FAIL(TTranscodeException, CODEC_UTF8, DIRECTION_DECODE, REASON_INVALID, i, n_out);
											break;
									}
								}
							}
							else
							{
								if((b & 0xC0) == 0x80)
								{
									this->shift--;
									this->state <<= 6;
									this->state |= (b & 0x3F);
								}
								else	//	errors...
								{
									TTranscodeException e(CODEC_UTF8, DIRECTION_DECODE, REASON_INVALID, i, n_out);
									on_error.Raise(*this, e);
									switch(e.action)
									{
										case ERRORACTION_CONTINUE:
											Reset();
											goto gt_again;
										case ERRORACTION_ABORT:
											CL3_CLASS_FAIL(TTranscodeException, CODEC_UTF8, DIRECTION_DECODE, REASON_INVALID, i, n_out);
											break;
									}
								}
							}

							if(this->shift == 0)
							{
								this->sink->Write((const TUTF32*)&state, 1);
								n_out++;
							}
						}

						return n_items_write_max;
					}

					void			TUTF8Decoder::Sink	(IOut<TUTF32>* os)
					{
						sink = os;
					}

					IOut<TUTF32>*	TUTF8Decoder::Sink	() const
					{
						return sink;
					}

					CLASS	TUTF8Decoder::TUTF8Decoder	() : sink(NULL), shift(0), state(0U) {}
					CLASS	TUTF8Decoder::~TUTF8Decoder	()
					{
						CL3_CLASS_ERROR(this->shift != 0, TTranscodeException, CODEC_UTF8, DIRECTION_DECODE, REASON_INCOMPLETE, this->shift, 0);
					}

					string::TString	TUTF8Codec::Name	() const
					{
						return "utf8";
					}

					system::memory::TUniquePtr<IEncoder>	TUTF8Codec::CreateEncoder	() const
					{
						return system::memory::MakeUniquePtr<IEncoder>(new TUTF8Encoder());
					}

					system::memory::TUniquePtr<IDecoder>	TUTF8Codec::CreateDecoder	() const
					{
						return system::memory::MakeUniquePtr<IDecoder>(new TUTF8Decoder());
					}

					static const TUTF8Codec _CODEC_UTF8;
					const TUTF8Codec* const CODEC_UTF8 = &_CODEC_UTF8;
				}
			}
		}
	}
}
