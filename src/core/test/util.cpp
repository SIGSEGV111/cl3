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
#include <cl3/core/util_function.hpp>
#include <cl3/core/util_event.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/system_memory.hpp>

#include <gtest/gtest.h>
#include <string.h>

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

	TEST(util, mkstrcpy)
	{
		const char* src = "hello world 123 !";
		auto p = MakeCStringCopy(src);
		EXPECT_TRUE(strcmp(src, p.Object()) == 0);
	}
}

namespace
{
	using namespace cl3::util;
	using namespace cl3::util::function;

	static void TestFuncVoid(int& v, int nv)
	{
		v = nv + 10;
	}

	TEST(util_function, StaticFunctionVoid)
	{
		int v = 0;
		TFunction<void, int&, int> func = &TestFuncVoid;
		func(v, 10);
		EXPECT_EQ(20, v);
	}

	TEST(util_function, LamdaVoid)
	{
		int v = 0;
		TFunction<void, int> func = [&v](int nv){ v = nv + 10; };
		func(10);
		EXPECT_EQ(20, v);
	}

	TEST(util_function, MemberFunctionVoid)
	{
		struct TTest
		{
			int v;

			void Callback(int nv)
			{
				this->v += nv + 10;
			}
		};

		TTest test;
		test.v = 0;

		TFunction<void, int> func(&test, &TTest::Callback);

		func(10);
		EXPECT_EQ(20, test.v);
	}

	int TestFuncInt(int& v, int nv)
	{
		return v += nv + 10;
	}

	TEST(util_function, StaticFunctionInt)
	{
		int v = 0;
		TFunction<int, int&, int> func = &TestFuncInt;
		EXPECT_EQ(20, func(v, 10));
		EXPECT_EQ(20, v);

		TFunction<int, int&, int> f2 = func;
		EXPECT_EQ(40, f2(v, 10));
		EXPECT_EQ(40, v);
	}

	TEST(util_function, LamdaInt)
	{
		int v = 0;
		TFunction<int, int> func = [&v](int nv){ return v += nv + 10; };
		EXPECT_EQ(20, func(10));
		EXPECT_EQ(20, v);

		TFunction<int, int> f2 = func;
		EXPECT_EQ(40, f2(10));
		EXPECT_EQ(40, v);
	}

	TEST(util_function, MemberFunctionInt)
	{
		struct TTest
		{
			int v;

			int Callback(int nv)
			{
				return this->v += nv + 10;
			}
		};

		TTest test;
		test.v = 0;

		TFunction<int, int> func(&test, &TTest::Callback);

		EXPECT_EQ(20, func(10));
		EXPECT_EQ(20, test.v);

		TFunction<int, int> f2 = func;
		EXPECT_EQ(40, f2(10));
		EXPECT_EQ(40, test.v);
	}
}

namespace
{
	using namespace cl3::util;
	using namespace cl3::util::event;
	using namespace cl3::system::memory;

	TEST(util_event, NoListener)
	{
		int v = 0;
		TEvent<int&, int> event;
		event.Raise(v, 10);
		EXPECT_EQ(0, v);
	}

	TEST(util_event, SingleListener)
	{
		int v = 0;
		TEvent<int&, int> event;
		auto l1 = event.Register([](int& v, int nv){ v += nv; });
		event.Raise(v, 10);
		EXPECT_EQ(10, v);
	}

	TEST(util_event, TwoListeners)
	{
		int v = 0;
		TEvent<int&, int> event;
		auto l1 = event.Register([](int& v, int nv){ v += nv; });
		auto l2 = event.Register([](int& v, int nv){ v += nv; });
		event.Raise(v, 10);
		EXPECT_EQ(20, v);
	}

	TEST(util_event, Unregister)
	{
		int v = 0;
		TEvent<int&, int> event;

		auto l1 = event.Register([](int& v, int nv){ v += nv; });
		TUniquePtr<decltype(l1)> l3;

		{
			auto l2 = event.Register([](int& v, int nv){ v += nv; });
			l3 = MakeUniquePtr(new TEvent<int&,int>::TListener(event.Register([](int& v, int nv){ v += nv; })));
		}

		event.Raise(v, 10);
		EXPECT_EQ(20, v);
	}
}
