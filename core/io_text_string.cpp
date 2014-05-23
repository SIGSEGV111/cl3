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
				CLASS	TString::TString	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TString::TString	(const char*     str, usys_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TString::TString	(const wchar_t* wstr, usys_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TString::TString	(const TUTF32*  ustr, usys_t maxlen)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TString::TString	(const TString& other) : TList<TUTF32>(other)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TString::~TString	()
				{
				}

				CLASS	TCString::TCString	(const TString&, const encoding::ICodec*)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TStringUPtr	Stringify	(FPrint print, const void* object)
				{
					TStringUPtr ret;
					if(print)
					{
						ret = system::memory::MakeUniquePtr(new TString());
						print(ret.Object(), object);
					}
					else
					{
						ret = system::memory::MakeUniquePtr(new TString(L"<unprintable>"));
					}
					return ret;
				}
			}
		}
	}
}
