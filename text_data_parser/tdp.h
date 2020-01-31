#ifndef __tdp__h__
#define __tdp__h__

/****************************************************************************
**
** File: tdp.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
** -------------------------------------------------------------------------
** - Date: Jan-31st, 2020.
** - Version: 1.2.0.11
** - array 다중줄 처리 수정 (bugfix)
** - ini 다중줄 처리 수정 (bugfix)
** - ini 주석 처리 수정 (bugfix)
** - 이름 수정
**
** -------------------------------------------------------------------------
** - Date: Jan-22nd, 2020.
** - Version: 1.0.0.0
** - First release
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
typedef signed int    tdp_int_t    ;
typedef unsigned int  tdp_uint_t   ;
typedef unsigned int  tdp_uint32_t ;

typedef unsigned int  tdp_bool_t   ;
typedef unsigned char tdp_byte_t   ;

typedef char          tdp_char_t   ;

typedef float         tdp_float_t  ;
typedef double        tdp_double_t ;

typedef void*         tdp_pointer_t;





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#define TDP_TRUE  1u
#define TDP_FALSE 0u

#define TDP_NULL_POINTER 0u





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#if defined ( __cplusplus )
#	define TDP_API extern "C"
#else
#	define TDP_API extern
#endif





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include "tdp_string.h"
#include "tdp_ini.h"
#include "tdp_array.h"





#endif




