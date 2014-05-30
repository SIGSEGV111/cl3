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

	TEST(io_text_encoding_utf8, codec)
	{
		EXPECT_TRUE(CODEC_UTF8 != NULL);

		TUniquePtr<IEncoder> e = CODEC_UTF8->CreateEncoder();
		TUniquePtr<IDecoder> d = CODEC_UTF8->CreateDecoder();

		EXPECT_TRUE(e.UObjPtr() != NULL);
		EXPECT_TRUE(d.UObjPtr() != NULL);
	}

	TEST(io_text_encoding_utf8, decode)
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

	TEST(io_text_encoding_utf8, encode)
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
}
