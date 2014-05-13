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
		using namespace collection::list;

		namespace	text
		{
			namespace	string
			{
				CLASS	TUString::TUString	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const char*     str, usys_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const wchar_t* wstr, usys_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const TUTF32*  ustr, usys_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const TUString& other) : TList<TUTF32>(other)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::TUString	(const IString&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TUString::~TUString	()
				{
				}

				CLASS	TCString::TCString	(const TUString&, const encoding::ICodec*)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TUStringUPtr	Stringify	(FPrint print, const void* object)
				{
					TUStringUPtr ret;
					if(print)
					{
						ret = system::memory::MakeUniquePtr(new TUString());
						print(ret.Object(), object);
					}
					else
					{
						ret = system::memory::MakeUniquePtr(new TUString(L"<unprintable>"));
					}
					return ret;
				}
			}
		}
	}
}
