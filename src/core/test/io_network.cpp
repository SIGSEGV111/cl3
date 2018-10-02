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
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/error.hpp>
#include <gtest/gtest.h>

using namespace ::testing;

namespace
{
	using namespace cl3::io::network;
	using namespace cl3::system::types;
	using namespace cl3::io::text::string;

// 	TEST(io_network, TCP_Loopback_v4)
// 	{
// 		TTCPServer server(TIPv4(127,0,0,1));
// 		TTCPClient client1(TIPv4(127,0,0,1), server.Port());
//
// 		server.OnClientConnecting().WaitFor();
// 		TTCPClient client2 = server.Accept();
//
// 		const byte_t test_data[] = { 1,2,3,4,5,6,7,8,9 };
// 		client1.Write(test_data, sizeof(test_data));
//
// 		byte_t buffer[sizeof(test_data)] = {};
// 		client2.Read(buffer, sizeof(test_data));
//
// 		EXPECT_TRUE(memcmp(test_data, buffer, sizeof(test_data)) == 0);
// 	}
//
// 	TEST(io_network, TCP_Loopback_v6)
// 	{
// 		TTCPServer server(TIPv6("0:0:0:0:0:0:0:1"));
// 		TTCPClient client1(TIPv6("0:0:0:0:0:0:0:1"), server.Port());
//
// 		server.OnClientConnecting().WaitFor();
// 		TTCPClient client2 = server.Accept();
//
// 		const byte_t test_data[] = { 1,2,3,4,5,6,7,8,9 };
// 		client1.Write(test_data, sizeof(test_data));
//
// 		byte_t buffer[sizeof(test_data)] = {};
// 		client2.Read(buffer, sizeof(test_data));
//
// 		EXPECT_TRUE(memcmp(test_data, buffer, sizeof(test_data)) == 0);
// 	}

	TEST(io_network_TIPv4, MaskToCIDR)
	{
		EXPECT_EQ(0, TIPv4::MaskToCIDR(TIPv4("0.0.0.0")));
		EXPECT_EQ(8, TIPv4::MaskToCIDR(TIPv4("255.0.0.0")));
		EXPECT_EQ(16, TIPv4::MaskToCIDR(TIPv4("255.255.0.0")));
		EXPECT_EQ(24, TIPv4::MaskToCIDR(TIPv4("255.255.255.0")));
		EXPECT_EQ(32, TIPv4::MaskToCIDR(TIPv4("255.255.255.255")));
		EXPECT_EQ(20, TIPv4::MaskToCIDR(TIPv4("255.255.240.0")));
	}

	TEST(io_network_TIPv4, CIDRToMask)
	{
		EXPECT_EQ(TIPv4::CIDRToMask(0), TIPv4("0.0.0.0"));
		EXPECT_EQ(TIPv4::CIDRToMask(8), TIPv4("255.0.0.0"));
		EXPECT_EQ(TIPv4::CIDRToMask(16), TIPv4("255.255.0.0"));
		EXPECT_EQ(TIPv4::CIDRToMask(24), TIPv4("255.255.255.0"));
		EXPECT_EQ(TIPv4::CIDRToMask(31), TIPv4("255.255.255.254"));
		EXPECT_EQ(TIPv4::CIDRToMask(32), TIPv4("255.255.255.255"));
		EXPECT_EQ(TIPv4::CIDRToMask(20), TIPv4("255.255.240.0"));
	}

	TEST(io_network_TIPv4, ToString)
	{
		const TString src = "192.168.168.17";
		const TIPv4 ip(src);
		TString dst;
		dst<<ip;
		EXPECT_EQ(src, dst);
	}
}
