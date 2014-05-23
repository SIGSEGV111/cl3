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
#include "io_buffer.hpp"
#include "io_text_string.hpp"
#include "io_collection_list.hpp"
#include "system_time.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	file
		{
			class	CL3PUBT	TStream;
			class	CL3PUBT	TFile;

			enum	EAccess
			{
				FILE_ACCESS_READ,
				FILE_ACCESS_WRITE,
				FILE_ACCESS_EXECUTE
			};

			enum	ECreate
			{
				FILE_CREATE_NEVER,	//	never create the file - the file must exist or the call fails
				FILE_CREATE_ALWAYS,	//	always create a new file - destroy/truncate existing file, fails if it can not create a new file
				FILE_CREATE_CAN		//	only create if the file does not yet exist
			};

			class	TFileStream : public virtual buffer::IBufferedStream<byte_t>
			{
				protected:
					TFile* file;
					usys_t index;

				public:
					//	from IBufferedStream<byte_t>
					CL3PUBF	void	Index	(uoff_t) CL3_SETTER;
					CL3PUBF	uoff_t	Index	() const CL3_GETTER;

					//	from IIn<byte_t>
					CL3PUBF	usys_t	Read	(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min = (usys_t)-1);
					CL3PUBF	uoff_t	WriteOut(IOut<byte_t>& os, uoff_t n_items_wo_max, uoff_t n_items_wo_min = (uoff_t)-1);

					//	from IOut<byte_t>
					CL3PUBF	usys_t	Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min = (usys_t)-1);
					CL3PUBF	uoff_t	ReadIn	(IIn<byte_t>& is, uoff_t n_items_ri_max, uoff_t n_items_ri_min = (uoff_t)-1);

					CL3PUBF	CLASS	TFileStream	(TFile*);
					CL3PUBF	CLASS	TFileStream	(const TStream&);
					CL3PUBF	CLASS	~TFileStream();
			};

			class	TFile : public virtual buffer::IBuffer<byte_t>
			{
				protected:
					fd_t fd;
					byte_t* p_data;
					usys_t sz_data;

				public:
					//	from IBuffer<byte_t>
					CL3PUBF	void	Read	(uoff_t index, byte_t* arr_items_read, usys_t n_items_read);
					CL3PUBF	void	WriteOut(uoff_t index, stream::IOut<byte_t>& os, usys_t n_items_wo);
					CL3PUBF	void	Write	(uoff_t index, const byte_t* arr_items_write, usys_t n_items_write);
					CL3PUBF	void	ReadIn	(uoff_t index, stream::IIn<byte_t>& is, usys_t n_items_ri);
					CL3PUBF	system::memory::TUniquePtr<buffer::IBufferedStream<byte_t> >	CreateStream	() CL3_WARN_UNUSED_RESULT;

					CL3PUBF	CLASS	TFile	();
					CL3PUBF	CLASS	TFile	(const text::string::TString& name, int access = FILE_ACCESS_READ, ECreate create = FILE_CREATE_NEVER);
					CL3PUBF	CLASS	TFile	(const TFile&);
					CL3PUBF	CLASS	~TFile	();
			};

			enum	EEntryType
			{
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
				uoff_t sz_virtual;
				uoff_t sz_physical;
				system::time::TTime ts_create;
				system::time::TTime ts_change;
				system::time::TTime ts_access;
			};

			class   CL3PUBT	TDiectoryBrowser
			{
				private:
					fd_t fd;

				public:
					CL3PUBF	const text::string::TString&
									AbsolutePath		() const;
					CL3PUBF	void	EnterSubDirectory	(const text::string::TString& name);	//	use ".." to go to the parent directory
					CL3PUBF	void	EnumEntries			(collection::list::TList<TFileInfo>&) const;
					CL3PUBF	void	EnumEntries			(collection::list::TList<text::string::TString>&) const;

					CL3PUBF	CLASS	TDiectoryBrowser	();	//	starts in current working directory
					CL3PUBF	CLASS	TDiectoryBrowser	(const text::string::TString& path);
					CL3PUBF	CLASS	TDiectoryBrowser	(const TDiectoryBrowser&);
					CL3PUBF	CLASS	~TDiectoryBrowser	();
			};
		}
	}
}

#endif
