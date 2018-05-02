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

#include "io_text.hpp"
#include "io_text_string.hpp"
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

			/******************************** TNumberFormat ******************************************************/

			CLASS	TNumberFormat::TNumberFormat	() : prefix(NULL), prefix_sign(NULL), prefix_digits(NULL), prefix_decimal(NULL), postfix(NULL), postfix_digits(NULL), postfix_sign(NULL), postfix_decimal(NULL), digits(&COLLECTION_DECIMAL_DIGITS), positive_mark(TUTF32::TERMINATOR), negative_mark('-'), zero_mark(TUTF32::TERMINATOR), decimal_mark('.'), grouping_mark(TUTF32::TERMINATOR), integer_padding(TUTF32::TERMINATOR), fractional_padding(TUTF32::TERMINATOR), exponential_mark('e'), positive_mark_placement(TNumberFormat::ESymbolPlacement::BEFORE), negative_mark_placement(TNumberFormat::ESymbolPlacement::BEFORE), zero_mark_placement(TNumberFormat::ESymbolPlacement::BEFORE), grouping_length(0), integer_length_min(0), fractional_length_min(0), fractional_length_max((u16_t)-1)
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

			CLASS	TTextIOCommon::TTextIOCommon	() : number_format(NULL) {}

			/******************************** ITextReader ******************************************************/

			template<bool b_signed, class T>
			static	void	ParseInteger	(IIn<TUTF32>& s, const TNumberFormat*, T& v)
			{
				// FIXME make use of TNumberFormat

				v = (T)0;
				TUTF32 chr;

				for(;;)
				{
					s.Read(&chr, 1);
					if(chr == ' ')
						continue;
					else
						break;
				}

				CL3_NONCLASS_ERROR(!((chr >= '0' && chr <= '9') || chr == '-'), TException, "unable to parse integer");

				bool negative = false;
				if(chr == '-')
				{
					negative = true;
					s.Read(&chr, 1);
					CL3_NONCLASS_ERROR(!(chr >= '0' && chr <= '9'), TException, "unable to parse integer");
				}

				do
				{
					if(chr >= '0' && chr <= '9')
					{
						v *= 10;
						v += (chr.code - '0');
					}
					else
						break;
				}
				while(s.Read(&chr, 1, 0) == 1);

				if(negative)
					v *= (T)-1;
			}

			template<class T>
			static	void	ParseFloat		(IIn<TUTF32>&, const TNumberFormat*, T&)
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

			static TString StrigifyUnsignedInteger(const u64_t num, const u16_t grouping_length, const TUTF32 grouping_mark, const bool allow_leading_grouping_mark, const IArray<const TUTF32>& digits)
			{
				CL3_NONCLASS_LOGIC_ERROR(digits.Count() < 2);
				const u64_t base = digits.Count();

				TUTF32 buffer[128] = {};
				TUTF32* const last = buffer + (sizeof(buffer)/sizeof(TUTF32)) - 1;
				TUTF32* out = last;

				if(num == 0)
				{
					*out-- = digits[0];
				}
				else
				{
					u16_t n_digits = 0;
					u64_t ii = num;

					for(;;)
					{
						*out-- = digits[ii % base];
						ii /= base;

						if(ii == 0)
							break;

						if(grouping_length != 0)
						{
							n_digits++;
							if((n_digits % grouping_length) == 0)
								*out-- = grouping_mark;
						}
					}

					if(allow_leading_grouping_mark)
						if(grouping_length != 0)
						{
							n_digits++;
							if((n_digits % grouping_length) == 0)
								*out-- = grouping_mark;
						}
				}

				// base  2 => 64
				// base 10 => 20
				// base 16 => 16

				return TString(out+1, last - out);
			}

			static void AddSign(TString& buffer, const bool is_negative, const TNumberFormat& nf, const TNumberFormat::ESymbolPlacement current)
			{
				if(is_negative && nf.negative_mark != TUTF32::TERMINATOR && nf.negative_mark_placement == current)
				{
					if(nf.prefix_sign != NULL)
						buffer += *nf.prefix_sign;

					buffer += nf.negative_mark;

					if(nf.postfix_sign != NULL)
						buffer += *nf.postfix_sign;
				}
				else if(!is_negative && nf.positive_mark != TUTF32::TERMINATOR && nf.positive_mark_placement == current)
				{
					if(nf.prefix_sign != NULL)
						buffer += *nf.prefix_sign;

					buffer += nf.positive_mark;

					if(nf.postfix_sign != NULL)
						buffer += *nf.postfix_sign;
				}
			}

			static TString StrigifyInteger(const u64_t num, const bool is_negative, const TNumberFormat& nf)
			{
				// <prefix_sign> <sign> <postfix_sign> | <prefix_digits> <pad> <digits with grouping marks> <postfix_digits> | <decimal mark> <prefix_decimal> <decimal> <postfix_decimal> <postfix_sign> <postfix>

				CL3_NONCLASS_ERROR(nf.digits == NULL, TException, "digits must not be NULL");

				TString buffer;

				if(nf.prefix != NULL)
					buffer += *nf.prefix;

				AddSign(buffer, is_negative, nf, TNumberFormat::ESymbolPlacement::BEFORE);

				const bool padding_is_digit = nf.digits->Contains(nf.integer_padding);
				const TString str_digits = StrigifyUnsignedInteger(num, nf.grouping_length, nf.grouping_mark, padding_is_digit, *nf.digits);

				if(str_digits.Count() < nf.integer_length_min && nf.integer_padding != TUTF32::TERMINATOR)
					buffer.Pad(POSITION_TAIL, nf.integer_length_min - str_digits.Count(), nf.integer_padding);

				AddSign(buffer, is_negative, nf, TNumberFormat::ESymbolPlacement::MIDDLE);

				if(nf.prefix_digits != NULL)
					buffer += *nf.prefix_digits;

				buffer += str_digits;

				if(nf.postfix_digits != NULL)
					buffer += *nf.postfix_digits;

				if(nf.fractional_length_min > 0)
				{
					if(nf.decimal_mark != TUTF32::TERMINATOR)
						buffer += nf.decimal_mark;

					if(nf.prefix_decimal != NULL)
						buffer += *nf.prefix_decimal;

					buffer.Pad(POSITION_TAIL, nf.fractional_length_min, (*nf.digits)[0]);

					if(nf.postfix_decimal != NULL)
						buffer += *nf.postfix_decimal;
				}

				AddSign(buffer, is_negative, nf, TNumberFormat::ESymbolPlacement::AFTER);

				if(nf.postfix != NULL)
					buffer += *nf.postfix;

				return buffer;
			}

			static void PrintNumber(IOut<TUTF32>& os, const TNumberFormat& nf, u64_t num, const bool is_negative, const bool /*is_signed*/)
			{
				os << StrigifyInteger(num, is_negative, nf);
			}

			static void PrintNumber(IOut<TUTF32>& os, const TNumberFormat&, f64_t num)
			{
// 				struct B
// 				{
// 					u64_t	s : 1,
// 							e : 11,
// 							m : 52;
// 				};
//
// 				const B& bits = *reinterpret_cast<const B*>(&num);
//
// 				const s64_t e = (s64_t)bits.e - 1023;
// 				const u64_t i = 1 + bits.m / (2 * 52);

				wchar_t buffer[128] = {};
				swprintf(buffer, 128, L"%f", num);
				os << buffer;
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
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)CL3_ABS(((s64_t)v)), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u8_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s16_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)CL3_ABS(((s64_t)v)), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u16_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s32_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)CL3_ABS(((s64_t)v)), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u32_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(s64_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)CL3_ABS(((s64_t)v)), v < 0, true);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(u64_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (u64_t)v, false, false);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f32_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (f64_t)v);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(f64_t v)
			{
				PrintNumber(*this, this->number_format ? *this->number_format : *TNumberFormat::default_format, (f64_t)v);
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

			ITextWriter&	ITextWriter::operator<<	(const void* v)
			{
				char buffer[sizeof(void*)*2+8];
				const int n = snprintf(buffer, sizeof(buffer), "%p", v);
				Write(buffer, n);
				return *this;
			}

			ITextWriter&	ITextWriter::operator<<	(const TNumberFormat* nf)
			{
				this->number_format = nf;
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

				if(this->decoder_char != NULL)
					CL3_CLASS_LOGIC_ERROR( ((n = this->decoder_char->Write((const byte_t*)arr_items_write, n_items_write_max * sizeof(char), n_items_write_min * sizeof(char))) % sizeof(char)) != 0 );
				else
					n = this->Sink()->Write((const byte_t*)arr_items_write, n_items_write_max, n_items_write_min);

				return n;
			}

			usys_t	TTextWriter::Write	(const wchar_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
			{
				if(n_items_write_min == (usys_t)-1) n_items_write_min = n_items_write_max;
				usys_t n;

				if(this->decoder_wchar != NULL)
					CL3_CLASS_LOGIC_ERROR( ((n = this->decoder_wchar->Write((const byte_t*)arr_items_write, n_items_write_max * sizeof(wchar_t), n_items_write_min * sizeof(wchar_t))) % sizeof(wchar_t)) != 0 );
				else
					n = this->Sink()->Write((const byte_t*)arr_items_write, n_items_write_max, n_items_write_min);

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

			CLASS	TTextWriter::TTextWriter	(const encoding::ICodec* target_encoding)
			{
				this->encoder = target_encoding->CreateEncoder();

				if(target_encoding != encoding::CODEC_CXX_CHAR)
				{
					this->decoder_char = encoding::CODEC_CXX_CHAR->CreateDecoder();
					this->decoder_char->Sink(this->encoder.Object());
				}

				if(target_encoding != encoding::CODEC_CXX_WCHAR)
				{
					this->decoder_wchar = encoding::CODEC_CXX_WCHAR->CreateDecoder();
					this->decoder_wchar->Sink(this->encoder.Object());
				}
			}
		}
	}
}
