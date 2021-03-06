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

#include <cl3/core/io_text.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/system_types_typeinfo.hpp>
#include <gtest/gtest.h>
#include "common.hpp"

using namespace ::testing;

namespace __
{
	template<class A, class B, int x>
	class	TTest {};
}

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::types::typeinfo;
	using namespace cl3::io::text;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace cl3::unittest_support;

	struct	TT_POD
	{
		int a,b,c;
	};

	//	make the TT_POD printable
	cl3::io::text::ITextWriter& operator<<(cl3::io::text::ITextWriter&, const TT_POD&) { throw TTestException(); }

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
			static bool ok;
			TT_public_stdctor() { ok = true; }
		private:
			TT_public_stdctor(const TT_public_stdctor&);
			~TT_public_stdctor();
	};

	struct	TT_public_copyctor
	{
		public:
			static bool ok;
			TT_public_copyctor(const TT_public_copyctor&) { ok = true; }
		private:
			TT_public_copyctor();
			~TT_public_copyctor();
	};

	struct	TT_public_dtor
	{
		public:
			static bool ok;
			~TT_public_dtor() { ok = true; }
		private:
			TT_public_dtor();
			TT_public_dtor(const TT_public_dtor&);
	};

	struct	TT_public_nonstdctor
	{
		public:
			static bool ok;
			TT_public_nonstdctor(int) { ok = true; }
		private:
			TT_public_nonstdctor(const TT_public_stdctor&);
			~TT_public_nonstdctor();
	};

	bool TT_public_stdctor::ok = false;
	bool TT_public_copyctor::ok = false;
	bool TT_public_dtor::ok = false;
	bool TT_public_nonstdctor::ok = false;

	//	*************************************************

	TEST(system_types_typeinfo_TCTTI, StandardConstructor)
	{
		volatile FStandardConstructor v;

		v = TCTTI<int>::ctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_POD>::ctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_Nothing>::ctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_stdctor>::ctor;
		EXPECT_TRUE(v != NULL);

		{
			TT_public_stdctor::ok = false;
			char buffer[sizeof(TT_public_stdctor)] = {};
			TCTTI<TT_public_stdctor>::ctor(buffer);
			EXPECT_TRUE(TT_public_stdctor::ok);
		}

		v = TCTTI<TT_public_copyctor>::ctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_dtor>::ctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_nonstdctor>::ctor;
		EXPECT_TRUE(v == NULL);
	}

	TEST(system_types_typeinfo_TCTTI, Destructor)
	{
		volatile FDestructor v;

		v = TCTTI<int>::dtor;
		EXPECT_TRUE(v != NULL);

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

		{
			TT_public_dtor::ok = false;
			char buffer[sizeof(TT_public_dtor)] = {};
			TCTTI<TT_public_dtor>::dtor(buffer);
			EXPECT_TRUE(TT_public_dtor::ok);
		}
	}

	TEST(system_types_typeinfo_TCTTI, CopyConstructor)
	{
		volatile FCopyConstructor v;

		v = TCTTI<int>::copyctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_POD>::copyctor;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_Nothing>::copyctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_stdctor>::copyctor;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_copyctor>::copyctor;
		EXPECT_TRUE(v != NULL);

		{
			TT_public_copyctor::ok = false;
			const char buffer1[sizeof(TT_public_copyctor)] = {};
			char buffer2[sizeof(TT_public_copyctor)] = {};
			memset(buffer2, 255, sizeof(TT_public_copyctor));
			TCTTI<TT_public_copyctor>::copyctor(buffer2, buffer1);
			EXPECT_TRUE(TT_public_copyctor::ok);
		}

		v = TCTTI<TT_public_dtor>::copyctor;
		EXPECT_TRUE(v == NULL);
	}

	//	*************************************************

	TEST(system_types_typeinfo_TCTTI, Print)
	{
		volatile cl3::io::text::FPrint v;

		v = TCTTI<int>::print;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_POD>::print;
		EXPECT_TRUE(v != NULL);

		v = TCTTI<TT_Nothing>::print;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_stdctor>::print;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_copyctor>::print;
		EXPECT_TRUE(v == NULL);

		v = TCTTI<TT_public_dtor>::print;
		EXPECT_TRUE(v == NULL);

		TString s;

		EXPECT_THROW(TCTTI<TT_POD>::print(s, NULL), TTestException);
	}

	//	*************************************************

	TEST(system_types_typeinfo_TRTTI, Name)
	{
		//	puts(TCString(*TCTTI<TTestException>::rtti.Name(), CODEC_CXX_CHAR).Chars());

		EXPECT_TRUE( *TCTTI<signed char>::rtti.Name() == "s8_t" );
		EXPECT_TRUE( *TCTTI<unsigned char>::rtti.Name() == "u8_t" );

		EXPECT_TRUE( *TCTTI<signed short>::rtti.Name() == "s16_t" );
		EXPECT_TRUE( *TCTTI<unsigned short>::rtti.Name() == "u16_t" );

		EXPECT_TRUE( *TCTTI<signed int>::rtti.Name() == "s32_t" );
		EXPECT_TRUE( *TCTTI<unsigned int>::rtti.Name() == "u32_t" );

		if(sizeof(long) == 4)
		{
			EXPECT_TRUE( *TCTTI<signed long>::rtti.Name() == "s32_t" );
			EXPECT_TRUE( *TCTTI<unsigned long>::rtti.Name() == "u32_t" );
		}
		else if(sizeof(long) == 8)
		{
			EXPECT_TRUE( *TCTTI<signed long>::rtti.Name() == "s64_t" );
			EXPECT_TRUE( *TCTTI<unsigned long>::rtti.Name() == "u64_t" );
		}
		else
			throw;

		EXPECT_TRUE( *TCTTI<float>::rtti.Name() == "f32_t" );
		EXPECT_TRUE( *TCTTI<double>::rtti.Name() == "f64_t" );

		EXPECT_TRUE( *TCTTI<s8_t>::rtti.Name() == "s8_t" );
		EXPECT_TRUE( *TCTTI<u8_t>::rtti.Name() == "u8_t" );

		EXPECT_TRUE( *TCTTI<s16_t>::rtti.Name() == "s16_t" );
		EXPECT_TRUE( *TCTTI<u16_t>::rtti.Name() == "u16_t" );

		EXPECT_TRUE( *TCTTI<s32_t>::rtti.Name() == "s32_t" );
		EXPECT_TRUE( *TCTTI<u32_t>::rtti.Name() == "u32_t" );

		EXPECT_TRUE( *TCTTI<s64_t>::rtti.Name() == "s64_t" );
		EXPECT_TRUE( *TCTTI<u64_t>::rtti.Name() == "u64_t" );

		EXPECT_TRUE( *TCTTI<f32_t>::rtti.Name() == "f32_t" );
		EXPECT_TRUE( *TCTTI<f64_t>::rtti.Name() == "f64_t" );

		//	*************************************************

		EXPECT_TRUE( *TCTTI<TTestException>::rtti.Name() == "cl3::unittest_support::TTestException" );
		EXPECT_TRUE( *TCTTI<TT_POD>::rtti.Name() == "(anonymous namespace)::TT_POD" );

		//	*************************************************

		EXPECT_TRUE( (*TCTTI< __::TTest<signed short,signed char,17L> >::rtti.Name()) == "__::TTest<s16_t, s8_t, 17>" );
	}

	TEST(system_types_typeinfo_TRTTI, Hash)
	{
		//printf("hash = %08x\n", TCTTI<int>::rtti.Hash());
		EXPECT_TRUE(TCTTI<int>::rtti.Hash() == 0xe333d759);
	}

	TEST(system_types_typeinfo_features, is_class)
	{
		EXPECT_TRUE(features::is_class<TT_Nothing>::value);
		EXPECT_TRUE(features::is_class<TT_POD>::value);
		EXPECT_FALSE(features::is_class<void>::value);
		EXPECT_FALSE(features::is_class<int>::value);
		EXPECT_FALSE(features::is_class<long>::value);
		EXPECT_FALSE(features::is_class<short>::value);
		EXPECT_FALSE(features::is_class<bool>::value);

		EXPECT_FALSE(features::is_class<TT_Nothing*>::value);
		EXPECT_FALSE(features::is_class<TT_POD*>::value);
		EXPECT_FALSE(features::is_class<void*>::value);
		EXPECT_FALSE(features::is_class<int*>::value);
		EXPECT_FALSE(features::is_class<long*>::value);
		EXPECT_FALSE(features::is_class<short*>::value);
		EXPECT_FALSE(features::is_class<bool*>::value);

		EXPECT_FALSE(features::is_class<TT_Nothing&>::value);
		EXPECT_FALSE(features::is_class<TT_POD&>::value);
		EXPECT_FALSE(features::is_class<int&>::value);
		EXPECT_FALSE(features::is_class<long&>::value);
		EXPECT_FALSE(features::is_class<short&>::value);
		EXPECT_FALSE(features::is_class<bool&>::value);
	}
}
