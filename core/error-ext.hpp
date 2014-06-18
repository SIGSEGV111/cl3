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

#ifndef	_include_cl3_core_error_ext_hpp_
#define	_include_cl3_core_error_ext_hpp_

#include "error-base.hpp"
#include "io_text_string.hpp"
#include "system_types_typeinfo.hpp"

namespace	cl3
{
	namespace	error
	{
		class	TArgumentException : public TException
		{
			public:
				io::text::string::TString msgbuf;
				template<class T>
				CL3PUBF	CLASS	TArgumentException	(const char* name, const T& value) :
						TException("function-call argument \"%s\" has invalid value \"%s\"", name,
									io::text::string::TCString(*io::text::string::Stringify(value).UObjPtr(), io::text::encoding::CODEC_CXX_CHAR).Chars()) {}

				CL3PUBF	CLASS	TArgumentException	(const TArgumentException&) : TException(NULL) { CL3_NOT_IMPLEMENTED; }
				CL3PUBF	CLASS	~TArgumentException	() { CL3_NOT_IMPLEMENTED; }
		};

		#define	CL3_CLASS_ARGUMENT_ERROR(expression, argument, message) \
			do \
			{ \
				if(expression) \
				{ \
					cl3::error::TArgumentException ae(#argument, argument); \
					ae.Set(this, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					ae.msgbuf<<message; \
					throw ae; \
				} \
			} \
			while(false)

		#define	CL3_NONCLASS_ARGUMENT_ERROR(expression, argument, message) \
			do \
			{ \
				if(expression) \
				{ \
					cl3::error::TArgumentException ae(#argument, argument); \
					ae.Set(NULL, __FILE__, __PRETTY_FUNCTION__, #expression, NULL, __LINE__); \
					ae.msgbuf<<message; \
					throw ae; \
				} \
			} \
			while(false)
	}
}

#endif
