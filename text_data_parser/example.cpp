/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include <stdio.h>
#include <string.h>

#include "tdp.h"





#if 1
/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static tdp_char_t _text_data_ini[]="\
; comment                     \r\n\
[object]                      \r\n\
v_int    = -123 ; signed int  \r\n\
v_double = 0.123              \r\n\
v_ip     = 127.0.0.1          \r\n\
v_array  = value0, \"value1\"     \
";





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
typedef struct _example_object_t
{
	signed int   v_int          ;
	double       v_double       ;
	unsigned int v_ip           ;
	char         v_array [2][32];
}
example_object_t;

typedef struct _example_t
{
	example_object_t object;
}
example_t;





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
static void example_object_v_array_handler (tdp_array_t* ctx)
{
	tdp_uint_t    element_index;
	tdp_string_t* element_value;


	element_index = tdp_array_element_index(ctx);
	element_value = tdp_array_element_value(ctx);


	example_t*   data;
	char*        dpointer;
	unsigned int dlength;


	data = (example_t*)tdp_array_parameter(ctx);

	dpointer =        data->object.v_array[element_index];
	dlength  = sizeof(data->object.v_array[element_index])/sizeof(char);

	tdp_string_copy_to_c_string(element_value, dpointer, dlength);
}

static void example_object_v_array (tdp_ini_t* ini, tdp_string_t* value)
{
	tdp_array_t  array;
	tdp_array_t* ctx;


	ctx = &array;

	tdp_array_initialize          (ctx);
	tdp_array_set_parameter       (ctx, ini->parameter);
	tdp_array_set_handler_element (ctx, example_object_v_array_handler);
	tdp_array_parse               (ctx, value->begin, value->length);


	if (ctx->state != TDP_ARRAY_STATE_DONE)
	{
		printf("\r\n# ARRAY [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n", 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);

		tdp_ini_set_error(ini, value->begin);
	}
}

static void example_object (tdp_ini_t* ini)
{
	//-----------------------------------------------------------------------
	example_t* data;


	data = (example_t*)tdp_ini_parameter(ini);


	//-----------------------------------------------------------------------
	tdp_string_t* value;


	value = tdp_ini_value (ini);


	if      (TDP_TRUE==tdp_ini_is_variable (ini, "v_int"   )){data->object.v_int    = tdp_string_parse_integer    (value);}
	else if (TDP_TRUE==tdp_ini_is_variable (ini, "v_double")){data->object.v_double = tdp_string_parse_real_number(value);}
	else if (TDP_TRUE==tdp_ini_is_variable (ini, "v_ip"    )){data->object.v_ip     = tdp_string_parse_ip_v4      (value);}
	else if (TDP_TRUE==tdp_ini_is_variable (ini, "v_array" )){example_object_v_array(ini, value)                         ;}
}

//===========================================================================
static void example_handler (tdp_ini_t* ctx)
{
	if (TDP_TRUE==tdp_ini_is_section(ctx, "object"))
	{
		example_object(ctx);
	}
}

//===========================================================================
void example (void)
{
	//-----------------------------------------------------------------------
	tdp_char_t* spointer;
	tdp_uint_t  slength;


	spointer = _text_data_ini;
	slength  = strlen(_text_data_ini);
	printf ("\r\n");
	printf ("===========================================================================\r\n");
	printf ("length=%d\r\n", slength);
	printf ("===========================================================================\r\n");
	printf (_text_data_ini);
	printf ("\r\n");
	printf ("===========================================================================\r\n");


	//-----------------------------------------------------------------------
	example_t example;


	memset(&example, 0, sizeof(example));


	//-----------------------------------------------------------------------
	tdp_ini_t  ini;
	tdp_ini_t* ctx;


	ctx = &ini;

	tdp_ini_initialize          (ctx);
	tdp_ini_set_parameter       (ctx, &example);
	tdp_ini_set_handler_element (ctx, example_handler);
	tdp_ini_parse               (ctx, spointer, slength);
 	if (ctx->state != TDP_INI_STATE_DONE)
	{
		printf("\r\n# INI [ERROR LINE:%d COLUMN:%d] = %d \r\n\r\n", 
			ctx->error_line,
			ctx->error_column,
			ctx->state
			);

		return;
	}


	//-----------------------------------------------------------------------
	printf ("example.object.v_int      = {%d}     \r\n", example.object.v_int     );
	printf ("example.object.v_double   = {%f}     \r\n", example.object.v_double  );
	printf ("example.object.v_ip       = {0x%04x} \r\n", example.object.v_ip      );
	printf ("example.object.v_array[0] = {%s}     \r\n", example.object.v_array[0]);
	printf ("example.object.v_array[1] = {%s}     \r\n", example.object.v_array[1]);
	printf ("\r\n");
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
/*

===========================================================================
length=192
===========================================================================
; comment
[object]
v_int    = -123 ; signed int
v_double = 0.123
v_ip     = 127.0.0.1
v_array  = value0, "value1"
===========================================================================
example.object.v_int      = {-123}
example.object.v_double   = {0.123000}
example.object.v_ip       = {0x7f000001}
example.object.v_array[0] = {value0}
example.object.v_array[1] = {value1}

*/





#endif




