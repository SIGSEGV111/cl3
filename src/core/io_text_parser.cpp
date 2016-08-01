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

				TParser TParser::operator||(const TParser& rhs) const
				{
					return TParser(MakeSharedPtr<INode>(new TDisjunction(this->node, rhs.node)));
				}

				TParser TParser::operator&&(const TParser& rhs) const
				{
					return TParser(MakeSharedPtr<INode>(new TConjunction(this->node, rhs.node)));
				}

				TParser TParser::operator+(const TParser& rhs) const
				{
					return TParser(MakeSharedPtr<INode>(new TSequence(this->node, rhs.node)));
				}

				TParser TParser::operator!() const
				{
					return TParser(MakeSharedPtr<INode>(new TNegation(this->node)));
				}

				CLASS TParser::TParser(system::memory::TSharedPtr<INode> node) : node(node)
				{
				}

				CLASS TParser::~TParser()
				{
				}

				TParser Literal(const string::TString& str)
				{
					return TParser(MakeSharedPtr<INode>(new TLiteralMatcher(str)));
				}

				TParser Range(TUTF32 range_start, TUTF32 range_end)
				{
					return TParser(MakeSharedPtr<INode>(new TRangeMatcher(range_start, range_end)));
				}

				TParser Repetition(const TParser& p, unsigned rep_min, unsigned rep_max)
				{
					return TParser(MakeSharedPtr<INode>(new TRepetition(p.Node(), rep_min, rep_max)));
				}

				TParser Optional(const TParser& p)
				{
					return TParser(MakeSharedPtr<INode>(new TRepetition(p.Node(), 0, 1)));
				}

			}
		}
	}
}
