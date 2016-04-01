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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

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
		namespace	text
		{
			namespace	string
			{
				using namespace collection;
				using namespace collection::list;
				using namespace system::memory;
				using namespace encoding;
				using namespace error;

				static	usys_t	UnicodeStringLength	(const TUTF32* str, usys_t maxlen)
				{
					for(usys_t i = 0; i < maxlen; i++)
						if(str[i].code == 0U)
							return i;
					return maxlen;
				}

				/*****************************************************************/

				void		TString::Prepend		(const char& item_prepend)
				{
					this->Insert(0, TUTF32(item_prepend));
				}

				void		TString::Prepend		(const char* arr_items_prepend, usys_t n_items_prepend)
				{
					if(n_items_prepend == (usys_t)-1)
						n_items_prepend = strlen(arr_items_prepend);
					TString tmp;
					TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
					d->Sink(&tmp);
					d->Write((const byte_t*)arr_items_prepend, n_items_prepend);
					this->Insert(0, tmp);
				}

				void		TString::Prepend		(const IStaticCollection<const char>& collection)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void		TString::Prepend		(const wchar_t& item_prepend)
				{
					this->Insert(0, TUTF32(item_prepend));
				}

				void		TString::Prepend		(const wchar_t* arr_items_prepend, usys_t n_items_prepend)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void		TString::Prepend		(const IStaticCollection<const wchar_t>& collection)
				{
					CL3_NOT_IMPLEMENTED;
				}

				void		TString::Append		(const char& item_append)
				{
					this->Append(TUTF32(item_append));
				}

				void		TString::Append		(const char* arr_items_append, usys_t n_items_append)
				{
					if(n_items_append == (usys_t)-1)
						n_items_append = strlen(arr_items_append);
					TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
					d->Sink(this);
					d->Write((const byte_t*)arr_items_append, n_items_append);
				}

				void		TString::Append		(const IStaticCollection<const char>& collection)
				{
					TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
					TUniquePtr<collection::IStaticIterator<const char> > it = collection.CreateStaticIterator();
					d->Sink(this);

					it->MoveHead();
					while(it->MoveNext())
						d->Write((const byte_t*)&it->Item(), sizeof(char));
				}

				void		TString::Append		(const wchar_t& item_append)
				{
					this->Append(TUTF32(item_append));
				}

				void		TString::Append		(const wchar_t* arr_items_append, usys_t n_items_append)
				{
					if(n_items_append == (usys_t)-1)
						n_items_append = wcslen(arr_items_append);
					TUniquePtr<IDecoder> d = CODEC_CXX_WCHAR->CreateDecoder();
					d->Sink(this);
					d->Write((const byte_t*)arr_items_append, n_items_append * sizeof(wchar_t));
				}

				void		TString::Append		(const IStaticCollection<const wchar_t>& collection)
				{
					if(CODEC_CXX_WCHAR == CODEC_UTF32)
					{
						TUniquePtr<collection::IStaticIterator<const wchar_t> > it = collection.CreateStaticIterator();

						it->MoveHead();
						while(it->MoveNext())
							this->Append((const TUTF32&)it->Item());
					}
					else
					{
						TUniquePtr<IDecoder> d = CODEC_CXX_WCHAR->CreateDecoder();
						TUniquePtr<collection::IStaticIterator<const wchar_t> > it = collection.CreateStaticIterator();
						d->Sink(this);

						it->MoveHead();
						while(it->MoveNext())
							d->Write((const byte_t*)&it->Item(), sizeof(wchar_t));
					}
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
					this->Append(str_append, UnicodeStringLength(str_append, (usys_t)-1));
					return *this;
				}

				TString&	TString::operator+=	(const collection::IStaticCollection<const char>& collection_append)
				{
					this->Append(collection_append);
					return *this;
				}

				TString&	TString::operator+=	(const collection::IStaticCollection<const wchar_t>& collection_append)
				{
					this->Append(collection_append);
					return *this;
				}

				TString&	TString::operator+=	(const collection::IStaticCollection<const TUTF32>& collection_append)
				{
					this->Append(collection_append);
					return *this;
				}

				TString		TString::operator+	(const TString& rhs) const
				{
					return TString(*this) += rhs;
				}

				TString		TString::operator+	(TString&& rhs) const
				{
					rhs.Insert(0, *this);
					return rhs;
				}

				/*
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
				*/

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

				int			TString::Compare	(const TString& other, const TCollationSequence* collation_sequence) const
				{
					if(collation_sequence == NULL)
					{
						const usys_t n = CL3_MIN(this->n_items_current, other.n_items_current);

						//	compare chararacters
						for(usys_t i = 0; i < n; i++)
							if(this->arr_items[i] > other.arr_items[i])
								return 1;
							else if(this->arr_items[i] < other.arr_items[i])
								return -1;

						//	compare length
						if(this->n_items_current > other.n_items_current)
							return 1;
						else if(this->n_items_current < other.n_items_current)
							return -1;

						//	equal
						return 0;
					}
					else
						CL3_NOT_IMPLEMENTED;
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
					for(usys_t n_times = 0, idx_start = 0; n_times < n_times_max && idx_start < this->Count(); idx_start += str_find.Count(), n_times++)
					{
						idx_start = this->Find(str_find, idx_start, DIRECTION_FORWARD);
						if(idx_start == (usys_t)-1)
							return n_times;
						this->Replace(idx_start, str_find.Count(), str_replace);
					}
					return n_times_max;
				}

				usys_t		TString::Find		(const TString& str_find) const
				{
					return this->Find(str_find, 0, DIRECTION_FORWARD);
				}

				usys_t		TString::Find		(const TString& str_find, usys_t idx_start, collection::EDirection direction) const
				{
					if(this->Count() < str_find.Count())
						return (usys_t)-1;

					if(idx_start == (usys_t)-1)
						idx_start = this->Count()-1;

					CL3_CLASS_ERROR(idx_start >= this->Count(), TIndexOutOfBoundsException, idx_start, this->Count());

					const usys_t n = this->Count() - str_find.Count();
					switch(direction)
					{
						case DIRECTION_FORWARD:
							for(usys_t i = idx_start; i <= n; i++)
								if(memcmp(this->ItemPtr(i), str_find.ItemPtr(0), str_find.Count() * 4) == 0)
									return i;
							return (usys_t)-1;

						case DIRECTION_BACKWARD:
							for(usys_t i = idx_start; i != (usys_t)-1 ; i--)
								if(memcmp(this->ItemPtr(i), str_find.ItemPtr(0), str_find.Count() * 4) == 0)
									return i;
							return (usys_t)-1;
					}
					CL3_UNREACHABLE; //LCOV_EXCL_LINE
				}

				TString		TString::Left		(usys_t n_chars) const
				{
					if(n_chars > this->Count())
						n_chars = this->Count();
					if(n_chars == 0)
						return TString();
					TString r;
					r.Append(this->ItemPtr(0), n_chars);
					return r;
				}

				TString		TString::Right		(usys_t n_chars) const
				{
					if(n_chars > this->Count())
						n_chars = this->Count();
					if(n_chars == 0)
						return TString();
					TString r;
					r.Append(this->ItemPtr(this->Count() - n_chars), n_chars);
					return r;
				}

				TString		TString::Slice		(usys_t index, usys_t n_chars) const
				{
					if(n_chars == (usys_t)-1)
						n_chars = this->Count() - index;
					if(n_chars == 0)
						return TString();
					CL3_CLASS_ERROR(index + n_chars > this->Count(), collection::TIndexOutOfBoundsException, index + n_chars - 1, this->Count());
					TString r;
					r.Append(this->ItemPtr(index), n_chars);
					return r;
				}

				void		TString::Trim		(const IStaticCollection<const TUTF32>& collection, int position)
				{
					TUniquePtr<collection::IStaticIterator<const TUTF32> > it = collection.CreateStaticIterator();

					const usys_t n = this->Count();
					usys_t n_head = 0;
					usys_t n_tail = 0;

					if(position & POSITION_HEAD)
					{
						for(; n_head < n; n_head++)
						{
							const TUTF32 chr = (*this)[n_head];
							it->MoveHead();
							while(it->MoveNext())
								if( chr == it->Item() )
									goto gt_next_head;
							break;
							gt_next_head:;
						}
					}

					if(position & POSITION_TAIL)
					{
						for(; n_tail < n; n_tail++)
						{
							const TUTF32 chr = (*this)[n-1-n_tail];
							it->MoveHead();
							while(it->MoveNext())
								if( chr == it->Item() )
									goto gt_next_tail;
							break;
							gt_next_tail:;
						}
					}

					this->Cut(n_head, n_tail);
				}

				void		TString::Pad			(EPosition position, usys_t n_pad, TUTF32 chr_pad)
				{
					switch(position)
					{
						case POSITION_HEAD:
							this->Prealloc(n_pad);
							memmove(this->arr_items + n_pad, this->arr_items, this->n_items_current * sizeof(TUTF32));
							this->n_items_current += n_pad;
							this->n_items_prealloc -= n_pad;
							for(usys_t i = 0; i < n_pad; i++)
								this->arr_items[i] = chr_pad;
							break;
						case POSITION_TAIL:
							this->Grow(n_pad, chr_pad);
							break;
					}
				}

				TString		TString::Lower		() const	//	LCOV_EXCL_LINE
				{
					CL3_NOT_IMPLEMENTED;	//	LCOV_EXCL_LINE
				}

				TString		TString::Upper		() const	//	LCOV_EXCL_LINE
				{
					CL3_NOT_IMPLEMENTED;	//	LCOV_EXCL_LINE
				}

				TString&	TString::ToLower	()	//	LCOV_EXCL_LINE
				{
					CL3_NOT_IMPLEMENTED;	//	LCOV_EXCL_LINE
				}

				TString&	TString::ToUpper	()	//	LCOV_EXCL_LINE
				{
					CL3_NOT_IMPLEMENTED;	//	LCOV_EXCL_LINE
				}

				usys_t	TString::Length		() const
				{
					return UnicodeStringLength(this->arr_items, this->n_items_current);
				}

				CLASS	TString::TString	() : TList<TUTF32>()
				{
					//	nothing else to do
				}

				CLASS	TString::TString	(const char*    str, usys_t maxlen) : TList<TUTF32>()
				{
					CL3_CLASS_ERROR(str == NULL, TException, "invalid source string pointer");
					TUniquePtr<IDecoder> d = CODEC_CXX_CHAR->CreateDecoder();
					d->Sink(this);
					d->Write((const byte_t*)str, strnlen(str, maxlen) * sizeof(char));
				}

				CLASS	TString::TString	(const wchar_t* str, usys_t maxlen) : TList<TUTF32>()
				{
					CL3_CLASS_ERROR(str == NULL, TException, "invalid source string pointer");
					if(CODEC_CXX_WCHAR == CODEC_UTF32)
					{
						this->Append((const TUTF32*)str, wcsnlen(str, maxlen));
					}
					else
					{
						TUniquePtr<IDecoder> d = CODEC_CXX_WCHAR->CreateDecoder();
						d->Sink(this);
						d->Write((const byte_t*)str, wcsnlen(str, maxlen) * sizeof(wchar_t));
					}
				}

				CLASS	TString::TString	(const TUTF32*  str, usys_t maxlen) : TList<TUTF32>()
				{
					CL3_CLASS_ERROR(str == NULL, TException, "invalid source string pointer");
					this->Append(str, UnicodeStringLength(str, maxlen));
				}

				CLASS	TString::TString	(const TString& other) : event::IObservable(), TList<const TUTF32>(other), TList<TUTF32>(other)
				{
					//	nothing else to do
				}

				CLASS	TString::TString	(TString&& other) //: TList<TUTF32>(static_cast<TList<TUTF32>&&>(other))
				{
					//	nothing else to do


					//	FIXME BUG WORKAROUND:
					//CL3_CLASS_LOGIC_ERROR(other.arr_items != NULL);
					CL3_CLASS_LOGIC_ERROR(this->arr_items != NULL);
					this->arr_items = other.arr_items;
					other.arr_items = NULL;
					this->n_items_current = other.n_items_current;
					other.n_items_current = 0;
					this->n_items_prealloc = other.n_items_prealloc;
					other.n_items_prealloc = 0;
				}

				CLASS	TString::~TString	()
				{
					//	nothing to do
				}

				TCString&	TCString::operator=	(const TCString& rhs)
				{
					static_cast<TList<byte_t>&>(*this) = rhs;
					this->codec = rhs.codec;
					return *this;
				}

				TCString&	TCString::operator=	(TCString&& rhs)
				{
					static_cast<TList<byte_t>&>(*this) = rhs;
					this->codec = rhs.codec;
					return *this;
				}

				CLASS		TCString::TCString	(const encoding::ICodec* codec) : TList<byte_t>(), codec(codec)
				{
				}

				CLASS		TCString::TCString	(const TString& str, const encoding::ICodec* codec) : TList<byte_t>(), codec(codec)
				{
					TUniquePtr<IEncoder> e = codec->CreateEncoder();
					e->Sink(this);
					if(str.Count() > 0)
						e->Write(str.ItemPtr(0), str.Length());
					e->Write(&TUTF32::TERMINATOR, 1);
				}

				CLASS		TCString::TCString	(const TCString& other) : event::IObservable(), TList<const byte_t>(other), TList<byte_t>(other), codec(other.codec)
				{
					//	nothing else to do
				}

				CLASS		TCString::TCString	(TCString&& other) // : TList<byte_t>(other), codec(other.codec)
				{
					//	nothing else to do

					//	FIXME BUG WORKAROUND
					CL3_CLASS_LOGIC_ERROR(this->arr_items != NULL);
					this->arr_items = other.arr_items;
					other.arr_items = NULL;
					this->n_items_current = other.n_items_current;
					other.n_items_current = 0;
					this->n_items_prealloc = other.n_items_prealloc;
					other.n_items_prealloc = 0;
				}

				TString	Stringify	(FPrint print, const void* object)
				{
					if(print)
					{
						TString ret;
						print(ret, object);
						return ret;
					}
					else
					{
						return "<unprintable>";
					}
				}
			}
		}
	}
}
