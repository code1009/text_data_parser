/****************************************************************************
**
** File: tdp_array.c
**
** Created by MOON, Eui-kwon.
** Created on Jan-10th, 2020.
**
****************************************************************************/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include "tdp.h"





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
void tdp_array_initialize (tdp_array_t* ctx)
{
	//-----------------------------------------------------------------------
	ctx->parameter             = TDP_NULL_POINTER;
	ctx->handler_line          = TDP_NULL_POINTER;
	ctx->handler_element_value = TDP_NULL_POINTER;
	ctx->handler_element       = TDP_NULL_POINTER;


	//-----------------------------------------------------------------------
	tdp_string_clear(&ctx->stream             );
	tdp_string_clear(&ctx->parsing            );
	tdp_string_clear(&ctx->token_line         );
	tdp_string_clear(&ctx->token_element_value);


	//-----------------------------------------------------------------------
	ctx->state = TDP_ARRAY_STATE_ERROR;


	//-----------------------------------------------------------------------
	ctx->element_index = 0u;


	//-----------------------------------------------------------------------
	ctx->error_position = TDP_NULL_POINTER;
	ctx->error_line     = 0u;
	ctx->error_column   = 0u;
}

//===========================================================================
tdp_pointer_t tdp_array_parameter     (tdp_array_t* ctx){return ctx->parameter           ;}
tdp_string_t* tdp_array_line          (tdp_array_t* ctx){return &ctx->token_line         ;}
tdp_string_t* tdp_array_element_value (tdp_array_t* ctx){return &ctx->token_element_value;}
tdp_uint_t    tdp_array_element_index (tdp_array_t* ctx){return ctx->element_index       ;}

//===========================================================================
void tdp_array_set_parameter             (tdp_array_t* ctx, tdp_pointer_t parameter)    { ctx->parameter = parameter; };
void tdp_array_set_handler_line          (tdp_array_t* ctx, tdp_array_handler_t handler){ ctx->handler_line          = handler; };
void tdp_array_set_handler_element_value (tdp_array_t* ctx, tdp_array_handler_t handler){ ctx->handler_element_value = handler; };
void tdp_array_set_handler_element       (tdp_array_t* ctx, tdp_array_handler_t handler){ ctx->handler_element       = handler; };

//===========================================================================
static void tdp_array_handle_line (tdp_array_t* ctx)
{
	tdp_string_trim(tdp_array_line(ctx));

	if (ctx->handler_line)
	{
		ctx->handler_line(ctx);
	}
}

static void tdp_array_handle_element_value (tdp_array_t* ctx)
{
	tdp_string_trim(tdp_array_element_value(ctx));
	tdp_string_trim_dquotes(tdp_array_element_value(ctx));

	if (ctx->handler_element_value)
	{
		ctx->handler_element_value(ctx);
	}
}

static void tdp_array_handle_element (tdp_array_t* ctx)
{
	if (ctx->handler_element)
	{
		ctx->handler_element(ctx);
	}

	ctx->element_index++;
}

//===========================================================================
static void tdp_array_transition_state (tdp_array_t* ctx, tdp_array_state_t state)
{
	if (ctx->state != state)
	{
		if (TDP_ARRAY_STATE_ERROR!=state)
		{
			switch (ctx->state)
			{
			case TDP_ARRAY_STATE_TOKEN_LINE:
				tdp_array_handle_line(ctx);
				break;

			case TDP_ARRAY_STATE_TOKEN_ELEMENT_VALUE:
				tdp_array_handle_element_value(ctx);
				tdp_array_handle_element(ctx);
				break;

			default:
				break;
			}
		}
	}

	ctx->state = state;
}

//===========================================================================
static void tdp_array_state_token_element_value (tdp_array_t* ctx)
{
	tdp_char_t* s;
	tdp_char_t  ch;

	tdp_char_t* found;


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
			found = tdp_parse_escape_sequence (s, ctx->token_line.end);
			if (TDP_NULL_POINTER==found)
			{
				if ( (s+1)!=ctx->token_line.end )
				{
					tdp_array_set_error(ctx, ctx->token_element_value.begin);
					return;
				}
			}
			else
			{
				s = found;
			}
			break;
		case '\"':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '\"');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '\'':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '\'');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '{':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '}');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '(':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, ')');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;
		case '[':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, ']');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_element_value.begin);
				return;
			}
			break;

		case ',':
			{
				tdp_string_end(&ctx->token_element_value, s);

				ctx->token_line.begin  = s+1;
				ctx->token_line.length = ctx->token_line.end - ctx->token_line.begin;

				tdp_array_transition_state(ctx, TDP_ARRAY_STATE_SCAN_LINE);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	tdp_string_end(&ctx->token_element_value, s);

	tdp_array_transition_state(ctx, TDP_ARRAY_STATE_SCAN);
	return;
}

static void tdp_array_state_scan_line (tdp_array_t* ctx)
{
	//-----------------------------------------------------------------------
	if ( 0u==ctx->token_line.length )
	{
		tdp_string_begin(&ctx->token_element_value, ctx->token_line.begin);

		tdp_array_transition_state(ctx, TDP_ARRAY_STATE_TOKEN_ELEMENT_VALUE);
		return;
	}


	//-----------------------------------------------------------------------
	tdp_char_t* s;
/*
	tdp_char_t  ch;


	for (s=ctx->token_line.begin; s!=ctx->token_line.end; )
	{
		ch = *s;

		switch (ch)
		{
		default:
			{
				tdp_string_begin(&ctx->token_element_value, s);

				tdp_array_transition_state(ctx, TDP_ARRAY_STATE_TOKEN_ELEMENT_VALUE);
				return;
			}
			break;
		}
	}
*/


	//-----------------------------------------------------------------------
	s=ctx->token_line.begin;

	tdp_string_begin(&ctx->token_element_value, s);

	tdp_array_transition_state(ctx, TDP_ARRAY_STATE_TOKEN_ELEMENT_VALUE);
	return;
}

static void tdp_array_state_token_line (tdp_array_t* ctx)
{
	tdp_char_t* s;
	tdp_char_t  ch;


	for (s=ctx->token_line.begin; s!=ctx->stream.end; s++)
	{
		ch = *s;


		switch (ch)
		{
		case '\n':
			{
				tdp_string_end(&ctx->parsing, s+1);

				tdp_string_end(&ctx->token_line, s+1);

				tdp_array_transition_state(ctx, TDP_ARRAY_STATE_SCAN_LINE);
				return;
			}
			break;

		case '\r':
			break;

		case '\t':
			break;

		case '\\':
			s = tdp_parse_escape_sequence (s, ctx->stream.end);
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '\"':
			s = tdp_parse_token_char (s+1, ctx->stream.end, '\"');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '\'':
			s = tdp_parse_token_char (s+1, ctx->stream.end, '\'');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '{':
			s = tdp_parse_token_char (s+1, ctx->stream.end, '}');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '(':
			s = tdp_parse_token_char (s+1, ctx->stream.end, ')');
			if (TDP_NULL_POINTER==s)
			{
				tdp_array_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	tdp_string_end(&ctx->parsing, s);

	tdp_string_end(&ctx->token_line, s);

	tdp_array_transition_state(ctx, TDP_ARRAY_STATE_SCAN_LINE);
	return;
}

static void tdp_array_state_scan (tdp_array_t* ctx)
{
	if ( ctx->parsing.end==ctx->stream.end )
	{
		tdp_array_transition_state(ctx, TDP_ARRAY_STATE_DONE);
		return;
	}
	else
	{
		if ( ctx->parsing.end!=TDP_NULL_POINTER )
		{
			tdp_array_set_error(ctx, ctx->parsing.end);
			return;
		}


		tdp_string_begin(&ctx->token_line, ctx->parsing.begin);

		tdp_array_transition_state(ctx, TDP_ARRAY_STATE_TOKEN_LINE);
		return;
	}
}

static void tdp_array_state_machine (tdp_array_t* ctx)
{
	tdp_bool_t loop;


	loop = TDP_TRUE;
	while (loop)
	{
		switch (ctx->state)
		{
		case TDP_ARRAY_STATE_SCAN:
			tdp_array_state_scan(ctx);
			break;

		case TDP_ARRAY_STATE_TOKEN_LINE:
			tdp_array_state_token_line(ctx);
			break;

		case TDP_ARRAY_STATE_SCAN_LINE:
			tdp_array_state_scan_line(ctx);
			break;

		case TDP_ARRAY_STATE_TOKEN_ELEMENT_VALUE:
			tdp_array_state_token_element_value(ctx);
			break;

		default:
			loop = TDP_FALSE;
			break;
		}
	}
}



//===========================================================================
void tdp_array_parse (tdp_array_t* ctx, tdp_char_t* spointer, tdp_uint_t slength)
{
	//-----------------------------------------------------------------------
	tdp_string_begin(&ctx->stream, spointer);
	tdp_string_end  (&ctx->stream, spointer + slength);


	//-----------------------------------------------------------------------
	tdp_string_begin(&ctx->parsing, ctx->stream.begin);


	//-----------------------------------------------------------------------
	tdp_string_clear(&ctx->token_line         );
	tdp_string_clear(&ctx->token_element_value);


	//-----------------------------------------------------------------------
	ctx->element_index = 0u;


	//-----------------------------------------------------------------------
	ctx->error_position = TDP_NULL_POINTER;
	ctx->error_line     = 0u;
	ctx->error_column   = 0u;


	//-----------------------------------------------------------------------
	if ( (TDP_NULL_POINTER!=ctx->parsing.begin) && (0u<ctx->stream.length) )
	{
		ctx->state = TDP_ARRAY_STATE_SCAN;

		tdp_array_state_machine (ctx);
	}
	else
	{
		ctx->state = TDP_ARRAY_STATE_DONE;
	}
}



//===========================================================================
void tdp_array_set_error (tdp_array_t* ctx, tdp_char_t* position)
{
	tdp_char_t* s;
	tdp_char_t  ch;

	tdp_uint_t  column;
	tdp_uint_t  line  ;


	if (TDP_FALSE==tdp_string_empty(&ctx->stream))
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


	tdp_array_transition_state(ctx, TDP_ARRAY_STATE_ERROR);
}




