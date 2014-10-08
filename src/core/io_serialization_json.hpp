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

#ifndef	_include_cl3_core_io_serialization_json_hpp_
#define	_include_cl3_core_io_serialization_json_hpp_

#include "io_serialization.hpp"
#include "io_text.hpp"

namespace	cl3
{
	namespace	io
	{
		namespace	serialization
		{
			namespace	json
			{
				class	TJSONSerializer : public ISerializer
				{
					private:
						CLASS	TJSONSerializer	(const TJSONSerializer&) = delete;
						io::text::ITextWriter* tw;
						bool pretty;
						bool empty_scope;
						usys_t depth;

						void	BeginNewElement	(const char* name);

					public:
						CL3PUBF	void	Push	(const char*, u8_t) final override;
						CL3PUBF	void	Push	(const char*, s8_t) final override;
						CL3PUBF	void	Push	(const char*, u16_t) final override;
						CL3PUBF	void	Push	(const char*, s16_t) final override;
						CL3PUBF	void	Push	(const char*, u32_t) final override;
						CL3PUBF	void	Push	(const char*, s32_t) final override;
						CL3PUBF	void	Push	(const char*, u64_t) final override;
						CL3PUBF	void	Push	(const char*, s64_t) final override;

						CL3PUBF	void	Push	(const char*, f32_t) final override;
						CL3PUBF	void	Push	(const char*, f64_t) final override;

						CL3PUBF	void	Push	(const char*, const char*) final override;
						CL3PUBF	void	Push	(const char*, const wchar_t*) final override;

						CL3PUBF	void	Push	(const char*, const ISerializable&) final override;

						CL3PUBF	CLASS	TJSONSerializer		(io::text::ITextWriter*, bool pretty = false);
						CL3PUBF	CLASS	~TJSONSerializer	();
				};

				class	TJSONDeserializer : public IDeserializer
				{
					private:
						CLASS	TJSONDeserializer	(const TJSONDeserializer&) = delete;
						io::text::ITextReader* tr;

					public:
						CL3PUBF	void	Pop		(const char*, u8_t&) final override;
						CL3PUBF	void	Pop		(const char*, s8_t&) final override;
						CL3PUBF	void	Pop		(const char*, u16_t&) final override;
						CL3PUBF	void	Pop		(const char*, s16_t&) final override;
						CL3PUBF	void	Pop		(const char*, u32_t&) final override;
						CL3PUBF	void	Pop		(const char*, s32_t&) final override;
						CL3PUBF	void	Pop		(const char*, u64_t&) final override;
						CL3PUBF	void	Pop		(const char*, s64_t&) final override;

						CL3PUBF	void	Pop		(const char*, f32_t&) final override;
						CL3PUBF	void	Pop		(const char*, f64_t&) final override;

						CL3PUBF	void	Pop		(const char*, const char*&) final override;
						CL3PUBF	void	Pop		(const char*, const wchar_t*&) final override;

						CL3PUBF	void	Pop		(const char*, ISerializable&) final override;

						CL3PUBF	CLASS	TJSONDeserializer	(io::text::ITextReader*);
						CL3PUBF	CLASS	~TJSONDeserializer	();
				};
			}
		}
	}
}

#endif
