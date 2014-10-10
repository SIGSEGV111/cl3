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

#include <cl3/core/math_formular.hpp>
#include <cl3/core/io_text_string.hpp>
#include <gtest/gtest.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/TargetSelect.h>
// #include <llvm/PassManager.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/PassManager.h>

using namespace ::testing;

namespace
{
	using namespace cl3::math::formular;
	using namespace cl3::system::memory;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace llvm;
	using namespace std;

	TEST(math_formular, llvm)
	{
		InitializeNativeTarget();
		Module* module = new Module("test", getGlobalContext());
		std::string error_buffer;
		TUniquePtr<ExecutionEngine> execution_engine = MakeUniquePtr<ExecutionEngine>(EngineBuilder(module).setErrorStr(&error_buffer).setEngineKind(EngineKind::JIT).create());
		if(execution_engine == NULL)
		{
			puts(error_buffer.c_str());
			return;
		}

		TUniquePtr<IFormularNode> root = MakeUniquePtr<IFormularNode>(new TOperatorNode(
				MakeUniquePtr<IFormularNode>(new TLiteralNode(17.4)),
				MakeUniquePtr<IFormularNode>(new TLiteralNode(10.4)),
				OPERATION_ADD
			));


// 		module->setDataLayout(execution_engine->getDataLayout());
//
// 		FunctionPassManager fpm;
// // 		ModulePassManager mpm;
//
// 		PassManagerBuilder pmb;
// 		pmb.OptLevel = 3;
// 		pmb.populateFunctionPassManager(fpm);
// // 		pmb.populateModulePassManager(mpm);
//
		TUniquePtr<Function> code = root->GenerateCode(*module);
//
// 		fpm.run(code.Object());
// // 		mpm.run(module);

		module->dump();


		printf("execution_engine = %p\n", execution_engine.Object());
		double (*func)() = (double (*)())execution_engine->getPointerToFunction(code.Object());
		printf("func = %p\n", func);
		printf("result: %f\n", func());
	}
}
