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

#include "io_text.hpp"
#include "io_text_string.hpp"
#include "io_text_parser.hpp"
#include "io_text_encoding.hpp"
#include "error.hpp"
#include "io_collection_array.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	text
		{
			using namespace stream;
			using namespace string;
			using namespace collection::array;
			using namespace error;

			/******************************** Constants and Variables ******************************************************/

			static const TUTF32 ARR_WHITESPACE_DEFAULT[] = { 0x0020U, 0x0009U, 0x000AU, 0x000CU, 0x000DU, 0x000BU };
			static const collection::array::TArray<const TUTF32> COLLECTION_WHITESPACE_DEFAULT(ARR_WHITESPACE_DEFAULT, sizeof(ARR_WHITESPACE_DEFAULT) / sizeof(TUTF32), false);
			const collection::IStaticCollection<const TUTF32>* whitespace = &COLLECTION_WHITESPACE_DEFAULT;

			const collection::IStaticCollection<const TUTF32>* eos_markers = &COLLECTION_WHITESPACE_DEFAULT;

			static const TUTF32 ARR_NEWLINE_DEFAULT[] = { 0x000AU };
			static const collection::array::TArray<const TUTF32> COLLECTION_NEWLINE_DEFAULT(ARR_NEWLINE_DEFAULT, sizeof(ARR_NEWLINE_DEFAULT) / sizeof(TUTF32), false);
			const collection::IStaticCollection<const TUTF32>* newline_markers = &COLLECTION_NEWLINE_DEFAULT;

			static const TUTF32 ARR_OCTAL_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7' };
			static const collection::array::TArray<const TUTF32> COLLECTION_OCTAL_DIGITS(ARR_OCTAL_DIGITS, sizeof(ARR_OCTAL_DIGITS) / sizeof(TUTF32), false);

			static const TUTF32 ARR_DECIMAL_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
			static const collection::array::TArray<const TUTF32> COLLECTION_DECIMAL_DIGITS(ARR_DECIMAL_DIGITS, sizeof(ARR_DECIMAL_DIGITS) / sizeof(TUTF32), false);

			static const TUTF32 ARR_HEX_DIGITS[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
			static const collection::array::TArray<const TUTF32> COLLECTION_HEX_DIGITS(ARR_HEX_DIGITS, sizeof(ARR_HEX_DIGITS) / sizeof(TUTF32), false);

			CL3_CONTEXT_VARIABLE_IMPL(const TCollationSequence*, default_collation_sequence, NULL);

			/******************************** TUTF32 ******************************************************/

			const TUTF32 TUTF32::TERMINATOR((u32_t)0);

			usys_t	TUTF32::strlen	(const TUTF32* str)
			{
				const TUTF32* p = str;
				for(; p->code != 0; p++);
				return p - str;
			}

			CLASS	TUTF32::TUTF32	(char chr)
			{
				if((unsigned char)chr < 128)
					code = chr;
				else
					CL3_NOT_IMPLEMENTED;
			}

			CLASS	TUTF32::TUTF32	(wchar_t wchr)
			{
				if(encoding::CODEC_CXX_WCHAR == encoding::CODEC_UTF32)
				{
					code = wchr;
				}
				else
				{
					if(wchr < 128)
						code = wchr;
					else
						CL3_NOT_IMPLEMENTED;
				}
			}

			CLASS	TTextFormat::TTextFormat	() :	eos_markers(text::eos_markers), /*max_string_length((usys_t)-1),*/ discard_eos_marker(true)
			{
			}

			/******************************** TNumberFormat ******************************************************/

			CLASS	TNumberFormat::TNumberFormat	() : prefix_sign(NULL), prefix_digits(NULL), prefix_decimal(NULL), postfix_digits(NULL), postfix_sign(NULL), postfix_decimal(NULL), digits(&COLLECTION_DECIMAL_DIGITS), positive_mark(TUTF32::TERMINATOR), negative_mark('-'), zero_mark(TUTF32::TERMINATOR), decimal_mark('.'), grouping_mark(TUTF32::TERMINATOR), integer_padding(TUTF32::TERMINATOR), fractional_padding(TUTF32::TERMINATOR), exponential_mark('e'), positive_mark_placement(TNumberFormat::SYMBOL_PLACEMENT_BEFORE), negative_mark_placement(TNumberFormat::SYMBOL_PLACEMENT_BEFORE), zero_mark_placement(TNumberFormat::SYMBOL_PLACEMENT_BEFORE), grouping_length(0), integer_length_min(0), fractional_length_min(0), fractional_length_max((u16_t)-1)
			{
			}

			static	TNumberFormat	MakeStdHexNumberFormat	()
			{
				TNumberFormat format;
				format.digits = &COLLECTION_HEX_DIGITS;
				return format;
			}

			const TNumberFormat TNumberFormat::DECIMAL_SIMPLE;
			const TNumberFormat TNumberFormat::HEX = MakeStdHexNumberFormat();
			const TNumberFormat* TNumberFormat::default_format = &TNumberFormat::DECIMAL_SIMPLE;

			/******************************** TTextIOCommon ******************************************************/

			CLASS	TTextIOCommon::TTextIOCommon	() : text_format(NULL), number_format(NULL) {}

			/******************************** ITextReader ******************************************************/

			template<bool b_signed, class T>
			static	void	ParseInteger	(IIn<TUTF32>& is, const TNumberFormat* format, T& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			template<class T>
			static	void	ParseFloat		(IIn<TUTF32>& is, const TNumberFormat* format, T& v)
			{
				CL3_NOT_IMPLEMENTED;
			}

			ITextReader&	ITextReader::operator>>	(char& v)
			{
				Read(&v, 1);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(wchar_t& v)
			{
				Read(&v, 1);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(TUTF32& v)
			{
				Read(&v, 1);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s8_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u8_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s16_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u16_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s32_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u32_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(s64_t& v)
			{
				ParseInteger<true>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(u64_t& v)
			{
				ParseInteger<false>(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f32_t& v)
			{
				ParseFloat(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(f64_t& v)
			{
				ParseFloat(*this, this->number_format, v);
				return *this;
			}

			ITextReader&	ITextReader::operator>>	(string::TString& v)
			{
				TUTF32 chr;
				v.Clear();
				while(this->Read(&chr, 1, 0) != 0)
				{
					if(chr == '\n')
					{
						if(v.Count() > 0 && v[-1] == TUTF32(10U))
							v.Cut(0,-1);
						break;
					}
					v += chr;
				}
				return *this;
			}

			/******************************** ITextWriter ******************************************************/

			static	void	PrintInteger(TString& buffer, const u64_t i, const IArray<const TUTF32>& digits)
			{
				const usys_t base = digits.Count();

				if(i == 0)
				{
					buffer.Append(digits[0]);
				}
				else
				{
					unsigned n_digits_integer = 1;
					{
						u64_t ii = i;
						while(ii >= base)
						{
							ii /= base;
							n_digits_integer++;
						}
					}

					{
						buffer.Count(buffer.Count() + n_digits_integer);
						TUTF32* out = buffer.ItemPtr(buffer.Count()-1);
						u64_t ii = i;
						while(ii != 0)
						{
							*out-- = digits[ii % base];
							ii /= base;
						}
					}
				}
			}

			static	void	PrintNumber	(IOut<TUTF32>& os, const bool negative, const u64_t i, const s64_t e, const TNumberFormat& f)
			{
				//	<+|-><pad><integer><decimal-mark><fractional><pad><+|->

				CL3_NONCLASS_ERROR(f.digits == NULL, TException, "TNumberFormat::digits must be non-NULL");
				CL3_NONCLASS_ERROR(f.digits->Count() < 2, TException, "TNumberFormat::digits must contain at least 2 digits");

				TString buffer;

				if(f.prefix_sign)
					buffer<<*f.prefix_sign;

				if(i != 0 && !negative && f.positive_mark != TUTF32::TERMINATOR && f.positive_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_BEFORE)
					buffer<<f.positive_mark;
				else if(i != 0 && negative && f.negative_mark != TUTF32::TERMINATOR && f.negative_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_BEFORE)
					buffer<<f.negative_mark;
				else if(i == 0 && f.zero_mark != TUTF32::TERMINATOR && f.zero_mark_placement == TNumberFormat::SYMBOL_PLACEMENT_BEFORE)
					buffer<<f.zero_mark;

				if(f.prefix_digits)
					buffer<<*f.prefix_digits;

				PrintInteger(buffer, i, *f.digits);

				fprintf(stderr, "buffer = \"%s\"\n", TCString(buffer, encoding::CODEC_CXX_CHAR).Chars());

				os<<buffer;
			}

			static	void	PrintNumber	(IOut<TUTF32>& os, const TNumberFormat& f, u64_t num, bool negative, bool is_signed)
			{
				PrintNumber(os, negative, num, 1, f);
			}

			static	void	PrintNumber	(IOut<TUTF32>& os, const TNumberFormat& f, f64_t num)
			{
				struct B
				{
					u64_t	s : 1,
							e : 11,
							m : 52;
				};

				const B& bits = *reinterpret_cast<const B*>(&num);

				const s64_t e = (s64_t)bits.e - 1023;
				const u64_t i = 1 + bits.m / (2 * 52);

				PrintNumber(os, bits.s != 0, i, e, f);
			}

			ITextWriter&	ITextWriter::operator<<	(char v)
			{
				Write(&v, 1);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(wchar_t v)
			{
				Write(&v, 1);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(TUTF32 v)
			{
				Write(&v, 1);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s8_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, CL3_ABS(v), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u8_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s16_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, CL3_ABS(v), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u16_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s32_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, CL3_ABS(v), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u32_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s64_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, CL3_ABS(v), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u64_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f32_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f64_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const string::TString& str)
			{
				if(str.Count() > 0)
					this->Write(str.ItemPtr(0), str.Count());
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const char* v)
			{
				CL3_CLASS_ERROR(v == NULL, TException, "v must point to a valid null-terminated c-string");
				Write(v, strlen(v));
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const wchar_t* v)
			{
				CL3_CLASS_ERROR(v == NULL, TException, "v must point to a valid null-terminated wide-string");
				Write(v, wcslen(v));
				return *this;
			}

			/******************************** TTextWriter ******************************************************/

			void	TTextWriter::Flush	()
			{
				this->decoder_char->Flush();
				this->decoder_wchar->Flush();
				this->encoder->Flush();
			}

			usys_t	TTextWriter::Space	() const
			{
				return ((this->encoder->Sink() == NULL) ? 0 : (this->encoder->Sink()->Space() == 0 ? 0 : (usys_t)-1));
			}

			usys_t	TTextWriter::Write	(const char*    arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				if(n_items_write_min == (usys_t)-1) n_items_write_min = n_items_write_max;
				usys_t n;
				CL3_CLASS_LOGIC_ERROR( ((n = this->decoder_wchar->Write((const byte_t*)arr_items_write, n_items_write_max * sizeof(char), n_items_write_min * sizeof(char))) % sizeof(char)) != 0 );
				return n;
			}

			usys_t	TTextWriter::Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				if(n_items_write_min == (usys_t)-1) n_items_write_min = n_items_write_max;
				usys_t n;
				CL3_CLASS_LOGIC_ERROR( ((n = this->decoder_wchar->Write((const byte_t*)arr_items_write, n_items_write_max * sizeof(wchar_t), n_items_write_min * sizeof(wchar_t))) % sizeof(wchar_t)) != 0 );
				return n;
			}

			usys_t	TTextWriter::Write	(const TUTF32*  arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				return this->encoder->Write(arr_items_write, n_items_write_max, n_items_write_min);
			}

			void	TTextWriter::Sink	(IOut<byte_t>* os)
			{
				this->encoder->Sink(os);
			}

			IOut<byte_t>*
					TTextWriter::Sink	() const
			{
				return this->encoder->Sink();
			}

			CLASS	TTextWriter::TTextWriter	(const encoding::ICodec* codec)
			{
				this->encoder = codec->CreateEncoder();

				if(codec != encoding::CODEC_CXX_CHAR)
				{
					this->decoder_char = encoding::CODEC_CXX_CHAR->CreateDecoder();
					this->decoder_char->Sink(this->encoder.Object());
				}

				if(codec != encoding::CODEC_CXX_WCHAR)
				{
					this->decoder_wchar = encoding::CODEC_CXX_WCHAR->CreateDecoder();
					this->decoder_wchar->Sink(this->encoder.Object());
				}
			}
		}
	}
}
