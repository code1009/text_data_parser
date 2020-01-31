// text_data_parser.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#if 1
#include "stdafx.h"


/////////////////////////////////////////////////////////////////////////////
//
// test
//
/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include <stdio.h>
#include <fcntl.h>  
#include <io.h>
#include <string.h>
#include <tchar.h>
#include "td.h"

//===========================================================================
//#pragma execution_character_set("utf-8")
static td_char_t _text_data_string[]=_T("\
; comment                         \r\n\
[section]                         \r\n\
variable=value                    \r\n\
\r\n\
[section1]                        \r\n\
variable = value1  , \\\r\n\
  value2 ,value3     \\\r\n\
, (value4,value5) ,  \\\r\n\
\r\n\
[section2] ;comment1              \r\n\
variable1=  value1  \\\\;comment-a\r\n\
   \" variable2= \" = value       \r\n\
\r\n\
[section3]                        \r\n\
variable3 = value3   ; comment-b  \r\n\
[\" section4 \\\" 0] \"]          \r\n\
\r\n\
[class]              ; comment01  \r\n\
var_i  = -12         ; comment02  \r\n\
var_u  = -12         ; comment03  \r\n\
var_f  = 10.05       ; comment04  \r\n\
var_s  = \"xxx\"     ; comment05  \r\n\
var_ip = 255.0.254.0 ; comment06  \r\n\
var_ip = 255.0.254.1 ; comment07  \r\n\
\r\n\
[array]                           \r\n\
element =  , = 1, (2), d, \"e;\", (;), { \" x, \" }, {1,2,3,4,}, 0,  \r\n\
element = {, = 1, (2), d, \"e;\", (;), { \" x, \" }, {1,2,3,4,}, 0,} \r\n\
element = \"a\", \"b\", c\\\"\r\n\
\r\n\
[한글]                            \r\n\
변수 = 값 \";\"                      ");



//===========================================================================
typedef struct _user_data_t
{
	int          var_i;
	unsigned int var_u;
	double       var_f;
	unsigned int var_ip;
	td_char_t    var_s[100];

	td_char_t hangul[100];

	td_char_t array[20][100];
}
user_data_t;



//===========================================================================
static void print_td_string(td_string_t* p)
{
	td_uint_t i;
	td_char_t ch;


	for (i=0u; i<p->length; i++)
	{
		ch = *(p->begin+i);

		_tprintf(_T("%c"), ch);
	}
}



//===========================================================================
static void array_handler_line (td_array_t* ctx)
{
	td_string_t* line;


	line = td_array_line(ctx);


	_tprintf(_T("\tARRAY-LINE={"));
	print_td_string (line);
	_tprintf(_T("}\r\n"));
}

static void array_handler_element_value (td_array_t* ctx)
{
	td_uint_t    element_index;
	td_string_t* element_value;


	element_index = td_array_element_index(ctx);
	element_value = td_array_element_value(ctx);


	_tprintf(_T("\tARRAY-ELEMENT-VALUE[%d]={"), element_index);
	print_td_string (element_value);
	_tprintf(_T("}\r\n"));
}

static void array_handler_element (td_array_t* ctx)
{
	td_uint_t    element_index;
	td_string_t* element_value;


	element_index = td_array_element_index(ctx);
	element_value = td_array_element_value(ctx);


	_tprintf(_T("\tARRAY-ELEMENT[%d]={"), element_index);
	print_td_string (element_value);
	_tprintf(_T("}\r\n"));
}

//===========================================================================
static void array_handler_element2 (td_array_t* ctx)
{
	td_uint_t    element_index;
	td_string_t* element_value;


	element_index = td_array_element_index(ctx);
	element_value = td_array_element_value(ctx);


	_tprintf(_T("\t\t[%d]={"), element_index);
	print_td_string (element_value);
	_tprintf(_T("}\r\n"));
}

static void array_handler_element1 (td_array_t* ctx)
{
	//--------------------------------------------------------------------------
	td_uint_t    element_index;
	td_string_t* element_value;


	element_index = td_array_element_index(ctx);
	element_value = td_array_element_value(ctx);


	_tprintf(_T("\t[%d]={"), element_index);
	print_td_string (element_value);
	_tprintf(_T("}\r\n"));


	//--------------------------------------------------------------------------
	user_data_t* user_data;


	user_data = (user_data_t*)td_array_parameter(ctx);


	//--------------------------------------------------------------------------
	td_string_copy_to_c_string(element_value, user_data->array[element_index], sizeof(user_data->array[element_index]) );


	//--------------------------------------------------------------------------
	if (element_index==7u)
	{
		td_array_t array;


		td_string_trim_braces(element_value);


		td_array_initialize (&array);
		td_array_set_handler_element (&array, array_handler_element2);		
		td_array_parse (&array, element_value->begin, element_value->length);
		if (array.state != TD_ARRAY_STATE_DONE)
		{
			_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
				array.error_line,
				array.error_column,
				array.state
				);
		}
	}
}

//===========================================================================
static void process_ini_section1 (user_data_t* user_data, td_string_t* variable, td_string_t* value)
{
	td_array_t  array;
	td_array_t* ctx;


	ctx = &array;

	td_array_initialize (ctx);

	td_array_set_parameter (ctx, user_data);

//	td_array_set_handler_line          (ctx, array_handler_line);
//	td_array_set_handler_element_value (ctx, array_handler_element_value);
	td_array_set_handler_element       (ctx, array_handler_element);

	td_array_parse (ctx, value->begin, value->length);


	if (ctx->state != TD_ARRAY_STATE_DONE)
	{
		_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);
	}
}

static void process_ini_section2 (user_data_t* user_data, td_string_t* variable, td_string_t* value)
{
	td_array_t  array;
	td_array_t* ctx;


	ctx = &array;

	td_array_initialize (ctx);

	td_array_set_parameter (ctx, user_data);

//	td_array_set_handler_line          (ctx, array_handler_line);
//	td_array_set_handler_element_value (ctx, array_handler_element_value);
	td_array_set_handler_element       (ctx, array_handler_element);

	td_array_parse (ctx, value->begin, value->length);


	if (ctx->state != TD_ARRAY_STATE_DONE)
	{
		_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);
	}
}

static void process_ini_class (user_data_t* user_data, td_string_t* variable, td_string_t* value)
{
	if (TD_TRUE==td_string_compare(variable, _T("var_i"), TD_FALSE))
	{
		user_data->var_i = td_string_parse_integer(value);

		_tprintf(_T(":{%d}\r\n"), user_data->var_i);
	}

	else if (TD_TRUE==td_string_compare(variable, _T("var_u"), TD_FALSE))
	{
		user_data->var_u = td_string_parse_uinteger(value);

		_tprintf(_T(":{%u}\r\n"), user_data->var_u);
	}

	else if (TD_TRUE==td_string_compare(variable, _T("var_f"), TD_FALSE))
	{
		user_data->var_f = td_string_parse_real_number(value);

		_tprintf(_T(":{%f}\r\n"), user_data->var_f);
	}

	else if (TD_TRUE==td_string_compare(variable, _T("var_ip"), TD_FALSE))
	{
		user_data->var_ip = td_string_parse_ip_v4(value);
		
		_tprintf(_T(":{%x}\r\n"), user_data->var_ip);
	}

	else if (TD_TRUE==td_string_compare(variable, _T("var_s"), TD_FALSE))
	{
		td_string_copy_to_c_string(value, user_data->var_s, 100);

		_tprintf(_T(":{%s}\r\n"), user_data->var_s);
	}
}

static void process_ini_array (user_data_t* user_data, td_string_t* variable, td_string_t* value)
{
	if (TD_TRUE==td_string_compare(variable, _T("element"), TD_FALSE))
	{
		td_array_t  array;
		td_array_t* ctx;


		ctx = &array;

		td_array_initialize (ctx);
		td_array_set_parameter (ctx, user_data);
		td_array_set_handler_element (ctx, array_handler_element1);
		td_array_parse (ctx, value->begin, value->length);
		if (ctx->state != TD_ARRAY_STATE_DONE)
		{
			_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
				ctx->error_line,
				ctx->error_column,
				ctx->state
				);
		}
	}
}

//===========================================================================
static void ini_handler_line     (td_ini_t* ctx){_tprintf(_T("\r\n")
                                                          _T("LINE    ={"));print_td_string(td_ini_line    (ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_comment  (td_ini_t* ctx){_tprintf(_T("COMMENT ={"));print_td_string(td_ini_comment (ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_section  (td_ini_t* ctx){_tprintf(_T("SECTION ={"));print_td_string(td_ini_section (ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_variable (td_ini_t* ctx){_tprintf(_T("VARIABLE={"));print_td_string(td_ini_variable(ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_value    (td_ini_t* ctx){_tprintf(_T("VALUE   ={"));print_td_string(td_ini_value   (ctx));_tprintf(_T("}\r\n"));}
                                            
static void ini_handler_element  (td_ini_t* ctx){_tprintf(_T("ELEMENT ={"));print_td_string(td_ini_section (ctx));_tprintf(_T("}{"));
                                                                            print_td_string(td_ini_variable(ctx));_tprintf(_T("}{"));
                                                                            print_td_string(td_ini_value   (ctx));_tprintf(_T("}\r\n"));
	//-----------------------------------------------------------------------
	user_data_t* user_data;

	
	user_data = (user_data_t*)td_ini_parameter(ctx);


	//-----------------------------------------------------------------------
	td_string_t* section;
	td_string_t* variable;
	td_string_t* value;


	section  = td_ini_section (ctx);
	variable = td_ini_variable(ctx);
	value    = td_ini_value   (ctx);


	//-----------------------------------------------------------------------
#if 0
	td_char_t s[100];


	td_string_copy_to_c_string(section , s, 100);_tprintf(_T("        =""{%s}\r\n"), s);
	td_string_copy_to_c_string(variable, s, 100);_tprintf(_T("        =""{%s}\r\n"), s);
	td_string_copy_to_c_string(value   , s, 100);_tprintf(_T("        =""{%s}\r\n"), s);
#endif


	//-----------------------------------------------------------------------
	if (TD_TRUE==td_ini_is_section(ctx, _T("section1")))
	{
		process_ini_section1(user_data, variable, value);
	}

	if (TD_TRUE==td_ini_is_section(ctx, _T("section2")))
	{
		process_ini_section2(user_data, variable, value);
	}

	if (TD_TRUE==td_ini_is_section(ctx, _T("class")))
	{
		process_ini_class(user_data, variable, value);
	}

	if (TD_TRUE==td_ini_is_section(ctx, _T("array")))
	{
		process_ini_array(user_data, variable, value);
	}

	if (TD_TRUE==td_ini_is_section_variable(ctx, _T("한글"), _T("변수")))
	{
		td_string_copy_to_c_string(value, user_data->hangul, 100);

		_tprintf(_T(":{%s}\r\n"), user_data->hangul);
	}


	_tprintf (_T("\r\n"));
}

//===========================================================================
void td_ini_test (void)
{
	//----------------------------------------------------------------------------------------
	td_char_t* spointer;
	td_uint_t  slength;


	spointer = _text_data_string;
	slength  = _tcslen(_text_data_string);
	_tprintf (_T("\r\n"));
	_tprintf (_T("===========================================================================\r\n"));
	_tprintf (_T("length=%d\r\n"), slength);
	_tprintf (_T("===========================================================================\r\n"));
	_tprintf (_text_data_string);
	_tprintf (_T("\r\n"));
	_tprintf (_T("===========================================================================\r\n"));


	//----------------------------------------------------------------------------------------
	user_data_t user_data;


	memset(&user_data      , 0    , sizeof(user_data));
	memset(&user_data.array, 0xFFu, sizeof(user_data.array));


	//----------------------------------------------------------------------------------------
	td_ini_t  ini;
	td_ini_t* ctx;


	ctx = &ini;

	td_ini_initialize (ctx);

	td_ini_set_parameter (ctx, &user_data);

//	td_ini_set_handler_line     (ctx, ini_handler_line    );
	td_ini_set_handler_comment  (ctx, ini_handler_comment );
	td_ini_set_handler_section  (ctx, ini_handler_section );
//	td_ini_set_handler_variable (ctx, ini_handler_variable);
//	td_ini_set_handler_value    (ctx, ini_handler_value   );

	td_ini_set_handler_element  (ctx, ini_handler_element );

	td_ini_parse (ctx, spointer, slength);


 	if (ctx->state != TD_INI_STATE_DONE)
	{
		_tprintf(_T("\r\n# INI [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);
	}
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
int _tmain(int argc, _TCHAR* argv[])
{
#if 0
	_setmode(_fileno(stdout), _O_U16TEXT);
	wchar_t* s = L"한";
	wprintf(L"%ls", s);
#endif

	td_ini_test();

	return 0;
}





#endif




