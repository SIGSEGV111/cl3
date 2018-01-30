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

#ifndef	_include_cl3_core_system_compiler_jit_hpp_
#define	_include_cl3_core_system_compiler_jit_hpp_

#include <cl3/core/system_compiler.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/system_library.hpp>

namespace	llvm
{
	class Module;
	class LLVMContext;
	class TargetMachine;
	class ExecutionEngine;
	class Function;
}

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			namespace	jit
			{
				class	TBinary : public library::ILibrary
				{
					protected:
						friend class TJIT;
						llvm::LLVMContext* context;
						llvm::Module* module;
						llvm::TargetMachine* tm;
						llvm::ExecutionEngine* ee;

						CLASS TBinary(const TBinary&) = delete;
						CL3PUBF CLASS TBinary(llvm::LLVMContext* context, llvm::Module* module, u8_t optlvl);

					public:
						CL3PUBF void* ResolveSymbol(const io::text::string::TString& name) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF void* ResolveSymbol(const char* name) final override CL3_WARN_UNUSED_RESULT;
						CL3PUBF void* ResolveSymbol(llvm::Function*) CL3_WARN_UNUSED_RESULT;


						CL3PUBF CLASS ~TBinary();
				};

				class	TJIT
				{
					protected:
						TJIT(const TJIT&) = delete;
						llvm::LLVMContext* context;
						llvm::Module* module;

					public:
						inline llvm::Module& Module() CL3_GETTER { return *this->module; }

						CL3PUBF	system::memory::TUniquePtr<TBinary> Compile(u8_t optlvl = 0) CL3_WARN_UNUSED_RESULT;

						CL3PUBF	CLASS	TJIT	();
						CL3PUBF	CLASS	~TJIT	();
				};
			}
		}
	}
}

#endif
