/*
    libcl2 - common library version 2
    Copyright (C) 2010	Simon Brennecke

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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "math.hpp"

#if (CL3_CXX == CL3_CXX_MSVC)
	long	lround	(double x)		{ return (long)((x) > 0 ? (x) + 0.5  : ceil((x)  - 0.5)); }
	long	lroundf	(float x)		{ return (long)((x) > 0 ? (x) + 0.5f : ceilf((x) - 0.5f)); }
	long	lroundl	(long double x)	{ return (long)((x) > 0 ? (x) + 0.5l : ceil((x) - 0.5l)); }
#endif

namespace	cl3
{
	namespace	math
	{
		template class CL3PUBT TVector<f32_t, 1>;
		template class CL3PUBT TVector<f32_t, 2>;
		template class CL3PUBT TVector<f32_t, 3>;
		template class CL3PUBT TVector<f32_t, 4>;

		template class CL3PUBT TVector<f64_t, 1>;
		template class CL3PUBT TVector<f64_t, 2>;
		template class CL3PUBT TVector<f64_t, 3>;
		template class CL3PUBT TVector<f64_t, 4>;

		float		log	(float x, float b)
		{
			return ::logf(x) / ::logf(b);
		}

		double		log	(double x, double b)
		{
			return ::log(x) / ::log(b);
		}

		long double	log	(long double x, long double b)
		{
			return ::logl(x) / ::logl(b);
		}

		float		sigsym	(float v)
		{
			//	source: wikipedia
			return 2.0f / (1.0f + expf(-v)) - 1.0f;
		}

		double		sigsym	(double v)
		{
			//	source: wikipedia
			return 2.0 / (1.0 + exp(-v)) - 1.0;
		}

		long double	sigsym	(long double v)
		{
			//	source: wikipedia
			return 2.0l / (1.0l + expl(-v)) - 1.0l;
		}

		float		sigsym_diff	(float x)
		{
			return 1.0f / (coshf(x) + 1.0f);
		}

		double		sigsym_diff	(double x)
		{
			return 1.0 / (cosh(x) + 1.0);
		}

		long double	sigsym_diff	(long double x)
		{
			return 1.0l / (coshl(x) + 1.0l);
		}

		float		siglog	(float x)
		{
			return 1.0f / (1.0f + expf(-x));
		}

		double		siglog	(double x)
		{
			return 1.0 / (1.0 + exp(-x));
		}

		long double	siglog	(long double x)
		{
			return 1.0l / (1.0l + expl(-x));
		}

		float		siglog_diff	(float x)
		{
			float ex = expf(x);
			return ex / ((ex+1.0f)*(ex+1.0f));
		}

		double		siglog_diff	(double x)
		{
			double ex = exp(x);
			return ex / ((ex+1.0)*(ex+1.0));
		}

		long double	siglog_diff	(long double x)
		{
			long double ex = expl(x);
			return ex / ((ex+1.0l)*(ex+1.0l));
		}
	}
}
