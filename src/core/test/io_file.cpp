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

#include "common.hpp"
#include <cl3/core/io_file.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::memory;
	using namespace cl3::system::time;
	using namespace cl3::io::file;
	using namespace cl3::io::text::string;
	using namespace cl3::io::collection::list;
	using namespace cl3::error;

	TEST(io_file_TFile, create_always)
	{
		{
			//	create the file once
			TFile file("io_file_TFile_create_always.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"fail\n", 5);
		}

		{
			//	now the file already exists and has to be replaced
			TFile file("io_file_TFile_create_always.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"hello world\n", 12);
		}
	}

	TEST(io_file_TFile, create_never)
	{
		{
			//	create the file
			TFile file("io_file_TFile_create_never.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"fail\n", 5);
		}

		{
			//	now open the file we created
			TFile file("io_file_TFile_create_never.tmp", TAccess::READ | TAccess::WRITE, ECreate::NEVER);
			TStream stream(&file);
			stream.Write((const byte_t*)"hello world\n", 12);
		}

		{
			//	this must fail
			bool ok = false;
			try
			{
				TFile file("io_file_TFile_create_never2.tmp", TAccess::READ | TAccess::WRITE, ECreate::NEVER);
			}
			catch(TException& e)
			{
				ok = true;
			}
			EXPECT_TRUE(ok);
		}
	}

	TEST(io_file_TFile, create_can)
	{
		{
			//	create the file
			TFile file("io_file_TFile_create_can.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"fail\n", 5);
		}

		{
			//	now open the file we created
			TFile file("io_file_TFile_create_can.tmp", TAccess::READ | TAccess::WRITE, ECreate::CAN);
			TStream stream(&file);
			stream.Write((const byte_t*)"hello world\n", 12);
		}

		{
			//	now try to open a file which does not exist yet
			TFile file("io_file_TFile_create_can2.tmp", TAccess::READ | TAccess::WRITE, ECreate::CAN);
			TStream stream(&file);
			stream.Write((const byte_t*)"hello world\n", 12);
		}
	}

	TEST(io_file_TFile, tempfile)
	{
		TFile tmpfile;

		{
			TStream stream(&tmpfile);
			stream.Write((const byte_t*)"hello world\n", 12);
		}

		{
			TStream stream(&tmpfile);
			byte_t buffer[16];
			stream.Read(buffer, 12);
			EXPECT_TRUE(memcmp(buffer, "hello world\n", 12) == 0);
		}
	}

	TEST(io_file_TDirectoryBrowser, EnumEntries)
	{
		TList<TString> files;
		TDirectoryBrowser browser = cl3::unittest_support::TestDataDirectory();
		browser.EnterDirectory("folder_with_mixed_files");

		browser.EnumEntries(files);

		EXPECT_TRUE(files.Count() >= 6);
		EXPECT_TRUE(files.Contains(".posix_hidden_file"));
		EXPECT_TRUE(files.Contains("blns.txt"));
		EXPECT_TRUE(files.Contains("empty_file"));
		EXPECT_TRUE(files.Contains("posix_symbolic_link"));
		EXPECT_TRUE(files.Contains("random_data_128.bin"));
		EXPECT_TRUE(files.Contains("small_text_file.txt"));
		EXPECT_FALSE(files.Contains("."));
		EXPECT_FALSE(files.Contains(".."));

		// run again, to ensure EnumEntries() does not corrupt the FD
		files.Clear();

		browser.EnumEntries(files);

		EXPECT_TRUE(files.Count() >= 6);
		EXPECT_TRUE(files.Contains(".posix_hidden_file"));
		EXPECT_TRUE(files.Contains("blns.txt"));
		EXPECT_TRUE(files.Contains("empty_file"));
		EXPECT_TRUE(files.Contains("posix_symbolic_link"));
		EXPECT_TRUE(files.Contains("random_data_128.bin"));
		EXPECT_TRUE(files.Contains("small_text_file.txt"));
		EXPECT_FALSE(files.Contains("."));
		EXPECT_FALSE(files.Contains(".."));
	}

	TEST(io_file_TDirectoryBrowser, OpenFile)
	{
		TDirectoryBrowser browser = cl3::unittest_support::TestDataDirectory();
		browser.EnterDirectory("folder_with_mixed_files");

		TFile file = browser.OpenFile("random_data_128.bin");
		EXPECT_TRUE(file.Size() == 128);
	}

	TEST(io_file_TDirectoryBrowser, GetFileInfo)
	{
		TDirectoryBrowser browser = cl3::unittest_support::TestDataDirectory();
		browser.EnterDirectory("folder_with_mixed_files");

		const TFileInfo fi_browser = browser.GetFileInfo("random_data_128.bin");
		const TFileInfo fi_file = browser.OpenFile("random_data_128.bin").Info();

		EXPECT_EQ(128U, fi_browser.sz_virtual);
		EXPECT_EQ(EEntryType::FILE, fi_browser.type);
		EXPECT_EQ(1U, fi_browser.n_hardlink);
		EXPECT_TRUE(fi_browser.ts_create < TTime::Now(EClock::REALTIME));
		EXPECT_TRUE(fi_browser.ts_status < TTime::Now(EClock::REALTIME));
		EXPECT_TRUE(fi_browser.ts_write < TTime::Now(EClock::REALTIME));
		EXPECT_EQ(0U, fi_browser.setuid);
		EXPECT_EQ(0U, fi_browser.setgid);
		EXPECT_EQ(0U, fi_browser.sticky);
		EXPECT_EQ(0U, fi_browser.executable);
		EXPECT_EQ(0U, fi_browser.hidden);
		EXPECT_EQ(0U, fi_browser.system);
		EXPECT_EQ(0U, fi_browser.mountpoint);
		EXPECT_EQ(0U, fi_browser.append_only);
		EXPECT_EQ(0U, fi_browser.immutable);
		EXPECT_EQ(0U, fi_browser.mountpoint);
		EXPECT_EQ(0U, fi_browser.topdir);
		EXPECT_EQ(0U, fi_browser.undeleteable);

		EXPECT_EQ(128U, fi_file.sz_virtual);
		EXPECT_EQ(EEntryType::FILE, fi_file.type);
		EXPECT_EQ(1U, fi_file.n_hardlink);
		EXPECT_TRUE(fi_file.ts_create < TTime::Now(EClock::REALTIME));
		EXPECT_TRUE(fi_file.ts_status < TTime::Now(EClock::REALTIME));
		EXPECT_TRUE(fi_file.ts_write < TTime::Now(EClock::REALTIME));
		EXPECT_EQ(0U, fi_file.setuid);
		EXPECT_EQ(0U, fi_file.setgid);
		EXPECT_EQ(0U, fi_file.sticky);
		EXPECT_EQ(0U, fi_file.executable);
		EXPECT_EQ(0U, fi_file.hidden);
		EXPECT_EQ(0U, fi_file.system);
		EXPECT_EQ(0U, fi_file.mountpoint);
		EXPECT_EQ(0U, fi_file.append_only);
		EXPECT_EQ(0U, fi_file.immutable);
		EXPECT_EQ(0U, fi_file.mountpoint);
		EXPECT_EQ(0U, fi_file.topdir);
		EXPECT_EQ(0U, fi_file.undeleteable);
	}

	TEST(io_file_TDirectoryBrowser, DeleteFile)
	{
		const char* const filename = "io_file_TDirectoryBrowser_DeleteFile.tmp";
		TDirectoryBrowser browser;
		browser.OpenFile(filename, TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
		browser.Delete(filename, true, false, false, false);
	}

	TEST(io_file_TDirectoryBrowser, CreateDirectory)
	{
		const char* const filename = "io_file_TDirectoryBrowser_CreateDirectory.tmp";
		TDirectoryBrowser root;
		root.Delete(filename, true, true, true, true);

		{
			root.CreateDirectory(filename, false);
			root.CreateDirectory(filename, true);

			TDirectoryBrowser tmp(root, filename);
			tmp.CreateDirectory("a", false);
			tmp.CreateDirectory("b", false);
			tmp.CreateDirectory("c", false);
			tmp.OpenFile("a/1.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			tmp.OpenFile("a/2.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			tmp.OpenFile("a/3.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			tmp.OpenFile("b/1.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			tmp.OpenFile("b/2.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
			tmp.OpenFile("b/3.tmp", TAccess::READ | TAccess::WRITE, ECreate::ALWAYS);
		}

		root.Delete(filename, true, true, false, true);

		EXPECT_EQ(EEntryType::NOT_EXIST, root.GetFileInfo(filename).type);
	}

	TEST(io_file_TFile, NoCache)
	{
		const usys_t SZ = 512;
		byte_t write_buffer_[SZ + NO_CACHE_MEMORY_ALIGNMENT];
		byte_t read_buffer_ [SZ + NO_CACHE_MEMORY_ALIGNMENT];

		byte_t* write_buffer = AlignMemory(write_buffer_, NO_CACHE_MEMORY_ALIGNMENT);
		byte_t* read_buffer  = AlignMemory(read_buffer_,  NO_CACHE_MEMORY_ALIGNMENT);

		memset(write_buffer, 0xff, SZ);
		memset(read_buffer,  0x00, SZ);

		TFile file("io_file_TFile_no_cache.tmp", TAccess::RW, ECreate::ALWAYS, TDirectoryBrowser(), TIOFlags::NO_CACHE);
		TStream stream(&file);

		stream.Write(write_buffer, SZ);
		stream.Position(0);
		stream.Read(read_buffer, SZ);

		EXPECT_TRUE(memcmp(read_buffer, write_buffer, SZ) == 0);
	}

	TEST(io_file_TFile, SynchronousIO)
	{
		const usys_t SZ = 512;
		byte_t write_buffer[SZ];
		byte_t read_buffer[SZ];

		memset(write_buffer, 0xff, SZ);
		memset(read_buffer,  0x00, SZ);

		TFile file("io_file_TFile_no_cache.tmp", TAccess::RW, ECreate::ALWAYS, TDirectoryBrowser(), TIOFlags::SYNC);
		TStream stream(&file);

		stream.Write(write_buffer, SZ);
		stream.Position(0);
		stream.Read(read_buffer, SZ);

		EXPECT_TRUE(memcmp(read_buffer, write_buffer, SZ) == 0);
	}
	TEST(io_file_TFile, Pos_Read_Write)
	{
		TFile test;

		byte_t buffer[300];
		byte_t reference[sizeof(buffer)];

		memset(reference, 0xAA, sizeof(buffer));
		EXPECT_EQ(test.Read(0, buffer, sizeof(buffer)), 0U);
		EXPECT_EQ(test.Write(0, reference, sizeof(reference)), sizeof(reference));
		EXPECT_EQ(test.Read(0, buffer, sizeof(buffer)), sizeof(reference));
		EXPECT_TRUE(memcmp(buffer, reference, sizeof(buffer)) == 0);

		memset(reference, 0x80, sizeof(buffer));
		EXPECT_EQ(test.Read(3467, buffer, sizeof(buffer)), 0U);
		EXPECT_EQ(test.Write(3467, reference, sizeof(reference)), sizeof(reference));
		EXPECT_EQ(test.Read(3467, buffer, sizeof(buffer)), sizeof(reference));
		EXPECT_TRUE(memcmp(buffer, reference, sizeof(buffer)) == 0);

		memset(reference, 0xAA, sizeof(buffer));
		EXPECT_EQ(test.Read(0, buffer, sizeof(buffer)), sizeof(reference));
		EXPECT_TRUE(memcmp(buffer, reference, sizeof(buffer)) == 0);

		memset(reference, 0, sizeof(buffer));
		EXPECT_EQ(test.Read(2000, buffer, sizeof(buffer)), sizeof(reference));
		EXPECT_TRUE(memcmp(buffer, reference, sizeof(buffer)) == 0);
	}
}
