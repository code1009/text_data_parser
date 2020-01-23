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
TD_API td_bool_t td_string_empty (td_string_t* p);
TD_API void      td_string_clear (td_string_t* p);

TD_API void td_string_begin (td_string_t* p, td_char_t* s);
TD_API void td_string_end   (td_string_t* p, td_char_t* s);

TD_API void td_string_trim                 (td_string_t* p); // ' ', '\n', '\r', '\t', '\\'
TD_API void td_string_trim_dquotes         (td_string_t* p); // ""
TD_API void td_string_trim_squotes         (td_string_t* p); // ''
TD_API void td_string_trim_curly_brackets  (td_string_t* p); // {}
TD_API void td_string_trim_round_brackets  (td_string_t* p); // ()
TD_API void td_string_trim_square_brackets (td_string_t* p); // []
TD_API void td_string_trim_angle_brackets  (td_string_t* p); // <>
#define     td_string_trim_braces          td_string_trim_curly_brackets 
#define     td_string_trim_parentheses     td_string_trim_round_brackets 
#define     td_string_trim_box_brackets    td_string_trim_square_brackets

TD_API td_bool_t td_string_compare (td_string_t* p, td_char_t* s, td_bool_t case_sensitive);

TD_API td_uint_t td_string_copy_to_c_string                          (td_string_t* p, td_char_t* dpointer, td_uint_t dsize);
TD_API td_uint_t td_string_copy_to_c_string_without_escape_multiline (td_string_t* p, td_char_t* dpointer, td_uint_t dsize);

TD_API td_double_t td_string_parse_real_number (td_string_t* p);
TD_API td_int_t    td_string_parse_integer     (td_string_t* p);
TD_API td_uint_t   td_string_parse_uinteger    (td_string_t* p);
TD_API td_uint_t   td_string_parse_ip_v4       (td_string_t* p);





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
TD_API td_char_t* td_parse_escape_multiline (td_char_t* begin, td_char_t* end);
TD_API td_char_t* td_parse_escape_sequence  (td_char_t* begin, td_char_t* end);
TD_API td_char_t* td_parse_token_char       (td_char_t* begin, td_char_t* end, td_char_t token);





#endif




