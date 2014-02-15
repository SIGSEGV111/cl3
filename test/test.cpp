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

#include <stdio.h>
#include <cl3/core/system_types_typeinfo.h>
#include <cl3/core/system_types.h>
#include <cl3/core/event.h>
#include <cl3/core/io_collection.h>

using namespace std;
using namespace cl3::event;

namespace	test
{
	template<class T>
	struct B {};

	B<char> b;

	template<class X, B<char>* b>
	struct A
	{
	};

	int array[] = { 10, 0 };

	template<class T, class A, int j>
	struct	TTest
	{
		int x,y,z;

		TTest() {}

		TEvent<TTest, int> on_x_change;
	};

	TTest<int,int,10> object;

	void static_func(TEvent<TTest<int,int,10>,int>& event, TTest<int,int,10>& sender, int data, char& rdata)
	{
		printf("hey! (%c)\n", rdata);
	}
}

using namespace test;

int main(int argc, char* argv[])
{
	try
	{
		object.on_x_change.Register(&static_func, 'a');
		object.on_x_change.Register(&static_func, 'b');
		printf("%zd receivers\n", object.on_x_change.Raise(object, 10));
		object.on_x_change.Unregister(&static_func, 'b');
		printf("%zd receivers\n", object.on_x_change.Raise(object, 10));

		int* x = new int(10);
		*x = 17;
		delete x;
		const cl3::system::types::typeinfo::TRTTI& rtti = cl3::system::types::typeinfo::TCTTI< TTest< A<B<int>,&b>, int[], 1045678> >::rtti;
		printf("%s\n%08x\n", rtti.Name().Array(), rtti.Hash());
		return 0;
	}
	catch(const cl3::error::TCoreException& e)
	{
		return 1;
	}
	catch(...)
	{
		return 2;
	}
}
