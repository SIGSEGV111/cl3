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

#ifndef	_include_cl3_core_math_hpp_
#define	_include_cl3_core_math_hpp_

#include <stdarg.h>
#include <math.h>
#include "system_types.hpp"
#include "system_compiler.hpp"
#include "error.hpp"

#if (CL3_CXX == CL3_CXX_MSVC)
	CL3PUBF	long	lround	(double x);
	CL3PUBF	long	lroundf	(float x);
	CL3PUBF	long	lroundl	(long double x);
#endif

namespace	cl3
{
	namespace	math
	{
		using namespace system::types;

		template<class T>
		inline	bool	FloatCompare	(const T a, const T b, const T epsilon)
		{
			return ((a + epsilon) > b) && ((a - epsilon) < b);
		}

		static const double PI = 3.14159265358979323846264338327950288419716939937510582097;
		static const double e  = 2.71828182845904523536028747135266249775724709369995957497;

		CL3PUBF	float		log	(float x, float b);
		CL3PUBF	double		log	(double x, double b);
		CL3PUBF	long double	log	(long double x, long double b);

		inline int logbd(u32_t v) { return 31 - system::compiler::CountLeadingZeroes(v); }
		inline int logbd(u64_t v) { return 63 - system::compiler::CountLeadingZeroes(v); }
		inline u32_t logbu(u32_t v) { u32_t lb = (31 - system::compiler::CountLeadingZeroes(v)); return (((u32_t)1) << lb) == v ? lb : lb + 1; }
		inline u64_t logbu(u64_t v) { u64_t lb = (63 - system::compiler::CountLeadingZeroes(v)); return (((u64_t)1) << lb) == v ? lb : lb + 1; }

		CL3PUBF	float		sigsym	(float);
		CL3PUBF	double		sigsym	(double);
		CL3PUBF	long double	sigsym	(long double);

		CL3PUBF	float		sigsym_diff	(float);
		CL3PUBF	double		sigsym_diff	(double);
		CL3PUBF	long double	sigsym_diff	(long double);

		CL3PUBF	float		siglog	(float);
		CL3PUBF	double		siglog	(double);
		CL3PUBF	long double	siglog	(long double);

		CL3PUBF	float		siglog_diff	(float);
		CL3PUBF	double		siglog_diff	(double);
		CL3PUBF	long double	siglog_diff	(long double);

		//	source: http://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
		template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

		template<class T, unsigned int nC, unsigned int nR>
		class CL3PUBT	TMatrix;

		template<class T, unsigned int nR>
		class CL3PUBT	TVector
		{
			public:
				static const unsigned int Rn = nR;
// 				typedef typename TCTTI<T>::T T;
			protected:
				T	v[nR];

			public:
				//	component-wise operators
				inline	TVector&	operator+=	(const TVector& op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] += op[i];
					return *this;
				}

				inline	TVector&	operator+=	(const T op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] += op;
					return *this;
				}

				inline	TVector		operator+	(const TVector& op) const
				{
					TVector r = *this;
					return r += op;
				}

				inline	TVector		operator+	(const T op) const
				{
					TVector r = *this;
					return r += op;
				}

				inline	TVector&	operator-=	(const TVector& op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] -= op[i];
					return *this;
				}

				inline	TVector&	operator-=	(const T op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] -= op;
					return *this;
				}

				inline	TVector		operator-	(const TVector& op) const
				{
					TVector r = *this;
					return r -= op;
				}

				inline	TVector		operator-	(const T op) const
				{
					TVector r = *this;
					return r -= op;
				}

				inline	TVector&	operator*=	(const TVector& op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] *= op[i];
					return *this;
				}

				inline	TVector&	operator*=	(const T op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] *= op;
					return *this;
				}

				inline	TVector		operator*	(const TVector& op) const
				{
					TVector r = *this;
					return r *= op;
				}

				inline	TVector		operator*	(const T op) const
				{
					TVector r = *this;
					return r *= op;
				}

				inline	TVector&	operator/=	(const TVector& op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] /= op[i];
					return *this;
				}

				inline	TVector&	operator/=	(const T op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] /= op;
					return *this;
				}

				inline	TVector		operator/	(const TVector& op) const
				{
					TVector r = *this;
					return r /= op;
				}

				inline	TVector		operator/	(const T op) const
				{
					TVector r = *this;
					return r /= op;
				}

				//	vector operators
				inline	TVector&	operator%=	(const TVector& op)			//	cross-product
				{
					CL3_CLASS_LOGIC_ERROR(nR != 3);
					T r[3];
					r[0] = v[1]*op[2] - v[2]*op[1];
					r[1] = v[2]*op[0] - v[0]*op[2];
					r[2] = v[0]*op[1] - v[1]*op[0];
					v[0] = r[0]; v[1] = r[1]; v[2] = r[2];
					return *this;
				}

				inline	TVector		operator%	(const TVector& op) const
				{
					TVector r = *this;
					return r %= op;
				}

				inline	T			operator^	(const TVector& op) const	//	dot-product
				{
					T r = (T)0;
					for(unsigned int i = 0; i < nR; i++)
						r += v[i] * op[i];
					return r;
				}

				//	index operators
				inline	T&			operator[]	(unsigned int idxR)
				{
					return v[idxR];
				}

				inline	const T&	operator[]	(unsigned int idxR) const
				{
					return v[idxR];
				}

				//	assignment operators
				template<class TOp, unsigned int nROp>
				inline	TVector&	operator=	(const TVector<TOp, nROp>& op)
				{
					for(unsigned int i = 0; i < (nR <= nROp ? nR : nROp); i++)	//	transfer all R's availabale
						v[i] = (T)op[i];
					//	leave all other dimensions untouched
					return *this;
				}

				inline	TVector&	operator=	(const TVector& op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] = op[i];
					return *this;
				}

				inline	TVector&	operator=	(const T op)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] = op;
					return *this;
				}

				//	type casting operators
				/*inline				operator T	() const
				{
					return Square();
				}*/

				//	comparators
				inline	bool		FloatCompare	(const T op, const T epsilon) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(!cl3::math::FloatCompare(v[i], op, epsilon))
							return false;
					return true;
				}

				inline	bool		FloatCompare	(const TVector op, const T epsilon) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(!cl3::math::FloatCompare(v[i], op[i], epsilon))
							return false;
					return true;
				}

				inline	bool		operator||	(TVector& op) const	//	parallel?
				{
					for(unsigned int i = 1; i < nR; i++)
						if(v[0] * op[i] != v[i] * op[0])
							return false;
					return true;
				}

				inline	bool		operator>	(const TVector& op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] <= op[i])
							return false;
					return true;
				}

				inline	bool		operator>	(const T op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] <= op)
							return false;
					return true;
				}

				inline	bool		operator>=	(const TVector& op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] < op[i])
							return false;
					return true;
				}

				inline	bool		operator>=	(const T op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] < op)
							return false;
					return true;
				}

				inline	bool		operator<	(const TVector& op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] >= op[i])
							return false;
					return true;
				}

				inline	bool		operator<	(const T op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] >= op)
							return false;
					return true;
				}

				inline	bool		operator<=	(const TVector& op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] > op[i])
							return false;
					return true;
				}

				inline	bool		operator<=	(const T op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] > op)
							return false;
					return true;
				}

				inline	bool		operator==	(const TVector& op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] != op[i])
							return false;
					return true;
				}

				inline	bool		operator==	(const T op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] != op)
							return false;
					return true;
				}

				inline	bool		operator!=	(const TVector& op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] != op[i])
							return true;
					return false;
				}

				inline	bool		operator!=	(const T op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] != op)
							return true;
					return false;
				}

				//	utility functions
				inline	T			Volume		() const
				{
					T r = v[0];
					for(unsigned int i = 1; i < nR; i++)
						r *= v[i];
					return r;
				}

				inline	T			Square		() const
				{
					T s = T(0);
					for(unsigned int i = 0; i < nR; i++)
						s += v[i] * v[i];
					return s;
				}

				inline	T			Length		() const
				{
					return sqrt(this->Square());
				}

				inline	bool		IsAnyBigger	(const TVector& vOp) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] > vOp[i])
							return true;
					return false;
				}

				inline	bool		IsAnyBigger	(const T Op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] > Op)
							return true;
					return false;
				}

				inline	bool		IsAnySmaller	(const TVector& vOp) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] < vOp[i])
							return true;
					return false;
				}

				inline	bool		IsAnySmaller	(const T Op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] < Op)
							return true;
					return false;
				}

				inline	bool		IsAnyEqual	(const TVector& vOp) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] == vOp[i])
							return true;
					return false;
				}

				inline	bool		IsAnyEqual	(const T Op) const
				{
					for(unsigned int i = 0; i < nR; i++)
						if(v[i] == Op)
							return true;
					return false;
				}

				//	component-wise max-value
				inline	T	Max	() const
				{
					T r = v[0];
					for(unsigned int i = 1; i < nR; i++)
						if(v[i] > r) r = v[i];
					return r;
				}

				//	component-wise min-value
				inline	T	Min	() const
				{
					T r = v[0];
					for(unsigned int i = 1; i < nR; i++)
						if(v[i] < r) r = v[i];
					return r;
				}

				inline	TVector	Abs	() const
				{
					TVector r;
					for(unsigned int i = 0; i < nR; i++)
						r[i] = CL3_ABS(v[i]);
					return r;
				}

				inline	TMatrix<T, nR, nR>	RotationMatrix	()	const
				{
					CL3_CLASS_LOGIC_ERROR(nR != 3);
					if((v[0] == 0.0) && (v[1] == 0.0) && (v[2] == 0.0))
						return	TMatrix<T, nR, nR>::Identity();
					T r[nR][nR];
					T l = Length(), n0 = v[0]/l, n1 = v[1]/l, n2 = v[2]/l;
					T sina = (T)sin(l), cosa = (T)cos(l), _cos = 1-cosa;
					r[0][0] = _cos*n0*n0+cosa;		r[1][0] = _cos*n0*n1-sina*n2;	r[2][0] = _cos*n0*n2+sina*n1;
					r[0][1] = _cos*n1*n0+sina*n2;	r[1][1] = _cos*n1*n1+cosa;		r[2][1] = _cos*n1*n2-sina*n0;
					r[0][2] = _cos*n2*n0-sina*n1;	r[1][2] = _cos*n2*n1+sina*n0;	r[2][2] = _cos*n2*n2+cosa;
					return TMatrix<T, nR, nR>(r);
				}

				inline	TMatrix<T, nR, nR>	SkewSymmetric	()	const
				{
					CL3_CLASS_LOGIC_ERROR(nR != 3);
					T r[nR][nR];
					r[0][0] = (T)0 ;	r[1][0] = -v[2];	r[2][0] =  v[1];
					r[0][1] =  v[2];	r[1][1] = (T)0 ;	r[2][1] = -v[0];
					r[0][2] = -v[1];	r[1][2] =  v[0];	r[2][2] = (T)0 ;
					return TMatrix<T, nR, nR>(r);
				}

				inline	TVector&	Normalize	()
				{
					T l = Length();
					if(l == 0)	*this = 0;
					else		*this /= l;
					return *this;
				}

				inline	TVector		Normalized	() const
				{
					TVector r = *this;
					return r.Normalize();
				}

				template<class TOp, unsigned int nROp>
				inline	TVector	(const TVector<TOp, nROp>& vOp)
				{
					unsigned int i;
					for(i = 0; i < (nR <= nROp ? nR : nROp); i++)	//	transfer all R's availabale
						v[i] = (T) vOp[i];
					for(; i < nR; i++)	//	if vOp had less nR than this vector, set spare R's to 0
						v[i] = 0;
				}

				inline	CLASS	TVector	()
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] = (T)0;
				}

				inline	CLASS	TVector	(T v0)
				{
					CL3_CLASS_LOGIC_ERROR(nR < 1);
					v[0] = v0;
					for(unsigned int i = 1; i < nR; i++)
						v[i] = (T)0;
				}

				inline	CLASS	TVector	(T v0, T v1)
				{
					CL3_CLASS_LOGIC_ERROR(nR < 2);
					v[0] = v0;
					v[1] = v1;
					for(unsigned int i = 2; i < nR; i++)
						v[i] = (T)0;
				}

				inline	CLASS	TVector	(T v0, T v1, T v2)
				{
					CL3_CLASS_LOGIC_ERROR(nR < 3);
					v[0] = v0;
					v[1] = v1;
					v[2] = v2;
					for(unsigned int i = 3; i < nR; i++)
						v[i] = (T)0;
				}

				inline	CLASS	TVector	(T v0, T v1, T v2, T v3)
				{
					CL3_CLASS_LOGIC_ERROR(nR < 4);
					v[0] = v0;
					v[1] = v1;
					v[2] = v2;
					v[3] = v3;
					for(unsigned int i = 4; i < nR; i++)
						v[i] = (T)0;
				}

				inline	CLASS	TVector	(T v0, T v1, T v2, T v3, T v4)
				{
					CL3_CLASS_LOGIC_ERROR(nR < 5);
					v[0] = v0;
					v[1] = v1;
					v[2] = v2;
					v[3] = v3;
					v[4] = v4;
					for(unsigned int i = 5; i < nR; i++)
						v[i] = (T)0;
				}

				inline	CLASS	TVector	(T v0, T v1, T v2, T v3, T v4, T v5)
				{
					CL3_CLASS_LOGIC_ERROR(nR < 6);
					v[0] = v0;
					v[1] = v1;
					v[2] = v2;
					v[3] = v3;
					v[4] = v4;
					v[5] = v5;
					for(unsigned int i = 6; i < nR; i++)
						v[i] = (T)0;
				}

				inline	CLASS	TVector	(const T vi[nR])
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] = vi[i];
				}

				inline	CLASS	TVector	(const TVector& rhs)
				{
					for(unsigned int i = 0; i < nR; i++)
						v[i] = rhs.v[i];
				}
		};

		template<class T, unsigned int nC, unsigned int nR>
		class CL3PUBT	TMatrix
		{
			public:
				static const unsigned int Cn = nC;
				static const unsigned int Rn = nR;

			protected:
				T	m[nC][nR];

			public:
			//	component-wise operators
				inline	TMatrix&	operator+=	(const TMatrix& op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] += op[i][j];
					return *this;
				}
				inline	TMatrix&	operator+=	(const T op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] += op;
					return *this;
				}
				inline	TMatrix		operator+	(const TMatrix& op) const
				{
					TMatrix r = *this;
					return r += op;
				}
				inline	TMatrix		operator+	(const T op) const
				{
					TMatrix r = *this;
					return r += op;
				}

				inline	TMatrix&	operator-=	(const TMatrix& op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] -= op[i][j];
					return *this;
				}
				inline	TMatrix&	operator-=	(const T op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] -= op;
					return *this;
				}
				inline	TMatrix		operator-	(const TMatrix& op) const
				{
					TMatrix r = *this;
					return r -= op;
				}
				inline	TMatrix		operator-	(const T op) const
				{
					TMatrix r = *this;
					return r -= op;
				}

				inline	TMatrix&	operator*=	(const TMatrix& op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] *= op[i][j];
					return *this;
				}
				inline	TMatrix&	operator*=	(const T op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] *= op;
					return *this;
				}
				inline	TMatrix		operator*	(const TMatrix& op) const
				{
					TMatrix r = *this;
					return r *= op;
				}
				inline	TMatrix		operator*	(const T op) const
				{
					TMatrix r = *this;
					return r *= op;
				}

				inline	TMatrix&	operator/=	(const TMatrix& op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] /= op[i][j];
					return *this;
				}
				inline	TMatrix&	operator/=	(const T op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] /= op;
					return *this;
				}
				inline	TMatrix		operator/	(const TMatrix& op) const
				{
					TMatrix r = *this;
					return r /= op;
				}
				inline	TMatrix		operator/	(const T op) const
				{
					TMatrix r = *this;
					return r /= op;
				}


			// matrix operators
				template<unsigned int nCOp>
				inline	TMatrix<T, nCOp, nR>		operator%	(const TMatrix<T, nCOp, nC>& op) const	// matrix product
				{
					TMatrix<T, nCOp, nR> r;
					for(unsigned int i = 0; i < nCOp; i++)
						for(unsigned int j = 0; j < nR; j++)
							for(unsigned int k = 0; k < nC; k++)
								r[i][j] += m[k][j] * op[i][k];	// m[k][j] * op[i][k];
					return r;
				}

				inline	TVector<T, nR>		operator%	(const TVector<T, nC>& op) const	// matrix transformation
				{
					TVector<T, nR> r;
					for(unsigned int i = 0; i < nR; i++)
						for(unsigned int j = 0; j < nC; j++)
							r[i] += m[j][i] * op[j];
					return r;
				}





			//	index operators
				inline	T*		operator[]	(unsigned int idxC)
				{
					return  m[idxC];
				}
				inline	const T*	operator[]	(unsigned int idxC) const
				{
					return  m[idxC];
				}


			//	assignment operators
				template<class T2, unsigned int nCOp, unsigned int nROp>
				inline	TMatrix&	operator=	(const TMatrix<T2, nCOp, nROp>& op)
				{
					for(unsigned int i = 0; i < (nC <= nCOp ? nC : nCOp); i++)		//	transfer all Rows availabale
						for(unsigned int j = 0; j < (nR <= nROp ? nR : nROp); j++)	//	transfer all Columns availabale
							m[i][j] = (T) op[i][j];
					//	leave all other dimensions untouched
					return *this;
				}

				template<unsigned int nCOp, unsigned int nROp>
				inline	TMatrix&	operator=	(const TMatrix<T, nCOp, nROp>& op)
				{
					for(unsigned int i = 0; i < (nC <= nCOp ? nC : nCOp); i++)		//	transfer all Rows availabale
						for(unsigned int j = 0; j < (nR <= nROp ? nR : nROp); j++)	//	transfer all Columns availabale
							m[i][j] = op[i][j];
					//	leave all other dimensions untouched
					return *this;
				}

				inline	TMatrix&	operator=	(const T op)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] = op;
					return *this;
				}

			// utility functions
				inline	TMatrix		Transpose	() const
				{
					TMatrix<T, nR, nC> r;
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							r[j][i] = m[i][j];
					return r;
				}
				inline	TMatrix<T, (nC > 1) ? nC-1 : 1, (nR > 1) ? nR-1 : 1>		Sub	(unsigned int iOp, unsigned int jOp) const
				{
					CL3_CLASS_ERROR(iOp > nC || jOp > nR, error::TException, "index out of range");
					TMatrix<T, (nC > 1) ? nC-1 : 1, (nR > 1) ? nR-1 : 1> r;
					for(unsigned int i = 0; i < nC-1; i++)
						for(unsigned int j = 0; j < nR-1; j++)
							r[i][j] = m[(i < iOp ? i : i+1)][(j < jOp ? j : j+1)];
					return r;
				}
				inline	TMatrix<T, nR, nC>		Adjugate	() const
				{
					TMatrix<T, nR, nC> r;
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							r[j][i] = Sub(i, j).Determinant() * (((i+j) % 2) ? (T)-1 : (T)1);
					return r;
				}
				inline	TMatrix		Inverse	() const
				{
					return Adjugate() /= Determinant();
				}

				inline	T	Determinant	() const
				{
					CL3_CLASS_LOGIC_ERROR(2 > nC || nC != nR || nR < 2);
					if(nC == 2)
						return m[0][0]*m[1][1] - m[1][0]*m[0][1];
					else if(nC == 3)
						return m[0][0]*m[1][1]*m[2][2] + m[1][0]*m[2][1]*m[0][2] + m[2][0]*m[0][1]*m[1][2] - m[2][0]*m[1][1]*m[0][2] - m[0][0]*m[2][1]*m[1][2] - m[1][0]*m[0][1]*m[2][2];
					else
					{
						T	temp = (T)0;
						for(unsigned int i = 0; i < nC; i++)
							temp += Sub(i, 0).Determinant() * m[i][0] * ((i % 2) ? (T)-1 : (T)1);
						return temp;
					}
				}

				static	TMatrix		Identity	()
				{
					TMatrix<T, nC, nR> r;
					for(unsigned int i = 0; i < nC && i < nR; i++)
						r[i][i] = (T)1;
					return r;
				}

				static	TMatrix		Projection	(T FOV, T ratio, T near_plane, T far_plane)
				{
					CL3_NONCLASS_ERROR( nC != 4 || nR != 4,error::TException,"Projection Matrix must be sized 4x4.");
					TMatrix r;
					r.ToIdentity();

					r[1][1] = 1.0f / tan(FOV * (T) cl3::math::PI / 360.0f );
					r[0][0] = r[1][1] / ratio;
					r[2][2] = ( far_plane + near_plane ) / ( near_plane - far_plane );
					r[3][2] = ( 2.0f * far_plane * near_plane) / (near_plane - far_plane);
					r[2][3] = -1.0f;
					r[3][3] = 0.0f;
					return r;
				}

				inline	TMatrix&		Rotate(T angle, T x, T y, T z)
				{
					CL3_CLASS_ERROR( nC != 4 || nR != 4, error::TException, "Rotation Matrix must be sized 4x4.");
					if(angle == 0 || (x == 0 && y == 0 && z == 0))
						return *this;
					TMatrix r;
					angle = angle/180.0f*(T)cl3::math::PI;
					TVector<T,3> axis(x,y,z);
					T len = 1.0f/(T) axis.Length();
					if( len != 1.0f )
					{
						x *= len;
						y *= len;
						z *= len;
					}

					T c = cos(angle);
					T s = sin(angle);

					r[0][0] = x*x*(1-c)+c;
					r[1][0] = x*y*(1-c)-z*s;
					r[2][0] = x*z*(1-c)+y*s;

					r[0][1] = y*x*(1-c)+z*s;
					r[1][1] = y*y*(1-c)+c;
					r[2][1] = y*z*(1-c)-x*s;

					r[0][2] = x*z*(1-c)-y*s;
					r[1][2] = y*z*(1-c)+x*s;
					r[2][2] = z*z*(1-c)+c;

					r[3][3] = 1.0f;

					*this = r % *this;
					return *this;
				}

				inline	TMatrix&		Translate(T x, T y, T z)
				{
					CL3_CLASS_ERROR( nC != 4 || nR != 4, error::TException, "Translation Matrix must be sized 4x4.");
					TMatrix r = Identity();
					r[3][0] = x;
					r[3][1] = y;
					r[3][2] = z;

					*this = r % *this;
					return *this;
				}

				inline	TMatrix&		Scale(T x, T y, T z)
				{
					CL3_CLASS_ERROR( nC != 4 || nR != 4, error::TException, "Scaling Matrix must be sized 4x4.");
					TMatrix r;
					r[0][0] = x;
					r[1][1] = y;
					r[2][2] = z;
					r[3][3] = 1.0f;
					*this = r % *this;
					return *this;
				}

				inline	TVector<T, nR>	RotationAxis	() const
				{
					CL3_CLASS_LOGIC_ERROR( nC < 3 || nR < 3);
					TVector<T, nR> r;
					T c = (m[0][0] + m[1][1] + m[2][2] - (T)1)/(T)2;
					if(c >= (T) 1)	return r;
					if(c <= (T)-1)
					{
						T x = (m[0][0] + (T)1) / (T)2;
						T y = (m[1][1] + (T)1) / (T)2;
						T z = (m[2][2] + (T)1) / (T)2;
						if(x > y && x > z && x > (T)0)
						{
							r[0] = sqrt(x);
							r[1] = (m[1][0] + m[0][1]) / ((T)4 * r[0]);
							r[2] = (m[2][0] + m[0][2]) / ((T)4 * r[0]);
						}
						else if(y > z && y > (T)0)
						{
							r[1] = sqrt(y);
							r[0] = (m[0][1] + m[1][0]) / ((T)4 * r[1]);
							r[2] = (m[2][1] + m[1][2]) / ((T)4 * r[1]);
						}
						else if(z > (T)0)
						{
							r[2] = sqrt(z);
							r[0] = (m[0][2] + m[2][0]) / ((T)4 * r[2]);
							r[1] = (m[1][2] + m[2][1]) / ((T)4 * r[2]);
						}
						r *= (float) PI;
						return r;
					}
					T s = acos(c) / ((T)2 * sqrt((T)1 - c * c));
					r[0] = (m[1][2] - m[2][1]) * s;
					r[1] = (m[2][0] - m[0][2]) * s;
					r[2] = (m[0][1] - m[1][0]) * s;
					return r;
				}

				inline	TMatrix		Diagonal	() const
				{
					TMatrix<T, nC, nR> r;
					for(unsigned int i = 0; i < nC && i < nR; i++)
						r[i][i] = m[i][i];
					return r;
				}
				inline	TMatrix		UpperTriangular	() const
				{
					TMatrix<T, nC, nR> r;
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							if(i > j)	r[i][j] = m[i][j];
					return r;
				}
				inline	TMatrix		LowerTriangular	() const
				{
					TMatrix<T, nC, nR> r;
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							if(i < j)	r[i][j] = m[i][j];
					return r;
				}

				inline	TMatrix&		ToIdentity	()
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							if(i == j)	m[i][j] = (T)1;
							else		m[i][j] = (T)0;
					return *this;
				}
				inline	TMatrix&		ToDiagonal	()
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							if(i != j)	m[i][j] = (T)0;
					return *this;
				}
				inline	TMatrix&		ToUpperTriangular	()
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							if(i <= j)	m[i][j] = (T)0;
					return *this;
				}
				inline	TMatrix&		ToLowerTriangular	()
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							if(i >= j)	m[i][j] = (T)0;
					return *this;
				}

			// constructors
				inline	CLASS	TMatrix ()
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] = (T)0;
				}

				inline	CLASS	TMatrix	(const T mi[nC][nR])
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] = mi[i][j];
				}

				template<class T2>
				inline	CLASS	TMatrix	(const TMatrix<T2,nC,nR>& ref)
				{
					for(unsigned int i = 0; i < nC; i++)
						for(unsigned int j = 0; j < nR; j++)
							m[i][j] = (T) ref[i][j];
				}
		};

		template<u8_t num, u64_t base>
		struct	TPow
		{
			static const u64_t RESULT = ((num == 0) ? (1) : (base * TPow<base, num-1>::RESULT));
		};

		template<u8_t num>
		struct	TPow<num, 2>
		{
			static const u64_t RESULT = ((u64_t)1 << (u64_t)num);
		};

		template<u64_t v>
		struct	TLogB_down
		{
			static const unsigned int RESULT = TLogB_down< v/2 >::RESULT + 1;
		};

		template<>	struct	TLogB_down<1>
		{
			static const unsigned int RESULT = 0;
		};

		template<>	struct	TLogB_down<0>
		{
		};



		template<u64_t v>
		struct	TLogB_up
		{
			static const unsigned int RESULT = (TPow<TLogB_up< v/2 >::RESULT + 1, 2>::RESULT >= v) ? (TLogB_up< v/2 >::RESULT + 1) : (TLogB_up< v/2 >::RESULT + 2);
		};

		template<>	struct	TLogB_up<1>
		{
			static const unsigned int RESULT = 0;
		};

		template<>	struct	TLogB_up<0>
		{
		};

		template class CL3PUBT TVector<f32_t, 1>;
		template class CL3PUBT TVector<f32_t, 2>;
		template class CL3PUBT TVector<f32_t, 3>;
		template class CL3PUBT TVector<f32_t, 4>;

		template class CL3PUBT TVector<f64_t, 1>;
		template class CL3PUBT TVector<f64_t, 2>;
		template class CL3PUBT TVector<f64_t, 3>;
		template class CL3PUBT TVector<f64_t, 4>;

		template class CL3PUBT TMatrix<f32_t, 1, 1>;
		template class CL3PUBT TMatrix<f32_t, 2, 2>;
		template class CL3PUBT TMatrix<f32_t, 3, 3>;
		template class CL3PUBT TMatrix<f32_t, 4, 4>;
	};
};

#endif
