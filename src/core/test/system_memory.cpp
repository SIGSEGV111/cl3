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
#include <cl3/core/util.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::memory;

	struct	TTestException {};

	struct	TFailAllocator : IDynamicAllocator
	{
		void*	Alloc	(usys_t) CL3_WARN_UNUSED_RESULT { throw TTestException(); }
		void	Free	(void*) { throw TTestException(); }
		void*	Realloc	(void*, usys_t, bool) CL3_WARN_UNUSED_RESULT  { throw TTestException(); }
		usys_t	SizeOf	(void*) const CL3_GETTER  { throw TTestException(); }
	};

	TEST(system_memory, Alloc_Realloc_Free)
	{
		byte_t* bytes = (byte_t*)Alloc(128, NULL);
		memset(bytes, 1, 128);
		bytes = (byte_t*)Realloc(bytes, 256, NULL, false);
		memset(bytes, 2, 256);
		Free(bytes);
	}

	TEST(system_memory, SizeOf)
	{
		byte_t* bytes = (byte_t*)Alloc(128, NULL);
		EXPECT_TRUE(SizeOf(bytes) >= 128 && SizeOf(bytes) < 128 + 128);
		bytes = (byte_t*)Realloc(bytes, 256, NULL, false);
		EXPECT_TRUE(SizeOf(bytes) >= 256 && SizeOf(bytes) < 256 + 128);
		Free(bytes);
	}

	TEST(system_memory, Alloc_Realloc_Free_w_allocator_switch)
	{
		TFailAllocator fail_allocator;
		byte_t* bytes = NULL;

		bytes = (byte_t*)Alloc(128, NULL);

		CL3_PARAMETER_STACK_PUSH(allocator, &fail_allocator);
		bytes = (byte_t*)Realloc(bytes, 256, NULL, false);
		Free(bytes);
	}

	TEST(system_memory, New_Delete)
	{
		//	fails if the libcl3-core is linked after libstdc++ (then the wrong operator new() implemention is used)
		int* x = new int(17);
		int* y = new int(56);
		int* z = new int(36);
		EXPECT_TRUE(*x == 17);
		EXPECT_TRUE(*y == 56);
		EXPECT_TRUE(*z == 36);
		IDynamicAllocator** p = reinterpret_cast<IDynamicAllocator**>(x)-1;
		EXPECT_TRUE(*p == CL3_PARAMETER_STACK_VALUE(allocator));
		delete x;
		delete y;
		delete z;
	}

	TEST(system_memory_TRestrictAllocator, bust_limit)
	{
		const usys_t sz_limit = 0x10000;
		TRestrictAllocator ra(CL3_PARAMETER_STACK_VALUE(allocator), sz_limit);
		CL3_PARAMETER_STACK_PUSH(allocator, &ra);

		int* x = new int(1);
		int* y = new int(2);
		int* z = new int(3);

		EXPECT_TRUE(*x == 1);
		EXPECT_TRUE(*y == 2);
		EXPECT_TRUE(*z == 3);

		byte_t* array = NULL;

		//	request one byte more than the limit (this must fail)
		EXPECT_THROW(array = new byte_t[sz_limit-sizeof(int)*3+1], TBadAllocException);

		//	if the above succeeded (which it should not, but anyway), we have to delete[] array again
		//	if the above did not succeeded (as it should), we can safely delete[] a NULL-pointer
		delete[] array;

		//	this should work (-256 because of alignment and padding which might be required)
		array = new byte_t[sz_limit-sizeof(int)*3-256];

		delete[] array;
		delete x;
		delete y;
		delete z;
	}

	TEST(system_memory_TRestrictAllocator, Realloc)
	{
		const usys_t sz_limit = 0x10000;
		TRestrictAllocator ra(CL3_PARAMETER_STACK_VALUE(allocator), sz_limit);

		void* array = ra.Alloc(17);
		array = ra.Realloc(array, 112, false);
		array = ra.Realloc(array, 200, false);
		array = ra.Realloc(array, 120, false);

		EXPECT_THROW(array = ra.Realloc(array, 0x10001, false), TBadAllocException);

		ra.Free(array);
	}

	TEST(system_memory_TRestrictAllocator, accounting)
	{
		//	this test could get influenced by gtest's own memory allocations (currently gtest does not seem to allocates additional memory during test execution)
		const usys_t sz_limit = 0x10000;
		usys_t sz_alloc_before = 0;

		TRestrictAllocator ra(CL3_PARAMETER_STACK_VALUE(allocator), sz_limit);
		EXPECT_TRUE(ra.BytesLimit() == sz_limit);
		EXPECT_TRUE((sz_alloc_before = ra.BytesAllocated()) == 0);
		CL3_PARAMETER_STACK_PUSH(allocator, &ra);

		int* x = new int(1);
		EXPECT_TRUE(ra.BytesAllocated() > sz_alloc_before);
		sz_alloc_before = ra.BytesAllocated();

		int* y = new int(2);
		EXPECT_TRUE(ra.BytesAllocated() > sz_alloc_before);
		sz_alloc_before = ra.BytesAllocated();

		int* z = new int(3);
		EXPECT_TRUE(ra.BytesAllocated() > sz_alloc_before);
		sz_alloc_before = ra.BytesAllocated();

		EXPECT_TRUE(*x == 1);
		EXPECT_TRUE(*y == 2);
		EXPECT_TRUE(*z == 3);

		EXPECT_TRUE(ra.BytesAllocated() >= 3*sizeof(int));
		EXPECT_TRUE(ra.BytesAllocated() == sz_alloc_before);
		sz_alloc_before = ra.BytesAllocated();

		delete x;
		EXPECT_TRUE(ra.BytesAllocated() >= 2*sizeof(int));
		EXPECT_TRUE(ra.BytesAllocated() < sz_alloc_before);
		sz_alloc_before = ra.BytesAllocated();

		delete y;
		EXPECT_TRUE(ra.BytesAllocated() >= 1*sizeof(int));
		EXPECT_TRUE(ra.BytesAllocated() < sz_alloc_before);
		sz_alloc_before = ra.BytesAllocated();

		delete z;
		EXPECT_TRUE(ra.BytesAllocated() == 0);
		EXPECT_TRUE(ra.BytesAllocated() < sz_alloc_before);
		sz_alloc_before = ra.BytesAllocated();

		EXPECT_TRUE(ra.BytesLimit() == sz_limit);
	}

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

		auto p1 = MakeSharedPtr<TCDCTester, THREADING_SINGLE>(&op1, 0);
		auto p2 = MakeSharedPtr<TCDCTester, THREADING_SINGLE>(&op2, 17);
		auto p3 = MakeSharedPtr<TCDCTester, THREADING_MULTI>(&op3, 112);

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

		p1 = NULL;

		EXPECT_TRUE(op1 == OP_ASSIGN);
		EXPECT_TRUE(op2 == OP_DESTRUCT);
		EXPECT_TRUE(op3 == OP_ASSIGN);

		p2 = NULL;

		EXPECT_TRUE(op1 == OP_DESTRUCT);
		EXPECT_TRUE(op2 == OP_DESTRUCT);
		EXPECT_TRUE(op3 == OP_ASSIGN);

		p3 = NULL;

		EXPECT_TRUE(op1 == OP_DESTRUCT);
		EXPECT_TRUE(op2 == OP_DESTRUCT);
		EXPECT_TRUE(op3 == OP_DESTRUCT);
	}

	TEST(system_memory_TUniquePtr, move_construct)
	{
		TUniquePtr<TTestException> p_te1 = MakeUniquePtr<TTestException>(new TTestException());
		EXPECT_TRUE(p_te1.Object() != NULL);
		TUniquePtr<TTestException> p_te2 = cl3::util::move(p_te1);
		EXPECT_TRUE(p_te1.Object() == NULL);
		EXPECT_TRUE(p_te2.Object() != NULL);
	}

	TEST(system_memory_TUniquePtr, move_assign)
	{
		TUniquePtr<TTestException> p_te1 = MakeUniquePtr<TTestException>(new TTestException());
		EXPECT_TRUE(p_te1.Object() != NULL);
		TUniquePtr<TTestException> p_te2 = MakeUniquePtr<TTestException>(new TTestException());
		EXPECT_TRUE(p_te2.Object() != NULL);
		p_te2 = cl3::util::move(p_te1);
		EXPECT_TRUE(p_te1.Object() == NULL);
		EXPECT_TRUE(p_te2.Object() != NULL);
	}
}