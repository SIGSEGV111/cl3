/*
 * l ibcl3 - common library version 3 *
 * Copyright (C) 2013	Simon Brennecke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>

#include "error.hpp"
#include "io_network.hpp"
#include "system_endian.hpp"

namespace cl3
{
	namespace io
	{
		namespace network
		{
			using namespace stream::fd;

			void TTCPClient::CloseTX()
			{
				CL3_CLASS_SYSERR(::shutdown(this->FD(), SHUT_WR));
			}

			void TTCPClient::CloseRX()
			{
				CL3_CLASS_SYSERR(::shutdown(this->FD(), SHUT_RD));
			}

			TTCPClient::TTCPClient(TIPv6 ip, u16_t port)
			{
				int fd;
				if(ip.IsIPv4())
				{
					CL3_CLASS_SYSERR(fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 6));
					this->FD(fd);
					sockaddr_in addr;
					memset(&addr, 0, sizeof(addr));
					addr.sin_family = AF_INET;
					addr.sin_port = system::endian::ConvertNativeToBigEndian(port);
					addr.sin_addr.s_addr = ip.IPv4().u32;
					CL3_CLASS_ERROR(connect(this->FD(), (const sockaddr*)&addr, sizeof(addr)) == -1 && errno != EINPROGRESS, error::TSyscallException, errno);
				}
				else if(ip.IsIPv6())
				{
					CL3_CLASS_SYSERR(fd = socket(AF_INET6, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 6));
					this->FD(fd);
					sockaddr_in6 addr;
					memset(&addr, 0, sizeof(addr));
					addr.sin6_family = AF_INET6;
					addr.sin6_port = system::endian::ConvertNativeToBigEndian(port);
					memcpy(addr.sin6_addr.s6_addr, ip.octet, 16);
					CL3_CLASS_ERROR(connect(this->FD(), (const sockaddr*)&addr, sizeof(addr)) == -1 && errno != EINPROGRESS, error::TSyscallException, errno);
				}
				else
					CL3_CLASS_LOGIC_ERROR(true);
			}

			TTCPClient TTCPServer::Accept()
			{
				union
				{
					sockaddr addr;
					sockaddr_in ip4;
					sockaddr_in6 ip6;
				};

				memset(&addr, 0, sizeof(addr));

				socklen_t sz = sizeof(addr);
				int fd;
				CL3_CLASS_SYSERR(fd = ::accept4(this->fd.FD(), &addr, &sz, SOCK_NONBLOCK | SOCK_CLOEXEC));
				CL3_CLASS_LOGIC_ERROR(sz > sizeof(ip4) && sz > sizeof(ip6));

				u16_t port_remote = 0;
				TIPv6 ip_remote;

				switch(addr.sa_family)
				{
					case AF_INET:
						port_remote = system::endian::ConvertBigEndianToNative(ip4.sin_port);
						ip_remote = TIPv6(TIPv4(ip4.sin_addr.s_addr));
						break;
					case AF_INET6:
						port_remote = system::endian::ConvertBigEndianToNative(ip6.sin6_port);
						ip_remote = TIPv6(ip6.sin6_addr.s6_addr);
						break;
					default:
						CL3_CLASS_LOGIC_ERROR(true);
				}

				return TTCPClient(fd, this->ip_local, ip_remote, this->port_local, port_remote);
			}

			//	bind to specific address and random select free port
			TTCPServer::TTCPServer(TIPv6 ip) : TTCPServer(ip, 0)
			{
			}

			TTCPServer::TTCPServer(TIPv6 ip, u16_t port) : ip_local(ip), port_local(port)
			{
				int fd;
				if(ip.IsIPv4())
				{
					CL3_CLASS_SYSERR(fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 6));
					this->fd.FD(fd);
					sockaddr_in addr;
					memset(&addr, 0, sizeof(addr));
					addr.sin_family = AF_INET;
					addr.sin_port = system::endian::ConvertNativeToBigEndian(port);
					addr.sin_addr.s_addr = ip.IPv4().u32;
					CL3_CLASS_SYSERR(bind(this->fd.FD(), (const sockaddr*)&addr, sizeof(addr)));

					if(port == 0)
					{
						socklen_t sz = sizeof(addr);
						CL3_CLASS_SYSERR(getsockname(this->fd.FD(), (sockaddr*)&addr, &sz));
						CL3_CLASS_LOGIC_ERROR(sz > sizeof(addr));
						this->port_local = system::endian::ConvertBigEndianToNative(addr.sin_port);
					}
				}
				else if(ip.IsIPv6())
				{
					CL3_CLASS_SYSERR(fd = socket(AF_INET6, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 6));
					this->fd.FD(fd);
					sockaddr_in6 addr;
					memset(&addr, 0, sizeof(addr));
					addr.sin6_family = AF_INET6;
					addr.sin6_port = system::endian::ConvertNativeToBigEndian(port);
					memcpy(addr.sin6_addr.s6_addr, ip.octet, 16);
					CL3_CLASS_SYSERR(bind(this->fd.FD(), (const sockaddr*)&addr, sizeof(addr)));

					if(port == 0)
					{
						socklen_t sz = sizeof(addr);
						CL3_CLASS_SYSERR(getsockname(this->fd.FD(), (sockaddr*)&addr, &sz));
						CL3_CLASS_LOGIC_ERROR(sz > sizeof(addr));
						this->port_local = system::endian::ConvertBigEndianToNative(addr.sin6_port);
					}
				}
				else
					CL3_CLASS_LOGIC_ERROR(true);

				CL3_CLASS_SYSERR(listen(this->fd.FD(), 32));
			}

			TTCPServer::TTCPServer(EIPVersionSelect, u16_t)
			{
				CL3_NOT_IMPLEMENTED;
			}

			TTCPServer::TTCPServer(EIPVersionSelect ipvs) : TTCPServer(ipvs, 0)
			{
			}
		}
	}
}
