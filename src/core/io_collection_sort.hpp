#include "system_random.hpp"
#include "system_types.hpp"

namespace cl3
{
	namespace io
	{
		namespace collection
		{
			namespace sort
			{
				using namespace system::types;

				// from https://de.wikipedia.org/wiki/Quicksort

				namespace _
				{
					template<typename L>
					static inline void Swap(typename L::value_t& a, typename L::value_t& b)
					{
						typename L::value_t tmp = system::def::move(a);
						a = system::def::move(b);
						b = system::def::move(tmp);
					}

					template<typename L, typename C>
					usys_t QuickSortDivide(L& array, C f, const ssys_t left, const ssys_t right)
					{
						ssys_t i = left;
						ssys_t j = right;
						const usys_t n = right - left + 1;
						ssys_t pivot = left + (system::random::TXorShift::SharedInstance().GenerateU64() % n);

						do
						{
							while(f(array[i], array[pivot]) < 0 && i < right)
								i++;

							while(f(array[j], array[pivot]) >= 0 && j > left)
								j--;

							if(i < j)
							{
								Swap<L>(array[j], array[i]);
								if(i == pivot)
									pivot = j;
								else if(j == pivot)
									pivot = i;
							}
						}
						while(i < j);

						if(f(array[i], array[pivot]) > 0)
							Swap<L>(array[i], array[pivot]);

						return i;
					}

					template<typename L, typename C>
					void QuickSort(L& array, C f, const ssys_t left, const ssys_t right)
					{
						if(left < right)
						{
							const usys_t pivot = QuickSortDivide(array, f, left, right);
							QuickSort(array, f, left, pivot-1);
							QuickSort(array, f, pivot+1, right);
						}
					}
				}

				template<typename L, typename C>
				void QuickSort(L& array, C f)
				{
					_::QuickSort(array, f, 0, array.Count()-1);
				}
			}
		}
	}
}
