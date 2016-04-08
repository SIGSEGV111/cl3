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

#include <cl3/core/math_formular.hpp>
#include <cl3/core/io_text_string.hpp>
#include <gtest/gtest.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

using namespace ::testing;

namespace
{
	using namespace cl3::math::formular;
	using namespace cl3::system::memory;
	using namespace llvm;

	TEST(math_formular_TFormular, Reference)
	{
		static LLVMContext context;
		Module m("", context);

		int x = 5;
		TFormular formular(&x);

		EXPECT_TRUE(formular.GenerateCode(m) != NULL);

		m.dump();
	}

	TEST(math_formular_TFormular, Constant_Int)
	{
		static LLVMContext context;
		Module m("", context);
		TFormular formular(5L);

		EXPECT_TRUE(formular.GenerateCode(m) != NULL);

		m.dump();
	}

	TEST(math_formular_TFormular, Constant_Float)
	{
		static LLVMContext context;
		Module m("", context);
		TFormular formular(5.0);

		EXPECT_TRUE(formular.GenerateCode(m) != NULL);

		m.dump();
	}

	TEST(math_formular_TFormular, AddTwoConstants)
	{
		static LLVMContext context;
		Module m("", context);
		TFormular formular(5);
		formular += 10;

		EXPECT_TRUE(formular.GenerateCode(m) != NULL);

		m.dump();
	}
}
