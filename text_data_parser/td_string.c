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
\b
\t

*/
//===========================================================================
/*
+-----------------+---------------------+----------------------------------------------------------------------------------------
| Escape sequence |  Hex Value in ASCII | Character represented
+-----------------+---------------------+----------------------------------------------------------------------------------------
| \a              |  07                 | Alert (Beep, Bell) (added in C89)[1]
| \b              |  08                 | Backspace
| \f              |  0C                 | Formfeed Page Break
| \n              |  0A                 | Newline (Line Feed); see notes below
| \r              |  0D                 | Carriage Return
| \t              |  09                 | Horizontal Tab
| \v              |  0B                 | Vertical Tab
| \\              |  5C                 | Backslash
| \'              |  27                 | Apostrophe or single quotation mark
| \"              |  22                 | Double quotation mark
| \?              |  3F                 | Question mark (used to avoid trigraphs)
| \e              |  1B                 | Escape character
| \nnn            |  any                | The byte whose numerical value is given by nnn interpreted as an octal number
| \xhh…          |  any                | The byte whose numerical value is given by hh… interpreted as a hexadecimal number
| \uhhhh          |  none               | Unicode code point below 10000 hexadecimal
| \Uhhhhhhhh      |  none               | Unicode code point where h is a hexadecimal digit
+-----------------+---------------------+----------------------------------------------------------------------------------------
*/





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static td_bool_t td_c_char_compare (td_char_t ch1, td_char_t ch2, td_bool_t case_sensitive)
{
	if (TD_TRUE==case_sensitive)
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


static void td_zero_memory (td_pointer_t pointer, td_uint_t size)
{
	td_byte_t* address;
	td_uint_t  i;
	td_byte_t  zero;


	if (TD_NULL_POINTER==pointer)
	{
		return;
	}


	address = (td_byte_t*)pointer;
	zero    = 0u;

	for (i=0u; i<size; i++)
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
void td_string_set_null (td_string_t* p)
{
	p->begin  = TD_NULL_POINTER;
	p->end    = TD_NULL_POINTER;
	p->length = 0u;
}

/*
void td_string_shallow_copy (td_string_t* s, td_string_t* d)
{
	d->begin  = s->begin ;
	d->end    = s->end   ;
	d->length = s->length;
}
*/

//===========================================================================
void td_string_trim (td_string_t* p)
{
	td_char_t ch;


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
		else
		{
			break;
		}
	}

	while(0u<p->length)
	{
		ch = *(p->end-1);
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
}

//===========================================================================
/*
td_bool_t td_string_is_dquott_expression (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);

		if ( ('\"' == ch1) && ('\"' == ch2) )
		{
			return TD_TRUE;
		}
	}

	return TD_FALSE;
}

void td_string_trim_double_qutation_expression (td_string_t* p)
{
	td_char_t ch1;
	td_char_t ch2;


	if (2u<=p->length)
	{
		ch1 = *(p->begin);
		ch2 = *(p->end-1);

		if ( ('\"' == ch1) && ('\"' == ch2) )
		{
			p->begin++;
			p->end--;
			p->length-=2u;
		}
	}
}
*/

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

td_uint_t td_string_copy_to_c_string (td_string_t* p, td_char_t* pointer, td_uint_t size)
{
	if (TD_NULL_POINTER==pointer)
	{
		return 0u;
	}

	if (0u==size)
	{
		return 0u;
	}


	td_uint_t i;


	for (i=0u; (i<size-1u) && (i<p->length); i++)
	{
		*pointer = *(p->begin+i);

		pointer++;
	}

	*pointer = 0x00;


	// NULL 문자 포함 복사 크기 반환

	return i+1u;
}

//===========================================================================
// atof
td_double_t td_string_to_real_number (td_string_t* p)
{
	td_int_t    sign; 
	td_double_t value; 
	td_double_t power;

	td_uint_t  state; 
	td_char_t* s;
	td_char_t  ch;
	td_bool_t  loop;



	if (0u==p->length)
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
td_int_t td_string_to_integer (td_string_t* p)
{
	td_int_t sign; 
	td_int_t value; 

	td_uint_t  state; 
	td_char_t* s;
	td_char_t  ch;
	td_bool_t  loop;



	if (0u==p->length)
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

td_uint_t td_string_to_uinteger (td_string_t* p)
{
	td_uint_t value; 

	td_uint_t  state; 
	td_char_t* s;
	td_char_t  ch;
	td_bool_t  loop;



	if (0u==p->length)
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

td_uint_t td_string_to_ip_v4 (td_string_t* p)
{
	// ip adddress: "000.000.000.000" 16 bytes
	td_char_t* address_string;

	td_uint_t string_length;
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
	

	address_string = p->begin;

	if (TD_NULL_POINTER!=address_string)
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

				ch = *(address_string+i);

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
