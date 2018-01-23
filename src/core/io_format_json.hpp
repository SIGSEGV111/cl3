/*
 *	 libcl3 - common library version 3
 *	 Copyright (C) 2013	Simon Brennecke
 *
 *	 This program is free software: you can redistribute it and/or modify
 *	 it under the terms of the GNU General Public License as published by
 *	 the Free Software Foundation, either version 3 of the License, or
 *	 (at your option) any later version.
 *
 *	 This program is distributed in the hope that it will be useful,
 *	 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *
 *	 You should have received a copy of the GNU General Public License
 *	 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	_include_cl3_core_io_format_json_hpp_
#define	_include_cl3_core_io_format_json_hpp_

#include "system_memory.hpp"
#include "system_types.hpp"
#include "io_text_string.hpp"
// #include "io_format.hpp"
#include "io_serialization.hpp"


namespace	cl3
{
	namespace	io
	{
		namespace	format
		{
			namespace	json
			{
				using namespace system::types;

				struct IElement
				{
					enum class EType
					{
						NULLVALUE,
						NUMBER,
						BOOL,
						TEXT,
						ARRAY,
						OBJECT
					};

					virtual EType Type() const CL3_GETTER = 0;
					virtual CLASS ~IElement() = default;

					bool operator==(const IElement& rhs) const { return this == &rhs; }
				};

				struct TNullValue : IElement
				{
					inline EType Type() const final override CL3_GETTER { return IElement::EType::NULLVALUE; }
				};

				struct TNumber : IElement
				{
					f64_t value;

					inline EType Type() const final override CL3_GETTER { return IElement::EType::NUMBER; }

					inline CLASS TNumber(f64_t value) : value(value) {}
				};

				struct TBoolean : IElement
				{
					bool value;

					inline EType Type() const final override CL3_GETTER { return IElement::EType::BOOL; }

					inline CLASS TBoolean(bool value) : value(value) {}
				};

				struct TText : IElement
				{
					text::string::TString text;

					inline EType Type() const final override CL3_GETTER { return IElement::EType::TEXT; }

					inline CLASS TText(text::string::TString text) : text(text) {}
				};

				struct TArray : IElement
				{
					collection::list::TList<IElement*> array;

					inline EType Type() const final override CL3_GETTER { return IElement::EType::ARRAY; }

					inline CLASS TArray(collection::list::TList<IElement*> array) : array(array) {}
				};

				struct TObject : IElement
				{
					struct TMember
					{
						bool operator==(const TMember& rhs) const { return this == &rhs; }

						text::string::TString name;
						IElement* element;
					};

					collection::list::TList<TMember> members;

					inline EType Type() const final override CL3_GETTER { return IElement::EType::OBJECT; }

					inline CLASS TObject(collection::list::TList<TMember> members) : members(members) {}
				};

				struct TBuilder
				{
					struct TMember;
					struct TValue;

					struct TValue
					{
						bool operator==(const TValue& rhs) const { return this == &rhs; }

						system::memory::TUniquePtr<IElement> element;

						inline CLASS TValue() : element(system::memory::MakeUniquePtr<IElement>(new TNullValue())) {}
						inline CLASS TValue(bool boolean) : element(system::memory::MakeUniquePtr<IElement>(new TBoolean(boolean))) {}
						inline CLASS TValue(f64_t number) : element(system::memory::MakeUniquePtr<IElement>(new TNumber(number))) {}
						inline CLASS TValue(text::string::TString text) : element(system::memory::MakeUniquePtr<IElement>(new TText(text))) {}
// 						CL3PUBF CLASS TValue(std::initializer_list<TValue> array);
// 						CL3PUBF CLASS TValue(std::initializer_list<TMember> object);
					};

					struct TMember
					{
						bool operator==(const TMember& rhs) const { return this == &rhs; }

						text::string::TString key;
						TValue value;

						inline CLASS TMember(text::string::TString key) : key(key), value() {}
						inline CLASS TMember(text::string::TString key, bool boolean) : key(key), value(boolean) {}
						inline CLASS TMember(text::string::TString key, f64_t number) : key(key), value(number) {}
						inline CLASS TMember(text::string::TString key, text::string::TString text) : key(key), value(text) {}
// 						inline CLASS TMember(text::string::TString key, std::initializer_list<TValue> array) : key(key), value(array) {}
// 						inline CLASS TMember(text::string::TString key, std::initializer_list<TMember> object) : key(key), value(object) {}
					};

					TValue root;

					inline operator IElement&() { return *root.element; }

					inline CLASS TBuilder() : root() {}
					inline CLASS TBuilder(bool boolean) : root(boolean) {}
					inline CLASS TBuilder(f64_t number) : root(number) {}
					inline CLASS TBuilder(text::string::TString text) : root(text) {}
// 					inline CLASS TBuilder(std::initializer_list<TValue> array) : root(array) {}
// 					inline CLASS TBuilder(std::initializer_list<TMember> object) : root(object) {}
				};

				CL3PUBF void Stringify(const IElement&, text::ITextWriter&);
				CL3PUBF system::memory::TUniquePtr<IElement> Parse(text::ITextReader&);
			}
		}
	}
}

#endif
