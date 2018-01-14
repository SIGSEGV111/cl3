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

// #include <utility>

#include "system_compiler.hpp"
#include "system_types.hpp"
#include "error.hpp"
#include "context.hpp"

extern "C"
{
	CL3PUBF void* malloc(cl3::system::types::usys_t sz_bytes) throw();
	CL3PUBF void* realloc(void* p, cl3::system::types::usys_t sz_bytes_new) throw();
	CL3PUBF void free(void* p) throw();
	CL3PUBF void *calloc(cl3::system::types::usys_t n_items, cl3::system::types::usys_t sz_item) throw();
};

namespace std
{
	struct nothrow_t;
}

CL3PUBF void* operator new(size_t sz);
CL3PUBF void* operator new[](size_t sz);
CL3PUBF void* operator new(size_t sz, void* p) throw();
CL3PUBF void* operator new(size_t sz, const std::nothrow_t&) throw();
CL3PUBF void* operator new[](size_t sz, const std::nothrow_t&) throw();

CL3PUBF void* operator new(unsigned long sz);
CL3PUBF void* operator new[](unsigned long sz);
CL3PUBF void* operator new(unsigned long sz, void* p) throw();
CL3PUBF void* operator new(unsigned long sz, const std::nothrow_t&) throw();
CL3PUBF void* operator new[](unsigned long sz, const std::nothrow_t&) throw();

CL3PUBF void operator delete(void* p) throw();
CL3PUBF void operator delete[](void* p) throw();

namespace	cl3
{
	namespace	system
	{
		namespace	types
		{
			namespace	typeinfo
			{
				struct	TRTTI;

				template<typename T>
				struct	TCTTI;
			}
		}

		namespace	memory
		{
			using namespace types;

			namespace _
			{
				template<typename T>
				void Delete(void* obj)
				{
					delete reinterpret_cast<T*>(obj);
				}
			}

			struct IManaged
			{
				virtual bool BeforeDispose() = 0;
			};

			CL3PUBF	void	Free	(void*);
			CL3PUBF	void*	Alloc	(usys_t, const typeinfo::TRTTI*) CL3_WARN_UNUSED_RESULT;	//	FIXME: Alloc() should return a TUniquePtr
			CL3PUBF	void*	Realloc	(void* p_mem, usys_t n_items_new, const typeinfo::TRTTI* rtti, bool inplace) CL3_WARN_UNUSED_RESULT;
			CL3PUBF	usys_t	SizeOf	(void*) CL3_GETTER;

			template<typename T>
			T* AlignMemory(T* p, const usys_t alignment)
			{
				const usys_t d = alignment - (((usys_t)p) % alignment);
				if(d == alignment)
					return p;
				return (T*)((usys_t)p + d);
			}

			template<typename T>
			T*	Alloc	(usys_t n_items)
			{
				return reinterpret_cast<T*>(Alloc(n_items, &types::typeinfo::TCTTI<T>::rtti));
			}

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

						this->object = (T*)0xDEADBEAF;
					}

				public:
					TUniquePtr&	operator=	(TUniquePtr&& rhs)
					{
						CL3_CLASS_LOGIC_ERROR(this == &rhs);
						CL3_CLASS_LOGIC_ERROR(this->object != NULL && this->object == rhs.object);
						this->Release();
						this->object = rhs.object;
						rhs.object = NULL;
						return *this;
					}

					void		Object		(T* new_object) CL3_SETTER
					{
						CL3_CLASS_LOGIC_ERROR(this->object != NULL && this->object == new_object);
						this->Release();
						this->object = new_object;
					}

					const T*	Object		() const CL3_GETTER { return this->object; }
					T*			Object		() CL3_GETTER { return this->object; }

					T*			operator->	() CL3_GETTER { return this->object; }
					T&			operator*	() CL3_GETTER { return *this->object; }
					const T*	operator->	() const CL3_GETTER { return this->object; }
					const T&	operator*	() const CL3_GETTER { return *this->object; }

					bool		operator==	(const TUniquePtr& rhs) const CL3_GETTER { return this->object == rhs.object; }
					bool		operator!=	(const TUniquePtr& rhs) const CL3_GETTER { return this->object != rhs.object; }
					bool		operator==	(const T* rhs) const CL3_GETTER { return this->object == rhs; }
					bool		operator!=	(const T* rhs) const CL3_GETTER { return this->object != rhs; }

					T*			Claim	() CL3_GETTER { T* tmp = this->object; this->object = NULL; return tmp; }
					void		Reset	() { this->object = NULL; }	//	does *NOT* release any memory - just removes control of the object from this class

					T*	AtomicSwap	(const T* compare, T* new_value)
					{
						return compiler::AtomicSwap(this->object, (T*)compare, new_value);
					}

					T*	AtomicSwap	(const T* compare, TUniquePtr new_value)
					{
						T* v = compiler::AtomicSwap(this->object, (T*)compare, new_value.Object());
						if(v == compare)
							new_value.Reset();
						return v;
					}

					CLASS	TUniquePtr	() throw() : object(NULL)
					{
					}

					CLASS	TUniquePtr	(TUniquePtr&& rhs) throw() : object(rhs.object)
					{
						rhs.object = NULL;
					}

					CLASS	~TUniquePtr	() { this->Release(); }
			};

			template<class T> CL3_GETTER static TUniquePtr<T, UPTR_OBJECT> MakeUniquePtr(T* object)
			{
				TUniquePtr<T, UPTR_OBJECT> uptr;
				uptr.Object(object);
				return system::def::move(uptr);
			}

			template<EUnqiuePtrType type, class T> CL3_GETTER static TUniquePtr<T, type> MakeUniquePtr(T* object)
			{
				TUniquePtr<T, type> uptr; uptr.Object(object); return uptr;
			}

			enum	EThreading
			{
				THREADING_SINGLE,
				THREADING_MULTI
			};

			struct TSharedInfo
			{
				unsigned n_refs;
				void* obj;
				void (*FDelete)(void*);

				inline void DecRef()
				{
					CL3_CLASS_LOGIC_ERROR(this->n_refs < 1);
					if(compiler::AtomicSub(this->n_refs, 1U) == 1)
					{
						this->FDelete(this->obj);
						delete this;
					}
				}

				inline void IncRef()
				{
					compiler::AtomicAdd(this->n_refs, 1U);
				}

				inline CLASS TSharedInfo(void* obj, void (*FDelete)(void*)) : n_refs(0), obj(obj), FDelete(FDelete)
				{
				}
			};

			template<typename T>
			class	CL3PUBT	TSharedPtr
			{
				protected:
					template<typename U>
					friend class TSharedPtr;

					T* obj;
					TSharedInfo* info;

					void Set(T* new_obj, TSharedInfo* new_info)
					{
						this->obj = new_obj;
						if(this->info != new_info)
						{
							if(this->info != NULL)
								this->info->DecRef();
							this->info = new_info;
							if(this->info)
								this->info->IncRef();
						}
					}

				public:
					inline	T*			operator->	() { return this->obj; }
					inline	const T*	operator->	() const { return this->obj; }
					inline	T&			operator*	() { return *this->obj; }
					inline	const T&	operator*	() const { return *this->obj; }

					inline	TSharedPtr&	operator=	(const TSharedPtr& rhs)
					{
						Set(rhs.obj, rhs.info);
						return *this;
					}

					inline	TSharedPtr&	operator=	(TSharedPtr&& rhs)
					{
						Clear();
						this->obj = rhs.obj;
						this->info = rhs.info;
						rhs.obj = NULL;
						rhs.info = NULL;
						return *this;
					}

					bool		operator==	(const TSharedPtr& rhs) const CL3_GETTER { return this->obj == rhs.obj; }
					bool		operator!=	(const TSharedPtr& rhs) const CL3_GETTER { return this->obj != rhs.obj; }
					bool		operator==	(const T* rhs) const CL3_GETTER { return this->obj == rhs; }
					bool		operator!=	(const T* rhs) const CL3_GETTER { return this->obj != rhs; }

					inline	T*			Object()
					{
						return this->obj;
					}

					inline	const T*	Object() const
					{
						return this->obj;
					}

					template<typename TT>
					CLASS	TSharedPtr	(TSharedPtr<TT>&& rhs) : obj(rhs.obj), info(rhs.info)
					{
						rhs.obj = NULL;
						rhs.info = NULL;
					}

					CLASS	TSharedPtr	(TSharedPtr&& rhs) : obj(rhs.obj), info(rhs.info)
					{
						rhs.obj = NULL;
						rhs.info = NULL;
					}

					template<typename TT>
					CLASS	TSharedPtr	(const TSharedPtr<TT>& rhs) : obj(NULL), info(NULL)
					{
						Set(rhs.obj, rhs.info);
					}

					CLASS	TSharedPtr	(const TSharedPtr& rhs) : obj(NULL), info(NULL)
					{
						Set(rhs.obj, rhs.info);
					}

					void	Clear		()
					{
						Set(NULL,NULL);
					}

					CLASS	TSharedPtr	() : obj(NULL), info(NULL) {}

					CLASS	TSharedPtr	(T* obj, TSharedInfo* info) : obj(NULL), info(NULL)
					{
						Set(obj, info);
					}

					CLASS	~TSharedPtr	()
					{
						Set(NULL,NULL);
					}
			};

			template<typename T>
			TSharedPtr<T>	MakeSharedPtr	(T* obj)
			{
				return TSharedPtr<T>(obj, new TSharedInfo(obj, &_::Delete<T>));
			}

			class	CL3PUBT	TBadAllocException : public error::TException
			{
				protected:
					usys_t sz_bytes;

				public:
					CL3PUBF	CLASS	TBadAllocException	(usys_t sz_bytes);
					CL3PUBF	CLASS	TBadAllocException	(TBadAllocException&&);
					CL3PUBF	virtual	~TBadAllocException	();
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

			CL3_CONTEXT_VARIABLE_DECL(IDynamicAllocator*, allocator_generic);
			CL3_CONTEXT_VARIABLE_DECL(IDynamicAllocator*, allocator_exception);
		}
	}
}

#endif
