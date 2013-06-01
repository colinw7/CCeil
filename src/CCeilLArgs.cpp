#include <CCeilL.h>

using std::string;

// Initialise Environment for Processing the Arguments
// supplied for an application defined command added
// using ClLanguageDefineCommand().
//
// This routine is automatically called before the
// application defined command's routine is called.
//
// Note: After this routines has been called the other argument
// processing calls can be used.
//
// The routine termArgs() should be called before exiting the routine.
//
// This routine is now called automatically before calling the
// application's command processing routine and termArgs() is called
// after the command processing routine terminates.

void
ClLanguageArgs::
startArgs(ClLanguageCommand *command)
{
  startArgs();

  setArgs(command);
}

void
ClLanguageArgs::
startArgs()
{
  if (arg_list_ != NULL)
    arg_list_stack_.push_back(arg_list_);

  arg_list_ = new ClLanguageArgList;
}

/*------------------------------------------------------------------*
 *
 * termArgs
 *   Terminate Environment for Processing the Arguments
 *   initialised by startArgs().
 *
 *   This routine frees off any resources allocated by
 *   the called Argument Processing routines.
 *
 *   This routine is automatically called after the
 *   application defined command's routine has been called.
 *
 * CALL:
 *   termArgs();
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *   startArgs() is now called automatically before calling the
 *   application's command processing routine and the routine is
 *   called after the command processing routine terminates.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
termArgs()
{
  delete arg_list_;

  if (! arg_list_stack_.empty()) {
    arg_list_ = arg_list_stack_.back();

    arg_list_stack_.pop_back();
  }
  else
    arg_list_ = NULL;

  if (string_stack_ != NULL) {
    int num_strings = string_stack_->size();

    while (num_strings--) {
      char *str = (*string_stack_)[num_strings];

      string_stack_->pop_back();

      delete [] str;
    }
  }

  if (chars_stack_ != NULL) {
    int num_chars = chars_stack_->size();

    while (num_chars--) {
      char **chars = (*chars_stack_)[num_chars];

      chars_stack_->pop_back();

      delete [] *chars;
    }
  }

  if (real_array_stack_ != NULL) {
    int num_real_arrays = real_array_stack_->size();

    while (num_real_arrays--) {
      void **real_array = (*real_array_stack_)[num_real_arrays];

      real_array_stack_->pop_back();

      delete [] ((double *) real_array[0]);
      delete [] ((uint *  ) real_array[1]);
    }
  }

  if (float_array_stack_ != NULL) {
    int num_float_arrays = float_array_stack_->size();

    while (num_float_arrays--) {
      void **float_array = (*float_array_stack_)[num_float_arrays];

      float_array_stack_->pop_back();

      delete [] ((float *) float_array[0]);
      delete [] ((uint * ) float_array[1]);
    }
  }

  if (integer_array_stack_ != NULL) {
    int num_integer_arrays = integer_array_stack_->size();

    while (num_integer_arrays--) {
      void **integer_array =
        (*integer_array_stack_)[num_integer_arrays];

      integer_array_stack_->pop_back();

      delete [] ((long *) integer_array[0]);
      delete [] ((uint *) integer_array[1]);
    }
  }

  if (word_array_stack_ != NULL) {
    int num_word_arrays = word_array_stack_->size();

    while (num_word_arrays--) {
      void **word_array = (*word_array_stack_)[num_word_arrays];

      word_array_stack_->pop_back();

      delete [] ((int * ) word_array[0]);
      delete [] ((uint *) word_array[1]);
    }
  }

  if (string_array_stack_ != NULL) {
    int num_string_arrays = string_array_stack_->size();

    while (num_string_arrays--) {
      void **string_array = (*string_array_stack_)[num_string_arrays];

      string_array_stack_->pop_back();

      delete [] ((void **) string_array[0]);
      delete [] ((uint * ) string_array[1]);
    }
  }
}

/*------------------------------------------------------------------*
 *
 * checkNumberOfArgs
 *   Check whether the number of arguments supplied with
 *   an application defined command are valid.
 *
 * CALL:
 *   int error = checkNumberOfArgs(int min, int max);
 *
 * INPUT:
 *   min : The minimum number of arguments allowed
 *
 *   max : The maximum number of arguments allowed
 *
 * RETURNS:
 *   error : Error Return Code :-
 *         :  0 - Number of Arguments OK
 *         :  1 - Too few Arguments
 *         :  2 - Too many Arguments
 *
 * NOTES:
 *   If 'max' is less than 'min' then checking of the maximum number of
 *   arguments is not performed. It is recommended than -1 is used for 'max'
 *   if no maximum checking is required.
 *
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

int
ClLanguageArgs::
checkNumberOfArgs(int min, int max)
{
  int error_code = 0;

  uint num_args = getNumArgs();

  const string &command_name = ClLanguageMgrInst->getCommandName();

  if      (min >= 0 && (int) num_args < min) {
    if      (min > max)
      ClLanguageMgrInst->syntaxError
       ("insufficient arguments for '%s' (requires %d or greater)", command_name.c_str(), min);
    else if (min == max)
      ClLanguageMgrInst->syntaxError
       ("insufficient arguments for '%s' (requires %d)", command_name.c_str(), min);
    else
      ClLanguageMgrInst->syntaxError
       ("insufficient arguments for '%s' (requires %d to %d)", command_name.c_str(), min, max);

    error_code = 1;
  }
  else if (max >= min && (int) num_args > max) {
    if (max == 0)
      ClLanguageMgrInst->syntaxError
       ("no arguments allowed for '%s'", command_name.c_str());
    else {
      if (min == max)
        ClLanguageMgrInst->syntaxError
         ("too many arguments for '%s' (requires %d)", command_name.c_str(), min);
      else
        ClLanguageMgrInst->syntaxError
         ("too many arguments for '%s' (requires %d to %d)", command_name.c_str(), min, max);
    }

    error_code = 2;
  }

  return error_code;
}

// Get the number of arguments supplied with an
// application defined command.
//
// Note: startArgs() must been called before using this routine.

uint
ClLanguageArgs::
getNumArgs() const
{
  if (arg_list_)
    return arg_list_->size();
  else
    return 0;
}

/*------------------------------------------------------------------*
 *
 * getArgList
 *   Get a List of Arguments for the Current Command.
 *
 * CALL:
 *   int num =
 *     getArgList(CLArgType type1, <type1_data>,
 *                CLArgType type2, <type2_data>,
 *                CL_ARG_TYPE_NONE);
 *
 * INPUT:
 *   type  : Argument Type :-
 *         :   CL_ARG_TYPE_SKIP -
 *         :     Skip this argument, no next argument should be supplied.
 *         :   CL_ARG_TYPE_SKIP_N -
 *         :     Skip a number of arguments, the next argument is the number
 *         :     of arguments to skip (supplied as an integer).
 *         :   CL_ARG_TYPE_REAL -
 *         :     Get the next argument as a real number, pointer to a double
 *         :     should be the next argument.
 *         :   CL_ARG_TYPE_INTEGER
 *         :     Get the next argument as an integer number, pointer to a long
 *         :     should be the next argument.
 *         :   CL_ARG_TYPE_WORD
 *         :     Get the next argument as an integer number, pointer to a int
 *         :     should be the next argument.
 *         :   CL_ARG_TYPE_STRING
 *         :     Get the next argument as a string, pointer to a char * should
 *         :     be the next argument.
 *         :   CL_ARG_TYPE_CHARS
 *         :     Get the next argument as a character array, pointer to a char *
 *         :     and an int should be the next two arguments.
 *         :   CL_ARG_TYPE_TEXT
 *         :     Get the next argument as a literal text, pointer to a char *
 *         :     should be the next argument.
 *         :   CL_ARG_TYPE_VALUE
 *         :     Get the next argument as a parser value, pointer to a
 *         :     ClParserValuePtr * should be the next argument.
 *         :   CL_ARG_TYPE_REALS
 *         :     Get the next argument as a single dimension real array, pointer
 *         :     to a double * and int, should be the next two arguments.
 *         :   CL_ARG_TYPE_FLOATS
 *         :     Get the next argument as a single dimension real array, pointer
 *         :     to a float * and int, should be the next two arguments.
 *         :   CL_ARG_TYPE_INTEGERS
 *         :     Get the next argument as a single dimension integer array, pointer
 *         :     to an long * and int, should be the next two arguments.
 *         :   CL_ARG_TYPE_WORDS
 *         :     Get the next argument as a single dimension integer array, pointer
 *         :     to an int * and int, should be the next two arguments.
 *         :   CL_ARG_TYPE_STRINGS
 *         :     Get the next argument as a single dimension string array, pointer
 *         :     to a char ** and int, should be the next two arguments.
 *         :   CL_ARG_TYPE_REALARR
 *         :     Get the next argument as a real array, pointer to a double *,
 *         :     int * and int, should be the next three arguments.
 *         :   CL_ARG_TYPE_FLTARR
 *         :     Get the next argument as a real array, pointer to a float *,
 *         :     int * and int, should be the next three arguments.
 *         :   CL_ARG_TYPE_INTARR
 *         :     Get the next argument as an integer array, pointer to a long *,
 *         :     int * and int, should be the next three arguments.
 *         :   CL_ARG_TYPE_WORDARR
 *         :     Get the next argument as an integer array, pointer to an int *,
 *         :     int * and int, should be the next three arguments.
 *         :   CL_ARG_TYPE_STRARR
 *         :     Get the next argument as a string array, pointer to a char **,
 *         :     int * and int, should be the next three arguments.
 *         :   CL_ARG_TYPE_STRMAT
 *         :     Get the next argument as a string matrix (an array of null terminated
 *         :     fixed length strings stored in a single char * array), pointer to a
 *         :     char * (string matrix), pointer to an int (number of strings) and an
 *         :     int (fixed string length), should be the next three arguments.
 *         :   CL_ARG_TYPE_CHRMAT
 *         :     Get the next argument as a character matrix (an array of non-null terminated
 *         :     fixed length strings stored in a single char * array), pointer to a char *
 *         :     (character matrix), pointer to an int (number of strings) and an int (fixed
 *         :     string length), should be the next three arguments.
 *         :   CL_ARG_TYPE_TYPED
 *         :     Get the next argument as a type which is constructed by an installed
 *         :     converter. The arguments are the type to convert from (one of the
 *         :     CL_ARG_TYPE_???? types), the type to convert to (specified when the
 *         :     converter was installed), the number of input arguments to be supplied
 *         :     to the converter followed by pointers to the output arguments.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   num   : Number of arguments processed by the routine (this includes skipped arguments).
 *
 * NOTES:
 *   The arguments required after the type argument are dependant on the value of the
 *   type argument. The list is terminated by a type of CL_ARG_TYPE_NONE.
 *
 *   startArgs() must been called before using this routine.
 *
 *   e.g.
 *
 *   int                num;
 *   char              *str;
 *   double             real;
 *   char              *text;
 *   int                word;
 *   ClParserValuePtr   value;
 *   long               integer;
 *   int               *word_array;
 *   double            *real_array;
 *   float             *float_array;
 *   char             **string_array;
 *   long              *integer_array;
 *   int               *word_array_dims;
 *   int               *real_array_dims;
 *   int               *float_array_dims;
 *   int               *string_array_dims;
 *   int               *integer_array_dims;
 *   int                num_real_array_dims;
 *   int                num_word_array_dims;
 *   int                num_float_array_dims;
 *   int                num_string_array_dims;
 *   int                num_integer_array_dims;
 *
 *   num = getArgList
 *    (CL_ARG_TYPE_SKIP,
 *     CL_ARG_TYPE_SKIP_N , 3,
 *     CL_ARG_TYPE_REAL   , &real,
 *     CL_ARG_TYPE_INTEGER, &integer,
 *     CL_ARG_TYPE_WORD   , &word,
 *     CL_ARG_TYPE_STRING , &str,
 *     CL_ARG_TYPE_TEXT   , &text,
 *     CL_ARG_TYPE_VALUE  , &value,
 *     CL_ARG_TYPE_REALARR, &real_array, &real_array_dims, &num_real_array_dims,
 *     CL_ARG_TYPE_FLTARR , &float_array, &float_array_dims, &num_float_array_dims,
 *     CL_ARG_TYPE_INTARR , &integer_array, &integer_array_dims, &num_integer_array_dims,
 *     CL_ARG_TYPE_WORDARR, &word_array, &word_array_dims, &num_word_array_dims,
 *     CL_ARG_TYPE_STRARR , &string_array, &string_array_dims, &num_string_array_dims,
 *     CL_ARG_TYPE_NONE);
 *
 *   if (num != 15) error();
 *
 *------------------------------------------------------------------*/

int
ClLanguageArgs::
getArgList(CLArgType type, ...)
{
  va_list vargs;

  /*CONSTANTCONDITION*/
  va_start(vargs, type);

  int num = getVArgList(type, &vargs);

  va_end(vargs);

  return num;
}

/*------------------------------------------------------------------*
 *
 * getVArgList
 *   Get a List of Arguments for the Current Command.
 *
 * CALL:
 *   int num = getVArgList(CLArgType type, &vargs);
 *
 * INPUT:
 *   type  : First Argument Type
 *
 *   vargs : Variable arguments from va_start().
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   num   : Number of arguments processed by the
 *         : routine (this includes skipped arguments).
 *
 * NOTES:
 *   Used by getArgList() and getStringArgList() to
 *   process their variable argument lists.
 *
 *------------------------------------------------------------------*/

int
ClLanguageArgs::
getVArgList(CLArgType type, va_list *vargs)
{
  int num = 0;

  int error_code = 0;

  while (type != CL_ARG_TYPE_NONE) {
    if      (type == CL_ARG_TYPE_SKIP)
      ;
    else if (type == CL_ARG_TYPE_SKIP_N) {
      int integer = va_arg(*vargs, int);

      num += integer - 1;
    }
    else if (type == CL_ARG_TYPE_REAL) {
      double *real = va_arg(*vargs, double *);

      *real = getRealArg(num + 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_INTEGER) {
      long *integer = va_arg(*vargs, long *);

      *integer = getIntegerArg(num + 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_WORD) {
      int *integer = va_arg(*vargs, int *);

      *integer = getIntegerArg(num + 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_STRING) {
      char **str = va_arg(*vargs, char **);

      string str1 = getStringArg(num + 1, &error_code);

      *str = strdup(str1.c_str());
    }
    else if (type == CL_ARG_TYPE_CHARS) {
      char **chars     = va_arg(*vargs, char **);
      int   *num_chars = va_arg(*vargs, int *);

      getCharArrayArg(num + 1, chars, num_chars, &error_code);
    }
    else if (type == CL_ARG_TYPE_TEXT) {
      char **str = va_arg(*vargs, char **);

      string arg = getArg(num + 1, &error_code);

      *str = (char *) arg.c_str();
    }
    else if (type == CL_ARG_TYPE_VALUE) {
      ClParserValuePtr *value = va_arg(*vargs, ClParserValuePtr *);

      *value = getValueArg(num + 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_REALS) {
      double **reals     = va_arg(*vargs, double **);
      int     *num_reals = va_arg(*vargs, int *);

      uint num_reals1;

      getRealArrayArg(num + 1, reals, &num_reals1, &error_code);

      if (error_code != 0)
        break;

      *num_reals = num_reals1;
    }
    else if (type == CL_ARG_TYPE_FLOATS) {
      float **reals     = va_arg(*vargs, float **);
      int    *num_reals = va_arg(*vargs, int *);

      uint num_reals1;

      getRealArrayArg(num + 1, reals, &num_reals1, &error_code);

      if (error_code != 0)
        break;

      *num_reals = num_reals1;
    }
    else if (type == CL_ARG_TYPE_INTEGERS) {
      uint *dims;
      uint  num_dims;

      long **integers     = va_arg(*vargs, long **);
      int   *num_integers = va_arg(*vargs, int *);

      getIntegerArrayArg(num + 1, integers, &dims, &num_dims, &error_code);

      if (error_code != 0)
        break;

      if      (num_dims != 0 && num_dims != 1) {
        ClLanguageMgrInst->syntaxError("Invalid Array argument %d", num + 1);
        error_code = 1;
        *num_integers = 0;
      }
      else if (num_dims == 1)
        *num_integers = dims[0];
      else
        *num_integers = 0;
    }
    else if (type == CL_ARG_TYPE_WORDS) {
      uint *dims;
      uint  num_dims;

      int **integers     = va_arg(*vargs, int **);
      int  *num_integers = va_arg(*vargs, int *);

      getIntegerArrayArg(num + 1, integers, &dims, &num_dims, &error_code);

      if (error_code != 0)
        break;

      if      (num_dims != 0 && num_dims != 1) {
        ClLanguageMgrInst->syntaxError("Invalid Array argument %d", num + 1);
        error_code = 1;
        *num_integers = 0;
      }
      else if (num_dims == 1)
        *num_integers = dims[0];
      else
        *num_integers = 0;
    }
    else if (type == CL_ARG_TYPE_STRINGS) {
      uint *dims;
      uint  num_dims;

      char ***strings     = va_arg(*vargs, char ***);
      int    *num_strings = va_arg(*vargs, int *);

      getStringArrayArg(num + 1, strings, &dims, &num_dims, &error_code);

      if (error_code != 0)
        break;

      if      (num_dims != 0 && num_dims != 1) {
        ClLanguageMgrInst->syntaxError("Invalid Array argument %d", num + 1);
        error_code = 1;
        *num_strings = 0;
      }
      else if (num_dims == 1)
        *num_strings = dims[0];
      else
        *num_strings = 0;
    }
    else if (type == CL_ARG_TYPE_REALARR) {
      double **reals    = va_arg(*vargs, double **);
      uint   **dims     = va_arg(*vargs, uint **);
      uint    *num_dims = va_arg(*vargs, uint *);

      getRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_FLTARR) {
      float **reals    = va_arg(*vargs, float **);
      uint  **dims     = va_arg(*vargs, uint **);
      uint   *num_dims = va_arg(*vargs, uint *);

      getRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_INTARR) {
      long **integers  = va_arg(*vargs, long **);
      uint **dims      = va_arg(*vargs, uint **);
      uint  *num_dims  = va_arg(*vargs, uint *);

      getIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_WORDARR) {
      int  **integers  = va_arg(*vargs, int **);
      uint **dims      = va_arg(*vargs, uint **);
      uint  *num_dims  = va_arg(*vargs, uint *);

      getIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_STRARR) {
      char ***strings  = va_arg(*vargs, char ***);
      uint  **dims     = va_arg(*vargs, uint **);
      uint   *num_dims = va_arg(*vargs, uint *);

      getStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_STRMAT) {
      uint  *dims;
      char **strings;
      uint   num_dims;

      char **matrix      = va_arg(*vargs, char **);
      int   *num_matrix  = va_arg(*vargs, int *);
      int    matrix_size = va_arg(*vargs, int);

      getStringArrayArg(num + 1, &strings, &dims, &num_dims, &error_code);

      if (error_code != 0)
        break;

      int flag = ClLanguageConverterMgrInst->convertStringArrayToStringMatrix
                  (strings, dims, num_dims,
                   matrix, num_matrix, matrix_size);

      if (! flag)
        error_code = -1;
    }
    else if (type == CL_ARG_TYPE_CHRMAT) {
      uint  *dims;
      char **strings;
      uint   num_dims;

      char **matrix      = va_arg(*vargs, char **);
      int   *num_matrix  = va_arg(*vargs, int *);
      int    matrix_size = va_arg(*vargs, int);

      getStringArrayArg(num + 1, &strings, &dims, &num_dims, &error_code);

      if (error_code != 0)
        break;

      int flag = ClLanguageConverterMgrInst->convertStringArrayToCharMatrix
                  (strings, dims, num_dims,
                   matrix, num_matrix, matrix_size);

      if (! flag)
        error_code = -1;
    }
    else if (type == CL_ARG_TYPE_TYPED) {
      char *args[CL_MAX_CONVERT_ARGS];

      int   from_type = va_arg(*vargs, int);
      char *to_type   = va_arg(*vargs, char *);
      int   num_args  = va_arg(*vargs, int);

      for (int i = 0; i < num_args; i++) {
        char *arg = va_arg(*vargs, char *);

        if (i < CL_MAX_CONVERT_ARGS)
          args[i] = arg;
      }

      if (from_type == CL_ARG_TYPE_STRARR)
        ClLanguageConverterMgrInst->getTypedStringArrayArg(this, num + 1,
          to_type, CL_MAX_CONVERT_ARGS,
          args[ 0], args[ 1], args[ 2], args[ 3], args[ 4],
          args[ 5], args[ 6], args[ 7], args[ 8], args[ 9],
          args[10], args[11], args[12], args[13], args[14],
          args[15], args[16], args[17], args[18], args[19],
          &error_code);
      else
        ClLanguageMgrInst->syntaxError("conversion from type unsupported");
    }
    else
      break;

    if (error_code != 0)
      break;

    num++;

    type = (CLArgType) va_arg(*vargs, int);
  }

  return num;
}

/*------------------------------------------------------------------*
 *
 * getStringArgList
 *   Get a List of Argument Values for the Parsed Arguments
 *   from the supplied string.
 *
 * CALL:
 *   int num =
 *     getStringArgList(const string &str,
 *                      CLArgType type1, <type1_data>,
 *                      CLArgType type2, <type2_data>,
 *                      CL_ARG_TYPE_NONE);
 *
 * INPUT:
 *   str  : String from which the returned values are parsed.
 *
 *   type : Argument Type (See getArgList()).
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   num  : Number of arguments processed by the
 *        : routine (this includes skipped arguments).
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

int
ClLanguageArgs::
getStringArgList(const string &str, CLArgType type, ...)
{
  va_list vargs;

  setArgs(str);

  /*CONSTANTCONDITION*/
  va_start(vargs, type);

  int num = getVArgList(type, &vargs);

  va_end(vargs);

  return num;
}

// Get the Nth argument supplied by the user as a real number.
//
// Note: startArgs() must been called before using this routine.

double
ClLanguageArgs::
getRealArg(int n, int *error_code)
{
  *error_code = 0;

  double real = 0.0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return real;

  if (! value->realValue(&real)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);
    *error_code = 3;

    return real;
  }

  return real;
}

// Get the Nth argument supplied by the user as an integer number.
//
// Note: startArgs() must been called before using this routine.

long
ClLanguageArgs::
getIntegerArg(int n, int *error_code)
{
  *error_code = 0;

  long integer = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return integer;

  if (! value->integerValue(&integer)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *error_code = 3;

    return integer;
  }

  return integer;
}

// Get the Nth argument supplied by the user as a string.
//
// Note: startArgs() must been called before using this routine.

string
ClLanguageArgs::
getStringArg(int n, int *error_code)
{
  *error_code = 0;

  string str;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return str;

  if (! value->stringValue(str)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *error_code = 3;

    return str;
  }

  return str;
}

// Get the Nth argument supplied by the user as a character array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getCharArrayArg(int n, char **chars, int *num_chars, int *error_code)
{
  *chars      = NULL;
  *num_chars  = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->stringValue(chars, num_chars)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *chars      = NULL;
    *num_chars  = 0;
    *error_code = 3;

    return;
  }

  stackChars(chars);
}

// Get the Nth argument supplied by the user as a real array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getRealArrayArg(int n, double **reals, uint *num_reals, int *error_code)
{
  uint *dims;
  uint  num_dims;

  getRealArrayArg(n, reals, &dims, &num_dims, error_code);

  if (*error_code != 0)
    return;

  if      (num_dims != 0 && num_dims != 1) {
    ClLanguageMgrInst->syntaxError("Invalid Array argument %d", n + 1);
    *error_code = 1;
    *num_reals = 0;
  }
  else if (num_dims == 1)
    *num_reals = dims[0];
  else
    *num_reals = 0;
}

// Get the Nth argument supplied by the user as a real array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getRealArrayArg(int n, float **reals, uint *num_reals, int *error_code)
{
  uint *dims;
  uint  num_dims;

  getRealArrayArg(n, reals, &dims, &num_dims, error_code);

  if (*error_code != 0)
    return;

  if      (num_dims != 0 && num_dims != 1) {
    ClLanguageMgrInst->syntaxError("Invalid Array argument %d", n + 1);
    *error_code = 1;
    *num_reals = 0;
  }
  else if (num_dims == 1)
    *num_reals = dims[0];
  else
    *num_reals = 0;
}

// Get the Nth argument supplied by the user as a real array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getRealArrayArg(int n, double **reals, uint **dims, uint *num_dims, int *error_code)
{
  *reals      = NULL;
  *dims       = NULL;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->realArrayValue(reals, dims, num_dims)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);
    *error_code = 3;

    return;
  }

  stackRealArray(*reals, *dims);
}

// Get the Nth argument supplied by the user as a float array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getRealArrayArg(int n, float **reals, uint **dims, uint *num_dims, int *error_code)
{
  *reals      = NULL;
  *dims       = NULL;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->realArrayValue(reals, dims, num_dims)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *error_code = 3;

    return;
  }

  stackRealArray(*reals, *dims);
}

// Get the Nth argument supplied by the user as an integer array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getIntegerArrayArg(int n, long **integers, uint **dims, uint *num_dims, int *error_code)
{
  *integers   = NULL;
  *dims       = NULL;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->integerArrayValue(integers, dims, num_dims)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *error_code = 3;

    return;
  }

  stackIntegerArray(*integers, *dims);
}

// Get the Nth argument supplied by the user as an integer array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getIntegerArrayArg(int n, int **integers, uint **dims, uint *num_dims, int *error_code)
{
  *integers   = NULL;
  *dims       = NULL;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->integerArrayValue(integers, dims, num_dims)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *error_code = 3;

    return;
  }

  stackIntegerArray(*integers, *dims);
}

// Get the Nth argument supplied by the user as a string array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
getStringArrayArg(int n, char ***strings, uint **dims, uint *num_dims, int *error_code)
{
  *strings    = NULL;
  *dims       = NULL;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->stringArrayValue(strings, dims, num_dims)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (CLERR_INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *error_code = 3;

    return;
  }

  stackStringArray(*strings, *dims);
}

// Get the Nth argument supplied by the user as a value.
//
// Note: startArgs() must been called before using this routine.

ClParserValuePtr
ClLanguageArgs::
getValueArg(int n, int *error_code)
{
  ClParserValuePtr value;
  int              error_code1;

  *error_code = 0;

  string arg = getArg(n, &error_code1);

  if (error_code1 != 0) {
    *error_code = 1;

    return value;
  }

  ClParserExpr expr(arg);

  if (! expr.exec(value)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION, "'%s' argument %d - '%s'",
                                        command_name.c_str(), n, arg.c_str());

    *error_code = 2;

    return value;
  }

  if (! value.isValid()) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->syntaxError("undefined '%s' argument %d - '%s'",
                                    command_name.c_str(), n, arg.c_str());

    *error_code = 2;

    return value;
  }

  return value;
}


// Get the Nth argument supplied by the user as a variable argument.
//
// Note: startArgs() must been called before using this routine.

string
ClLanguageArgs::
getVariableArg(int n, int *error_code)
{
  int error_code1;

  *error_code = 0;

  string arg = getArg(n, &error_code1);

  if (error_code1 != 0) {
    *error_code = 1;

    return arg;
  }

  if (! ClParserInst->isValidAssignString(arg)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->syntaxError
      ("'%s' argument %d (invalid return argument '%s')", command_name.c_str(), n, arg.c_str());

    *error_code = 2;

    return arg;
  }

  return arg;
}

// Get the Nth argument supplied by the user as literal text.
//
// Note: startArgs() must been called before using this routine.

string
ClLanguageArgs::
getArg(int n, int *error_code)
{
  *error_code = 0;

  string arg;

  uint num_args = getNumArgs();

  if (n >= 1 && n <= (int) num_args)
    arg = getIArg(n - 1);
  else
    *error_code = 1;

  return arg;
}

const string &
ClLanguageArgs::
getIArg(int i) const
{
  return (*arg_list_)[i];
}

/*------------------------------------------------------------------*
 *
 * setArgList
 *   Set a List of Arguments for the Current Command.
 *
 * CALL:
 *   int num = setArgList(CLArgType type1, <type1_data>,
 *                        CLArgType type2, <type2_data>,
 *                        CL_ARG_TYPE_NONE);
 *
 * INPUT:
 *   type  : Argument Type :-
 *         :   CL_ARG_TYPE_SKIP -
 *         :     Skip this argument, num next argument
 *         :     should be supplied.
 *         :   CL_ARG_TYPE_SKIP_N -
 *         :     Skip a number of arguments, the next
 *         :     argument is the number of arguments to
 *         :     skip (supplied as an integer).
 *         :   CL_ARG_TYPE_REAL -
 *         :     Set the next argument to a real number,
 *         :     double should be the next argument.
 *         :   CL_ARG_TYPE_INTEGER
 *         :     Set the next argument to an integer number,
 *         :     long should be the next argument.
 *         :   CL_ARG_TYPE_WORD
 *         :     Set the next argument to an integer number,
 *         :     int should be the next argument.
 *         :   CL_ARG_TYPE_STRING
 *         :     Set the next argument to a string,
 *         :     char * should be the next argument.
 *         :   CL_ARG_TYPE_CHARS
 *         :     Set the next argument to a character
 *         :     array, char * and int should be the next
 *         :     two arguments.
 *         :   CL_ARG_TYPE_VALUE
 *         :     Set the next argument to a parser value,
 *         :     pointer to a ClParserValuePtr should be
 *         :     the next argument.
 *         :   CL_ARG_TYPE_EXPR
 *         :     Set the next argument to the result of
 *         :     evaluating an expression, char * should
 *         :     be the next argument.
 *         :   CL_ARG_TYPE_REALS
 *         :     Set the next argument to a single
 *         :     dimension real array, pointer to a
 *         :     double * and int, should be the next
 *         :     two arguments.
 *         :   CL_ARG_TYPE_FLOATS
 *         :     Set the next argument to a single
 *         :     dimension real array, pointer to a
 *         :     float * and int, should be the next
 *         :     two arguments.
 *         :   CL_ARG_TYPE_INTEGERS
 *         :     Set the next argument to a single
 *         :     dimension integer array, pointer to an
 *         :     long * and int, should be the next
 *         :     two arguments.
 *         :   CL_ARG_TYPE_WORDS
 *         :     Set the next argument to a single
 *         :     dimension integer array, pointer to an
 *         :     int * and int, should be the next
 *         :     two arguments.
 *         :   CL_ARG_TYPE_STRINGS
 *         :     Set the next argument to a single
 *         :     dimension string array, pointer to a
 *         :     char ** and int, should be the next
 *         :     two arguments.
 *         :   CL_ARG_TYPE_REALARR
 *         :     Set the next argument to a real array,
 *         :     a double *, int * and int should be the
 *         :     next three arguments.
 *         :   CL_ARG_TYPE_FLTARR
 *         :     Set the next argument to a real array,
 *         :     a float *, int * and int should be the
 *         :     next three arguments.
 *         :   CL_ARG_TYPE_INTARR
 *         :     Set the next argument to an integer array,
 *         :     a long *, int * and int should be the
 *         :     next three arguments.
 *         :   CL_ARG_TYPE_WORDARR
 *         :     Set the next argument to an integer array,
 *         :     an int *, int * and int should be the
 *         :     next three arguments.
 *         :   CL_ARG_TYPE_STRARR
 *         :     Set the next argument to a string array,
 *         :     a char **, int * and int should be the
 *         :     next three arguments.
 *         :   CL_ARG_TYPE_STRMAT
 *         :     Set the next argument to a string matrix
 *         :     (an array of null terminated fixed length
 *         :     strings stored in a single char * array),
 *         :     a char * (string matrix), an int (number of
 *         :     strings) and an int (fixed string length),
 *         :     should be the next three arguments.
 *         :   CL_ARG_TYPE_CHRMAT
 *         :     Set the next argument to a character
 *         :     matrix (an array of non-null terminated
 *         :     fixed length strings stored in a single
 *         ;     char * array), a char * (character matrix),
 *         :     an int (number of strings) and an int (fixed
 *         :     string length), should be the next three
 *         :     arguments.
 *         :   CL_ARG_TYPE_TYPED
 *         :     Set the next argument to a type which
 *         :     is constructed by an installed converter.
 *         :     The arguments are the type to convert
 *         :     from (specified when the converter was
 *         :     installed), the type to convert to (one of
 *         :     the CL_ARG_TYPE_???? types), the number of
 *         :     input arguments to be supplied to the
 *         :     converter followed by the input arguments.
 *
 * OUTPUT:
 *     None
 *
 * RETURNS:
 *     num : Number of arguments processed by the
 *         : routine (this includes skipped arguments).
 *
 * NOTES:
 *   The arguments required after the type argument are
 *   dependant on the value of the type argument. The
 *   list is terminated by a type of CL_ARG_TYPE_NONE.
 *
 *   startArgs() must been called before using this routine.
 *
 *   e.g.
 *
 *   int                word;
 *   double             real;
 *   char              *expr;
 *   ClParserValuePtr  *value;
 *   char              *str;
 *   long               integer;
 *   double            *real_array;
 *   int               *word_array;
 *   int               *word_array;
 *   float             *float_array;
 *   char             **string_array;
 *   long              *integer_array;
 *   int               *real_array_dims;
 *   int               *word_array_dims;
 *   int               *float_array_dims;
 *   int               *string_array_dims;
 *   int               *integer_array_dims;
 *   int                num_real_array_dims;
 *   int                num_word_array_dims;
 *   int                num_float_array_dims;
 *   int                num_string_array_dims;
 *   int                num_integer_array_dims;
 *
 *   int num = setArgList
 *    (CL_ARG_TYPE_SKIP,
 *     CL_ARG_TYPE_SKIP_N , 3,
 *     CL_ARG_TYPE_REAL   , real,
 *     CL_ARG_TYPE_INTEGER, integer,
 *     CL_ARG_TYPE_WORD   , word,
 *     CL_ARG_TYPE_STRING , str,
 *     CL_ARG_TYPE_VALUE  , value,
 *     CL_ARG_TYPE_EXPR   , expr,
 *     CL_ARG_TYPE_REALARR, real_array, real_array_dims, num_real_array_dims,
 *     CL_ARG_TYPE_FLTARR , float_array, float_array_dims, num_float_array_dims,
 *     CL_ARG_TYPE_INTARR , integer_array, integer_array_dims, num_integer_array_dims,
 *     CL_ARG_TYPE_WORDARR, word_array, word_array_dims, num_word_array_dims,
 *     CL_ARG_TYPE_STRARR , string_array, string_array_dims, num_string_array_dims,
 *     CL_ARG_TYPE_NONE);
 *
 *   if (num != 15) error();
 *
 *------------------------------------------------------------------*/

int
ClLanguageArgs::
setArgList(CLArgType type, ...)
{
  va_list vargs;

  /*CONSTANTCONDITION*/
  va_start(vargs, type);

  int num = setVArgList(type, &vargs);

  va_end(vargs);

  return num;
}

int
ClLanguageArgs::
setVArgList(CLArgType type, va_list *vargs)
{
  int num = 0;

  int error_code = 0;

  while (type != CL_ARG_TYPE_NONE) {
    if      (type == CL_ARG_TYPE_SKIP)
      ;
    else if (type == CL_ARG_TYPE_SKIP_N) {
      int integer = va_arg(*vargs, int);

      num += integer - 1;
    }
    else if (type == CL_ARG_TYPE_REAL) {
      double real = va_arg(*vargs, double);

      setRealArg(num + 1, real, &error_code);
    }
    else if (type == CL_ARG_TYPE_INTEGER) {
      long integer = va_arg(*vargs, long);

      setIntegerArg(num + 1, integer, &error_code);
    }
    else if (type == CL_ARG_TYPE_WORD) {
      int integer = va_arg(*vargs, int);

      setIntegerArg(num + 1, integer, &error_code);
    }
    else if (type == CL_ARG_TYPE_STRING) {
      char *str = va_arg(*vargs, char *);

      setStringArg(num + 1, str, &error_code);
    }
    else if (type == CL_ARG_TYPE_CHARS) {
      char *chars     = va_arg(*vargs, char *);
      int   num_chars = va_arg(*vargs, int);

      setCharArrayArg(num + 1, chars, num_chars, &error_code);
    }
    else if (type == CL_ARG_TYPE_VALUE) {
      ClParserValuePtr *value = va_arg(*vargs, ClParserValuePtr *);

      setValueArg(num + 1, *value, &error_code);
    }
    else if (type == CL_ARG_TYPE_EXPR) {
      char *expr = va_arg(*vargs, char *);

      setExpressionArg(num + 1, expr, &error_code);
    }
    else if (type == CL_ARG_TYPE_REALS) {
      uint dims[1];

      double *reals     = va_arg(*vargs, double *);
      int     num_reals = va_arg(*vargs, int);

      dims[0] = num_reals;

      setRealArrayArg(num + 1, reals, dims, 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_FLOATS) {
      uint dims[1];

      float *reals     = va_arg(*vargs, float *);
      int    num_reals = va_arg(*vargs, int);

      dims[0] = num_reals;

      setRealArrayArg(num + 1, reals, dims, 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_INTEGERS) {
      uint dims[1];

      long *integers     = va_arg(*vargs, long *);
      int   num_integers = va_arg(*vargs, int);

      dims[0] = num_integers;

      setIntegerArrayArg(num + 1, integers, dims, 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_WORDS) {
      uint dims[1];

      int *integers     = va_arg(*vargs, int *);
      int  num_integers = va_arg(*vargs, int);

      dims[0] = num_integers;

      setIntegerArrayArg(num + 1, integers, dims, 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_STRINGS) {
      uint dims[1];

      char **strings     = va_arg(*vargs, char **);
      int    num_strings = va_arg(*vargs, int);

      dims[0] = num_strings;

      setStringArrayArg(num + 1, strings, dims, 1, &error_code);
    }
    else if (type == CL_ARG_TYPE_REALARR) {
      double *reals    = va_arg(*vargs, double *);
      uint   *dims     = va_arg(*vargs, uint *);
      uint    num_dims = va_arg(*vargs, uint);

      setRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_FLTARR) {
      float *reals    = va_arg(*vargs, float *);
      uint  *dims     = va_arg(*vargs, uint *);
      uint   num_dims = va_arg(*vargs, uint);

      setRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_INTARR) {
      long *integers = va_arg(*vargs, long *);
      uint *dims     = va_arg(*vargs, uint *);
      uint  num_dims = va_arg(*vargs, uint);

      setIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_WORDARR) {
      int  *integers = va_arg(*vargs, int *);
      uint *dims     = va_arg(*vargs, uint *);
      uint  num_dims = va_arg(*vargs, uint);

      setIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_STRARR) {
      char **strings  = va_arg(*vargs, char **);
      uint  *dims     = va_arg(*vargs, uint *);
      uint   num_dims = va_arg(*vargs, uint);

      setStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_STRMAT) {
      uint  *dims;
      char **strings;
      uint   num_dims;

      char *matrix      = va_arg(*vargs, char *);
      int   num_matrix  = va_arg(*vargs, int);
      int   matrix_size = va_arg(*vargs, int);

      int flag = ClLanguageConverterMgrInst->convertStringMatrixToStringArray
                  (&strings, &dims, &num_dims,
                   matrix, num_matrix, matrix_size);

      if (! flag) {
        error_code = -1;
        break;
      }

      setStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_CHRMAT) {
      uint  *dims;
      uint   num_dims;
      char **strings;

      char *matrix      = va_arg(*vargs, char *);
      int   num_matrix  = va_arg(*vargs, int);
      int   matrix_size = va_arg(*vargs, int);

      int flag = ClLanguageConverterMgrInst->convertCharMatrixToStringArray
                  (&strings, &dims, &num_dims,
                   matrix, num_matrix, matrix_size);

      if (! flag) {
        error_code = -1;
        break;
      }

      setStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CL_ARG_TYPE_TYPED) {
      char *arg;
      char *args[CL_MAX_CONVERT_ARGS];

      char *from_type = va_arg(*vargs, char *);
      int   to_type   = va_arg(*vargs, int);
      int   num_args  = va_arg(*vargs, int);

      for (int i = 0; i < num_args; i++) {
        arg = va_arg(*vargs, char *);

        if (i < CL_MAX_CONVERT_ARGS)
          args[i] = arg;
      }

      if (to_type == CL_ARG_TYPE_STRARR)
        ClLanguageConverterMgrInst->setTypedStringArrayArg(this, num + 1,
          from_type, CL_MAX_CONVERT_ARGS,
          args[ 0], args[ 1], args[ 2], args[ 3], args[ 4],
          args[ 5], args[ 6], args[ 7], args[ 8], args[ 9],
          args[10], args[11], args[12], args[13], args[14],
          args[15], args[16], args[17], args[18], args[19],
          &error_code);
      else
        ClLanguageMgrInst->syntaxError("conversion to type unsupported");
    }
    else
      break;

    if (error_code != 0)
      break;

    num++;

    type = (CLArgType) va_arg(*vargs, int);
  }

  return num;
}

void
ClLanguageArgs::
setArgs(const string &str)
{
  stringToArgList(str, arg_list_);
}

void
ClLanguageArgs::
setArgs(ClLanguageCommand *command)
{
  if (command != NULL)
    getCommandArgList(command, arg_list_);
}

/*------------------------------------------------------------------*
 *
 * setRealArg
 *   Set the Nth argument supplied by the user to a real
 *   number.
 *
 * CALL:
 *   setRealArg(int n, double real, int &error_code);
 *
 * INPUT:
 *   n          : The number of the argument to be set.
 *
 *   real       : The real number to set the argument to.
 *
 * OUTPUT:
 *   error_code : Error Return Code :-
 *              :  0 - Success
 *              :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setRealArg(int n, double real, int *error_code)
{
  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  ClParserValuePtr value = ClParserValueMgrInst->createValue(real);

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setIntegerArg
 *   Set the Nth argument supplied by the user to an integer
 *   number.
 *
 * CALL:
 *   setIntegerArg(n, integer, &error_code);
 *
 * INPUT:
 *     n          : The number of the argument to be set.
 *      int       :
 *
 *     integer    : The integer number to set the
 *      long      : argument to.
 *
 * OUTPUT:
 *     error_code : Error Return Code :-
 *      int       :  0 - Success
 *                :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setIntegerArg(int n, long integer, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(integer);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setStringArg
 *   Set the Nth argument supplied by the user to a string.
 *
 * CALL:
 *   setStringArg(int n, const string &str, int &error_code);
 *
 * INPUT:
 *   n          : The number of the argument to be set.
 *
 *   str        : The string to set the argument to.
 *
 * OUTPUT:
 *   error_code : Error Return Code :-
 *              :  0 - Success
 *              :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setStringArg(int n, const string &str, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(str);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setCharArrayArg
 *   Set the Nth argument supplied by the user to a
 *   character array.
 *
 * CALL:
 *   setCharArrayArg(n, chars, num_chars, &error_code);
 *
 * INPUT:
 *     n          : The number of the argument to be set.
 *      int       :
 *
 *     chars      : The characters to set the argument to.
 *      char *    :
 *
 *     num_chars   : The number of characters in the
 *      char *    : above array.
 *
 * OUTPUT:
 *     error_code : Error Return Code :-
 *      int       :  0 - Success
 *          :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setCharArrayArg(int n, char *chars, int num_chars, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(chars, num_chars);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setRealArrayArg
 *   Set the Nth argument supplied by the user to a real
 *   array.
 *
 * CALL:
 *   setRealArrayArg(n, reals, dims, num_dims, &error_code);
 *
 * INPUT:
 *     n          : The number of the argument to be set.
 *      int       :
 *
 *     reals      : Array of real numbers to set the
 *      double *  : argument to.
 *
 *     dims       : Dimensions of real array.
 *      int *     :
 *
 *     num_dims    : Number of dimensions of real array.
 *      int       :
 *
 * OUTPUT:
 *     error_code : Error Return Code :-
 *      int       :  0 - Success
 *          :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setRealArrayArg(int n, double *reals, uint *dims, uint num_dims, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(dims, num_dims, reals);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setRealArrayArg
 *   Set the Nth argument supplied by the user to a real
 *   array.
 *
 * CALL:
 *   setRealArrayArg(n, reals, dims, num_dims, &error_code);
 *
 * INPUT:
 *     n         : The number of the argument to be set.
 *      int      :
 *
 *     reals      : Array of real numbers to set the
 *      float *   : argument to.
 *
 *     dims       : Dimensions of real array.
 *      int *     :
 *
 *     num_dims    : Number of dimensions of real array.
 *      int       :
 *
 * OUTPUT:
 *     error_code : Error Return Code :-
 *      int       :  0 - Success
 *          :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setRealArrayArg(int n, float *reals, uint *dims, uint num_dims, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(dims, num_dims, reals);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

// Set the Nth argument supplied by the user to an
// integer array.
//
// Note: startArgs() must been called before using this routine.

void
ClLanguageArgs::
setIntegerArrayArg(int n, long *integers, uint *dims, uint num_dims, int *error_code)
{
  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  ClParserValuePtr value =
    ClParserValueMgrInst->createValue(dims, num_dims, integers);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                        command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setIntegerArrayArg
 *   Set the Nth argument supplied by the user to an
 *   integer array.
 *
 * CALL:
 *   setIntegerArrayArg(n, integers, dims, num_dims, &error_code);
 *
 * INPUT:
 *     n          : The number of the argument to be set.
 *      int       :
 *
 *     integers   : Array of integer numbers to set the
 *      int *     : argument to.
 *
 *     dims       : Dimensions of integer array.
 *      int *     :
 *
 *     num_dims    : Number of dimensions of integer array.
 *      int       :
 *
 * OUTPUT:
 *     error_code : Error Return Code :-
 *      int       :  0 - Success
 *          :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setIntegerArrayArg(int n, int *integers, uint *dims, uint num_dims, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(dims, num_dims, integers);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setStringArrayArg
 *   Set the Nth argument supplied by the user to a
 *   string array.
 *
 * CALL:
 *   setStringArrayArg(n, strings, dims, num_dims, &error_code);
 *
 * INPUT:
 *     n          : The number of the argument to be set.
 *      int       :
 *
 *     strings    : Array of strings to set the
 *      char **   : argument to.
 *
 *     dims       : Dimensions of string array.
 *      int *     :
 *
 *     num_dims    : Number of dimensions of string array.
 *      int       :
 *
 * OUTPUT:
 *     error_code : Error Return Code :-
 *      int       :  0 - Success
 *          :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setStringArrayArg(int n, char **strings, uint *dims, uint num_dims, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->
    createValue(dims, num_dims, (const char **) strings);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setExpressionArg
 *   Set the Nth argument supplied by the user to the
 *   result of evaluating an expression.
 *
 * CALL:
 *   setExpressionArg(int n, const string &expression, int &error_code);
 *
 * INPUT:
 *   n          : The number of the argument to be set.
 *
 *   expression : The expression to evaluate.
 *
 * OUTPUT:
 *   error_code : Error Return Code :-
 *              :  0 - Success
 *              :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setExpressionArg(int n, const string &expression, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  ClParserExpr expr(expression);

  if (! expr.exec(value)) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION, "'%s' argument %d - '%s'",
                                        command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }

  if (! value.isValid()) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->syntaxError("undefined '%s' argument %d - '%s'",
                                    command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 4;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setValueArg
 *   Set the Nth argument supplied by the user to a value.
 *
 * CALL:
 *   setValueArg(int n, const ClParserValuePtr &value, int &error_code);
 *
 * INPUT:
 *   n          : The number of the argument to be set.
 *
 *   value      : The value to set the argument to.
 *
 * OUTPUT:
 *   error_code : Error Return Code :-
 *              :  0 - Success
 *              :  1 - Failure
 *
 * NOTES:
 *   startArgs() must been called before using this routine.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
setValueArg(int n, const ClParserValuePtr &value, int *error_code)
{
  *error_code = 0;

  string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                        command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * getCommandArgValues
 *   Convert all the arguments supplied with a command
 *   to values.
 *
 * CALL:
 *   int flag = getCommandArgValues
 *     (ClLanguageCommand *command, ClParserValuePtr &(*values), int &num_values);
 *
 * INPUT:
 *   command    : Structure containing details
 *              : of the command and arguments
 *              : supplied by the user.
 *
 * OUTPUT:
 *   values     : Array of values extracted
 *              : from the command's arguments.
 *
 *   num_values : Number of Values in the above array.
 *
 * RETURNS:
 *   flag       : Indicator at to whether the
 *              : arguments were successfully
 *              : converted to values.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageArgs::
getCommandArgValues(ClLanguageCommand *command, ClParserValuePtr **values, int *num_values)
{
  int error_code;

  /* Set Return Flag */

  bool flag = false;

  /* Get Command's Arguments */

  startArgs(command);

  /* Create Return Values Array */

  *num_values = getNumArgs();

  if (*num_values > 0) {
    *values = new ClParserValuePtr [*num_values];

    for (int i = 0; i < *num_values; i++)
      (*values)[i] = ClParserValuePtr();
  }
  else
    *values = NULL;

  /* Evaluate each Argument */

  for (int i = 1; i <= *num_values; i++) {
    string arg = getArg(i, &error_code);

    if (error_code != 0)
      goto GetCommandValues_1;

    ClParserExpr expr(arg);

    if (! expr.exec((*values)[i - 1])) {
      const string &command_name = ClLanguageMgrInst->getCommandName();

      ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
                                          "'%s' argument %d - '%s'",
                                          command_name.c_str(), i, arg.c_str());

      goto GetCommandValues_1;
    }

    if (! (*values)[i - 1].isValid()) {
      const string &command_name = ClLanguageMgrInst->getCommandName();

      ClLanguageMgrInst->syntaxError("undefined '%s' argument %d - '%s'",
                                      command_name.c_str(), i, arg.c_str());

      goto GetCommandValues_1;
    }
  }

  /* Set Return Flag to Success */

  flag = true;

  goto GetCommandValues_2;

 GetCommandValues_1:
  delete [] *values;

  *values    = NULL;
  *num_values = 0;

 GetCommandValues_2:
  termArgs();

  return flag;
}

/*------------------------------------------------------------------*
 *
 * getCommandArgList
 *   Convert the string supplied after a command to a list
 *   of argument strings.
 *
 *   The arguments are in the form <arg1>,<arg2>,...,<arg3>
 *
 * CALL:
 *   getCommandArgList(ClLanguageCommand *command, ClLanguageArgList *arg_list);
 *
 * INPUT:
 *   command  : Structure containing details
 *            : of the command and arguments
 *            : supplied by the user.
 *
 *   arg_list : List to add the arguments to
 *            : (already created)
 *
 * OUTPUT:
 *   arg_list : List with argument strings added.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
getCommandArgList(ClLanguageCommand *command, ClLanguageArgList *arg_list)
{
  if (command != NULL)
    stringToArgList(command->getArgs(), arg_list);
}

/*------------------------------------------------------------------*
 *
 * stringToArgList
 *   Convert a string to a list of argument strings.
 *
 *   The arguments are in the form <arg1>,<arg2>,...,<arg3>
 *
 * CALL:
 *   stringToArgList(const string &str, ClLanguageArgList *arg_list);
 *
 * INPUT:
 *   str      : String to convert.
 *
 *   arg_list : List to add the arguments to (already created)
 *
 * OUTPUT:
 *   arg_list : List with argument strings added.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
stringToArgList(const string &str, ClLanguageArgList *arg_list)
{
  arg_list->clear();

  int in_brackets1 = 0;
  int in_brackets2 = 0;
  int in_brackets3 = 0;
  int in_brackets4 = 0;

  int len = str.size();

  int i = 0;

  CStrUtil::skipSpace(str, &i);

  string args_temp_string = "";

  while (i < len) {
    if      (str[i] == '\"') {
      args_temp_string += str[i++];

      while (i < len && str[i] != '\"') {
        if (str[i] == '\\') {
          args_temp_string += str[i++];

          if (i < len)
            args_temp_string += str[i++];
        }
        else
          args_temp_string += str[i++];
      }

      if (i < len && str[i] == '\"')
        args_temp_string += str[i++];
      else
        args_temp_string += '\"';
    }
    else if (str[i] == '\'') {
      args_temp_string += str[i++];

      while (i < len && str[i] != '\'') {
        if (str[i] == '\\') {
          args_temp_string += str[i++];

          if (i < len)
            args_temp_string += str[i++];
        }
        else
          args_temp_string += str[i++];
      }

      if (i < len && str[i] == '\'')
        args_temp_string += str[i++];
      else
        args_temp_string += '\'';
    }
    else if (str[i] == '(') {
      in_brackets1++;

      args_temp_string += str[i++];
    }
    else if (str[i] == ')') {
      in_brackets1--;

      args_temp_string += str[i++];
    }
    else if (str[i] == '[') {
      in_brackets2++;

      args_temp_string += str[i++];
    }
    else if (str[i] == ']') {
      in_brackets2--;

      args_temp_string += str[i++];
    }
    else if (i < len - 1 && str[i] == '{' && str[i + 1] == '{') {
      in_brackets3++;

      args_temp_string += str[i++];
      args_temp_string += str[i++];
    }
    else if (i < len - 1 && str[i] == '}' && str[i + 1] == '}') {
      in_brackets3--;

      args_temp_string += str[i++];
      args_temp_string += str[i++];
    }
    else if (str[i] == '{') {
      in_brackets4++;

      args_temp_string += str[i++];
    }
    else if (str[i] == '}') {
      in_brackets4--;

      args_temp_string += str[i++];
    }
    else if (! space_separated_ && str[i] == ',') {
      if (in_brackets1 == 0 && in_brackets2 == 0 &&
          in_brackets3 == 0 && in_brackets4 == 0) {
        i++;

        if (args_temp_string != "") {
          CStrUtil::stripSpaces(args_temp_string);

          char *arg = strdup(args_temp_string.c_str());

          arg_list->push_back(arg);
        }

        args_temp_string = "";
      }
      else
        args_temp_string += str[i++];
    }
    else if (str[i] == ';') {
      if (in_brackets1 == 0 && in_brackets2 == 0 &&
          in_brackets3 == 0 && in_brackets4 == 0)
        break;

      args_temp_string += str[i++];
    }
    else if (CStrUtil::isCComment(str, i)) {
      if (in_brackets1 == 0 && in_brackets2 == 0 &&
          in_brackets3 == 0 && in_brackets4 == 0)
        break;

      args_temp_string += str[i++];
      args_temp_string += str[i++];
    }
    else if (space_separated_ && isspace(str[i])) {
      if (in_brackets1 == 0 && in_brackets2 == 0 &&
          in_brackets3 == 0 && in_brackets4 == 0) {
        i++;

        if (args_temp_string != "") {
          CStrUtil::stripSpaces(args_temp_string);

          char *arg = strdup(args_temp_string.c_str());

          arg_list->push_back(arg);
        }

        args_temp_string = "";
      }
      else
        args_temp_string += str[i++];
    }
    else
      args_temp_string += str[i++];
  }

  if (args_temp_string != "") {
    CStrUtil::stripSpaces(args_temp_string);

    char *arg = strdup(args_temp_string.c_str());

    arg_list->push_back(arg);
  }
}

bool
ClLanguageArgs::
readArgList(const string &str, int *pos, int end_char, string &text)
{
  int pos1 = *pos;

  if (! skipArgList(str, pos, end_char))
    return false;

  text = str.substr(pos1, *pos - pos1);

  CStrUtil::stripSpaces(text);

  (*pos)++;

  return true;
}

/*------------------------------------------------------------------*
 *
 * skipArgList
 *   Skip characters defining an argument list.
 *
 *   The arguments are in the form <arg1>,<arg2>,...,<arg3>
 *
 * CALL:
 *   bool flag =
 *     skipArgList(const string &str, int &pos, int end_char);
 *
 * INPUT:
 *   str      : String containing argument list.
 *
 *   pos      : Position in the string where the
 *            : argument list starts.
 *
 *   end_char : Character which terminates the
 *            : argument list.
 *
 * OUTPUT:
 *   pos      : Position in the string where the
 *            : argument list ends.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageArgs::
skipArgList(const string &str, int *pos, int end_char)
{
  bool flag = false;

  int in_brackets1 = 0;
  int in_brackets2 = 0;
  int in_brackets3 = 0;
  int in_brackets4 = 0;

  int len = str.size();

  while (*pos < len) {
    if (str[*pos] == end_char &&
        in_brackets1 == 0 && in_brackets2 == 0 &&
        in_brackets3 == 0 && in_brackets4 == 0) {
      flag = true;

      goto SkipArgList_1;
    }

    if      (str[*pos] == '\"') {
      (*pos)++;

      while (*pos < len && str[*pos] != '\"') {
        if (str[*pos] == '\\') {
          (*pos)++;

          if (*pos < len)
            (*pos)++;
        }
        else
          (*pos)++;
      }

      if (*pos < len && str[*pos] == '\"')
        (*pos)++;
    }
    else if (str[*pos] == '\'') {
      (*pos)++;

      while (*pos < len && str[*pos] != '\'') {
        if (str[*pos] == '\\') {
          (*pos)++;

          if (*pos < len)
            (*pos)++;
        }
        else
          (*pos)++;
      }

      if (*pos < len && str[*pos] == '\'')
        (*pos)++;
    }
    else if (str[*pos] == '(') {
      in_brackets1++;

      (*pos)++;
    }
    else if (str[*pos] == ')') {
      in_brackets1--;

      (*pos)++;
    }
    else if (str[*pos] == '[') {
      in_brackets2++;

      (*pos)++;
    }
    else if (str[*pos] == ']') {
      in_brackets2--;

      (*pos)++;
    }
    else if (*pos < len - 1 && str[*pos] == '{' && str[*pos + 1] == '{') {
      in_brackets3++;

      (*pos)++;
      (*pos)++;
    }
    else if (*pos < len - 1 && str[*pos] == '}' && str[*pos + 1] == '}') {
      in_brackets3--;

      (*pos)++;
      (*pos)++;
    }
    else if (str[*pos] == '{') {
      in_brackets4++;

      (*pos)++;
    }
    else if (str[*pos] == '}') {
      in_brackets4--;

      (*pos)++;
    }
    else if (str[*pos] == ';') {
      if (in_brackets1 == 0 && in_brackets2 == 0 &&
          in_brackets3 == 0 && in_brackets4 == 0)
        break;

      (*pos)++;
    }
    else if (CStrUtil::isCComment(str, *pos)) {
      if (in_brackets1 == 0 && in_brackets2 == 0 &&
          in_brackets3 == 0 && in_brackets4 == 0)
        break;

      (*pos)++;
      (*pos)++;
    }
    else
      (*pos)++;
  }

  if (end_char == '\0' && *pos >= len)
    flag = true;

 SkipArgList_1:
  return flag;
}

/*------------------------------------------------------------------*
 *
 * replaceCharsInArgList
 *   Replace a character in an argument list.
 *
 * CALL:
 *   replaceCharsInArgList(string &str, int c1, int c2);
 *
 * INPUT:
 *   str : String containing argument list.
 *
 *   c1  : Character to be replaced.
 *
 *   c2  : Character to replace it with.
 *
 * OUTPUT:
 *   str : Modified String.
 *
 * NOTES:
 *   Useful when another character (apart from ',') is
 *   used as the argument separator.
 *
 *------------------------------------------------------------------*/

void
ClLanguageArgs::
replaceCharsInArgList(string &str, int c1, int c2)
{
  int len = str.size();

  int i = 0;

  while (i < len) {
    if (str[i] == '\"') {
      i++;

      while (i < len && str[i] != '\"') {
        if (str[i] == '\\') {
          i++;

          if (i < len)
            i++;
        }
        else
          i++;
      }

      if (i < len && str[i] == '\"')
        i++;
    }
    else if (str[i] == '\'') {
      i++;

      while (i < len && str[i] != '\'') {
        if (str[i] == '\\') {
          i++;

          if (i < len)
            i++;
        }
        else
          i++;
      }

      if (str[i] == '\'')
        i++;
    }
    else {
      if (str[i] == c1)
        str[i] = c2;

      i++;
    }
  }
}

void
ClLanguageArgs::
stackChars(char **chars)
{
  if (chars_stack_ == NULL)
    chars_stack_ = new CharsStack;

  chars_stack_->push_back(chars);
}

void
ClLanguageArgs::
stackRealArray(double *reals, uint *dims)
{
  void **real_array = new void * [2];

  real_array[0] = (void *) reals;
  real_array[1] = (void *) dims;

  if (real_array_stack_ == NULL)
    real_array_stack_ = new RealArrayStack;

  real_array_stack_->push_back(real_array);
}

void
ClLanguageArgs::
stackRealArray(float *reals, uint *dims)
{
  void **real_array = new void * [2];

  real_array[0] = (void *) reals;
  real_array[1] = (void *) dims;

  if (float_array_stack_ == NULL)
    float_array_stack_ = new FloatArrayStack;

  float_array_stack_->push_back(real_array);
}

void
ClLanguageArgs::
stackIntegerArray(long *integers, uint *dims)
{
  void **integer_array = new void * [2];

  integer_array[0] = (void *) integers;
  integer_array[1] = (void *) dims;

  if (integer_array_stack_ == NULL)
    integer_array_stack_ = new IntegerArrayStack;

  integer_array_stack_->push_back(integer_array);
}

void
ClLanguageArgs::
stackIntegerArray(int *integers, uint *dims)
{
  void **integer_array = new void * [2];

  integer_array[0] = (void *) integers;
  integer_array[1] = (void *) dims;

  if (word_array_stack_ == NULL)
    word_array_stack_ = new WordArrayStack;

  word_array_stack_->push_back(integer_array);
}

void
ClLanguageArgs::
stackStringArray(char **strs, uint *dims)
{
  void **string_array = new void * [2];

  string_array[0] = (void *) strs;
  string_array[1] = (void *) dims;

  if (string_array_stack_ == NULL)
    string_array_stack_ = new StringArrayStack;

  string_array_stack_->push_back(string_array);
}
