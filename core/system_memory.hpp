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

#ifndef	_include_cl3_core_system_memory_hpp_
#define	_include_cl3_core_system_memory_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "error.hpp"
#include "context.hpp"

extern "C" void free(void*) throw();

namespace	cl3
{
	namespace	system
	{
		namespace	types
		{
			namespace	typeinfo
			{
				struct	TRTTI;
			}
		}

		namespace	memory
		{
			using namespace types;

			CL3PUBF	void	Free	(void*);
			CL3PUBF	void*	Alloc	(usys_t, const typeinfo::TRTTI*) CL3_WARN_UNUSED_RESULT;
			CL3PUBF	void*	Realloc	(void* p_mem, usys_t n_items_new, const typeinfo::TRTTI* rtti, bool inplace) CL3_WARN_UNUSED_RESULT;
			CL3PUBF	usys_t	SizeOf	(void*) CL3_GETTER;

			enum	EUnqiuePtrType
			{
				UPTR_OBJECT,
				UPTR_ARRAY,
				UPTR_ALLOC,
				UPTR_MALLOC
			};

			template<class T, EUnqiuePtrType type = UPTR_OBJECT>
			class	CL3PUBT	TUniquePtr
			{
				private:
					TUniquePtr&	operator=	(const TUniquePtr&) = delete;
					CLASS	TUniquePtr	(const TUniquePtr&) = delete;
					CLASS	TUniquePtr	(T* object) throw() : object(object) {}

				protected:
					T* object;

					void	Release	()
					{
						switch(type)
						{
							case UPTR_OBJECT:
								delete object;
								break;
							case UPTR_ARRAY:
								delete[] object;
								break;
							case UPTR_ALLOC:
								cl3::system::memory::Free((void*)object);
								break;
							case UPTR_MALLOC:
								::free((void*)object);
								break;
						};
					}

				public:
					TUniquePtr&	operator=	(TUniquePtr&& rhs)
					{
						this->Release();
						this->object = rhs.object;
						rhs.object = NULL;
						return *this;
					}

					void		Object		(T* new_object) CL3_SETTER
					{
						this->Release();
						this->object = new_object;
					}

					const T*	Object		() const CL3_GETTER { return this->object; }
					T*			Object		() CL3_GETTER { return this->object; }

					T*			operator->	() CL3_GETTER { return this->object; }
					T&			operator*	() CL3_GETTER { return *this->object; }
					const T*	operator->	() const CL3_GETTER { return this->object; }
					const T&	operator*	() const CL3_GETTER { return *this->object; }

					T*			Claim	() CL3_GETTER { T* tmp = this->object; this->object = NULL; return tmp; }
					void		Reset	() { this->object = NULL; }	//	does *NOT* release any memory - just removes control of the object from this class

					T*	AtomicSwap	(const T* compare, T* new_value)
					{
						return compiler::AtomicSwap(this->object, (T*)compare, new_value);
					}

					CLASS	TUniquePtr	() throw() : object(NULL) {}
					CLASS	TUniquePtr	(TUniquePtr&& rhs) throw() : object(rhs.object) { rhs.object = NULL; }
					CLASS	~TUniquePtr	() { this->Release(); }
			};

			template<class T> CL3_GETTER static TUniquePtr<T, UPTR_OBJECT> MakeUniquePtr(T* object) { TUniquePtr<T, UPTR_OBJECT> uptr; uptr.Object(object); return uptr; }
			template<EUnqiuePtrType type, class T> CL3_GETTER static TUniquePtr<T, type> MakeUniquePtr(T* object) { TUniquePtr<T, type> uptr; uptr.Object(object); return uptr; }

			enum	EThreading
			{
				THREADING_SINGLE,
				THREADING_MULTI
			};

			template<class T, EThreading> class TSharedPtr;

			template<class T, EThreading th = THREADING_MULTI>
			class	CL3PUBT	TShared
			{
				friend class TSharedPtr<T,th>;
				friend class TUniquePtr< TShared<T,th>, UPTR_OBJECT>;
				private:
					mutable usys_t n_refs;
					T object;

					void	IncRef	() const
					{
						switch(th)
						{
							case THREADING_SINGLE:
								++n_refs;
								break;
							case THREADING_MULTI:
								system::compiler::AtomicAdd<usys_t>(n_refs, 1);
								break;
						}
					}

					void	DecRef	() const
					{
						usys_t n_refs_now;
						switch(th)
						{
							case THREADING_SINGLE:
								n_refs_now = n_refs--;
								break;
							case THREADING_MULTI:
								n_refs_now = system::compiler::AtomicSub<usys_t>(n_refs, 1);
								break;
						}
						if(n_refs_now == 1)
							delete this;
					}

					CLASS	~TShared() {}
					CLASS	TShared	(const TShared&);

				public:
					CLASS	TShared	() : n_refs(0), object() {}
					template<class A1>
					CLASS	TShared(A1 a1) : n_refs(0), object(a1) {}
					template<class A1, class A2>
					CLASS	TShared(A1 a1, A2 a2) : n_refs(0), object(a1, a2) {}
					template<class A1, class A2, class A3>
					CLASS	TShared(A1 a1, A2 a2, A3 a3) : n_refs(0), object(a1, a2, a3) {}
					template<class A1, class A2, class A3, class A4>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4) : n_refs(0), object(a1, a2, a3, a4) {}
					template<class A1, class A2, class A3, class A4, class A5>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) : n_refs(0), object(a1, a2, a3, a4, a5) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) : n_refs(0), object(a1, a2, a3, a4, a5, a6) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8, a9) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) {}
					template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15>
					CLASS	TShared(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14, A15 a15) : n_refs(0), object(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) {}
			};

			template<class T, EThreading th = THREADING_MULTI>
			class	CL3PUBT	TSharedPtr
			{
				protected:
					TShared<T, th>* rcobj;

				public:
					inline	T*			operator->	() { return &rcobj->object; }
					inline	const T*	operator->	() const { return &rcobj->object; }
					inline	T&			operator*	() { return rcobj->object; }
					inline	const T&	operator*	() const { return rcobj->object; }
					inline	TSharedPtr&	operator=	(const TSharedPtr& rhs) { if(rcobj != rhs.rcobj) { if(rcobj) rcobj->DecRef(); rcobj = rhs.rcobj; if(rcobj) rcobj->IncRef(); } return *this; }
					inline	TSharedPtr&	operator=	(TShared<T,th>* _rcobj) { if(rcobj != _rcobj) { if(rcobj) rcobj->DecRef(); rcobj = _rcobj; if(rcobj) rcobj->IncRef(); } return *this; }

					CLASS	TSharedPtr	(TSharedPtr&& rhs) : rcobj(rhs.rcobj)			{ rhs.rcobj = NULL; }
					CLASS	TSharedPtr	(TShared<T,th>* rcobj = NULL) : rcobj(rcobj)	{ if(rcobj) rcobj->IncRef(); }
					CLASS	TSharedPtr	(const TSharedPtr& rhs) : rcobj(rhs.rcobj)		{ if(rcobj) rcobj->IncRef(); }
					CLASS	~TSharedPtr	() 												{ if(rcobj) rcobj->DecRef(); }
			};

			template<class T, EThreading th> static TSharedPtr<T,th> MakeSharedPtr() { return TSharedPtr<T,th>(new TShared<T,th>()); }
			template<class T, EThreading th, class A1> static TSharedPtr<T,th> MakeSharedPtr(A1 a1) { return TSharedPtr<T,th>(new TShared<T,th>(a1)); }
			template<class T, EThreading th, class A1, class A2> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2)); }
			template<class T, EThreading th, class A1, class A2, class A3> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8, a9)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)); }
			template<class T, EThreading th, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15> static TSharedPtr<T,th> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14, A15 a15) { return TSharedPtr<T,th>(new TShared<T,th>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)); }

			class	CL3PUBT	TBadAllocException : public error::TException
			{
				protected:
					usys_t sz_bytes;

				public:
					CL3PUBF	CLASS	TBadAllocException	(usys_t sz_bytes);
					CL3PUBF	CLASS	TBadAllocException	(const TBadAllocException&);
					CL3PUBF	virtual	~TBadAllocException	();
			};

			class	CL3PUBT	TDirtyAllocatorException : public error::TException
			{
				protected:
					usys_t sz_bytes;

				public:
					CL3PUBF	CLASS	TDirtyAllocatorException	(usys_t sz_bytes);
					CL3PUBF	CLASS	TDirtyAllocatorException	(const TDirtyAllocatorException&);
					CL3PUBF	virtual	~TDirtyAllocatorException	();
			};

			struct	CL3PUBT	IDynamicAllocator
			{
				virtual	void*	Alloc	(usys_t sz_bytes) CL3_WARN_UNUSED_RESULT = 0;
				virtual	void	Free	(void* p_mem) = 0;
				virtual	void*	Realloc	(void* p_mem, usys_t sz_bytes_new, bool inplace) CL3_WARN_UNUSED_RESULT = 0;
				virtual	usys_t	SizeOf	(void* p_mem) const CL3_GETTER = 0;
			};

			class	CL3PUBT	TRestrictAllocator : public IDynamicAllocator
			{
				private:
					CLASS	TRestrictAllocator	(const TRestrictAllocator&);

				protected:
					IDynamicAllocator* allocator;
					usys_t sz_limit;
					usys_t sz_current;

				public:
					CL3PUBF	void*	Alloc	(usys_t sz_bytes) CL3_WARN_UNUSED_RESULT;
					CL3PUBF	void	Free	(void* p_mem);
					CL3PUBF	void*	Realloc	(void* p_mem, usys_t sz_bytes_new, bool inplace) CL3_WARN_UNUSED_RESULT;
					CL3PUBF	usys_t	SizeOf	(void* p_mem) const CL3_GETTER;
					CL3PUBF	usys_t	BytesAllocated	() const CL3_GETTER;
					CL3PUBF	usys_t	BytesLimit		() const CL3_GETTER;

					CL3PUBF	CLASS	TRestrictAllocator	(IDynamicAllocator* allocator, usys_t sz_limit);
					CL3PUBF	CLASS	~TRestrictAllocator	();
			};

			CL3_PARAMETER_STACK_DECL(IDynamicAllocator*, allocator);
			CL3PUBF extern IDynamicAllocator* exception_allocator;
		}
	}
}

#endif
