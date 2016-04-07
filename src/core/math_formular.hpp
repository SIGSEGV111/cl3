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

namespace	llvm
{
	class Module;
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
				enum class EType
				{
					S8,
					U8,
					S16,
					U16,
					S32,
					U32,
					S64,
					U64,
					F32,
					F64,
					BOOL
				} type;
				union UValue
				{
					s8_t s8;
					u8_t u8;
					s16_t s16;
					u16_t u16;
					s32_t s32;
					u32_t u32;
					s64_t s64;
					u64_t u64;
					f32_t f32;
					f64_t f64;
					bool b;
				} value;
			};

			namespace	node
			{
				using namespace system::memory;

				struct INode
				{
					virtual llvm::Function* GenerateCode(llvm::Module&) const CL3_WARN_UNUSED_RESULT = 0;
					virtual TValue::EType Decltype() const CL3_WARN_UNUSED_RESULT = 0;
					CL3PUBF virtual ~INode();
				};

				struct TReferenceNode : INode
				{
					TValue::EType type;
					const void* ptr;

					CL3PUBF	llvm::Function* GenerateCode(llvm::Module&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TValue::EType Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TReferenceNode	(TValue::EType type, const void* ptr);
				};

				struct TConstantNode : INode
				{
					TValue value;

					CL3PUBF	llvm::Function* GenerateCode(llvm::Module&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TValue::EType Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TConstantNode	(TValue value);
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

					CL3PUBF	llvm::Function* GenerateCode(llvm::Module&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TValue::EType Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TBinaryOperatorNode	(EOperation op, TSharedPtr<INode> lhs, TSharedPtr<INode> rhs);
				};

				struct TFunctionCallNode : INode
				{
					void* func;
					TValue::EType type_return;
					TValue::EType type_arg;
					TSharedPtr<INode*> arg;

					CL3PUBF	llvm::Function* GenerateCode(llvm::Module&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TValue::EType Decltype() const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TFunctionCallNode	(void* func, TValue::EType type_return , TValue::EType type_arg, TSharedPtr<INode*> arg);
				};
			}

			class TFormular : public node::INode
			{
				protected:
					system::memory::TSharedPtr<node::INode> node_root;

					CLASS TFormular(system::memory::TSharedPtr<node::INode>);

				public:
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

					CL3PUBF	CLASS		TFormular	(const TFormular&);
					CL3PUBF	CLASS		TFormular	(TFormular&&);
					CL3PUBF	CLASS		TFormular	(const io::text::string::TString&);

					CL3PUBF	llvm::Function* GenerateCode(llvm::Module&) const final override CL3_WARN_UNUSED_RESULT;
					CL3PUBF	TValue::EType	Decltype() const final override CL3_WARN_UNUSED_RESULT;

			};
		}
	}
}

#endif
