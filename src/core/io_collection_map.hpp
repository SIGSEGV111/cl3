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

#ifndef	_include_cl3_core_io_collection_list_hpp_
#define	_include_cl3_core_io_collection_list_hpp_

// #include <stdio.h>

#include "io_collection.hpp"
#include "io_collection_array.hpp"
#include "io_collection_list.hpp"
#include "error.hpp"
#include "system_memory.hpp"
#include "io_serialization.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	map
			{
				template<class TItem, class TKey, TKey TItem::*key>
				struct	IMap<const TItem> : public IDynamicCollection<const TItem>
				{
					virtual	const TItem&	operator[]	(const TKey& key) const CL3_GETTER = 0;
				};

				template<class TItem, class TKey, TKey TItem::*key>
				struct	IMap<const TItem> : public IDynamicCollection<const TItem>
				{
					virtual	TItem&	operator[]	(const TKey& key) CL3_GETTER = 0;
				};
			}
		}
	}
}

#endif
