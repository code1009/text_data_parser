#ifndef __td_string__h__
#define __td_string__h__

/****************************************************************************
**
** File: td_string.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
typedef struct _td_string_t
{
	td_char_t* begin;
	td_char_t* end;
	td_uint_t  length;
}
td_string_t;



/////////////////////////////////////////////////////////////////////////////
//===========================================================================
TD_API void td_string_set_null     (td_string_t* p);
TD_API void td_string_shallow_copy (td_string_t* s, td_string_t* d);

TD_API void td_string_trim (td_string_t* p);

//TD_API td_bool_t td_string_is_double_qutation_expression   (td_string_t* p);
//TD_API void      td_string_trim_double_qutation_expression (td_string_t* p);

TD_API td_bool_t td_string_compare (td_string_t* p, td_char_t* s, td_bool_t case_sensitive);

TD_API td_uint_t td_string_copy_to_c_string (td_string_t* p, td_char_t* pointer, td_uint_t size);

TD_API td_double_t td_string_to_real_number (td_string_t* p);
TD_API td_int_t    td_string_to_integer     (td_string_t* p);
TD_API td_uint_t   td_string_to_uinteger    (td_string_t* p);
TD_API td_uint_t   td_string_to_ip_v4       (td_string_t* p);





#endif




