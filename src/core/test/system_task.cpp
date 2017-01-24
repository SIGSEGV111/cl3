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
#include <cl3/core/system_task_async.hpp>
#include <gtest/gtest.h>
#include <stdlib.h>
#include "common.hpp"
//#include <valgrind/valgrind.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::task;
	using namespace cl3::system::memory;
	using namespace cl3::system::time;
	using namespace cl3::system::task::async;
	using namespace cl3::io::text;
	using namespace cl3::io::text::encoding;
	using namespace cl3::io::text::string;
	using namespace cl3::io::collection::list;
	using namespace cl3::io::collection::array;
	using namespace cl3::io::collection::map;
	using namespace cl3::io::collection;
	using namespace cl3::error;

	TEST(system_task_CreateProcess, Invocation)
	{
		//	will not output anything, but will test if the call to CreateProcess works without error

		TList<TString> args;
		args.Append("-n");

		TChildProcess cp("/bin/echo", args);
	}

	TEST(system_task_CreateProcess, InjectEnv)
	{
		TList<TString> args;
		args.Append("-c");
		args.Append("export -p | sort");
		TStdMap<TString,TString> env = TLocalProcess::Self()->Environment();

		//	add a new environment variables named "foo" with the value "bar"
		env["foo"] = "bar";

		TString output;
		utf8::TUTF8Decoder decoder;
		decoder.Sink(&output);

		TChildProcess cp("/bin/bash", args, env, NULL, &decoder);

		//	FIXME: TAsyncEventProcessor should not be used like this
		//	WARNING: below code is plain out wrong and WIP
		while(TAsyncEventProcessor::Default().CountCallbacks() > 0)
			TAsyncEventProcessor::Default().ProcessEvents();

		//	see if "export -p" printed out the injected environment variable
		EXPECT_TRUE(output.Contains("declare -x foo=\"bar\"\n"));
	}

	TEST(system_task_CreateProcess, Pipe)
	{
		TList<TString> args;

		TList<byte_t> input;
		TList<byte_t> output;

		input.Append((const byte_t*)"a\nz\ne\nx\n", 8);

		TChildProcess cp("/bin/sort", args, TLocalProcess::Self()->Environment(), &input, &output);

		//	FIXME: TAsyncEventProcessor should not be used like this
		//	WARNING: below code is plain out wrong and WIP
		while(TAsyncEventProcessor::Default().CountCallbacks() > 0)
			TAsyncEventProcessor::Default().ProcessEvents();

		EXPECT_TRUE(output.Count() == 8 && memcmp(output.ItemPtr(0), "a\ne\nx\nz\n", 8) == 0);
	}

	TEST(system_task_TLocalProcess, Arguments)
	{
		//	test if the commandline arguments to the process running these tests are dummy1 dummy2 dummy3
		auto& args = TLocalProcess::Self()->Arguments();
		EXPECT_EQ(TString("dummy1"), args[1]);
		EXPECT_EQ(TString("dummy2"), args[2]);
		EXPECT_EQ(TString("dummy3"), args[3]);
	}

	TEST(system_task_TLocalProcess, Environment)
	{
		auto& env = TLocalProcess::Self()->Environment();

		EXPECT_TRUE(env.Count() > 2);
// 		EXPECT_EQ(env["PWD"], cl3::io::file::TDirectoryBrowser::ProcessCurrentWorkingDirectory().AbsolutePath());	//	won't work when a path component is symlinked
	}
}
