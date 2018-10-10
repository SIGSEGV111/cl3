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
				struct is_expensive_to_copy
				{
					static const bool value = false;
				};

				template<typename T>
				void MoveItems(T* const arr_source, T* const arr_dest, const usys_t n_move);

				template<typename T>
				class TList2
				{
					public:
						using valuearg_t = typename std::conditional<(sizeof(T) <= sizeof(void*) && std::is_copy_constructible<T>::value && !is_expensive_to_copy<T>::value), const T, const T&>::type;

					protected:
						T* arr_items;
						usys_t n_items;
						usys_t n_prealloc_current;
						u16_t n_prealloc_static;
						u8_t div_prealloc;
						bool preserve_order;

					public:
						void Prealloc(const usys_t n_want);
						void MoveAppend(T&& item_append);
						void Append(valuearg_t item_append);
						void Append(const usys_t n_items_append, const T* const arr_items_append);
						void Clear();

						CLASS TList2(const usys_t n_prealloc = 0);
						CLASS TList2(const TList2&);
						CLASS TList2(TList2&&);
						CLASS ~TList2();
				};

				template<bool>
				void MoveItem(T* const item_source, T* const item_dest);

				template<bool>
				void MoveItem<true>(T* const item_source, T* const item_dest)
				{
				}

				template<bool>
				void MoveItem<false>(T* const item_source, T* const item_dest)
				{
				}

				template<typename T>
				void MoveItems(T* const arr_source, T* const arr_dest, const usys_t n_move)
				{
					if(std::is_trivially_move_constructible<T>::value)
					{
						memmove((void*)arr_dest, (void*)arr_source, n_move * sizeof(T));
					}
					else
					{
						for(usys_t i = 0; i < n_move; i++)
						{
							try
							{
								if constexpr(std::is_move_constructible<T>::value)
									new (arr_dest + i) T(system::def::move(arr_source[i]));
								else
									new (arr_dest + i) T(arr_source[i]);
							}
							catch(...)
							{
								// TODO: add annotation to error

								// rollback
								for(usys_t j = i-1; j != (usys_t)-1; j--)
								{
									if constexpr(std::is_move_constructible<T>::value)
										arr_source[j] = system::def::move(arr_dest[j]);
									arr_dest[j].~T();
								}

								throw;
							}
						}
					}
				}

				template<typename T>
				void TList2<T>::Prealloc(const usys_t n_want)
				{
					if(n_want > this->n_prealloc_current)
					{
						const usys_t n_prealloc = CL3_MAX((this->n_prealloc_static + this->n_items / this->div_prealloc), n_want);

						T* arr_items_new;

						if(std::is_trivially_move_constructible<T>::value)
							arr_items_new = (T*)realloc((void*)this->arr_items, sizeof(T) * (n_prealloc + this->n_items));
						else
							arr_items_new = (T*)malloc(sizeof(T) * (n_prealloc + this->n_items));

						CL3_CLASS_ERROR(arr_items_new == nullptr, error::TException, "out of memory during TList2 realloc (n_items: %zu, n_prealloc_current: %zu, n_want: %zu, n_prealloc: %zu, sizeof(T): %zu bytes)", this->n_items, this->n_prealloc_current, n_want, n_prealloc, sizeof(T));

						if(!std::is_trivially_move_constructible<T>::value)
						{
							try { MoveItems(this->arr_items, arr_items_new, this->n_items); }
							catch(...) { free(arr_items_new); throw; }

							free(this->arr_items);
						}

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
				CLASS TList2<T>::TList2(const usys_t n_prealloc) : arr_items(nullptr), n_items(0), n_prealloc_current(0), n_prealloc_static(128U / sizeof(T)), div_prealloc(64), preserve_order(true)
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
