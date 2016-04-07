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

namespace	cl3
{
	namespace	math
	{
		namespace	formular
		{
			using namespace io::text::string;
			using namespace system::memory;
			using namespace node;

// 			template<TValue::EType type>
// 			struct	get_type;
//
// 			template<>
// 			struct	get_type<TValue::EType::S8>
// 			{
// 				typedef s8_t T;
// 			};

			namespace node
			{
				CLASS	INode::~INode()
				{
				}

				TValue::EType TReferenceNode::Decltype() const
				{
					return this->type;
				}

				CLASS	TReferenceNode::TReferenceNode				(TValue::EType type, const void* ptr) : type(type), ptr(ptr) {}

				TValue::EType TConstantNode::Decltype() const
				{
					return this->value.type;
				}

				CLASS	TConstantNode::TConstantNode				(TValue value) : value(value) {}

				TValue::EType TBinaryOperatorNode::Decltype() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TBinaryOperatorNode::TBinaryOperatorNode	(EOperation op, TSharedPtr<INode> lhs, TSharedPtr<INode> rhs) : op(op), lhs(lhs), rhs(rhs) {}

				TValue::EType TFunctionCallNode::Decltype() const
				{
					return this->type_return;
				}

				CLASS	TFunctionCallNode::TFunctionCallNode		(void* func, TValue::EType type_return , TValue::EType type_arg, TSharedPtr<INode*> arg) : func(func), type_return(type_return), type_arg(type_arg), arg(arg) {}
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
			CLASS		TFormular::TFormular	(const f32_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const f64_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u8_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s8_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u16_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s16_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u32_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s32_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u64_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s64_t*)
			{
				CL3_NOT_IMPLEMENTED;
			}

			//	creates a new sub-expression which holds a copy of the variable
			CLASS		TFormular::TFormular	(const f32_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const f64_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u8_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s8_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u16_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s16_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u32_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s32_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const u64_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const s64_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(const TFormular&)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(TFormular&&)
			{
				CL3_NOT_IMPLEMENTED;
			}

			CLASS		TFormular::TFormular	(system::memory::TSharedPtr<node::INode> node_root) : node_root(node_root)
			{
			}

			CLASS		TFormular::TFormular	(const io::text::string::TString&)
			{
				CL3_NOT_IMPLEMENTED;
			}

			llvm::Function*	TFormular::GenerateCode	(llvm::Module&) const
			{
				CL3_NOT_IMPLEMENTED;
			}

			TValue::EType	TFormular::Decltype() const
			{
				CL3_NOT_IMPLEMENTED;
			}
		}
	}
}
