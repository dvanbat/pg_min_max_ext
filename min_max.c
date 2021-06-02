#include <postgres.h>
#include <fmgr.h>
#include <utils/array.h>
#include <utils/lsyscache.h>
#include <utils/builtins.h>
#include <stdio.h>
#include "executor/spi.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#else
#error "PG_MODULE_MAGIC wasn't defined!"
#endif

PG_FUNCTION_INFO_V1(min_max_func);
PG_FUNCTION_INFO_V1(min_max_result);

Datum
min_max_func(PG_FUNCTION_ARGS)
{
	ArrayType*  resultArray;
    int32       arg0 = PG_GETARG_INT32(1);
  	ArrayType *array1;
  	Oid arrayElementType1;
  	int16 arrayElementTypeWidth1;
	bool arrayElementTypeByValue1;
  	char arrayElementTypeAlignmentCode1;
  	Datum *arrayContent1;
  	bool *arrayNullFlags1;
  	int arrayLength1;
  	Datum* sumContent;

  	// Extract the PostgreSQL arrays from the parameters passed to this function call.
  	array1 = PG_GETARG_ARRAYTYPE_P(0);

  	// Determine the array element types.
  	arrayElementType1 = ARR_ELEMTYPE(array1);
  	get_typlenbyvalalign(arrayElementType1, &arrayElementTypeWidth1, &arrayElementTypeByValue1, &arrayElementTypeAlignmentCode1);

  	// Extract the array contents (as Datum objects).
  	deconstruct_array(array1, arrayElementType1, arrayElementTypeWidth1, arrayElementTypeByValue1, arrayElementTypeAlignmentCode1, 
  		&arrayContent1, &arrayNullFlags1, &arrayLength1);

  	sumContent = palloc(sizeof(Datum) * arrayLength1);

  	// Generate the sums.
  	if (NULL == arg0) {
  		sumContent[0] = arrayContent1[0];
  		sumContent[1] = arrayContent1[1];
  	} else {
  		if ( arg0 < arrayContent1[0] || arrayNullFlags1[0] ) sumContent[0] = arg0;
  		if ( arg0 > arrayContent1[1] || arrayNullFlags1[1] ) sumContent[1] = arg0;
  	}

    resultArray = construct_array(sumContent, arrayLength1, arrayElementType1, arrayElementTypeWidth1, 
    	arrayElementTypeByValue1, arrayElementTypeAlignmentCode1);
    PG_RETURN_ARRAYTYPE_P(resultArray);
}

Datum
min_max_result(PG_FUNCTION_ARGS)
{
  	ArrayType *array1;
  	Oid arrayElementType1;
  	int16 arrayElementTypeWidth1;
  	bool arrayElementTypeByValue1;
  	char arrayElementTypeAlignmentCode1;
  	Datum *arrayContent1;
  	bool *arrayNullFlags1;
  	int arrayLength1;

  	array1 = PG_GETARG_ARRAYTYPE_P(0);
  	arrayElementType1 = ARR_ELEMTYPE(array1);
  	get_typlenbyvalalign(arrayElementType1, &arrayElementTypeWidth1, &arrayElementTypeByValue1, &arrayElementTypeAlignmentCode1);
  	deconstruct_array(array1, arrayElementType1, arrayElementTypeWidth1, arrayElementTypeByValue1, arrayElementTypeAlignmentCode1,
  		&arrayContent1, &arrayNullFlags1, &arrayLength1);

  	char min_str[100];
   	char max_str[100];

  	int min_num = arrayContent1[0];
   	int max_num = arrayContent1[1];

  	sprintf(min_str, "%d", min_num);
   	sprintf(max_str, "%d", max_num);

	text *min_out    = cstring_to_text(min_str);
	int32 min_out_sz = VARSIZE(min_out) - VARHDRSZ;

	text *max_out	= cstring_to_text(max_str);
	int32 max_out_sz	= VARSIZE(max_out) - VARHDRSZ;

	text *delimiter;

	/* set delimiter */
	int  ret;
    Datum result;
    bool isnull;
    /* connect to SPI manager */
    if ((ret = SPI_connect()) < 0)
        elog(INFO, "SPI_connect returned %d", ':', ret);
    ret = SPI_execute("SELECT d FROM delimiter", true, 1);

    if (SPI_processed == 1 && ret > 0)
    	{
    		result = SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull);
    		delimiter = cstring_to_text(result);
    	}
	/*if delimiter is not defined - set default */
    else delimiter = cstring_to_text(" -> ");
    SPI_finish();		

	int32 delimiter_sz	= VARSIZE(delimiter) - VARHDRSZ;

	int32 out_sz	= min_out_sz + delimiter_sz + max_out_sz + VARHDRSZ;
	text *out		= (text *) palloc(out_sz);

	SET_VARSIZE(out, out_sz);

	memcpy(VARDATA(out), VARDATA(min_out), min_out_sz);
	memcpy(VARDATA(out) + min_out_sz, VARDATA(delimiter), delimiter_sz);
	memcpy(VARDATA(out) + min_out_sz + delimiter_sz, VARDATA(max_out), max_out_sz);

	PG_RETURN_TEXT_P(out);
}