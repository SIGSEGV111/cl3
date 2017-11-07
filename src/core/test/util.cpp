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

#include <cl3/core/util.hpp>
#include <cl3/core/io_text_string.hpp>

#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::util;
	using namespace cl3::io::text::string;

	TEST(util, Hexdump)
	{
		TString out;
		char in[] = "Hello World\nabcdefghijklmnopqrstuvwxyz\0\x01\x02";

		char ref[512];

		if(sizeof(void*) == 8)
			snprintf(ref, sizeof(ref), "[0000] %016zx: 48 65 6c 6c 6f 20 57 6f 72 6c 64 0a 61 62 63 64 |Hello World.abcd|\n[0001] %016zx: 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 |efghijklmnopqrst|\n[0002] %016zx: 75 76 77 78 79 7a 00 01 02 00                   |uvwxyz....      |\n", (size_t)in, (size_t)in+16, (size_t)in+32);
		else if(sizeof(void*) == 4)
			snprintf(ref, sizeof(ref), "[0000]         %08zx: 48 65 6c 6c 6f 20 57 6f 72 6c 64 0a 61 62 63 64 |Hello World.abcd|\n[0001]         %08zx: 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 |efghijklmnopqrst|\n[0002]         %08zx: 75 76 77 78 79 7a 00 01 02 00                   |uvwxyz....      |\n", (size_t)in, (size_t)in+16, (size_t)in+32);
		else
			throw;
		Hexdump(in, sizeof(in), out);

		EXPECT_TRUE(out == ref);
	}
}
