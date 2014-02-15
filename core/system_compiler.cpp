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

#include "system_compiler.h"
#include "system_memory.h"
#include "system_context.h"

using namespace cl3::system::memory;
using namespace cl3::system::context;

void* operator new(size_t sz)   { return OptionStack<IDynamicAllocator>().Current()->Alloc(sz); }
void* operator new[](size_t sz) { return OptionStack<IDynamicAllocator>().Current()->Alloc(sz); }

void operator delete(void* ptr)   { return OptionStack<IDynamicAllocator>().Current()->Free(ptr); }
void operator delete[](void* ptr) { return OptionStack<IDynamicAllocator>().Current()->Free(ptr); }
