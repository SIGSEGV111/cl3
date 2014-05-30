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
#include "io_text_encoding.hpp"

extern "C" size_t strnlen(const char *s, size_t maxlen) throw();
extern "C" size_t wcsnlen(const wchar_t *s, size_t maxlen) throw();

namespace	cl3
{
	namespace	io
	{
		using namespace collection::list;

		namespace	text
		{
			namespace	string
			{
				using namespace system::memory;
				using namespace encoding;

				static	usys_t	ustrlen	(const TUTF32* str, usys_t maxlen)
				{
					for(usys_t i = 0; i < maxlen; i++)
						if(str[i].code == 0U)
							return i;
					return maxlen;
				}

				TString&	TString::operator=	(const TString& other)
				{
					static_cast<TList<TUTF32>& >(*this) = other;
					return *this;
				}

				TString&	TString::operator=	(TString&& other)
				{
					static_cast<TList<TUTF32>& >(*this) = other;
					return *this;
				}

				usys_t	TString::Length		() const
				{
					return ustrlen(this->arr_items, this->n_items_current);
				}

				CLASS	TString::TString	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TString::TString	(const char*    str, usys_t maxlen) : TList()
				{
					TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
					d->Sink(this);
					d->Write((const byte_t*)str, strnlen(str, maxlen) * sizeof(char));
				}

				CLASS	TString::TString	(const wchar_t* str, usys_t maxlen) : TList()
				{
					TUniquePtr<IDecoder> d = CODEC_CXX_WCHAR->CreateDecoder();
					d->Sink(this);
					d->Write((const byte_t*)str, wcsnlen(str, maxlen) * sizeof(wchar_t));
				}

				CLASS	TString::TString	(const TUTF32*  str, usys_t maxlen) : TList()
				{
					this->Append(str, ustrlen(str, maxlen));
				}

				CLASS	TString::TString	(const TString& other) : TList<TUTF32>(other)
				{
					//	nothing else to do
				}

				CLASS	TString::TString	(TString&& other) : TList(other)
				{
					//	nothing else to do
				}

				CLASS	TString::~TString	()
				{
					//	nothing to do
				}

				CLASS	TCString::TCString	(const TString& str, const encoding::ICodec* codec)
				{
					TUniquePtr<IEncoder> e = codec->CreateEncoder();
					e->Sink(this);
					e->Write(str.ItemPtr(0), str.Length());
				}

				TStringUPtr	Stringify	(FPrint print, const void* object)
				{
					TStringUPtr ret;
					if(print)
					{
						ret = system::memory::MakeUniquePtr(new TString());
						print(*ret.UObjPtr(), object);
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
