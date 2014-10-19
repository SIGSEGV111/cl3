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

#ifndef	_include_cl3_core_io_collection_matcher_hpp_
#define	_include_cl3_core_io_collection_matcher_hpp_

#include "io_collection.hpp"
#include "io_collection_array.hpp"
#include "io_collection_bitmask.hpp"
#include "io_collection_list.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	matcher
			{
				template<class T>
				class	CL3PUBT	TArrayMatcher : public IMatcher<T>
				{
					protected:
						const T* arr_items_local;
						usys_t n_items_local;

					public:
						inline	bool	Match		(const T& item_match) const final override CL3_GETTER;
						CL3PUBF	void	Match		(EDirection direction, const T* arr_items_match, bitmask::TBitmask& bm_result) const final override;
						CL3PUBF	usys_t	Match		(EDirection direction, const T* arr_items_match, usys_t n_items_match, list::TList<usys_t>& ls_result, usys_t offset = 0, usys_t n_hits_max = (usys_t)-1) const final override;
						CL3PUBF	usys_t	MatchFirst	(EDirection direction, const T* arr_items_match, usys_t n_items_match) const final override CL3_GETTER ;

						CL3PUBF	CLASS	TArrayMatcher	(const T* arr_items_local, usys_t n_items_local);
				};


				template<class T>
				bool	TArrayMatcher<T>::Match			(const T& item_match) const
				{
					for(usys_t i = 0; i < n_items_local; i++)
						if(arr_items_local[i] == item_match)
							return true;
					return false;
				}

				template<class T>
				void	TArrayMatcher<T>::Match			(EDirection direction, const T* arr_items_match, bitmask::TBitmask& bm_result) const
				{
					const usys_t n_items_match = bm_result.Count();

					switch(direction)
					{
						case DIRECTION_FORWARD:
							for(usys_t i = 0; i < n_items_match; i++)
								bm_result.Bit(i, this->Match(arr_items_match[i]));
							break;
						case DIRECTION_BACKWARD:
							CL3_NOT_IMPLEMENTED;
							break;
					}
				}

				template<class T>
				usys_t	TArrayMatcher<T>::Match			(EDirection direction, const T* arr_items_match, usys_t n_items_match, list::TList<usys_t>& ls_result, usys_t offset, usys_t n_hits_max) const
				{
					usys_t n = 0;
					switch(direction)
					{
						case DIRECTION_FORWARD:
							for(usys_t i = 0; i < n_items_match; i++)
								if(this->Match(arr_items_match[i]))
								{
									ls_result.Add(offset + i);
									n++;
								}
							break;
						case DIRECTION_BACKWARD:
							CL3_NOT_IMPLEMENTED;
							break;
					}
					return n;
				}

				template<class T>
				usys_t	TArrayMatcher<T>::MatchFirst	(EDirection direction, const T* arr_items_match, usys_t n_items_match) const
				{
					switch(direction)
					{
						case DIRECTION_FORWARD:
							for(usys_t i = 0; i < n_items_match; i++)
								if(this->Match(arr_items_match[i]))
									return i;
							break;
						case DIRECTION_BACKWARD:
							CL3_NOT_IMPLEMENTED;
							break;
					}
					return (usys_t)-1;
				}

				template<class T>
				CLASS	TArrayMatcher<T>::TArrayMatcher	(const T* arr_items_local, usys_t n_items_local) : arr_items_local(arr_items_local), n_items_local(n_items_local)
				{
				}
			}
		}
	}
}

#endif
