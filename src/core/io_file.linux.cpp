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

#define _FILE_OFFSET_BITS 64

#include "system_os.hpp"

#if (CL3_OS_DERIVATIVE == CL3_OS_DERIVATIVE_POSIX_LINUX)

#include "io_file.hpp"
#include "system_types_typeinfo.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/syscall.h>
#include <string.h>

namespace	cl3
{
	namespace	io
	{
		namespace	file
		{
			using namespace text::encoding;
			using namespace text::string;
			using namespace system::memory;
			using namespace system::types::typeinfo;
			using namespace system::time;
			using namespace collection::list;
			using namespace error;

			const usys_t NO_CACHE_MEMORY_ALIGNMENT = 512;

			CLASS	TFile::TFile	() : access(TAccess::READ | TAccess::WRITE /*| (executeable ? TAccess::EXECUTE : 0)*/)
			{
				const int mode = /*(access & TAccess::EXECUTE) ? 0777 :*/ 0666;
				const int flags = O_LARGEFILE | O_NOCTTY | O_CLOEXEC | O_TMPFILE | O_RDWR;

				this->fd = open(".", flags, mode);
				if(this->fd == -1)
				{
					if(errno == EOPNOTSUPP || errno == EINVAL || errno == EISDIR)
					{
						perror("O_TMPFILE not supported by kernel/filesystem, falling back to workaround");
						char tmpfile_name[] = ".cl3tmp-XXXXXX";
						CL3_CLASS_SYSERR(this->fd = mkostemp(tmpfile_name, O_CLOEXEC|O_NOCTTY|O_LARGEFILE|O_RDWR));
						CL3_CLASS_SYSERR(::unlink(tmpfile_name));
					}
					else
					{
						perror("open call failed with unhandled error code");
						CL3_CLASS_FAIL(TSyscallException, errno);
					}
				}
			}

			struct linux_dirent64
			{
				ino64_t        d_ino;    /* 64-bit inode number */
				off64_t        d_off;    /* 64-bit offset to next structure */
				unsigned short d_reclen; /* Size of this dirent */
				unsigned char  d_type;   /* File type */
				char           d_name[]; /* Filename (null-terminated) */
			};

			inline static int getdents64(unsigned int fd, struct linux_dirent64 *dirp, unsigned int count)
			{
				return ::syscall(SYS_getdents64, fd, dirp, count);
			}

			inline static bool TestStdDirs(const char* buffer)
			{
				// tests for "." and ".."
				// buffer is assumed to be at least 3 bytes long

				if(buffer[0] == '.' && buffer[1] == 0)
					return true;

				if(buffer[0] == '.' && buffer[1] == '.' && buffer[2] == 0)
					return true;

				return false;
			}

			#ifdef CL3_DEBUG
				static const usys_t GETDENTS_BUFFER_SIZE = 8;
			#else
				static const usys_t GETDENTS_BUFFER_SIZE = 4000;
			#endif

			usys_t	TDirectoryBrowser::EnumEntries	(util::function::TFunction<bool, const TDirectoryBrowser&, const text::string::TString&> func) const
			{
				usys_t n = 0;
				int n_bytes_used;
				TList<byte_t> buffer;

				CL3_CLASS_SYSERR(lseek(this->fd, 0, SEEK_SET));
				buffer.Count(GETDENTS_BUFFER_SIZE);

				for(;;)
				{
					CL3_CLASS_ERROR( (n_bytes_used = getdents64(this->fd, (linux_dirent64*)buffer.ItemPtr(0), buffer.Count() - 2)) == -1 && errno != EINVAL, TSyscallException, errno );

					if(n_bytes_used == -1 && errno == EINVAL)
					{
						// buffer too small
						buffer.Count(buffer.Count() * 2);
						continue;
					}

					if(n_bytes_used == 0)
						break;

					const byte_t* const e = buffer.ItemPtr(0) + n_bytes_used;
					const byte_t* p = buffer.ItemPtr(0);
					for(;p < e;)
					{
						linux_dirent64* d = (linux_dirent64*)p;
						p += d->d_reclen;

						if(!TestStdDirs(d->d_name))
						{
							n++;
							if(!func(*this, TString(d->d_name)))
								return n;
						}
					}
				}

				return n;
			}

			static TString Readlink(const int fd_dir, const TString linkname)
			{
				char buffer[256];
				ssize_t r;
				CL3_NONCLASS_SYSERR(r = readlinkat(fd_dir, TCString(linkname, CODEC_CXX_CHAR).Chars(), buffer, sizeof(buffer)));
				return TString(buffer, r);
			}

			static TFileInfo MakeFileInfo(const TString& filename, const struct stat& st, const int iflags, dev_t devid_parent, const int fd_dir)
			{
				TFileInfo fi;

				if(S_ISREG(st.st_mode))
					fi.type = EEntryType::FILE;
				else if(S_ISDIR(st.st_mode))
					fi.type = EEntryType::DIRECTORY;
				else if(S_ISCHR(st.st_mode))
					fi.type = EEntryType::DEVICE_CHAR;
				else if(S_ISBLK(st.st_mode))
					fi.type = EEntryType::DEVICE_BLOCK;
				else if(S_ISFIFO(st.st_mode))
					fi.type = EEntryType::PIPE;
				else if(S_ISLNK(st.st_mode))
					fi.type = EEntryType::SYMBOLIC_LINK;
				else if(S_ISSOCK(st.st_mode))
					fi.type = EEntryType::SOCKET;
				else
					fi.type = EEntryType::UNKNOWN;

				fi.n_hardlink = st.st_nlink;

				fi.setuid = (st.st_mode & S_ISUID) ? 1 : 0;
				fi.setgid = (st.st_mode & S_ISGID) ? 1 : 0;
				fi.sticky = (st.st_mode & S_ISVTX) ? 1 : 0;
				fi.executable = ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH)) ? 1 : 0;
				fi.archive = (iflags & FS_NODUMP_FL) ? 0 : 1;
				fi.encrypted  = 0;
				fi.hidden = (filename.Count() >= 1 && filename.Left(1) == '.') ? 1 : 0;
				fi.offline = 0;	// FIXME TODO posix: currently not mounted auto-mounted directory / windows: offline flag
				fi.system = 0;
				fi.mountpoint = st.st_dev != devid_parent ? 1 : 0;
				fi.append_only = (iflags & FS_APPEND_FL) ? 1 : 0;
				fi.compressed = (iflags & FS_COMPR_FL) ? 1 : 0;
				fi.dirsync = (iflags & FS_DIRSYNC_FL) ? 1 : 0;
				fi.immutable = (iflags & FS_IMMUTABLE_FL) ? 1 : 0;
				fi.journaled = (iflags & FS_JOURNAL_DATA_FL) ? 1 : 0;
				fi.no_atime = (iflags & FS_NOATIME_FL) ? 1 : 0;
				fi.no_cow = (iflags & FS_NOCOW_FL) ? 1 : 0;
				fi.no_tail_merge = (iflags & FS_NOTAIL_FL) ? 1 : 0;
				fi.secure_delete = (iflags & FS_SECRM_FL) ? 1 : 0;
				fi.sync = (iflags & FS_SYNC_FL) ? 1 : 0;
				fi.topdir = (iflags & FS_TOPDIR_FL) ? 1 : 0;
				fi.undeleteable = (iflags & FS_UNRM_FL) ? 1 : 0;

				fi.sz_virtual = st.st_size;
				fi.sz_physical = st.st_blksize * st.st_blocks;
				fi.ts_create = TTime(0,0);
				fi.ts_status = TTime(st.st_ctime);
				fi.ts_access = TTime(st.st_atime);
				fi.ts_write = TTime(st.st_mtime);

				if(fi.type == EEntryType::SYMBOLIC_LINK)
					fi.link_target = Readlink(fd_dir, filename);

				return fi;
			}

			usys_t	TDirectoryBrowser::EnumEntries	(collection::IDynamicCollection<text::string::TString>& collection) const
			{
				return this->EnumEntries([&collection](const TDirectoryBrowser&, const text::string::TString& filename) -> bool { collection.Add(filename); return true; });
			}

			TFileInfo TFile::Info	() const
			{
				struct stat st;
				memset(&st, 0, sizeof(st));
				CL3_CLASS_SYSERR(fstat(this->fd, &st));
				int iflags = 0;
				CL3_CLASS_SYSERR(ioctl(this->fd, FS_IOC_GETFLAGS, &iflags));
				return MakeFileInfo(TString(), st, iflags, st.st_dev, AT_FDCWD);
			}

			TFileInfo	GetFileInfo	(const text::string::TString& name)
			{
				struct stat st;
				memset(&st, 0, sizeof(st));
				const int ret = fstatat(AT_FDCWD, TCString(name, CODEC_CXX_CHAR).Chars(), &st, AT_NO_AUTOMOUNT|AT_SYMLINK_NOFOLLOW);
				if(ret == -1)
				{
					if(errno == ENOENT)
					{
						TFileInfo fi;
						fi.type = EEntryType::NOT_EXIST;
						return fi;
					}
					else
						CL3_NONCLASS_FAIL(TSyscallException, errno);
				}

				return MakeFileInfo(name, st, 0, st.st_dev, AT_FDCWD);
			}

			TFileInfo TDirectoryBrowser::GetFileInfo		(const text::string::TString& name) const
			{
				struct stat st;
				memset(&st, 0, sizeof(st));
				const int ret = fstatat(this->fd, TCString(name, CODEC_CXX_CHAR).Chars(), &st, AT_NO_AUTOMOUNT|AT_SYMLINK_NOFOLLOW);
				if(ret == -1)
				{
					if(errno == ENOENT)
					{
						TFileInfo fi;
						fi.type = EEntryType::NOT_EXIST;
						return fi;
					}
					else
						CL3_CLASS_FAIL(TSyscallException, errno);
				}

				return MakeFileInfo(name, st, 0, this->st_dir.st_dev, this->fd);
			}

			void	TDirectoryBrowser::CreateDirectory		(const text::string::TString& name, const bool ignore_existing)
			{
				const int ret = mkdirat(this->fd, TCString(name, CODEC_CXX_CHAR).Chars(), 0755);
				const int e = errno;

				if(ret == -1)
				{
					if(e == EEXIST && ignore_existing)
					{
						struct stat st;
						memset(&st, 0, sizeof(st));
						CL3_CLASS_SYSERR(fstatat(this->fd, TCString(name, CODEC_CXX_CHAR).Chars(), &st, AT_NO_AUTOMOUNT|AT_SYMLINK_NOFOLLOW));
						if(S_ISDIR(st.st_mode))
							return;
					}

					CL3_CLASS_FAIL(TSyscallException, e);
				}
			}

			text::string::TString
					TDirectoryBrowser::AbsolutePath			() const
					{
						char name_link[32];
						struct ::stat st;
						snprintf(name_link, 32, "/proc/self/fd/%d", this->fd.FD());
						CL3_CLASS_SYSERR(::lstat(name_link, &st));
						auto name_target = MakeUniquePtr<UPTR_MALLOC>((char*)malloc(st.st_size + 1));
						ssize_t r;
						CL3_CLASS_SYSERR(r = ::readlink(name_link, name_target.Object(), st.st_size));
						CL3_CLASS_LOGIC_ERROR(r > st.st_size);
						name_target.Object()[r] = '\0';
						return TString(name_target.Object());
					}
		}
	}
}

#endif
