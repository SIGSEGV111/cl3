#include <cl3/core.cpp>

// small test to check if source bundle compiles and is generally working for applications

using namespace cl3::system::types;
using namespace cl3::io::collection::list;
using namespace cl3::io::text::string;

int main()
{
	TList<int> list;

	list.Add(7);

	TString str = "hello world";
	str.Reverse();

	if(list.Count() == 1 && list[0] == 7 && str == "dlrow olleh")
		return 0;
	else
		return 1;
}
