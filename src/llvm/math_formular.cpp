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

#ifndef INSIDE_CL3LLVM
#error "compiling cl3 source code but macro INSIDE_CL3LLVM is not defined"
#endif

#include "math_formular.hpp"

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/ManagedStatic.h>

namespace	cl3
{
	namespace	math
	{
		namespace	formular
		{
			using namespace io::text::string;
			using namespace io::text::encoding;
			using namespace system::memory;
			using namespace node;
			using namespace error;
			using namespace system::types::typeinfo;
			using namespace llvm;

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

			static const TRTTI* ComputeBiggerType(const TRTTI* t1, const TRTTI* t2)
			{
				if( (t1->is_integer && t2->is_integer) || (t1->is_float && t2->is_float) )
					return t1->sz_bytes > t2->sz_bytes ? t1 : t2;
				else if( (t1->is_integer && t2->is_float) || (t1->is_float && t2->is_integer) )
					return &TCTTI<f64_t>::rtti;
				else
					CL3_NONCLASS_FAIL(TException, "unsupported datatype");
			}

			static Value* CheckAndBuildCast(Value* value, const TRTTI* type_source, const TRTTI* type_target, IRBuilder<>& builder)
			{
				Type* type_target_llvm = GetLLVMType(type_target, builder.getContext());

				if(value->getType() == type_target_llvm)
					return value;
				else
				{
					if(type_source->is_integer && type_target->is_integer)
					{
						if(type_source->is_signed)
							return builder.CreateCast(Instruction::SExt, value, type_target_llvm);
						else
							return builder.CreateCast(Instruction::ZExt, value, type_target_llvm);
					}
					else if(type_source->is_float && type_target->is_float)
					{
						return builder.CreateCast(Instruction::FPExt, value, type_target_llvm);
					}
					else if(type_source->is_integer && type_target->is_float)
					{
						if(type_source->is_signed)
							return builder.CreateCast(Instruction::SIToFP, value, type_target_llvm);
						else
							return builder.CreateCast(Instruction::UIToFP, value, type_target_llvm);
					}
					else if(type_source->is_float && type_target->is_integer)
					{
						if(type_target->is_signed)
							return builder.CreateCast(Instruction::FPToSI, value, type_target_llvm);
						else
							return builder.CreateCast(Instruction::FPToUI, value, type_target_llvm);
					}
					CL3_NONCLASS_FAIL(TException, "unsupported datatype");
				}
			}

			namespace node
			{
				CLASS	INode::~INode()
				{
				}

				Value*	TReferenceNode::GenerateCode	(IRBuilder<>& builder) const
				{
					Type* type_return = GetLLVMType(this->type, builder.getContext());

					CL3_CLASS_ERROR(!PointerType::isValidElementType(type_return), TException, "!isValidElementType");

					ConstantInt* addr = ConstantInt::get(Type::getIntNTy(builder.getContext(), sizeof(void*) * 8), reinterpret_cast<usys_t>(this->ptr));
					Value* value_ptr = ConstantExpr::getIntToPtr(addr, PointerType::getUnqual(type_return));

					return builder.CreateLoad(value_ptr, "");
				}

				const TRTTI* TReferenceNode::Decltype() const
				{
					return this->type;
				}

				CLASS	TReferenceNode::TReferenceNode				(const TRTTI* type, const void* ptr) : type(type), ptr(ptr)
				{
// 					printf("%p: TReferenceNode()\n", this);
				}

				CLASS	TReferenceNode::~TReferenceNode				()
				{
// 					printf("%p: ~TReferenceNode()\n", this);
				}

				Value*	TConstantNode::GenerateCode	(IRBuilder<>& builder) const
				{
					Type* type_return = GetLLVMType(this->value.type, builder.getContext());

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

					return c;
				}

				const TRTTI* TConstantNode::Decltype() const
				{
					return this->value.type;
				}

				CLASS	TConstantNode::TConstantNode				(const TRTTI* type, const void* data)
				{
// 					printf("%p: TConstantNode()\n", this);
					CL3_CLASS_ERROR(type->sz_bytes > sizeof(this->value.data), TException, "requested datatype too big");
					this->value.type = type;
					memcpy(this->value.data, data, type->sz_bytes);
					memset(this->value.data + type->sz_bytes, 0, sizeof(this->value.data) - type->sz_bytes);
				}

				CLASS	TConstantNode::~TConstantNode				()
				{
// 					printf("%p: ~TConstantNode()\n", this);
				}

				Value*	TBinaryOperatorNode::GenerateCode	(IRBuilder<>& builder) const
				{
					const TRTTI* type_work = ComputeBiggerType(this->lhs->Decltype(), this->rhs->Decltype());

					Value* v1 = CheckAndBuildCast(this->lhs->GenerateCode(builder), this->lhs->Decltype(), type_work, builder);
					Value* v2 = CheckAndBuildCast(this->rhs->GenerateCode(builder), this->rhs->Decltype(), type_work, builder);

					Value* r = NULL;

					switch(this->op)
					{
						case EOperation::ADD:
							if(type_work->is_integer)
								r = builder.CreateAdd(v1, v2);
							else if(type_work->is_float)
								r = builder.CreateFAdd(v1, v2);
							break;
						case EOperation::SUB:
							if(type_work->is_integer)
								r = builder.CreateSub(v1, v2);
							else if(type_work->is_float)
								r = builder.CreateFSub(v1, v2);
							break;
						case EOperation::MUL:
							if(type_work->is_integer)
								r = builder.CreateMul(v1, v2);
							else if(type_work->is_float)
								r = builder.CreateFMul(v1, v2);
							break;
						case EOperation::DIV:
							if(type_work->is_integer)
							{
								if(type_work->is_signed)
									r = builder.CreateSDiv(v1, v2);
								else
									r = builder.CreateUDiv(v1, v2);
							}
							else if(type_work->is_float)
								r = builder.CreateFAdd(v1, v2);
							break;
						case EOperation::MOD:
							break;
						case EOperation::POW:
							break;
						case EOperation::BIGGER_THAN:
							if(type_work->is_integer)
							{
								if(type_work->is_signed)
									r = builder.CreateICmpSGT(v1, v2);
								else
									r = builder.CreateICmpUGT(v1, v2);
							}
							else if(type_work->is_float)
								r = builder.CreateFCmpOGT(v1, v2);
							break;
						case EOperation::LESS_THAN:
							if(type_work->is_integer)
							{
								if(type_work->is_signed)
									r = builder.CreateICmpSLT(v1, v2);
								else
									r = builder.CreateICmpULT(v1, v2);
							}
							else if(type_work->is_float)
								r = builder.CreateFCmpOLT(v1, v2);
							break;
						case EOperation::BIGGER_OR_EQUAL_THAN:
							if(type_work->is_integer)
							{
								if(type_work->is_signed)
									r = builder.CreateICmpSGE(v1, v2);
								else
									r = builder.CreateICmpUGE(v1, v2);
							}
							else if(type_work->is_float)
								r = builder.CreateFCmpOGE(v1, v2);
							break;
						case EOperation::LESS_OR_EQUAL_THAN:
							if(type_work->is_integer)
							{
								if(type_work->is_signed)
									r = builder.CreateICmpSLE(v1, v2);
								else
									r = builder.CreateICmpULE(v1, v2);
							}
							else if(type_work->is_float)
								r = builder.CreateFCmpOLE(v1, v2);
							break;
						case EOperation::EQUAL:
							if(type_work->is_integer)
								r = builder.CreateICmpEQ(v1, v2);
							else if(type_work->is_float)
								r = builder.CreateFCmpOEQ(v1, v2);
							break;
						case EOperation::NOT_EQUAL:
							if(type_work->is_integer)
								r = builder.CreateICmpNE(v1, v2);
							else if(type_work->is_float)
								r = builder.CreateFCmpONE(v1, v2);
							break;
					}

					CL3_CLASS_ERROR(r == NULL, TException, "unable to generate code for requested operation");

					switch(this->op)
					{
						case EOperation::ADD:
						case EOperation::SUB:
						case EOperation::MUL:
						case EOperation::DIV:
						case EOperation::MOD:
						case EOperation::POW:
							break;

						case EOperation::BIGGER_THAN:
						case EOperation::LESS_THAN:
						case EOperation::BIGGER_OR_EQUAL_THAN:
						case EOperation::LESS_OR_EQUAL_THAN:
						case EOperation::EQUAL:
						case EOperation::NOT_EQUAL:
							r = builder.CreateCast(Instruction::ZExt, r, Type::getIntNTy(builder.getContext(), 8));
							break;
					}

					return r;
				}

				const TRTTI* TBinaryOperatorNode::Decltype() const
				{
					switch(this->op)
					{
						case EOperation::ADD:
						case EOperation::SUB:
						case EOperation::MUL:
						case EOperation::DIV:
						case EOperation::MOD:
						case EOperation::POW:
							return ComputeBiggerType(this->lhs->Decltype(), this->rhs->Decltype());

						case EOperation::BIGGER_THAN:
						case EOperation::LESS_THAN:
						case EOperation::BIGGER_OR_EQUAL_THAN:
						case EOperation::LESS_OR_EQUAL_THAN:
						case EOperation::EQUAL:
						case EOperation::NOT_EQUAL:
							return &TCTTI<bool>::rtti;
					}

					CL3_CLASS_FAIL(TException, "unknown operation");
				}

				CLASS	TBinaryOperatorNode::TBinaryOperatorNode	(EOperation op, TSharedPtr<INode> lhs, TSharedPtr<INode> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs))
				{
// 					printf("%p: TBinaryOperatorNode()\n", this);
				}

				CLASS	TBinaryOperatorNode::~TBinaryOperatorNode	()
				{
// 					printf("%p: ~TBinaryOperatorNode()\n", this);
				}

				Value*	TFunctionCallNode::GenerateCode	(IRBuilder<>&) const
				{
					CL3_NOT_IMPLEMENTED;	//	TODO
				}

				const TRTTI* TFunctionCallNode::Decltype() const
				{
					return this->type_return;
				}

				CLASS	TFunctionCallNode::TFunctionCallNode		(void* func, const TRTTI* type_return , const TRTTI* type_arg, TSharedPtr<INode*> arg) : func(func), type_return(type_return), type_arg(type_arg), arg(arg)
				{
// 					printf("%p: TFunctionCallNode()\n", this);
				}


				CLASS	TFunctionCallNode::~TFunctionCallNode		()
				{
// 					printf("%p: ~TFunctionCallNode()\n", this);
				}
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

			TFormular&	TFormular::operator=	(const TFormular& rhs)
			{
				this->node_root = rhs.node_root;
				return *this;
			}

			TFormular&	TFormular::operator=	(TFormular&& rhs)
			{
				this->node_root = std::move(rhs.node_root);
				return *this;
			}

			//	creates a new sub-expression which refers to the pointed-to variable
			CLASS		TFormular::TFormular	(const f32_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const f64_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const u8_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const s8_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const u16_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const s16_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const u32_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const s32_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const u64_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const s64_t* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
			{
			}

			CLASS		TFormular::TFormular	(const bool* v) : node_root(MakeSharedPtr(new TReferenceNode(&TCTTI<std::remove_cv<decltype(v)>::type>::rtti, v)))
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

			Function*	TFormular::GenerateCode	(Module& module) const
			{
				char name[32];
				snprintf(name, sizeof(name), "TFormular_%p", this);

				FunctionType* type_function =  FunctionType::get(GetLLVMType(this->node_root->Decltype(), module.getContext()), ArrayRef<Type*>(), false);

				IRBuilder<> builder(module.getContext());
				Function* function = Function::Create(type_function, Function::ExternalLinkage, name, &module);
				BasicBlock *bb = BasicBlock::Create(module.getContext(), "", function);
				builder.SetInsertPoint(bb);
				builder.CreateRet(this->node_root->GenerateCode(builder));

				CL3_CLASS_ERROR(verifyFunction(*function, NULL), TException, "failure during verification");

				return function;
			}

			Function*	TFormular::GenerateCode	(Module& module, const io::text::string::TString& name) const
			{
				FunctionType* type_function =  FunctionType::get(GetLLVMType(this->node_root->Decltype(), module.getContext()), ArrayRef<Type*>(), false);

				IRBuilder<> builder(module.getContext());
				Function* function = Function::Create(type_function, Function::ExternalLinkage, TCString(name, CODEC_CXX_CHAR).Chars(), &module);
				BasicBlock *bb = BasicBlock::Create(module.getContext(), "", function);
				builder.SetInsertPoint(bb);
				builder.CreateRet(this->node_root->GenerateCode(builder));

				CL3_CLASS_ERROR(verifyFunction(*function, NULL), TException, "failure during verification");

				return function;
			}

			const TRTTI*	TFormular::Decltype() const
			{
				return this->node_root->Decltype();
			}
		}
	}
}
