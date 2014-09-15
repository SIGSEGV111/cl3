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

#include "io_text_encoding.hpp"
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
				using namespace error;

				struct	TUTF32Encoder : IEncoder
				{
					stream::IOut<byte_t>* sink;

					//	from IXCoder
					const ICodec*	Codec	() const final override CL3_GETTER { return CODEC_UTF32; }
					void			Reset	() final override {}
					bool			IsDirty	() const final override CL3_GETTER { return false; }

					//	from IOut<TUTF32>
					usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
					{
						CL3_CLASS_ERROR(this->sink == NULL, TException, "Sink() must point to a valid sink");
						if(n_items_write_min == (usys_t)-1)
							n_items_write_min = n_items_write_max;
						return sink->Write((const byte_t*)arr_items_write, n_items_write_max * 4, n_items_write_min * 4) / 4;
					}

					//	from ISource<byte_t>
					void	Sink	(stream::IOut<byte_t>* os) CL3_SETTER { this->sink = os; }
					stream::IOut<byte_t>*	Sink	() const CL3_GETTER { return this->sink; }
				};

				struct	TUTF32Decoder : IDecoder
				{
					stream::IOut<TUTF32>* sink;

					//	from IXCoder
					const ICodec*	Codec	() const final override CL3_GETTER { return CODEC_UTF32; }
					void			Reset	() final override {}
					bool			IsDirty	() const final override CL3_GETTER { return false; }

					//	from IOut<byte_t>
					usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
					{
						CL3_CLASS_ERROR(this->sink == NULL, TException, "Sink() must point to a valid sink");
						if(n_items_write_min == (usys_t)-1)
							n_items_write_min = n_items_write_max;
						return sink->Write((const TUTF32*)arr_items_write, n_items_write_max / 4, n_items_write_min / 4) * 4;
					}

					//	from ISource<TUTF32>
					void	Sink	(stream::IOut<TUTF32>* os) CL3_SETTER { this->sink = os; }
					stream::IOut<TUTF32>*	Sink	() const CL3_GETTER { return this->sink; }
				};

				struct	TUTF32Codec : ICodec
				{
					string::TString	Name	() const { return "utf32"; }
					system::memory::TUniquePtr<IEncoder>	CreateEncoder	() const { return system::memory::MakeUniquePtr<IEncoder>(new TUTF32Encoder()); }
					system::memory::TUniquePtr<IDecoder>	CreateDecoder	() const { return system::memory::MakeUniquePtr<IDecoder>(new TUTF32Decoder()); }
					CLASS	TUTF32Codec	() {}
				};

				static const TUTF32Codec _CODEC_UTF32;
				const ICodec* const CODEC_UTF32 = &_CODEC_UTF32;

				static const char* ReasonCodeToString(EReason reason)
				{
					switch(reason)
					{
						case REASON_INCOMPLETE:
							return "input was incomplete";
						case REASON_NOT_REPRESENTABLE:
							return "input was unrepresentable in the target encoding";
						case REASON_INVALID:
							return "input was invalid";
						default:
							CL3_NONCLASS_FAIL(TException, "invalid reason code");
					}
				}

				CLASS	TTranscodeException::TTranscodeException(
						const ICodec* codec,
						EDirection direction,
						EReason reason,
						usys_t n_input_items_processed,
						usys_t n_output_items_written
					)
					: TException(
							"unable to transcode from %s to %s because %s after processing %lu input items into %lu output items",
							(direction == DIRECTION_ENCODE ? "utf32" : string::TCString(codec->Name(), CODEC_CXX_CHAR).Chars()),
							(direction == DIRECTION_ENCODE ? string::TCString(codec->Name(), CODEC_CXX_CHAR).Chars() : "utf32"),
							ReasonCodeToString(reason),
							n_input_items_processed,
							n_output_items_written
						),
	  					codec(codec),
						direction(direction),
						reason(reason),
						n_input_items_processed(n_input_items_processed),
						n_output_items_written(n_output_items_written),
						action(ERRORACTION_ABORT)
				{}


				CLASS	TTranscodeException::TTranscodeException	(TTranscodeException&& other) : TException(static_cast<TException&&>(other)), codec(other.codec), direction(other.direction), reason(other.reason), n_input_items_processed(other.n_input_items_processed), n_output_items_written(other.n_output_items_written), action(other.action)
				{}

				CLASS	TTranscodeException::~TTranscodeException	()
				{
				}

				usys_t	ACharDecoder::Write		(const char* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					if(this->decoder.Object() == NULL)
					{
						system::memory::TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
						if(this->decoder.AtomicSwap(NULL, d.Object()) == NULL)
						{
							d.Reset();
							this->decoder.Object()->Sink(this);
						}
					}

					return this->decoder.Object()->Write((const byte_t*)arr_items_write, n_items_write_max, n_items_write_min);
				}

				usys_t	AWCharDecoder::Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					if(CODEC_CXX_WCHAR == CODEC_UTF32)
					{
						return this->Write((const TUTF32*)arr_items_write, n_items_write_max, n_items_write_min);
					}
					else
					{
						if(this->decoder.Object() == NULL)
						{
							system::memory::TUniquePtr<IDecoder> d = CODEC_CXX_WCHAR->CreateDecoder();
							if(this->decoder.AtomicSwap(NULL, d.Object()) == NULL)
							{
								d.Reset();
								this->decoder.Object()->Sink(this);
							}
						}

						return this->decoder.Object()->Write((const byte_t*)arr_items_write, n_items_write_max, n_items_write_min);
					}
				}

				usys_t	ACharEncoder::Read		(char* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	AWCharEncoder::Read		(wchar_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}
