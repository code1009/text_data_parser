#ifndef __tdp_string__h__
#define __tdp_string__h__

/****************************************************************************
**
** File: tdp_string.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
typedef struct _tdp_string_t
{
	tdp_char_t* begin;
	tdp_char_t* end;
	tdp_uint_t  length;
}
tdp_string_t;





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
TDP_API tdp_bool_t tdp_string_empty (tdp_string_t* p);
TDP_API void       tdp_string_clear (tdp_string_t* p);
TDP_API void       tdp_string_copy  (tdp_string_t* sp, tdp_string_t* dp);

TDP_API void tdp_string_begin (tdp_string_t* p, tdp_char_t* s);
TDP_API void tdp_string_end   (tdp_string_t* p, tdp_char_t* s);

TDP_API void tdp_string_trim                 (tdp_string_t* p); // ' ', '\n', '\r', '\t', '\\'
TDP_API void tdp_string_trim_dquotes         (tdp_string_t* p); // ""
TDP_API void tdp_string_trim_squotes         (tdp_string_t* p); // ''
TDP_API void tdp_string_trim_curly_brackets  (tdp_string_t* p); // {}
TDP_API void tdp_string_trim_round_brackets  (tdp_string_t* p); // ()
TDP_API void tdp_string_trim_square_brackets (tdp_string_t* p); // []
TDP_API void tdp_string_trim_angle_brackets  (tdp_string_t* p); // <>
#define      tdp_string_trim_braces          tdp_string_trim_curly_brackets 
#define      tdp_string_trim_parentheses     tdp_string_trim_round_brackets 
#define      tdp_string_trim_box_brackets    tdp_string_trim_square_brackets

TDP_API tdp_bool_t tdp_string_prefix_compare             (tdp_string_t* p, tdp_char_t* prefix, tdp_bool_t case_sensitive);
TDP_API tdp_bool_t tdp_string_suffix_compare             (tdp_string_t* p, tdp_char_t* suffix, tdp_bool_t case_sensitive);
TDP_API tdp_bool_t tdp_string_prefix_suffix_compare      (tdp_string_t* p, tdp_char_t* prefix, tdp_char_t* suffix, tdp_bool_t case_sensitive);
TDP_API void       tdp_string_part_without_prefix_suffix (tdp_string_t* p, tdp_char_t* prefix, tdp_char_t* suffix, tdp_bool_t case_sensitive, tdp_string_t* dp);

TDP_API tdp_bool_t tdp_string_compare (tdp_string_t* p, tdp_char_t* s, tdp_bool_t case_sensitive);

TDP_API tdp_uint_t tdp_string_copy_to_c_string                          (tdp_string_t* p, tdp_char_t* dpointer, tdp_uint_t dlength);
TDP_API tdp_uint_t tdp_string_copy_to_c_string_without_escape_multiline (tdp_string_t* p, tdp_char_t* dpointer, tdp_uint_t dlength);

TDP_API tdp_double_t tdp_string_parse_real_number (tdp_string_t* p);
TDP_API tdp_int_t    tdp_string_parse_integer     (tdp_string_t* p);
TDP_API tdp_uint_t   tdp_string_parse_uinteger    (tdp_string_t* p);
TDP_API tdp_uint_t   tdp_string_parse_ip_v4       (tdp_string_t* p);





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
TDP_API tdp_char_t* tdp_parse_escape_multiline (tdp_char_t* begin, tdp_char_t* end);
TDP_API tdp_char_t* tdp_parse_escape_sequence  (tdp_char_t* begin, tdp_char_t* end);
TDP_API tdp_char_t* tdp_parse_token_char       (tdp_char_t* begin, tdp_char_t* end, tdp_char_t token);





#endif




