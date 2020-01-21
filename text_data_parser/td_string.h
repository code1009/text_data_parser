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
TD_API void td_string_set_null (td_string_t* p);

TD_API void td_string_trim       (td_string_t* p);
TD_API void td_string_trim_dquot (td_string_t* p);

TD_API td_bool_t td_string_compare (td_string_t* p, td_char_t* s, td_bool_t case_sensitive);

TD_API td_uint_t td_string_copy_to_c_string                          (td_string_t* p, td_char_t* dpointer, td_uint_t dsize);
TD_API td_uint_t td_string_copy_to_c_string_without_multiline_escape (td_string_t* p, td_char_t* dpointer, td_uint_t dsize);

TD_API td_double_t td_string_parse_real_number (td_string_t* p);
TD_API td_int_t    td_string_parse_integer     (td_string_t* p);
TD_API td_uint_t   td_string_parse_uinteger    (td_string_t* p);
TD_API td_uint_t   td_string_parse_ip_v4       (td_string_t* p);





#endif




