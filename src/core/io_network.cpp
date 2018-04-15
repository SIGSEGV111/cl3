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
#include <arpa/inet.h>

#include <cl3/core/io_network.hpp>
#include <cl3/core/error.hpp>
#include <cl3/core/io_text_string.hpp>
#include <cl3/core/io_text_encoding.hpp>
#include <cl3/core/system_endian.hpp>

namespace cl3
{
	namespace io
	{
		namespace network
		{
			using namespace text::string;
			using namespace error;
			using namespace system::endian;

			TIPv4::TIPv4()
			{
				this->u32 = 0;
			}

			TIPv4::TIPv4(u8_t a, u8_t b, u8_t c, u8_t d)
			{
				this->octet[0] = a;
				this->octet[1] = b;
				this->octet[2] = c;
				this->octet[3] = d;
			}

			TIPv4::TIPv4(const char* ipstr)
			{
				CL3_CLASS_ERROR(::sscanf(ipstr, "%hhu.%hhu.%hhu.%hhu", this->octet + 0, this->octet + 1, this->octet + 2, this->octet + 3) != 4, TException, "unable to parse IPv4 address from string");
			}

			TIPv4::TIPv4(const text::string::TString& ipstr) : TIPv4(TCString(ipstr, text::encoding::CODEC_CXX_CHAR).Chars())
			{
				// TODO: switch to use parser
			}

			TIPv4::TIPv4(u32_t v)
			{
				this->u32 = v;
			}

			u8_t TIPv4::MaskToCIDR(const TIPv4 mask)
			{
				return system::compiler::CountLeadingZeroes(~ConvertBigEndianToNative(mask.u32));
			}

			TIPv4 TIPv4::CIDRToMask(const u8_t cidr)
			{
				TIPv4 mask;
				mask.u32 = ConvertNativeToBigEndian( (u32_t)((u64_t)0xffffffff << (32-cidr)) );
				return mask;
			}

			text::ITextWriter& operator<<(text::ITextWriter& w, const TIPv4 ip)
			{
				w<<ip.octet[0]<<'.'<<ip.octet[1]<<'.'<<ip.octet[2]<<'.'<<ip.octet[3];
				return w;
			}


			TIPv6::TIPv6()
			{
				this->u32[0] = 0;
				this->u32[1] = 0;
				this->u32[2] = 0;
				this->u32[3] = 0;
			}

			TIPv6::TIPv6(TIPv4 v4)
			{
				this->group[0] = 0;
				this->group[1] = 0;
				this->group[2] = 0;
				this->group[3] = 0;
				this->group[4] = 0;
				this->group[5] = 0xffff;
				this->u32[3] = v4.u32;
			}

			TIPv6::TIPv6(const byte_t octet[16])
			{
				for(unsigned i = 0; i < 16; i++)
					this->octet[i] = octet[i];
			}

			TIPv6::TIPv6(const char* ipstr)
			{
				CL3_CLASS_ERROR(::sscanf(ipstr, "%hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx", this->group + 0, this->group + 1, this->group + 2, this->group + 3, this->group + 4, this->group + 5, this->group + 6, this->group + 7) != 8, TException, "unable to parse IPv6 address from string");

				for(unsigned i = 0; i < 16; i++)
					this->group[i] = ConvertNativeToBigEndian(this->group[i]);
			}

			TIPv6::TIPv6(const text::string::TString& ipstr) : TIPv6(TCString(ipstr, text::encoding::CODEC_CXX_CHAR).Chars())
			{
				// TODO: switch to use parser
			}

			TTCPClient::TTCPClient(fd_t fd, TIPv6 ip_local, TIPv6 ip_remote, u16_t port_local, u16_t port_remote) : TFDStream(fd), ip_local(ip_local), ip_remote(ip_remote), port_local(port_local), port_remote(port_remote)
			{
			}

			TIPv4 TIPv6::IPv4() const
			{
				CL3_CLASS_ERROR(!this->IsIPv4(), error::TException, "this is not an IPv4 address, and cannot be returned as such");
				return TIPv4(this->u32[3]);
			}

			bool TIPv6::IsIPv4() const
			{
				return
					this->group[0] == 0 &&
					this->group[1] == 0 &&
					this->group[2] == 0 &&
					this->group[3] == 0 &&
					this->group[4] == 0 && // 80 bits == 0x0
					this->group[5] == 0xffff;
			}

			bool TIPv6::IsIPv6() const
			{
				return !this->IsIPv4();
			}

			TIPv6 TTCPClient::LocalIP() const
			{
				return this->ip_local;
			}

			TIPv6 TTCPClient::RemoteIP() const
			{
				return this->ip_remote;
			}

			u16_t TTCPClient::LocalPort() const
			{
				return this->port_local;
			}

			u16_t TTCPClient::RemotePort() const
			{
				return this->port_remote;
			}

			TTCPClient::~TTCPClient()
			{
			}

			u16_t TTCPServer::Port() const
			{
				return this->port_local;
			}

			TIPv6 TTCPServer::IP() const
			{
				return this->ip_local;
			}

			TTCPServer::~TTCPServer()
			{
			}

			stream::fd::TWaitable TTCPServer::OnClientConnecting() const
			{
				return stream::fd::TWaitable(this->fd.FD(), true, false, false);
			}
		}
	}
}
