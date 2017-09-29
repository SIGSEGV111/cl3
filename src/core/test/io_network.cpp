/*
 *   libcl3 - common library version 3
 *   Copyright (C) 2013	Simon Brennecke
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cl3/core/io_network.hpp>
#include <cl3/core/error.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::io::network;
	using namespace cl3::system::types;

	TEST(io_network, TCP_Loopback)
	{
		TTCPServer server(TIPv4(127,0,0,1));
		TTCPClient client1(TIPv4(127,0,0,1), server.Port());

		server.OnClientConnecting().WaitFor();
		TTCPClient client2 = server.Accept();

		const byte_t test_data[] = { 1,2,3,4,5,6,7,8,9 };
		client1.Write(test_data, sizeof(test_data));

		byte_t buffer[sizeof(test_data)] = {};
		client2.Read(buffer, sizeof(test_data));

		EXPECT_TRUE(memcmp(test_data, buffer, sizeof(test_data)) == 0);
	}
}
