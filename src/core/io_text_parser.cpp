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
				using namespace stream;
				using namespace error;

				bool	TTokenizer::Next		()
				{
					this->current_termination = TUTF32::TERMINATOR;
					this->current_token.Clear();

					struct	TFlowController : IOut<TUTF32>
					{
						TTokenizer* tokenizer;

						usys_t	Space	() const final override CL3_GETTER
						{
							return 1;
						}

						bool	Test	(const TUTF32 chr) const CL3_GETTER
						{
							switch(this->tokenizer->mt)
							{
								case MATCHTYPE_INCLUDE:
									return this->tokenizer->collection->Contains(chr);
								case MATCHTYPE_EXCLUDE:
									return !this->tokenizer->collection->Contains(chr);
							}
							CL3_UNREACHABLE;
						}

						usys_t	Write	(const TUTF32* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT
						{
							if(n_items_write_min == (usys_t)-1)
								n_items_write_min = n_items_write_max;
							CL3_CLASS_LOGIC_ERROR(n_items_write_min > n_items_write_max);

							usys_t i = 0;

							for(; i < n_items_write_max; i++)
								if(!Test(arr_items_write[i]))
								{
									this->tokenizer->current_termination = arr_items_write[i];
									break;
								}

							this->tokenizer->current_token.Append(arr_items_write, i);

							if(this->tokenizer->current_termination != TUTF32::TERMINATOR)
								i++;	//	we read a end-of-string marker character and we processed it, so add it to the amount of consumed input

							return i;
						}

						TFlowController(TTokenizer* tokenizer) : tokenizer(tokenizer)
						{
						}
					}
					flow_controller(this);

					bool status = false;
					if(this->is->Remaining() != 0)
						status = this->is->WriteOut(flow_controller, (usys_t)-1, 1) > 0;

					return status;
				}

				CLASS	TTokenizer::TTokenizer	(stream::IIn<TUTF32>* is, EMatchType mt, const collection::IStaticCollection<const TUTF32>* collection) : collection(collection), is(is), mt(mt), current_termination(0U), current_token()
				{
					CL3_CLASS_ERROR(mt != MATCHTYPE_INCLUDE && mt != MATCHTYPE_EXCLUDE, TException, "mt must be MATCHTYPE_INCLUDE or MATCHTYPE_EXCLUDE");
				}

				CLASS	TTokenizer::TTokenizer	(const TTokenizer& other) : collection(other.collection), is(other.is), mt(other.mt), current_termination(0U), current_token()
				{
				}

				CLASS	TTokenizer::~TTokenizer	()
				{
				}
			}
		}
	}
}
