/*
 *	 libcl3 - common library version 3
 *	 Copyright (C) 2013	Simon Brennecke
 *
 *	 This program is free software: you can redistribute it and/or modify
 *	 it under the terms of the GNU General Public License as published by
 *	 the Free Software Foundation, either version 3 of the License, or
 *	 (at your option) any later version.
 *
 *	 This program is distributed in the hope that it will be useful,
 *	 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *
 *	 You should have received a copy of the GNU General Public License
 *	 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	_include_cl3_core_io_format_json_hpp_
#define	_include_cl3_core_io_format_json_hpp_

#include "system_types.hpp"
#include "io_format.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	format
		{
			namespace	json
			{
				CL3PUBF void SerializeJSON(const TValue& root, io::text::ITextWriter&);
				CL3PUBF TValue DeserializeJSON(io::text::ITextReader&);
			}
		}
	}
}

#endif
