/****************************************************************************
**
** File: td_array.c
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
void td_array_initialize (td_array_t* ctx)
{
	//-----------------------------------------------------------------------
	ctx->parameter             = TD_NULL_POINTER;
	ctx->handler_line          = TD_NULL_POINTER;
	ctx->handler_element_value = TD_NULL_POINTER;
	ctx->handler_element       = TD_NULL_POINTER;


	//-----------------------------------------------------------------------
	td_string_clear(&ctx->stream             );
	td_string_clear(&ctx->parsing            );
	td_string_clear(&ctx->token_line         );
	td_string_clear(&ctx->token_element_value);


	//-----------------------------------------------------------------------
	ctx->state = TD_ARRAY_STATE_ERROR;


	//-----------------------------------------------------------------------
	ctx->element_index = 0u;


	//-----------------------------------------------------------------------
	ctx->error_position = TD_NULL_POINTER;
	ctx->error_line     = 0u;
	ctx->error_column   = 0u;
}

//===========================================================================
td_pointer_t td_array_parameter     (td_array_t* ctx){return ctx->parameter           ;}
td_string_t* td_array_line          (td_array_t* ctx){return &ctx->token_line         ;}
td_string_t* td_array_element_value (td_array_t* ctx){return &ctx->token_element_value;}
td_uint_t    td_array_element_index (td_array_t* ctx){return ctx->element_index       ;}

//===========================================================================
void td_array_set_parameter             (td_array_t* ctx, td_pointer_t parameter)    { ctx->parameter = parameter; };
void td_array_set_handler_line          (td_array_t* ctx, td_array_handler_t handler){ ctx->handler_line          = handler; };
void td_array_set_handler_element_value (td_array_t* ctx, td_array_handler_t handler){ ctx->handler_element_value = handler; };
void td_array_set_handler_element       (td_array_t* ctx, td_array_handler_t handler){ ctx->handler_element       = handler; };

//===========================================================================
static void td_array_handle_line (td_array_t* ctx)
{
	td_string_trim(td_array_line(ctx));

	if (ctx->handler_line)
	{
		ctx->handler_line(ctx);
	}
}

static void td_array_handle_element_value (td_array_t* ctx)
{
	td_string_trim(td_array_element_value(ctx));
	td_string_trim_dquotes(td_array_element_value(ctx));

	if (ctx->handler_element_value)
	{
		ctx->handler_element_value(ctx);
	}
}

static void td_array_handle_element (td_array_t* ctx)
{
	if (ctx->handler_element)
	{
		ctx->handler_element(ctx);
	}

	ctx->element_index++;
}

//===========================================================================
static void td_array_transition_state (td_array_t* ctx, td_array_state_t state)
{
	if (ctx->state != state)
	{
		if (TD_ARRAY_STATE_ERROR!=state)
		{
			switch (ctx->state)
			{
			case TD_ARRAY_STATE_TOKEN_LINE:
				td_array_handle_line(ctx);
				break;

			case TD_ARRAY_STATE_TOKEN_ELEMENT_VALUE:
				td_array_handle_element_value(ctx);
				td_array_handle_element(ctx);
				break;

			default:
				break;
			}
		}
	}

	ctx->state = state;
}

//===========================================================================
static void td_array_state_token_element_value (td_array_t* ctx)
{
	td_char_t* s;
	td_char_t  ch;

	td_char_t* found;


	for (s=ctx->token_element_value.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\n':
		case '\r':
		case '\t':
			break;

		case '\\':
			found = td_parse_escape_sequence (s, ctx->token_line.end);
			if (TD_NULL_POINTER==found)
			{
				if ( (s+1)!=ctx->token_line.end )
				{
					td_array_set_error(ctx, ctx->token_element_value.begin);
					return;
				}
			}
			else
			{
				s = found;
			}
			break;
		case '\"':
			s = td_parse_token_char (s+1, ctx->token_line.end, '\"');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '\'':
			s = td_parse_token_char (s+1, ctx->token_line.end, '\'');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '{':
			s = td_parse_token_char (s+1, ctx->token_line.end, '}');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '(':
			s = td_parse_token_char (s+1, ctx->token_line.end, ')');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '[':
			s = td_parse_token_char (s+1, ctx->token_line.end, ']');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;

		case ',':
			{
				td_string_end(&ctx->token_element_value, s);

				ctx->token_line.begin  = s+1;
				ctx->token_line.length = ctx->token_line.end - ctx->token_line.begin;

				td_array_transition_state(ctx, TD_ARRAY_STATE_SCAN_LINE);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	td_string_end(&ctx->token_element_value, s);

	td_array_transition_state(ctx, TD_ARRAY_STATE_SCAN);
	return;
}

static void td_array_state_scan_line (td_array_t* ctx)
{
	//-----------------------------------------------------------------------
	if ( 0u==ctx->token_line.length )
	{
		td_string_begin(&ctx->token_element_value, ctx->token_line.begin);

		td_array_transition_state(ctx, TD_ARRAY_STATE_TOKEN_ELEMENT_VALUE);
		return;
	}


	//-----------------------------------------------------------------------
	td_char_t* s;
	td_char_t  ch;


	for (s=ctx->token_line.begin; s!=ctx->token_line.end; )
	{
		ch = *s;

		switch (ch)
		{
		default:
			{
				td_string_begin(&ctx->token_element_value, s);

				td_array_transition_state(ctx, TD_ARRAY_STATE_TOKEN_ELEMENT_VALUE);
				return;
			}
			break;
		}
	}
}

static void td_array_state_token_line (td_array_t* ctx)
{
	td_char_t* s;
	td_char_t  ch;


	for (s=ctx->token_line.begin; s!=ctx->stream.end; s++)
	{
		ch = *s;


		switch (ch)
		{
		case '\n':
			{
				td_string_end(&ctx->parsing, s+1);

				td_string_end(&ctx->token_line, s+1);

				td_array_transition_state(ctx, TD_ARRAY_STATE_SCAN_LINE);
				return;
			}
			break;

		case '\r':
			break;

		case '\t':
			break;

		case '\\':
			s = td_parse_escape_sequence (s, ctx->stream.end);
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '\"':
			s = td_parse_token_char (s+1, ctx->stream.end, '\"');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '\'':
			s = td_parse_token_char (s+1, ctx->stream.end, '\'');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '{':
			s = td_parse_token_char (s+1, ctx->stream.end, '}');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '(':
			s = td_parse_token_char (s+1, ctx->stream.end, ')');
			if (TD_NULL_POINTER==s)
			{
				td_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	td_string_end(&ctx->parsing, s);

	td_string_end(&ctx->token_line, s);

	td_array_transition_state(ctx, TD_ARRAY_STATE_SCAN_LINE);
	return;
}

static void td_array_state_scan (td_array_t* ctx)
{
	if ( ctx->parsing.end==ctx->stream.end )
	{
		td_array_transition_state(ctx, TD_ARRAY_STATE_DONE);
		return;
	}
	else
	{
		if ( ctx->parsing.end!=TD_NULL_POINTER )
		{
			td_array_set_error(ctx, ctx->parsing.end);
			return;
		}


		td_string_begin(&ctx->token_line, ctx->parsing.begin);

		td_array_transition_state(ctx, TD_ARRAY_STATE_TOKEN_LINE);
		return;
	}
}

static void td_array_state_machine (td_array_t* ctx)
{
	td_bool_t loop;


	loop = TD_TRUE;
	while (loop)
	{
		switch (ctx->state)
		{
		case TD_ARRAY_STATE_SCAN:
			td_array_state_scan(ctx);
			break;

		case TD_ARRAY_STATE_TOKEN_LINE:
			td_array_state_token_line(ctx);
			break;

		case TD_ARRAY_STATE_SCAN_LINE:
			td_array_state_scan_line(ctx);
			break;

		case TD_ARRAY_STATE_TOKEN_ELEMENT_VALUE:
			td_array_state_token_element_value(ctx);
			break;

		default:
			loop = TD_FALSE;
			break;
		}
	}
}



//===========================================================================
void td_array_parse (td_array_t* ctx, td_pointer_t pointer, td_uint_t size)
{
	td_char_t* s;


	//-----------------------------------------------------------------------
	s = (td_char_t*) pointer;


	//-----------------------------------------------------------------------
	td_string_begin(&ctx->stream, s);
	td_string_end  (&ctx->stream, s + size);


	//-----------------------------------------------------------------------
	td_string_begin(&ctx->parsing, ctx->stream.begin);


	//-----------------------------------------------------------------------
	td_string_clear(&ctx->token_line         );
	td_string_clear(&ctx->token_element_value);


	//-----------------------------------------------------------------------
	ctx->element_index = 0u;


	//-----------------------------------------------------------------------
	ctx->error_position = TD_NULL_POINTER;
	ctx->error_line     = 0u;
	ctx->error_column   = 0u;


	//-----------------------------------------------------------------------
	if ( (TD_NULL_POINTER!=ctx->parsing.begin) && (0u<ctx->stream.length) )
	{
		ctx->state = TD_ARRAY_STATE_SCAN;

		td_array_state_machine (ctx);
	}
	else
	{
		ctx->state = TD_ARRAY_STATE_DONE;
	}
}



//===========================================================================
void td_array_set_error (td_array_t* ctx, td_char_t* position)
{
	td_char_t* s;
	td_char_t  ch;

	td_uint_t  column;
	td_uint_t  line  ;


	if (TD_FALSE==td_string_empty(&ctx->stream))
	{
		if (ctx->stream.begin<=position && position<=ctx->stream.end)
		{
			column = 1u;
			line   = 1u;

			for (s=ctx->stream.begin; s!=position; s++)
			{
				ch = *s;

				switch (ch)
				{
				case '\n':
					line++;
					column=1u;

				default:
					column++;
					break;
				}
			}

			ctx->error_position = position;
			ctx->error_line     = line;
			ctx->error_column   = column;
		}
	}


	td_array_transition_state(ctx, TD_ARRAY_STATE_ERROR);
}




