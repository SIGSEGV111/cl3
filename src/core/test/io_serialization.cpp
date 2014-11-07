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
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/io_text_terminal.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::io::serialization;
	using namespace cl3::io::serialization::json;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace cl3::system::memory;
	using namespace cl3::io::collection::list;

	struct	TTest : ISerializable
	{
		int x,y,z;
		const char* str;
		int array_of_ints[4];

		void	Serialize	(ISerializer& s) const final override
		{
			s.Push("x", x);
			s.Push("y", y);
			s.Push("z", z);
			s.Push("str", str);

			TUniquePtr<IArraySerializer> as = s.PushArray("array_of_ints");
			as->Push(array_of_ints[0]);
			as->Push(array_of_ints[1]);
			as->Push(array_of_ints[2]);
			as->Push(array_of_ints[3]);
		}

		void	Deserialize	(IDeserializer& ds) final override
		{
			ds.Pop("x", x);
			ds.Pop("y", y);
			ds.Pop("z", z);
			ds.Pop("str", str);

			TUniquePtr<IArrayDeserializer> ads = ds.PopArray("array_of_ints");
			ads->Pop(array_of_ints[0]);
			ads->Pop(array_of_ints[1]);
			ads->Pop(array_of_ints[2]);
			ads->Pop(array_of_ints[3]);
		}

		CLASS	TTest	() : x(10), y(20), z(30), str("test")
		{
			array_of_ints[0] = 0;
			array_of_ints[1] = 1;
			array_of_ints[2] = 2;
			array_of_ints[3] = 3;
		}
	};

	TEST(io_serialization_json, Object_pretty)
	{
		TString buffer;

		{
			TJSONSerializer s(&buffer, true);
			TTest test;
			s.Push("test", test);
		}

// 		puts(TCString(buffer, CODEC_CXX_CHAR).Chars());

		EXPECT_TRUE(buffer == "{\n\t\"test\" : {\n\t\t\"x\" : 10,\n\t\t\"y\" : 20,\n\t\t\"z\" : 30,\n\t\t\"str\" : \"test\",\n\t\t\"array_of_ints\" : [ 0, 1, 2, 3 ]\n\t}\n}");
	}

	TEST(io_serialization_json, Object_ugly)
	{
		TString buffer;

		{
			TJSONSerializer s(&buffer, false);
			TTest test;
			s.Push("test", test);
		}

// 		puts(TCString(buffer, CODEC_CXX_CHAR).Chars());

		EXPECT_TRUE(buffer == "{\"test\":{\"x\":10,\"y\":20,\"z\":30,\"str\":\"test\",\"array_of_ints\":[0,1,2,3]}}");
	}

	TEST(io_collection_list, TSerializableList)
	{
		{
			TString buffer;
			{
				TJSONSerializer s(&buffer, false);
				TSerializableList<int> list;
				list.Add(17);
				list.Add(0);
				list.Add(24);
				list.Add(41);
				list.Add(-42);
				list.Add(0x0fffffff);
				s.Push("list", list);
			}
			EXPECT_TRUE(buffer == "{\"list\":{\"count\":6,\"items\":[17,0,24,41,-42,268435455]}}");
		}

		{
			const char* string_list_expected = "{\"list\":{\"count\":4,\"items\":[\"hello world\",\"test\",\"foo\",\"bar\"]}}";
			{
				TString buffer;
				{
					TJSONSerializer s(&buffer, false);
					TSerializableList<const char*> list;
					list.Add("hello world");
					list.Add("test");
					list.Add("foo");
					list.Add("bar");
					s.Push("list", list);
				}
				EXPECT_TRUE(buffer == string_list_expected);
			}

			{
				TString buffer;
				{
					TJSONSerializer s(&buffer, false);
					TSerializableList<TString> list;
					list.Add("hello world");
					list.Add("test");
					list.Add("foo");
					list.Add("bar");
					s.Push("list", list);
				}
				EXPECT_TRUE(buffer == string_list_expected);
			}
		}
	}
}
