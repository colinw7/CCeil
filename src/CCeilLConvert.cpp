#include <CCeilL.h>

/*------------------------------------------------------------------*
 *
 * setTypedStringArrayArg
 *   Create a CL String Array from an application defined type using a
 *   conversion routine.
 *
 * CALL:
 *   setTypedStringArrayArg(int num, char *from_type, int num_args,
 *                          char *arg, ..., int &error_code);
 *
 * INPUT:
 *   num        : The number of the argument to be set.
 *
 *   from_type  : The name of the application defined type which is to be converted from.
 *              : A converter for this type to a "cl_string_array" must have been
 *              : added using addConverter().
 *
 *   num_args   : The number of arguments which are to be passed to the conversion routine.
 *
 *   arg, ...   : The arguments to be passed to the conversion routine (there must be
 *              : exactly the same number as the specified number of arguments
 *              : 'num_args'.
 *
 * OUTPUT:
 *   error_code : Error Return Code :-
 *              :  0 - Success
 *              :  1 - Failure
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   The conversion routine will be called as follows :-
 *
 *     int flag = (*conversion_proc)
 *                  (char **&strings, uint *&dims, uint &num_dims, char *arg, ...);
 *
 *     ON ENTRY
 *       arg, ... : The arguments specified as those to
 *                : be passed to the conversion routine.
 *
 *     ON RETURN
 *       strings  : Array of allocated strings.
 *
 *       dims     : Dimensions of str arrays.
 *
 *       num_dims : Number of dimensions of str arrays.
 *
 *     RETURNS
 *       flag     : Whether the conversion was successful or not.
 *
 *------------------------------------------------------------------*/

void
ClLanguageConverterMgr::
setTypedStringArrayArg(ClLanguageArgs *args, int num, const char *from_type, int num_args,
                       char *arg, ...)
{
  va_list  vargs;
  char    *cargs[CL_MAX_CONVERT_ARGS];

  /*CONSTANTCONDITION*/
  va_start(vargs, arg);

  int num_args1 = 0;

  cargs[num_args1++] = arg;

  while (num_args1 != num_args) {
    arg = va_arg(vargs, char *);

    if (num_args1 < CL_MAX_CONVERT_ARGS)
      cargs[num_args1++] = arg;
  }

  int *error_code = va_arg(vargs, int *);

  va_end(vargs);

  /*-----------*/

  *error_code = 0;

  /*-----------*/

  ClLanguageConverter *converter =
    ClLanguageConverterMgrInst->getConverter(from_type, "cl_string_array");

  if (! converter) {
    ClLanguageMgrInst->error("No converter for '%s' to str array", from_type);
    *error_code = -1;
    return;
  }

  /*-----------*/

  char **array;
  uint  *dims;
  uint   num_dims;

  int flag =
    (*converter->proc)(&array, &dims, &num_dims,
                       cargs[ 0], cargs[ 1], cargs[ 2], cargs[ 3], cargs[ 4],
                       cargs[ 5], cargs[ 6], cargs[ 7], cargs[ 8], cargs[ 9],
                       cargs[10], cargs[11], cargs[12], cargs[13], cargs[14],
                       cargs[15], cargs[16], cargs[17], cargs[18], cargs[19]);

  if (! flag) {
    *error_code = -1;
    return;
  }

  args->setStringArrayArg(num, array, dims, num_dims, error_code);
}

/*------------------------------------------------------------------*
 *
 * getTypedStringArrayArg
 *   Convert a CL String Array to an application
 *   defined type using a conversion routine.
 *
 * CALL:
 *   getTypedStringArrayArg(int num, char *to_type, int num_args,
 *                          char *arg, ..., int &error_code);
 *
 * INPUT:
 *   num        : The number of the argument to be retrieved
 *
 *   to_type    : The name of the application defined type which is to be converted to.
 *              : A converter for "cl_string_array" to this type must have been added
 *              : using addConverter().
 *
 *   num_args   : The number of arguments which are to be passed to the conversion routine.
 *
 *   arg, ...   : The arguments to be passed to the conversion routine (there must be
 *              : exactly the same number as the specified number of arguments 'num_args'.
 *
 * OUTPUT:
 *   error_code : Error Return Code :-
 *              :  0 - Success
 *              :  1 - Failure
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   The conversion routine will be called as follows :-
 *
 *     int flag = (*conversion_proc)
 *                  (char **strings, int *dims, int num_dims, char *arg, ...);
 *
 *     ON ENTRY
 *       arg, ... : The arguments specified as those to be passed to the conversion routine.
 *
 *       strings  : Array of strings to convert from.
 *
 *       dims     : Dimensions of str arrays.
 *
 *       num_dims : Number of dimensions of str arrays.
 *
 *     ON RETURN
 *       arg, ... : The arguments specified as those to
 *                : be passed to the conversion routine.
 *
 *     RETURNS
 *       flag     : Whether the conversion was successful or not.
 *
 *------------------------------------------------------------------*/

void
ClLanguageConverterMgr::
getTypedStringArrayArg(ClLanguageArgs *args, int num, const char *to_type, int num_args,
                       char *arg, ...)
{
  va_list  vargs;
  char    *cargs[CL_MAX_CONVERT_ARGS];

  /*CONSTANTCONDITION*/
  va_start(vargs, arg);

  int num_args1 = 0;

  cargs[num_args1++] = arg;

  while (num_args1 != num_args) {
    arg = va_arg(vargs, char *);

    if (num_args1 < CL_MAX_CONVERT_ARGS)
      cargs[num_args1++] = arg;
  }

  int *error_code = va_arg(vargs, int *);

  va_end(vargs);

  /*-----------*/

  *error_code = 0;

  /*-----------*/

  ClLanguageConverter *converter =
    ClLanguageConverterMgrInst->getConverter("cl_string_array", to_type);

  if (! converter) {
    ClLanguageMgrInst->error("No converter for str array to '%s'", to_type);
    *error_code = -1;
    return;
  }

  /*-----------*/

  uint  *dims;
  char **array;
  uint   num_dims;

  args->getStringArrayArg(num, &array, &dims, &num_dims, error_code);

  if (*error_code != 0) return;

  int flag =
    (*converter->proc)(array, dims, num_dims,
                       cargs[ 0], cargs[ 1], cargs[ 2], cargs[ 3], cargs[ 4],
                       cargs[ 5], cargs[ 6], cargs[ 7], cargs[ 8], cargs[ 9],
                       cargs[10], cargs[11], cargs[12], cargs[13], cargs[14],
                       cargs[15], cargs[16], cargs[17], cargs[18], cargs[19]);

  if (! flag) {
    *error_code = -1;
    return;
  }
}

/*------------------------------------------------------------------*
 *
 * convertStringMatrixToStringArray
 *   Routine to convert from a String Matrix (an array of
 *   null terminated fixed length strings stored in a
 *   single 'char *' array) to an array of strings.
 *
 * CALL:
 *   flag = convertStringMatrixToStringArray
 *           (&array, &dims, &num_dims, matrix, num_matrix, matrix_size);
 *
 * INPUT:
 *     matrix      : String Matrix
 *      char *     :
 *
 *     num_matrix  : Number of Strings in Matrix.
 *      char *     :
 *
 *     matrix_size : Fixed Length of Strings in
 *      int        : Matrix.
 *
 * OUTPUT:
 *     array       : Converted Array Strings.
 *      char **    :
 *
 *     dims        : Array of Dimensions
 *      int *      :
 *
 *     num_dims    : Number of Dimensions
 *      int        :
 *
 * RETURNS:
 *     flag        : Whether the conversion was
 *      int        : successful.
 *
 * NOTES:
 *   The strange calling sequence is so the routine
 *   can be added as a type converter (see addConverter()).
 *
 *   If successful 1 dimension will be always be
 *   returned with 'dims[0]' equal to 'num_matrix'.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageConverterMgr::
convertStringMatrixToStringArray(char ***array, uint **dims, uint *num_dims, char *matrix,
                                 int num_matrix, int matrix_size)
{
  *num_dims = 1;

  *dims = new uint [1];

  (*dims)[0] = uint(num_matrix);

  if (num_matrix > 0) {
    *array = new char * [uint(num_matrix)];

    for (uint i = 0; i < uint(num_matrix); i++)
      (*array)[i] = strdup(&matrix[i*uint(matrix_size)]);
  }
  else
    *array = nullptr;

  return true;
}

/*------------------------------------------------------------------*
 *
 * convertStringArrayToStringMatrix
 *   Routine to convert from an array of strings to
 *   a String Matrix (an array of null terminated fixed
 *   length strings stored in a single 'char *' array).
 *
 * CALL:
 *   flag = convertStringArrayToStringMatrix
 *           (array, dims, num_dims, &matrix, &num_matrix, matrix_size);
 *
 * INPUT:
 *     array       : Array of Strings.
 *      char **    :
 *
 *     dims        : Array of Dimensions
 *      int *      :
 *
 *     num_dims     : Number of Dimensions
 *      int        :
 *
 *     matrix_size : Fixed Length of Strings in
 *      int        : Matrix.
 *
 * OUTPUT:
 *     matrix      : Converted String Matrix.
 *      char *     :
 *
 *     num_matrix   : Number of Strings in Matrix.
 *      char *     :
 *
 * RETURNS:
 *     flag        : Whether the conversion was
 *      int        : successful.
 *
 * NOTES:
 *   The strange calling sequence is so the routine
 *   can be added as a type converter (see addConverter()).
 *
 *   The input array of strings must be single or
 *   zero dimensioned.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageConverterMgr::
convertStringArrayToStringMatrix(char **array, uint *dims, uint num_dims, char **matrix,
                                 int *num_matrix, int matrix_size)
{
  if (num_dims == 0) {
    *num_matrix = 0;
    *matrix     = nullptr;

    return true;
  }

  if (num_dims != 1) {
    ClLanguageMgrInst->syntaxError("Invalid String Array");
    return false;
  }

  *num_matrix = int(dims[0]);

  if (*num_matrix > 0) {
    *matrix = new char [uint(*num_matrix)*uint(matrix_size)];

    for (uint i = 0; i < uint(*num_matrix); i++) {
      uint len = uint(strlen(array[i]));

      if (len >= uint(matrix_size))
        len = uint(matrix_size - 1);

      strncpy(&(*matrix)[i*uint(matrix_size)], array[i], len);

      for (uint j = len; j < uint(matrix_size); j++)
        (&(*matrix)[i*uint(matrix_size)])[j] = '\0';
    }
  }
  else
    *matrix = nullptr;

  return true;
}

/*------------------------------------------------------------------*
 *
 * convertCharMatrixToStringArray
 *   Routine to convert from a Character Matrix (an array of
 *   non-null terminated fixed length strings stored in a
 *   single 'char *' array) to an array of strings.
 *
 * CALL:
 *   flag = convertCharMatrixToStringArray
 *           (&array, &dims, &num_dims, matrix, num_matrix, matrix_size);
 *
 * INPUT:
 *     matrix      : Character Matrix
 *      char *     :
 *
 *     num_matrix   : Number of Strings in Matrix.
 *      char *     :
 *
 *     matrix_size : Fixed Length of Strings in
 *      int        : Matrix.
 *
 * OUTPUT:
 *     array       : Converted Array Strings.
 *      char **    :
 *
 *     dims        : Array of Dimensions
 *      int *      :
 *
 *     num_dims     : Number of Dimensions
 *      int        :
 *
 * RETURNS:
 *     flag        : Whether the conversion was
 *      int        : successful.
 *
 * NOTES:
 *   The strange calling sequence is so the routine
 *   can be added as a type converter (see addConverter()).
 *
 *   If successful 1 dimension will be always be
 *   returned with 'dims[0]' equal to 'num_matrix'.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageConverterMgr::
convertCharMatrixToStringArray(char ***array, uint **dims, uint *num_dims, char *matrix,
                               int num_matrix, int matrix_size)
{
  *num_dims = 1;

  *dims = new uint [1];

  (*dims)[0] = uint(num_matrix);

  if (num_matrix > 0) {
    *array = new char * [uint(num_matrix)];

    for (uint i = 0; i < uint(num_matrix); i++) {
      (*array)[i] = new char [uint(matrix_size + 1)];

      strncpy((*array)[i], &matrix[i*uint(matrix_size)], uint(matrix_size));
      (*array)[i][uint(matrix_size)] = '\0';
    }
  }
  else
    *array = nullptr;

  return true;
}

/*------------------------------------------------------------------*
 *
 * convertStringArrayToCharMatrix
 *   Routine to convert from an array of strings to a
 *   Character Matrix (an array of non-null terminated fixed
 *   length strings stored in a single 'char *' array).
 *
 * CALL:
 *   flag = convertStringArrayToCharMatrix
 *           (array, dims, num_dims, &matrix, &num_matrix, matrix_size);
 *
 * INPUT:
 *     array       : Array of Strings.
 *      char **    :
 *
 *     dims        : Array of Dimensions
 *      int *      :
 *
 *     num_dims     : Number of Dimensions
 *      int        :
 *
 *     matrix_size : Fixed Length of Strings in
 *      int        : Matrix.
 *
 * OUTPUT:
 *     matrix      : Converted Character Matrix.
 *      char *     :
 *
 *     num_matrix   : Number of Strings in Matrix.
 *      char *     :
 *
 * RETURNS:
 *     flag        : Whether the conversion was
 *      int        : successful.
 *
 * NOTES:
 *   The strange calling sequence is so the routine
 *   can be added as a type converter (see addConverter()).
 *
 *   The input array of strings must be single or
 *   zero dimensioned.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageConverterMgr::
convertStringArrayToCharMatrix(char **array, uint *dims, uint num_dims, char **matrix,
                               int *num_matrix, int matrix_size)
{
  if (num_dims == 0) {
    *num_matrix = 0;
    *matrix     = nullptr;

    return true;
  }

  if (num_dims != 1) {
    ClLanguageMgrInst->syntaxError("Invalid String Array");
    return false;
  }

  *num_matrix = int(dims[0]);

  if (*num_matrix > 0) {
    *matrix = new char [uint(*num_matrix)*uint(matrix_size)];

    for (uint i = 0; i < uint(*num_matrix); i++) {
      uint len = uint(strlen(array[i]));

      if (len > uint(matrix_size))
        len = uint(matrix_size);

      strncpy(&(*matrix)[i*uint(matrix_size)], array[i], len);

      for (uint j = len; j < uint(matrix_size); j++)
        (*matrix)[i*uint(matrix_size) + j] = ' ';
    }
  }
  else
    *matrix = nullptr;

  return true;
}

//-----------------

/*------------------------------------------------------------------*
 *
 * addConverter
 *   Add a conversion routine to convert between two
 *   named types.
 *
 * CALL:
 *   addConverter(const std::string &from_type, const std::string *to_type,
 *                ClLanguageConvertProc proc);
 *
 * INPUT:
 *   from_type : Type to convert from.
 *
 *   to_type   : Type to convert to.
 *
 *   proc      : Conversion routine.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   One of 'from_type' or 'to_type' should be a CL
 *   type and the other the application defined type.
 *
 *   If the 'from_type' is a CL type see the associated
 *   ClLanguageGetTyped...Arg() routine for details
 *   of the conversion process.
 *
 *   If the 'to_type' is a CL type see the associated
 *   ClLanguageSetTyped...Arg() routine for details
 *   of the conversion process.
 *
 *   CL types supported and their associated routines :-
 *
 *   CL Type     : "cl_string_array"
 *   Get Routine : ClLanguageGetTypedStringArrayArg
 *   Set Routine : setTypedStringArrayArg
 *
 *------------------------------------------------------------------*/

void
ClLanguageConverterMgr::
addConverter(const std::string &from_type, const std::string &to_type, ClLanguageConvertProc proc)
{
  ClLanguageConverter *converter = new ClLanguageConverter;

  converter->from_type = from_type;
  converter->to_type   = to_type;
  converter->proc      = proc;

  converter_list_.push_back(converter);
}

/*------------------------------------------------------------------*
 *
 * getConverter
 *   Get the conversion routine to convert from one named
 *   type to another.
 *
 * CALL:
 *   ClLanguageConverter *proc =
 *     getConverter(const std::string &from_type, const std::string &to_type);
 *
 * INPUT:
 *   from_type : Type to convert from.
 *
 *   to_type   : Type to convert to.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   proc      : Conversion routine (null if none).
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

ClLanguageConverter *
ClLanguageConverterMgr::
getConverter(const std::string &from_type, const std::string &to_type) const
{
  uint num = uint(converter_list_.size());

  for (uint i = 0; i < num; ++i) {
    ClLanguageConverter *converter = converter_list_[i];

    if (converter->from_type == from_type &&
        converter->to_type   == to_type  )
      return converter;
  }

  return nullptr;
}
