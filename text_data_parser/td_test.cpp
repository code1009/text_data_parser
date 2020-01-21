// text_data_parser.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


#if 1
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
[class]                           \r\n\
var_i  = -12                      \r\n\
var_u  = -12                      \r\n\
var_f  = 10.05                    \r\n\
var_s  = \"xxx\"                  \r\n\
var_ip = 255.0.254.0              \r\n\
                                  \r\n\
[array]                           \r\n\
element = , = 1,(2),a,[], \" x \" \r\n\
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

	char array[10][100];
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
static void array_element (td_array_parser_t* ctx)
{
	user_data_t* data;

	data = (user_data_t*)ctx->parameter;


	td_string_copy_to_c_string(&ctx->element_value, data->array[ctx->element_index], sizeof(data->array[ctx->element_index]));


	printf("\tarray[%d]={",ctx->element_index);
	print (&ctx->element_value);
	printf("}\r\n");
}
*/

static void comment  (td_ini_t* ctx){printf("comment ={");print(td_ini_comment (ctx));printf("}\r\n");}
static void section  (td_ini_t* ctx){printf("section ={");print(td_ini_section (ctx));printf("}\r\n");}
static void variable (td_ini_t* ctx){printf("variable={");print(td_ini_variable(ctx));printf("}\r\n");}
static void value    (td_ini_t* ctx){printf("value   ={");print(td_ini_value   (ctx));printf("}\r\n");}

static void element  (td_ini_t* ctx){printf("element ={");print(td_ini_section (ctx));printf("}{");
                                                          print(td_ini_variable(ctx));printf("}{");
                                                          print(td_ini_value   (ctx));printf("}\r\n");
	user_data_t* data;

	data = (user_data_t*)ctx->parameter;


	td_char_t s[100];

	td_string_copy_to_c_string(td_ini_section (ctx), s, 100);
	td_string_copy_to_c_string(td_ini_variable(ctx), s, 100);
	td_string_copy_to_c_string(td_ini_value   (ctx), s, 100);


/*	
	if (TD_TRUE==td_ini_is_section_variable(ctx, "array", "element"))
	{
		td_array_parser_parse(data, array_element, td_ini_value(ctx));
	}
*/

	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_i"))
	{
		data->var_i = td_string_to_integer(td_ini_value(ctx));
		printf("%d\r\n", data->var_i);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_u"))
	{
		data->var_u = td_string_to_uinteger(td_ini_value(ctx));
		printf("%u\r\n", data->var_u);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_f"))
	{
		data->var_f = td_string_to_real_number(td_ini_value(ctx));
		printf("%f\r\n", data->var_f);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_ip"))
	{
		data->var_ip = td_string_to_ip_v4(td_ini_value(ctx));
		printf("%x\r\n", data->var_ip);
	}
	if (TD_TRUE==td_ini_is_section_variable(ctx, "class", "var_s"))
	{
//		td_string_trim_double_qutation_expression(td_ini_value(ctx));
		td_string_copy_to_c_string(td_ini_value(ctx), data->var_s, 100);
		printf("%s\r\n", data->var_s);
	}


	if (TD_TRUE==td_ini_is_section_variable(ctx, "한글", "변수"))
	{
//		td_string_trim_double_qutation_expression(td_ini_value(ctx));
		td_string_copy_to_c_string(td_ini_value(ctx), data->hangul, 100);
		printf("%s\r\n", data->hangul);
	}


	printf ("\r\n");
}

void td_ini_test (void)
{
	user_data_t data;
	td_ini_t ctx;


	memset(&data      , 0    , sizeof(data));
	memset(&data.array, 0xFFu, sizeof(data.array));

	
	td_ini_initialize (&ctx);
	td_ini_set_parameter        (&ctx, &data   );
	td_ini_set_handler_element  (&ctx, element );
	td_ini_set_handler_section  (&ctx, section );
	td_ini_set_handler_variable (&ctx, variable);
	td_ini_set_handler_value    (&ctx, value   );
	td_ini_set_handler_comment  (&ctx, comment );


	td_ini_parse (&ctx, _text, sizeof(_text));

	if (ctx.state != TD_INI_STATE_DONE)
	{
		printf("\r\n[ERROR] \r\n");
	}
}

/*
comment ={; comment}
section ={section}
variable={variable}
value   ={value}
element ={section}{variable}{value}

section ={class}
variable={var_i}
value   ={-12}
element ={class}{var_i}{-12}
-12

variable={var_u}
value   ={-12}
element ={class}{var_u}{-12}
0

variable={var_f}
value   ={10.05}
element ={class}{var_f}{10.05}
10.050000

variable={var_s}
value   ={"xxx"}
element ={class}{var_s}{"xxx"}
xxx

variable={var_ip}
value   ={255.0.254.0}
element ={class}{var_ip}{255.0.254.0}
ff00fe00

section ={array}
variable={element}
value   ={, = 1,(2),a,[], " x "}
element ={array}{element}{, = 1,(2),a,[], " x "}
        array[0]={}
        array[1]={= 1}
        array[2]={(2)}
        array[3]={a}
        array[4]={[]}
        array[5]={" x "}

section ={한글}
variable={변수}
value   ={값}
element ={한글}{변수}{값}
값


*/

#endif





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
int _tmain(int argc, _TCHAR* argv[])
{
	td_ini_test();

	return 0;
}




