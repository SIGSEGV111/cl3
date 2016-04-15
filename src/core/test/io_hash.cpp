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

#include <cl3/core/io_hash.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::io::hash;

	TEST(io_hash, Jenkins)
	{
		const char* const data = "hello world";
		const u64_t hash = JenkinsHash((const byte_t*)data, sizeof(data));

		EXPECT_EQ(0x336BBD3102656489U, hash);
	}
}
