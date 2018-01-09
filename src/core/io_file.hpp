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
			class   CL3PUBT	TDirectoryBrowser;

			/************************************************************************/

			struct TAccess
			{
				bool read;
				bool write;
				bool execute;

				CL3PUBF static const TAccess READ;
				CL3PUBF static const TAccess WRITE;
				CL3PUBF static const TAccess EXECUTE;

				inline TAccess operator|(TAccess other) const { return { this->read || other.read, this->write || other.write, this->execute || other.execute }; }
				inline TAccess operator&(TAccess other) const { return { this->read && other.read, this->write && other.write, this->execute && other.execute }; }

				inline operator bool() const { return this->read || this->write || this->execute; }
			};

			enum class ECreate
			{
				NEVER,	//	never create the file - the file must exist or the call fails
				ALWAYS,	//	always create a new file - destroy/truncate existing file, fails if it can not create a new file
				CAN		//	only create if the file does not yet exist
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
				uoff_t sz_virtual;	//	virtual file size, or "apparent" file size
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
			};

			/************************************************************************/

			class	CL3PUBT	TStream : public stream::IOut<byte_t>, public stream::IIn<byte_t>
			{
				protected:
					TFile* file;
					uoff_t pos;

				public:
					//	from IIn<byte_t>
					using IIn<byte_t>::Read;
					CL3PUBF	usys_t	Read	(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min) final override CL3_WARN_UNUSED_RESULT ;

					//	from IOut<byte_t>
					using IOut<byte_t>::Write;
					CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min) final override CL3_WARN_UNUSED_RESULT ;

					//	from TStream
					CL3PUBF	void	Position(uoff_t new_pos) CL3_SETTER { this->pos = new_pos; }
					CL3PUBF	uoff_t	Position() const CL3_GETTER { return this->pos; }

					CL3PUBF	CLASS	TStream	(TFile*);
					CL3PUBF	CLASS	TStream	(const TStream&);
			};

			/************************************************************************/

			class	CL3PUBT	TFileSystem
			{
				virtual	text::string::TString	Type		() const CL3_GETTER = 0;
				virtual	text::string::TString	Device		() const CL3_GETTER = 0;

				virtual	void	EnumMountPoints		(collection::IDynamicCollection<const text::string::TString>&) const = 0;
			};

			struct	CL3PUBT	IFileSystemObject
			{
				virtual	TFileSystem*			FileSystem	() const CL3_GETTER = 0;
				virtual	text::string::TString	URI			() const CL3_GETTER = 0;
				virtual	text::string::TString	Name		() const CL3_GETTER = 0;
				virtual	void					Name		(const text::string::TString&) CL3_SETTER = 0;
			};

			/************************************************************************/

			class   CL3PUBT	TDirectoryBrowser
			{
				private:
					stream::fd::TFDStream fd;

				public:
					CL3PUBF	fd_t	Handle				() const CL3_GETTER;
					CL3PUBF	text::string::TString
									AbsolutePath		() const;
					CL3PUBF	void	EnterDirectory		(const text::string::TString& name);	//	use ".." to go to the parent directory
					CL3PUBF	usys_t	EnumEntries			(collection::IDynamicCollection<text::string::TString>&) const;
					CL3PUBF	bool	IsRoot				() const CL3_GETTER;
					CL3PUBF collection::list::TList<text::string::TString> Entries() const CL3_GETTER;
					CL3PUBF TFile	OpenFile			(const text::string::TString& name, TAccess access = TAccess::READ, ECreate create = ECreate::NEVER);

					CL3PUBF	CLASS	TDirectoryBrowser	();	//	starts in current working directory
					CL3PUBF	explicit TDirectoryBrowser	(const text::string::TString& path);
					CL3PUBF	CLASS	TDirectoryBrowser	(const TDirectoryBrowser&);
					CL3PUBF	CLASS	TDirectoryBrowser	(TDirectoryBrowser&&);
					CL3PUBF	CLASS	~TDirectoryBrowser	();
			};

			/************************************************************************/

			class	CL3PUBT	TFile
			{
				friend class TStream;
				friend class TMapping;
				protected:
					stream::fd::TFDStream fd;
					int access;

				public:
					CL3PUBF	usys_t	Size	() const CL3_GETTER;
					CL3PUBF	void	Size	(uoff_t) CL3_SETTER;

					CL3PUBF	CLASS	TFile	();	//	create temporary file
					CL3PUBF	CLASS	TFile	(const text::string::TString& name, TAccess access = TAccess::READ, ECreate create = ECreate::NEVER, const TDirectoryBrowser& directory = TDirectoryBrowser());	//	open specific file
					CL3PUBF	CLASS	TFile	(TFile&&);
					CL3PUBF	CLASS	~TFile	();
			};
		}
	}
}

#endif
