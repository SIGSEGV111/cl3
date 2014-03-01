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

#ifndef _include_cl3_core_system_memory_gc_h_
#define	_include_cl3_core_system_memory_gc_h_

#include "system_compiler.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	memory
		{
			namespace	gc
			{
				namespace	_
				{
					typedef void(*TFDestruct)(void*);

					template<class T> static void FDestruct(void* object) { reinterpret_cast<T*>(object)->~T(); }

					struct	TTypeInfo
					{
						size_t size;
						TFDestruct f_destruct;

						CLASS	TTypeInfo	(size_t size, TFDestruct f_destruct) : size(size), f_destruct(f_destruct) {}

						template<class T>
						static	TTypeInfo	Make	() { return TTypeInfo(sizeof(T), &FDestruct<T>); }
					};
				}

				class	TBasePtr
				{
					public:
						void* object;

					private:
						#ifdef CL3_EXPORT_TLS
							static CL3PUBF CL3_THREAD TBasePtr* last;
						#endif
						TBasePtr* prev;

						CL3PUBF	void	RemoveFromChain	() throw();

					protected:
						CLASS	TBasePtr	(void* object) throw();
						CLASS	TBasePtr	(const TBasePtr& other) throw();
						CLASS	~TBasePtr	() throw();
				};

				#ifdef CL3_EXPORT_TLS
					CLASS	TBasePtr::TBasePtr	(void* object) throw() : object(object), prev(last) { last = this; }
					CLASS	TBasePtr::TBasePtr	(const TBasePtr& other) throw() : object(other.object), prev(last) { last = this; }
					CLASS	TBasePtr::~TBasePtr	() throw() { if(last == this) last = prev; else RemoveFromChain(); }
				#endif

				CL3PUBF	void	TBasePtr::RemoveFromChain	() throw()
				{
					TBasePtr* p = NULL;
					for(TBasePtr* c = last; c != NULL; c = c->prev)
					{
						if(c == this)
						{
							if(p == NULL)
								last = prev;
							else
								p->prev = prev;
							break;
						}
						p = c;
					}
				}

				template<class T>
				class	TGCPtr : protected TBasePtr
				{
					protected:

					public:

						CLASS	TGCPtr	(T* object) throw() : TBasePtr(object) {}
						CLASS	TGCPtr	(const TGCPtr& other) throw() : TBasePtr(other) {}
						CLASS	~TGCPtr	() {}
				};

				CL3PUBF	void*	gc_new	(const _::TTypeInfo& ti, size_t n_items);

				template<class T>
				inline TGCPtr<T> gc_new	(size_t n_items) { return reinterpret_cast<T*>(gc_new(_::TTypeInfo::Make<T>(), n_items)); }
			}
		}
	}
}

#endif	//	_include_cl3_core_system_memory_gc_h_
