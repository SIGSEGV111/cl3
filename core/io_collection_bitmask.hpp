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

#ifndef	_include_cl3_core_io_collection_bitmask_hpp_
#define	_include_cl3_core_io_collection_bitmask_hpp_

#include "system_compiler.hpp"
#include "system_types.hpp"

namespace	cl3
{
	using namespace system::types;

	namespace	io
	{
		namespace	collection
		{
			namespace	bitmask
			{
				struct	IBitmask
				{
					virtual	usys_t	Count	() const GETTER = 0;
					virtual	bool	Bit		(usys_t index) GETTER = 0;
					virtual	void	Bit		(usys_t index, bool value) SETTER = 0;
				};

				class	TBitmask : IBitmask
				{
					protected:
						byte_t* arr_bits;
						usys_t n_bits;

					public:
						CL3PUBF	usys_t	Count	() const GETTER;
						CL3PUBF	void	Count	(usys_t new_count) SETTER;
						CL3PUBF	bool	Bit		(usys_t index) GETTER;
						CL3PUBF	void	Bit		(usys_t index, bool value) SETTER;

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
