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

#ifndef	_include_cl3_core_math_formular_hpp_
#define	_include_cl3_core_math_formular_hpp_

#include "config.hpp"

#include <cl3/core/io_text_string.hpp>
#include <cl3/core/system_memory.hpp>

#include <llvm/IR/IRBuilder.h>

namespace	llvm
{
	class Module;
	class Value;
	class Function;
}

namespace	cl3
{
	namespace	math
	{
		namespace	formular
		{
			struct TValue
			{
				const system::types::typeinfo::TRTTI* type;

				union
				{
					u8_t u8;
					u16_t u16;
					u32_t u32;
					u64_t u64;
					f32_t f32;
					f64_t f64;
				};
			};

			namespace	node
			{
				using namespace system::memory;

				struct INode
				{
					void* tag;

					virtual llvm::Value* GenerateCode(llvm::IRBuilder<>&) const CL3_WARN_UNUSED_RESULT = 0;
					virtual const system::types::typeinfo::TRTTI* Decltype() const CL3_WARN_UNUSED_RESULT = 0;

					LL3PUBF virtual ~INode();
				};

				struct TReferenceNode : INode
				{
					const system::types::typeinfo::TRTTI* type;
					const void* ptr;

					LL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					LL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF	CLASS	TReferenceNode	(const system::types::typeinfo::TRTTI* type, const void* ptr);
					LL3PUBF	CLASS	~TReferenceNode	();
				};

				struct TConstantNode : INode
				{
					TValue value;

					LL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					LL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF	CLASS	TConstantNode	(const system::types::typeinfo::TRTTI* type, const void* data);
					LL3PUBF	CLASS	~TConstantNode	();
				};

				struct TBinaryOperatorNode : INode
				{
					enum class EOperation
					{
						ADD, SUB, MUL, DIV, MOD, POW, BIGGER_THAN, LESS_THAN, BIGGER_OR_EQUAL_THAN, LESS_OR_EQUAL_THAN, EQUAL, NOT_EQUAL
					};

					EOperation op;
					TSharedPtr<INode> lhs;
					TSharedPtr<INode> rhs;

					LL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					LL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF	CLASS	TBinaryOperatorNode	(EOperation op, TSharedPtr<INode> lhs, TSharedPtr<INode> rhs);
					LL3PUBF	CLASS	~TBinaryOperatorNode();
				};

				struct TFunctionCallNode : INode
				{
					void* func;
					const system::types::typeinfo::TRTTI* type_return;
					const system::types::typeinfo::TRTTI* type_arg;
					TSharedPtr<INode*> arg;

					LL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					LL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF	CLASS	TFunctionCallNode	(void* func, const system::types::typeinfo::TRTTI* type_return , const system::types::typeinfo::TRTTI* type_arg, TSharedPtr<INode*> arg);
					LL3PUBF	CLASS	~TFunctionCallNode	();
				};
			}

			class LL3PUBT TFormular
			{
				protected:
					CLASS TFormular(system::memory::TSharedPtr<node::INode>);

				public:
					system::memory::TSharedPtr<node::INode> node_root;

					LL3PUBF	TFormular	operator+	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator-	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator*	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator/	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator%	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator^	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator>	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator<	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator>=	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator<=	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator==	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	TFormular	operator!=	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;

					//	appends to the current formular object
					LL3PUBF	TFormular&	operator+=	(const TFormular& rhs);
					LL3PUBF	TFormular&	operator-=	(const TFormular& rhs);
					LL3PUBF	TFormular&	operator*=	(const TFormular& rhs);
					LL3PUBF	TFormular&	operator/=	(const TFormular& rhs);
					LL3PUBF	TFormular&	operator%=	(const TFormular& rhs);
					LL3PUBF	TFormular&	operator^=	(const TFormular& rhs);

					LL3PUBF	TFormular&	operator=	(const TFormular&);
					LL3PUBF	TFormular&	operator=	(TFormular&&);

					//	creates a new sub-expression which refers to the pointed-to variable
					LL3PUBF	CLASS		TFormular	(const f32_t*);
					LL3PUBF	CLASS		TFormular	(const f64_t*);
					LL3PUBF	CLASS		TFormular	(const u8_t*);
					LL3PUBF	CLASS		TFormular	(const s8_t*);
					LL3PUBF	CLASS		TFormular	(const u16_t*);
					LL3PUBF	CLASS		TFormular	(const s16_t*);
					LL3PUBF	CLASS		TFormular	(const u32_t*);
					LL3PUBF	CLASS		TFormular	(const s32_t*);
					LL3PUBF	CLASS		TFormular	(const u64_t*);
					LL3PUBF	CLASS		TFormular	(const s64_t*);
					LL3PUBF	CLASS		TFormular	(const bool*);

					//	creates a new sub-expression which holds a copy of the variable
					LL3PUBF	CLASS		TFormular	(const f32_t);
					LL3PUBF	CLASS		TFormular	(const f64_t);
					LL3PUBF	CLASS		TFormular	(const u8_t);
					LL3PUBF	CLASS		TFormular	(const s8_t);
					LL3PUBF	CLASS		TFormular	(const u16_t);
					LL3PUBF	CLASS		TFormular	(const s16_t);
					LL3PUBF	CLASS		TFormular	(const u32_t);
					LL3PUBF	CLASS		TFormular	(const s32_t);
					LL3PUBF	CLASS		TFormular	(const u64_t);
					LL3PUBF	CLASS		TFormular	(const s64_t);
					LL3PUBF	CLASS		TFormular	(const bool);

					LL3PUBF	CLASS		TFormular	(const TFormular&);
					LL3PUBF	CLASS		TFormular	(TFormular&&);
					LL3PUBF	CLASS		TFormular	(const io::text::string::TString&);

					LL3PUBF	llvm::Function* GenerateCode(llvm::Module&) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	llvm::Function* GenerateCode(llvm::Module&, const io::text::string::TString& name) const CL3_WARN_UNUSED_RESULT;
					LL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const CL3_WARN_UNUSED_RESULT;
					LL3PUBF io::text::string::TString ToString() const CL3_GETTER;
			};
		}
	}
}

#endif
