#ifndef __td_ini__h__
#define __td_ini__h__

/****************************************************************************
**
** File: td_ini.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
struct _td_ini_t;

typedef void (*td_ini_handler_t)(struct _td_ini_t* ctx);

typedef enum _td_ini_state_t
{
	TD_INI_STATE_ERROR          = 0,
	TD_INI_STATE_SCAN           = 1,
	TD_INI_STATE_DONE           = 2,
	TD_INI_STATE_TOKEN_LINE     = 3,
	TD_INI_STATE_TOKEN_COMMENT  = 4,
	TD_INI_STATE_TOKEN_SECTION  = 5,
	TD_INI_STATE_TOKEN_VARIABLE = 6,
	TD_INI_STATE_TOKEN_VALUE    = 7
}
td_ini_state_t;

typedef struct _td_ini_t
{
	td_pointer_t parameter;


	td_ini_handler_t handler_line    ;

	td_ini_handler_t handler_comment ;
	td_ini_handler_t handler_section ;
	td_ini_handler_t handler_variable;
	td_ini_handler_t handler_value   ;

	td_ini_handler_t handler_element ;


	td_string_t stream        ;
	td_string_t parsing       ;
	td_string_t token_line    ;
	td_string_t token_comment ;
	td_string_t token_section ;
	td_string_t token_variable;
	td_string_t token_value   ;


	td_ini_state_t state;
}
td_ini_t;




/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 초기화 설정
//===========================================================================
TD_API void td_ini_initialize           (td_ini_t* ctx);
TD_API void td_ini_set_parameter        (td_ini_t* ctx, td_pointer_t parameter);
TD_API void td_ini_set_handler_line     (td_ini_t* ctx, td_ini_handler_t handler);
TD_API void td_ini_set_handler_comment  (td_ini_t* ctx, td_ini_handler_t handler);
TD_API void td_ini_set_handler_section  (td_ini_t* ctx, td_ini_handler_t handler);
TD_API void td_ini_set_handler_variable (td_ini_t* ctx, td_ini_handler_t handler);
TD_API void td_ini_set_handler_value    (td_ini_t* ctx, td_ini_handler_t handler);
TD_API void td_ini_set_handler_element  (td_ini_t* ctx, td_ini_handler_t handler);



//===========================================================================
// parsing
//===========================================================================
TD_API void td_ini_parse (td_ini_t* ctx, td_pointer_t pointer, td_uint_t size);



//===========================================================================
// handler에서 호출
//===========================================================================
TD_API td_string_t* td_ini_comment (td_ini_t* ctx);
TD_API td_string_t* td_ini_section (td_ini_t* ctx);
TD_API td_string_t* td_ini_variable(td_ini_t* ctx);
TD_API td_string_t* td_ini_value   (td_ini_t* ctx);



//===========================================================================
// handler에서 호출
//===========================================================================
TD_API td_bool_t td_ini_is_section          (td_ini_t* ctx, td_char_t* section);
TD_API td_bool_t td_ini_is_variable         (td_ini_t* ctx, td_char_t* variable);
TD_API td_bool_t td_ini_is_section_variable (td_ini_t* ctx, td_char_t* section, td_char_t* variable);



//===========================================================================
// handler에서 호출
//===========================================================================
TD_API void td_ini_set_error (td_ini_t* ctx);







#endif




