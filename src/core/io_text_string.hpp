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
				class	TString;
				class	TCString;

				enum	EPosition
				{
					POSITION_HEAD = 1,
					POSITION_TAIL = 2
				};

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
						using collection::list::TList<TUTF32>::Append;
						using collection::list::TList<TUTF32>::Find;

						CL3PUBF	void		Prepend		(const char& item_append);
						CL3PUBF	void		Prepend		(const char* arr_items_append, usys_t n_items_append = (usys_t)-1);
						CL3PUBF	void		Prepend		(const IStaticCollection<const char>& collection);
						CL3PUBF	void		Prepend		(const wchar_t& item_append);
						CL3PUBF	void		Prepend		(const wchar_t* arr_items_append, usys_t n_items_append = (usys_t)-1);
						CL3PUBF	void		Prepend		(const IStaticCollection<const wchar_t>& collection);

						CL3PUBF	void		Append		(const char& item_append);
						CL3PUBF	void		Append		(const char* arr_items_append, usys_t n_items_append = (usys_t)-1);
						CL3PUBF	void		Append		(const IStaticCollection<const char>& collection);
						CL3PUBF	void		Append		(const wchar_t& item_append);
						CL3PUBF	void		Append		(const wchar_t* arr_items_append, usys_t n_items_append = (usys_t)-1);
						CL3PUBF	void		Append		(const IStaticCollection<const wchar_t>& collection);

						//	append string
						CL3PUBF	TString&	operator+=	(const char chr_append);
						CL3PUBF	TString&	operator+=	(const wchar_t chr_append);
						CL3PUBF	TString&	operator+=	(const TUTF32 chr_append);
						CL3PUBF	TString&	operator+=	(const char* str_append);
						CL3PUBF	TString&	operator+=	(const wchar_t* str_append);
						CL3PUBF	TString&	operator+=	(const TUTF32* str_append);
						CL3PUBF	TString&	operator+=	(const collection::IStaticCollection<const char>& collection_append);
						CL3PUBF	TString&	operator+=	(const collection::IStaticCollection<const wchar_t>& collection_append);
						CL3PUBF	TString&	operator+=	(const collection::IStaticCollection<const TUTF32>& collection_append);

						CL3PUBF	TString		operator+	(const TString& rhs);
						CL3PUBF	TString		operator+	(TString&& rhs);

						//	remove string
						/*
						CL3PUBF	TString&	operator-=	(const char chr_remove);
						CL3PUBF	TString&	operator-=	(const wchar_t chr_remove);
						CL3PUBF	TString&	operator-=	(const TUTF32 chr_remove);
						CL3PUBF	TString&	operator-=	(const char* str_remove);
						CL3PUBF	TString&	operator-=	(const wchar_t* str_remove);
						CL3PUBF	TString&	operator-=	(const TUTF32* str_remove);
						CL3PUBF	TString&	operator-=	(const collection::IStaticCollection<char>& collection_remove);
						CL3PUBF	TString&	operator-=	(const collection::IStaticCollection<wchar_t>& collection_remove);
						CL3PUBF	TString&	operator-=	(const collection::IStaticCollection<TUTF32>& collection_remove);
						*/

						//	assign
						CL3PUBF	TString&	operator=	(const TString&);
						CL3PUBF	TString&	operator=	(TString&&);

						//	compare
						inline	bool		operator==	(const TString& rhs) const CL3_GETTER	{ return this->Compare(rhs) == 0; }
						inline	bool		operator!=	(const TString& rhs) const CL3_GETTER	{ return this->Compare(rhs) != 0; }
						inline	bool		operator>	(const TString& rhs) const CL3_GETTER	{ return this->Compare(rhs) >  0; }
						inline	bool		operator>=	(const TString& rhs) const CL3_GETTER	{ return this->Compare(rhs) >= 0; }
						inline	bool		operator<	(const TString& rhs) const CL3_GETTER	{ return this->Compare(rhs) <  0; }
						inline	bool		operator<=	(const TString& rhs) const CL3_GETTER	{ return this->Compare(rhs) <= 0; }
						CL3PUBF	int			Compare		(const TString& other, const TCollationSequence* collation_sequence = default_collation_sequence()) const CL3_GETTER;

						//	replace by index / find & replace
						CL3PUBF	void		Replace		(usys_t index, usys_t length, const TString& str_replace);
						CL3PUBF	usys_t		Replace		(const TString& str_find, const TString& str_replace, usys_t n_times_max = (usys_t)-1);	//	returns the number of times of which <find> was replaced with <replace>

						//	find substring
						CL3PUBF	usys_t		Find		(const TString& str_find) const CL3_GETTER;
						CL3PUBF	usys_t		Find		(const TString& str_find, usys_t idx_start, collection::EDirection direction) const CL3_GETTER;

						//	return parts of the string
						CL3PUBF	TString		Left		(usys_t n_chars) const CL3_GETTER;
						CL3PUBF	TString		Right		(usys_t n_chars) const CL3_GETTER;
						CL3PUBF	TString		Slice		(usys_t index, usys_t n_chars = (usys_t)-1) const CL3_GETTER;

						//	remove/append whitespace (or other characters)
						CL3PUBF	void		Trim		(const IStaticCollection<const TUTF32>& collection = *whitespace, int position = POSITION_HEAD | POSITION_TAIL);
						CL3PUBF	void		Pad			(EPosition position, usys_t n_pad, TUTF32 chr_pad = ' ');

						CL3PUBF	TString&	ToLower		();
						CL3PUBF	TString&	ToUpper		();
						CL3PUBF	TString		Lower		() const CL3_GETTER;
						CL3PUBF	TString		Upper		() const CL3_GETTER;

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

				class	CL3PUBT	TCString : public virtual collection::list::TList<byte_t>
				{
					protected:
						const encoding::ICodec* codec;

					public:
						using collection::IStaticCollection<byte_t>::Count;
						const encoding::ICodec*
										Codec	() const { return this->codec; }
						const char*		Chars	() const { return (const char*)this->ItemPtr(0); }

						CL3PUBF	TCString&	operator=	(const TCString&);
						CL3PUBF	TCString&	operator=	(TCString&&);

						using collection::list::TList<byte_t>::Claim;

						CL3PUBF	CLASS	TCString	(const encoding::ICodec*);
						CL3PUBF	CLASS	TCString	(const TString&, const encoding::ICodec*);
						CL3PUBF	CLASS	TCString	(const TCString&);
						CL3PUBF	CLASS	TCString	(TCString&&);
				};

				CL3PUBF	TString	Stringify	(FPrint print, const void* object);
			}
		}
	}
}

#endif
