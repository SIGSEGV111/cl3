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

#include <cl3/core/context.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::context;

	namespace	some_namespace
	{
		CL3_PARAMETER_STACK_DECL(int, my_int_stack);
		CL3_PARAMETER_STACK_IMPL(int, my_int_stack, 10);
	}

	static void some_function__int()
	{
		EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 17);
		CL3_PARAMETER_STACK_PUSH(some_namespace::my_int_stack, 0);
		EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 0);
	}

	TEST(Context, ParameterStack)
	{
		EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 10);

		{
			CL3_PARAMETER_STACK_PUSH(some_namespace::my_int_stack, 0);
			EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 0);

			{
				CL3_PARAMETER_STACK_PUSH(some_namespace::my_int_stack, 17);
				EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 17);

				some_function__int();

				EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 17);
			}

			EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 0);
		}

		EXPECT_TRUE(CL3_PARAMETER_STACK_VALUE(some_namespace::my_int_stack) == 10);
	}
}
