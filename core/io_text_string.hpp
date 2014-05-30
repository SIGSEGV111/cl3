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
		namespace	collection
		{
			namespace	list
			{
				template class CL3PUBF TList<io::text::TUTF32>;
			}
		}

		namespace	text
		{
			namespace	string
			{
				class	TString;
				class	TCString;

				typedef	system::memory::TUniquePtr<TString>	TStringUPtr;

				class	CL3PUBT	TString :
								public virtual ITextStream,
								public virtual collection::list::TList<TUTF32>,
								public virtual encoding::ACharDecoder,
								public virtual encoding::ACharEncoder,
								public virtual encoding::AWCharDecoder,
								public virtual encoding::AWCharEncoder
				{
					public:
						CL3PUBF	TString&	operator=	(const TString&);
						CL3PUBF	TString&	operator=	(TString&&);

						//	append string
						CL3PUBF	TString&	operator+=	(const char chr_append);
						CL3PUBF	TString&	operator+=	(const wchar_t chr_append);
						CL3PUBF	TString&	operator+=	(const TUTF32 chr_append);
						CL3PUBF	TString&	operator+=	(const char* str_append);
						CL3PUBF	TString&	operator+=	(const wchar_t* str_append);
						CL3PUBF	TString&	operator+=	(const TUTF32* str_append);
						CL3PUBF	TString&	operator+=	(const collection::IStaticCollection<char>& collection_append);
						CL3PUBF	TString&	operator+=	(const collection::IStaticCollection<wchar_t>& collection_append);
						CL3PUBF	TString&	operator+=	(const collection::IStaticCollection<TUTF32>& collection_append);

						//	remove string
						CL3PUBF	TString&	operator-=	(const char chr_remove);
						CL3PUBF	TString&	operator-=	(const wchar_t chr_remove);
						CL3PUBF	TString&	operator-=	(const TUTF32 chr_remove);
						CL3PUBF	TString&	operator-=	(const char* str_remove);
						CL3PUBF	TString&	operator-=	(const wchar_t* str_remove);
						CL3PUBF	TString&	operator-=	(const TUTF32* str_remove);
						CL3PUBF	TString&	operator-=	(const collection::IStaticCollection<char>& collection_remove);
						CL3PUBF	TString&	operator-=	(const collection::IStaticCollection<wchar_t>& collection_remove);
						CL3PUBF	TString&	operator-=	(const collection::IStaticCollection<TUTF32>& collection_remove);

						CL3PUBF	usys_t		Replace		(const TString& find, const TString& replace, usys_t n_times = (usys_t)-1);	//	returns the number of times of which <find> was replaced with <replace>

						CL3PUBF	TString		Left		(usys_t n_chars) const CL3_GETTER;
						CL3PUBF	TString		Right		(usys_t n_chars) const CL3_GETTER;
						CL3PUBF	TString		Mid			(usys_t index, usys_t n_chars) const CL3_GETTER;
						CL3PUBF	TString		Lower		() const CL3_GETTER;
						CL3PUBF	TString		Upper		() const CL3_GETTER;

						CL3PUBF	TString&	Pad			(usys_t n_pad, TUTF32 uchr_pad);
						CL3PUBF	TString&	ToLower		();
						CL3PUBF	TString&	ToUpper		();

						//	determines the actual length in characters of the string
						//	this might differ from Count() as Length() stops at the first termination character
						//	while in contrast Count() ignores the termination character and retuns the internally
						//	accounted number of allocated characters stored within the string
						//	thus Length() has to scan the string, while Count() only returns the value of a member variable
						CL3PUBF	usys_t		Length		() const CL3_GETTER;

						CL3PUBF	CLASS	TString	();
						CL3PUBF	CLASS	TString	(const char*    cstr, usys_t maxlen = (usys_t)-1);
						CL3PUBF	CLASS	TString	(const wchar_t* wstr, usys_t maxlen = (usys_t)-1);
						CL3PUBF	CLASS	TString	(const TUTF32*  ustr, usys_t maxlen = (usys_t)-1);
						CL3PUBF	CLASS	TString	(const TString&);
						CL3PUBF	CLASS	TString	(TString&&);
						CL3PUBF	virtual	~TString();
				};

				class	CL3PUBT	TCString : public collection::list::TList<byte_t>
				{
					protected:
						byte_t* arr_bytes;
						usys_t n_bytes;

					public:
						const char*		Chars		() const { return (const char*)arr_bytes; }
						const byte_t*	Bytes		() const { return arr_bytes; }
						usys_t			Count		() const { return n_bytes; }

						CL3PUBF	CLASS	TCString	(const TString&, const encoding::ICodec*);
				};

				CL3PUBF	TStringUPtr	Stringify	(FPrint print, const void* object);
			}
		}
	}
}

#endif
