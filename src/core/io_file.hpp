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

#if (CL3_OS == CL3_OS_POSIX)
	#include <sys/stat.h>
#endif

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
				CL3PUBF static const TAccess RW;

				inline TAccess operator|(TAccess other) const { return { this->read || other.read, this->write || other.write, this->execute || other.execute }; }
				inline TAccess operator&(TAccess other) const { return { this->read && other.read, this->write && other.write, this->execute && other.execute }; }

				inline operator bool() const { return this->read || this->write || this->execute; }
			};

			struct TIOFlags
			{
				bool synchronous_io;
				bool disable_cache;

				CL3PUBF static const TIOFlags NONE;
				CL3PUBF static const TIOFlags SYNC;
				CL3PUBF static const TIOFlags NO_CACHE;

				inline TIOFlags operator|(TIOFlags other) const { return { this->synchronous_io || other.synchronous_io, this->disable_cache || other.disable_cache }; }
				inline TIOFlags operator&(TIOFlags other) const { return { this->synchronous_io && other.synchronous_io, this->disable_cache && other.disable_cache }; }
			};

			enum class ECreate
			{
				NEVER,	//	never create the file - the file must exist or the call fails
				ALWAYS,	//	always create a new file - destroy/truncate existing file, fails if it can not create a new file
				CAN		//	only create if the file does not yet exist
			};

			enum class EEntryType : u8_t
			{
				UNKNOWN,
				NOT_EXIST,	// all fields in TFileInfo will have undefined value
				FILE,
				DIRECTORY,
				PIPE,
				SOCKET,
				DEVICE_CHAR,
				DEVICE_BLOCK,
				SYMBOLIC_LINK
			};

			struct	TFileInfo
			{
				EEntryType type;

				u16_t n_hardlink;		// posix: number of hardlinks to this inode / windows: not mapped (yet)

				u32_t
					setuid : 1,		// posix: setuid permission bit / windows: not mapped (yet)
					setgid : 1,		// posix: setgid permission bit / windows: not mapped (yet)
					sticky : 1,		// posix: sticky flag / windows: not mapped (yet)
					executable : 1,	// posix: any executable bit set / windows: file-extension is ".exe"
					archive : 1,	// posix: inverse FS_NODUMP_FL / windows: archive flag
					encrypted : 1,	// posix: not mapped (yet) / windows: encrypted flag
					hidden : 1,		// posix: filename starts with a dot / windows: hidden flag
					offline : 1,	// posix: currently not mounted auto-mounted directory / windows: offline flag
					system : 1,		// posix: not mapped (yet) / windows: system flag
					mountpoint : 1,	// posix: directory is a mount point / windows: directory is a mount point NOTE: only valid when using TDirectoryBrowser::GetFileInfo()

					append_only : 1,		// linux: FS_APPEND_FL / windows: not mapped (yet)
					compressed : 1,			// linux: FS_COMPR_FL / windows: compressed flag
					dirsync : 1,			// linux: FS_DIRSYNC_FL / windows: not mapped (yet)
 					immutable : 1,			// linux: FS_IMMUTABLE_FL / windows: readonly flag
					journaled : 1,			// linux: FS_JOURNAL_DATA_FL / windows: not mapped (yet)
					no_atime : 1,			// linux: FS_NOATIME_FL / windows: not mapped (yet)
					no_cow : 1,				// linux: FS_NOCOW_FL / windows: not mapped (yet)
					no_tail_merge : 1,		// linux: FS_NOTAIL_FL / windows: not mapped (yet)
					secure_delete : 1,		// linux: FS_SECRM_FL / windows: not mapped (yet)
					sync : 1,				// linux: FS_SYNC_FL / windows: not mapped (yet)
					topdir : 1,				// linux: FS_TOPDIR_FL / windows: not mapped (yet)
					undeleteable : 1;		// linux: FS_UNRM_FL / windows: not mapped (yet)

				uoff_t sz_virtual;	//	virtual file size, or "apparent" file size
				uoff_t sz_physical;	//	physical file size, or "actual disk usage"
				system::time::TTime ts_create;	// => TTime(0,0), ftCreationTime
				system::time::TTime ts_status;	// => ctime,
				system::time::TTime ts_access;	// => atime, ftLastAccessTime
				system::time::TTime ts_write;	// => mtime, ftLastWriteTime
				text::string::TString link_target;	// only valid if type is SYMBOLIC_LINK
			};

			static const usys_t MAP_COUNT_FULLFILE = (usys_t)-1;
			static const usys_t INDEX_HEAD = (usys_t)-1;
			static const usys_t INDEX_TAIL = (usys_t)-2;

			CL3PUBV const usys_t NO_CACHE_MEMORY_ALIGNMENT;

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

					#if (CL3_OS == CL3_OS_POSIX)
						struct stat st_dir;
					#endif

				public:
					CL3PUBF	fd_t	Handle				() const CL3_GETTER;
					CL3PUBF	text::string::TString
									AbsolutePath		() const;
					CL3PUBF	void	EnterDirectory		(const text::string::TString& name);	//	use ".." to go to the parent directory
					CL3PUBF	usys_t	EnumEntries			(collection::IDynamicCollection<text::string::TString>&) const;
					CL3PUBF	usys_t	EnumEntries			(util::function::TFunction<bool, const TDirectoryBrowser&, const text::string::TString&>) const;
					CL3PUBF	bool	IsRoot				() const CL3_GETTER;
					CL3PUBF collection::list::TList<text::string::TString> Entries() const CL3_GETTER;
					CL3PUBF	TFile	OpenFile			(const text::string::TString& name, TAccess access = TAccess::READ, ECreate create = ECreate::NEVER, TIOFlags flags = TIOFlags::NONE);
					CL3PUBF	TFileInfo GetFileInfo		(const text::string::TString& name) const CL3_GETTER;

					CL3PUBF	void	Delete				(const text::string::TString& name, const bool file_ok = true, const bool dir_ok = true, const bool ignore_not_exist = true, const bool recursive = false) const;

					CL3PUBF	void	CreateDirectory		(const text::string::TString& name, const bool ignore_existing = false);

					CL3PUBF	CLASS	TDirectoryBrowser	();	//	starts in current working directory
					CL3PUBF	explicit TDirectoryBrowser	(const text::string::TString& path);
					CL3PUBF	CLASS	TDirectoryBrowser	(const TDirectoryBrowser&, const text::string::TString& path);
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
					TAccess access;

				public:
					inline const stream::fd::TFDStream& FD() const { return this->fd; }
					inline stream::fd::TFDStream& FD() { return this->fd; }

					CL3PUBF TFileInfo Info	() const CL3_GETTER;

					CL3PUBF usys_t	Read	(u64_t pos, byte_t* arr_items_read, usys_t n_items_read_max);
					CL3PUBF usys_t	Write	(u64_t pos, const byte_t* arr_items_write, usys_t n_items_write_max);

					CL3PUBF	usys_t	Size	() const CL3_GETTER;
					CL3PUBF	void	Size	(uoff_t) CL3_SETTER;

					CL3PUBF	CLASS	TFile	();	//	create temporary file
					CL3PUBF	CLASS	TFile	(const text::string::TString& name, TAccess access = TAccess::READ, ECreate create = ECreate::NEVER, const TDirectoryBrowser& directory = TDirectoryBrowser(), TIOFlags flags = TIOFlags::NONE);	//	open specific file
					CL3PUBF	CLASS	TFile	(TFile&&);
					CL3PUBF	CLASS	~TFile	();
			};
		}
	}
}

#endif
