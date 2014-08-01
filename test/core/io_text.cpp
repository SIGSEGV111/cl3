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

#include <cl3/core/system_compiler.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/system_types_typeinfo.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_text.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/io_text_encoding_utf8.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::memory;
	using namespace cl3::io::text;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace cl3::io::text::encoding::utf8;
	using namespace cl3::io::collection::list;
	using namespace cl3::io::collection;

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

// 		TList<char> lchr;
// 		lchr.Append('a');
// 		lchr.Append('z');
// 		lchr.Append('9');
//
// 		a += lchr;
// 		EXPECT_TRUE(a.Length() == 4);
// 		EXPECT_TRUE(a.Count() == 4);
// 		EXPECT_TRUE(a == "öaz9");
//
// 		TList<wchar_t> wchr;
// 		wchr.Append(L'ä');
// 		wchr.Append(L'§');
// 		wchr.Append(L'ü');
//
// 		a += wchr;
// 		EXPECT_TRUE(a.Length() == 7);
// 		EXPECT_TRUE(a.Count() == 7);
// 		EXPECT_TRUE(a == "öaz9ä§ü");
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
		EXPECT_TRUE(s != "mello worlt");

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
		EXPECT_TRUE(s.Find("foo") == 12);
		EXPECT_TRUE(s.Find("hello world foo") == 0);
		EXPECT_TRUE(s.Find("world foo") == 6);
		EXPECT_TRUE(s.Find("hello foo") == (usys_t)-1);
		EXPECT_TRUE(s.Find("hello world foo ") == (usys_t)-1);
	};

	TEST(io_text_string_TString, Find_Backward)
	{
		TString s = "hello world foo";
		EXPECT_TRUE(s.Find("hello", 0, DIRECTION_BACKWARD) == 0);
		EXPECT_TRUE(s.Find("world", 0, DIRECTION_BACKWARD) == 6);
		EXPECT_TRUE(s.Find("foo", 0, DIRECTION_BACKWARD) == 12);
		EXPECT_TRUE(s.Find("hello world foo", 0, DIRECTION_BACKWARD) == 0);
		EXPECT_TRUE(s.Find("world foo", 0, DIRECTION_BACKWARD) == 6);
		EXPECT_TRUE(s.Find("hello foo", 0, DIRECTION_BACKWARD) == (usys_t)-1);
		EXPECT_TRUE(s.Find("hello world foo ", 0, DIRECTION_BACKWARD) == (usys_t)-1);
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

	TEST(io_text_string_TString, Left_Right_Mid)
	{
		TString s = "hello world foo";

		EXPECT_TRUE(s.Left(5) == "hello");
		EXPECT_TRUE(s.Right(3) == "foo");
		EXPECT_TRUE(s.Mid(6, 5) == "world");

		EXPECT_TRUE(s.Left(15) == "hello world foo");
		EXPECT_TRUE(s.Right(15) == "hello world foo");
		EXPECT_TRUE(s.Mid(0, 15) == "hello world foo");
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
		TString s = "hello";
		TString a(cl3::util::move(s));
		TString b = "world";
		TString c;
		EXPECT_TRUE(a == "hello");
		EXPECT_TRUE(b == "world");
		EXPECT_TRUE(c == "");
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
}
