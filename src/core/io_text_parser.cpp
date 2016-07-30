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

				CLASS INode::~INode()
				{
				}

				CLASS TDisjunction::~TDisjunction()
				{
					for(usys_t i = 0; i < this->nodes.Count(); i++)
						delete this->nodes[i];
				}

				CLASS TConjunction::~TConjunction()
				{
					for(usys_t i = 0; i < this->nodes.Count(); i++)
						delete this->nodes[i];
				}

				CLASS TRepetition::~TRepetition()
				{
				}

				CLASS TSequence::~TSequence()
				{
					for(usys_t i = 0; i < this->nodes.Count(); i++)
						delete this->nodes[i];
				}

				CLASS TNegation::~TNegation()
				{
				}

				CLASS TLiteralMatcher::~TLiteralMatcher()
				{
				}

				CLASS TRangeMatcher::~TRangeMatcher()
				{
				}

				TParser TParser::operator||(const TParser& rhs) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TParser TParser::operator&&(const TParser& rhs) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TParser TParser::operator+(const TParser& rhs) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TParser TParser::operator!() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				

				CLASS TParser::~TParser()
				{
				}

				TParser Literal(const char* cstr)
				{
					const TString str = cstr;
					return TParser(TString("the literal string \"") + str + TString("\""), str);
				}

				TParser Literal(const string::TString& str)
				{
					return TParser(TString("the literal string \"") + str + TString("\""), str);
				}

				TParser Range(TUTF32 range_start, TUTF32 range_end)
				{
					return TParser( TString("unicode characters in the range between '") + range_start + "' and '" + range_end + "'", range_start, range_end );
				}

				TParser Repetition(const TParser& p, unsigned rep_min, unsigned rep_max)
				{
					return TParser(p, rep_min, rep_max);
				}

				TParser Optional(const TParser& p)
				{
					return TParser(p, 0, 1);
				}

			}
		}
	}
}
