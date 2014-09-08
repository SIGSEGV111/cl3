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

#include "io_text_encoding_utf8.hpp"
#include "io_text_string.hpp"

extern "C" void* memcpy(void* dest, const void* src, size_t n) throw();
extern "C" void* memmove(void* dest, const void* src, size_t n) throw();

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

					/*******************************************************************************/

					CLASS	TUTF8Encoder::TUTF8Encoder		() : sink(NULL), shift(0U) {}
					CLASS	TUTF8Encoder::~TUTF8Encoder		()	{ CL3_CLASS_ERROR(this->shift != 0, TTranscodeException, CODEC_UTF8, DIRECTION_ENCODE, REASON_INCOMPLETE, this->shift, 0); }

					void			TUTF8Encoder::Sink		(IOut<byte_t>* os)	{ sink = os; }
					IOut<byte_t>*	TUTF8Encoder::Sink		() const			{ return sink; }

					const ICodec*	TUTF8Encoder::Codec		() const	{ return CODEC_UTF8; }
					void			TUTF8Encoder::Reset		()			{ this->shift = 0U; }
					bool			TUTF8Encoder::IsDirty	() const	{ return this->shift != 0; }

					usys_t			TUTF8Encoder::Write		(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
					{
						if(n_items_write_min == (usys_t)-1)
							n_items_write_min = n_items_write_max;

						CL3_CLASS_ERROR(this->sink == NULL, TException, "Sink() must point to a valid sink");

						//	flush global state
						if(this->shift != 0)
						{
							const usys_t r = this->sink->Write(state, this->shift, n_items_write_min > 0 ? this->shift : 0);
							if(r == 0) return 0;	//	we couldn't even write somthing from the global state, so we don't even need to try to write more from the actual data

							memmove(this->state, this->state + r, this->shift - r);
							this->shift -= r;

							if(this->shift != 0)	//	we weren't able to completly clear the global state, so we cannot proceed with the actual data (we have to preserve the order, and it is unlikly that the sink will suddenly accept more data)
								return r;
						}
						//	state is cleared, continue with processing actual data...

						usys_t n_out = 0;
						usys_t i = 0;
						for(; i < n_items_write_max; i++)
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

							if(i < n_items_write_min)
								this->sink->Write(arr_out, length);
							else
							{
								const usys_t r = this->sink->Write(arr_out, length, 0);
								if(r < length)
								{
									if(r == 0)
										break;	//	sink full
									else
									{
										//	put the remaining bytes into the global state
										this->shift = length - r;
										memcpy(this->state, arr_out + r, length - r);
									}
								}
							}
							n_out += length;
						}

						return i;
					}

					/*******************************************************************************/

					CLASS	TUTF8Decoder::TUTF8Decoder		() : sink(NULL), shift(0U), state(0U) {}
					CLASS	TUTF8Decoder::~TUTF8Decoder		()	{ CL3_CLASS_ERROR(this->shift != 0, TTranscodeException, CODEC_UTF8, DIRECTION_DECODE, REASON_INCOMPLETE, this->shift, 0); }

					void			TUTF8Decoder::Sink		(IOut<TUTF32>* os) { sink = os; }
					IOut<TUTF32>*	TUTF8Decoder::Sink		() const { return sink; }

					const ICodec*	TUTF8Decoder::Codec		() const	{ return CODEC_UTF8; }
					void			TUTF8Decoder::Reset		()			{ this->shift = 0U; }
					bool			TUTF8Decoder::IsDirty	() const	{ return this->shift != 0; }

					usys_t			TUTF8Decoder::Write		(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
					{
						if(n_items_write_min == (usys_t)-1)
							n_items_write_min = n_items_write_max;

						CL3_CLASS_ERROR(this->sink == NULL, TException, "Sink() must point to a valid sink");

						unsigned local_shift = this->shift;
						u32_t local_state = this->state;

						usys_t n_out = 0;
						usys_t i = 0;
						for(; i < n_items_write_max; i++)
						{
							const u32_t b = arr_items_write[i];

							gt_again:;
							if(local_shift == 0)
							{
								if((b & 0x80) == 0x00)	//	1-byte-codes
								{
									local_state = b;
									if(i < n_items_write_min)
										this->sink->Write((const TUTF32*)&local_state, 1);
									else if(this->sink->Write((const TUTF32*)&local_state, 1, 0) == 0)
										break;
									n_out++;
								}
								else if((b & 0xE0) == 0xC0)	//	2-byte-codes
								{
									local_shift = 1;
									local_state = (b & 0x1F);
								}
								else if((b & 0xF0) == 0xE0)	//	3-byte-codes
								{
									local_shift = 2;
									local_state = (b & 0x0F);
								}
								else if((b & 0xF8) == 0xF0)	//	4-byte-codes
								{
									local_shift = 3;
									local_state = (b & 0x07);
								}
								else	//	errors...
								{
									this->shift = local_shift;
									this->state = local_state;
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
									if(i < n_items_write_min)
									{
										//	normal procedure...
										local_shift--;
										local_state <<= 6;
										local_state |= (b & 0x3F);

										if(local_shift == 0)
											this->sink->Write((const TUTF32*)&local_state, 1);
									}
									else
									{
										//	check if last byte of sequence
										if(local_shift == 1)
										{
											//	this is the last byte of the sequence
											//	we already processed n_items_write_min items, so this byte is optional,
											//	we will only accept it, if we can write the resulting UTF-32 character to the sink
											//	make a backup copy of the local_*
											const unsigned backup_shift = local_shift;
											const u32_t backup_state = local_state;

											//	process the byte as we normally would
											local_shift--;
											local_state <<= 6;
											local_state |= (b & 0x3F);

											//	try to write the UTF-32 character to the sink
											if(this->sink->Write((const TUTF32*)&local_state, 1, 0) == 0)
											{
												//	the sink did not accept the character >= rollback by loading global state from backup and exit
												this->shift = backup_shift;
												this->state = backup_state;
												return i;
											}
											//	else	//	the sink accepted the character, continue normally...
										}
										else
										{
											//	just another follow byte
											local_shift--;
											local_state <<= 6;
											local_state |= (b & 0x3F);
										}
									}
								}
								else	//	errors...
								{
									this->shift = local_shift;
									this->state = local_state;
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
						}

						this->shift = local_shift;
						this->state = local_state;

						return i;
					}

					/*******************************************************************************/

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
