/****************************************************************************
**
** File: td_ini.c
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include "td.h"





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
void td_ini_initialize (td_ini_t* ctx)
{
	//-----------------------------------------------------------------------
	ctx->parameter        = TD_NULL_POINTER;	
	ctx->handler_line     = TD_NULL_POINTER;	
	ctx->handler_comment  = TD_NULL_POINTER;
	ctx->handler_section  = TD_NULL_POINTER;
	ctx->handler_variable = TD_NULL_POINTER;
	ctx->handler_value    = TD_NULL_POINTER;
	ctx->handler_element  = TD_NULL_POINTER;


	//-----------------------------------------------------------------------
	td_string_set_null(&ctx->stream);
	td_string_set_null(&ctx->parsing);
	td_string_set_null(&ctx->token_line);
	td_string_set_null(&ctx->token_comment);
	td_string_set_null(&ctx->token_section);
	td_string_set_null(&ctx->token_variable);
	td_string_set_null(&ctx->token_value);


	//-----------------------------------------------------------------------
	ctx->state = TD_INI_STATE_ERROR;
}

//===========================================================================
void td_ini_set_parameter        (td_ini_t* ctx, td_pointer_t parameter)  { ctx->parameter = parameter; };
void td_ini_set_handler_line     (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_line     = handler; };
void td_ini_set_handler_comment  (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_comment  = handler; };
void td_ini_set_handler_section  (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_section  = handler; };
void td_ini_set_handler_variable (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_variable = handler; };
void td_ini_set_handler_value    (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_value    = handler; };
void td_ini_set_handler_element  (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_element  = handler; };

//===========================================================================
static void td_ini_transition_state (td_ini_t* ctx, td_ini_state_t state)
{
	if (ctx->state != state)
	{
		ctx->parsing_begin = TD_NULL_POINTER;
		ctx->parsing_end   = TD_NULL_POINTER;

		if (TD_PARSER_STATUS_ERROR!=ctx->status)
		{
			if (TD_PARSING_STATE_COMMENT == ctx->parsing_state)
			{
				if (ctx->comment_handler)
				{
					ctx->comment_handler(ctx);
				}
			}

			if (TD_PARSING_STATE_SECTION == ctx->parsing_state)
			{
				if (ctx->section_handler)
				{
					ctx->section_handler(ctx);
				}
			}

			if (TD_PARSING_STATE_VARIABLE == ctx->parsing_state)
			{
				if (ctx->variable_handler)
				{
					ctx->variable_handler(ctx);
				}
			}

			if (TD_PARSING_STATE_VALUE == ctx->parsing_state)
			{
				if (ctx->value_handler)
				{
					ctx->value_handler(ctx);
				}

				if (ctx->element_handler)
				{
					ctx->element_handler(ctx);
				}
			}
		}
	}

	ctx->parsing_state = parsing_state;
}

//===========================================================================
static void td_ini_state_token_line (td_ini_t* ctx)
{
	td_char_t* p;
	td_char_t  ch;

	td_uint_t flag_multi_line;
	td_uint_t flag_multi_line_cr;

	
	// "\n"
	// "\r\n"

	// "\\\n"
	// "\\\r\n"

	// "\\\\\n"   : X
	// "\\\\\r\n" : X


	flag_multi_line    = TD_FALSE;
	flag_multi_line_cr = TD_FALSE;

	for (p=ctx->parsing.begin; p!=ctx->stream.end; p++)
	{
		ch = *p;


		switch (ch)
		{
		case '\\':
			if ( TD_FALSE==flag_multi_line )
			{
				flag_multi_line = TD_TRUE;
			}
			else
			{
				flag_multi_line = TD_FALSE;
			}
			break;

		case '\r':
			if (TD_TRUE==flag_multi_line)
			{
				flag_multi_line_cr = TD_TRUE;
			}
			break;

		case '\n':
			if ( (TD_TRUE==flag_multi_line    ) &&
			     (TD_TRUE==flag_multi_line_cr )  )
			{
				flag_multi_line    = TD_FALSE;
				flag_multi_line_cr = TD_FALSE;
			}
			else
			{
				ctx->parsing.end    = p+1u;
				ctx->parsing.length = ctx->parsing.begin-ctx->parsing.end;

				td_string_shallow_copy (&ctx->token_line, &ctx->parsing);

				td_string_set_null (&ctx->parsing);
				ctx->parsing.begin = p+1u;

				td_ini_transition_state(ctx, TD_INI_STATE_DONE);
				return;
			}
			break;

		default:
			flag_multi_line    = TD_FALSE;
			flag_multi_line_cr = TD_FALSE;
			break;
		}
	}


	td_ini_transition_state(ctx, TD_INI_STATE_ERROR);
	return;
}

static void td_ini_state_scan (td_ini_t* ctx)
{
	if ( ctx->parsing.begin==ctx->stream.end )
	{
		td_ini_transition_state(ctx, TD_INI_STATE_DONE);
		return;
	}
	else
	{
		td_ini_transition_state(ctx, TD_INI_STATE_TOKEN_LINE);
		return;
	}
}

static void td_ini_state_machine (td_ini_t* ctx)
{
	td_bool_t loop;


	loop = TD_TRUE;
	while (loop)
	{
		switch (ctx->state)
		{
		case TD_INI_STATE_SCAN:
			td_ini_state_scan(ctx);
			break;

		case TD_INI_STATE_TOKEN_LINE:
			td_ini_state_token_line(ctx);
			break;

		default:
			loop = TD_FALSE;
			break;
		}
	}
}



//===========================================================================
void td_ini_parse (td_ini_t* ctx, td_pointer_t pointer, td_uint_t size)
{
	td_char_t* p;


	//-----------------------------------------------------------------------
	p = (td_char_t*) pointer;

	//-----------------------------------------------------------------------
	ctx->stream.begin  = p;
	ctx->stream.end    = p + size;
	ctx->stream.length = size;


	//-----------------------------------------------------------------------
	ctx->parsing.begin  = ctx->stream.begin;
	ctx->parsing.end    = TD_NULL_POINTER;
	ctx->parsing.length = 0u;


	//-----------------------------------------------------------------------
	td_string_set_null(&ctx->token_line);
	td_string_set_null(&ctx->token_comment);
	td_string_set_null(&ctx->token_section);
	td_string_set_null(&ctx->token_variable);
	td_string_set_null(&ctx->token_value);

	
	//-----------------------------------------------------------------------
	if ( (TD_NULL_POINTER!=ctx->parsing.begin) && (0u<ctx->stream.length) )
	{
		ctx->state = TD_INI_STATE_SCAN;

		td_ini_state_machine (ctx);
	}
	else
	{
		ctx->state = TD_INI_STATE_DONE;
	}
}

//===========================================================================
void td_ini_set_error (td_ini_t* ctx)
{
}

//===========================================================================
td_string_t* td_ini_comment (td_ini_t* ctx)
{
	return &ctx->token_comment;
}

td_string_t* td_ini_section (td_ini_t* ctx)
{
	return &ctx->token_section;
}

td_string_t* td_ini_variable (td_ini_t* ctx)
{
	return &ctx->token_variable;
}

td_string_t* td_ini_value (td_ini_t* ctx)
{
	return &ctx->token_value;
}

//===========================================================================
td_bool_t td_ini_is_section (td_ini_t* ctx, td_char_t* section)
{
	if (TD_FALSE==td_string_compare(td_ini_section(ctx), section, TD_FALSE))
	{
		return TD_FALSE;
	}

	return TD_TRUE;
}

td_bool_t td_ini_is_variable (td_ini_t* ctx, td_char_t* variable)
{
	if (TD_FALSE==td_string_compare(td_ini_variable(ctx), variable, TD_FALSE))
	{
		return TD_FALSE;
	}

	return TD_TRUE;
}

td_bool_t td_ini_is_section_variable (td_ini_t* ctx, td_char_t* section, td_char_t* variable)
{
	if (TD_FALSE==td_string_compare(td_ini_section(ctx), section, TD_FALSE))
	{
		return TD_FALSE;
	}

	if (TD_FALSE==td_string_compare(td_ini_variable(ctx), variable, TD_FALSE))
	{
		return TD_FALSE;
	}

	return TD_TRUE;
}




