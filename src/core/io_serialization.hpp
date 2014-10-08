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

#ifndef	_include_cl3_core_io_serialization_hpp_
#define	_include_cl3_core_io_serialization_hpp_

#include "system_types.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	serialization
		{
			using namespace system::types;

			struct	ISerializer;
			struct	IDeserializer;
			struct	ISerializable;

			struct	ISerializable
			{
				virtual	void	Serialize	(ISerializer&) const = 0;
				virtual	void	Deserialize	(IDeserializer& value) = 0;
			};

			struct	ISerializer
			{
				virtual	void	Push	(const char* name, u8_t value) = 0;
				virtual	void	Push	(const char* name, s8_t value) = 0;
				virtual	void	Push	(const char* name, u16_t value) = 0;
				virtual	void	Push	(const char* name, s16_t value) = 0;
				virtual	void	Push	(const char* name, u32_t value) = 0;
				virtual	void	Push	(const char* name, s32_t value) = 0;
				virtual	void	Push	(const char* name, u64_t value) = 0;
				virtual	void	Push	(const char* name, s64_t value) = 0;

				virtual	void	Push	(const char* name, f32_t value) = 0;
				virtual	void	Push	(const char* name, f64_t value) = 0;

				virtual	void	Push	(const char* name, const char* value) = 0;
				virtual	void	Push	(const char* name, const wchar_t* value) = 0;

				virtual	void	Push	(const char* name, const ISerializable& value) = 0;
			};

			struct	IDeserializer
			{
				virtual	void	Pop		(const char* name, u8_t& value) = 0;
				virtual	void	Pop		(const char* name, s8_t& value) = 0;
				virtual	void	Pop		(const char* name, u16_t& value) = 0;
				virtual	void	Pop		(const char* name, s16_t& value) = 0;
				virtual	void	Pop		(const char* name, u32_t& value) = 0;
				virtual	void	Pop		(const char* name, s32_t& value) = 0;
				virtual	void	Pop		(const char* name, u64_t& value) = 0;
				virtual	void	Pop		(const char* name, s64_t& value) = 0;

				virtual	void	Pop		(const char* name, f32_t& value) = 0;
				virtual	void	Pop		(const char* name, f64_t& value) = 0;

				virtual	void	Pop		(const char* name, const char*& value) = 0;
				virtual	void	Pop		(const char* name, const wchar_t*& value) = 0;

				virtual	void	Pop		(const char* name, ISerializable& value) = 0;
			};
		}
	}
}

#endif
