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

#include "io_text_string.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	list
			{
// 				template class CL3PUBF TList<io::text::TUTF32>;
			}
		}

		namespace	text
		{
			namespace	string
			{
				off64_t	TUString::Left	(size_t sz_unit) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				off64_t	TUString::Space	(size_t sz_unit) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const char*     str, size_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const wchar_t* wstr, size_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const TUTF32*  ustr, size_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const TUString&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const IString&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::~TUString	()
				{
					CL3_NOT_IMPLEMENTED;
				}
			}
		}
	}
}
