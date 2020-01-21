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
 static td_char_t* td_ini_parse_escape_sequence (td_char_t* begin, td_char_t* end)
 {
	 td_char_t* s;
	 td_char_t  ch;


	 if (begin>=end)
	 {
		return TD_NULL_POINTER;
	 }


	 ch = *begin;
	 if (ch!='\\')
	 {
		return TD_NULL_POINTER;
	 }
	 begin++;


	 for (s=begin; s!=end; s++)
	 {
		ch = *s;

		switch (ch)
		{
		case 'r':
		case 'n':
		case 't':
		case '\'':
		case '\"':
			{
				return s;
			}
			break;

		default:
			break;
		}
	 }

 	 return TD_NULL_POINTER;
 }

 static td_char_t* td_ini_parse_token_char (td_char_t* begin, td_char_t* end, td_char_t token)
 {
	 td_char_t* s;
	 td_char_t  ch;


	 for (s=begin; s!=end; s++)
	 {
		ch = *s;


		if (token==ch)
		{
			return s;
		}


		if (('\''==token) || ('\"'==token) )
		{
			switch (ch)
			{
			case '\\':
				s = td_ini_parse_escape_sequence(s, end);
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;

			default:
				break;
			}
		}
		else
		{
			switch (ch)
			{
			case '\\':
				s = td_ini_parse_escape_sequence(s, end);
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;

			case '\'':
				s=td_ini_parse_token_char(s+1, end, '\'');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '\"':
				s=td_ini_parse_token_char(s+1, end, '\"');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '(':
				s=td_ini_parse_token_char(s+1, end, ')');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '{':
				s=td_ini_parse_token_char(s+1, end, '}');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '[':
				s=td_ini_parse_token_char(s+1, end, ']');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;

			default:
				break;
			}
		}
	}

	return TD_NULL_POINTER;
 }





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
td_pointer_t td_ini_parameter(td_ini_t* ctx)
{
	return ctx->parameter;
}

td_string_t* td_ini_line (td_ini_t* ctx)
{
	return &ctx->token_line;
}

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
void td_ini_set_parameter        (td_ini_t* ctx, td_pointer_t parameter)  { ctx->parameter = parameter; };
void td_ini_set_handler_line     (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_line     = handler; };
void td_ini_set_handler_comment  (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_comment  = handler; };
void td_ini_set_handler_section  (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_section  = handler; };
void td_ini_set_handler_variable (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_variable = handler; };
void td_ini_set_handler_value    (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_value    = handler; };
void td_ini_set_handler_element  (td_ini_t* ctx, td_ini_handler_t handler){ ctx->handler_element  = handler; };

//===========================================================================
static void td_ini_handle_line (td_ini_t* ctx)
{
	td_string_trim(td_ini_line(ctx));

	if (ctx->handler_line)
	{
		ctx->handler_line(ctx);
	}
}

static void td_ini_handle_comment (td_ini_t* ctx)
{
	td_string_t* p;
	td_char_t    ch;


	p = td_ini_comment(ctx);
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

static void td_ini_handle_section (td_ini_t* ctx)
{
	td_string_trim(td_ini_section(ctx));


	if (ctx->handler_section)
	{
		ctx->handler_section(ctx);
	}
}

static void td_ini_handle_variable (td_ini_t* ctx)
{
	td_string_trim(td_ini_variable(ctx));


	if (ctx->handler_variable)
	{
		ctx->handler_variable(ctx);
	}
}

static void td_ini_handle_value (td_ini_t* ctx)
{
	td_string_trim(td_ini_value(ctx));


	if (ctx->handler_value)
	{
		ctx->handler_value(ctx);
	}
}

static void td_ini_handle_element  (td_ini_t* ctx)
{
	if (ctx->handler_element)
	{
		ctx->handler_element(ctx);
	}
}

//===========================================================================
static void td_ini_transition_state (td_ini_t* ctx, td_ini_state_t state)
{
	if (ctx->state != state)
	{
		if (TD_INI_STATE_ERROR!=state)
		{
			switch (ctx->state)
			{
			case TD_INI_STATE_TOKEN_LINE:
				td_ini_handle_line(ctx);
				break;

			case TD_INI_STATE_TOKEN_COMMENT:
				td_ini_handle_comment(ctx);
				break;

			case TD_INI_STATE_TOKEN_SECTION:
				td_ini_handle_section(ctx);
				break;

			case TD_INI_STATE_TOKEN_VARIABLE:
				td_ini_handle_variable(ctx);
				break;

			case TD_INI_STATE_TOKEN_VALUE:
				td_ini_handle_value(ctx);
				td_ini_handle_element(ctx);
				break;

			default:
				break;
			}
		}
	}

	ctx->state = state;
}

//===========================================================================
static void td_ini_state_token_value (td_ini_t* ctx)
{
	td_char_t* s;
//	td_char_t  ch;


/*
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
	ctx->token_value.end    = s;
	ctx->token_value.length = ctx->token_value.end-ctx->token_value.begin;

	td_ini_transition_state(ctx, TD_INI_STATE_SCAN);
	return;
}

static void td_ini_state_token_variable (td_ini_t* ctx)
{
	td_char_t* s;
	td_char_t  ch;

	
	for (s=ctx->token_variable.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\\':
			s = td_ini_parse_escape_sequence (s, ctx->token_line.end);
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '\'':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, '\'');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '\"':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, '\"');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '(':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, ')');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '{':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, '}');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;
		case '[':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, ']');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_variable.begin);
				return;
			}
			break;

		case '\r':
		case '\n':
			{
				td_ini_set_error(ctx, s);
				return;
			}
			break;

		case '=':
			{
				ctx->token_variable.end = s;
				ctx->token_variable.length = ctx->token_variable.end-ctx->token_variable.begin;

				td_string_set_null (&ctx->token_value);
				ctx->token_value.begin = s+1;

				td_ini_transition_state(ctx, TD_INI_STATE_TOKEN_VALUE);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	td_ini_set_error(ctx, s);
	return;
}

static void td_ini_state_token_section (td_ini_t* ctx)
{
	td_char_t* s;
	td_char_t  ch;

	
	for (s=ctx->token_section.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\\':
			s = td_ini_parse_escape_sequence (s, ctx->token_line.end);
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '\'':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, '\'');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '\"':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, '\"');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '(':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, ')');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;
		case '{':
			s = td_ini_parse_token_char (s+1, ctx->token_line.end, '}');
			if (TD_NULL_POINTER==s)
			{
				td_ini_set_error(ctx, ctx->token_section.begin);
				return;
			}
			break;

		case '\r':
		case '\n':
			{
				td_ini_set_error(ctx, s);
				return;
			}
			break;

		case '[':
			{
				td_ini_set_error(ctx, s);
				return;
			}
			break;

		case ']':
			{
				if ( (s+1)!=ctx->token_line.end )
				{
					td_ini_set_error(ctx, s);
					return;
				}

				ctx->token_section.end = s;
				ctx->token_section.length = ctx->token_section.end-ctx->token_section.begin;

				td_ini_transition_state(ctx, TD_INI_STATE_SCAN);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	td_ini_set_error(ctx, s);
	return;
}

static void td_ini_state_token_comment (td_ini_t* ctx)
{
	td_char_t* s;
	td_char_t  ch;

	
	for (s=ctx->token_comment.begin; s!=ctx->token_line.end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\r':
		case '\n':
			{
				td_ini_set_error(ctx, s);
				return;
			}
			break;

		default:
			break;
		}
	}


	//-----------------------------------------------------------------------
	ctx->token_comment.end    = s;
	ctx->token_comment.length = ctx->token_comment.end-ctx->token_comment.begin;

	td_ini_transition_state(ctx, TD_INI_STATE_SCAN);
	return;
}

static void td_ini_state_scan_line (td_ini_t* ctx)
{
	//-----------------------------------------------------------------------
	if ( 0u==ctx->token_line.length )
	{
		td_ini_transition_state(ctx, TD_INI_STATE_SCAN);
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
		case ';':
			{
				td_string_set_null (&ctx->token_comment);
				ctx->token_comment.begin = s+1;

				td_ini_transition_state(ctx, TD_INI_STATE_TOKEN_COMMENT);
				return;
			}
			break;

		case '[':
			{
				td_string_set_null (&ctx->token_section);
				ctx->token_section.begin = s+1;

				td_ini_transition_state(ctx, TD_INI_STATE_TOKEN_SECTION);
				return;
			}
			break;

		default:
			{
				td_string_set_null (&ctx->token_variable);
				ctx->token_variable.begin = s;

				td_ini_transition_state(ctx, TD_INI_STATE_TOKEN_VARIABLE);
				return;
			}
			break;
		}
	}
}

static void td_ini_state_token_line (td_ini_t* ctx)
{
	td_char_t* s;
	td_char_t  ch;

	td_uint_t flag_multiline;
	td_uint_t flag_multiline_cr;

	
	// "\n"
	// "\r\n"

	// "\\\n"
	// "\\\r\n"

	// "\\\\\n"   : X
	// "\\\\\r\n" : X


	flag_multiline    = TD_FALSE;
	flag_multiline_cr = TD_FALSE;

	for (s=ctx->token_line.begin; s!=ctx->stream.end; s++)
	{
		ch = *s;


		switch (ch)
		{
		case '\\':
			if ( TD_FALSE==flag_multiline )
			{
				flag_multiline = TD_TRUE;
			}
			else
			{
				flag_multiline = TD_FALSE;
			}
			break;

		case '\r':
			if (TD_TRUE==flag_multiline)
			{
				flag_multiline_cr = TD_TRUE;
			}
			break;

		case '\n':
			if ( (TD_TRUE==flag_multiline    ) &&
			     (TD_TRUE==flag_multiline_cr )  )
			{
				flag_multiline    = TD_FALSE;
				flag_multiline_cr = TD_FALSE;
			}
			else
			{
				ctx->parsing.end = s+1;
				ctx->parsing.length = ctx->parsing.end-ctx->parsing.begin;

				ctx->token_line.end = s+1;
				ctx->token_line.length = ctx->token_line.end-ctx->token_line.begin;

				td_ini_transition_state(ctx, TD_INI_STATE_SCAN_LINE);
				return;
			}
			break;

		default:
			flag_multiline    = TD_FALSE;
			flag_multiline_cr = TD_FALSE;
			break;
		}
	}


	//-----------------------------------------------------------------------
	td_ini_set_error(ctx, s);
	return;
}

static void td_ini_state_scan (td_ini_t* ctx)
{
	if ( ctx->parsing.end==ctx->stream.end )
	{
		td_ini_transition_state(ctx, TD_INI_STATE_DONE);
		return;
	}
	else
	{
		if ( ctx->parsing.end!=TD_NULL_POINTER )
		{
			ctx->parsing.begin  = ctx->parsing.end;
			ctx->parsing.end    = TD_NULL_POINTER;
			ctx->parsing.length = 0u;
		}


		td_string_set_null (&ctx->token_line);
		ctx->token_line.begin = ctx->parsing.begin;

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

		case TD_INI_STATE_SCAN_LINE:
			td_ini_state_scan_line(ctx);
			break;

		case TD_INI_STATE_TOKEN_COMMENT:
			td_ini_state_token_comment(ctx);
			break;

		case TD_INI_STATE_TOKEN_SECTION:
			td_ini_state_token_section(ctx);
			break;

		case TD_INI_STATE_TOKEN_VARIABLE:
			td_ini_state_token_variable(ctx);
			break;

		case TD_INI_STATE_TOKEN_VALUE:
			td_ini_state_token_value(ctx);
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
	td_char_t* s;


	//-----------------------------------------------------------------------
	s = (td_char_t*) pointer;

	//-----------------------------------------------------------------------
	ctx->stream.begin  = s;
	ctx->stream.end    = s + size;
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
void td_ini_set_error (td_ini_t* ctx, td_char_t* position)
{
	ctx->error_position = position;

	td_ini_transition_state(ctx, TD_INI_STATE_ERROR);
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




