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
				template<class T>	class	IArray;
			}
		}

		namespace	text
		{
			namespace	parser
			{
				enum	EMatchType
				{
					MATCHTYPE_INCLUDE,
					MATCHTYPE_EXCLUDE
				};

				class	CL3PUBT	TTokenizer
				{
					protected:
						const collection::IStaticCollection<const TUTF32>* collection;
						stream::IIn<TUTF32>* is;
						EMatchType mt;

						TUTF32 current_termination;
						string::TString current_token;

					public:
						inline	TUTF32					CurrentTermination	() const CL3_GETTER { return this->current_termination; }
						inline	const string::TString&	CurrentToken		() const CL3_GETTER { return this->current_token; }

						CL3PUBF	bool	Next		();

						CL3PUBF	CLASS	TTokenizer	(stream::IIn<TUTF32>*, EMatchType, const collection::IStaticCollection<const TUTF32>*);
						CL3PUBF	CLASS	TTokenizer	(const TTokenizer&);
						CL3PUBF	CLASS	~TTokenizer	();
				};
			}
		}
	}
}

#endif
