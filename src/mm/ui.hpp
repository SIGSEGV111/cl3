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

#ifndef	_include_cl3_mm_ui_hpp_
#define	_include_cl3_mm_ui_hpp_

#include <cl3/core/io_collection.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_serialization.hpp>
#include <cl3/core/math.hpp>
#include <cl3/core/event.hpp>
#include <cl3/core/system_time.hpp>

namespace	cl3
{
	namespace	ui
	{
		using namespace system::types;
		typedef math::TVector<s32_t,2> TV2d;

		class	TView;
		struct	IUI;

		namespace	elements
		{
			class	IElement;
		}

		namespace input
		{

			class IDevice
			{
			};

			class TEnclosure
			{
				protected:
					io::collection::list::TList<IDevice*> devices;
			};

			struct	TOnInputEventData
			{
			};

			struct	TOnKeyPressData : TOnInputEventData
			{
// 				EKeyState state;	//	the current state of the key (down/up)
				u32_t key;			//	the actual "keyboard" (or mouse, or touchscreen, or similar device) key pressed
				io::text::TUTF32 chr;	//	keycode including modifier keys translated into a unicode character if applicable, TUTF32::TERMINATOR otherwise
			};

			struct	TOnPointerMoveData : TOnInputEventData
			{
				TV2d position;
			};

			struct	TOnClickData : TOnInputEventData
			{
			};

			enum class EFocus
			{
				FOCUS_ACQUIRED,
				FOCUS_LOST
			};

			struct	TOnFocusChangeData : TOnInputEventData
			{
				EFocus state;
			};

			typedef	event::TEvent<elements::IElement, TOnFocusChangeData&>	TOnFocusChangeEvent;
			typedef	event::TEvent<elements::IElement, TOnPointerMoveData&>	TOnPointerMoveEvent;
			typedef	event::TEvent<elements::IElement, TOnKeyPressData&>		TOnKeyPressEvent;
		}

		class	CL3PUBT	IElement;
		class	CL3PUBT	TButton;
		class	CL3PUBT	TOptionBox;
		class	CL3PUBT	TListBox;
		class	CL3PUBT	TCheckBox;
		class	CL3PUBT	TTextBox;

		class	IElement
		{
			public:
				CL3PUBF	const input::TOnPointerMoveEvent&	OnPointerMove	() const CL3_GETTER;
				CL3PUBF	const input::TOnKeyPressEvent&		OnKeyPress		() const CL3_GETTER;
				CL3PUBF	const input::TOnFocusChangeEvent&	OnFocusChange	() const CL3_GETTER;

				CL3PUBF	bool	Enabled	() const CL3_GETTER;
				CL3PUBF	void	Enabled	(bool) CL3_SETTER;

				CL3PUBF	CLASS	IElement	(io::text::string::TString id);
				CL3PUBF	virtual	~IElement	();
		};

		class	TButton : public IElement
		{
			public:
				CL3PUBF	const io::text::string::TString&
								Text	() const CL3_GETTER;
				CL3PUBF	void	Text	(io::text::string::TString) CL3_SETTER;

				CL3PUBF	CLASS	TButton	(io::text::string::TString id);
				CL3PUBF	CLASS	~TButton();
		};

		class	TOptionBox : public IElement
		{
			public:
				typedef io::collection::list::TList<TOptionBox*> TOptionGroup;

				CL3PUBF	const io::text::string::TString&
								Text	() const CL3_GETTER;
				CL3PUBF	void	Text	(io::text::string::TString) CL3_SETTER;

				CL3PUBF	CLASS	TOptionBox	(io::text::string::TString id, TOptionGroup*);
				CL3PUBF	CLASS	~TOptionBox	();
		};

		class	TListBox : public IElement
		{
			protected:
				io::collection::IStaticCollection<IElement*>* collection;

			public:
				CL3PUBF	io::collection::IStaticCollection<IElement*>*
								Collection	() const CL3_GETTER;
				CL3PUBF	void	Collection	(io::collection::IStaticCollection<IElement*>*) CL3_SETTER;

				CL3PUBF	CLASS	TListBox	(io::text::string::TString id);
				CL3PUBF	CLASS	~TListBox	();
		};

		class	TPictureBox : public IElement
		{
			public:
				CL3PUBF	const io::text::string::TString&
								AlternateText	() const CL3_GETTER;
				CL3PUBF	void	AlternateText	(io::text::string::TString) CL3_SETTER;
		};

		class	TCheckBox : public IElement
		{
			public:
				CL3PUBF	const io::text::string::TString&
								Text	() const CL3_GETTER;
				CL3PUBF	void	Text	(io::text::string::TString) CL3_SETTER;

				CL3PUBF	bool	State	() const CL3_GETTER;
				CL3PUBF	void	State	(bool) CL3_SETTER;

				CL3PUBF	CLASS	TCheckBox	(io::text::string::TString id);
				CL3PUBF	CLASS	~TCheckBox	();
		};

		class	TTextBox : public IElement
		{
			public:
				CL3PUBF	const io::text::string::TString&
								Text	() const CL3_GETTER;
				CL3PUBF	void	Text	(io::text::string::TString) CL3_SETTER;

				CL3PUBF	CLASS	TTextBox	(io::text::string::TString id);
				CL3PUBF	CLASS	~TTextBox	();
		};

		class	CL3PUBT	TView
		{
			private:
				TView(const TView&) = delete;
				TView& operator=(const TView&) = delete;

			protected:
				io::collection::list::TList<IElement*> elements;

			public:
				CL3PUBF	io::collection::list::TList<IElement*>&
						Elements() CL3_GETTER;

				CL3PUBF	CLASS	TView	(io::text::string::TString id);
				CL3PUBF	CLASS	~TView	();
		};
	}
}

#endif
