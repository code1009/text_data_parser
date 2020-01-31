#ifndef __tdp_ini__h__
#define __tdp_ini__h__

/****************************************************************************
**
** File: tdp_ini.h
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
struct _tdp_ini_t;

typedef void (*tdp_ini_handler_t)(struct _tdp_ini_t* ctx);

typedef enum _tdp_ini_state_t
{
	TDP_INI_STATE_ERROR          = 0,
	TDP_INI_STATE_DONE           = 1,
	TDP_INI_STATE_SCAN           = 2,
	TDP_INI_STATE_TOKEN_LINE     = 3,
	TDP_INI_STATE_SCAN_LINE      = 4,
	TDP_INI_STATE_TOKEN_COMMENT  = 5,
	TDP_INI_STATE_TOKEN_SECTION  = 6,
	TDP_INI_STATE_TOKEN_VARIABLE = 7,
	TDP_INI_STATE_TOKEN_VALUE    = 8
}
tdp_ini_state_t;

typedef struct _tdp_ini_t
{
	tdp_pointer_t parameter;


	tdp_ini_handler_t handler_line;

	tdp_ini_handler_t handler_comment ;
	tdp_ini_handler_t handler_section ;
	tdp_ini_handler_t handler_variable;
	tdp_ini_handler_t handler_value   ;

	tdp_ini_handler_t handler_element;


	tdp_string_t stream        ;
	tdp_string_t parsing       ;
	tdp_string_t token_line    ;
	tdp_string_t token_comment ;
	tdp_string_t token_section ;
	tdp_string_t token_variable;
	tdp_string_t token_value   ;


	tdp_ini_state_t state;


	tdp_char_t* error_position;
	tdp_uint_t  error_line    ;
	tdp_uint_t  error_column  ;
}
tdp_ini_t;





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 초기화 설정
//===========================================================================
TDP_API void tdp_ini_initialize           (tdp_ini_t* ctx);
TDP_API void tdp_ini_set_parameter        (tdp_ini_t* ctx, tdp_pointer_t parameter);
TDP_API void tdp_ini_set_handler_line     (tdp_ini_t* ctx, tdp_ini_handler_t handler);
TDP_API void tdp_ini_set_handler_comment  (tdp_ini_t* ctx, tdp_ini_handler_t handler);
TDP_API void tdp_ini_set_handler_section  (tdp_ini_t* ctx, tdp_ini_handler_t handler);
TDP_API void tdp_ini_set_handler_variable (tdp_ini_t* ctx, tdp_ini_handler_t handler);
TDP_API void tdp_ini_set_handler_value    (tdp_ini_t* ctx, tdp_ini_handler_t handler);
TDP_API void tdp_ini_set_handler_element  (tdp_ini_t* ctx, tdp_ini_handler_t handler);



//===========================================================================
// parsing
//===========================================================================
TDP_API void tdp_ini_parse (tdp_ini_t* ctx, tdp_char_t* spointer, tdp_uint_t slength);



//===========================================================================
// handler에서 호출
//===========================================================================
TDP_API tdp_pointer_t tdp_ini_parameter(tdp_ini_t* ctx);
TDP_API tdp_string_t* tdp_ini_line     (tdp_ini_t* ctx);
TDP_API tdp_string_t* tdp_ini_comment  (tdp_ini_t* ctx);
TDP_API tdp_string_t* tdp_ini_section  (tdp_ini_t* ctx);
TDP_API tdp_string_t* tdp_ini_variable (tdp_ini_t* ctx);
TDP_API tdp_string_t* tdp_ini_value    (tdp_ini_t* ctx);



//===========================================================================
// handler에서 호출
//===========================================================================
TDP_API tdp_bool_t tdp_ini_is_section          (tdp_ini_t* ctx, tdp_char_t* section);
TDP_API tdp_bool_t tdp_ini_is_variable         (tdp_ini_t* ctx, tdp_char_t* variable);
TDP_API tdp_bool_t tdp_ini_is_section_variable (tdp_ini_t* ctx, tdp_char_t* section, tdp_char_t* variable);



//===========================================================================
// handler에서 호출
//===========================================================================
TDP_API void tdp_ini_set_error (tdp_ini_t* ctx, tdp_char_t* position);





#endif




