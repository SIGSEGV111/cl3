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

#ifndef	_include_cl3_core_math_formular_hpp_
#define	_include_cl3_core_math_formular_hpp_

#include "io_text_string.hpp"
#include "system_compiler_jit.hpp"

namespace	cl3
{
	namespace	math
	{
		namespace	formular
		{
			CL3PUBV	system::compiler::jit::TModule jit_module;

			CL3PUBF	system::compiler::jit::TFunction&	Parse	(const io::text::string::TString& formular);
			CL3PUBF	system::compiler::jit::TFunction&	Parse	(const io::text::string::TString& formular, system::compiler::jit::TModule& module);
		}
	}
}

#endif
