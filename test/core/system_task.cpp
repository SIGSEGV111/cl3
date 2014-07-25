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

#include <cl3/core/system_compiler.hpp>
#include <cl3/core/system_types.hpp>
#include <cl3/core/system_types_typeinfo.hpp>
#include <cl3/core/system_memory.hpp>
#include <cl3/core/io_collection_list.hpp>
#include <cl3/core/io_text.hpp>
#include <cl3/core/io_file.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/io_text_encoding_utf8.hpp>
#include <cl3/core/system_task.hpp>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <valgrind/valgrind.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::task;
	using namespace cl3::system::memory;
	using namespace cl3::system::time;
	using namespace cl3::io::text;
	using namespace cl3::io::file;
	using namespace cl3::io::text::string;
	using namespace cl3::io::text::encoding;
	using namespace cl3::io::text::encoding::utf8;
	using namespace cl3::io::collection::list;
	using namespace cl3::io::collection;
	using namespace cl3::error;

	TEST(system_task_TProcessRunner, cat)
	{
		byte_t buffer[32];

		{
			TProcessRunner runner("cat");

			runner.Start();
			runner.Write((const byte_t*)"Hello World\n", 12);
			runner.CloseTX();
			runner.Read(buffer, 12);
			runner.CloseRX();
		}

		EXPECT_TRUE(memcmp(buffer, "Hello World\n", 12) == 0);
	}

	TEST(system_task_TProcessRunner, gzip)
	{
		byte_t buffer[32];
		TList<TString> args;
		args.Append("--fast");

		{
			TProcessRunner runner("gzip");

			runner.Start(args);
			runner.Write((const byte_t*)"Hello World\n", 12);
			runner.CloseTX();
			runner.Read(buffer, 32);
			runner.CloseRX();
		}

		{
			args.Append("--decompress");
			TProcessRunner runner("gzip");

			runner.Start(args);
			runner.Write(buffer, 32);
			runner.CloseTX();
			runner.Read(buffer, 12);
			runner.CloseRX();
		}

		EXPECT_TRUE(memcmp(buffer, "Hello World\n", 12) == 0);
	}

	TEST(system_task_TProcessRunner, sleep)
	{
		TList<TString> args;
		args.Append("0.5");
		TProcessRunner runner("sleep");

		const TTime t_start = TTime::Now();
		runner.Start(args);
		runner.Wait();
		const TTime t_end = TTime::Now();
		const TTime t_delta = t_end - t_start;
		EXPECT_TRUE(t_delta.ConvertToI(TIME_UNIT_MILLISECONDS) >= 500);
	}

	TEST(system_task_TProcessRunner, kill)
	{
		TList<TString> args;
		args.Append("0.5");
		TProcessRunner runner("sleep");

		const TTime t_start = TTime::Now();
		runner.Start(args);
		runner.Kill();
		runner.Wait();
		const TTime t_end = TTime::Now();
		const TTime t_delta = t_end - t_start;
		EXPECT_TRUE(t_delta.ConvertToI(TIME_UNIT_MILLISECONDS) <= 250);
	}

	TEST(system_task_TProcess, Name)
	{
		const TString name_original = TProcess::Self()->Name();
		const TString name_want = "gtest";

		EXPECT_TRUE(name_original.Find("/exe") != (usys_t)-1);
		EXPECT_TRUE(system(TCString(TString("pgrep -f ") + name_original + " >/dev/null", CODEC_CXX_CHAR).Chars()) == 0);

		TProcess::Self()->Name(name_want);
		EXPECT_TRUE(TProcess::Self()->Name() == name_want);

		if(RUNNING_ON_VALGRIND == 0)
			EXPECT_TRUE(system("pidof gtest >/dev/null") == 0);

		TProcess::Self()->Name(name_original);
		EXPECT_TRUE(system(TCString(TString("pgrep -f ") + name_original + " >/dev/null", CODEC_CXX_CHAR).Chars()) == 0);
	}

	TEST(system_task_TProcess, Environment)
	{
		const IStaticCollection<const TString>& ls = TProcess::Self()->Environment();
		auto it = ls.CreateStaticIterator();

		const TString* cmd = NULL;
		const TString* pwd = NULL;
		const TString* user = NULL;

		for(it->MoveFirst(); it->IsValid(); it->MoveNext())
		{
			if(it->Item().Left(2) == "_=")
				cmd = &it->Item();
			if(it->Item().Left(4) == "PWD=")
				pwd = &it->Item();
			if(it->Item().Left(5) == "USER=")
				user = &it->Item();
		}

		EXPECT_TRUE(cmd != NULL);
		EXPECT_TRUE(pwd != NULL);
		EXPECT_TRUE(user != NULL);
	}

	TEST(system_task_TProcess, CommandlineArguments)
	{
		const IStaticCollection<const TString>& ls = TProcess::Self()->CommandlineArguments();
		auto it = ls.CreateStaticIterator();

		EXPECT_TRUE(it->MoveFirst());
		EXPECT_TRUE(it->Item() == "./exe");
		EXPECT_TRUE(it->MoveNext());
		EXPECT_TRUE(it->Item() == "dummy1");
		EXPECT_TRUE(it->MoveNext());
		EXPECT_TRUE(it->Item() == "dummy2");
		EXPECT_TRUE(it->MoveNext());
		EXPECT_TRUE(it->Item() == "dummy3");
		EXPECT_TRUE(!it->MoveNext());
	}
}
