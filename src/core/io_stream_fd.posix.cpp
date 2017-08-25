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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "io_stream_fd.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>

namespace	cl3
{
	namespace	io
	{
		namespace	stream
		{
			namespace	fd
			{
				using namespace error;

				fd_t	TFDStream::FD		() const
				{
					return this->fd;
				}

				void	TFDStream::FD		(fd_t new_fd)
				{
					if(new_fd == this->fd)
						return;

					if(this->fd != -1)
					{
						CL3_CLASS_SYSERR(close(this->fd));
						this->fd = -1;
					}

					if(new_fd != -1)
					{
						//	read the flags on the file-descriptor and make sure they contain O_NONBLOCK
						long flags;
						CL3_CLASS_SYSERR(flags = ::fcntl(new_fd, F_GETFL));
						if((flags & O_NONBLOCK) == 0)
							CL3_CLASS_SYSERR(::fcntl(new_fd, F_SETFL, flags | O_NONBLOCK));
						this->fd = new_fd;
					}
				}

				/******************************************************************/

				usys_t	TFDStream::Read		(byte_t* arr_items_read, usys_t n_items_read_max, usys_t n_items_read_min)
				{
					if(n_items_read_min == (usys_t)-1)
						n_items_read_min = n_items_read_max;

					bool b_would_block = false;
					usys_t n_already_read = 0;
					errno = 0;	//	reset error indicator

					do
					{
						if(b_would_block)
						{
							//	we already know that the next read-syscall would block (if it weren't marked as O_NONBLOCK)
							//	instead of busy-waiting for more data, we use poll() to actually block until more data is available
							struct ::pollfd pfd = { this->fd, POLLIN, 0 };
							CL3_CLASS_SYSERR(::poll(&pfd, 1, -1));

							CL3_CLASS_ERROR(pfd.revents & POLLERR || pfd.revents & POLLNVAL, TSyscallException, errno);
							CL3_CLASS_ERROR((pfd.revents & POLLHUP) != 0 && (pfd.revents & POLLIN) == 0 && n_already_read < n_items_read_min, TSourceDryException, n_items_read_max, n_items_read_min, n_items_read_min - n_already_read, 0);
							CL3_CLASS_LOGIC_ERROR((pfd.revents & POLLIN) == 0);	//	not error, not HUP, but also no data available? unknown situation...
						}

						const usys_t n_read_now = n_items_read_max - n_already_read;	//	calculate how much to read at most in this loop iteration
						const ssys_t status = ::read(this->fd, arr_items_read + n_already_read, n_read_now);	//	do the actual read-syscall to the kernel

						if(status > 0 && errno == 0)
						{
							//	if everything went well (no error, and actual data read)
							n_already_read += status;
							b_would_block = false;
						}
						else // if(status <= 0 || errno != 0)
						{
							if(errno == EAGAIN || errno == EWOULDBLOCK)
								//	this read would normally have blocked, but didn't since it was marked as O_NONBLOCK
								b_would_block = true;
							else if(status == 0)
							{
								CL3_CLASS_ERROR(n_already_read < n_items_read_min, TSourceDryException, n_items_read_max, n_items_read_min, n_items_read_min - n_already_read, 0);
								break;
							}
							else
								//	some other error happend
								CL3_CLASS_SYSERR(status);	//	throw exception

							errno = 0;	//	reset error indicator
						}
					}
					while(n_already_read < n_items_read_min || (n_already_read < n_items_read_max && !b_would_block));

					return n_already_read;
				}

				/******************************************************************/

				usys_t	TFDStream::Write	(const byte_t* arr_items_write, usys_t n_items_write_max, usys_t n_items_write_min)
				{
					if(n_items_write_min == (usys_t)-1)
						n_items_write_min = n_items_write_max;

					bool b_would_block = false;
					usys_t n_already_written = 0;
					errno = 0;	//	reset error indicator

					do
					{
						if(b_would_block)
						{
							//	we already know that the next write-syscall would block (if it weren't marked as O_NONBLOCK)
							//	instead of busy-waiting for more space, we use poll() to actually block until we can write more data
							struct ::pollfd pfd = { this->fd, POLLOUT, 0 };
							CL3_CLASS_SYSERR(::poll(&pfd, 1, -1));

							CL3_CLASS_ERROR(pfd.revents & POLLERR || pfd.revents & POLLNVAL, TSyscallException, errno);
							CL3_CLASS_ERROR(pfd.revents & POLLHUP && n_already_written < n_items_write_min, TSinkFloodedException, n_items_write_max, n_items_write_min, n_items_write_min - n_already_written, 0);
							CL3_CLASS_LOGIC_ERROR((pfd.revents & POLLOUT) == 0);	//	not error, not HUP, and still cannot write, unknown situation...
						}

						const usys_t n_write_now = n_items_write_max - n_already_written;	//	calculate how much to write at most in this loop iteration
						const ssys_t status = ::write(this->fd, arr_items_write + n_already_written, n_write_now);	//	do the actual write-syscall to the kernel

						if(status > 0 && errno == 0)
						{
							//	if everything went well (no error, and actual data written)
							n_already_written += status;
							b_would_block = false;
						}
						else // if(status <= 0 || errno != 0)
						{
							if(errno == EAGAIN || errno == EWOULDBLOCK)
								//	this write would normally have blocked, but didn't since it was marked as O_NONBLOCK
								b_would_block = true;
							else if(status == 0 || errno == ENOSPC)
							{
								//	no more can be written (disk full)
								CL3_CLASS_ERROR(n_already_written < n_items_write_min, TSinkFloodedException, n_items_write_max, n_items_write_min, n_items_write_min - n_already_written, 0);
								break;
							}
							else
								//	some other error happend
								CL3_CLASS_SYSERR(status);	//	throw exception

							errno = 0;	//	reset error indicator
						}
					}
					while(n_already_written < n_items_write_min || (n_already_written < n_items_write_max && !b_would_block));

					return n_already_written;
				}

				/******************************************************************/

				usys_t	TFDStream::Remaining	() const
				{
					struct pollfd pfd = { this->fd, POLLIN|POLLPRI|POLLERR|POLLHUP|POLLNVAL|POLLRDHUP, 0 };
					CL3_CLASS_SYSERR(::poll(&pfd, 1, 0));
					if(pfd.revents)
					{
						if((pfd.revents & POLLIN) == POLLIN || (pfd.revents & POLLPRI) == POLLPRI)
							return (usys_t)-1;	//	yep, there is data
						else
							return 0;	//	error
					}
					else
						return (usys_t)-1;	//	the FD seems operational, so we can assume there is more data to be read at some future point
				}

				void	TFDStream::Close		()
				{
					if(this->fd != -1)
					{
						CL3_CLASS_SYSERR(::close(this->fd));
						this->fd = -1;
					}
				}

				/******************************************************************/

				CLASS	TFDStream::TFDStream	() : fd(-1)
				{
				}

				CLASS	TFDStream::TFDStream	(fd_t fd) : fd(-1)
				{
					CL3_CLASS_ERROR(fd == -1, TException, "file-descriptor is invalid");
					this->FD(fd);
				}

				CLASS	TFDStream::TFDStream	(const TFDStream& other)
				{
					//	duplicate the file-descriptor and while doing so, atomically set O_CLOEXEC flag
					CL3_CLASS_SYSERR(this->fd = ::fcntl(other.fd, F_DUPFD_CLOEXEC, 0));
				}

				CLASS	TFDStream::TFDStream	(TFDStream&& other) : fd(other.fd)
				{
					other.fd = -1;
				}

				/******************************************************************/

				CLASS	TFDStream::~TFDStream	()
				{
					this->Close();
				}

				/******************************************************************/

				TWaitable TFDStream::OnOutputReady() const
				{
					return TWaitable(this->fd, false, true, true);
				}

				TWaitable TFDStream::OnInputReady() const
				{
					return TWaitable(this->fd, true, false, true);
				}

				/******************************************************************/

				CLASS TWaitable::TWaitable() : fd(-1), input(false), output(false), error(false)
				{
				}

				CLASS TWaitable::TWaitable(fd_t fd, bool input, bool output, bool error) : fd(fd), input(input), output(output), error(error)
				{
				}

				system::task::synchronization::waitinfo_t TWaitable::WaitInfo() const
				{
					struct ::pollfd pfd = {
						this->fd,
						(short)(
							(this->input ? POLLIN|POLLPRI : 0) |
							(this->output ? POLLOUT : 0) |
							(this->error ? POLLERR : 0)
						),
						0
					};
					return pfd;
				}
			}
		}
	}
}

#endif
