﻿#ifndef __td__h__
#define __td__h__

/****************************************************************************
**
** File: td.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
** -------------------------------------------------------------------------
** - Date: Jan-30th, 2020.
** - Version: 1.1.2.9
** - array 다중줄 처리 수정 (bugfix)
** - ini 다중줄 처리 수정 (bugfix)
** - ini 주석 처리 수정 (bugfix)
**
** -------------------------------------------------------------------------
** - Date: Jan-22nd, 2020.
** - Version: 1.0.0.0
** - First release
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
typedef signed int         td_int_t    ;
typedef unsigned int       td_uint_t   ;
typedef unsigned int       td_uint32_t ;
//===========================================================================
typedef unsigned int       td_bool_t   ;
typedef unsigned char      td_byte_t   ;
//===========================================================================
typedef char               td_char_t   ;
//===========================================================================
typedef float              td_float_t  ;
typedef double             td_double_t ;
//===========================================================================
typedef void*              td_pointer_t;





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#define TD_TRUE  1u
#define TD_FALSE 0u
#define TD_NULL_POINTER 0u





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#if defined ( __cplusplus )
#	define TD_API extern "C"
#else
#	define TD_API extern
#endif





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include "td_string.h"
#include "td_ini.h"
#include "td_array.h"





#endif




