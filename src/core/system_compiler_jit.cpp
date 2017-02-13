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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "system_compiler_jit.hpp"

#include <memory>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/ManagedStatic.h>

#include <string>

namespace
{
	struct T
	{
		T()
		{
			llvm::InitializeNativeTarget();
			llvm::InitializeNativeTargetAsmPrinter();
		}

		~T()
		{
		}
	};

	static T t;
	static llvm::llvm_shutdown_obj __fix_the_leaks;
}

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			namespace	jit
			{
				using namespace system::memory;
				using namespace llvm;

				void* TBinary::ResolveSymbol(const io::text::string::TString&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void* TBinary::ResolveSymbol(const char*)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void* TBinary::ResolveSymbol(Function* function)
				{
					return this->ee->getPointerToFunction(function);
				}

				CLASS TBinary::TBinary(llvm::LLVMContext* context, llvm::Module* module, u8_t /*optlvl*/) : context(context), module(module)
				{
					EngineBuilder eb((std::unique_ptr<llvm::Module>(module)));

					std::string err;

					eb.setErrorStr(&err);
					this->tm = eb.selectTarget();
					//eb.setUseMCJIT(true);
					eb.setEngineKind(EngineKind::Kind::JIT);
					eb.setOptLevel(CodeGenOpt::Level::Default);
					this->ee = eb.create(tm);

					CL3_CLASS_ERROR(ee == NULL, error::TException, err.c_str());

					ee->finalizeObject();
				}

				CLASS TBinary::~TBinary()
				{
					delete this->ee;
					delete this->context;
				}

				/**********************************************************/

				TUniquePtr<TBinary> TJIT::Compile(u8_t optlvl)
				{
					TUniquePtr<TBinary> binary = MakeUniquePtr(new TBinary(this->context, this->module, optlvl));

					this->context = new LLVMContext();
					this->module = new llvm::Module("", *this->context);

// 					GenericValue v = ee->runFunction(function, std::vector<llvm::GenericValue>());

					return binary;
				}

				CLASS	TJIT::TJIT	()
				{
					this->context = new LLVMContext();
					this->module = new llvm::Module("", *this->context);
				}

				CLASS	TJIT::~TJIT	()
				{
					delete this->module;
					delete this->context;
				}
			}
		}
	}
}
