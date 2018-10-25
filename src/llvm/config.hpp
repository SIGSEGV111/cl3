#pragma once

#if (defined CL3_LLVM_SOURCE_BUNDLE && !defined CL3_LLVM_AMALGAM)
	#define	LL3PUBF
	#define	LL3PUBT
	#define	LL3PUBV extern
#else
	#ifdef	INSIDE_CL3LLVM
		#define	LL3PUBF CL3_CXX_EXPORT_FUNC
		#define	LL3PUBT CL3_CXX_EXPORT_TYPE
		#define	LL3PUBV extern CL3_CXX_EXPORT_FUNC
	#else
		#define	LL3PUBF CL3_CXX_IMPORT_FUNC
		#define	LL3PUBT CL3_CXX_IMPORT_TYPE
		#define	LL3PUBV extern CL3_CXX_IMPORT_FUNC
	#endif
#endif
