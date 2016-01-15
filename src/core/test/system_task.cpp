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
#include "common.hpp"
//#include <valgrind/valgrind.h>

using namespace ::testing;

namespace
{
	using namespace cl3::system::types;
	using namespace cl3::system::task;
	using namespace cl3::system::task::synchronization;
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

}
