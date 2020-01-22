#ifndef __td_array__h__
#define __td_array__h__

/****************************************************************************
**
** File: td_array.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
struct _td_array_t;

typedef void (*td_array_handler_t)(struct _td_array_t* ctx);

typedef enum _td_array_state_t
{
	TD_ARRAY_STATE_ERROR               = 0,
	TD_ARRAY_STATE_DONE                = 1,
	TD_ARRAY_STATE_SCAN                = 2,
	TD_ARRAY_STATE_TOKEN_LINE          = 3,
	TD_ARRAY_STATE_SCAN_LINE           = 4,
	TD_ARRAY_STATE_TOKEN_ELEMENT_VALUE = 5
}
td_array_state_t;

typedef struct _td_array_t
{
	td_pointer_t parameter;


	td_array_handler_t handler_line    ;

	td_array_handler_t handler_element_value ;

	td_array_handler_t handler_element ;


	td_string_t stream             ;
	td_string_t parsing            ;
	td_string_t token_line         ;
	td_string_t token_element_value;

	td_array_state_t state;
	td_uint_t        element_index;


	td_char_t* error_position;
	td_uint_t  error_line    ;
	td_uint_t  error_column  ;
}
td_array_t;




/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 초기화 설정
//===========================================================================
TD_API void td_array_initialize                (td_array_t* ctx);
TD_API void td_array_set_parameter             (td_array_t* ctx, td_pointer_t parameter);
TD_API void td_array_set_handler_line          (td_array_t* ctx, td_array_handler_t handler);
TD_API void td_array_set_handler_element_value (td_array_t* ctx, td_array_handler_t handler);
TD_API void td_array_set_handler_element       (td_array_t* ctx, td_array_handler_t handler);



//===========================================================================
// parsing
//===========================================================================
TD_API void td_array_parse (td_array_t* ctx, td_pointer_t pointer, td_uint_t size);



//===========================================================================
// handler에서 호출
//===========================================================================
TD_API td_pointer_t td_array_parameter    (td_array_t* ctx);
TD_API td_string_t* td_array_line         (td_array_t* ctx);
TD_API td_string_t* td_array_element_value(td_array_t* ctx);
TD_API td_uint_t    td_array_element_index(td_array_t* ctx);


//===========================================================================
// handler에서 호출
//===========================================================================
TD_API void td_array_set_error (td_array_t* ctx, td_char_t* position);





#endif




