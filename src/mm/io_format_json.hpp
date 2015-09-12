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

#ifndef	_include_cl3_mm_io_format_json_hpp_
#define	_include_cl3_mm_io_format_json_hpp_

#include <cl3/core/system_compiler.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/io_text.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_stream.hpp>

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	format
		{
			namespace	json
			{
				using namespace system::types;

				class	IElement
				{
					public:
						CL3PUBF	virtual	CLASS	~IElement	() {}
						CL3PUBF	virtual	void	Save		(text::ITextWriter& tw) = 0;
				};

				struct	TNumber : public IElement
				{
					double value;
					
					CL3PUBF	void	Save	(text::ITextWriter& tw) final override;
				};

				struct	TText : public IElement
				{
					text::string::TString text;
					
					CL3PUBF	void	Save	(text::ITextWriter& tw) final override;
				};

				struct	TBoolean : public IElement
				{
					bool value;
					
					CL3PUBF	void	Save	(text::ITextWriter& tw) final override;
				};

				class	TObject : public IElement
				{
					public:
						struct	TMember
						{
							text::string::TString name;
							system::memory::TUniquePtr<IElement> node;
						};

					protected:
						collection::list::TList<TMember*> members;

					public:
						CL3PUBF	const collection::list::TList<const TMember* const>&
										Members	() const CL3_GETTER;
						CL3PUBF	collection::list::TList<TMember*>&
										Members	() CL3_GETTER;

						CL3PUBF	TMember&		operator[]	(const TString& name) CL3_GETTER;
						CL3PUBF	TMember&		operator[]	(ssys_t index) CL3_GETTER;

						CL3PUBF	const TMember&	operator[]	(const TString& name) const CL3_GETTER;
						CL3PUBF	const TMember&	operator[]	(ssys_t index) const CL3_GETTER;

						CL3PUBF	void	Save	(text::ITextWriter& tw) final override;
				};

				class	TArray : public IElement
				{
					protected:
						collection::list::TList<IElement*> members;

					public:
						CL3PUBF	const collection::list::TList<const IElement* const>&
										Items	() const CL3_GETTER;
						CL3PUBF	collection::list::TList<TMember*>&
										Items	() CL3_GETTER;
										
						CL3PUBF	void	Save	(text::ITextWriter& tw) final override;
				};

				system::memory::TUniquePtr<IElement>	Parse	(text::ITextReader& tr);
			}
		}
	}
}

#endif
