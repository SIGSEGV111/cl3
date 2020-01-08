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

#include "io_network.hpp"
#include "error.hpp"
#include "io_text_string.hpp"
#include "io_text_encoding.hpp"
#include "system_endian.hpp"

namespace cl3
{
	namespace io
	{
		namespace network
		{
			using namespace text::string;
			using namespace error;
			using namespace system::endian;

			TIPv4& TIPv4::operator&=(const TIPv4 rhs)
			{
				this->u32 &= rhs.u32;
				return *this;
			}

			TIPv4  TIPv4::operator&(const TIPv4 rhs) const
			{
				return TIPv4(*this) &= rhs;
			}

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

			TIPv6& TIPv6::operator&=(const TIPv6 rhs)
			{
				this->u64[0] &= rhs.u64[0];
				this->u64[1] &= rhs.u64[1];
				return *this;
			}

			TIPv6 TIPv6::operator&(const TIPv6 rhs) const
			{
				return TIPv6(*this) &= rhs;
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

			text::ITextWriter& operator<<(text::ITextWriter& w, const TIPv6 ip)
			{
// 				if(ip.IsIPv4())
// 					w<<((TIPv4)ip);
// 				else
// 				{
					cl3::io::text::TNumberFormat nf_group = cl3::io::text::TNumberFormat::HEX;
					nf_group.integer_length_min = 4;

					w<<TString(ip.group[0], &nf_group);
					for(unsigned i = 1; i < 8; i++)
						w<<":"<<TString(ConvertBigEndianToNative(ip.group[i]), &nf_group);
// 				}

				return w;
			}

			u8_t  TIPv6::MaskToCIDR(const TIPv6 mask)
			{
				const u8_t f = system::compiler::CountLeadingZeroes(~ConvertBigEndianToNative(mask.u64[0]));
				if(f == 64)
					return 64 + system::compiler::CountLeadingZeroes(~ConvertBigEndianToNative(mask.u64[1]));
				else
					return f;
			}

			static const u64_t U64_MAX = 0xffffffffffffffff;

			TIPv6 TIPv6::CIDRToMask(const u8_t cidr)
			{
				TIPv6 mask;

				if(cidr == 0)
				{
					mask.u64[0] = 0x0;
					mask.u64[1] = 0x0;
				}
				else if(cidr > 64)
				{
					mask.u64[0] = U64_MAX;
					mask.u64[1] = ConvertNativeToBigEndian( (U64_MAX << (128-cidr)) );
				}
				else if(cidr < 64)
				{
					mask.u64[0] = ConvertNativeToBigEndian( (U64_MAX << (64-cidr)) );
					mask.u64[1] = 0x0;
				}
				else
				{
					mask.u64[0] = U64_MAX;
					mask.u64[1] = 0x0;
				}

				return mask;
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
// 				collection::list::TList<TString> l = TString(ipstr).Split(":");
// 				CL3_CLASS_ERROR(l.Count() < 3 || l.Count() > 8, TException, "unable to parse IPv6 address from string '%s' (too many/too few ':')", ipstr);
//
// 				const usys_t i = l.IndexOf("");
// 				if(i != (usys_t)-1)
// 				{
// 					l.Remove(i, 1);
//
// 					while( l.Count() < 8 )
// 						l.Insert(i, TString("0"));
// 				}
//
// 				CL3_CLASS_ERROR(l.Find("") != NULL, TException, "unable to parse IPv6 address from string '%s' (contains more than one '::')", ipstr);
//
// 				for(unsigned i = 0; i < 8; i++)
// 				{
// 					l[i].number_format = &text::TNumberFormat::HEX;
// 					l[i] >> this->group[i];
// 					this->group[i] = ConvertNativeToBigEndian(this->group[i]);
// 				}

				CL3_CLASS_ERROR(inet_pton(AF_INET6, ipstr, this->octet) != 1, TException, "unable to parse IPv6 address '%s'", ipstr);
			}

			TIPv4::operator text::string::TString() const
			{
				TString s;
				s << *this;
				return s;
			}

			TIPv6::operator text::string::TString() const
			{
				TString s;
				s << *this;
				return s;
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


			CLASS TMAC::TMAC()
			{
				::memset(this->octet, 0, sizeof(this->octet));
			}

			CLASS TMAC::TMAC(const char* const s)
			{
				CL3_CLASS_ERROR(sscanf(s, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", octet+0, octet+1, octet+2, octet+3, octet+4, octet+5) != 6, TException, "cannot parse MAC address");
			}

			CLASS TMAC::TMAC(const cl3::io::text::string::TString& s)
			{
				const TCString cstr(s, text::encoding::CODEC_CXX_CHAR);
				CL3_CLASS_ERROR(sscanf(cstr.Chars(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", octet+0, octet+1, octet+2, octet+3, octet+4, octet+5) != 6, TException, "cannot parse MAC address");
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
