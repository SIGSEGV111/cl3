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

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include "system_os.hpp"

#if (CL3_OS == CL3_OS_POSIX)

#include "io_stream_fd.hpp"
#include "error-ext.hpp"

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

				size_t	TFDStream::Read		(byte* arr_items_read, size_t n_items_read_max, size_t n_items_read_min)
				{
					bool b_would_block = false;
					size_t n_already_read = 0;
					errno = 0;	//	reset error indicator

					do
					{
						if(b_would_block)
						{
							//	we already know that the next read-syscall would block (if it weren't marked as O_NONBLOCK)
							//	instead of busy-waiting for more data, we use poll() to actually block until more data is available
							struct ::pollfd pfd = { fd, POLLIN, 0 };
							CL3_CLASS_SYSERR(::poll(&pfd, 1, -1));
							CL3_CLASS_ERROR(pfd.revents != POLLIN, TException, "unknown events were returned by ::poll()");
						}

						const size_t n_read_now = n_items_read_max - n_already_read;	//	calculate how much to read at most in this loop iteration
						const ssize_t status = ::read(fd, arr_items_read + n_already_read, n_read_now);	//	do the actual read-syscall to the kernel

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
							if(status == 0)
							{
								CL3_CLASS_ERROR(n_already_read < n_items_read_min, TSourceDryException, n_items_read_max, n_items_read_min, n_items_read_min - n_already_read);
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

				off64_t	TFDStream::Left		(size_t sz_unit) const
				{
					CL3_CLASS_ARGUMENT_ERROR(sz_unit != 1, sz_unit, "this class only works with 1-byte-sized units");

					errno = 0;
					const off64_t current_offset = lseek64(fd, 0, SEEK_CUR);
					if(current_offset == (off64_t)-1)
					{
						if(errno == ESPIPE)
						{
							//	fd is not seekable
							struct ::pollfd pfd = { fd, POLLIN, 0 };
							CL3_CLASS_SYSERR(::poll(&pfd, 1, 0));
							if((pfd.revents & POLLIN) != 0)
								return 1;
							else if((pfd.revents & POLLRDHUP) != 0)
								return 0;
							else
								return (size_t)-1;
						}
						else
							//	error during lseek64
							CL3_CLASS_SYSERR(errno);
						CL3_CLASS_LOGIC_ERROR(true);
					}
					else
					{
						//	fd is seekable
						off64_t end_offset, return_offset;
						CL3_CLASS_SYSERR(end_offset = lseek64(fd, 0, SEEK_END));
						CL3_CLASS_SYSERR(return_offset = lseek64(fd, current_offset, SEEK_SET));
						CL3_CLASS_LOGIC_ERROR(return_offset != current_offset || end_offset < current_offset);
						return end_offset - current_offset;
					}
				}

				size_t	TFDStream::Write	(const byte* arr_items_write, size_t n_items_write_max, size_t n_items_write_min)
				{
					bool b_would_block = false;
					size_t n_already_written = 0;
					errno = 0;	//	reset error indicator

					do
					{
						if(b_would_block)
						{
							//	we already know that the next write-syscall would block (if it weren't marked as O_NONBLOCK)
							//	instead of busy-waiting for more space, we use poll() to actually block until we can write more data
							struct ::pollfd pfd = { fd, POLLOUT, 0 };
							CL3_CLASS_SYSERR(::poll(&pfd, 1, -1));
							CL3_CLASS_ERROR(pfd.revents != POLLOUT, TException, "unknown events were returned by ::poll()");
						}

						const size_t n_write_now = n_items_write_max - n_already_written;	//	calculate how much to write at most in this loop iteration
						const ssize_t status = ::write(fd, arr_items_write + n_already_written, n_write_now);	//	do the actual write-syscall to the kernel

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
								CL3_CLASS_ERROR(n_already_written < n_items_write_min, TSinkFloodedException, n_items_write_max, n_items_write_min, n_items_write_min - n_already_written);
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

				off64_t	TFDStream::Space	(size_t sz_unit) const
				{
					CL3_CLASS_ARGUMENT_ERROR(sz_unit != 1, sz_unit, "this class only works with 1-byte-sized units");
					struct ::pollfd pfd = { fd, POLLOUT, 0 };
					CL3_CLASS_SYSERR(::poll(&pfd, 1, 0));
					if((pfd.revents & POLLOUT) != 0)
						return 1;
					else if((pfd.revents & POLLHUP) != 0)
						return 0;
					else
						return (size_t)-1;
				}

				CLASS	TFDStream::TFDStream	(TFD fd) : p_buffer(NULL), sz_buffer(0), fd(fd)
				{
					CL3_CLASS_ERROR(fd == -1, TException, "file-descriptor is invalid");

					//	read the flags on the file-descriptor and make sure they contain O_NONBLOCK
					long flags;
					CL3_CLASS_SYSERR(flags = ::fcntl(fd, F_GETFL));
					if((flags & O_NONBLOCK) == 0)
						CL3_CLASS_SYSERR(::fcntl(fd, F_SETFL, flags | O_NONBLOCK));
				}

				CLASS	TFDStream::TFDStream	(const TFDStream& other) : p_buffer(NULL), sz_buffer(0)
				{
					//	duplicate the file-descriptor and while doing so, atomically set O_CLOEXEC flag
					CL3_CLASS_SYSERR(this->fd = ::fcntl(other.fd, F_DUPFD_CLOEXEC, 0));
				}

				CLASS	TFDStream::~TFDStream	()
				{
					CL3_CLASS_SYSERR(::close(fd));
				}
			}
		}
	}
}

#endif
