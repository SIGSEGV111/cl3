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

#ifndef	_include_cl3_core_io_text_parser_hpp_
#define	_include_cl3_core_io_text_parser_hpp_

#include "io_text.hpp"
#include "io_text_string.hpp"
#include "io_collection_list.hpp"
#include "system_memory.hpp"
#include "system_compiler_jit.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace parser
			{
// 				enum class ENodeType
// 				{
// 					LITERAL, RANGE, DISJUNCTION, CONJUNCTION, REPETITION, SEQUENCE, NEGATION
// 				};

				struct INode
				{
					CL3PUBF CLASS virtual ~INode();
				};

				struct TDisjunction : INode
				{
					system::memory::TSharedPtr<INode> lhs;
					system::memory::TSharedPtr<INode> rhs;

					CL3PUBF CLASS TDisjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs);
					CL3PUBF CLASS ~TDisjunction();
				};

				struct TConjunction : INode
				{
					system::memory::TSharedPtr<INode> lhs;
					system::memory::TSharedPtr<INode> rhs;

					CL3PUBF CLASS TConjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs);
					CL3PUBF CLASS ~TConjunction();
				};

				struct TRepetition : INode
				{
					system::memory::TSharedPtr<INode> node;
					unsigned n_rep_min;
					unsigned n_rep_max;

					CL3PUBF CLASS TRepetition(system::memory::TSharedPtr<INode> node, unsigned n_rep_min, unsigned n_rep_max);
					CL3PUBF CLASS ~TRepetition();
				};

				struct TSequence : INode
				{
					system::memory::TSharedPtr<INode> lhs;
					system::memory::TSharedPtr<INode> rhs;

					CL3PUBF CLASS TSequence(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs);
					CL3PUBF CLASS ~TSequence();
				};

				struct TNegation : INode
				{
					system::memory::TSharedPtr<INode> node;

					CL3PUBF CLASS TNegation(system::memory::TSharedPtr<INode> node);
					CL3PUBF CLASS ~TNegation();
				};

				struct TLiteralMatcher : INode
				{
					const string::TString literal;

					CL3PUBF CLASS TLiteralMatcher(const string::TString&);
					CL3PUBF CLASS ~TLiteralMatcher();
				};

				struct TRangeMatcher : INode
				{
					TUTF32 range_start;
					TUTF32 range_end;

					CL3PUBF CLASS TRangeMatcher(TUTF32 range_start, TUTF32 range_end);
					CL3PUBF CLASS ~TRangeMatcher();
				};

				class TParser
				{
					protected:
						system::memory::TSharedPtr<INode> node;

					public:
						inline system::memory::TSharedPtr<INode> Node() const CL3_GETTER { return this->node; }

						CL3PUBF TParser operator||(const TParser& rhs) const;
						CL3PUBF TParser operator&&(const TParser& rhs) const;
						CL3PUBF TParser operator+(const TParser& rhs) const;
						CL3PUBF TParser operator!() const;

						CL3PUBF CLASS TParser(system::memory::TSharedPtr<INode>);
						CL3PUBF CLASS TParser(const TParser&);
						CL3PUBF CLASS ~TParser();

						CL3PUBF TParser& operator=(TParser&&);

// 						CL3PUBF llvm::Function* GenerateCode(system::compiler::jit::TJIT&) const CL3_WARN_UNUSED_RESULT;
				};

				static const unsigned INFINITE = (unsigned)-1;

				CL3PUBF TParser Literal(const string::TString&) CL3_GETTER;
				CL3PUBF TParser Range(TUTF32 range_start, TUTF32 range_end) CL3_GETTER;
				CL3PUBF TParser Repetition(const TParser&, unsigned rep_min, unsigned rep_max) CL3_GETTER;
				CL3PUBF TParser Optional(const TParser&) CL3_GETTER;
			}
		}
	}
}

#endif
