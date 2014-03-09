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

#ifndef	_include_cl3_core_io_text_string_hpp_
#define	_include_cl3_core_io_text_string_hpp_

#include "io_text.hpp"
#include "io_collection_list.hpp"
#include "io_text_encoding.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	text
		{
			namespace	string
			{
				struct	IString;
				class	TUString;
				class	TCString;

				struct	IString : virtual ITextStream, virtual collection::IDynamicCollection<TUTF32>
				{
					virtual	CLASS	~IString	() {}
				};

				class	TUString :	public virtual IString,
									public virtual collection::list::TList<TUTF32>,
									public virtual encoding::ACharDecoder,
									public virtual encoding::ACharEncoder,
									public virtual encoding::AWCharDecoder,
									public virtual encoding::AWCharEncoder
				{
					public:
						off64_t	Left	(size_t sz_unit) const;
						off64_t	Space	(size_t sz_unit) const;

						CL3PUBF	CLASS	TUString	();
						CL3PUBF	CLASS	TUString	(const char*     str, size_t maxlen = (size_t)-1);
						CL3PUBF	CLASS	TUString	(const wchar_t* wstr, size_t maxlen = (size_t)-1);
						CL3PUBF	CLASS	TUString	(const TUTF32*  ustr, size_t maxlen = (size_t)-1);
						CL3PUBF	CLASS	TUString	(const TUString&);
						CL3PUBF	CLASS	TUString	(const IString&);
						CL3PUBF	CLASS	~TUString	();

						CL3PUBF	GETTER	system::task::synchronization::IMutex&	Mutex	();
				};
			}
		}
	}
}

#endif
