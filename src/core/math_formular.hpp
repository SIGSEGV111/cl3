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

#include "system_memory.hpp"
#include "io_text_string.hpp"

#include <llvm/IR/Module.h>

namespace	cl3
{
	namespace	math
	{
		namespace	formular
		{
			struct	CL3PUBT	IFormularNode
			{
				CL3PUBF	virtual	~IFormularNode	();

				CL3PUBF	virtual	system::memory::TUniquePtr<llvm::Function>
								GenerateCode	(llvm::Module&) const CL3_WARN_UNUSED_RESULT = 0;

				CL3PUBF	virtual	operator io::text::string::TString	() const CL3_GETTER = 0;
			};

			class	TLiteralNode : public IFormularNode
			{
				protected:
					double value;

				public:
					CL3PUBF	system::memory::TUniquePtr<llvm::Function>
							GenerateCode	(llvm::Module&) const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	operator io::text::string::TString	() const final override CL3_GETTER;

					CL3PUBF	CLASS	TLiteralNode	(double value);
					CL3PUBF	CLASS	TLiteralNode	(const TLiteralNode&);
			};

			enum	EOperation
			{
				OPERATION_ADD,
				OPERATION_SUB,
				OPERATION_MUL,
				OPERATION_DIV,
				OPERATION_MOD
			};

			class	TOperatorNode : public IFormularNode
			{
				private:
					CLASS	TOperatorNode	(const TOperatorNode&) = delete;

				protected:
					system::memory::TUniquePtr<IFormularNode> lhs;
					system::memory::TUniquePtr<IFormularNode> rhs;
					EOperation op;

				public:
					CL3PUBF	system::memory::TUniquePtr<llvm::Function>
							GenerateCode	(llvm::Module&) const final override CL3_WARN_UNUSED_RESULT;

					CL3PUBF	operator io::text::string::TString	() const final override CL3_GETTER;

					CL3PUBF	CLASS	TOperatorNode	(system::memory::TUniquePtr<IFormularNode>&& lhs, system::memory::TUniquePtr<IFormularNode>&& rhs, EOperation op);
			};
		}
	}
}

#endif
