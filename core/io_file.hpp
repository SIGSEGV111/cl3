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

#ifndef	_include_cl3_core_io_file_hpp_
#define	_include_cl3_core_io_file_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "io_stream.hpp"
#include "io_stream_fd.hpp"
#include "io_text_string.hpp"
#include "io_collection.hpp"
#include "io_collection_array.hpp"
#include "system_time.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	file
		{
			class	CL3PUBT	TStream;
			class	CL3PUBT	TMapping;
			class	CL3PUBT	TFile;
			class   CL3PUBT	TDiectoryBrowser;

			/************************************************************************/

			enum	EAccess
			{
				FILE_ACCESS_READ = 1,
				FILE_ACCESS_WRITE = 2,
				FILE_ACCESS_EXECUTE = 4
			};

			enum	ESharing
			{
				MAP_SHARING_RW,			//	the mappings are kept coherent at all times
				MAP_SHARING_COW,		//	create a simple copy-on-write mapping. changes to the original file or mapping might be visible in the copy
				MAP_SHARING_SNAPSHOT	//	take a snapshot-copy from the original mapping and employ copy-on-write semantics afterwards, changes to the original mapping are not visible in the copy
			};

			enum	ECreate
			{
				FILE_CREATE_NEVER,	//	never create the file - the file must exist or the call fails
				FILE_CREATE_ALWAYS,	//	always create a new file - destroy/truncate existing file, fails if it can not create a new file
				FILE_CREATE_CAN		//	only create if the file does not yet exist
			};

			enum	EEntryType
			{
				ENTRY_TYPE_UNKNOWN = 0,
				ENTRY_TYPE_FILE,
				ENTRY_TYPE_DIRECTORY,
				ENTRY_TYPE_PIPE,
				ENTRY_TYPE_SOCKET,
				ENTRY_TYPE_DEVICE_CHAR,
				ENTRY_TYPE_DEVICE_BLOCK
			};

			struct	TFileInfo
			{
				text::string::TString name;
				EEntryType type;
				uoff_t sz_virtual;	//	virtual file size, or "reserved" file size
				uoff_t sz_physical;	//	physical file size, or "actual disk usage"
				system::time::TTime ts_create;
				system::time::TTime ts_change;
				system::time::TTime ts_access;
			};

			static const usys_t MAP_COUNT_FULLFILE = (usys_t)-1;
			static const usys_t INDEX_HEAD = (usys_t)-1;
			static const usys_t INDEX_TAIL = (usys_t)-2;

			/************************************************************************/

			class	CL3PUBT	TMapping : public virtual collection::array::TArray<byte_t>
			{
				friend class TStream;
				private:
					CLASS	TMapping	(const TMapping&);
				protected:
					TFile* file;
					uoff_t index;

				public:
					using collection::array::TArray<byte_t>::Count;

					CL3PUBF	void	Remap		(uoff_t new_index, usys_t new_count);
					CL3PUBF	void	Count		(usys_t) CL3_SETTER;
					CL3PUBF	void	Index		(uoff_t) CL3_SETTER;
					CL3PUBF	uoff_t	Index		() const CL3_GETTER;

					CL3PUBF	CLASS	TMapping	(TFile* file, uoff_t index = 0, usys_t count = (usys_t)-1);
					CL3PUBF	CLASS	TMapping	(TMapping&&);
					CL3PUBF	CLASS	~TMapping	();

					CL3PUBF	static	TMapping	CreateSnapshot	(const TMapping& source);
			};

			/************************************************************************/

			class	CL3PUBT	TStream : public virtual collection::IStaticIterator<byte_t>, public virtual stream::IOut<byte_t>
			{
				protected:
					uoff_t index;
					TMapping map;

				public:
					//	from IIn<byte_t>
					using IIn<byte_t>::Read;
					CL3PUBF	usys_t	Read	(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min);

					//	from IOut<byte_t>
					using IOut<byte_t>::Write;
					CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min);

					//	from IStaticIterator
					CL3PUBF	const byte_t&	Item	() const CL3_GETTER;
					CL3PUBF	byte_t&			Item	() CL3_GETTER;
					CL3PUBF	bool	FindNext	(const collection::IMatcher<byte_t>& matcher);
					CL3PUBF	bool	FindPrev	(const collection::IMatcher<byte_t>& matcher);
					CL3PUBF	bool	IsValid		() const CL3_GETTER;
					CL3PUBF	void	MoveHead	();
					CL3PUBF	void	MoveTail	();
					CL3PUBF	bool	MoveFirst	();
					CL3PUBF	bool	MoveLast	();
					CL3PUBF	bool	MoveNext	();
					CL3PUBF	bool	MovePrev	();

					//	from TStream
					CL3PUBF	void	Index	(uoff_t) CL3_SETTER;
					CL3PUBF	uoff_t	Index	() const CL3_GETTER;

					CL3PUBF	CLASS	TStream	(TFile*);
					CL3PUBF	CLASS	TStream	(const TStream&);
			};

			/************************************************************************/

			class	CL3PUBT	TFile : public virtual collection::IStaticCollection<byte_t>
			{
				friend class TStream;
				friend class TMapping;
				protected:
					fd_t fd;
					int access;
					const event::TEvent<const collection::IStaticCollection<const byte_t>, collection::TOnChangeData<const byte_t> > on_change;

				public:
					//	from IObservable
					inline	const event::TEvent<const collection::IStaticCollection<const byte_t>, collection::TOnChangeData<const byte_t> >&	OnChange	() const CL3_GETTER { return on_change; }

					//	from IStaticCollection
					CL3PUBF	system::memory::TUniquePtr<collection::IStaticIterator<const byte_t> >	CreateStaticIterator	() const CL3_WARN_UNUSED_RESULT;
					CL3PUBF	system::memory::TUniquePtr<collection::IStaticIterator<byte_t> >		CreateStaticIterator	() CL3_WARN_UNUSED_RESULT;
					CL3PUBF	usys_t	Count		() const CL3_GETTER;

					//	from TFile
					CL3PUBF	void	Count	(uoff_t) CL3_SETTER;
					CL3PUBF	text::string::TString
									Name	() const CL3_GETTER;

					CL3PUBF	CLASS	TFile	();	//	create temporary file
					CL3PUBF	CLASS	TFile	(const text::string::TString& name, int access = FILE_ACCESS_READ, ECreate create = FILE_CREATE_NEVER);	//	open specific file
					CL3PUBF	CLASS	TFile	(TFile&&);
					CL3PUBF	CLASS	~TFile	();
			};

			/************************************************************************/

			class   CL3PUBT	TDiectoryBrowser
			{
				private:
					fd_t fd;

				public:
					CL3PUBF	const text::string::TString&
									AbsolutePath		() const;
					CL3PUBF	void	EnterDirectory		(const text::string::TString& name);	//	use ".." to go to the parent directory
					CL3PUBF	void	EnumEntries			(collection::IDynamicCollection<TFileInfo>&) const;
					CL3PUBF	void	EnumEntries			(collection::IDynamicCollection<text::string::TString>&) const;

					CL3PUBF	CLASS	TDiectoryBrowser	();	//	starts in current working directory
					CL3PUBF	CLASS	TDiectoryBrowser	(const text::string::TString& path);
					CL3PUBF	CLASS	TDiectoryBrowser	(const TDiectoryBrowser&);
					CL3PUBF	CLASS	TDiectoryBrowser	(TDiectoryBrowser&&);
					CL3PUBF	CLASS	~TDiectoryBrowser	();
			};
		}
	}
}

#endif
