#if 1
// text_data_parser.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


/////////////////////////////////////////////////////////////////////////////
//
// test
//
/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include <stdio.h>
#include <string.h>

#include "td.h"

//#pragma execution_character_set("utf-8")

td_char_t _text[]="\
; comment                         \r\n\
[section]                         \r\n\
variable=value                    \r\n\
                                  \r\n\
[section1]                        \r\n\
variable=value1,                \\\r\n\
value2, value3,                 \\\r\n\
 value4,value5                    \r\n\
                                  \r\n\
[\" section2 \\\" 0] \"]          \r\n\
variable1=value               \\\\\r\n\
\" variable2= \" = value          \r\n\
                                  \r\n\
[class]                           \r\n\
var_i  = -12                      \r\n\
var_u  = -12                      \r\n\
var_f  = 10.05                    \r\n\
var_s  = \"xxx\"                  \r\n\
var_ip = 255.0.254.0              \r\n\
                                  \r\n\
[array]                           \r\n\
element = , = 1, (2), a, , [], { \" x, \" }, 1 \r\n\
                                  \r\n\
[한글]                            \r\n\
변수 = 값                         \r\n\
";

typedef struct _user_data_t
{
	int          var_i;
	unsigned int var_u;
	double       var_f;
	unsigned int var_ip;
	char         var_s[100];

	char hangul[100];

	char array[20][100];
}
user_data_t;

static void print(td_string_t* p)
{
	td_uint_t i;
	td_char_t ch;


	for (i=0u; i<p->length; i++)
	{
		ch = *(p->begin+i);

		printf("%c", ch);
	}
}
/*
static void array_element (td_array_t* ctx)
{
	user_data_t* data;
	td_uint_t    index;
	td_string_t* value;


	data  = (user_data_t*)td_array_parameter(ctx);
	index = td_array_element_index(ctx);
	value = td_array_element(ctx);

	td_string_copy_to_c_string(value, data->array[index], sizeof(data->array[index]) );


	printf("\tarray[%d]={",index);
	print (&ctx->element_value);
	printf("}\r\n");
}
*/

static void line     (td_ini_t* ctx){printf("\r\n"
                                            "line    ={");print(td_ini_line    (ctx));printf("}\r\n");}
static void comment  (td_ini_t* ctx){printf("comment ={");print(td_ini_comment (ctx));printf("}\r\n");}
static void section  (td_ini_t* ctx){printf("section ={");print(td_ini_section (ctx));printf("}\r\n");}
static void variable (td_ini_t* ctx){printf("variable={");print(td_ini_variable(ctx));printf("}\r\n");}
static void value    (td_ini_t* ctx){printf("value   ={");print(td_ini_value   (ctx));printf("}\r\n");}
                                            
static void element  (td_ini_t* ctx){printf("element ={");print(td_ini_section (ctx));printf("}{");
                                                          print(td_ini_variable(ctx));printf("}{");
                                                          print(td_ini_value   (ctx));printf("}\r\n");
	user_data_t* data;

	data = (user_data_t*)td_ini_parameter(ctx);


	td_char_t s[100];


	td_string_copy_to_c_string(td_ini_section (ctx), s, 100);printf("        =""{%s}\r\n", s);
	td_string_copy_to_c_string(td_ini_variable(ctx), s, 100);printf("        =""{%s}\r\n", s);
	td_string_copy_to_c_string(td_ini_value   (ctx), s, 100);printf("        =""{%s}\r\n", s);


/*	
	if (TD_TRUE==td_ini_is_section_variable(ctx, "array", "element"))
	{
		td_array_t ctx_array;

		td_array_initialize          (&ctx_array);
		td_array_set_parameter       (&ctx_array, data);
		td_array_set_handler_element (&ctx_array, array_element);
		td_array_parse               (&ctx_array, td_ini_value(ctx)->begin, td_ini_value(ctx)->length);
	}
*/

	if (TD_TRUE==td_ini_is_section_variable(ctx, "section1", "variable"))
	{
		td_string_copy_to_c_string_without_multiline_escape(td_ini_value(ctx), s, 100);

		printf("        :""{%s}\r\n", s);
	}

	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_i"))
	{
		data->var_i = td_string_parse_integer(td_ini_value(ctx));
		printf("        :""{%d}\r\n", data->var_i);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_u"))
	{
		data->var_u = td_string_parse_uinteger(td_ini_value(ctx));
		printf("        :""{%u}\r\n", data->var_u);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_f"))
	{
		data->var_f = td_string_parse_real_number(td_ini_value(ctx));
		printf("        :""{%f}\r\n", data->var_f);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_ip"))
	{
		data->var_ip = td_string_parse_ip_v4(td_ini_value(ctx));
		printf("        :""{%x}\r\n", data->var_ip);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_s"))
	{
		td_string_trim_dquot(td_ini_value(ctx));
		td_string_copy_to_c_string(td_ini_value(ctx), data->var_s, 100);
		printf("        :""{%s}\r\n", data->var_s);
	}

	if (TD_TRUE==td_ini_is_section_variable(ctx, "한글", "변수"))
	{
		td_string_trim_dquot(td_ini_value(ctx));
		td_string_copy_to_c_string(td_ini_value(ctx), data->hangul, 100);
		printf("        :""{%s}\r\n", data->hangul);
	}


	printf ("\r\n");
}

void td_ini_test (void)
{
	user_data_t data;
	td_ini_t ctx_ini;
	td_ini_t* ctx;

	memset(&data      , 0    , sizeof(data));
	memset(&data.array, 0xFFu, sizeof(data.array));

	
	ctx = &ctx_ini;
	td_ini_initialize (ctx);
	td_ini_set_parameter        (ctx, &data   );
#if 1
	td_ini_set_handler_line     (ctx, line    );
	td_ini_set_handler_comment  (ctx, comment );
	td_ini_set_handler_section  (ctx, section );
	td_ini_set_handler_variable (ctx, variable);
	td_ini_set_handler_value    (ctx, value   );
#endif
	td_ini_set_handler_element  (ctx, element );


//	td_ini_parse (ctx, _text, sizeof(_text)); // +2 바이트 됨
	td_ini_parse (ctx, _text, strlen(_text));

	if (ctx->state != TD_INI_STATE_DONE)
	{
		printf("\r\n[ERROR] = %d \r\n", ctx->state);
	}
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
int _tmain(int argc, _TCHAR* argv[])
{
	td_ini_test();

	return 0;
}





#endif




