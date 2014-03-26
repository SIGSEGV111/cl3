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

	struct	TT_POD
	{
		int a,b,c;
	};

	struct	TT_Nothing
	{
		private:
			TT_Nothing(const TT_Nothing& other);
			TT_Nothing();
			~TT_Nothing();
	};

	struct	TT_public_stdctor
	{
		public:
			TT_public_stdctor() {}
		private:
			TT_public_stdctor(const TT_public_stdctor&);
			~TT_public_stdctor();
	};

	struct	TT_public_copyctor
	{
		public:
			TT_public_copyctor(const TT_public_copyctor&) {}
		private:
			TT_public_copyctor();
			~TT_public_copyctor();
	};

	struct	TT_public_dtor
	{
		public:
			~TT_public_dtor() {}
		private:
			TT_public_dtor();
			TT_public_dtor(const TT_public_dtor&);
	};

	struct	TT_public_nonstdctor
	{
		public:
			TT_public_nonstdctor(int) {}
		private:
			TT_public_nonstdctor(const TT_public_stdctor&);
			~TT_public_nonstdctor();
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

	TEST(TypeInfo, StandardConstructor)
	{
		volatile FStandardConstructor v;

		v = TCTTI<TT_POD>::ctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_Nothing>::ctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_stdctor>::ctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_public_copyctor>::ctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_dtor>::ctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_nonstdctor>::ctor;
		EXPECT_TRUE(v == NULL);
	}

	TEST(TypeInfo, Destructor)
	{
		volatile FDestructor v;

		v = TCTTI<TT_POD>::dtor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_Nothing>::dtor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_stdctor>::dtor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_copyctor>::dtor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_dtor>::dtor;
		EXPECT_TRUE(v != NULL);
	}

	TEST(TypeInfo, CopyConstructor)
	{
		volatile FCopyConstructor v;

		v = TCTTI<TT_POD>::copyctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_Nothing>::copyctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_stdctor>::copyctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_copyctor>::copyctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_public_dtor>::copyctor;
		EXPECT_TRUE(v == NULL);
	}
}
