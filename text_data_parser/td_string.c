/****************************************************************************
**
** File: td_string.c
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
\r
\n
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
static td_bool_t td_c_char_compare (td_char_t ch1, td_char_t ch2, td_bool_t case_sensitive)
{
	if (TD_FALSE==case_sensitive)
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

	return (ch1 == ch2) ? TD_TRUE : TD_FALSE;
}

// strlen()
static td_uint_t td_c_string_length (td_char_t* s)
{
	td_uint_t i;


	i = 0u;

	if (TD_NULL_POINTER != s)
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
static td_bool_t td_c_char_is_digit (td_char_t ch)
{
	if ('0'<=ch && ch <= '9')
	{
		return TD_TRUE;
	}

	return TD_FALSE;
}


static void td_zero_memory (td_pointer_t dpointer, td_uint_t dsize)
{
	td_byte_t* address;
	td_uint_t  i;
	td_byte_t  zero;


	if (TD_NULL_POINTER==dpointer)
	{
		return;
	}


	address = (td_byte_t*)dpointer;
	zero    = 0u;

	for (i=0u; i<dsize; i++)
	{
		*address++ = zero;
	}
}

static td_uint_t td_c_string_to_uinteger (td_char_t* p)
{
	td_uint_t length; 
	td_uint_t i;

	td_uint_t value; 

	td_uint_t  state; 
	td_char_t* s;
	td_char_t  ch;
	td_bool_t  loop;


	length = td_c_string_length(p);
	if (0u==length)
	{
		return 0u;
	}


	value = 0u;


	state = 0u;
	loop  = TD_TRUE;
	for (i=0u, s=p; (i<length) && (TD_TRUE==loop); i++)
	{
		ch = *s;

		switch (state)
		{
		case 0u: // integer
			if (td_c_char_is_digit(ch))
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
			loop = TD_FALSE;
			break;

		default:
			break;
		}
	}


	return value; 
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
td_bool_t td_string_is_null (td_string_t* p)
{
	if (p->begin  >= p->end         ) return TD_TRUE;
	if (p->begin  == TD_NULL_POINTER) return TD_TRUE;
	if (p->end    == TD_NULL_POINTER) return TD_TRUE;
	if (p->length == 0u             ) return TD_TRUE;

	return TD_FALSE;
}

void td_string_null (td_string_t* p)
{
	p->begin  = TD_NULL_POINTER;
	p->end    = TD_NULL_POINTER;
	p->length = 0u;
}

void td_string_begin (td_string_t* p, td_char_t* s)
{
	p->begin  = s;
	p->end    = TD_NULL_POINTER;
	p->length = 0u;
}

void td_string_end (td_string_t* p, td_char_t* s)
{
	if (p->begin!=TD_NULL_POINTER)
	{
		if (p->begin < s)
		{
			p->end    = s;
			p->length = p->end - p->begin;
		}
	}
}

//===========================================================================
void td_string_trim (td_string_t* p)
{
	td_char_t* s;
	td_char_t  ch;
	td_uint_t  parse;


	if ( td_string_is_null(p) )
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
			s = td_parse_escape_multiline(p->begin, p->end);
			if (TD_NULL_POINTER!=s)
			{
				s++;

				parse = (td_uint_t)(s - p->begin);
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
}

//===========================================================================
void td_string_trim_dquotes (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;

	td_char_t* s;


	if ( td_string_is_null(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = td_parse_token_char (p->begin+1, p->end, '\"');
		if (TD_NULL_POINTER!=s)
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

void td_string_trim_squotes (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;

	td_char_t* s;


	if ( td_string_is_null(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = td_parse_token_char (p->begin+1, p->end, '\'');
		if (TD_NULL_POINTER!=s)
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

void td_string_trim_round_brackets (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;

	td_char_t* s;



	if ( td_string_is_null(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = td_parse_token_char (p->begin+1, p->end, ')');
		if (TD_NULL_POINTER!=s)
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

void td_string_trim_square_brackets (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;

	td_char_t* s;


	if ( td_string_is_null(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = td_parse_token_char (p->begin+1, p->end, ']');
		if (TD_NULL_POINTER!=s)
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

void td_string_trim_curly_brackets (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;

	td_char_t* s;


	if ( td_string_is_null(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = td_parse_token_char (p->begin+1, p->end, '}');
		if (TD_NULL_POINTER!=s)
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

void td_string_trim_angle_brackets (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;

	td_char_t* s;


	if ( td_string_is_null(p) )
	{
		return;
	}


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);


		s = td_parse_token_char (p->begin+1, p->end, '>');
		if (TD_NULL_POINTER!=s)
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
td_bool_t td_string_compare (td_string_t* p, td_char_t* s, td_bool_t case_sensitive)
{
	td_char_t* p1;
	td_char_t* p2;
	td_uint_t  i;


	if (p->length != td_c_string_length(s))
	{
		return TD_FALSE;
	}
	if (0u == p->length)
	{
		return TD_TRUE;
	}
	if ( td_string_is_null(p) )
	{
		return TD_FALSE;
	}


	p1 = p->begin;
	p2 = s;

	for (i=0u; i<p->length; i++)
	{
		if (TD_FALSE==td_c_char_compare(*p1++, *p2++, case_sensitive))
		{
			return TD_FALSE;
		}
	}

	return TD_TRUE;
}

td_uint_t td_string_copy_to_c_string (td_string_t* p, td_char_t* dpointer, td_uint_t dsize)
{
	if ( td_string_is_null(p) )
	{
		return 0u;
	}


	if (TD_NULL_POINTER==dpointer)
	{
		return 0u;
	}

	if (0u==dsize)
	{
		return 0u;
	}


	td_uint_t i;


	for (i=0u; (i<dsize-1u) && (i<p->length); i++)
	{
		*dpointer = *(p->begin+i);

		dpointer++;
	}

	*dpointer = 0x00;


	// NULL 문자 포함 복사 크기 반환

	return i+1u;
}

//===========================================================================
td_uint_t td_string_copy_to_c_string_without_escape_multiline (td_string_t* p, td_char_t* dpointer, td_uint_t dsize)
{
	if ( td_string_is_null(p) )
	{
		return 0u;
	}


	if (TD_NULL_POINTER==dpointer)
	{
		return 0u;
	}

	if (0u==dsize)
	{
		return 0u;
	}


	td_char_t* s;
	td_char_t  ch;

	td_char_t* s1;
	td_char_t* s2;
	td_char_t  ch1;
	td_char_t  ch2;

	td_uint_t i;

	 
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
						if (i<dsize-1u) { *dpointer++ = ch1; i++; }
						if (i<dsize-1u) { *dpointer++ = ch2; i++; }
						s += 2;
					}
					else if (ch1=='\n')
					{
						if (i<dsize-1u) { *dpointer++ = ch1; i++; }
						s += 1;
					}
					else
					{
						if (i<dsize-1u) { *dpointer++ = ch; i++; }
					}
				}
				else
				{
					if ( ch1=='\n' )
					{
						if (i<dsize-1u) { *dpointer++ = ch1; i++; }
						s += 1;
					}
					else
					{
						if (i<dsize-1u) { *dpointer++ = ch; i++; }
					}
				}
			}
			else
			{
				if (i<dsize-1u) { *dpointer++ = ch; i++; }
			}
			break;

		default:
			if (i<dsize-1u) { *dpointer++ = ch; i++; }
			break;
		}
	}

	*dpointer = 0x00;


	// NULL 문자 포함 복사 크기 반환

	return i+1u;

}

//===========================================================================
// atof
td_double_t td_string_parse_real_number (td_string_t* p)
{
	td_int_t    sign; 
	td_double_t value; 
	td_double_t power;

	td_uint_t  state; 
	td_char_t* s;
	td_char_t  ch;
	td_bool_t  loop;



	if ( td_string_is_null(p) )
	{
		return 0.0;
	}


	sign  = 0;
	value = 0.0;
	power = 1.0;


	state = 0u;
	loop  = TD_TRUE;
	for (s=p->begin; (s!=p->end) && (TD_TRUE==loop); )
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
			if (td_c_char_is_digit(ch))
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
			if (td_c_char_is_digit(ch))
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
			loop = TD_FALSE;
			break;

		default:
			break;
		}
	}


	return sign * value / power; 
}

// atoi
td_int_t td_string_parse_integer (td_string_t* p)
{
	td_int_t sign; 
	td_int_t value; 

	td_uint_t  state; 
	td_char_t* s;
	td_char_t  ch;
	td_bool_t  loop;



	if ( td_string_is_null(p) )
	{
		return 0;
	}


	sign  = 0;
	value = 0;


	state = 0u;
	loop  = TD_TRUE;
	for (s=p->begin; (s!=p->end) && (TD_TRUE==loop); )
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
			if (td_c_char_is_digit(ch))
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
			loop = TD_FALSE;
			break;

		default:
			break;
		}
	}


	return sign * value; 
}

td_uint_t td_string_parse_uinteger (td_string_t* p)
{
	td_uint_t value; 

	td_uint_t  state; 
	td_char_t* s;
	td_char_t  ch;
	td_bool_t  loop;



	if ( td_string_is_null(p) )
	{
		return 0u;
	}


	value = 0u;


	state = 0u;
	loop  = TD_TRUE;
	for (s=p->begin; (s!=p->end) && (TD_TRUE==loop); )
	{
		ch = *s;

		switch (state)
		{
		case 0u: // integer
			if (td_c_char_is_digit(ch))
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
			loop = TD_FALSE;
			break;

		default:
			break;
		}
	}


	return value; 
}

td_uint_t td_string_parse_ip_v4 (td_string_t* p)
{
	// ip address: "000.000.000.000" 16 bytes
	td_char_t* string_pointer;
	td_uint_t  string_length;

	td_uint_t i;
	td_uint_t count;
	td_char_t ch;
	td_uint_t offset;
	td_bool_t validate;

	td_uint_t address_class_index;
	td_byte_t address_class[4];
	td_char_t address_class_string[4][32];
	td_uint_t address;


	offset   = 0u;
	validate = TD_TRUE;

	address = 0u;
	address_class_index = 0u;
	td_zero_memory (address_class,        sizeof(address_class));
	td_zero_memory (address_class_string, sizeof(address_class_string));
	

	if ( td_string_is_null(p) )
	{
		return 0u;
	}


	string_pointer = p->begin;


	if (TD_NULL_POINTER!=string_pointer)
	{
		string_length = p->length;

		if ( (7u <= string_length) && (string_length  <= 16u) )
		{
			count = string_length;
			for (i=0u; i<count;i++)
			{
				if (TD_FALSE==validate)
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
						validate = TD_FALSE; 
					}
					break;

				case '.':
					offset = 0u;
					address_class_index++;
					if ( 4u <= address_class_index )
					{
						validate = TD_FALSE; 
					}
					break;

				default:
					validate = TD_FALSE; 
					break;
				}
			}

			if ( TD_TRUE==validate )
			{
				count = 4u;
				for (i=0u; i<count;i++)
				{
					address_class[i] = td_c_string_to_uinteger( address_class_string[i] );
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
td_char_t* td_parse_escape_multiline (td_char_t* begin, td_char_t* end)
 {
	td_char_t* s;
	td_char_t  ch;


	if (TD_NULL_POINTER==begin)
	{
		return TD_NULL_POINTER;
	}
	if (TD_NULL_POINTER==end)
	{
		return TD_NULL_POINTER;
	}
	if (begin>=end)
	{
		return TD_NULL_POINTER;
	}

	
	s = begin;


	ch = *s;
	if ('\\'!=ch)
	{
		return TD_NULL_POINTER;
	}
	s++;
	if (s==end)
	{
		return TD_NULL_POINTER;
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


	return TD_NULL_POINTER;
}

td_char_t* td_parse_escape_sequence (td_char_t* begin, td_char_t* end)
{
	td_char_t* s;
	td_char_t  ch;


	if (TD_NULL_POINTER==begin)
	{
		return TD_NULL_POINTER;
	}
	if (TD_NULL_POINTER==end)
	{
		return TD_NULL_POINTER;
	}
	if (begin>=end)
	{
		return TD_NULL_POINTER;
	}


	s = begin;


	ch = *s;
	if (ch!='\\')
	{
		return TD_NULL_POINTER;
	}
	s++;


	for (; s!=end; s++)
	{
		ch = *s;

		switch (ch)
		{
		case '\r':
			{
				return td_parse_escape_multiline(begin, end);
			}
			break;

		case '\n':
			{
				return td_parse_escape_multiline(begin, end);
			}
			break;

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
			{
				return TD_NULL_POINTER;
			}
			break;
		}
	}

	return TD_NULL_POINTER;
}

td_char_t* td_parse_token_char (td_char_t* begin, td_char_t* end, td_char_t token)
{
	td_char_t* s;
	td_char_t  ch;


	if (TD_NULL_POINTER==begin)
	{
		return TD_NULL_POINTER;
	}
	if (TD_NULL_POINTER==end)
	{
		return TD_NULL_POINTER;
	}
	if (begin>=end)
	{
		return TD_NULL_POINTER;
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
				s = td_parse_escape_sequence(s, end);
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
				s = td_parse_escape_sequence(s, end);
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;

			case '\'':
				s=td_parse_token_char(s+1, end, '\'');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '\"':
				s=td_parse_token_char(s+1, end, '\"');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '(':
				s=td_parse_token_char(s+1, end, ')');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '{':
				s=td_parse_token_char(s+1, end, '}');
				if (TD_NULL_POINTER==s)
				{
					return TD_NULL_POINTER;
				}
				break;
			case '[':
				s=td_parse_token_char(s+1, end, ']');
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




