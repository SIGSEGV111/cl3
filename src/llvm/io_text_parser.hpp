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

#include <cl3/core/io_text.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/system_memory.hpp>
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

				enum EStatus
				{
					NEED_MORE_DATA,
					MATCHED,
					NOT_MATCHED
				};

				struct INode
				{
					virtual EStatus Parse(const TUTF32*& pos, usys_t length) CL3_WARN_UNUSED_RESULT = 0;
					LL3PUBF CLASS virtual ~INode();
				};

				struct TDisjunction : INode
				{
					system::memory::TSharedPtr<INode> lhs;
					system::memory::TSharedPtr<INode> rhs;

					LL3PUBF EStatus Parse(const TUTF32*& pos, usys_t length) final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF CLASS TDisjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs);
					LL3PUBF CLASS ~TDisjunction();
				};

				struct TConjunction : INode
				{
					system::memory::TSharedPtr<INode> lhs;
					system::memory::TSharedPtr<INode> rhs;

					LL3PUBF EStatus Parse(const TUTF32*& pos, usys_t length) final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF CLASS TConjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs);
					LL3PUBF CLASS ~TConjunction();
				};

				struct TRepetition : INode
				{
					system::memory::TSharedPtr<INode> node;
					unsigned n_rep_min;
					unsigned n_rep_max;

					LL3PUBF EStatus Parse(const TUTF32*& pos, usys_t length) final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF CLASS TRepetition(system::memory::TSharedPtr<INode> node, unsigned n_rep_min, unsigned n_rep_max);
					LL3PUBF CLASS ~TRepetition();
				};

				struct TSequence : INode
				{
					system::memory::TSharedPtr<INode> lhs;
					system::memory::TSharedPtr<INode> rhs;

					LL3PUBF EStatus Parse(const TUTF32*& pos, usys_t length) final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF CLASS TSequence(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs);
					LL3PUBF CLASS ~TSequence();
				};

				struct TNegation : INode
				{
					system::memory::TSharedPtr<INode> node;

					LL3PUBF EStatus Parse(const TUTF32*& pos, usys_t length) final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF CLASS TNegation(system::memory::TSharedPtr<INode> node);
					LL3PUBF CLASS ~TNegation();
				};

				struct TLiteralMatcher : INode
				{
					const string::TString literal;

					LL3PUBF EStatus Parse(const TUTF32*& pos, usys_t length) final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF CLASS TLiteralMatcher(const string::TString&);
					LL3PUBF CLASS ~TLiteralMatcher();
				};

				struct TRangeMatcher : INode
				{
					TUTF32 range_start;
					TUTF32 range_end;

					LL3PUBF EStatus Parse(const TUTF32*& pos, usys_t length) final override CL3_WARN_UNUSED_RESULT;

					LL3PUBF CLASS TRangeMatcher(TUTF32 range_start, TUTF32 range_end);
					LL3PUBF CLASS ~TRangeMatcher();
				};

				class TParserSpec
				{
					protected:
						system::memory::TSharedPtr<INode> node;

					public:
						inline system::memory::TSharedPtr<INode> Node() const CL3_GETTER { return this->node; }

						LL3PUBF TParserSpec operator||(const TParserSpec& rhs) const;
						LL3PUBF TParserSpec operator&&(const TParserSpec& rhs) const;
						LL3PUBF TParserSpec operator+(const TParserSpec& rhs) const;
						LL3PUBF TParserSpec operator!() const;

						LL3PUBF CLASS TParserSpec(system::memory::TSharedPtr<INode>);
						LL3PUBF CLASS TParserSpec(const TParserSpec&);
						LL3PUBF CLASS ~TParserSpec();

						LL3PUBF TParserSpec& operator=(TParserSpec&&);

// 						LL3PUBF llvm::Function* GenerateCode(system::compiler::jit::TJIT&) const CL3_WARN_UNUSED_RESULT;
				};

				class TParser : public stream::IOut<TUTF32>
				{
					protected:
						struct TNode
						{
							system::memory::TSharedPtr<INode> node;
							usys_t idx_match;	//	index into TParser::buffer
							usys_t len_match;	//	length of matched region
						};

						collection::list::TList<TUTF32> buffer;

					public:
						//	from IOut
						using stream::IOut<TUTF32>::Write;
						LL3PUBF void	Flush	();
						LL3PUBF usys_t	Space	() const CL3_GETTER;
						LL3PUBF usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) CL3_WARN_UNUSED_RESULT;

						LL3PUBF CLASS TParser(const TParser&);
						LL3PUBF explicit CLASS TParser(const TParserSpec& def);
						LL3PUBF CLASS ~TParser();
				};

				static const unsigned INFINITE = (unsigned)-1;

				LL3PUBF TParserSpec Literal(const string::TString&) CL3_GETTER;
				LL3PUBF TParserSpec Range(TUTF32 range_start, TUTF32 range_end) CL3_GETTER;
				LL3PUBF TParserSpec Repetition(const TParserSpec&, unsigned rep_min, unsigned rep_max) CL3_GETTER;
				LL3PUBF TParserSpec Optional(const TParserSpec&) CL3_GETTER;
			}
		}
	}
}

#endif
