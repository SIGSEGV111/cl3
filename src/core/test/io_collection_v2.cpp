#include "common.hpp"
#include <gtest/gtest.h>
#include <cl3/core/io_text_terminal.hpp>
#include <cl3/core/io_collection.hpp>
#include <cl3/core/io_collection_list_v2.hpp>
#include <cl3/core/io_text_string.hpp>

using namespace ::testing;

using namespace cl3::io::text::terminal;

namespace
{
	using namespace cl3::io::collection::list_v2;
	using namespace cl3::system::def;

	struct TSimple
	{
	};

	struct TNoMove
	{
		TNoMove& operator=(TNoMove&&) = delete;
		TNoMove& operator=(const TNoMove&) = default;

		CLASS TNoMove(TNoMove&&) = delete;
		CLASS TNoMove(const TNoMove&) = default;
		CLASS TNoMove() {}
	};

	struct TNoCopy
	{
		TNoCopy& operator=(TNoCopy&&) = default;
		TNoCopy& operator=(const TNoCopy&) = delete;

		CLASS TNoCopy(TNoCopy&&) = default;
		CLASS TNoCopy(const TNoCopy&) = delete;
		CLASS TNoCopy() {}
	};

	struct TNoDefaultConstruct
	{
		CLASS TNoDefaultConstruct(const char*, int, long, bool) {}
	};

	TEST(io_collection_list_v2_TList2, EvilStructs)
	{
		TList2<TSimple> ls_simple;
		TList2<TNoMove> ls_no_move;
		TList2<TNoCopy> ls_no_copy;
		TList2<TNoDefaultConstruct> ls_no_default_construct;

		TSimple a;
		ls_simple.Append(a);
		ls_simple.Append(TSimple());
		ls_simple.Append(move(TSimple()));

		TNoMove no_move;
		ls_no_move.Append(no_move);

		ls_no_copy.Append(move(TNoCopy()));

		TNoDefaultConstruct b("",0,0L,true);
		ls_no_default_construct.Append(b);
		ls_no_default_construct.Append(TNoDefaultConstruct("",0,0L,true));
	}
}
