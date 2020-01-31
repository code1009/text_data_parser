#ifndef __tdp_array__h__
#define __tdp_array__h__

/****************************************************************************
**
** File: tdp_array.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
struct _tdp_array_t;

typedef void (*tdp_array_handler_t)(struct _tdp_array_t* ctx);

typedef enum _tdp_array_state_t
{
	TDP_ARRAY_STATE_ERROR               = 0,
	TDP_ARRAY_STATE_DONE                = 1,
	TDP_ARRAY_STATE_SCAN                = 2,
	TDP_ARRAY_STATE_TOKEN_LINE          = 3,
	TDP_ARRAY_STATE_SCAN_LINE           = 4,
	TDP_ARRAY_STATE_TOKEN_ELEMENT_VALUE = 5
}
tdp_array_state_t;

typedef struct _tdp_array_t
{
	tdp_pointer_t parameter;


	tdp_array_handler_t handler_line;

	tdp_array_handler_t handler_element_value;

	tdp_array_handler_t handler_element;


	tdp_string_t stream             ;
	tdp_string_t parsing            ;
	tdp_string_t token_line         ;
	tdp_string_t token_element_value;

	tdp_array_state_t state;
	tdp_uint_t        element_index;


	tdp_char_t* error_position;
	tdp_uint_t  error_line    ;
	tdp_uint_t  error_column  ;
}
tdp_array_t;





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 초기화 설정
//===========================================================================
TDP_API void tdp_array_initialize                (tdp_array_t* ctx);
TDP_API void tdp_array_set_parameter             (tdp_array_t* ctx, tdp_pointer_t parameter);
TDP_API void tdp_array_set_handler_line          (tdp_array_t* ctx, tdp_array_handler_t handler);
TDP_API void tdp_array_set_handler_element_value (tdp_array_t* ctx, tdp_array_handler_t handler);
TDP_API void tdp_array_set_handler_element       (tdp_array_t* ctx, tdp_array_handler_t handler);



//===========================================================================
// parsing
//===========================================================================
TDP_API void tdp_array_parse (tdp_array_t* ctx, tdp_char_t* spointer, tdp_uint_t slength);



//===========================================================================
// handler에서 호출
//===========================================================================
TDP_API tdp_pointer_t tdp_array_parameter    (tdp_array_t* ctx);
TDP_API tdp_string_t* tdp_array_line         (tdp_array_t* ctx);
TDP_API tdp_string_t* tdp_array_element_value(tdp_array_t* ctx);
TDP_API tdp_uint_t    tdp_array_element_index(tdp_array_t* ctx);


//===========================================================================
// handler에서 호출
//===========================================================================
TDP_API void tdp_array_set_error (tdp_array_t* ctx, tdp_char_t* position);





#endif




