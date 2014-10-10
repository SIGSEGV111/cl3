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

#include "math_formular.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>


namespace	cl3
{
	namespace	math
	{
		namespace	formular
		{
			using namespace system::memory;
			using namespace io::text::string;
			using namespace llvm;

			CLASS	IFormularNode::~IFormularNode	()
			{
			}

			/*********************************************/

			system::memory::TUniquePtr<Function>
					TLiteralNode::GenerateCode	(llvm::Module& module) const
			{
				std::vector<Type*> __dummy_args(0, Type::getDoubleTy(getGlobalContext()));
				FunctionType* ft = FunctionType::get(Type::getDoubleTy(getGlobalContext()), __dummy_args, false);
				Function* f = Function::Create(ft, Function::PrivateLinkage, "TLiteralNode", &module);
				BasicBlock* bb = BasicBlock::Create(getGlobalContext(), "entry", f);
				IRBuilder<> builder(getGlobalContext());
				builder.SetInsertPoint(bb);
				builder.CreateRet(ConstantFP::get(getGlobalContext(), APFloat(this->value)));
				verifyFunction(*f);
				return MakeUniquePtr<Function>(f);
			}

			TLiteralNode::operator TString	() const
			{
				TString s;
				s<<this->value;
				return s;
			}

			CLASS	TLiteralNode::TLiteralNode	(double value) : value(value)
			{
			}

			CLASS	TLiteralNode::TLiteralNode	(const TLiteralNode& other) : value(other.value)
			{
			}

			/*********************************************/

			system::memory::TUniquePtr<Function>
					TOperatorNode::GenerateCode	(llvm::Module& module) const
			{
				std::vector<Type*> __dummy_args(0, Type::getDoubleTy(getGlobalContext()));
				FunctionType* ft = FunctionType::get(Type::getDoubleTy(getGlobalContext()), __dummy_args, false);
				Function* f = Function::Create(ft, Function::PrivateLinkage, "TOperatorNode", &module);
				BasicBlock* bb = BasicBlock::Create(getGlobalContext(), "entry", f);
				IRBuilder<> builder(getGlobalContext());
				builder.SetInsertPoint(bb);

				CallInst* call_lhs = builder.CreateCall(lhs->GenerateCode(module).Claim());
				CallInst* call_rhs = builder.CreateCall(rhs->GenerateCode(module).Claim());
				Value* value = NULL;

				switch(op)
				{
					case OPERATION_ADD: value = builder.CreateFAdd(call_lhs, call_rhs, "addtmp"); break;
					case OPERATION_SUB: value = builder.CreateFSub(call_lhs, call_rhs, "subtmp"); break;
					case OPERATION_MUL: value = builder.CreateFMul(call_lhs, call_rhs, "multmp"); break;
					case OPERATION_DIV: value = builder.CreateFDiv(call_lhs, call_rhs, "divtmp"); break;
					case OPERATION_MOD: value = builder.CreateFRem(call_lhs, call_rhs, "modtmp"); break;
				}

				builder.CreateRet(value);

				verifyFunction(*f);
				return MakeUniquePtr<Function>(f);
			}

			TOperatorNode::operator io::text::string::TString	() const
			{
				TString s;
				s<<'('<<*lhs;
				switch(op)
				{
					case OPERATION_ADD: s<<" + "; break;
					case OPERATION_SUB: s<<" - "; break;
					case OPERATION_MUL: s<<" * "; break;
					case OPERATION_DIV: s<<" / "; break;
					case OPERATION_MOD: s<<" % "; break;
				}
				s<<*rhs<<')';
				return s;
			}

			CLASS	TOperatorNode::TOperatorNode	(TUniquePtr<IFormularNode>&& lhs, TUniquePtr<IFormularNode>&& rhs, EOperation op)
				: lhs(system::def::move(lhs)), rhs(system::def::move(rhs)), op(op)
			{
			}
		}
	}
}
