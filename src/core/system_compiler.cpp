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

#include "system_compiler.hpp"
// #include "system_memory.hpp"
//
// #include <unistd.h>
// #include <malloc.h>
// #include <signal.h>
// #include <string.h>

// static bool hooks_hot = false;	//	FIXME

// namespace
// {
// 	typedef void* (*FMallocHook)(size_t size, const void *caller);
// 	typedef void* (*FReallocHook)(void *ptr, size_t size, const void *caller);
// 	typedef void  (*FFreeHook)(void *ptr, const void *caller);
// 	typedef void* (*FMemalignHook)(size_t alignment, size_t size, const void *caller);
//
// 	FMallocHook malloc_old;
// 	FReallocHook realloc_old;
// 	FFreeHook free_old;
// 	FMemalignHook memalign_old;
//
// 	static void* my_malloc(size_t size, const void* caller)
// 	{
// // 		__malloc_hook = malloc_old;
// 		void* v = NULL;
// 		try { v = cl3::system::memory::Alloc(size, NULL); }
// 		catch(...) {}
// // 		malloc_old = __malloc_hook;
// 		__malloc_hook = my_malloc;
// 		return v;
// 	}
//
// 	static void* my_realloc(void* ptr, size_t size, const void* caller)
// 	{
// //  		__realloc_hook = realloc_old;
// 		void* v = NULL;
// 		if(sbrk(0) > ptr)
// 		{
// 			try { v = cl3::system::memory::Alloc(size, NULL); }
// 			catch(...) {}
//
// 			if(v != NULL)
// 			{
// 				memcpy(v, ptr, CL3_MIN(malloc_usable_size(ptr), size));
// 				free(ptr);
// 			}
// 		}
// 		else
// 			try { v = cl3::system::memory::Realloc(ptr, size, NULL, false); }
// 			catch(...) {}
// //  		realloc_old = __realloc_hook;
//  		__realloc_hook = my_realloc;
// 		return v;
// 	}
//
// 	static void my_free(void* ptr, const void* caller)
// 	{
//  		__free_hook = free_old;
// 		if(sbrk(0) > ptr)
// 			::free(ptr);
// 		else
// 			try { cl3::system::memory::Free(ptr); }
// 			catch(...) {}
//  		free_old = __free_hook;
// 		__free_hook = my_free;
// 	}
//
// 	static void* my_memalign(size_t alignment, size_t size, const void* caller)
// 	{
// 		return my_malloc(size, caller);
// 	}
//
// 	class TMallocHook
// 	{
// 		public:
// 			TMallocHook()
// 			{
// 				hooks_hot = true;
//
// 				malloc_old   = __malloc_hook;
// 				realloc_old  = __realloc_hook;
// 				free_old     = __free_hook;
// 				memalign_old = __memalign_hook;
//
// 				__malloc_hook   = my_malloc;
// 				__realloc_hook  = my_realloc;
// 				__free_hook     = my_free;
// 				__memalign_hook = my_memalign;
// 			}
//
// 			~TMallocHook()
// 			{
// // 				__malloc_hook   = malloc_old;
// // 				__realloc_hook  = realloc_old;
// // 				__free_hook     = free_old;
// // 				__memalign_hook = memalign_old;
// 			}
// 	};
//
// 	static TMallocHook hooker;
// }
