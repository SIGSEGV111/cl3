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

#include <cl3/core/system_types.hpp>
#include <cl3/core/system_types_typeinfo.hpp>
#include <gtest/gtest.h>

using namespace ::testing;
using namespace std;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::types::typeinfo;

	struct	TTestType_POD
	{
		int a,b,c;
	};

	struct	TTestType_NA
	{
		private:
			TTestType_NA(const TTestType_NA& other) : a(other.a), b(other.a), c(other.a) {}

		public:
			TTestType_NA(int) : a(-1), b(-2), c(-3) {}
			int a,b,c;
			~TTestType_NA() {}
	};

	/*************************************************/

	TEST(TypeInfo, StandardConstructor_int)
	{
		volatile FStandardConstructor ctor = TCTTI<int>::ctor;
		EXPECT_TRUE(ctor != NULL);
	}

	TEST(TypeInfo, Destructor_int)
	{
		volatile FDestructor dtor = TCTTI<int>::dtor;
		EXPECT_TRUE(dtor != NULL);
	}

	TEST(TypeInfo, CopyConstructor_int)
	{
		volatile FCopyConstructor copyctor = TCTTI<int>::copyctor;
		EXPECT_TRUE(copyctor != NULL);
	}

	/*************************************************/

	TEST(TypeInfo, StandardConstructor_POD)
	{
		volatile FStandardConstructor ctor = TCTTI<TTestType_POD>::ctor;
		EXPECT_TRUE(ctor != NULL);
	}

	TEST(TypeInfo, Destructor_POD)
	{
		volatile FDestructor dtor = TCTTI<TTestType_POD>::dtor;
		EXPECT_TRUE(dtor != NULL);
	}

	TEST(TypeInfo, CopyConstructor_POD)
	{
		volatile FCopyConstructor copyctor = TCTTI<TTestType_POD>::copyctor;
		EXPECT_TRUE(copyctor != NULL);
	}

	/*************************************************/

	TEST(TypeInfo, StandardConstructor_NA)
	{
		volatile FStandardConstructor ctor = TCTTI<TTestType_NA>::ctor;
		EXPECT_TRUE(ctor == NULL);
	}

	TEST(TypeInfo, Destructor_NA)
	{
		volatile FDestructor dtor = TCTTI<TTestType_NA>::dtor;
		EXPECT_TRUE(dtor == NULL);
	}

	TEST(TypeInfo, CopyConstructor_NA)
	{
		volatile FCopyConstructor copyctor = TCTTI<TTestType_NA>::copyctor;
		EXPECT_TRUE(copyctor == NULL);

	}

	/*************************************************/
}
