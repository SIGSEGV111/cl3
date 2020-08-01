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
#include <cl3/core/io_text_terminal.hpp>
#include <cl3/core/util.hpp>
#include <gtest/gtest.h>
#include "common.hpp"

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::memory;
	using namespace cl3::unittest_support;

	enum	ELastOp
	{
		OP_UNDEFINED,
		OP_CONSTRUCT,
		OP_COPYCONSTRUCT,
		OP_COPY,
		OP_DESTRUCT,
		OP_ASSIGN
	};

	struct	TCDCTester
	{
		ELastOp* op;
		int value;

		TCDCTester& operator=(int new_value) { *op = OP_ASSIGN; value = new_value; return *this; }
		operator int() const { return value; }

		TCDCTester(ELastOp* op, int init_value) : op(op), value(init_value) { *op = OP_CONSTRUCT; }
		TCDCTester(const TCDCTester& other) { *op = OP_COPYCONSTRUCT; *other.op = OP_COPY; }
		~TCDCTester() { *op = OP_DESTRUCT; }
	};

	TEST(system_memory_TSharedPtr, basic)
	{
		ELastOp op1 = OP_UNDEFINED, op2 = OP_UNDEFINED, op3 = OP_UNDEFINED;

		TSharedPtr<TCDCTester> p1 = MakeSharedPtr(new TCDCTester(&op1, 0));
		TSharedPtr<TCDCTester> p2 = MakeSharedPtr(new TCDCTester(&op2, 17));
		TSharedPtr<TCDCTester> p3 = MakeSharedPtr(new TCDCTester(&op3, 112));

		EXPECT_TRUE(op1 == OP_CONSTRUCT);
		EXPECT_TRUE(op2 == OP_CONSTRUCT);
		EXPECT_TRUE(op3 == OP_CONSTRUCT);

		EXPECT_TRUE(*p1 == 0);
		EXPECT_TRUE(*p2 == 17);
		EXPECT_TRUE(*p3 == 112);

		EXPECT_TRUE(op1 == OP_CONSTRUCT);
		EXPECT_TRUE(op2 == OP_CONSTRUCT);
		EXPECT_TRUE(op3 == OP_CONSTRUCT);

		*p1 = 18;
		*p2 = 56;
		*p3 = 134;

		EXPECT_TRUE(op1 == OP_ASSIGN);
		EXPECT_TRUE(op2 == OP_ASSIGN);
		EXPECT_TRUE(op3 == OP_ASSIGN);

		EXPECT_TRUE(*p1 == 18);
		EXPECT_TRUE(*p2 == 56);
		EXPECT_TRUE(*p3 == 134);

		p2 = p1;

		EXPECT_TRUE(op1 == OP_ASSIGN);
		EXPECT_TRUE(op2 == OP_DESTRUCT);
		EXPECT_TRUE(op3 == OP_ASSIGN);

		p2.Clear();

		EXPECT_TRUE(op1 == OP_ASSIGN);
		EXPECT_TRUE(op2 == OP_DESTRUCT);
		EXPECT_TRUE(op3 == OP_ASSIGN);

		p1.Clear();

		EXPECT_TRUE(op1 == OP_DESTRUCT);
		EXPECT_TRUE(op2 == OP_DESTRUCT);
		EXPECT_TRUE(op3 == OP_ASSIGN);

		p3.Clear();

		EXPECT_TRUE(op1 == OP_DESTRUCT);
		EXPECT_TRUE(op2 == OP_DESTRUCT);
		EXPECT_TRUE(op3 == OP_DESTRUCT);
	}

	TEST(system_memory_TUniquePtr, move_construct)
	{
		TUniquePtr<TTestException> p_te1 = MakeUniquePtr<TTestException>(new TTestException());
		EXPECT_TRUE(p_te1.Object() != NULL);
		TUniquePtr<TTestException> p_te2 = cl3::system::def::move(p_te1);
		EXPECT_TRUE(p_te1.Object() == NULL);
		EXPECT_TRUE(p_te2.Object() != NULL);
	}

	TEST(system_memory_TUniquePtr, move_assign)
	{
		TUniquePtr<TTestException> p_te1 = MakeUniquePtr<TTestException>(new TTestException());
		EXPECT_TRUE(p_te1.Object() != NULL);
		TUniquePtr<TTestException> p_te2 = MakeUniquePtr<TTestException>(new TTestException());
		EXPECT_TRUE(p_te2.Object() != NULL);
		p_te2 = cl3::system::def::move(p_te1);
		EXPECT_TRUE(p_te1.Object() == NULL);
		EXPECT_TRUE(p_te2.Object() != NULL);
	}

	TEST(system_memory, UniqueNew)
	{
		TUniquePtr<int> p = New<int>(7);
		EXPECT_EQ(7, *p);
	}
}
