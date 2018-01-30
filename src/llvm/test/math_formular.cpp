/*
 *    libcl3 - common library version 3
 *    Copyright (C) 2013	Simon Brennecke
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cl3/llvm/math_formular.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/llvm/system_compiler_jit.hpp>
#include <llvm/IR/Module.h>

#include <gtest/gtest.h>


using namespace ::testing;

namespace
{
	using namespace cl3::math::formular;
	using namespace cl3::system::memory;
	using namespace cl3::system::compiler::jit;
	using namespace llvm;

	TEST(math_formular_TFormular, FirstTest)
	{
		TJIT jit;

		TFormular formular(5);

		formular += 10.0;

		float x = 0.1f;

		formular *= &x;

		TFormular bigger_than = formular > 1.0;

		TFormular less_or_equal = formular <= 4;

		Function* f1 = formular.GenerateCode(jit.Module());
		EXPECT_TRUE(f1 != NULL);

		Function* f2 = bigger_than.GenerateCode(jit.Module());
		EXPECT_TRUE(f2 != NULL);

		Function* f3 = less_or_equal.GenerateCode(jit.Module());
		EXPECT_TRUE(f3 != NULL);

// 		jit.Module().dump();;

		TUniquePtr<TBinary> binary = jit.Compile();

		double (*F1)() = (double (*)())binary->ResolveSymbol(f1);
		bool (*F2)() = (bool (*)())binary->ResolveSymbol(f2);
		bool (*F3)() = (bool (*)())binary->ResolveSymbol(f3);

// 		printf("%p\n%p\n%p\n", F1, F2, F3);

		EXPECT_TRUE(F1() > 1.49 && F1() < 1.51);
		EXPECT_TRUE(F2());
		EXPECT_TRUE(F3());

		x = 0.0f;

		EXPECT_EQ(0.0, F1());
		EXPECT_FALSE(F2());
		EXPECT_TRUE(F3());
	}
}
