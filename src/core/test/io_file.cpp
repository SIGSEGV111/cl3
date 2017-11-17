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

#include <cl3/core/io_file.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::io::file;
	using namespace cl3::io::text::string;
	using namespace cl3::io::collection::list;
	using namespace cl3::error;

	TEST(io_file_TFile, create_always)
	{
		{
			//	create the file once
			TFile file("io_file_TFile_create_always.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"fail\n", 5);
		}

		{
			//	now the file already exists and has to be replaced
			TFile file("io_file_TFile_create_always.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"hello world\n", 12);
		}
	}

	TEST(io_file_TFile, create_never)
	{
		{
			//	create the file
			TFile file("io_file_TFile_create_never.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"fail\n", 5);
		}

		{
			//	now open the file we created
			TFile file("io_file_TFile_create_never.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_NEVER);
			TStream stream(&file);
			stream.Write((const byte_t*)"hello world\n", 12);
		}

		{
			//	this must fail
			bool ok = false;
			try
			{
				TFile file("io_file_TFile_create_never2.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_NEVER);
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
			TFile file("io_file_TFile_create_can.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_ALWAYS);
			TStream stream(&file);
			stream.Write((const byte_t*)"fail\n", 5);
		}

		{
			//	now open the file we created
			TFile file("io_file_TFile_create_can.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_CAN);
			TStream stream(&file);
			stream.Write((const byte_t*)"hello world\n", 12);
		}

		{
			//	now try to open a file which does not exist yet
			TFile file("io_file_TFile_create_can2.tmp", FILE_ACCESS_READ | FILE_ACCESS_WRITE, FILE_CREATE_CAN);
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

	static TDirectoryBrowser TestDataDirectory()
	{
		TDirectoryBrowser b;

		while(!b.IsRoot() && !b.Entries().Contains(".gitignore"))
			b.EnterDirectory("..");

		b.EnterDirectory("data/tests");

		return b;
	}

	TEST(io_file_TDirectoryBrowser, EnumEntries)
	{
		TList<TString> files;
		TDirectoryBrowser browser = TestDataDirectory();
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
		TDirectoryBrowser browser = TestDataDirectory();
		browser.EnterDirectory("folder_with_mixed_files");

		TFile file = browser.OpenFile("random_data_128.bin");
		EXPECT_TRUE(file.Size() == 128);
	}
}
