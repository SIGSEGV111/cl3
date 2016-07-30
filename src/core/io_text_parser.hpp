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
					collection::list::TList<INode*> nodes;

					CL3PUBF CLASS ~TDisjunction();
				};

				struct TConjunction : INode
				{
					collection::list::TList<INode*> nodes;

					CL3PUBF CLASS ~TConjunction();
				};

				struct TRepetition : INode
				{
					system::memory::TUniquePtr<INode> node;
					unsigned n_rep_min;
					unsigned n_rep_max;

					CL3PUBF CLASS ~TRepetition();
				};

				struct TSequence : INode
				{
					collection::list::TList<INode*> nodes;

					CL3PUBF CLASS ~TSequence();
				};

				struct TNegation : INode
				{
					system::memory::TUniquePtr<INode> node;

					CL3PUBF CLASS ~TNegation();
				};

				struct TLiteralMatcher : INode
				{
					const string::TString literal;

					CL3PUBF CLASS ~TLiteralMatcher();
				};

				struct TRangeMatcher : INode
				{
					TUTF32 range_start;
					TUTF32 range_end;

					CL3PUBF CLASS ~TRangeMatcher();
				};

				class TParser
				{
					protected:
						system::memory::TUniquePtr<INode> node;

					public:
						CL3PUBF TParser operator||(const TParser& rhs) const;
						CL3PUBF TParser operator||(TParser&& rhs) const;
						CL3PUBF TParser operator&&(const TParser& rhs) const;
						CL3PUBF TParser operator&&(TParser&& rhs) const;
						CL3PUBF TParser operator+(const TParser& rhs) const;
						CL3PUBF TParser operator+(TParser&& rhs) const;
						CL3PUBF TParser operator!() const;

						CL3PUBF CLASS TParser(const string::TString& descr, system::memory::TUniquePtr<INode>);
						CL3PUBF CLASS TParser(TParser&&);
						CL3PUBF CLASS ~TParser();

						CL3PUBF TParser& operator=(TParser&&);

// 						CL3PUBF llvm::Function* GenerateCode(system::compiler::jit::TJIT&) const CL3_WARN_UNUSED_RESULT;
				};

				static const unsigned INFINITE = (unsigned)-1;

				CL3PUBF TParser Literal(const char*) CL3_GETTER;
				CL3PUBF TParser Literal(const string::TString&) CL3_GETTER;
				CL3PUBF TParser Range(TUTF32 range_start, TUTF32 range_end) CL3_GETTER;
				CL3PUBF TParser Repetition(const TParser&, unsigned rep_min, unsigned rep_max) CL3_GETTER;
				CL3PUBF TParser Optional(const TParser&) CL3_GETTER;

				static const TParser digit_excl_zero = Range('1', '9');
				static const TParser digit = Literal("0") || digit_excl_zero;
				static const TParser digits = Repetition(digit, 1, INFINITE);
				static const TParser integer = Literal("0") || (digit_excl_zero + Optional(digits));

				static const TParser modifier = (Literal("+") || Literal("-")) + integer;
				static const TParser multiplier = integer + (Literal("x") || Literal("*"));
				static const TParser dice_spec = integer + Literal("d") + Optional(integer);
				static const TParser dice = Optional(multiplier) + dice_spec + Optional(modifier);

				//	5x3d6+4
			}
		}
	}
}

#endif
