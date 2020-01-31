/****************************************************************************
**
** File: tdp_ini.c
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
void tdp_ini_initialize (tdp_ini_t* ctx)
{
	//-----------------------------------------------------------------------
	ctx->parameter        = TDP_NULL_POINTER;
	ctx->handler_line     = TDP_NULL_POINTER;
	ctx->handler_comment  = TDP_NULL_POINTER;
	ctx->handler_section  = TDP_NULL_POINTER;
	ctx->handler_variable = TDP_NULL_POINTER;
	ctx->handler_value    = TDP_NULL_POINTER;
	ctx->handler_element  = TDP_NULL_POINTER;


	//-----------------------------------------------------------------------
	tdp_string_clear(&ctx->stream        );
	tdp_string_clear(&ctx->parsing       );
	tdp_string_clear(&ctx->token_line    );
	tdp_string_clear(&ctx->token_comment );
	tdp_string_clear(&ctx->token_section );
	tdp_string_clear(&ctx->token_variable);
	tdp_string_clear(&ctx->token_value   );


	//-----------------------------------------------------------------------
	ctx->state = TDP_INI_STATE_ERROR;


	//-----------------------------------------------------------------------
	ctx->error_position = TDP_NULL_POINTER;
	ctx->error_line     = 0u;
	ctx->error_column   = 0u;
}

//===========================================================================
tdp_pointer_t tdp_ini_parameter(tdp_ini_t* ctx){return  ctx->parameter     ;}
tdp_string_t* tdp_ini_line     (tdp_ini_t* ctx){return &ctx->token_line    ;}
tdp_string_t* tdp_ini_comment  (tdp_ini_t* ctx){return &ctx->token_comment ;}
tdp_string_t* tdp_ini_section  (tdp_ini_t* ctx){return &ctx->token_section ;}
tdp_string_t* tdp_ini_variable (tdp_ini_t* ctx){return &ctx->token_variable;}
tdp_string_t* tdp_ini_value    (tdp_ini_t* ctx){return &ctx->token_value   ;}

//===========================================================================
void tdp_ini_set_parameter        (tdp_ini_t* ctx, tdp_pointer_t parameter)  { ctx->parameter = parameter; };
void tdp_ini_set_handler_line     (tdp_ini_t* ctx, tdp_ini_handler_t handler){ ctx->handler_line     = handler; };
void tdp_ini_set_handler_comment  (tdp_ini_t* ctx, tdp_ini_handler_t handler){ ctx->handler_comment  = handler; };
void tdp_ini_set_handler_section  (tdp_ini_t* ctx, tdp_ini_handler_t handler){ ctx->handler_section  = handler; };
void tdp_ini_set_handler_variable (tdp_ini_t* ctx, tdp_ini_handler_t handler){ ctx->handler_variable = handler; };
void tdp_ini_set_handler_value    (tdp_ini_t* ctx, tdp_ini_handler_t handler){ ctx->handler_value    = handler; };
void tdp_ini_set_handler_element  (tdp_ini_t* ctx, tdp_ini_handler_t handler){ ctx->handler_element  = handler; };

//===========================================================================
static void tdp_ini_handle_line (tdp_ini_t* ctx)
{
	tdp_string_trim(tdp_ini_line(ctx));

	if (ctx->handler_line)
	{
		ctx->handler_line(ctx);
	}
}

static void tdp_ini_handle_comment (tdp_ini_t* ctx)
{
	tdp_string_t* p;
	tdp_char_t    ch;


	p = tdp_ini_comment(ctx);
	if(0u<p->length)
	{
		ch = *(p->begin);

		if (' ' == ch)
		{
			p->begin++;
			p->length--;
		}
	}

	if (ctx->handler_comment)
	{
		ctx->handler_comment(ctx);
	}
}

static void tdp_ini_handle_section (tdp_ini_t* ctx)
{
	tdp_string_trim(tdp_ini_section(ctx));
	tdp_string_trim_dquotes(tdp_ini_section(ctx));


	if (ctx->handler_section)
	{
		ctx->handler_section(ctx);
	}
}

static void tdp_ini_handle_variable (tdp_ini_t* ctx)
{
	tdp_string_trim(tdp_ini_variable(ctx));
	tdp_string_trim_dquotes(tdp_ini_variable(ctx));


	if (ctx->handler_variable)
	{
		ctx->handler_variable(ctx);
	}
}

static void tdp_ini_handle_value (tdp_ini_t* ctx)
{
	tdp_string_trim(tdp_ini_value(ctx));
	tdp_string_trim_dquotes(tdp_ini_value(ctx));


	if (ctx->handler_value)
	{
		ctx->handler_value(ctx);
	}
}

static void tdp_ini_handle_element (tdp_ini_t* ctx)
{
	if (ctx->handler_element)
	{
		ctx->handler_element(ctx);
	}
}

//===========================================================================
static void tdp_ini_transition_state (tdp_ini_t* ctx, tdp_ini_state_t state)
{
	if (ctx->state != state)
	{
		if (TDP_INI_STATE_ERROR!=state)
		{
			switch (ctx->state)
			{
			case TDP_INI_STATE_TOKEN_LINE:
				tdp_ini_handle_line(ctx);
				break;

			case TDP_INI_STATE_TOKEN_COMMENT:
				tdp_ini_handle_comment(ctx);
				break;

			case TDP_INI_STATE_TOKEN_SECTION:
				tdp_ini_handle_section(ctx);
				break;

			case TDP_INI_STATE_TOKEN_VARIABLE:
				tdp_ini_handle_variable(ctx);
				break;

			case TDP_INI_STATE_TOKEN_VALUE:
				tdp_ini_handle_value(ctx);
				tdp_ini_handle_element(ctx);
				break;

			default:
				break;
			}
		}
	}

	ctx->state = state;
}

//===========================================================================
static void tdp_ini_state_token_value (tdp_ini_t* ctx)
{
	tdp_char_t* s;
/*
	tdp_char_t  ch;


	for (s=ctx->token_value.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		default:
			break;
		}
	}
*/
	s = ctx->token_line.end;


	//-----------------------------------------------------------------------
	tdp_string_end(&ctx->token_value, s);

	tdp_ini_transition_state(ctx, TDP_INI_STATE_SCAN);
	return;
}

static void tdp_ini_state_token_variable (tdp_ini_t* ctx)
{
	tdp_char_t* s;
	tdp_char_t  ch;

	
	for (s=ctx->token_variable.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\r':
		case '\n':
		case '\t':
			{
				tdp_ini_set_error(ctx, s);
				return;
			}
			break;

		case '\\':
			s = tdp_parse_escape_sequence (s, ctx->token_line.end);
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '\"':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '\"');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '\'':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '\'');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '{':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '}');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '(':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, ')');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '[':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, ']');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;

		case '=':
			{
				tdp_string_end(&ctx->token_variable, s);

				tdp_string_begin(&ctx->token_value, s+1);

				tdp_ini_transition_state(ctx, TDP_INI_STATE_TOKEN_VALUE);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	tdp_ini_set_error(ctx, s);
	return;
}

static void tdp_ini_state_token_section (tdp_ini_t* ctx)
{
	tdp_char_t* s;
	tdp_char_t  ch;

	
	for (s=ctx->token_section.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\n':
		case '\r':
		case '\t':
			{
				tdp_ini_set_error(ctx, s);
				return;
			}
			break;

		case '\\':
			s = tdp_parse_escape_sequence (s, ctx->token_line.end);
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '\"':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '\"');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '\'':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '\'');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '{':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, '}');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '(':
			s = tdp_parse_token_char (s+1, ctx->token_line.end, ')');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;

		case '[':
			{
				tdp_ini_set_error(ctx, s);
				return;
			}
			break;

		case ']':
			{
				if ( (s+1)!=ctx->token_line.end )
				{
					tdp_ini_set_error(ctx, s);
					return;
				}

				tdp_string_end(&ctx->token_section, s);

				tdp_ini_transition_state(ctx, TDP_INI_STATE_SCAN);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	tdp_ini_set_error(ctx, s);
	return;
}

static void tdp_ini_state_token_comment (tdp_ini_t* ctx)
{
	tdp_char_t* s;
	tdp_char_t  ch;

	
	for (s=ctx->token_comment.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\n':
		case '\r':
			{
				tdp_ini_set_error(ctx, s);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	tdp_string_end(&ctx->token_comment, s);

	tdp_ini_transition_state(ctx, TDP_INI_STATE_SCAN);
	return;
}

static void tdp_ini_state_scan_line (tdp_ini_t* ctx)
{
	//-----------------------------------------------------------------------
	if ( 0u==ctx->token_line.length )
	{
		tdp_ini_transition_state(ctx, TDP_INI_STATE_SCAN);
		return;
	}


	//-----------------------------------------------------------------------
	tdp_char_t* s;
	tdp_char_t  ch;


	for (s=ctx->token_line.begin; s!=ctx->token_line.end; )
	{
		ch = *s;

		switch (ch)
		{
		case ';':
			{
				tdp_string_begin(&ctx->token_comment, s+1);

				tdp_ini_transition_state(ctx, TDP_INI_STATE_TOKEN_COMMENT);
				return;
			}
			break;

		case '[':
			{
				tdp_string_begin(&ctx->token_section, s+1);

				tdp_ini_transition_state(ctx, TDP_INI_STATE_TOKEN_SECTION);
				return;
			}
			break;

		default:
			{
				tdp_string_begin(&ctx->token_variable, s);

				tdp_ini_transition_state(ctx, TDP_INI_STATE_TOKEN_VARIABLE);
				return;
			}
			break;
		}
	}
}

static void tdp_ini_state_token_line (tdp_ini_t* ctx)
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

				tdp_ini_transition_state(ctx, TDP_INI_STATE_SCAN_LINE);
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
				tdp_ini_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '\"':
			s = tdp_parse_token_char (s+1, ctx->stream.end, '\"');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '\'':
			s = tdp_parse_token_char (s+1, ctx->stream.end, '\'');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '{':
			s = tdp_parse_token_char (s+1, ctx->stream.end, '}');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;
		case '(':
			s = tdp_parse_token_char (s+1, ctx->stream.end, ')');
			if (TDP_NULL_POINTER==s)
			{
				tdp_ini_set_error(ctx, ctx->token_line.begin);
				return;
			}
			break;

		case ';':
			if (ctx->token_line.begin!=s)
			{
				tdp_string_end(&ctx->parsing, s);

				tdp_string_end(&ctx->token_line, s);

				tdp_ini_transition_state(ctx, TDP_INI_STATE_SCAN_LINE);
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

	tdp_ini_transition_state(ctx, TDP_INI_STATE_SCAN_LINE);
	return;
}

static void tdp_ini_state_scan (tdp_ini_t* ctx)
{
	if ( ctx->parsing.end==ctx->stream.end )
	{
		tdp_ini_transition_state(ctx, TDP_INI_STATE_DONE);
		return;
	}
	else
	{
		if ( ctx->parsing.end!=TDP_NULL_POINTER )
		{
			tdp_string_begin(&ctx->parsing, ctx->parsing.end);
		}


		tdp_string_begin(&ctx->token_line, ctx->parsing.begin);

		tdp_ini_transition_state(ctx, TDP_INI_STATE_TOKEN_LINE);
		return;
	}
}

static void tdp_ini_state_machine (tdp_ini_t* ctx)
{
	tdp_bool_t loop;


	loop = TDP_TRUE;
	while (loop)
	{
		switch (ctx->state)
		{
		case TDP_INI_STATE_SCAN:
			tdp_ini_state_scan(ctx);
			break;

		case TDP_INI_STATE_TOKEN_LINE:
			tdp_ini_state_token_line(ctx);
			break;

		case TDP_INI_STATE_SCAN_LINE:
			tdp_ini_state_scan_line(ctx);
			break;

		case TDP_INI_STATE_TOKEN_COMMENT:
			tdp_ini_state_token_comment(ctx);
			break;

		case TDP_INI_STATE_TOKEN_SECTION:
			tdp_ini_state_token_section(ctx);
			break;

		case TDP_INI_STATE_TOKEN_VARIABLE:
			tdp_ini_state_token_variable(ctx);
			break;

		case TDP_INI_STATE_TOKEN_VALUE:
			tdp_ini_state_token_value(ctx);
			break;

		default:
			loop = TDP_FALSE;
			break;
		}
	}
}



//===========================================================================
void tdp_ini_parse (tdp_ini_t* ctx, tdp_char_t* spointer, tdp_uint_t slength)
{
	//-----------------------------------------------------------------------
	tdp_string_begin(&ctx->stream, spointer);
	tdp_string_end  (&ctx->stream, spointer + slength);


	//-----------------------------------------------------------------------
	tdp_string_begin(&ctx->parsing, ctx->stream.begin);


	//-----------------------------------------------------------------------
	tdp_string_clear(&ctx->token_line    );
	tdp_string_clear(&ctx->token_comment );
	tdp_string_clear(&ctx->token_section );
	tdp_string_clear(&ctx->token_variable);
	tdp_string_clear(&ctx->token_value   );

	
	//-----------------------------------------------------------------------
	ctx->error_position = TDP_NULL_POINTER;
	ctx->error_line     = 0u;
	ctx->error_column   = 0u;


	//-----------------------------------------------------------------------
	if ( (TDP_NULL_POINTER!=ctx->parsing.begin) && (0u<ctx->stream.length) )
	{
		ctx->state = TDP_INI_STATE_SCAN;

		tdp_ini_state_machine (ctx);
	}
	else
	{
		ctx->state = TDP_INI_STATE_DONE;
	}
}



//===========================================================================
void tdp_ini_set_error (tdp_ini_t* ctx, tdp_char_t* position)
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


	tdp_ini_transition_state(ctx, TDP_INI_STATE_ERROR);
}



//===========================================================================
tdp_bool_t tdp_ini_is_section (tdp_ini_t* ctx, tdp_char_t* section)
{
	if (TDP_FALSE==tdp_string_compare(tdp_ini_section(ctx), section, TDP_FALSE))
	{
		return TDP_FALSE;
	}

	return TDP_TRUE;
}

tdp_bool_t tdp_ini_is_variable (tdp_ini_t* ctx, tdp_char_t* variable)
{
	if (TDP_FALSE==tdp_string_compare(tdp_ini_variable(ctx), variable, TDP_FALSE))
	{
		return TDP_FALSE;
	}

	return TDP_TRUE;
}

tdp_bool_t tdp_ini_is_section_variable (tdp_ini_t* ctx, tdp_char_t* section, tdp_char_t* variable)
{
	if (TDP_FALSE==tdp_string_compare(tdp_ini_section(ctx), section, TDP_FALSE))
	{
		return TDP_FALSE;
	}

	if (TDP_FALSE==tdp_string_compare(tdp_ini_variable(ctx), variable, TDP_FALSE))
	{
		return TDP_FALSE;
	}

	return TDP_TRUE;
}




