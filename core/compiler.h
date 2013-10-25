/*
    libcl2 - common library version 3
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

#ifndef	_include_cl3_system_compiler_h_
#define	_include_cl3_system_compiler_h_

#include "def.h"

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			#define	CXX_GNU		1	//	GNU G++
			#define	CXX_MSVC	2	//	Microsoft Visual C++

			#define	CL3_ENDIANITY_LITTLE	1
			#define	CL3_ENDIANITY_BIG		2

			#ifndef _CXX
				#ifdef _MSC_VER
					#define	_CXX	CXX_MSVC
				#endif

				#ifdef __GNUC__
					#define	_CXX	CXX_GNU
				#endif
			#endif

			#if (_CXX == CXX_GNU)
				#include <alloca.h>
				#include <stdint.h>

				#define	THREAD	__thread
				#define	FCONST	__attribute__((const))
				#define	FPURE	__attribute__((pure))

				#define	CXX_EXPORT_FUNC	__attribute__ ((visibility("default")))
				#define	CXX_IMPORT_FUNC	__attribute__ ((visibility("default")))
				#define	CXX_EXPORT_TYPE	__attribute__ ((visibility("default")))
				#define	CXX_IMPORT_TYPE	__attribute__ ((visibility("default")))

				#define PACK( ... ) __VA_ARGS__ __attribute__((__packed__))

				#define	warn_unused_result	__attribute__((warn_unused_result))

				//	Returns the number of leading 0-bits in x, starting at the most significant bit position.
				inline static int COUNT_LEADING_ZEROES(uint32_t v) { return v == 0 ? (32) : (sizeof(int ) == 4 ? __builtin_clz (v) : __builtin_clzl (v)); }
				inline static int COUNT_LEADING_ZEROES(uint64_t v) { return v == 0 ? (64) : (sizeof(long) == 8 ? __builtin_clzl(v) : __builtin_clzll(v)); }

				//	Returns one plus the index of the least significant 1-bit of v, or if v is zero, returns zero.
				inline static int FIND_FIRST_SET(uint32_t v) { return sizeof(int ) == 4 ? __builtin_ffs (v) : __builtin_ffsl (v); }
				inline static int FIND_FIRST_SET(uint64_t v) { return sizeof(long) == 8 ? __builtin_ffsl(v) : __builtin_ffsll(v); }

				//	Returns the number of trailing 0-bits in x, starting at the least significant bit position.
				inline static int COUNT_TRAILING_ZEROES(uint32_t v) { return v == 0 ? 32 : FIND_FIRST_SET(v); }
				inline static int COUNT_TRAILING_ZEROES(uint64_t v) { return v == 0 ? 64 : FIND_FIRST_SET(v); }

				//	Returns one plus the index of the most significant 1-bit of v, or if v is zero, returns zero.
				inline static int FIND_LAST_SET(uint32_t v) { return 32 - COUNT_LEADING_ZEROES(v); }
				inline static int FIND_LAST_SET(uint64_t v) { return 64 - COUNT_LEADING_ZEROES(v); }

				#ifdef CL3_OPT__MULTITHREADED
					#define	ATOMIC_ADD(var,add)			__sync_fetch_and_add(&var,add)
					#define	ATOMIC_SUB(var,sub)			__sync_fetch_and_sub(&var,sub)
					#define	ATOMIC_SWAP(var,cmp,newval)	__sync_val_compare_and_swap(&var,cmp,newval)
					#define	SYNC()						__sync_synchronize()
				#else
					template<class T1>
					inline static __gcc_not_atomic_swap(T1* var, T1 cmp, T1 newval)
					{
						if(*var == cmp)
						{
							*var = newval;
							return cmp;
						}
						else
							return *var;
					}

					#define	ATOMIC_ADD(var,add)			((var += add) - add)
					#define	ATOMIC_SUB(var,sub)			((var -= add) + add)
					#define	ATOMIC_SWAP(var,cmp,newval)	__gcc_not_atomic_swap(&var, cmp, newval)
					#define	SYNC						__sync_synchronize
				#endif

				#ifndef CL3_OPT__IO_TEXT_ENCODING_WCHAR_CXX
					#define	CL3_OPT__IO_TEXT_ENCODING_WCHAR_CXX		CODEC_UTF32
				#endif

				#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
					#define	CL3_ENDIANITY	CL3_ENDIANITY_LITTLE
				#else
					#define	CL3_ENDIANITY	CL3_ENDIANITY_BIG
				#endif

				#define	PREFETCH(adr,rw,tloc)	__builtin_prefetch(adr,rw,tloc)
			#endif

			#if (_CXX == CXX_MSVC)
				#include <WinSock2.h>
				#include <windows.h>
				#include <intrin.h>
				#include <Specstrings.h>

				#define	THREAD	__declspec(thread)
				#define	FCONST	__declspec(noalias)
				#define	FPURE	__declspec(noalias)

				#define	CXX_EXPORT_FUNC	__declspec(dllexport)
				#define	CXX_IMPORT_FUNC	__declspec(dllimport)
				#define	CXX_EXPORT_TYPE
				#define	CXX_IMPORT_TYPE

				#define PACK( ... ) __pragma( pack(push, 1) ); __VA_ARGS__; __pragma( pack(pop) )

				#define	ATOMIC_ADD(var,add)			__msvc_atomic_add(&var,add)
				#define	ATOMIC_SUB(var,sub)			__msvc_atomic_sub(&var,sub)
				#define	ATOMIC_SWAP(var,cmp,newval)	__msvc_atomic_swap(&var,cmp,newval)
				#define	SYNC()						_ReadWriteBarrier()

				#define	warn_unused_result	__checkReturn

				//	Returns the number of leading 0-bits in x, starting at the most significant bit position.
				inline static DWORD COUNT_LEADING_ZEROES(unsigned __int32 v) { DWORD r; _BitScanReverse(&r, v); return 31-r; }
				inline static DWORD COUNT_LEADING_ZEROES(unsigned __int64 v);

				//	Returns one plus the index of the least significant 1-bit of v, or if v is zero, returns zero.
				inline static DWORD FIND_FIRST_SET(unsigned __int32 v) { DWORD r; return _BitScanReverse(&r, v) ? r : 0; }
				inline static DWORD FIND_FIRST_SET(unsigned __int64 v);

				//	Returns the number of trailing 0-bits in x, starting at the least significant bit position.
				inline static int COUNT_TRAILING_ZEROES(unsigned __int32 v) { return v == 0 ? 32 : FIND_FIRST_SET(v); }
				inline static int COUNT_TRAILING_ZEROES(unsigned __int64 v) { return v == 0 ? 64 : FIND_FIRST_SET(v); }

				//	Returns one plus the index of the most significant 1-bit of v, or if v is zero, returns zero.
				inline static int FIND_LAST_SET(unsigned __int32 v) { return 32 - COUNT_LEADING_ZEROES(v); }
				inline static int FIND_LAST_SET(unsigned __int64 v) { return 64 - COUNT_LEADING_ZEROES(v); }

				#ifdef _WIN64
					inline static DWORD COUNT_LEADING_ZEROES(unsigned __int64 v) { DWORD r; _BitScanReverse64(&r, v); return 63-r; }
					inline static DWORD FIND_FIRST_SET      (unsigned __int64 v) { DWORD r; return _BitScanReverse64(&r, v) ? r : 0; }
				#else
					inline static DWORD COUNT_LEADING_ZEROES(unsigned __int64 v)
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

					inline static DWORD FIND_FIRST_SET      (unsigned __int64 v)
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

				#define	snprintf	_snprintf
				#define alloca		_alloca
				#define	strtoull	_strtoui64

				#pragma warning(disable : 4250)	//	disable inherits via dominance warning
				#pragma warning(disable : 4996)	//	disable unsafe warning

				#ifndef CL3_OPT__IO_TEXT_ENCODING_WCHAR_CXX
					#define	CL3_OPT__IO_TEXT_ENCODING_WCHAR_CXX		CODEC_UTF16
				#endif

				#define	CL3_ENDIANITY	CL3_ENDIANITY_LITTLE

				#undef Yield
				#undef Sleep
				#undef SendMessage

				template<class T1, class T2>
				inline	T1	__msvc_atomic_add	(T1* var, T2 add)
				{
				#ifdef CL3_OPT__MULTITHREADED
					if(sizeof(T1) == 4)
						return (T1)InterlockedExchangeAdd((unsigned int*)var, (unsigned int)add);
					else if(sizeof(T1) == 8)
						return (T1)InterlockedExchangeAdd64((LONG64*)var, (LONG64)add);
				#else
					T1 tmp = *var;
					*var += add;
					return tmp;
				#endif
				}

				template<class T1, class T2>
				inline	T1	__msvc_atomic_sub	(T1* var, T2 sub)
				{
				#ifdef CL3_OPT__MULTITHREADED
					if(sizeof(T1) == 4)
						return (T1)InterlockedExchangeAdd((unsigned int*)var, (unsigned int)-sub);
					else if(sizeof(T1) == 8)
						return (T1)InterlockedExchangeAdd64((LONG64*)var, (LONG64)-sub);
				#else
					T1 tmp = *var;
					*var -= sub;
					return tmp;
				#endif
				}

				template<class T1, class T2, class T3>
				inline	T1	__msvc_atomic_swap	(T1* var, T2 cmp, T3 newval)
				{
				#ifdef CL3_OPT__MULTITHREADED
					if(sizeof(T1) == 4)
						return (T1)InterlockedCompareExchange((unsigned int*)var, (unsigned int)newval, (unsigned int)cmp);
					else if(sizeof(T1) == 8)
						return (T1)InterlockedCompareExchange64((LONG64*)var, (LONG64)newval, (LONG64)cmp);
				#else
					if(*var == cmp)
					{
						*var = newval;
						return cmp;
					}
					else
						return *var;
				#endif
				}

				#define	PREFETCH(adr,rw,tloc)	_mm_prefetch(reinterpret_cast<const char*>(adr),tloc)
			#endif

			namespace	cl2
			{
				struct	TRecurseGuard
				{
					bool& var;

					inline	CLASS	TRecurseGuard	(bool& var) : var(var)
					{
						var = true;
					}

					inline	CLASS	~TRecurseGuard	()
					{
						var = false;
					}
				};

				#define	RECURSEGUARD	static THREAD bool __recurse_guard_var = false; if(__recurse_guard_var) return;	TRecurseGuard __recurse_guard(__recurse_guard_var)
			}

			#ifdef	INSIDE_CL3
				#define	CL2PUBF	CXX_EXPORT_FUNC
				#define	CL2PUBT	CXX_EXPORT_TYPE
				#define	CL2PUBV	extern CXX_EXPORT_FUNC
			#else
				#define	CL2PUBF	CXX_IMPORT_FUNC
				#define	CL2PUBT	CXX_IMPORT_TYPE
				#define	CL2PUBV	extern CXX_IMPORT_FUNC
			#endif

			#ifndef _CXX
				#error "unknown compiler"
			#endif

			#ifndef CL3_OPT__IO_TEXT_ENCODING_CHAR_CXX
				#define	CL3_OPT__IO_TEXT_ENCODING_CHAR_CXX	CODEC_UTF8
			#endif

			#define	GETTER	warn_unused_result
			#define	SETTER
		}
	}
}

#endif
