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
#include <llvm/Support/ManagedStatic.h>

namespace	cl3
{
	namespace	math
	{
		namespace	formular
		{
			using namespace io::text::string;
			using namespace system::memory;
			using namespace node;
			using namespace error;
			using namespace system::types::typeinfo;
			using namespace llvm;

			static llvm_shutdown_obj __fix_the_leaks;

			static Type* GetLLVMType(const TRTTI* type, LLVMContext& context)
			{
				if(type->is_integer || type->is_bool)
					return Type::getIntNTy(context, type->sz_bytes * 8);
				else if(type->is_float)
					switch(type->sz_bytes)
					{
						case 4: return Type::getFloatTy(context);
						case 8: return Type::getDoubleTy(context);
					}

				CL3_NONCLASS_FAIL(TException, "unsupported datatype");
			}

			namespace node
			{
				CLASS	INode::~INode()
				{
				}

				llvm::Function*	TReferenceNode::GenerateCode	(llvm::Module& module) const
				{
					Type* type_return = GetLLVMType(this->type, module.getContext());

					CL3_CLASS_ERROR(!PointerType::isValidElementType(type_return), TException, "!isValidElementType");

					ConstantInt* addr = ConstantInt::get(Type::getIntNTy(module.getContext(), sizeof(void*) * 8), reinterpret_cast<usys_t>(this->ptr));
					Value* value_ptr = ConstantExpr::getIntToPtr(addr, PointerType::getUnqual(type_return));

					FunctionType* type_function =  FunctionType::get(
						type_return,		//	set the return type to the type specified for the reference variable
						ArrayRef<Type*>(),	//	function takes no parameters (empty array)
						false	//	!isVarArg
					);

					IRBuilder<> builder(module.getContext());
					Function* function = Function::Create(type_function, Function::PrivateLinkage, "", &module);
					BasicBlock *bb = BasicBlock::Create(module.getContext(), "entry", function);
					builder.SetInsertPoint(bb);
					LoadInst* li = builder.CreateLoad(value_ptr, "");
					builder.CreateRet(li);

					return function;
				}

				const system::types::typeinfo::TRTTI* TReferenceNode::Decltype() const
				{
					return this->type;
				}

				CLASS	TReferenceNode::TReferenceNode				(const TRTTI* type, const void* ptr) : type(type), ptr(ptr) {}

				llvm::Function*	TConstantNode::GenerateCode	(llvm::Module& module) const
				{
					Type* type_return = GetLLVMType(this->value.type, module.getContext());

					FunctionType* type_function =  FunctionType::get(
						type_return,		//	set the return type to the type specified for the reference variable
						ArrayRef<Type*>(),	//	function takes no parameters (empty array)
						false	//	!isVarArg
					);

					IRBuilder<> builder(module.getContext());
					Function* function = Function::Create(type_function, Function::PrivateLinkage, "", &module);
					BasicBlock *bb = BasicBlock::Create(module.getContext(), "entry", function);
					builder.SetInsertPoint(bb);

					Constant* c = NULL;
					if(this->value.type->is_integer || this->value.type->is_bool)
						switch(this->value.type->sz_bytes)
						{
							case 1: c = ConstantInt::get(type_return, (u64_t)*reinterpret_cast<const u8_t*>(this->value.data), this->value.type->is_signed); break;
							case 2: c = ConstantInt::get(type_return, (u64_t)*reinterpret_cast<const u16_t*>(this->value.data), this->value.type->is_signed); break;
							case 4: c = ConstantInt::get(type_return, (u64_t)*reinterpret_cast<const u32_t*>(this->value.data), this->value.type->is_signed); break;
							case 8: c = ConstantInt::get(type_return, (u64_t)*reinterpret_cast<const u64_t*>(this->value.data), this->value.type->is_signed); break;
						}
					else if(this->value.type->is_float)
						switch(this->value.type->sz_bytes)
						{
							case 4: c = ConstantFP::get(type_return, *reinterpret_cast<const f32_t*>(this->value.data)); break;
							case 8: c = ConstantFP::get(type_return, *reinterpret_cast<const f64_t*>(this->value.data)); break;
						}

					CL3_CLASS_ERROR(c == NULL, TException, "unsupported datatype");

					builder.CreateRet(c);

					return function;
				}

				const system::types::typeinfo::TRTTI* TConstantNode::Decltype() const
				{
					return this->value.type;
				}

				CLASS	TConstantNode::TConstantNode				(const TRTTI* type, const void* data)
				{
					CL3_CLASS_ERROR(type->sz_bytes > sizeof(this->value.data), TException, "requested datatype too big");
					this->value.type = type;
					memcpy(this->value.data, data, type->sz_bytes);
					memset(this->value.data + type->sz_bytes, 0, sizeof(this->value.data) - type->sz_bytes);
				}

				llvm::Function*	TBinaryOperatorNode::GenerateCode	(llvm::Module& module) const
				{
					Function* f1 = this->lhs->GenerateCode(module);
					Function* f2 = this->rhs->GenerateCode(module);

					Type* type_return = NULL;

					switch(this->op)
					{
						case EOperation::ADD:
							type_return = f1->getReturnType();
							break;
						case EOperation::SUB:
							break;
						case EOperation::MUL:
							break;
						case EOperation::DIV:
							break;
						case EOperation::MOD:
							break;
						case EOperation::POW:
							break;
						case EOperation::BIGGER_THAN:
							break;
						case EOperation::LESS_THAN:
							break;
						case EOperation::BIGGER_OR_EQUAL_THAN:
							break;
						case EOperation::LESS_OR_EQUAL_THAN:
							break;
						case EOperation::EQUAL:
							break;
						case EOperation::NOT_EQUAL:
							break;
					}

					CL3_CLASS_ERROR(type_return == NULL, TException, "cannot determine return type");

					FunctionType* type_function =  FunctionType::get(
						type_return,		//	set the return type to the type specified for the reference variable
						ArrayRef<Type*>(),	//	function takes no parameters (empty array)
						false	//	!isVarArg
					);

					IRBuilder<> builder(module.getContext());
					Function* function = Function::Create(type_function, Function::PrivateLinkage, "", &module);
					BasicBlock *bb = BasicBlock::Create(module.getContext(), "entry", function);
					builder.SetInsertPoint(bb);

					CallInst* c1 = builder.CreateCall(f1, ArrayRef<Value*>());
					CallInst* c2 = builder.CreateCall(f2, ArrayRef<Value*>());

					Value* r = NULL;

					switch(this->op)
					{
						case EOperation::ADD:
							r = builder.CreateAdd(c1, c2);
							break;
						case EOperation::SUB:
							r = builder.CreateSub(c1, c2);
							break;
						case EOperation::MUL:
							break;
						case EOperation::DIV:
							break;
						case EOperation::MOD:
							break;
						case EOperation::POW:
							break;
						case EOperation::BIGGER_THAN:
							break;
						case EOperation::LESS_THAN:
							break;
						case EOperation::BIGGER_OR_EQUAL_THAN:
							break;
						case EOperation::LESS_OR_EQUAL_THAN:
							break;
						case EOperation::EQUAL:
							break;
						case EOperation::NOT_EQUAL:
							break;
					}

					CL3_CLASS_ERROR(r == NULL, TException, "unable to generate code for requested operation");

					builder.CreateRet(r);

					return function;
				}

				const system::types::typeinfo::TRTTI* TBinaryOperatorNode::Decltype() const
				{
					CL3_NOT_IMPLEMENTED;	//	TODO
				}

				CLASS	TBinaryOperatorNode::TBinaryOperatorNode	(EOperation op, TSharedPtr<INode> lhs, TSharedPtr<INode> rhs) : op(op), lhs(lhs), rhs(rhs) {}

				llvm::Function*	TFunctionCallNode::GenerateCode	(llvm::Module& module) const
				{
					CL3_NOT_IMPLEMENTED;	//	TODO
				}

				const system::types::typeinfo::TRTTI* TFunctionCallNode::Decltype() const
				{
					return this->type_return;
				}

				CLASS	TFunctionCallNode::TFunctionCallNode		(void* func, const system::types::typeinfo::TRTTI* type_return , const system::types::typeinfo::TRTTI* type_arg, TSharedPtr<INode*> arg) : func(func), type_return(type_return), type_arg(type_arg), arg(arg) {}
			}

			TFormular	TFormular::operator+	(const TFormular& rhs) const
			{
				return TFormular(*this) += rhs;
			}

			TFormular	TFormular::operator-	(const TFormular& rhs) const
			{
				return TFormular(*this) -= rhs;
			}

			TFormular	TFormular::operator*	(const TFormular& rhs) const
			{
				return TFormular(*this) *= rhs;
			}

			TFormular	TFormular::operator/	(const TFormular& rhs) const
			{
				return TFormular(*this) /= rhs;
			}

			TFormular	TFormular::operator%	(const TFormular& rhs) const
			{
				return TFormular(*this) %= rhs;
			}

			TFormular	TFormular::operator^	(const TFormular& rhs) const
			{
				return TFormular(*this) ^= rhs;
			}

			TFormular	TFormular::operator>	(const TFormular& rhs) const
			{
				return TFormular(MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::BIGGER_THAN, this->node_root, rhs.node_root)));
			}

			TFormular	TFormular::operator<	(const TFormular& rhs) const
			{
				return TFormular(MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::LESS_THAN, this->node_root, rhs.node_root)));
			}

			TFormular	TFormular::operator>=	(const TFormular& rhs) const
			{
				return TFormular(MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::BIGGER_OR_EQUAL_THAN, this->node_root, rhs.node_root)));
			}

			TFormular	TFormular::operator<=	(const TFormular& rhs) const
			{
				return TFormular(MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::LESS_OR_EQUAL_THAN, this->node_root, rhs.node_root)));
			}

			TFormular	TFormular::operator==	(const TFormular& rhs) const
			{
				return TFormular(MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::EQUAL, this->node_root, rhs.node_root)));
			}

			TFormular	TFormular::operator!=	(const TFormular& rhs) const
			{
				return TFormular(MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::NOT_EQUAL, this->node_root, rhs.node_root)));
			}

			//	appends to the current formular object
			TFormular&	TFormular::operator+=	(const TFormular& rhs)
			{
				this->node_root = MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::ADD, this->node_root, rhs.node_root));
				return *this;
			}

			TFormular&	TFormular::operator-=	(const TFormular& rhs)
			{
				this->node_root = MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::SUB, this->node_root, rhs.node_root));
				return *this;
			}

			TFormular&	TFormular::operator*=	(const TFormular& rhs)
			{
				this->node_root = MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::MUL, this->node_root, rhs.node_root));
				return *this;
			}

			TFormular&	TFormular::operator/=	(const TFormular& rhs)
			{
				this->node_root = MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::DIV, this->node_root, rhs.node_root));
				return *this;
			}

			TFormular&	TFormular::operator%=	(const TFormular& rhs)
			{
				this->node_root = MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::MOD, this->node_root, rhs.node_root));
				return *this;
			}

			TFormular&	TFormular::operator^=	(const TFormular& rhs)
			{
				this->node_root = MakeSharedPtr(new TBinaryOperatorNode(TBinaryOperatorNode::EOperation::POW, this->node_root, rhs.node_root));
				return *this;
			}

			//	creates a new sub-expression which refers to the pointed-to variable
			CLASS		TFormular::TFormular	(const f32_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const f64_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u8_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s8_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u16_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s16_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u32_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s32_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u64_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s64_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const bool* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			//	creates a new sub-expression which holds a copy of the variable
			CLASS		TFormular::TFormular	(const f32_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const f64_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u8_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s8_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u16_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s16_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u32_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s32_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const u64_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const s64_t v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const bool v) : node_root(MakeSharedPtr(new TConstantNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, &v)))
			{
			}

			CLASS		TFormular::TFormular	(const TFormular& other) : node_root(other.node_root)
			{
			}

			CLASS		TFormular::TFormular	(TFormular&& other) : node_root(std::move(other.node_root))
			{
			}

			CLASS		TFormular::TFormular	(system::memory::TSharedPtr<node::INode> node_root) : node_root(node_root)
			{
			}

			CLASS		TFormular::TFormular	(const io::text::string::TString&)
			{
				CL3_NOT_IMPLEMENTED;	//	TODO
			}

			llvm::Function*	TFormular::GenerateCode	(llvm::Module& module) const
			{
				return this->node_root->GenerateCode(module);
			}

			const system::types::typeinfo::TRTTI*	TFormular::Decltype() const
			{
				return this->node_root->Decltype();
			}
		}
	}
}
