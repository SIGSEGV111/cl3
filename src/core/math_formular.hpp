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

#include "io_text_string.hpp"
#include "system_memory.hpp"

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
				byte_t data[CL3_MAX(8, sizeof(void*))];
			};

			namespace	node
			{
				using namespace system::memory;

				struct INode
				{
					void* tag;

					virtual llvm::Value* GenerateCode(llvm::IRBuilder<>&) const CL3_WARN_UNUSED_RESULT = 0;
					virtual const system::types::typeinfo::TRTTI* Decltype() const CL3_WARN_UNUSED_RESULT = 0;

					CL3PUBF virtual ~INode();
				};

				struct TReferenceNode : INode
				{
					const system::types::typeinfo::TRTTI* type;
					const void* ptr;

					CL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TReferenceNode	(const system::types::typeinfo::TRTTI* type, const void* ptr);
					CL3PUBF	CLASS	~TReferenceNode	();
				};

				struct TConstantNode : INode
				{
					TValue value;

					CL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TConstantNode	(const system::types::typeinfo::TRTTI* type, const void* data);
					CL3PUBF	CLASS	~TConstantNode	();
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

					CL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TBinaryOperatorNode	(EOperation op, TSharedPtr<INode> lhs, TSharedPtr<INode> rhs);
					CL3PUBF	CLASS	~TBinaryOperatorNode();
				};

				struct TFunctionCallNode : INode
				{
					void* func;
					const system::types::typeinfo::TRTTI* type_return;
					const system::types::typeinfo::TRTTI* type_arg;
					TSharedPtr<INode*> arg;

					CL3PUBF	llvm::Value* GenerateCode(llvm::IRBuilder<>&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TFunctionCallNode	(void* func, const system::types::typeinfo::TRTTI* type_return , const system::types::typeinfo::TRTTI* type_arg, TSharedPtr<INode*> arg);
					CL3PUBF	CLASS	~TFunctionCallNode	();
				};
			}

			class CL3PUBT TFormular
			{
				protected:
					CLASS TFormular(system::memory::TSharedPtr<node::INode>);

				public:
					system::memory::TSharedPtr<node::INode> node_root;

					CL3PUBF	TFormular	operator+	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator-	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator*	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator/	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator%	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator^	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator>	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator<	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator>=	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator<=	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator==	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TFormular	operator!=	(const TFormular& rhs) const CL3_WARN_UNUSED_RESULT;

					//	appends to the current formular object
					CL3PUBF	TFormular&	operator+=	(const TFormular& rhs);
					CL3PUBF	TFormular&	operator-=	(const TFormular& rhs);
					CL3PUBF	TFormular&	operator*=	(const TFormular& rhs);
					CL3PUBF	TFormular&	operator/=	(const TFormular& rhs);
					CL3PUBF	TFormular&	operator%=	(const TFormular& rhs);
					CL3PUBF	TFormular&	operator^=	(const TFormular& rhs);

					CL3PUBF	TFormular&	operator=	(const TFormular&);
					CL3PUBF	TFormular&	operator=	(TFormular&&);

					//	creates a new sub-expression which refers to the pointed-to variable
					CL3PUBF	CLASS		TFormular	(const f32_t*);
					CL3PUBF	CLASS		TFormular	(const f64_t*);
					CL3PUBF	CLASS		TFormular	(const u8_t*);
					CL3PUBF	CLASS		TFormular	(const s8_t*);
					CL3PUBF	CLASS		TFormular	(const u16_t*);
					CL3PUBF	CLASS		TFormular	(const s16_t*);
					CL3PUBF	CLASS		TFormular	(const u32_t*);
					CL3PUBF	CLASS		TFormular	(const s32_t*);
					CL3PUBF	CLASS		TFormular	(const u64_t*);
					CL3PUBF	CLASS		TFormular	(const s64_t*);
					CL3PUBF	CLASS		TFormular	(const bool*);

					//	creates a new sub-expression which holds a copy of the variable
					CL3PUBF	CLASS		TFormular	(const f32_t);
					CL3PUBF	CLASS		TFormular	(const f64_t);
					CL3PUBF	CLASS		TFormular	(const u8_t);
					CL3PUBF	CLASS		TFormular	(const s8_t);
					CL3PUBF	CLASS		TFormular	(const u16_t);
					CL3PUBF	CLASS		TFormular	(const s16_t);
					CL3PUBF	CLASS		TFormular	(const u32_t);
					CL3PUBF	CLASS		TFormular	(const s32_t);
					CL3PUBF	CLASS		TFormular	(const u64_t);
					CL3PUBF	CLASS		TFormular	(const s64_t);
					CL3PUBF	CLASS		TFormular	(const bool);

					CL3PUBF	CLASS		TFormular	(const TFormular&);
					CL3PUBF	CLASS		TFormular	(TFormular&&);
					CL3PUBF	CLASS		TFormular	(const io::text::string::TString&);

					CL3PUBF	llvm::Function* GenerateCode(llvm::Module&) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	llvm::Function* GenerateCode(llvm::Module&, const io::text::string::TString& name) const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	const system::types::typeinfo::TRTTI* Decltype() const CL3_WARN_UNUSED_RESULT;
					CL3PUBF io::text::string::TString ToString() const CL3_GETTER;
			};
		}
	}
}

#endif
