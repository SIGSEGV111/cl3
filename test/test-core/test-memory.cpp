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
#include <cl3/core/system_memory.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
		void*	Realloc	(void*, usys_t) CL3_WARN_UNUSED_RESULT  { throw TTestException(); }
		usys_t	SizeOf	(void*) const GETTER  { throw TTestException(); }
	};

	TEST(MemoryAllocator, Alloc_Realloc_Free)
	{
		byte_t* bytes = (byte_t*)Alloc(128);
		memset(bytes, 1, 128);
		bytes = (byte_t*)Realloc(bytes, 256);
		memset(bytes, 2, 256);
		Free(bytes);
	}

	TEST(MemoryAllocator, Alloc_Realloc_Free_w_allocator_switch)
	{
		TFailAllocator fail_allocator;
		byte_t* bytes = NULL;

		bytes = (byte_t*)Alloc(128);

		CL3_PARAMETER_STACK_PUSH(allocator, &fail_allocator);
		bytes = (byte_t*)Realloc(bytes, 256);
		Free(bytes);
	}

	TEST(MemoryAllocator, New_Delete)
	{
		//	fails if the libcl3-core is linked after libstdc++ (then the wrong operator new() implemention is used)
		int* x = new int(17);
		EXPECT_TRUE(*x == 17);
		IDynamicAllocator** p = reinterpret_cast<IDynamicAllocator**>(x)-1;
		EXPECT_TRUE(*p == CL3_PARAMETER_STACK_VALUE(allocator));
		delete x;
	}

	TEST(MemoryAllocator, TRestrictAllocator)
	{
		TRestrictAllocator ra(CL3_PARAMETER_STACK_VALUE(allocator), 0x10000);
		CL3_PARAMETER_STACK_PUSH(allocator, &ra);

		int* x = new int(1);
		int* y = new int(2);
		int* z = new int(3);

		EXPECT_TRUE(*x == 1);
		EXPECT_TRUE(*y == 2);
		EXPECT_TRUE(*z == 3);

		byte_t* array = NULL;

		EXPECT_THROW(array = new byte_t[0x10000], TBadAllocException);

		delete x;
		delete y;
		delete z;
	}
}
