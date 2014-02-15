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

#ifndef	_include_cl3_core_io_collection_bitmask_h_
#define	_include_cl3_core_io_collection_bitmask_h_

#include "system_compiler.h"
#include "system_types.h"

namespace	cl3
{
	namespace	io
	{
		namespace	collection
		{
			namespace	bitmask
			{
				using namespace system::types;

				struct	IBitmask
				{
					virtual	GETTER	size_t	Count	() const = 0;
					virtual	GETTER	bool	Bit		(size_t index) = 0;
					virtual	SETTER	void	Bit		(size_t index, bool value) = 0;
				};

				class	TBitmask : IBitmask
				{
					protected:
						byte* arr_bits;
						size_t n_bits;

					public:
						CL3PUBF	GETTER	size_t	Count	() const;
						CL3PUBF	SETTER	void	Count	(size_t new_count);
						CL3PUBF	GETTER	bool	Bit		(size_t index);
						CL3PUBF	SETTER	void	Bit		(size_t index, bool value);

						CL3PUBF	TBitmask&	operator=	(const TBitmask&);

						CL3PUBF	CLASS	TBitmask();
						CL3PUBF	CLASS	TBitmask(const TBitmask&);
						CL3PUBF	CLASS	~TBitmask();
				};
			}
		}
	}
}

#endif
