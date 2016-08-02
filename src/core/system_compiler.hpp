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

#ifndef	_include_cl3_core_system_compiler_hpp_
#define	_include_cl3_core_system_compiler_hpp_

#include "system_def.hpp"
#include "system_os.hpp"

#define	CL3_CXX_GCC		1	//	GNU G++
#define	CL3_CXX_MSVC	2	//	Microsoft Visual C++
#define	CL3_CXX_CLANG	3	//	LLVM CLANG

#ifndef CL3_CXX
	#ifdef _MSC_VER
		#define	CL3_CXX CL3_CXX_MSVC
	#endif

	#ifdef __clang__
		#define	CL3_CXX	CL3_CXX_CLANG
	#else
		#ifdef __GNUG__
			#define	CL3_CXX CL3_CXX_GCC
		#endif
	#endif
#endif

#if ((CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_CLANG) && CL3_OS == CL3_OS_POSIX)
	#include <alloca.h>
	#include <limits.h>
#endif

#if (CL3_CXX == CL3_CXX_MSVC)
	#include <WinSock2.h>
	#include <windows.h>
	#include <intrin.h>
	#include <Specstrings.h>
#endif

#if (CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_CLANG)
	namespace	cl3
	{
		namespace	system
		{
			namespace	types
			{
				typedef	unsigned char		u8_t;
				typedef	signed char			s8_t;
				typedef	unsigned short		u16_t;
				typedef	signed short		s16_t;

				#if (UINT_MAX == 4294967295)
					typedef	unsigned int	u32_t;
					typedef	signed int		s32_t;
				#else
				#error
				#endif

				#if (ULONG_MAX == 18446744073709551615ULL)
					typedef	unsigned long	u64_t;
					typedef	signed long		s64_t;
				#else
					typedef	unsigned long long	u64_t;
					typedef	signed long long	s64_t;
				#endif

				typedef	float			f32_t;
				typedef	double			f64_t;

				typedef unsigned char	byte_t;

				#if (__SIZEOF_SIZE_T__ == 4)
					typedef	u32_t	usys_t;
					typedef	s32_t	ssys_t;
				#elif (__SIZEOF_SIZE_T__ == 8)
					typedef	u64_t	usys_t;
					typedef	s64_t	ssys_t;
				#else
					#error "unknown wordsize"
				#endif

				typedef s64_t	soff_t;
				typedef u64_t	uoff_t;

				#if (CL3_OS == CL3_OS_WINDOWS)
					typedef void* fd_t;	//	HANDLE
				#elif (CL3_OS == CL3_OS_POSIX)
					typedef int fd_t;	//	fd
				#endif
			};
		};
	};
#elif (CL3_CXX == CL3_CXX_MSVC)
	namespace	cl3
	{
		namespace	system
		{
			namespace	types
			{
				#if (_MSC_VER < 1300)
					typedef	unsigned char		u8_t;
					typedef	signed char			s8_t;
					typedef	unsigned short		u16_t;
					typedef	signed short		s16_t;
					typedef	unsigned int		u32_t;
					typedef	signed int			s32_t;
					typedef	unsigned long long	u64_t;
					typedef	signed long long	s64_t;
				#else
					typedef	unsigned __int8		u8_t;
					typedef	signed __int8		s8_t;
					typedef	unsigned __int16	u16_t;
					typedef	signed __int16		s16_t;
					typedef	unsigned __int32	u32_t;
					typedef	signed __int32		s32_t;
					typedef	unsigned __int64	u64_t;
					typedef	signed __int64		s64_t;
				#endif

				typedef	float	f32_t;
				typedef	double	f64_t;

				typedef u8_t byte_t;

				#ifdef _WIN64
					typedef s64	ssys_t;
					typedef u64	usys_t;
				#else
					typedef s32	ssys_t;
					typedef u32	usys_t;
				#endif

				typedef s64_t	soff_t;
				typedef u64_t	uoff_t;
			};
		};
	};
#endif

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			using namespace types;

			enum ECachePrefetchMode
			{
				PREFETCH_READ,
				PREFETCH_WRITE
			};






			#if (CL3_CXX == CL3_CXX_GCC || CL3_CXX == CL3_CXX_CLANG)
				#define	CL3_THREAD	__thread
				#define	CL3_FCONST	__attribute__((const))
				#define	CL3_FPURE	__attribute__((pure))

				#define	CL3_CXX_EXPORT_FUNC	__attribute__ ((visibility("default")))
				#define	CL3_CXX_IMPORT_FUNC	__attribute__ ((visibility("default")))
				#define	CL3_CXX_EXPORT_TYPE	__attribute__ ((visibility("default")))
				#define	CL3_CXX_IMPORT_TYPE	__attribute__ ((visibility("default")))

				#define CL3_PACK( ... ) __VA_ARGS__ __attribute__((__packed__))
				#define	CL3_WARN_UNUSED_RESULT	__attribute__((warn_unused_result))

				#ifdef CL3_DEBUG
					#define CL3_UNREACHABLE	CL3_NONCLASS_LOGIC_ERROR(true) /* LCOV_EXCL_LINE */
				#else
					#define CL3_UNREACHABLE	__builtin_unreachable() /* LCOV_EXCL_LINE */
				#endif

				#define	CL3_LIKELY(expr)	__builtin_expect((expr), true)
				#define	CL3_UNLIKELY(expr)	__builtin_expect((expr), false)

				template<class T> inline static T AtomicAdd(T& var, T add) { return __sync_fetch_and_add(&var,add); }
				template<class T> inline static T AtomicSub(T& var, T sub) { return __sync_fetch_and_sub(&var,sub); }
				template<class T1, class T2, class T3> inline static T1 AtomicSwap(T1& var, T2 cmpval, T3 newval) { return __sync_val_compare_and_swap(&var,cmpval,newval); }

				inline static void CacheSync() { __sync_synchronize(); }
				#define	CachePrefetch(adr,rw,tloc)	__builtin_prefetch((adr),(rw) == PREFETCH_READ ? 0 : ( (rw) == PREFETCH_WRITE ? 1 : 2 ),(tloc))

				//	Returns the number of leading 0-bits in x, starting at the most significant bit position.
				inline static int CountLeadingZeroes(u32_t v) { return v == 0 ? (32) : (sizeof(int ) == 4 ? __builtin_clz (v) : __builtin_clzl (v)); }
				inline static int CountLeadingZeroes(u64_t v) { return v == 0 ? (64) : (sizeof(long) == 8 ? __builtin_clzl(v) : __builtin_clzll(v)); }

				//	Returns one plus the index of the least significant 1-bit of v, or if v is zero, returns zero.
				inline static int FindFirstSet(u32_t v) { return sizeof(int ) == 4 ? __builtin_ffs (v) : __builtin_ffsl (v); }
				inline static int FindFirstSet(u64_t v) { return sizeof(long) == 8 ? __builtin_ffsl(v) : __builtin_ffsll(v); }

				//	Returns the number of trailing 0-bits in x, starting at the least significant bit position.
				inline static int CountTrailingZeroes(u32_t v) { return v == 0 ? 32 : FindFirstSet(v); }
				inline static int CountTrailingZeroes(u64_t v) { return v == 0 ? 64 : FindFirstSet(v); }

				//	Returns one plus the index of the most significant 1-bit of v, or if v is zero, returns zero.
				inline static int FindLastSet(u32_t v) { return 32 - CountLeadingZeroes(v); }
				inline static int FindLastSet(u64_t v) { return 64 - CountLeadingZeroes(v); }

				#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
					#define	CL3_IS_LITTLE_ENDIAN	1
					#define	CL3_IS_BIG_ENDIAN		0
				#else
					#define	CL3_IS_LITTLE_ENDIAN	0
					#define	CL3_IS_BIG_ENDIAN		1
				#endif

			#endif







			#if (CL3_CXX == CL3_CXX_MSVC)
				#define	CL3_THREAD	__declspec(thread)
				#define	CL3_FCONST	__declspec(noalias)
				#define	CL3_FPURE	__declspec(noalias)

				#define	CL3_CXX_EXPORT_FUNC	__declspec(dllexport)
				#define	CL3_CXX_IMPORT_FUNC	__declspec(dllimport)
				#define CL3_CXX_EXPORT_TYPE
				#define CL3_CXX_IMPORT_TYPE

				#define CL3_PACK( ... ) __pragma( pack(push, 1) ); __VA_ARGS__; __pragma( pack(pop) )
				#define	CL3_WARN_UNUSED_RESULT	__checkReturn
				#define	CL3_UNREACHABLE		__assume(0)
				#define	CL3_LIKELY(expr)	(expr)
				#define	CL3_UNLIKELY(expr)	(expr)

				template<class T> inline static T AtomicAdd (T& var, T add) { if(sizeof(T) == 4) return (T)InterlockedExchangeAdd((unsigned int*)var, (unsigned int) add); else if(sizeof(T) == 8) return (T)InterlockedExchangeAdd64((LONG64*)var, (LONG64) add); }
				template<class T> inline static T AtomicSub (T& var, T sub) { if(sizeof(T) == 4) return (T)InterlockedExchangeAdd((unsigned int*)var, (unsigned int)-sub); else if(sizeof(T) == 8) return (T)InterlockedExchangeAdd64((LONG64*)var, (LONG64)-sub); }
				template<class T> inline static T AtomicSwap(T& var, T cmp, T newval) { if(sizeof(T1) == 4) return (T1)InterlockedCompareExchange((unsigned int*)var, (unsigned int)newval, (unsigned int)cmp); else if(sizeof(T1) == 8) return (T1)InterlockedCompareExchange64((LONG64*)var, (LONG64)newval, (LONG64)cmp); }

				inline static void CacheSync() { _ReadWriteBarrier(); }
				#define	CachePrefetch(adr,rw,tloc)	_mm_prefetch(reinterpret_cast<const char*>(adr),tloc)

				//	Returns the number of leading 0-bits in x, starting at the most significant bit position.
				inline static DWORD CountLeadingZeroes(unsigned __int32 v) { DWORD r; _BitScanReverse(&r, v); return 31-r; }
				inline static DWORD CountLeadingZeroes(unsigned __int64 v);

				//	Returns one plus the index of the least significant 1-bit of v, or if v is zero, returns zero.
				inline static DWORD FindFirstSet(unsigned __int32 v) { DWORD r; return _BitScanReverse(&r, v) ? r : 0; }
				inline static DWORD FindFirstSet(unsigned __int64 v);

				//	Returns the number of trailing 0-bits in x, starting at the least significant bit position.
				inline static int CountTrailingZeroes(unsigned __int32 v) { return v == 0 ? 32 : FindFirstSet(v); }
				inline static int CountTrailingZeroes(unsigned __int64 v) { return v == 0 ? 64 : FindFirstSet(v); }

				//	Returns one plus the index of the most significant 1-bit of v, or if v is zero, returns zero.
				inline static int FindLastSet(unsigned __int32 v) { return 32 - CountLeadingZeroes(v); }
				inline static int FindLastSet(unsigned __int64 v) { return 64 - CountLeadingZeroes(v); }

				#ifdef _WIN64
					inline static DWORD CountLeadingZeroes(unsigned __int64 v) { DWORD r; _BitScanReverse64(&r, v); return 63-r; }
					inline static DWORD FindFirstSet      (unsigned __int64 v) { DWORD r; return _BitScanReverse64(&r, v) ? r : 0; }
				#else
					inline static DWORD CountLeadingZeroes(unsigned __int64 v)
					{
						const unsigned __int32* const arr = reinterpret_cast<unsigned __int32*>(&v);
						DWORD r;
						if(_BitScanReverse(&r, arr[0]))
						{
							return 31 - r;
						}
						else
						{
							if(_BitScanReverse(&r, arr[1]))
							{
								return 63 - r;
							}
							else
							{
								return 64;
							}
						}
					}

					inline static DWORD FindFirstSet      (unsigned __int64 v)
					{
						const unsigned __int32* const arr = reinterpret_cast<unsigned __int32*>(&v);
						DWORD r;
						if(_BitScanReverse(&r, arr[0]))
						{
							return r;
						}
						else
						{
							if(_BitScanReverse(&r, arr[1]))
							{
								return 32+r;
							}
							else
							{
								return 0;
							}
						}
					}
				#endif

				//	fix some broken stuff on windows:
				#define	snprintf	_snprintf
				#define alloca		_alloca
				#define	strtoull	_strtoui64
				#undef Yield
				#undef Sleep
				#undef SendMessage
			#endif

			#ifdef	INSIDE_CL3
				#define	CL3PUBF CL3_CXX_EXPORT_FUNC
				#define	CL3PUBT CL3_CXX_EXPORT_TYPE
				#define	CL3PUBV extern CL3_CXX_EXPORT_FUNC
			#else
				#define	CL3PUBF CL3_CXX_IMPORT_FUNC
				#define	CL3PUBT CL3_CXX_IMPORT_TYPE
				#define	CL3PUBV extern CL3_CXX_IMPORT_FUNC
			#endif

			#ifndef CL3_CXX
				#error "unknown compiler"
			#endif

			#define	CL3_GETTER CL3_WARN_UNUSED_RESULT CL3_FPURE
			#define	CL3_SETTER
		}
	}
}


#include <new>

CL3PUBF	void* operator new(size_t sz);
CL3PUBF	void* operator new[](size_t sz);
CL3PUBF	void* operator new(size_t sz, const std::nothrow_t&) throw();
CL3PUBF	void* operator new[](size_t sz, const std::nothrow_t&) throw();

CL3PUBF	void  operator delete(void* p_mem) throw();
CL3PUBF	void  operator delete[](void* p_mem) throw();
CL3PUBF	void  operator delete(void* p_mem, const std::nothrow_t&) throw();
CL3PUBF	void  operator delete[](void* p_mem, const std::nothrow_t&) throw();

#endif
