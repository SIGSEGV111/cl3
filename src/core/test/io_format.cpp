/*
 *    libcl3 - common library version 3
 *    Copyright (C) 2013	Simon Brennecke
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cl3/core/io_format_json.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::error;
	using namespace cl3::io::format;
	using namespace cl3::io::format::json;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;

	TEST(io_format_json_SerializeJSON, integer)
	{
		TString json;
		TValue root;
		root.type = TValue::EType::SINT32;
		root.s32 = 42;
		SerializeJSON(root, json);

		EXPECT_TRUE(json == "42");
	}

	TEST(io_format_json_SerializeJSON, floatnum)
	{
		TString json;
		TValue root;
		root.type = TValue::EType::FLOAT64;
		root.s32 = 42.7;
		SerializeJSON(root, json);

// 		EXPECT_TRUE(json == "42.7");	//	FIXME ITextWriter
	}

	TEST(io_format_json_SerializeJSON, text)
	{
		TString json;
		TValue root;
		root.type = TValue::EType::STRING;
		root.text = new TString("test123\"\\");
		SerializeJSON(root, json);

		EXPECT_TRUE(json == "\"test123\\\"\\\\\"");
	}

	TEST(io_format_json_SerializeJSON, array_integer)
	{
		TString json;
		TValue root;
		root.type = TValue::EType::ARRAY;
		root.array = new TArray();

		TValue value;
		value.type = TValue::EType::SINT32;
		value.s32 = 42;
		root.array->items.Add(value);
		value.s32 = 17;
		root.array->items.Add(value);
		value.s32 = 32;
		root.array->items.Add(value);
		value.s32 = 9;
		root.array->items.Add(value);

		SerializeJSON(root, json);

		EXPECT_TRUE(json == "[42,17,32,9]");
	}

	TEST(io_format_json_SerializeJSON, array_mixed)
	{
		TString json;
		TValue root;
		root.type = TValue::EType::ARRAY;
		root.array = new TArray();

		TValue value;
		value.type = TValue::EType::SINT32;
		value.s32 = 42;
		root.array->items.Add(value);
		value.s32 = 17;
		root.array->items.Add(value);
		value.type = TValue::EType::FLOAT64;
		value.f32 = 32.3;
// 		root.array->items.Add(value);	//	FIXME ITextWriter
		value.type = TValue::EType::STRING;
		value.text = new TString("hello world");
		root.array->items.Add(value);

		SerializeJSON(root, json);

		EXPECT_TRUE(json == "[42,17,\"hello world\"]");

		printf("json: %s\n", TCString(json, CODEC_CXX_CHAR).Chars());
	}
}
