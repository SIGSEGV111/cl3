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

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::memory;

	struct	TTestException {};

	struct	TTestAllocator : IDynamicAllocator
	{
		void*	Alloc	(size_t sz)		{ IDynamicAllocator** p = (IDynamicAllocator**)::malloc(sz + sizeof(IDynamicAllocator*)); *p = this; return p+1; }
		void*	Realloc	(void*, size_t)	{ throw TTestException(); }
		void	Free	(void*)			{ throw TTestException(); }
		size_t	SizeOf	(void*)			{ throw TTestException(); }
	};

	static TTestAllocator test_allocator;

	TEST(MemoryAllocator, Alloc_Realloc_Free)
	{
		byte* bytes = (byte*)CL3_PARAMETER_STACK_VALUE(allocator)->Alloc(128);
		memset(bytes, 1, 128);
		CL3_PARAMETER_STACK_VALUE(allocator)->Realloc(bytes, 256);
		memset(bytes, 2, 256);
		CL3_PARAMETER_STACK_VALUE(allocator)->Free(bytes);
	}

	/*TEST(MemoryAllocator, Alloc_Realloc_Free_w_allocator_switch)
	{
		byte* bytes = NULL;

		{
			CL3_PARAMETER_STACK_PUSH(allocator, &test_allocator);
			bytes = (byte*)CL3_PARAMETER_STACK_VALUE(allocator)->Alloc(128);
		}

		EXPECT_THROW(CL3_PARAMETER_STACK_VALUE(allocator)->Realloc(bytes, 256), TTestException);
		EXPECT_THROW(CL3_PARAMETER_STACK_VALUE(allocator)->Free(bytes), TTestException);
	}*/

	/*TEST(MemoryAllocator, New_Delete)
	{
		//	fails if the libcl3-core is linked after libstdc++ (then the wrong operator new() implemention is used)
		int* x = new int(17);
		EXPECT_TRUE(*x == 17);
		IDynamicAllocator** p = reinterpret_cast<IDynamicAllocator**>(x);
		p--;
		EXPECT_TRUE(*p == CL3_PARAMETER_STACK_VALUE(allocator));
		delete x;
	}*/

	/*TEST(MemoryAllocator, TRestrictAllocator)
	{
		TRestrictAllocator ra(CL3_PARAMETER_STACK_VALUE(allocator), 0x10000);
		CL3_PARAMETER_STACK_PUSH(allocator, &ra);

		int* x = new int(1);
		int* y = new int(2);
		int* z = new int(3);

		EXPECT_TRUE(*x == 1);
		EXPECT_TRUE(*y == 2);
		EXPECT_TRUE(*z == 3);

		byte* array = NULL;

		try
		{
			array = new byte[0x10000];
		}
		catch(...)
		{
		}

		delete x;
		delete y;
		delete z;
	}*/
}
