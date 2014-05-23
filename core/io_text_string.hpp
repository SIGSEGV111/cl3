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
				struct	IString;
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
						using collection::list::TList<TUTF32>::Append;
						CL3PUBF	void	Append		(const char item_append);
						CL3PUBF	void	Append		(const char* arr_items_append, usys_t n_items_append);
						CL3PUBF	void	Append		(const collection::IStaticCollection<char>& collection);
						CL3PUBF	void	Append		(const wchar_t item_append);
						CL3PUBF	void	Append		(const wchar_t* arr_items_append, usys_t n_items_append);
						CL3PUBF	void	Append		(const collection::IStaticCollection<wchar_t>& collection);

						CL3PUBF	usys_t	Replace		(const TString& find, const TString& replace, usys_t n_times = (usys_t)-1);	//	returns the number of times of which <find> was replaced with <replace>

						CL3PUBF	TString	Left		(usys_t n_chars) const;
						CL3PUBF	TString	Right		(usys_t n_chars) const;
						CL3PUBF	TString	Mid			(usys_t index, usys_t n_chars) const;

						CL3PUBF	void	Pad			(usys_t n_pad, TUTF32 uchr_pad);
						CL3PUBF	void	ToLower		();
						CL3PUBF	void	ToUpper		();

						CL3PUBF	CLASS	TString	();
						CL3PUBF	CLASS	TString	(const char*     str, usys_t maxlen = (usys_t)-1);
						CL3PUBF	CLASS	TString	(const wchar_t* wstr, usys_t maxlen = (usys_t)-1);
						CL3PUBF	CLASS	TString	(const TUTF32*  ustr, usys_t maxlen = (usys_t)-1);
						CL3PUBF	CLASS	TString	(const TString&);
						CL3PUBF	CLASS	TString	(const IString&);
						CL3PUBF	virtual	~TString	();
				};

				class	CL3PUBT	TCString
				{
					protected:
						byte_t* arr_bytes;
						usys_t n_bytes;

					public:
						byte_t*			Bytes		() const { return arr_bytes; }
						usys_t			Count		() const { return n_bytes; }

						CL3PUBF	CLASS	TCString	(const TString&, const encoding::ICodec*);
				};

				CL3PUBF	TStringUPtr	Stringify	(FPrint print, const void* object);
			}
		}
	}
}

#endif
