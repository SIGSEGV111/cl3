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
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::io::file;
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
}