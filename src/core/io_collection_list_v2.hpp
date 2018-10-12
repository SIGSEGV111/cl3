#ifndef	_include_cl3_core_io_collection_list_v2_hpp_
#define	_include_cl3_core_io_collection_list_v2_hpp_

#include "error.hpp"
#include <type_traits>

namespace cl3
{
	namespace io
	{
		namespace collection
		{
			namespace list_v2
			{
				template<typename T>
				void MoveItems(T* const arr_source, T* const arr_dest, const usys_t n_move);

				// NOTE: only accepts datatypes which are trivially moveable!

				template<typename T>
				class TList2
				{
					public:
						using valuearg_t = typename std::conditional<(sizeof(T) <= sizeof(void*) && std::is_trivially_copyable<T>::value), const T, const T&>::type;

					protected:
						T* arr_items;
						usys_t n_items;
						usys_t n_prealloc_current;
						u16_t n_prealloc_static;
						u8_t div_prealloc;
						bool preserve_order;

						inline usys_t NormalizeIndex(const ssys_t index) const CL3_GETTER;

					public:
						void Prealloc(const usys_t n_want);
						void MoveAppend(T&& item_append);
						void Append(valuearg_t item_append);
						void Append(const usys_t n_items_append, const T* const arr_items_append);
						void Clear();

						inline usys_t Count() const CL3_GETTER { return this->n_items; }
						inline T& operator[](const ssys_t index) CL3_GETTER;
						inline valuearg_t operator[](const ssys_t index) const CL3_GETTER;

						TList2& operator=(const TList2&);
						TList2& operator=(TList2&&);

						CLASS TList2(const usys_t n_prealloc = 0, const u16_t n_prealloc_static = 128U / sizeof(T), const u8_t div_prealloc = 64U, const bool preserve_order = true);
						CLASS TList2(const TList2&);
						CLASS TList2(TList2&&);
						CLASS ~TList2();
				};

				template<typename T>
				usys_t TList2<T>::NormalizeIndex(const ssys_t index) const
				{
					if(CL3_LIKELY(index >= 0))
						return index;
					else
						return this->n_items - index;
				}

				template<typename T>
				void TList2<T>::Prealloc(const usys_t n_want)
				{
					if(n_want > this->n_prealloc_current)
					{
						const usys_t n_prealloc = CL3_MAX((this->n_prealloc_static + this->n_items / this->div_prealloc), n_want);

						T* const arr_items_new = (T*)realloc((void*)this->arr_items, sizeof(T) * (n_prealloc + this->n_items));
						CL3_CLASS_ERROR(arr_items_new == nullptr, error::TException, "out of memory during TList2 realloc (n_items: %zu, n_prealloc_current: %zu, n_want: %zu, n_prealloc: %zu, sizeof(T): %zu bytes)", this->n_items, this->n_prealloc_current, n_want, n_prealloc, sizeof(T));

						this->arr_items = arr_items_new;
						this->n_prealloc_current = n_prealloc;
					}
				}

				template<typename T>
				void TList2<T>::MoveAppend(T&& item_append)
				{
					this->Prealloc(1);
					new (this->arr_items + this->n_items) T(system::def::move(item_append));
					this->n_items++;
					this->n_prealloc_current--;
				}

				template<typename T>
				void TList2<T>::Append(valuearg_t item_append)
				{
					this->Prealloc(1);
					new (this->arr_items + this->n_items) T(system::def::move(item_append));
					this->n_items++;
					this->n_prealloc_current--;
				}

				template<typename T>
				void TList2<T>::Append(const usys_t n_items_append, const T* const arr_items_append)
				{
					this->Prealloc(n_items_append);
					for(usys_t i = 0; i < n_items_append; i++)
						try
						{
							new (this->arr_items + this->n_items + i) T(arr_items_append[i]);
						}
						catch(...)
						{
							// rollback
							for(usys_t j = i-1; j != (usys_t)-1; j--)
								this->arr_items[j].~T();
							throw;
						}

					this->n_items += n_items_append;
					this->n_prealloc_current -=  n_items_append;
				}

				template<typename T>
				void TList2<T>::Clear()
				{
					for(usys_t i = 0; i < this->n_items; i++)
						this->arr_items[i].~T();

					free(this->arr_items);

					this->arr_items = nullptr;
					this->n_items = 0;
					this->n_prealloc_current = 0;
				}

				template<typename T>
				T& TList2<T>::operator[](const ssys_t index)
				{
					const usys_t _index = this->NormalizeIndex(index);
					return this->arr_items[_index];
				}

				template<typename T>
				typename TList2<T>::valuearg_t TList2<T>::operator[](const ssys_t index) const
				{
					const usys_t _index = this->NormalizeIndex(index);
					return this->arr_items[_index];
				}

				template<typename T>
				TList2<T>& TList2<T>::operator=(const TList2& rhs)
				{
					this->Clear();
					this->Prealloc(rhs.Count());

					usys_t i = 0;
					try
					{
						for(; i < rhs.Count(); i++)
							new (this->arr_items + i) T(rhs.arr_items[i]);
					}
					catch(...)
					{
						for(i--; i != (usys_t)-1; i--)
							this->arr_items[i].~T();

						free(this->arr_items);
						this->arr_items = nullptr;
						throw;
					}

					this->n_items = rhs.n_items;
					return *this;
				}

				template<typename T>
				TList2<T>& TList2<T>::operator=(TList2&& rhs)
				{
					this->Clear();
					this->arr_items = rhs.arr_items;
					this->n_items = rhs.n_items;
					this->n_prealloc_current = rhs.n_prealloc_current;
					this->n_prealloc_static = rhs.n_prealloc_static;
					this->div_prealloc = rhs.div_prealloc;
					this->preserve_order = rhs.preserve_order;
					rhs.arr_items = nullptr;
					rhs.n_items = 0;
					rhs.n_prealloc_current = 0;
					return *this;
				}

				template<typename T>
				CLASS TList2<T>::TList2(const usys_t n_prealloc, const u16_t n_prealloc_static, const u8_t div_prealloc, const bool preserve_order) : arr_items(nullptr), n_items(0), n_prealloc_current(0), n_prealloc_static(n_prealloc_static), div_prealloc(div_prealloc), preserve_order(preserve_order)
				{
					if(n_prealloc > 0)
						this->Prealloc(n_prealloc);
				}

				template<typename T>
				CLASS TList2<T>::TList2(const TList2& other) : arr_items(nullptr), n_items(0), n_prealloc_current(0)
				{
					Prealloc(other.n_items);

					for(usys_t i = 0; i < other.n_items; i++)
						try
						{
							new (this->arr_items + this->n_items + i) T(system::def::move(other.arr_items[i]));
						}
						catch(...)
						{
							// TODO: add annotation to error

							// rollback
							for(usys_t j = i-1; j != (usys_t)-1; j--)
								this->arr_items[j].~T();
							throw;
						}

					this->n_items += other.n_items;
					this->n_prealloc_current -=  other.n_items;
				}

				template<typename T>
				CLASS TList2<T>::TList2(TList2&& other) : arr_items(other.arr_items), n_items(other.n_items), n_prealloc_current(other.n_prealloc_current)
				{
					other.arr_items = nullptr;
					other.n_items = 0;
					other.n_prealloc_current = 0;
				}

				template<typename T>
				CLASS TList2<T>::~TList2()
				{
					this->Clear();
				}
			}
		}
	}
}

#endif