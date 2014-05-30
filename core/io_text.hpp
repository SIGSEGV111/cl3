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
		namespace	collection
		{
			template<class T>	struct	IStaticCollection;
		}

		namespace	text
		{
			namespace	string
			{
				class	TString;
			}

			namespace	encoding
			{
				struct	TTranscodeException;
				struct	ICodec;
				class	IEncoder;
				class	IDecoder;
			};

			struct	TUTF32
			{
				CL3PUBF const static TUTF32 TERMINATOR;	//	end-of-string marker (always code-value 0)

				u32_t code;

				inline bool operator==(TUTF32 rhs) const { return code == rhs.code; }

				CL3PUBF	CLASS	TUTF32	(char chr);
				CL3PUBF	CLASS	TUTF32	(wchar_t wchr);
				inline	CLASS	TUTF32	(u32_t code = 0U) throw() : code(code) {}
			};

			struct	TNumberFormat
			{
				enum	ESymbolPlacement
				{
					SYMBOL_PLACEMENT_BEFORE,
					SYMBOL_PLACEMENT_AFTER
				};

				//	if a specific symbol is set to TUTF32::TERMINATOR, then it will not be displayed

				collection::IStaticCollection<TUTF32>* digits;

				TUTF32 positive_mark;	//	the charcater which indicates a positive value (usually not set)
				TUTF32 negative_mark;	//	the charcater which indicates a negative value (usually '-')
				TUTF32 decimal_mark;	//	character which separates the integer part from the fractional part (usually ',' in germany and '.' in USA)
				TUTF32 grouping_mark;	//	character which seperates groups of digits (usually '.' in germany and ',' in USA)
				TUTF32 before_decimal_padding;	//	character which gets used when padding is required left of the decimal mark (usually ' ')
				TUTF32 after_decimal_padding;	//	character which gets used when padding is required right of the decimal mark (usually '0')
				TUTF32 exponential_mark;	//	character which seperates the base from the exponent (e.g.: if set to 'e' => 2.76e76)

				ESymbolPlacement positive_mark_placement;	//	specifies where to place the positive mark (usually SYMBOL_PLACEMENT_BEFORE)
				ESymbolPlacement negative_mark_placement;	//	specifies where to place the negative mark (usually SYMBOL_PLACEMENT_BEFORE)

				u16_t grouping_length;	//	length of a group of digits (usually 3 for base 10/decimal formats)
				u16_t before_decimal_length_min;	//	minimum number of characters before the decimal mark
				u16_t before_decimal_length_max;	//	maximum number of characters before the decimal mark
				u16_t after_decimal_length_min;	//	minimum number of characters after the decimal mark
				u16_t after_decimal_length_max;	//	maximum number of characters after the decimal mark
				u16_t total_length_min;	//	minimum total length
				u16_t total_length_max;	//	maximum total length

				//	some predefined commonly used formats
				CL3PUBF const static TNumberFormat OCTAL;	//	standard octal format (base 8)
				CL3PUBF const static TNumberFormat DECIMAL;	//	default values from your current operating system locale (base 10)
				CL3PUBF const static TNumberFormat HEX;		//	standard hexadecimal format (base 16)
				CL3PUBF const static TNumberFormat BASE64;	//	standard base64 format (base 64)

				struct	TPackage
				{
					//	formats for "data classes"
					//	cl3 will refer to these when printing or parsing values of the repective class
					const TNumberFormat* addresses;	//	used when printing pointers, memory addresses or file offsets (defaults to HEX)
					const TNumberFormat* integers;	//	used when printing integer numbers (defaults to DECIMAL)
					const TNumberFormat* floats;	//	used when printing floating-point numbers (defaults to DECIMAL)
				};

				CL3PUBF const static TPackage default_formats;
			};

			struct	ITextReader : public virtual stream::IIn<TUTF32>, public virtual stream::IIn<wchar_t>, public virtual stream::IIn<char>
			{
				using stream::IIn<TUTF32>::Read;
				using stream::IIn<TUTF32>::WriteOut;

				using stream::IIn<wchar_t>::Read;
				using stream::IIn<wchar_t>::WriteOut;

				using stream::IIn<char>::Read;
				using stream::IIn<char>::WriteOut;

				TNumberFormat::TPackage number_formats;

				CL3PUBF	ITextReader&	operator>>	(TUTF32&);
				CL3PUBF	ITextReader&	operator>>	(s8_t&);
				CL3PUBF	ITextReader&	operator>>	(u8_t&);
				CL3PUBF	ITextReader&	operator>>	(s16_t&);
				CL3PUBF	ITextReader&	operator>>	(u16_t&);
				CL3PUBF	ITextReader&	operator>>	(s32_t&);
				CL3PUBF	ITextReader&	operator>>	(u32_t&);
				CL3PUBF	ITextReader&	operator>>	(s64_t&);
				CL3PUBF	ITextReader&	operator>>	(u64_t&);
				CL3PUBF	ITextReader&	operator>>	(f32_t&);
				CL3PUBF	ITextReader&	operator>>	(f64_t&);
				CL3PUBF	ITextReader&	operator>>	(string::TString&);
			};

			struct	ITextWriter : public virtual stream::IOut<TUTF32>, public virtual stream::IOut<wchar_t>, public virtual stream::IOut<char>
			{
				using stream::IOut<TUTF32>::Write;
				using stream::IOut<TUTF32>::ReadIn;

				using stream::IOut<wchar_t>::Write;
				using stream::IOut<wchar_t>::ReadIn;

				using stream::IOut<char>::Write;
				using stream::IOut<char>::ReadIn;

				CL3PUBF	ITextWriter&	operator<<	(TUTF32);
				CL3PUBF	ITextWriter&	operator<<	(s8_t);
				CL3PUBF	ITextWriter&	operator<<	(u8_t);
				CL3PUBF	ITextWriter&	operator<<	(s16_t);
				CL3PUBF	ITextWriter&	operator<<	(u16_t);
				CL3PUBF	ITextWriter&	operator<<	(s32_t);
				CL3PUBF	ITextWriter&	operator<<	(u32_t);
				CL3PUBF	ITextWriter&	operator<<	(s64_t);
				CL3PUBF	ITextWriter&	operator<<	(u64_t);
				CL3PUBF	ITextWriter&	operator<<	(f32_t);
				CL3PUBF	ITextWriter&	operator<<	(f64_t);
				CL3PUBF	ITextWriter&	operator<<	(const string::TString&);
				CL3PUBF	ITextWriter&	operator<<	(const char*);
				CL3PUBF	ITextWriter&	operator<<	(const wchar_t*);
			};

			struct	ITextStream : virtual ITextReader, virtual ITextWriter
			{
				using ITextReader::operator>>;
				using ITextWriter::operator<<;
			};

			//	converts object value to human-readable string
			typedef	void (*FPrint)(ITextWriter&, const void*);
		}
	}
}

#endif
