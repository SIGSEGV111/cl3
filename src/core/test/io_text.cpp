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

#include "common.hpp"
#include <cl3/core/system_compiler.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/system_types_typeinfo.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_collection_array.hpp>
#include <cl3/core/io_text.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/io_text_encoding_utf8.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::unittest_support;
	using namespace cl3::io::stream;
	using namespace cl3::io::collection::array;
	using namespace cl3::system::types;
	using namespace cl3::system::memory;
	using namespace cl3::io::text;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace cl3::io::text::encoding::utf8;
	using namespace cl3::io::collection::list;
	using namespace cl3::io::collection;
	using namespace cl3::system::types::typeinfo;

	TEST(io_text_encoding_utf8, Codec)
	{
		EXPECT_TRUE(CODEC_UTF8 != NULL);

		TUniquePtr<IEncoder> e = CODEC_UTF8->CreateEncoder();
		TUniquePtr<IDecoder> d = CODEC_UTF8->CreateDecoder();

		EXPECT_TRUE(e.Object() != NULL);
		EXPECT_TRUE(d.Object() != NULL);
	}

	TEST(io_text_encoding_utf8, Decode)
	{
		TUTF8Decoder d;
		TList<TUTF32> o;
		d.Sink(&o);

		const u32_t arr_expected[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9',0x00E4U,0x00F6U,0x00FCU,'+','#','-',0x20ACU,'!',0x1D11EU,'&',0x1D11EU};

		d.Write((const byte_t*)"abcdefghijklmnopqrstuvwxyz0123456789äöü+#-€!\xF0\x9D\x84\x9E&", 54);
		EXPECT_TRUE(o.Count() == 46);

		d.Write((const byte_t*)"\xF0\x9D", 2);
		EXPECT_TRUE(o.Count() == 46);

		d.Write((const byte_t*)"\x84\x9E", 2);
		EXPECT_TRUE(o.Count() == 47);

		for(usys_t i = 0; i < CL3_MIN(sizeof(arr_expected)/4, o.Count()); i++)
			EXPECT_TRUE(o[i].code == arr_expected[i]);
	}

	TEST(io_text_encoding_utf8, Encode)
	{
		TUTF8Encoder e;
		TList<byte_t> o;
		e.Sink(&o);

		const TUTF32 arr_in[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9',0x00E4U,0x00F6U,0x00FCU,'+','#','-',0x20ACU,'!',0x1D11EU,'&',0x1D11EU};

		const char* arr_expected = "abcdefghijklmnopqrstuvwxyz0123456789äöü+#-€!\xF0\x9D\x84\x9E&\xF0\x9D\x84\x9E";

		e.Write(arr_in, sizeof(arr_in) / 4);
		EXPECT_TRUE(o.Count() == 58);

		for(usys_t i = 0; i < CL3_MIN(sizeof(arr_expected), o.Count()); i++)
			EXPECT_TRUE(o[i] == arr_expected[i]);
	}

	TEST(io_text_string_TString, Length)
	{
		TString a = "hällo wörld";
		EXPECT_TRUE(a.Length() == 11);
		EXPECT_TRUE(a.Count() == 11);
		a += "abc\0def";
		EXPECT_TRUE(a.Length() == 14);
		EXPECT_TRUE(a.Count() == 14);
		a.Append("abc\0def", 7);
		EXPECT_TRUE(a.Length() == 17);
		EXPECT_TRUE(a.Count() == 21);
	}

	TEST(io_text_string_TString, Append)
	{
		TString a = "hällo wörld";
		EXPECT_TRUE(a.Length() == 11);
		EXPECT_TRUE(a.Count() == 11);
		EXPECT_TRUE(a == "hällo wörld");

		a += "abc\0def";
		EXPECT_TRUE(a.Length() == 14);
		EXPECT_TRUE(a.Count() == 14);
		EXPECT_TRUE(a == "hällo wörldabc");

		a += L"abc\0def";
		EXPECT_TRUE(a.Length() == 17);
		EXPECT_TRUE(a.Count() == 17);
		EXPECT_TRUE(a == "hällo wörldabcabc");

		const TUTF32 s[] = { 'a', 'b', 'c', TUTF32::TERMINATOR, 'd', 'e', 'f' };
		a += s;
		EXPECT_TRUE(a.Length() == 20);
		EXPECT_TRUE(a.Count() == 20);
		EXPECT_TRUE(a == "hällo wörldabcabcabc");

		a += 'a';
		EXPECT_TRUE(a.Length() == 21);
		EXPECT_TRUE(a.Count() == 21);
		EXPECT_TRUE(a == "hällo wörldabcabcabca");

		a += L'ä';
		EXPECT_TRUE(a.Length() == 22);
		EXPECT_TRUE(a.Count() == 22);
		EXPECT_TRUE(a == "hällo wörldabcabcabcaä");

		a += TUTF32(L'ö');
		EXPECT_TRUE(a.Length() == 23);
		EXPECT_TRUE(a.Count() == 23);
		EXPECT_TRUE(a == "hällo wörldabcabcabcaäö");

		a += TString(L"h°llü");
		EXPECT_TRUE(a.Length() == 28);
		EXPECT_TRUE(a.Count() == 28);
		EXPECT_TRUE(a == "hällo wörldabcabcabcaäöh°llü");

		a.Append(TString(L"hällö"));
		EXPECT_TRUE(a.Length() == 33);
		EXPECT_TRUE(a.Count() == 33);
		EXPECT_TRUE(a == "hällo wörldabcabcabcaäöh°llühällö");

		a.Append("hällö", strlen("hällö"));
		EXPECT_TRUE(a.Length() == 38);
		EXPECT_TRUE(a.Count() == 38);
		EXPECT_TRUE(a == "hällo wörldabcabcabcaäöh°llühällöhällö");

		a.Append('a');
		EXPECT_TRUE(a.Length() == 39);
		EXPECT_TRUE(a.Count() == 39);
		EXPECT_TRUE(a == "hällo wörldabcabcabcaäöh°llühällöhällöa");

		a.Append(s, sizeof(s) / sizeof(TUTF32));
		EXPECT_TRUE(a.Length() == 42);
		EXPECT_TRUE(a.Count() == 46);

		a.Append("test\0abc", 8);
		EXPECT_TRUE(a.Length() == 42);
		EXPECT_TRUE(a.Count() == 54);

		a = "";
		a.Append(L'ö');
		EXPECT_TRUE(a.Length() == 1);
		EXPECT_TRUE(a.Count() == 1);
		EXPECT_TRUE(a == "ö");

		TList<char> lchr;
		lchr.Append('a');
		lchr.Append('z');
		lchr.Append('9');

		a += lchr;
		EXPECT_TRUE(a.Length() == 4);
		EXPECT_TRUE(a.Count() == 4);
		EXPECT_TRUE(a == "öaz9");

		TList<wchar_t> wchr;
		wchr.Append(L'ä');
		wchr.Append(L'§');
		wchr.Append(L'ü');

		a += wchr;
		EXPECT_TRUE(a.Length() == 7);
		EXPECT_TRUE(a.Count() == 7);
		EXPECT_TRUE(a == "öaz9ä§ü");

		CODEC_CXX_WCHAR = CODEC_UTF8;

		wchr.Clear();
		wchr.Append(0x01010101);

		a = "";
		a += wchr;
		EXPECT_TRUE(a.Length() == 4);
		EXPECT_TRUE(a.Count() == 4);
		EXPECT_TRUE(a == "\x01\x01\x01\x01");

		CODEC_CXX_WCHAR = CODEC_UTF32;
	}

	TEST(io_text_string_TString, Compare)
	{
		TString s = "hello world";
		EXPECT_TRUE(s == "hello world");
		EXPECT_TRUE(s != "hello");
		EXPECT_TRUE(s != "world");
		EXPECT_TRUE(s != "aaaaaaaaaaa");
		EXPECT_TRUE(s != "aaaaaaaaaaaaaaaaaaaaaa");
		EXPECT_TRUE(s != "hello worlt");
		EXPECT_TRUE(s != "mello world");
		EXPECT_TRUE(s != "hello world ");
		EXPECT_TRUE(s != "hello worlda");
		EXPECT_TRUE(s != "hello world.");
		EXPECT_TRUE(s != "hello worldZ");

		s = "spe world";
		EXPECT_TRUE(s != "special world");
	}

	TEST(io_text_string_TString, Replace_by_Index)
	{
		TString s = "hello world";
		s.Replace(0, 5, "new");
		EXPECT_TRUE(s == "new world");
		s.Replace(0, 3, "special");
		EXPECT_TRUE(s == "special world");
		s.Replace(8, 5, "test");
		EXPECT_TRUE(s == "special test");
		s.Replace(7, 1, "-");
		EXPECT_TRUE(s == "special-test");
	}

	TEST(io_text_string_TString, Find_Forward)
	{
		TString s = "hello world foo";
		EXPECT_TRUE(s.Find("hello") == 0);
		EXPECT_TRUE(s.Find("world") == 6);
		EXPECT_TRUE(s.Find("world", 2) == 6);
		EXPECT_TRUE(s.Find("world", 6) == 6);
		EXPECT_TRUE(s.Find("world", 7) == (usys_t)-1);
		EXPECT_TRUE(s.Find("foo") == 12);
		EXPECT_TRUE(s.Find("foo", 12) == 12);
		EXPECT_TRUE(s.Find("foo", 11) == 12);
		EXPECT_TRUE(s.Find("foo", 13) == (usys_t)-1);
		EXPECT_TRUE(s.Find("hello world foo") == 0);
		EXPECT_TRUE(s.Find("world foo") == 6);
		EXPECT_TRUE(s.Find("hello foo") == (usys_t)-1);
		EXPECT_TRUE(s.Find("hello world foo ") == (usys_t)-1);
	};

	TEST(io_text_string_TString, Find_Backward)
	{
		TString s = "hello world foo";
		EXPECT_TRUE(s.Find("hello", -1, DIRECTION_BACKWARD) == 0);
		EXPECT_TRUE(s.Find("world", -1, DIRECTION_BACKWARD) == 6);
		EXPECT_TRUE(s.Find("world", s.Count()-5, DIRECTION_BACKWARD) == 6);
		EXPECT_TRUE(s.Find("world", s.Count()-6, DIRECTION_BACKWARD) == (usys_t)-1);
		EXPECT_TRUE(s.Find("foo", -1, DIRECTION_BACKWARD) == 12);
		EXPECT_TRUE(s.Find("hello world foo", -1, DIRECTION_BACKWARD) == 0);
		EXPECT_TRUE(s.Find("world foo", -1, DIRECTION_BACKWARD) == 6);
		EXPECT_TRUE(s.Find("world foo", s.Count()-1, DIRECTION_BACKWARD) == 6);
		EXPECT_TRUE(s.Find("world foo", s.Count()-2, DIRECTION_BACKWARD) == (usys_t)-1);
		EXPECT_TRUE(s.Find("hello foo", -1, DIRECTION_BACKWARD) == (usys_t)-1);
		EXPECT_TRUE(s.Find("hello world foo ", -1, DIRECTION_BACKWARD) == (usys_t)-1);
	};

	TEST(io_text_string_TString, Find_and_Replace)
	{
		TString s = "hello world foo";
		s.Replace("hello", "foo");
		EXPECT_TRUE(s == "foo world foo");
		s.Replace("foo", "abc");
		EXPECT_TRUE(s == "abc world abc");
		s.Replace("abc", "abcdef");
		EXPECT_TRUE(s == "abcdef world abcdef");
		s.Replace("abcdef", "new", 1);
		EXPECT_TRUE(s == "new world abcdef");
	}

	TEST(io_text_string_TString, Find_and_Replace_recursive1)
	{
		TString s = "hello world foo";
		s.Replace("hello", "hellohello");
		EXPECT_TRUE(s == "hellohello world foo");
	}

	TEST(io_text_string_TString, Find_and_Replace_recursive2)
	{
		TString s = "hellllo world foo";
		s.Replace("ll", "l");
		EXPECT_TRUE(s == "hello world foo");
		EXPECT_FALSE(s == "helo world foo");
	}

	TEST(io_text_string_TString, Left_Right_Slice)
	{
		TString s = "hello world foo";

		EXPECT_TRUE(s.Left(5) == "hello");
		EXPECT_TRUE(s.Right(3) == "foo");
		EXPECT_TRUE(s.Slice(6, 5) == "world");

		EXPECT_TRUE(s.Left(15) == "hello world foo");
		EXPECT_TRUE(s.Right(15) == "hello world foo");
		EXPECT_TRUE(s.Slice(0, 15) == "hello world foo");
	}

	TEST(io_text_string_TString, assign)
	{
		TString a = "hello";
		TString b = "world";
		EXPECT_TRUE(a == "hello");
		EXPECT_TRUE(b == "world");
		a = b;
		EXPECT_TRUE(a == "world");
		b = TString("foo");
		EXPECT_TRUE(b == "foo");
	}

	TEST(io_text_string_TString, construct)
	{
		{
			TString s = "hello";
			TString a(cl3::system::def::move(s));
			TString b = "world";
			TString c;
			EXPECT_TRUE(a == "hello");
			EXPECT_TRUE(b == "world");
			EXPECT_TRUE(c == "");
		}

		{
			const TUTF32 s[] = { 'a', 'b', 'c', TUTF32::TERMINATOR, 'd', 'e', 'f' };
			TString str(s, sizeof(s) / sizeof(TUTF32));
		}
	}

	TEST(io_text_string_TString, Pad)
	{
		{
			TString s = "hello world foo";

			s.Pad(POSITION_HEAD, 2, ' ');
			EXPECT_TRUE(s == "  hello world foo");

			s.Pad(POSITION_TAIL, 2, ' ');
			EXPECT_TRUE(s == "  hello world foo  ");
		}

		{
			TString s;

			s.Pad(POSITION_HEAD, 2, ' ');
			EXPECT_TRUE(s == "  ");
		}

		{
			TString s;

			s.Pad(POSITION_TAIL, 2, ' ');
			EXPECT_TRUE(s == "  ");
		}
	}

	TEST(io_text_string_TString, Trim)
	{
		TString s = " \t hello world foo \n \t \n\x0D\x0A\x09\x0B \t\t";

		s.Trim();
		EXPECT_TRUE(s == "hello world foo");
	}

	TEST(io_text_string_TString, Reverse)
	{
		TString s = "hello world";
		s.Reverse();
		EXPECT_TRUE(s == "dlrow olleh");

		s = "a";
		s.Reverse();
		EXPECT_TRUE(s == "a");

		s = "ab";
		s.Reverse();
		EXPECT_TRUE(s == "ba");

		s = "abc";
		s.Reverse();
		EXPECT_TRUE(s == "cba");

		s = "abcd";
		s.Reverse();
		EXPECT_TRUE(s == "dcba");

		s = "abcde";
		s.Reverse();
		EXPECT_TRUE(s == "edcba");
	}

	TEST(io_text_string_TString, Operator_Add)
	{
		TString s;

		s = "hello";
		EXPECT_TRUE(s == "hello");

		s += " ";
		EXPECT_TRUE(s == "hello ");

		s += "world";
		EXPECT_TRUE(s == "hello world");

		TString x = " test";
		TString y = s + " test";
		TString z = s + x;

		EXPECT_TRUE(s == "hello world");
		EXPECT_TRUE(y == "hello world test");
		EXPECT_TRUE(z == "hello world test");
		EXPECT_TRUE(x == " test");
	}

	TEST(io_text_string_TCString, construct)
	{
		TCString cstr1(TString("hällö wörld"), CODEC_CXX_CHAR);
		EXPECT_TRUE(strcmp(cstr1.Chars(), "hällö wörld") == 0);
		TCString cstr2(cstr1);
		EXPECT_TRUE(strcmp(cstr2.Chars(), "hällö wörld") == 0);
		TCString cstr3(cl3::system::def::move(cstr1));
		EXPECT_TRUE(strcmp(cstr3.Chars(), "hällö wörld") == 0);
		EXPECT_TRUE(strcmp(cstr2.Chars(), "hällö wörld") == 0);
	}

	TEST(io_text_string_TCString, assign)
	{
		TCString cstr1(TString("hällö wörld"), CODEC_CXX_CHAR);
		TCString cstr2(TString("hello world"), CODEC_CXX_CHAR);
		TCString cstr3(TString("abc abc"), CODEC_CXX_CHAR);
		EXPECT_TRUE(strcmp(cstr1.Chars(), "hällö wörld") == 0);
		EXPECT_TRUE(strcmp(cstr2.Chars(), "hello world") == 0);
		EXPECT_TRUE(strcmp(cstr3.Chars(), "abc abc") == 0);
		cstr2 = cstr1;
		EXPECT_TRUE(strcmp(cstr1.Chars(), "hällö wörld") == 0);
		EXPECT_TRUE(strcmp(cstr2.Chars(), "hällö wörld") == 0);
		EXPECT_TRUE(strcmp(cstr3.Chars(), "abc abc") == 0);
		cstr3 = cl3::system::def::move(cstr1);
		EXPECT_TRUE(strcmp(cstr2.Chars(), "hällö wörld") == 0);
		EXPECT_TRUE(strcmp(cstr3.Chars(), "hällö wörld") == 0);
	}

	TEST(io_text_encoding_utf8, LimitedSink_Encode)
	{
		const TString s = "hällä wörld";

		{
			TLimitedBuffer<byte_t> limited_sink(7);
			TUTF8Encoder e;
			e.Sink(&limited_sink);

			EXPECT_TRUE(e.Write(s.ItemPtr(0), 11, 5) == 5);
		}

		{
			TLimitedBuffer<byte_t> limited_sink(6);
			TUTF8Encoder e;
			e.Sink(&limited_sink);

			EXPECT_TRUE(e.Write(s.ItemPtr(0), 11, 4) == 5);

			EXPECT_TRUE(e.IsDirty());
			e.Reset();
		}

		{
			TUTF8Encoder e;
			{
				TLimitedBuffer<byte_t> limited_sink(6);
				e.Sink(&limited_sink);

				EXPECT_TRUE(e.Write(s.ItemPtr(0), 11, 4) == 5);
				EXPECT_TRUE(e.IsDirty());
				EXPECT_TRUE(limited_sink.array[5] == 0xC3);
			}

			{
				TLimitedBuffer<byte_t> limited_sink(1);
				e.Sink(&limited_sink);

				EXPECT_TRUE(e.Write(s.ItemPtr(5), 6, 0) == 0);
				EXPECT_TRUE(!e.IsDirty());
				EXPECT_TRUE(limited_sink.array[0] == 0xA4);
			}
		}

		{
			TLimitedBuffer<byte_t> limited_sink(14);
			TUTF8Encoder e;
			e.Sink(&limited_sink);

			EXPECT_TRUE(e.Write(s.ItemPtr(0), s.Count(), (usys_t)-1) == s.Count());

			EXPECT_TRUE(!e.IsDirty());
			e.Reset();
		}
	}

	TEST(io_text_encoding_utf8, LimitedSink_Decode)
	{
		const char s[] = "h€llö wörld";

		{
			TLimitedBuffer<TUTF32> limited_sink(4);
			TUTF8Decoder d;
			d.Sink(&limited_sink);
			EXPECT_TRUE(d.Write((const byte_t*)s, sizeof(s), 6) >= 6);
			EXPECT_TRUE(d.IsDirty());
			EXPECT_TRUE(limited_sink.index == 4);
			d.Reset();
		}

		{
			TLimitedBuffer<TUTF32> limited_sink(4);
			TUTF8Decoder d;
			d.Sink(&limited_sink);
			EXPECT_TRUE(d.Write((const byte_t*)s, sizeof(s), 0) >= 6);
			EXPECT_TRUE(d.IsDirty());
			EXPECT_TRUE(limited_sink.index == 4);
			d.Reset();
		}

		{
			TLimitedBuffer<TUTF32> limited_sink(0);
			TUTF8Decoder d;
			d.Sink(&limited_sink);
			EXPECT_TRUE(d.Write((const byte_t*)s, sizeof(s), 0) == 0);
			EXPECT_TRUE(!d.IsDirty());
			EXPECT_TRUE(limited_sink.index == 0);
			d.Reset();
		}

		{
			TLimitedBuffer<TUTF32> limited_sink(11);
			TUTF8Decoder d;
			d.Sink(&limited_sink);
			EXPECT_TRUE(d.Write((const byte_t*)s, sizeof(s)-1, (usys_t)-1) == sizeof(s)-1);	//	-1 skip the \0 at the end
			EXPECT_TRUE(!d.IsDirty());
			EXPECT_TRUE(limited_sink.index == 11);
			d.Reset();
		}
	}

	TEST(io_text_encoding_utf8, utility)
	{
		TLimitedBuffer<TUTF32> limited_sink_utf32(0);
		TLimitedBuffer<byte_t> limited_sink_byte(0);

		TUTF8Decoder d;
		d.Sink(&limited_sink_utf32);
		EXPECT_TRUE(d.Sink() == &limited_sink_utf32);
		EXPECT_TRUE(d.Codec() == CODEC_UTF8);

		TUTF8Encoder e;
		e.Sink(&limited_sink_byte);
		EXPECT_TRUE(e.Sink() == &limited_sink_byte);
		EXPECT_TRUE(e.Codec() == CODEC_UTF8);

		EXPECT_TRUE(CODEC_UTF8->Name() == "utf8");
	}

	TEST(io_text_encoding_utf8, Invalid_Decode)
	{
		const char cstr_incomplete_mid[] = "hello world \xE2\x82 foobar!";
		const char cstr_invalid[] = "hello world \x82\xAC\xE2\x82\xAC foobar!";

		{
			bool b = false;
			TString str;
			TUTF8Decoder d;
			try
			{
				d.Sink(&str);
				d.Write((const byte_t*)cstr_incomplete_mid, sizeof(cstr_incomplete_mid)-1);
			}
			catch(const TTranscodeException& te)
			{
				EXPECT_TRUE(te.codec == CODEC_UTF8);
				EXPECT_TRUE(te.direction == DIRECTION_DECODE);
				EXPECT_TRUE(te.reason == REASON_INCOMPLETE);
				EXPECT_TRUE(te.n_input_items_processed == 14);
				EXPECT_TRUE(te.n_output_items_written == 12);
				b = true;
			}
			catch(...) {}
			EXPECT_TRUE(b);
			d.Reset();
		}

		{
			bool b = false;
			TString str;
			TUTF8Decoder d;
			try
			{
				d.Sink(&str);
				d.Write((const byte_t*)cstr_invalid, sizeof(cstr_invalid)-1);
			}
			catch(const TTranscodeException& te)
			{
				EXPECT_TRUE(te.codec == CODEC_UTF8);
				EXPECT_TRUE(te.direction == DIRECTION_DECODE);
				EXPECT_TRUE(te.reason == REASON_INVALID);
				EXPECT_TRUE(te.n_input_items_processed == 12);
				EXPECT_TRUE(te.n_output_items_written == 12);
				b = true;
			}
			catch(...) {}
			EXPECT_TRUE(b);
			d.Reset();
		}
	}

	TEST(io_text_encoding_utf8, Invalid_Encode)
	{
		TString str_not_representable = "hello world ";
		str_not_representable.Add(3097152U);
		str_not_representable += " foobar!";

		{
			bool b = false;
			TUTF8Encoder e;
			TList<byte_t> list;
			e.Sink(&list);

			try
			{
				e.Write(str_not_representable.ItemPtr(0), str_not_representable.Count());
			}
			catch(const TTranscodeException& te)
			{
				EXPECT_TRUE(te.codec == CODEC_UTF8);
				EXPECT_TRUE(te.direction == DIRECTION_ENCODE);
				EXPECT_TRUE(te.reason == REASON_NOT_REPRESENTABLE);
				EXPECT_TRUE(te.n_input_items_processed == 12);
				EXPECT_TRUE(te.n_output_items_written == 12);
				b = true;
			}
			catch(...) {}
			EXPECT_TRUE(b);
			e.Reset();
		}
	}

	TEST(io_text_ITextReader, readline)
	{
		TString src = "hello world\nhello foobar!\nthis is a test\n\n  a\t";
		TString line;

		src>>line;
		EXPECT_TRUE(line == "hello world");

		src>>line;
		EXPECT_TRUE(line == "hello foobar!");

		src>>line;
		EXPECT_TRUE(line == "this is a test");

		src>>line;
		EXPECT_TRUE(line == "");

		src>>line;
		EXPECT_TRUE(line == "  a\t");

		EXPECT_TRUE(src == "");
	}

	TEST(io_text_ITextWriter, Integers_simple_decimal)
	{
		TNumberFormat::default_format = &TNumberFormat::DECIMAL_SIMPLE;
		{
			TString buffer;
			buffer<<0;
			EXPECT_TRUE(buffer == "0");
		}

		{
			TString buffer;
			buffer<<42;
			EXPECT_TRUE(buffer == "42");
		}

		{
			TString buffer;
			buffer<<2147483647;
			EXPECT_TRUE(buffer == "2147483647");
		}

		{
			TString buffer;
			buffer<<-2147483648;
			EXPECT_TRUE(buffer == "-2147483648");
		}

		{
			TString buffer;
			buffer<<9223372036854775807;
			EXPECT_TRUE(buffer == "9223372036854775807");
		}

		{
			TString buffer;
			buffer<<-9223372036854775807;
			EXPECT_TRUE(buffer == "-9223372036854775807");
		}
	}

	TEST(io_text_ITextWriter, Integers_hex)
	{
		TNumberFormat::default_format = &TNumberFormat::HEX;
		{
			TString buffer;
			buffer<<0;
			EXPECT_TRUE(buffer == "0");
		}

		{
			TString buffer;
			buffer<<0x42;
			EXPECT_TRUE(buffer == "42");
		}

		{
			TString buffer;
			buffer<<0x2A;
			EXPECT_TRUE(buffer == "2A");
		}

		{
			TString buffer;
			buffer<<0x7FFFFFFF;
			EXPECT_TRUE(buffer == "7FFFFFFF");
		}

		{
			TString buffer;
			buffer<<-0x7FFFFFFF;
			EXPECT_TRUE(buffer == "-7FFFFFFF");
		}

		{
			TString buffer;
			buffer<<0x7FFFFFFFFFFFFFFF;
			EXPECT_TRUE(buffer == "7FFFFFFFFFFFFFFF");
		}

		{
			TString buffer;
			buffer<<-0x7FFFFFFFFFFFFFFF;
			EXPECT_TRUE(buffer == "-7FFFFFFFFFFFFFFF");
		}
		TNumberFormat::default_format = &TNumberFormat::DECIMAL_SIMPLE;
	}

	TEST(io_text_ITextWriter, empty_string)
	{
		TString buffer;
		buffer << TString("");
		buffer << "";
		buffer << L"";
		EXPECT_EQ(0UL, buffer.Count());
	}

	TEST(io_text_string_TString, Split_csv)
	{
		TString str = "hello;world;foo;bar;123";
		auto fields = str.Split(";");
		EXPECT_EQ(5U, fields.Count());
		EXPECT_EQ(TString("hello"), fields[0]);
		EXPECT_EQ(TString("world"), fields[1]);
		EXPECT_EQ(TString("foo"), fields[2]);
		EXPECT_EQ(TString("bar"), fields[3]);
		EXPECT_EQ(TString("123"), fields[4]);
	}

	TEST(io_text_string_TString, Split_multichar_delimiter)
	{
		TString str = "hello DELIMITER world DELIMITER test";
		auto fields = str.Split(" DELIMITER ");
		EXPECT_EQ(3U, fields.Count());
		EXPECT_EQ(TString("hello"), fields[0]);
		EXPECT_EQ(TString("world"), fields[1]);
		EXPECT_EQ(TString("test"), fields[2]);
	}

	TEST(io_text_string_TString, Split_empty_fields)
	{
		{
			TString str = "hello;;foo;bar;123";
			auto fields = str.Split(";");
			EXPECT_EQ(5U, fields.Count());
			EXPECT_EQ(TString("hello"), fields[0]);
			EXPECT_EQ(TString(""), fields[1]);
			EXPECT_EQ(TString("foo"), fields[2]);
			EXPECT_EQ(TString("bar"), fields[3]);
			EXPECT_EQ(TString("123"), fields[4]);
		}
		{
			TString str = ";world;foo;bar;123";
			auto fields = str.Split(";");
			EXPECT_EQ(5U, fields.Count());
			EXPECT_EQ(TString(""), fields[0]);
			EXPECT_EQ(TString("world"), fields[1]);
			EXPECT_EQ(TString("foo"), fields[2]);
			EXPECT_EQ(TString("bar"), fields[3]);
			EXPECT_EQ(TString("123"), fields[4]);
		}
		{
			TString str = "hello;world;foo;bar;";
			auto fields = str.Split(";");
			EXPECT_EQ(5U, fields.Count());
			EXPECT_EQ(TString("hello"), fields[0]);
			EXPECT_EQ(TString("world"), fields[1]);
			EXPECT_EQ(TString("foo"), fields[2]);
			EXPECT_EQ(TString("bar"), fields[3]);
			EXPECT_EQ(TString(""), fields[4]);
		}
		{
			TString str = " DELIMITER world DELIMITER ";
			auto fields = str.Split(" DELIMITER ");
			EXPECT_EQ(3U, fields.Count());
			EXPECT_EQ(TString(""), fields[0]);
			EXPECT_EQ(TString("world"), fields[1]);
			EXPECT_EQ(TString(""), fields[2]);
		}
	}

	TEST(io_text_string_TString, Join_empty)
	{
		const TList<const TString> list;
		const TString str = TString::Join(list, ";");
		EXPECT_EQ(TString(""), str);
	}

	TEST(io_text_string_TString, Join_one_field)
	{
		const TList<const TString> list = { "hello" };
		const TString str = TString::Join(list, ";");
		EXPECT_EQ(TString("hello"), str);
	}

	TEST(io_text_string_TString, Join_csv)
	{
		const TList<const TString> list = { "hello", "world", "foo", "bar" };
		const TString str = TString::Join(list, ";");
		EXPECT_EQ(TString("hello;world;foo;bar"), str);
	}

	TEST(io_text_string_TString, Join_empty_fields)
	{
		{
			const TList<const TString> list = { "hello", "", "foo", "bar" };
			const TString str = TString::Join(list, ";");
			EXPECT_EQ(TString("hello;;foo;bar"), str);
		}
		{
			const TList<const TString> list = { "", "world", "foo", "bar" };
			const TString str = TString::Join(list, ";");
			EXPECT_EQ(TString(";world;foo;bar"), str);
		}
		{
			const TList<const TString> list = { "hello", "world", "foo", "" };
			const TString str = TString::Join(list, ";");
			EXPECT_EQ(TString("hello;world;foo;"), str);
		}
	}

	TEST(io_text_string_TString, Join_multichar_delimiter)
	{
		const TList<const TString> list = { "hello", "world", "foo", "bar" };
		const TString str = TString::Join(list, " DELIMITER ");
		EXPECT_EQ(TString("hello DELIMITER world DELIMITER foo DELIMITER bar"), str);
	}
}
