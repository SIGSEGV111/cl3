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
						CL3PUBF	void	Push		(const char*, u8_t) final override;
						CL3PUBF	void	Push		(const char*, s8_t) final override;
						CL3PUBF	void	Push		(const char*, u16_t) final override;
						CL3PUBF	void	Push		(const char*, s16_t) final override;
						CL3PUBF	void	Push		(const char*, u32_t) final override;
						CL3PUBF	void	Push		(const char*, s32_t) final override;
						CL3PUBF	void	Push		(const char*, u64_t) final override;
						CL3PUBF	void	Push		(const char*, s64_t) final override;
						CL3PUBF	void	Push		(const char*, f32_t) final override;
						CL3PUBF	void	Push		(const char*, f64_t) final override;
						CL3PUBF	void	Push		(const char*, const char*) final override;
						CL3PUBF	void	Push		(const char*, const wchar_t*) final override;
						CL3PUBF	void	Push		(const char*, const ISerializable&) final override;
						CL3PUBF	system::memory::TUniquePtr<IArraySerializer>
										PushArray	(const char*) final override;

						CL3PUBF	CLASS	TJSONSerializer		(io::text::ITextWriter*, bool pretty = false, usys_t depth = 0);
						CL3PUBF	CLASS	~TJSONSerializer	();
				};

				class	TJSONDeserializer : public IDeserializer
				{
					private:
						CLASS	TJSONDeserializer	(const TJSONDeserializer&) = delete;
						io::text::ITextReader* tr;

					public:
						CL3PUBF	void	Pop			(const char*, u8_t&) final override;
						CL3PUBF	void	Pop			(const char*, s8_t&) final override;
						CL3PUBF	void	Pop			(const char*, u16_t&) final override;
						CL3PUBF	void	Pop			(const char*, s16_t&) final override;
						CL3PUBF	void	Pop			(const char*, u32_t&) final override;
						CL3PUBF	void	Pop			(const char*, s32_t&) final override;
						CL3PUBF	void	Pop			(const char*, u64_t&) final override;
						CL3PUBF	void	Pop			(const char*, s64_t&) final override;
						CL3PUBF	void	Pop			(const char*, f32_t&) final override;
						CL3PUBF	void	Pop			(const char*, f64_t&) final override;
						CL3PUBF	void	Pop			(const char*, const char*&) final override;
						CL3PUBF	void	Pop			(const char*, const wchar_t*&) final override;
						CL3PUBF	void	Pop			(const char*, ISerializable&) final override;
						CL3PUBF	system::memory::TUniquePtr<IArrayDeserializer>
										PopArray	(const char*) final override;

						CL3PUBF	CLASS	TJSONDeserializer	(io::text::ITextReader*);
						CL3PUBF	CLASS	~TJSONDeserializer	();
				};

				/******************************************************/

				class	TJSONArraySerializer : public IArraySerializer
				{
					private:
						CLASS	TJSONArraySerializer	(const TJSONArraySerializer&) = delete;
						io::text::ITextWriter* tw;
						bool pretty;
						bool empty_scope;
						usys_t depth;

						void	BeginNewElement	();

					public:
						CL3PUBF	void	Push		(u8_t) final override;
						CL3PUBF	void	Push		(s8_t) final override;
						CL3PUBF	void	Push		(u16_t) final override;
						CL3PUBF	void	Push		(s16_t) final override;
						CL3PUBF	void	Push		(u32_t) final override;
						CL3PUBF	void	Push		(s32_t) final override;
						CL3PUBF	void	Push		(u64_t) final override;
						CL3PUBF	void	Push		(s64_t) final override;
						CL3PUBF	void	Push		(f32_t) final override;
						CL3PUBF	void	Push		(f64_t) final override;
						CL3PUBF	void	Push		(const char*) final override;
						CL3PUBF	void	Push		(const wchar_t*) final override;
						CL3PUBF	void	Push		(const ISerializable&) final override;
						CL3PUBF	system::memory::TUniquePtr<IArraySerializer>
										PushArray	() final override;

						CL3PUBF	CLASS	TJSONArraySerializer	(io::text::ITextWriter*, bool pretty = false, usys_t depth = 0);
						CL3PUBF	CLASS	~TJSONArraySerializer	();
				};

				class	TJSONArrayDeserializer : public IArrayDeserializer
				{
					private:
						CLASS	TJSONArrayDeserializer	(const TJSONArrayDeserializer&) = delete;
						io::text::ITextReader* tr;

					public:
						CL3PUBF	void	Pop			(u8_t&) final override;
						CL3PUBF	void	Pop			(s8_t&) final override;
						CL3PUBF	void	Pop			(u16_t&) final override;
						CL3PUBF	void	Pop			(s16_t&) final override;
						CL3PUBF	void	Pop			(u32_t&) final override;
						CL3PUBF	void	Pop			(s32_t&) final override;
						CL3PUBF	void	Pop			(u64_t&) final override;
						CL3PUBF	void	Pop			(s64_t&) final override;
						CL3PUBF	void	Pop			(f32_t&) final override;
						CL3PUBF	void	Pop			(f64_t&) final override;
						CL3PUBF	void	Pop			(const char*&) final override;
						CL3PUBF	void	Pop			(const wchar_t*&) final override;
						CL3PUBF	void	Pop			(ISerializable&) final override;
						CL3PUBF	system::memory::TUniquePtr<IArrayDeserializer>
										PopArray	() final override;

						CL3PUBF	CLASS	TJSONArrayDeserializer	(io::text::ITextReader*);
						CL3PUBF	CLASS	~TJSONArrayDeserializer	();
				};
			}
		}
	}
}

#endif
