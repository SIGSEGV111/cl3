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

				EStatus TDisjunction::Parse(const TUTF32*& /*pos*/, usys_t /*length*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TDisjunction::TDisjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs) : lhs(lhs), rhs(rhs)
				{
				}

				CLASS TDisjunction::~TDisjunction()
				{
				}

				EStatus TConjunction::Parse(const TUTF32*& /*pos*/, usys_t /*length*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TConjunction::TConjunction(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs) : lhs(lhs), rhs(rhs)
				{
				}

				CLASS TConjunction::~TConjunction()
				{
				}

				EStatus TRepetition::Parse(const TUTF32*& /*pos*/, usys_t /*length*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TRepetition::TRepetition(system::memory::TSharedPtr<INode> node, unsigned n_rep_min, unsigned n_rep_max) : node(node), n_rep_min(n_rep_min), n_rep_max(n_rep_max)
				{
				}

				CLASS TRepetition::~TRepetition()
				{
				}

				EStatus TSequence::Parse(const TUTF32*& /*pos*/, usys_t /*length*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TSequence::TSequence(system::memory::TSharedPtr<INode> lhs, system::memory::TSharedPtr<INode> rhs) : lhs(lhs), rhs(rhs)
				{
				}

				CLASS TSequence::~TSequence()
				{
				}

				EStatus TNegation::Parse(const TUTF32*& /*pos*/, usys_t /*length*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TNegation::TNegation(system::memory::TSharedPtr<INode> node) : node(node)
				{
				}

				CLASS TNegation::~TNegation()
				{
				}

				EStatus TLiteralMatcher::Parse(const TUTF32*& /*pos*/, usys_t /*length*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TLiteralMatcher::TLiteralMatcher(const string::TString& literal) : literal(literal)
				{
				}

				CLASS TLiteralMatcher::~TLiteralMatcher()
				{
				}

				EStatus TRangeMatcher::Parse(const TUTF32*& /*pos*/, usys_t /*length*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TRangeMatcher::TRangeMatcher(TUTF32 range_start, TUTF32 range_end) : range_start(range_start), range_end(range_end)
				{
				}

				CLASS TRangeMatcher::~TRangeMatcher()
				{
				}

				TParserSpec TParserSpec::operator||(const TParserSpec& rhs) const
				{
					return TParserSpec(MakeSharedPtr<INode>(new TDisjunction(this->node, rhs.node)));
				}

				TParserSpec TParserSpec::operator&&(const TParserSpec& rhs) const
				{
					return TParserSpec(MakeSharedPtr<INode>(new TConjunction(this->node, rhs.node)));
				}

				TParserSpec TParserSpec::operator+(const TParserSpec& rhs) const
				{
					return TParserSpec(MakeSharedPtr<INode>(new TSequence(this->node, rhs.node)));
				}

				TParserSpec TParserSpec::operator!() const
				{
					return TParserSpec(MakeSharedPtr<INode>(new TNegation(this->node)));
				}

				CLASS TParserSpec::TParserSpec(system::memory::TSharedPtr<INode> node) : node(node)
				{
				}

				CLASS TParserSpec::~TParserSpec()
				{
				}

				TParserSpec Literal(const string::TString& str)
				{
					return TParserSpec(MakeSharedPtr<INode>(new TLiteralMatcher(str)));
				}

				TParserSpec Range(TUTF32 range_start, TUTF32 range_end)
				{
					return TParserSpec(MakeSharedPtr<INode>(new TRangeMatcher(range_start, range_end)));
				}

				TParserSpec Repetition(const TParserSpec& p, unsigned rep_min, unsigned rep_max)
				{
					return TParserSpec(MakeSharedPtr<INode>(new TRepetition(p.Node(), rep_min, rep_max)));
				}

				TParserSpec Optional(const TParserSpec& p)
				{
					return TParserSpec(MakeSharedPtr<INode>(new TRepetition(p.Node(), 0, 1)));
				}

				void	TParser::Flush	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	TParser::Space	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	TParser::Write	(const TUTF32* /*arr_items_write*/, usys_t /*n_items_write_max*/, usys_t /*n_items_write_min*/)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TParser::TParser(const TParser&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TParser::TParser(const TParserSpec&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS TParser::~TParser()
				{
					//CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}
