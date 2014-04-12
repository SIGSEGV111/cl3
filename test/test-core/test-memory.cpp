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
		usys_t	SizeOf	(void*) const GETTER  { throw TTestException(); }
	};

	TEST(Memory, Alloc_Realloc_Free)
	{
		byte_t* bytes = (byte_t*)Alloc(128, NULL);
		memset(bytes, 1, 128);
		bytes = (byte_t*)Realloc(bytes, 256, NULL, false);
		memset(bytes, 2, 256);
		Free(bytes);
	}

	TEST(Memory, Alloc_Realloc_Free_w_allocator_switch)
	{
		TFailAllocator fail_allocator;
		byte_t* bytes = NULL;

		bytes = (byte_t*)Alloc(128, NULL);

		CL3_PARAMETER_STACK_PUSH(allocator, &fail_allocator);
		bytes = (byte_t*)Realloc(bytes, 256, NULL, false);
		Free(bytes);
	}

	TEST(Memory, New_Delete)
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

	TEST(RestrictAllocator, bust_limit)
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

	TEST(RestrictAllocator, accounting)
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
}
