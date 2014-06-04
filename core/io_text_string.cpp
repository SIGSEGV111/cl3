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

extern "C" size_t strlen(const char *s) throw();
extern "C" size_t strnlen(const char *s, size_t maxlen) throw();
extern "C" size_t wcsnlen(const wchar_t *s, size_t maxlen) throw();
extern "C" size_t wcslen(const wchar_t *s) throw();
extern "C" int memcmp(const void *s1, const void *s2, size_t n);

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

				void		TString::Append		(const char& item_append)
				{
					this->Append(TUTF32(item_append));
				}

				void		TString::Append		(const char* arr_items_append, usys_t n_items_append)
				{
					TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
					d->Sink(this);
					d->Write((const byte_t*)arr_items_append, n_items_append);
				}

				void		TString::Append		(const IStaticCollection<char>& collection)
				{
					/*
						TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
						TUniquePtr<collection::IStaticIterator<const char> > it = collection.CreateStaticIterator();
						d->Sink(this);
						d->ReadIn(*it.UObjPtr());
					*/
					CL3_NOT_IMPLEMENTED;
				}

				void		TString::Append		(const wchar_t& item_append)
				{
					this->Append(TUTF32(item_append));
				}

				void		TString::Append		(const wchar_t* arr_items_append, usys_t n_items_append)
				{
					TUniquePtr<IDecoder> d = CODEC_CXX_WCHAR->CreateDecoder();
					d->Sink(this);
					d->Write((const byte_t*)arr_items_append, n_items_append * sizeof(wchar_t));
				}

				void		TString::Append		(const IStaticCollection<wchar_t>& collection)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator+=	(const char chr_append)
				{
					this->Append(TUTF32(chr_append));
					return *this;
				}

				TString&	TString::operator+=	(const wchar_t chr_append)
				{
					this->Append(TUTF32(chr_append));
					return *this;
				}

				TString&	TString::operator+=	(const TUTF32 chr_append)
				{
					this->Append(chr_append);
					return *this;
				}

				TString&	TString::operator+=	(const char* str_append)
				{
					this->Append(str_append, strlen(str_append));
					return *this;
				}

				TString&	TString::operator+=	(const wchar_t* str_append)
				{
					this->Append(str_append, wcslen(str_append));
					return *this;
				}

				TString&	TString::operator+=	(const TUTF32* str_append)
				{
					this->Append(str_append, ustrlen(str_append, (usys_t)-1));
					return *this;
				}

				TString&	TString::operator+=	(const collection::IStaticCollection<char>& collection_append)
				{
					this->Append(collection_append);
					return *this;
				}

				TString&	TString::operator+=	(const collection::IStaticCollection<wchar_t>& collection_append)
				{
					this->Append(collection_append);
					return *this;
				}

				TString&	TString::operator+=	(const collection::IStaticCollection<TUTF32>& collection_append)
				{
					this->Append(collection_append);
					return *this;
				}

				TString&	TString::operator-=	(const char chr_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const wchar_t chr_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const TUTF32 chr_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const char* str_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const wchar_t* str_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const TUTF32* str_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const collection::IStaticCollection<char>& collection_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const collection::IStaticCollection<wchar_t>& collection_remove)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::operator-=	(const collection::IStaticCollection<TUTF32>& collection_remove)
				{
					CL3_NOT_IMPLEMENTED;
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

				bool		TString::operator==	(const TString& rhs) const
				{
					const usys_t n = this->Count();
					if(n != rhs.Count())
						return false;
					for(usys_t i = 0; i < n; i++)
						if((*this)[i] != rhs[i])
							return false;
					return true;
				}

				bool		TString::operator!=	(const TString& rhs) const
				{
					return !((*this) == rhs);
				}

				void		TString::Replace	(usys_t index, usys_t length, const TString& str_replace)
				{
					CL3_CLASS_ERROR(index + length > this->Count(), collection::TIndexOutOfBoundsException, index + length, this->Count());
					const usys_t l = CL3_MIN(length, str_replace.Count());
					for(usys_t i = 0; i < l; i++)
						(*this)[index + i] = str_replace[i];

					if(str_replace.Count() > length)
						this->Insert(index + l, str_replace.ItemPtr(l), str_replace.Count() - length);
					else if(str_replace.Count() < length)
						this->Remove(index + l, length - str_replace.Count());
				}

				usys_t		TString::Replace	(const TString& str_find, const TString& str_replace, usys_t n_times_max)
				{
					for(usys_t n_times = 0, idx_start = 0; n_times < n_times_max; idx_start += str_find.Count(), n_times++)
					{
						idx_start = this->Find(str_find, idx_start);
						if(idx_start == (usys_t)-1)
							return n_times;
						this->Replace(idx_start, str_find.Count(), str_replace);
					}
					return n_times_max;
				}

				usys_t		TString::Find		(const TString& str_find, usys_t idx_start, collection::list::EDirection direction) const
				{
					if(this->Count() < str_find.Count())
						return (usys_t)-1;

					const usys_t n = this->Count() - str_find.Count();
					switch(direction)
					{
						case DIRECTION_FORWARD:
							for(usys_t i = idx_start; i <= n; i++)
								if(memcmp(this->ItemPtr(i), str_find.ItemPtr(0), str_find.Count() * 4) == 0)
									return i;
							return (usys_t)-1;

						case DIRECTION_BACKWARD:
							for(usys_t i = idx_start; i <= n; i++)
								if(memcmp(this->ItemPtr(this->Count()-1-i), str_find.ItemPtr(0), str_find.Count() * 4) == 0)
									return i;
							return (usys_t)-1;
					}
				}

				TString		TString::Left		(usys_t n_chars) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString		TString::Right		(usys_t n_chars) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString		TString::Mid		(usys_t index, usys_t n_chars) const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString		TString::Lower		() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString		TString::Upper		() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::Pad		(usys_t n_pad, TUTF32 uchr_pad)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::ToLower	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				TString&	TString::ToUpper	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				usys_t	TString::Length		() const
				{
					return ustrlen(this->arr_items, this->n_items_current);
				}

				CLASS	TString::TString	() : TList()
				{
					//	nothing else to do
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

				CLASS	TCString::TCString	(const TString& str, const encoding::ICodec* codec) : TList(), codec(codec)
				{
					TUniquePtr<IEncoder> e = codec->CreateEncoder();
					e->Sink(this);
					e->Write(str.ItemPtr(0), str.Length());
					e->Write(&TUTF32::TERMINATOR, 1);
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
