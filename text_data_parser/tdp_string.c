/****************************************************************************
**
** File: tdp_string.c
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
/*
  0x20   0 0x30    @ 0x40   ' 0x60
! 0x21             A 0x41   a 0x61
" 0x22                           
# 0x23                           
$ 0x24                           
% 0x25                           
& 0x26                           
' 0x27                           
( 0x28                           
) 0x29   9 0x39                  
* 0x2A   : 0x3A   Z 0x5A   z 0x7A
+ 0x2B   ; 0x3B   [ 0x5B   { 0x7B
, 0x2C   < 0x3C   \ 0x5C   | 0x7C
- 0x2D   = 0x3D   ] 0x5D   } 0x7D
. 0x2E   > 0x3E   ^ 0x5E   ~ 0x7E
/ 0x2F   ? 0x3F   _ 0x5F     0x7F
*/
//===========================================================================
/*

{}
[]
()
""
''

!#$%&'*+,-./:;<=>?@\^_`|~

\\
\"
\'

\n
\r
\t

*/
//===========================================================================
/*
+-----------------+---------------------+-------------------------------------------------------------------------------------+
| Escape sequence |  Hex Value in ASCII | Character represented                                                               |
+-----------------+---------------------+-------------------------------------------------------------------------------------+
| \a              |  07                 | Alert (Beep, Bell) (added in C89)[1]                                                |
| \b              |  08                 | Backspace                                                                           |
| \f              |  0C                 | Formfeed Page Break                                                                 |
| \n              |  0A                 | Newline (Line Feed); see notes below                                                |
| \r              |  0D                 | Carriage Return                                                                     |
| \t              |  09                 | Horizontal Tab                                                                      |
| \v              |  0B                 | Vertical Tab                                                                        |
| \\              |  5C                 | Backslash                                                                           |
| \'              |  27                 | Apostrophe or single quotation mark                                                 |
| \"              |  22                 | Double quotation mark                                                               |
| \?              |  3F                 | Question mark (used to avoid trigraphs)                                             |
| \e              |  1B                 | Escape character                                                                    |
| \nnn            |  any                | The byte whose numerical value is given by nnn interpreted as an octal number       |
| \xhh            |  any                | The byte whose numerical value is given by hh interpreted as a hexadecimal number   |
| \uhhhh          |  none               | Unicode code point below 10000 hexadecimal                                          |
| \Uhhhhhhhh      |  none               | Unicode code point where h is a hexadecimal digit                                   |
+-----------------+---------------------+-------------------------------------------------------------------------------------+
*/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static tdp_bool_t tdp_c_char_compare (tdp_char_t ch1, tdp_char_t ch2, tdp_bool_t case_sensitive)
{
	if (TDP_FALSE==case_sensitive)
	{
		if ('a'<=ch1 && ch1<='z')
		{
			ch1 += 0x20;
		}
		if ('a'<=ch2 && ch2<='z')
		{
			ch2 += 0x20;
		}
	}

	return (ch1 == ch2) ? TDP_TRUE : TDP_FALSE;
}

// strlen()
static tdp_uint_t tdp_c_string_length (tdp_char_t* s)
{
	tdp_uint_t i;


	i = 0u;

	if (TDP_NULL_POINTER != s)
	{
		while (*s!=0x00)
		{
			i++;
			s++;
		}
	}

	return i;
}

// digit()
static tdp_bool_t tdp_c_char_is_digit (tdp_char_t ch)
{
	if ('0'<=ch && ch <= '9')
	{
		return TDP_TRUE;
	}

	return TDP_FALSE;
}


static void tdp_zero_memory (tdp_pointer_t dpointer, tdp_uint_t dsize)
{
	tdp_byte_t* address;
	tdp_uint_t  i;
	tdp_byte_t  zero;


	if (TDP_NULL_POINTER==dpointer)
	{
		return;
	}


	address = (tdp_byte_t*)dpointer;
	zero    = 0u;

	for (i=0u; i<dsize; i++)
	{
		*address++ = zero;
	}
}

static tdp_uint_t tdp_c_string_to_uinteger (tdp_char_t* p)
{
	tdp_uint_t length; 
	tdp_uint_t i;

	tdp_uint_t value; 

	tdp_uint_t  state; 
	tdp_char_t* s;
	tdp_char_t  ch;
	tdp_bool_t  loop;


	length = tdp_c_string_length(p);
	if (0u==length)
	{
		return 0u;
	}


	value = 0u;


	state = 0u;
	loop  = TDP_TRUE;
	for (i=0u, s=p; (i<length) && (TDP_TRUE==loop); i++)
	{
		ch = *s;

		switch (state)
		{
		case 0u: // integer
			if (tdp_c_char_is_digit(ch))
			{
				value = 10 * value + (ch - '0');

				s++;
			}
			else
			{
				state = 1u;
			}
			break;

		case 1u: // end
			loop = TDP_FALSE;
			break;

		default:
			break;
		}
	}


	return value; 
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
tdp_bool_t tdp_string_empty (tdp_string_t* p)
{
	if (p->begin  >= p->end          ) return TDP_TRUE;
	if (p->begin  == TDP_NULL_POINTER) return TDP_TRUE;
	if (p->end    == TDP_NULL_POINTER) return TDP_TRUE;
	if (p->length == 0u              ) return TDP_TRUE;

	return TDP_FALSE;
}

void tdp_string_clear (tdp_string_t* p)
{
	p->begin  = TDP_NULL_POINTER;
	p->end    = TDP_NULL_POINTER;
	p->length = 0u;
}

void tdp_string_copy (tdp_string_t* sp, tdp_string_t* dp)
{
	dp->begin  = sp->begin ;
	dp->end    = sp->end   ;
	dp->length = sp->length;
}

void tdp_string_begin (tdp_string_t* p, tdp_char_t* s)
{
	p->begin  = s;
	p->end    = TDP_NULL_POINTER;
	p->length = 0u;
}

void tdp_string_end (tdp_string_t* p, tdp_char_t* s)
{
	if (p->begin!=TDP_NULL_POINTER)
	{
		if (p->begin <= s)
		{
			p->end    = s;
			p->length = (p->end - p->begin);
		}
	}
}

//===========================================================================
void tdp_string_trim (tdp_string_t* p)
{
	tdp_char_t* s;
	tdp_char_t  ch;

	tdp_uint_t  parse;



	if ( tdp_string_empty(p) )
	{
		return;
	}


	while(0u<p->length)
	{
		ch = *(p->begin);

		if     ('\n' == ch)
		{

			p->begin++;
			p->length--;
		}
		else if ('\r' == ch)
		{

			p->begin++;
			p->length--;
		}
		else if ('\t' == ch)
		{

			p->begin++;
			p->length--;
		}
		else if (' ' == ch)
		{

			p->begin++;
			p->length--;
		}
		else if (' ' == ch)
		{

			p->begin++;
			p->length--;
		}
		else if ('\\' == ch)
		{
			s = tdp_parse_escape_multiline(p->begin, p->end);
			if (TDP_NULL_POINTER!=s)
			{
				s++;
				
				if (s!=p->end)
				{
					parse = s - p->begin;
					p->begin  += parse;
					p->length -= parse;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	
#if 0
	while(0u<p->length)
	{
		s = p->end-1;

		ch = *s;

		if      ('\n' == ch)
		{
			p->end--;
			p->length--;
		}
		else if ('\r' == ch)
		{
			p->end--;
			p->length--;
		}
		else if ('\t' == ch)
		{
			p->end--;
			p->length--;
		}
		else if ('\\' == ch)
		{
			p->end--;
			p->length--;
		}
		else if (' ' == ch)
		{
			p->end--;
			p->length--;
		}
		else
		{
			break;
		}
	}
#endif


#if 1
	parse = 0u;

	while(0u<p->length)
	{
		ch = *(p->end-1);
		if      ('\n' == ch)
		{
			p->end--;
			p->length--;

			parse = 1u;
		}
		else if ('\r' == ch)
		{
			p->end--;
			p->length--;

			if ( parse==1u )
			{
				parse = 2u;
			}
			else
			{
				parse = 0u;
			}
		}
		else if ('\t' == ch)
		{
			p->end--;
			p->length--;

			parse = 0u;
		}
		else if (' ' == ch)
		{
			p->end--;
			p->length--;

			parse = 0u;
		}
		else if ('\\' == ch)
		{
			if (parse==1u || parse==2u)
			{
				p->end--;
				p->length--;

				parse = 0;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
#endif
}

//===========================================================================
void tdp_string_trim_dquotes (tdp_string_t* p)
{
	tdp_char_t ch1;
	tdp_char_t ch2;

	tdp_char_t* s;


	if ( tdp_string_empty(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = tdp_parse_token_char (p->begin+1, p->end, '\"');
		if (TDP_NULL_POINTER!=s)
		{
			if ((s+1)==p->end)
			{
				if ( ('\"' == ch1) && ('\"' == ch2) )
				{
					p->begin++;
					p->end--;
					p->length-=2u;
				}
			}
		}
	}
}

void tdp_string_trim_squotes (tdp_string_t* p)
{
	tdp_char_t ch1;
	tdp_char_t ch2;

	tdp_char_t* s;


	if ( tdp_string_empty(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = tdp_parse_token_char (p->begin+1, p->end, '\'');
		if (TDP_NULL_POINTER!=s)
		{
			if ((s+1)==p->end)
			{
				if ( ('\'' == ch1) && ('\'' == ch2) )
				{
					p->begin++;
					p->end--;
					p->length-=2u;
				}
			}
		}
	}
}

void tdp_string_trim_curly_brackets (tdp_string_t* p)
{
	tdp_char_t ch1;
	tdp_char_t ch2;

	tdp_char_t* s;


	if ( tdp_string_empty(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = tdp_parse_token_char (p->begin+1, p->end, '}');
		if (TDP_NULL_POINTER!=s)
		{
			if ((s+1)==p->end)
			{
				if ( ('{' == ch1) && ('}' == ch2) )
				{
					p->begin++;
					p->end--;
					p->length-=2u;
				}
			}
		}
	}
}

void tdp_string_trim_round_brackets (tdp_string_t* p)
{
	tdp_char_t ch1;
	tdp_char_t ch2;

	tdp_char_t* s;

	
	if ( tdp_string_empty(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = tdp_parse_token_char (p->begin+1, p->end, ')');
		if (TDP_NULL_POINTER!=s)
		{
			if ((s+1)==p->end)
			{
				if ( ('(' == ch1) && (')' == ch2) )
				{
					p->begin++;
					p->end--;
					p->length-=2u;
				}
			}
		}
	}
}

void tdp_string_trim_square_brackets (tdp_string_t* p)
{
	tdp_char_t ch1;
	tdp_char_t ch2;

	tdp_char_t* s;


	if ( tdp_string_empty(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = tdp_parse_token_char (p->begin+1, p->end, ']');
		if (TDP_NULL_POINTER!=s)
		{
			if ((s+1)==p->end)
			{
				if ( ('[' == ch1) && (']' == ch2) )
				{
					p->begin++;
					p->end--;
					p->length-=2u;
				}
			}
		}
	}
}

void tdp_string_trim_angle_brackets (tdp_string_t* p)
{
	tdp_char_t ch1;
	tdp_char_t ch2;

	tdp_char_t* s;


	if ( tdp_string_empty(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = tdp_parse_token_char (p->begin+1, p->end, '>');
		if (TDP_NULL_POINTER!=s)
		{
			if ((s+1)==p->end)
			{
				if ( ('<' == ch1) && ('>' == ch2) )
				{
					p->begin++;
					p->end--;
					p->length-=2u;
				}
			}
		}
	}
}

//===========================================================================
tdp_bool_t tdp_string_prefix_compare (tdp_string_t* p, tdp_char_t* prefix, tdp_bool_t case_sensitive)
{
	tdp_char_t* p1;
	tdp_char_t* p2;
	tdp_uint_t  i;
	tdp_uint_t  count;


	count = tdp_c_string_length(prefix);
	if (p->length < count)
	{
		return TDP_FALSE;
	}


	p1 = p->begin;
	p2 = prefix;
	for (i=0u; i<count; i++)
	{
		if (TDP_FALSE==tdp_c_char_compare(*p1++, *p2++, case_sensitive))
		{
			return TDP_FALSE;
		}
	}

	return TDP_TRUE;
}

tdp_bool_t tdp_string_suffix_compare (tdp_string_t* p, tdp_char_t* suffix, tdp_bool_t case_sensitive)
{
	tdp_char_t* p1;
	tdp_char_t* p2;
	tdp_uint_t  i;
	tdp_uint_t  count;


	count = tdp_c_string_length(suffix);
	if (p->length < count)
	{
		return TDP_FALSE;
	}


	p1 = p->begin + (p->length-count);
	p2 = suffix;
	for (i=0u; i<count; i++)
	{
		if (TDP_FALSE==tdp_c_char_compare(*p1++, *p2++, case_sensitive))
		{
			return TDP_FALSE;
		}
	}

	return TDP_TRUE;
}

tdp_bool_t tdp_string_prefix_suffix_compare (tdp_string_t* p, tdp_char_t* prefix, tdp_char_t* suffix, tdp_bool_t case_sensitive)
{
	if (TDP_FALSE == tdp_string_prefix_compare(p, prefix, case_sensitive))
	{
		return TDP_FALSE;
	}

	if (TDP_FALSE == tdp_string_suffix_compare(p, suffix, case_sensitive))
	{
		return TDP_FALSE;
	}

	return TDP_TRUE;
}

void tdp_string_part_without_prefix_suffix (tdp_string_t* p, tdp_char_t* prefix, tdp_char_t* suffix, tdp_bool_t case_sensitive, tdp_string_t* dp)
{
	tdp_string_clear(dp);

	if (TDP_FALSE==tdp_string_prefix_suffix_compare(p, prefix, suffix, case_sensitive))
	{
		return;
	}


	tdp_uint_t prefix_length;
	tdp_uint_t suffix_length;


	prefix_length = tdp_c_string_length(prefix);
	suffix_length = tdp_c_string_length(suffix);

	tdp_string_copy (p, dp);

	dp->begin  += prefix_length;
	dp->end    -= suffix_length;
	dp->length -= (prefix_length+suffix_length);
}

tdp_bool_t tdp_string_compare (tdp_string_t* p, tdp_char_t* s, tdp_bool_t case_sensitive)
{
	tdp_char_t* p1;
	tdp_char_t* p2;
	tdp_uint_t  i;


	if (p->length != tdp_c_string_length(s))
	{
		return TDP_FALSE;
	}
	if (0u == p->length)
	{
		return TDP_TRUE;
	}
	if ( tdp_string_empty(p) )
	{
		return TDP_FALSE;
	}


	p1 = p->begin;
	p2 = s;

	for (i=0u; i<p->length; i++)
	{
		if (TDP_FALSE==tdp_c_char_compare(*p1++, *p2++, case_sensitive))
		{
			return TDP_FALSE;
		}
	}

	return TDP_TRUE;
}

tdp_uint_t tdp_string_copy_to_c_string (tdp_string_t* p, tdp_char_t* dpointer, tdp_uint_t dlength)
{
	if ( tdp_string_empty(p) )
	{
		return 0u;
	}


	if (TDP_NULL_POINTER==dpointer)
	{
		return 0u;
	}

	if (0u==dlength)
	{
		return 0u;
	}


	tdp_uint_t i;


	for (i=0u; (i<dlength-1u) && (i<p->length); i++)
	{
		*dpointer = *(p->begin+i);

		dpointer++;
	}

	*dpointer = 0x00;


	// NULL 문자 포함 복사 길이 반환

	return i+1u;
}

//===========================================================================
tdp_uint_t tdp_string_copy_to_c_string_without_escape_multiline (tdp_string_t* p, tdp_char_t* dpointer, tdp_uint_t dlength)
{
	if ( tdp_string_empty(p) )
	{
		return 0u;
	}


	if (TDP_NULL_POINTER==dpointer)
	{
		return 0u;
	}

	if (0u==dlength)
	{
		return 0u;
	}


	tdp_char_t* s;
	tdp_char_t  ch;

	tdp_char_t* s1;
	tdp_char_t* s2;
	tdp_char_t  ch1;
	tdp_char_t  ch2;

	tdp_uint_t i;


	i=0u;
	for (s=p->begin; s!=p->end; s++)
	{
		ch = *s;


		switch (ch)
		{
		case '\\':
			s1 = s+1;
			if (s1!=p->end)
			{
				ch1=*s1;

				s2 = s1+1;
				if (s2!=p->end)
				{
					ch2=*s2;

					if ( (ch1=='\r') && (ch2=='\n')  )
					{
						if (i<dlength-1u) { *dpointer++ = ch1; i++; }
						if (i<dlength-1u) { *dpointer++ = ch2; i++; }
						s += 2;
					}
					else if (ch1=='\n')
					{
						if (i<dlength-1u) { *dpointer++ = ch1; i++; }
						s += 1;
					}
					else
					{
						if (i<dlength-1u) { *dpointer++ = ch; i++; }
					}
				}
				else
				{
					if ( ch1=='\n' )
					{
						if (i<dlength-1u) { *dpointer++ = ch1; i++; }
						s += 1;
					}
					else
					{
						if (i<dlength-1u) { *dpointer++ = ch; i++; }
					}
				}
			}
			else
			{
				if (i<dlength-1u) { *dpointer++ = ch; i++; }
			}
			break;

		default:
			if (i<dlength-1u) { *dpointer++ = ch; i++; }
			break;
		}
	}

	*dpointer = 0x00;


	// NULL 문자 포함 복사 길이 반환

	return i+1u;

}

//===========================================================================
// atof
tdp_double_t tdp_string_parse_real_number (tdp_string_t* p)
{
	tdp_int_t    sign; 
	tdp_double_t value; 
	tdp_double_t power;

	tdp_uint_t  state; 
	tdp_char_t* s;
	tdp_char_t  ch;
	tdp_bool_t  loop;



	if ( tdp_string_empty(p) )
	{
		return 0.0;
	}


	sign  = 0;
	value = 0.0;
	power = 1.0;


	state = 0u;
	loop  = TDP_TRUE;
	for (s=p->begin; (s!=p->end) && (TDP_TRUE==loop); )
	{
		ch = *s;

		switch (state)
		{
		case 0u: // sign
			sign = (ch == '-') ? -1 : 1;
			if ( ('+' == ch) || ('-' == ch) )
			{
				s++;
			}
			state = 1u;
			break;

		case 1u: // integer
			if (tdp_c_char_is_digit(ch))
			{
				value = 10.0 * value + (ch - '0');

				s++;
			}
			else
			{
				state = 2u;
			}
			break;

		case 2u: // dot
			if ('.'==ch)
			{
				s++;

				state = 3u;
			}
			else
			{
				state = 4u;
			}
			break;

		case 3u: // fractional
			if (tdp_c_char_is_digit(ch))
			{
				value = 10.0 * value + (ch - '0');
				power = power*10.0;
				s++;
			}
			else
			{
				state = 4u;
			}
			break;

		case 4u: // end
			loop = TDP_FALSE;
			break;

		default:
			break;
		}
	}


	return sign * value / power; 
}

// atoi
tdp_int_t tdp_string_parse_integer (tdp_string_t* p)
{
	tdp_int_t sign; 
	tdp_int_t value; 

	tdp_uint_t  state; 
	tdp_char_t* s;
	tdp_char_t  ch;
	tdp_bool_t  loop;



	if ( tdp_string_empty(p) )
	{
		return 0;
	}


	sign  = 0;
	value = 0;


	state = 0u;
	loop  = TDP_TRUE;
	for (s=p->begin; (s!=p->end) && (TDP_TRUE==loop); )
	{
		ch = *s;

		switch (state)
		{
		case 0u: // sign
			sign = (ch == '-') ? -1 : 1;
			if ( ('+' == ch) || ('-' == ch) )
			{
				s++;
			}
			state = 1u;
			break;

		case 1u: // integer
			if (tdp_c_char_is_digit(ch))
			{
				value = 10 * value + (ch - '0');

				s++;
			}
			else
			{
				state = 2u;
			}
			break;

		case 2u: // end
			loop = TDP_FALSE;
			break;

		default:
			break;
		}
	}


	return sign * value; 
}

tdp_uint_t tdp_string_parse_uinteger (tdp_string_t* p)
{
	tdp_uint_t value; 

	tdp_uint_t  state; 
	tdp_char_t* s;
	tdp_char_t  ch;
	tdp_bool_t  loop;



	if ( tdp_string_empty(p) )
	{
		return 0u;
	}


	value = 0u;


	state = 0u;
	loop  = TDP_TRUE;
	for (s=p->begin; (s!=p->end) && (TDP_TRUE==loop); )
	{
		ch = *s;

		switch (state)
		{
		case 0u: // integer
			if (tdp_c_char_is_digit(ch))
			{
				value = 10 * value + (ch - '0');

				s++;
			}
			else
			{
				state = 1u;
			}
			break;

		case 1u: // end
			loop = TDP_FALSE;
			break;

		default:
			break;
		}
	}


	return value; 
}

tdp_uint_t tdp_string_parse_ip_v4 (tdp_string_t* p)
{
	// ip address: "000.000.000.000" 16 bytes
	tdp_char_t* string_pointer;
	tdp_uint_t  string_length;

	tdp_uint_t i;
	tdp_uint_t count;
	tdp_char_t ch;

	tdp_uint_t offset;
	tdp_bool_t validate;

	tdp_uint_t address;
	tdp_uint_t address_class_index;
	tdp_byte_t address_class[4];
	tdp_char_t address_class_string[4][32];


	offset   = 0u;
	validate = TDP_TRUE;

	address = 0u;
	address_class_index = 0u;
	tdp_zero_memory (address_class,        sizeof(address_class));
	tdp_zero_memory (address_class_string, sizeof(address_class_string));
	

	if ( tdp_string_empty(p) )
	{
		return 0u;
	}


	string_pointer = p->begin;


	if (TDP_NULL_POINTER!=string_pointer)
	{
		string_length = p->length;

		if ( (7u <= string_length) && (string_length  <= 16u) )
		{
			count = string_length;
			for (i=0u; i<count;i++)
			{
				if (TDP_FALSE==validate)
				{
					break;
				}

				ch = *(string_pointer+i);

				switch (ch)
				{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					address_class_string[address_class_index][offset] = ch;
					offset++;
					if (3u<offset)
					{
						validate = TDP_FALSE; 
					}
					break;

				case '.':
					offset = 0u;
					address_class_index++;
					if ( 4u <= address_class_index )
					{
						validate = TDP_FALSE; 
					}
					break;

				default:
					validate = TDP_FALSE; 
					break;
				}
			}

			if ( TDP_TRUE==validate )
			{
				count = 4u;
				for (i=0u; i<count;i++)
				{
					address_class[i] = tdp_c_string_to_uinteger( address_class_string[i] );
				}
			}
		}
	}


	address = 
		( (address_class[0]&0xFFu) << 24u ) +
		( (address_class[1]&0xFFu) << 16u ) +
		( (address_class[2]&0xFFu) <<  8u ) +
		( (address_class[3]&0xFFu)        ) ;
	

	return address;
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
tdp_char_t* tdp_parse_escape_multiline (tdp_char_t* begin, tdp_char_t* end)
 {
	tdp_char_t* s;
	tdp_char_t  ch;


	if (TDP_NULL_POINTER==begin)
	{
		return TDP_NULL_POINTER;
	}
	if (TDP_NULL_POINTER==end)
	{
		return TDP_NULL_POINTER;
	}
	if (begin>=end)
	{
		return TDP_NULL_POINTER;
	}

	
	s = begin;


	ch = *s;
	if ('\\'!=ch)
	{
		return TDP_NULL_POINTER;
	}
	s++;
	if (s==end)
	{
		return TDP_NULL_POINTER;
	}


	ch = *s;
	if ('\n'==ch)
	{
		return s;
	}


	if ('\r'==ch)
	{
		s++;
		if (s!=end)
		{
			ch = *s;

			if ('\n'==ch)
			{
				return s;
			}
		}
	}


	return TDP_NULL_POINTER;
}

tdp_char_t* tdp_parse_escape_sequence (tdp_char_t* begin, tdp_char_t* end)
{
	tdp_char_t* s;
	tdp_char_t  ch;


	if (TDP_NULL_POINTER==begin)
	{
		return TDP_NULL_POINTER;
	}
	if (TDP_NULL_POINTER==end)
	{
		return TDP_NULL_POINTER;
	}
	if (begin>=end)
	{
		return TDP_NULL_POINTER;
	}


	s = begin;


	ch = *s;
	if (ch!='\\')
	{
		return TDP_NULL_POINTER;
	}

/*
	if (s+1==end)
	{
		return s;
	}
*/

	s++;	
	for (; s!=end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\n':
			{
				return tdp_parse_escape_multiline(begin, end);
			}
			break;
		case '\r':
			{
				return tdp_parse_escape_multiline(begin, end);
			}
			break;

		case '\\':
			{
				return s;
			}
			break;

		case 'n':
		case 'r':
		case 't':
			{
				return s;
			}
			break;

		case '\"':
		case '\'':
			{
				return s;
			}
			break;

		default:
			{
				return TDP_NULL_POINTER;
			}
			break;
		}
	}


	return TDP_NULL_POINTER;
}

tdp_char_t* tdp_parse_token_char (tdp_char_t* begin, tdp_char_t* end, tdp_char_t token)
{
	tdp_char_t* s;
	tdp_char_t  ch;


	if (TDP_NULL_POINTER==begin)
	{
		return TDP_NULL_POINTER;
	}
	if (TDP_NULL_POINTER==end)
	{
		return TDP_NULL_POINTER;
	}
	if (begin>=end)
	{
		return TDP_NULL_POINTER;
	}


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
				s = tdp_parse_escape_sequence(s, end);
				if (TDP_NULL_POINTER==s)
				{
					return TDP_NULL_POINTER;
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
				s = tdp_parse_escape_sequence(s, end);
				if (TDP_NULL_POINTER==s)
				{
					return TDP_NULL_POINTER;
				}
				break;
			case '\"':
				s=tdp_parse_token_char(s+1, end, '\"');
				if (TDP_NULL_POINTER==s)
				{
					return TDP_NULL_POINTER;
				}
				break;
			case '\'':
				s=tdp_parse_token_char(s+1, end, '\'');
				if (TDP_NULL_POINTER==s)
				{
					return TDP_NULL_POINTER;
				}
				break;

			case '{':
				s=tdp_parse_token_char(s+1, end, '}');
				if (TDP_NULL_POINTER==s)
				{
					return TDP_NULL_POINTER;
				}
				break;
			case '(':
				s=tdp_parse_token_char(s+1, end, ')');
				if (TDP_NULL_POINTER==s)
				{
					return TDP_NULL_POINTER;
				}
				break;
			case '[':
				s=tdp_parse_token_char(s+1, end, ']');
				if (TDP_NULL_POINTER==s)
				{
					return TDP_NULL_POINTER;
				}
				break;

			default:
				break;
			}
		}
	}


	return TDP_NULL_POINTER;
}




