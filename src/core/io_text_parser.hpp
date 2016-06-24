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
#include "system_memory.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			template<class T>	struct	IStaticCollection;
			template<class T>	struct	IStaticIterator;

			namespace	array
			{
				template<class T>	struct	IArray;
			}
		}

		namespace	text
		{
			namespace	regex
			{
				struct TMatch
				{
					usys_t index;
					usys_t length;
				};

				struct TState
				{
				};

				typedef TMatch (*FMatch)(TState&, const string::TString&);

				CL3PUBF llvm::Function* CompileRegEx(const string::TString&, system::compiler::jit::TJIT&);

				class TRegEx : public stream::IOut<TUTF32>
				{
					protected:
						system::memory::TUniquePtr<system::compiler::jit::TJIT> jit;
						TState state;
						FMatch func;

					public:
						inline TMatch Match(const string::TString& str) const { state.Reset(); this->Write(str.ItemPtr(0), str.Count(), 0) }
						CLASS TRegEx(const string::TString&);
				};


			}

			namespace	tokenizer
			{
				class TToken
				{
					public:
						TToken	operator||	(const TToken&) const;
						TToken	operator&&	(const TToken&) const;

						CLASS TToken(const regex::TRegEx&);
						CLASS TToken(const string::TString&);
						CLASS TToken(const TToken&);
				};

// 				TToken T_INTEGER = TRegEx("(\+|-)?([0-9]|[1-9][0-9]*)");
// 				TToken T_FLOAT   = INTEGER && TRegEx("\.[0-9]+");
// 				TToken T_NUMBER  = INTEGER || FLOAT;
// 				TToken T_TRUE    = "true";
// 				TToken T_FALSE   = "false";
// 				TToken T_NULL    = "null";
// 				TToken T_UNDEF   = "undefined";
// 				TToken T_STRING  = TToken(TRegEx("\"([^\"\\]|\\.)*\"")) || TRegEx("'([^'\\]|\\.)*'");
//
// 				TParser p;
//
// 				p.AddToken(T_INTEGER, OnInteger);
// 				p.AddToken(T_FLOAT, OnFloat);
			}
		}
	}
}

#endif
