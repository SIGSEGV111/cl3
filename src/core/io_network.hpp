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

#pragma once

#include "system_types.hpp"
#include "system_compiler.hpp"
#include "io_stream_fd.hpp"
#include "io_collection_list.hpp"

namespace cl3
{
	namespace io
	{
		namespace text
		{
			class ITextWriter;

			namespace string
			{
				class TString;
			}
		}

		namespace network
		{
			struct TMAC
			{
				u8_t octet[6];
			};

			struct TIPv4
			{
				union
				{
					u32_t u32;
					u8_t octet[4];
				};

				CL3PUBF TIPv4();
				CL3PUBF TIPv4(u8_t a, u8_t b, u8_t c, u8_t d);
				CL3PUBF explicit TIPv4(u32_t);
				CL3PUBF explicit TIPv4(const char*);
				CL3PUBF explicit TIPv4(const text::string::TString&);
				TIPv4(const TIPv4&) = default;

				inline bool operator!=(const TIPv4& rhs) const { return this->u32 != rhs.u32; }
				inline bool operator==(const TIPv4& rhs) const { return this->u32 == rhs.u32; }
				inline bool operator>=(const TIPv4& rhs) const { return this->u32 >= rhs.u32; }
				inline bool operator<=(const TIPv4& rhs) const { return this->u32 <= rhs.u32; }
				inline bool operator> (const TIPv4& rhs) const { return this->u32 >  rhs.u32; }
				inline bool operator< (const TIPv4& rhs) const { return this->u32 <  rhs.u32; }

				CL3PUBF static u8_t MaskToCIDR(const TIPv4);
				CL3PUBF static TIPv4 CIDRToMask(const u8_t);
			};

			CL3PUBF text::ITextWriter& operator<<(text::ITextWriter&, const TIPv4);

			struct TIPv6
			{
				union
				{
					u8_t octet[16];
					u16_t group[8];
					u32_t u32[4];
				};

				CL3PUBF bool IsIPv4() const CL3_GETTER;
				CL3PUBF bool IsIPv6() const CL3_GETTER;

				CL3PUBF TIPv4 IPv4() const CL3_GETTER;

				CL3PUBF TIPv6();
				CL3PUBF TIPv6(TIPv4);
				CL3PUBF explicit TIPv6(const byte_t octet[16]);
				CL3PUBF explicit TIPv6(const char*);
				CL3PUBF explicit TIPv6(const text::string::TString&);
				TIPv6(const TIPv6&) = default;
			};

			CL3PUBF collection::list::TList<TIPv6> ResolveHostname(const text::string::TString&);

			class TTCPClient : public stream::fd::TFDStream
			{
				protected:
					friend class TTCPServer;
					TIPv6 ip_local, ip_remote;
					u16_t port_local, port_remote;

					CL3PUBF TTCPClient(fd_t fd, TIPv6 ip_local, TIPv6 ip_remote, u16_t port_local, u16_t port_remote);

				public:
					CL3PUBF void CloseTX();
					CL3PUBF void CloseRX();

					CL3PUBF TIPv6 LocalIP() const CL3_GETTER;
					CL3PUBF TIPv6 RemoteIP() const CL3_GETTER;
					CL3PUBF u16_t LocalPort() const CL3_GETTER;
					CL3PUBF u16_t RemotePort() const CL3_GETTER;

					CL3PUBF TTCPClient(TIPv6 ip, u16_t port);
					CL3PUBF ~TTCPClient();
					TTCPClient(TTCPClient&&) = default;
					TTCPClient(const TTCPClient&) = delete;
			};


			class TTCPServer
			{
				protected:
					stream::fd::TFDStream fd;
					TIPv6 ip_local;
					u16_t port_local;

				public:
					enum class EIPVersionSelect
					{
						ONLY_IP_V4,
						ONLY_IP_V6,
						BOTH_PREFER_V4,
						BOTH_PREFER_V6
					};

					CL3PUBF u16_t Port() const CL3_GETTER;
					CL3PUBF TIPv6 IP() const CL3_GETTER;

					CL3PUBF stream::fd::TWaitable OnClientConnecting() const;
					CL3PUBF TTCPClient Accept();

					//	bind to specific address and random select free port
					CL3PUBF TTCPServer(TIPv6 ip);

					//	bind to specific address and port
					CL3PUBF TTCPServer(TIPv6 ip, u16_t port);

					//	do not bind to a fixed address, instead listen on all local addresses, use listen on specified port
					CL3PUBF TTCPServer(EIPVersionSelect, u16_t port);

					//	do not bind to a fixed address, instead listen on all local addresses, listen on random free port
					CL3PUBF TTCPServer(EIPVersionSelect ipvs);

					CL3PUBF ~TTCPServer();
					TTCPServer(TTCPServer&&) = default;
					TTCPServer(const TTCPServer&) = delete;
			};
		}
	}
}
