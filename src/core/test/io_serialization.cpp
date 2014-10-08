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

#include <cl3/core/io_serialization.hpp>
#include <cl3/core/io_serialization_json.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_terminal.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::io::serialization;
	using namespace cl3::io::serialization::json;
	using namespace cl3::io::text::string;

	struct	TTest : ISerializable
	{
		int x,y,z;
		const char* str;

		void	Serialize	(ISerializer& s) const final override
		{
			s.Push("x", x);
			s.Push("y", y);
			s.Push("z", z);
			s.Push("str", str);
		}

		void	Deserialize	(IDeserializer& ds) final override
		{
			ds.Pop("x", x);
			ds.Pop("y", y);
			ds.Pop("z", z);
			ds.Pop("str", str);
		}

		CLASS	TTest	() : x(10), y(20), z(30), str("test") {}
	};

	TEST(io_serialization_json, pretty)
	{
		TString buffer;

		{
			TJSONSerializer s(&buffer, true);
			TTest test;
			s.Push("test", test);
		}

		EXPECT_TRUE(buffer == "{\n\t\"test\" : {\n\t\t\"x\" : 10,\n\t\t\"y\" : 20,\n\t\t\"z\" : 30,\n\t\t\"str\" : \"test\"\n\t}\n}");
	}

	TEST(io_serialization_json, ugly)
	{
		TString buffer;

		{
			TJSONSerializer s(&buffer, false);
			TTest test;
			s.Push("test", test);
		}

		EXPECT_TRUE(buffer == "{\"test\":{\"x\":10,\"y\":20,\"z\":30,\"str\":\"test\"}}");
	}
}
