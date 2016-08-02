/*
    libcl3 - common library version 3
    Copyright (C) 2013	Simon Brennecke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY
				{
					CL3_NOT_IMPLEMENTED;
				}
				 without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "error.hpp"
#include "io_collection.hpp"
#include "io_text_parser.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	parser
			{
				using namespace string;
				using namespace system::memory;

				CLASS INode::~INode()
				{
				}

				CLASS TDisjunction::TDisjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs) : lhs(lhs), rhs(rhs)
				{
				}

				CLASS TDisjunction::~TDisjunction()
				{
				}

				CLASS TConjunction::TConjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs) : lhs(lhs), rhs(rhs)
				{
				}

				CLASS TConjunction::~TConjunction()
				{
				}

				CLASS TRepetition::TRepetition(system::memory::TSharedPtr<INode> node, unsigned n_rep_min, unsigned n_rep_max) : node(node), n_rep_min(n_rep_min), n_rep_max(n_rep_max)
				{
				}

				CLASS TRepetition::~TRepetition()
				{
				}

				CLASS TSequence::TSequence(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs) : lhs(lhs), rhs(rhs)
				{
				}

				CLASS TSequence::~TSequence()
				{
				}

				CLASS TNegation::TNegation(system::memory::TSharedPtr<INode> node) : node(node)
				{
				}

				CLASS TNegation::~TNegation()
				{
				}

				CLASS TLiteralMatcher::TLiteralMatcher(const string::TString& literal) : literal(literal)
				{
				}

				CLASS TLiteralMatcher::~TLiteralMatcher()
				{
				}

				CLASS TRangeMatcher::TRangeMatcher(TUTF32 range_start, TUTF32 range_end) : range_start(range_start), range_end(range_end)
				{
				}

				CLASS TRangeMatcher::~TRangeMatcher()
				{
				}

				TParserDef TParserDef::operator||(const TParserDef& rhs) const
				{
					return TParserDef(MakeSharedPtr<INode>(new TDisjunction(this->node, rhs.node)));
				}

				TParserDef TParserDef::operator&&(const TParserDef& rhs) const
				{
					return TParserDef(MakeSharedPtr<INode>(new TConjunction(this->node, rhs.node)));
				}

				TParserDef TParserDef::operator+(const TParserDef& rhs) const
				{
					return TParserDef(MakeSharedPtr<INode>(new TSequence(this->node, rhs.node)));
				}

				TParserDef TParserDef::operator!() const
				{
					return TParserDef(MakeSharedPtr<INode>(new TNegation(this->node)));
				}

				CLASS TParserDef::TParserDef(system::memory::TSharedPtr<INode> node) : node(node)
				{
				}

				CLASS TParserDef::~TParserDef()
				{
				}

				TParserDef Literal(const string::TString& str)
				{
					return TParserDef(MakeSharedPtr<INode>(new TLiteralMatcher(str)));
				}

				TParserDef Range(TUTF32 range_start, TUTF32 range_end)
				{
					return TParserDef(MakeSharedPtr<INode>(new TRangeMatcher(range_start, range_end)));
				}

				TParserDef Repetition(const TParserDef& p, unsigned rep_min, unsigned rep_max)
				{
					return TParserDef(MakeSharedPtr<INode>(new TRepetition(p.Node(), rep_min, rep_max)));
				}

				TParserDef Optional(const TParserDef& p)
				{
					return TParserDef(MakeSharedPtr<INode>(new TRepetition(p.Node(), 0, 1)));
				}

				void	TParserInstance::Flush	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	TParserInstance::Space	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	TParserInstance::Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TParserInstance::TParserInstance(const TParserInstance&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TParserInstance::TParserInstance(const TParserDef& def)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TParserInstance::~TParserInstance()
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}
