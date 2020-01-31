/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include <stdio.h>
#include <string.h>

#include <tchar.h>

#include "tdp.h"





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
//#pragma execution_character_set("utf-8")





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static tdp_char_t _text_data_ini[]=_T("\
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





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
typedef struct _test_t
{
	int          var_i;
	unsigned int var_u;
	double       var_f;
	unsigned int var_ip;
	tdp_char_t   var_s[100];

	tdp_char_t array[20][100];

	tdp_char_t hangul[100];
}
test_t;





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static void print_tdp_string(tdp_string_t* p)
{
	tdp_uint_t i;
	tdp_char_t ch;


	for (i=0u; i<p->length; i++)
	{
		ch = *(p->begin+i);

		_tprintf(_T("%c"), ch);
	}
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static void array_handler_line (tdp_array_t* ctx)
{
	tdp_string_t* line;


	line = tdp_array_line(ctx);


	_tprintf(_T("\tARRAY-LINE={"));
	print_tdp_string (line);
	_tprintf(_T("}\r\n"));
}

static void array_handler_element_value (tdp_array_t* ctx)
{
	tdp_uint_t    element_index;
	tdp_string_t* element_value;


	element_index = tdp_array_element_index(ctx);
	element_value = tdp_array_element_value(ctx);


	_tprintf(_T("\tARRAY-ELEMENT-VALUE[%d]={"), element_index);
	print_tdp_string (element_value);
	_tprintf(_T("}\r\n"));
}

static void array_handler_element (tdp_array_t* ctx)
{
	tdp_uint_t    element_index;
	tdp_string_t* element_value;


	element_index = tdp_array_element_index(ctx);
	element_value = tdp_array_element_value(ctx);


	_tprintf(_T("\tARRAY-ELEMENT[%d]={"), element_index);
	print_tdp_string (element_value);
	_tprintf(_T("}\r\n"));
}



//===========================================================================
static void array_handler_element2 (tdp_array_t* ctx)
{
	tdp_uint_t    element_index;
	tdp_string_t* element_value;


	element_index = tdp_array_element_index(ctx);
	element_value = tdp_array_element_value(ctx);


	_tprintf(_T("\t\t[%d]={"), element_index);
	print_tdp_string (element_value);
	_tprintf(_T("}\r\n"));
}

static void array_handler_element1 (tdp_array_t* ctx)
{
	//--------------------------------------------------------------------------
	tdp_uint_t    element_index;
	tdp_string_t* element_value;


	element_index = tdp_array_element_index(ctx);
	element_value = tdp_array_element_value(ctx);


	_tprintf(_T("\t[%d]={"), element_index);
	print_tdp_string (element_value);
	_tprintf(_T("}\r\n"));


	//--------------------------------------------------------------------------
	test_t* test;


	test = (test_t*)tdp_array_parameter(ctx);


	//--------------------------------------------------------------------------
	tdp_string_copy_to_c_string(element_value, test->array[element_index], sizeof(test->array[element_index]) );


	//--------------------------------------------------------------------------
	if (element_index==7u)
	{
		tdp_array_t array;


		tdp_string_trim_braces(element_value);


		tdp_array_initialize (&array);
		tdp_array_set_handler_element (&array, array_handler_element2);		
		tdp_array_parse (&array, element_value->begin, element_value->length);
		if (array.state != TDP_ARRAY_STATE_DONE)
		{
			_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
				array.error_line,
				array.error_column,
				array.state
				);
		}
	}
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static void ini_section1 (test_t* test, tdp_string_t* variable, tdp_string_t* value)
{
	tdp_array_t  array;
	tdp_array_t* ctx;


	ctx = &array;

	tdp_array_initialize (ctx);

	tdp_array_set_parameter (ctx, test);

//	tdp_array_set_handler_line          (ctx, array_handler_line);
//	tdp_array_set_handler_element_value (ctx, array_handler_element_value);
	tdp_array_set_handler_element       (ctx, array_handler_element);

	tdp_array_parse (ctx, value->begin, value->length);


	if (ctx->state != TDP_ARRAY_STATE_DONE)
	{
		_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);
	}
}

static void ini_section2 (test_t* test, tdp_string_t* variable, tdp_string_t* value)
{
	tdp_array_t  array;
	tdp_array_t* ctx;


	ctx = &array;

	tdp_array_initialize (ctx);

	tdp_array_set_parameter (ctx, test);

//	tdp_array_set_handler_line          (ctx, array_handler_line);
//	tdp_array_set_handler_element_value (ctx, array_handler_element_value);
	tdp_array_set_handler_element       (ctx, array_handler_element);

	tdp_array_parse (ctx, value->begin, value->length);


	if (ctx->state != TDP_ARRAY_STATE_DONE)
	{
		_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);
	}
}

static void ini_class (test_t* test, tdp_string_t* variable, tdp_string_t* value)
{
	if (TDP_TRUE==tdp_string_compare(variable, _T("var_i"), TDP_FALSE))
	{
		test->var_i = tdp_string_parse_integer(value);

		_tprintf(_T(":{%d}\r\n"), test->var_i);
	}

	else if (TDP_TRUE==tdp_string_compare(variable, _T("var_u"), TDP_FALSE))
	{
		test->var_u = tdp_string_parse_uinteger(value);

		_tprintf(_T(":{%u}\r\n"), test->var_u);
	}

	else if (TDP_TRUE==tdp_string_compare(variable, _T("var_f"), TDP_FALSE))
	{
		test->var_f = tdp_string_parse_real_number(value);

		_tprintf(_T(":{%f}\r\n"), test->var_f);
	}

	else if (TDP_TRUE==tdp_string_compare(variable, _T("var_ip"), TDP_FALSE))
	{
		test->var_ip = tdp_string_parse_ip_v4(value);
		
		_tprintf(_T(":{%x}\r\n"), test->var_ip);
	}

	else if (TDP_TRUE==tdp_string_compare(variable, _T("var_s"), TDP_FALSE))
	{
		tdp_string_copy_to_c_string(value, test->var_s, 100);

		_tprintf(_T(":{%s}\r\n"), test->var_s);
	}
}

static void ini_array (test_t* test, tdp_string_t* variable, tdp_string_t* value)
{
	if (TDP_TRUE==tdp_string_compare(variable, _T("element"), TDP_FALSE))
	{
		tdp_array_t  array;
		tdp_array_t* ctx;


		ctx = &array;

		tdp_array_initialize (ctx);
		tdp_array_set_parameter (ctx, test);
		tdp_array_set_handler_element (ctx, array_handler_element1);
		tdp_array_parse (ctx, value->begin, value->length);
		if (ctx->state != TDP_ARRAY_STATE_DONE)
		{
			_tprintf(_T("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
				ctx->error_line,
				ctx->error_column,
				ctx->state
				);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static void ini_handler_line     (tdp_ini_t* ctx){_tprintf(_T("\r\n")
                                                           _T("INI-LINE    ={"));print_tdp_string(tdp_ini_line    (ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_comment  (tdp_ini_t* ctx){_tprintf(_T("INI-COMMENT ={"));print_tdp_string(tdp_ini_comment (ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_section  (tdp_ini_t* ctx){_tprintf(_T("INI-SECTION ={"));print_tdp_string(tdp_ini_section (ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_variable (tdp_ini_t* ctx){_tprintf(_T("INI-VARIABLE={"));print_tdp_string(tdp_ini_variable(ctx));_tprintf(_T("}\r\n"));}
static void ini_handler_value    (tdp_ini_t* ctx){_tprintf(_T("INI-VALUE   ={"));print_tdp_string(tdp_ini_value   (ctx));_tprintf(_T("}\r\n"));}
                                            
static void ini_handler_element  (tdp_ini_t* ctx){_tprintf(_T("INI-ELEMENT ={"));print_tdp_string(tdp_ini_section (ctx));_tprintf(_T("}{"));
                                                                                 print_tdp_string(tdp_ini_variable(ctx));_tprintf(_T("}{"));
                                                                                 print_tdp_string(tdp_ini_value   (ctx));_tprintf(_T("}\r\n"));
	//-----------------------------------------------------------------------
	test_t* test;

	
	test = (test_t*)tdp_ini_parameter(ctx);


	//-----------------------------------------------------------------------
	tdp_string_t* section;
	tdp_string_t* variable;
	tdp_string_t* value;


	section  = tdp_ini_section (ctx);
	variable = tdp_ini_variable(ctx);
	value    = tdp_ini_value   (ctx);


	//-----------------------------------------------------------------------
#if 0
	tdp_char_t s[100];


	tdp_string_copy_to_c_string(section , s, 100);_tprintf(_T("            =""{%s}\r\n"), s);
	tdp_string_copy_to_c_string(variable, s, 100);_tprintf(_T("            =""{%s}\r\n"), s);
	tdp_string_copy_to_c_string(value   , s, 100);_tprintf(_T("            =""{%s}\r\n"), s);
#endif


	//-----------------------------------------------------------------------
	if (TDP_TRUE==tdp_ini_is_section(ctx, _T("section1")))
	{
		ini_section1(test, variable, value);
	}

	if (TDP_TRUE==tdp_ini_is_section(ctx, _T("section2")))
	{
		ini_section2(test, variable, value);
	}

	if (TDP_TRUE==tdp_ini_is_section(ctx, _T("class")))
	{
		ini_class(test, variable, value);
	}

	if (TDP_TRUE==tdp_ini_is_section(ctx, _T("array")))
	{
		ini_array(test, variable, value);
	}

	if (TDP_TRUE==tdp_ini_is_section_variable(ctx, _T("한글"), _T("변수")))
	{
		tdp_string_copy_to_c_string(value, test->hangul, 100);

		_tprintf(_T(":{%s}\r\n"), test->hangul);
	}


	_tprintf (_T("\r\n"));
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
void test (void)
{
	//-----------------------------------------------------------------------
	tdp_char_t* spointer;
	tdp_uint_t  slength;


	spointer = _text_data_ini;
	slength  = _tcslen(_text_data_ini);
	_tprintf (_T("\r\n"));
	_tprintf (_T("===========================================================================\r\n"));
	_tprintf (_T("length=%d\r\n"), slength);
	_tprintf (_T("===========================================================================\r\n"));
	_tprintf (_text_data_ini);
	_tprintf (_T("\r\n"));
	_tprintf (_T("===========================================================================\r\n"));


	//-----------------------------------------------------------------------
	test_t test;


	memset(&test      , 0    , sizeof(test));
	memset(&test.array, 0xFFu, sizeof(test.array));


	//-----------------------------------------------------------------------
	tdp_ini_t  ini;
	tdp_ini_t* ctx;


	ctx = &ini;

	tdp_ini_initialize (ctx);

	tdp_ini_set_parameter (ctx, &test);

//	tdp_ini_set_handler_line     (ctx, ini_handler_line    );
	tdp_ini_set_handler_comment  (ctx, ini_handler_comment );
	tdp_ini_set_handler_section  (ctx, ini_handler_section );
//	tdp_ini_set_handler_variable (ctx, ini_handler_variable);
//	tdp_ini_set_handler_value    (ctx, ini_handler_value   );

	tdp_ini_set_handler_element  (ctx, ini_handler_element );

	tdp_ini_parse (ctx, spointer, slength);


 	if (ctx->state != TDP_INI_STATE_DONE)
	{
		_tprintf(_T("\r\n# INI [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n"), 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);
	}
}
