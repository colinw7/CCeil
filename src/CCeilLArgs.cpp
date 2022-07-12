#include <CCeilL.h>

class ClLanguageArgParser {
 public:
  ClLanguageArgParser(const std::string &str) :
   str_(str), pos_(0), len_(uint(str_.size())) {
    inBrackets_.resize(4);
  }

  uint pos() const { return pos_; }
  void setPos(uint pos) { pos_ = pos; }

  bool eof() const {
    return (pos_ >= len_);
  }

  bool isSpace() const {
    return isspace(str_[pos_]);
  }

  void skipSpaces() {
    CStrUtil::skipSpace(str_, &pos_);
  }

  bool isChar(char c) const {
    return (str_[pos_] == c);
  }

  bool isChars(const std::string &str) const {
    uint len = uint(str.size());

    if (pos_ + len >= len_)
      return false;

    for (uint i = 0; i < len; ++i)
      if (str_[pos_ + i] != str[i])
        return false;

    return true;
  }

  bool isComment() {
    return CStrUtil::isCComment(str_, pos_);
  }

  void addDoubleQuotedString(bool strip) {
    assert(str_[pos_] == '\"');

    if (! strip)
      addChar();
    else
      skipChar();

    while (! eof() && ! isChar('\"')) {
      if (isChar('\\')) {
        addChar();

        if (! eof())
          addChar();
      }
      else
        addChar();
    }

    if (! eof() && isChar('\"')) {
      if (! strip)
        addChar();
      else
        skipChar();
    }
    else {
      if (! strip)
        buffer_ += '\"';
    }
  }

  void addSingleQuotedString(bool strip) {
    assert(str_[pos_] == '\'');

    if (! strip)
      addChar();
    else
      skipChar();

    while (! eof() && ! isChar('\'')) {
      if (isChar('\\')) {
        addChar();

        if (! eof())
          addChar();
      }
      else
        addChar();
    }

    if (! eof() && isChar('\'')) {
      if (! strip)
        addChar();
      else
        skipChar();
    }
    else {
      if (! strip)
        buffer_ += '\'';
    }
  }

  void addChar() {
    buffer_ += str_[pos_++];
  }

  void skipChar() {
    ++pos_;
  }

  void startBracket(uint i) {
    assert(i >= 1 && i <= 4);

    ++inBrackets_[i - 1];
  }

  bool endBracket(uint i) {
    assert(i >= 1 && i <= 4);

    --inBrackets_[i - 1];

    return (inBrackets_[i - 1] >= 0);
  }

  bool inBrackets() const {
    for (uint i = 0; i < 4; ++i)
      if (inBrackets_[i] > 0)
        return true;

    return false;
  }

  void addArg(ClLanguageArgList *arg_list) {
    CStrUtil::stripSpaces(buffer_);

    if (buffer_ != "") {
      char *arg = strdup(buffer_.c_str());

      arg_list->push_back(arg);

      buffer_ = "";
    }

    skipSpaces();
  }

 private:
  const std::string& str_;
  uint               pos_ { 0 };
  uint               len_ { 0 };
  std::string        buffer_;
  std::vector<int>   inBrackets_;
};

// Initialise Environment for Processing the Arguments
// supplied for an application defined command added
// using ClLanguageDefineCommand().
//
// This routine is automatically called before the
// application defined command's routine is called.
//
// Note: After this routine has been called the other argument
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
  if (arg_list_)
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
    arg_list_ = nullptr;

  if (string_stack_) {
    uint num_strings = uint(string_stack_->size());

    while (num_strings--) {
      char *str = (*string_stack_)[num_strings];

      string_stack_->pop_back();

      delete [] str;
    }
  }

  if (chars_stack_) {
    uint num_chars = uint(chars_stack_->size());

    while (num_chars--) {
      char **chars = (*chars_stack_)[num_chars];

      chars_stack_->pop_back();

      delete [] *chars;
    }
  }

  if (real_array_stack_) {
    uint num_real_arrays = uint(real_array_stack_->size());

    while (num_real_arrays--) {
      void **real_array = (*real_array_stack_)[num_real_arrays];

      real_array_stack_->pop_back();

      delete [] (reinterpret_cast<double *>(real_array[0]));
      delete [] (reinterpret_cast<uint *  >(real_array[1]));
    }
  }

  if (float_array_stack_) {
    uint num_float_arrays = uint(float_array_stack_->size());

    while (num_float_arrays--) {
      void **float_array = (*float_array_stack_)[num_float_arrays];

      float_array_stack_->pop_back();

      delete [] (reinterpret_cast<float *>(float_array[0]));
      delete [] (reinterpret_cast<uint * >(float_array[1]));
    }
  }

  if (integer_array_stack_) {
    uint num_integer_arrays = uint(integer_array_stack_->size());

    while (num_integer_arrays--) {
      void **integer_array = (*integer_array_stack_)[num_integer_arrays];

      integer_array_stack_->pop_back();

      delete [] (reinterpret_cast<long *>(integer_array[0]));
      delete [] (reinterpret_cast<uint *>(integer_array[1]));
    }
  }

  if (word_array_stack_) {
    uint num_word_arrays = uint(word_array_stack_->size());

    while (num_word_arrays--) {
      void **word_array = (*word_array_stack_)[num_word_arrays];

      word_array_stack_->pop_back();

      delete [] (reinterpret_cast<int * >(word_array[0]));
      delete [] (reinterpret_cast<uint *>(word_array[1]));
    }
  }

  if (string_array_stack_) {
    uint num_string_arrays = uint(string_array_stack_->size());

    while (num_string_arrays--) {
      void **string_array = (*string_array_stack_)[num_string_arrays];

      string_array_stack_->pop_back();

      delete [] (reinterpret_cast<void **>(string_array[0]));
      delete [] (reinterpret_cast<uint * >(string_array[1]));
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
 *   min   : The minimum number of arguments allowed
 *
 *   max   : The maximum number of arguments allowed
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

  const std::string &command_name = ClLanguageMgrInst->getCommandName();

  if      (min >= 0 && int(num_args) < min) {
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
  else if (max >= min && int(num_args) > max) {
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
    return uint(arg_list_->size());
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
 *                CLArgType::NONE);
 *
 * INPUT:
 *   type  : Argument Type :-
 *         :   CLArgType::SKIP -
 *         :     Skip this argument, no next argument should be supplied.
 *         :   CLArgType::SKIP_N -
 *         :     Skip a number of arguments, the next argument is the number
 *         :     of arguments to skip (supplied as an integer).
 *         :   CLArgType::REAL -
 *         :     Get the next argument as a real number, pointer to a double
 *         :     should be the next argument.
 *         :   CLArgType::INTEGER
 *         :     Get the next argument as an integer number, pointer to a long
 *         :     should be the next argument.
 *         :   CLArgType::WORD
 *         :     Get the next argument as an integer number, pointer to a int
 *         :     should be the next argument.
 *         :   CLArgType::STRING
 *         :     Get the next argument as a string, pointer to a char * should
 *         :     be the next argument.
 *         :   CLArgType::CHARS
 *         :     Get the next argument as a character array, pointer to a char *
 *         :     and an int should be the next two arguments.
 *         :   CLArgType::TEXT
 *         :     Get the next argument as a literal text, pointer to a char *
 *         :     should be the next argument.
 *         :   CLArgType::VALUE
 *         :     Get the next argument as a parser value, pointer to a
 *         :     ClParserValuePtr * should be the next argument.
 *         :   CLArgType::REALS
 *         :     Get the next argument as a single dimension real array, pointer
 *         :     to a double * and int, should be the next two arguments.
 *         :   CLArgType::FLOATS
 *         :     Get the next argument as a single dimension real array, pointer
 *         :     to a float * and int, should be the next two arguments.
 *         :   CLArgType::INTEGERS
 *         :     Get the next argument as a single dimension integer array, pointer
 *         :     to an long * and int, should be the next two arguments.
 *         :   CLArgType::WORDS
 *         :     Get the next argument as a single dimension integer array, pointer
 *         :     to an int * and int, should be the next two arguments.
 *         :   CLArgType::STRINGS
 *         :     Get the next argument as a single dimension string array, pointer
 *         :     to a char ** and int, should be the next two arguments.
 *         :   CLArgType::REALARR
 *         :     Get the next argument as a real array, pointer to a double *,
 *         :     int * and int, should be the next three arguments.
 *         :   CLArgType::FLTARR
 *         :     Get the next argument as a real array, pointer to a float *,
 *         :     int * and int, should be the next three arguments.
 *         :   CLArgType::INTARR
 *         :     Get the next argument as an integer array, pointer to a long *,
 *         :     int * and int, should be the next three arguments.
 *         :   CLArgType::WORDARR
 *         :     Get the next argument as an integer array, pointer to an int *,
 *         :     int * and int, should be the next three arguments.
 *         :   CLArgType::STRARR
 *         :     Get the next argument as a string array, pointer to a char **,
 *         :     int * and int, should be the next three arguments.
 *         :   CLArgType::STRMAT
 *         :     Get the next argument as a string matrix (an array of null terminated
 *         :     fixed length strings stored in a single char * array), pointer to a
 *         :     char * (string matrix), pointer to an int (number of strings) and an
 *         :     int (fixed string length), should be the next three arguments.
 *         :   CLArgType::CHRMAT
 *         :     Get the next argument as a character matrix (an array of non-null terminated
 *         :     fixed length strings stored in a single char * array), pointer to a char *
 *         :     (character matrix), pointer to an int (number of strings) and an int (fixed
 *         :     string length), should be the next three arguments.
 *         :   CLArgType::TYPED
 *         :     Get the next argument as a type which is constructed by an installed
 *         :     converter. The arguments are the type to convert from (one of the
 *         :     CLArgType::???? types), the type to convert to (specified when the
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
 *   type argument. The list is terminated by a type of CLArgType::NONE.
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
 *    (CLArgType::SKIP,
 *     CLArgType::SKIP_N , 3,
 *     CLArgType::REAL   , &real,
 *     CLArgType::INTEGER, &integer,
 *     CLArgType::WORD   , &word,
 *     CLArgType::STRING , &str,
 *     CLArgType::TEXT   , &text,
 *     CLArgType::VALUE  , &value,
 *     CLArgType::REALARR, &real_array, &real_array_dims, &num_real_array_dims,
 *     CLArgType::FLTARR , &float_array, &float_array_dims, &num_float_array_dims,
 *     CLArgType::INTARR , &integer_array, &integer_array_dims, &num_integer_array_dims,
 *     CLArgType::WORDARR, &word_array, &word_array_dims, &num_word_array_dims,
 *     CLArgType::STRARR , &string_array, &string_array_dims, &num_string_array_dims,
 *     CLArgType::NONE);
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

  while (type != CLArgType::NONE) {
    if      (type == CLArgType::SKIP)
      ;
    else if (type == CLArgType::SKIP_N) {
      int integer = va_arg(*vargs, int);

      num += integer - 1;
    }
    else if (type == CLArgType::REAL) {
      double *real = va_arg(*vargs, double *);

      *real = getRealArg(num + 1, &error_code);
    }
    else if (type == CLArgType::INTEGER) {
      long *integer = va_arg(*vargs, long *);

      *integer = getIntegerArg(num + 1, &error_code);
    }
    else if (type == CLArgType::WORD) {
      int *integer = va_arg(*vargs, int *);

      *integer = int(getIntegerArg(num + 1, &error_code));
    }
    else if (type == CLArgType::STRING) {
      char **str = va_arg(*vargs, char **);

      std::string str1 = getStringArg(num + 1, &error_code);

      *str = strdup(str1.c_str());
    }
    else if (type == CLArgType::CHARS) {
      char **chars     = va_arg(*vargs, char **);
      int   *num_chars = va_arg(*vargs, int *);

      getCharArrayArg(num + 1, chars, num_chars, &error_code);
    }
    else if (type == CLArgType::TEXT) {
      char **str = va_arg(*vargs, char **);

      std::string arg = getArg(num + 1, &error_code);

      *str = const_cast<char *>(arg.c_str());
    }
    else if (type == CLArgType::VALUE) {
      ClParserValuePtr *value = va_arg(*vargs, ClParserValuePtr *);

      *value = getValueArg(num + 1, &error_code);
    }
    else if (type == CLArgType::REALS) {
      double **reals     = va_arg(*vargs, double **);
      int     *num_reals = va_arg(*vargs, int *);

      uint num_reals1;

      getRealArrayArg(num + 1, reals, &num_reals1, &error_code);

      if (error_code != 0)
        break;

      *num_reals = int(num_reals1);
    }
    else if (type == CLArgType::FLOATS) {
      float **reals     = va_arg(*vargs, float **);
      int    *num_reals = va_arg(*vargs, int *);

      uint num_reals1;

      getRealArrayArg(num + 1, reals, &num_reals1, &error_code);

      if (error_code != 0)
        break;

      *num_reals = int(num_reals1);
    }
    else if (type == CLArgType::INTEGERS) {
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
        *num_integers = int(dims[0]);
      else
        *num_integers = 0;
    }
    else if (type == CLArgType::WORDS) {
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
        *num_integers = int(dims[0]);
      else
        *num_integers = 0;
    }
    else if (type == CLArgType::STRINGS) {
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
        *num_strings = int(dims[0]);
      else
        *num_strings = 0;
    }
    else if (type == CLArgType::REALARR) {
      double **reals    = va_arg(*vargs, double **);
      uint   **dims     = va_arg(*vargs, uint **);
      uint    *num_dims = va_arg(*vargs, uint *);

      getRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::FLTARR) {
      float **reals    = va_arg(*vargs, float **);
      uint  **dims     = va_arg(*vargs, uint **);
      uint   *num_dims = va_arg(*vargs, uint *);

      getRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::INTARR) {
      long **integers  = va_arg(*vargs, long **);
      uint **dims      = va_arg(*vargs, uint **);
      uint  *num_dims  = va_arg(*vargs, uint *);

      getIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::WORDARR) {
      int  **integers  = va_arg(*vargs, int **);
      uint **dims      = va_arg(*vargs, uint **);
      uint  *num_dims  = va_arg(*vargs, uint *);

      getIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::STRARR) {
      char ***strings  = va_arg(*vargs, char ***);
      uint  **dims     = va_arg(*vargs, uint **);
      uint   *num_dims = va_arg(*vargs, uint *);

      getStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::STRMAT) {
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
                  (strings, dims, num_dims, matrix, num_matrix, matrix_size);

      if (! flag)
        error_code = -1;
    }
    else if (type == CLArgType::CHRMAT) {
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
                  (strings, dims, num_dims, matrix, num_matrix, matrix_size);

      if (! flag)
        error_code = -1;
    }
    else if (type == CLArgType::TYPED) {
      char *args[CL_MAX_CONVERT_ARGS];

      int   from_type = va_arg(*vargs, int);
      char *to_type   = va_arg(*vargs, char *);
      int   num_args  = va_arg(*vargs, int);

      for (int i = 0; i < num_args; i++) {
        char *arg = va_arg(*vargs, char *);

        if (i < CL_MAX_CONVERT_ARGS)
          args[i] = arg;
      }

      if (from_type == int(CLArgType::STRARR))
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

    type = static_cast<CLArgType>(va_arg(*vargs, int));
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
 *     getStringArgList(const std::string &str,
 *                      CLArgType type1, <type1_data>,
 *                      CLArgType type2, <type2_data>,
 *                      CLArgType::NONE);
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
getStringArgList(const std::string &str, CLArgType type, ...)
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
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);
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
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *error_code = 3;

    return integer;
  }

  return integer;
}

// Get the Nth argument supplied by the user as a string.
//
// Note: startArgs() must been called before using this routine.

std::string
ClLanguageArgs::
getStringArg(int n, int *error_code)
{
  *error_code = 0;

  std::string str;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return str;

  if (! value->stringValue(str)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

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
  *chars      = nullptr;
  *num_chars  = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->stringValue(chars, num_chars)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

    *chars      = nullptr;
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
  *reals      = nullptr;
  *dims       = nullptr;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->realArrayValue(reals, dims, num_dims)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);
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
  *reals      = nullptr;
  *dims       = nullptr;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->realArrayValue(reals, dims, num_dims)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

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
  *integers   = nullptr;
  *dims       = nullptr;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->integerArrayValue(integers, dims, num_dims)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

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
  *integers   = nullptr;
  *dims       = nullptr;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->integerArrayValue(integers, dims, num_dims)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

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
  *strings    = nullptr;
  *dims       = nullptr;
  *num_dims   = 0;
  *error_code = 0;

  ClParserValuePtr value = getValueArg(n, error_code);

  if (*error_code != 0)
    return;

  if (! value->stringArrayValue(strings, dims, num_dims)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError
      (ClErr::INVALID_CONVERSION, "'%s' argument %d", command_name.c_str(), n);

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

  std::string arg = getArg(n, &error_code1);

  if (error_code1 != 0) {
    *error_code = 1;

    return value;
  }

  ClParserExpr expr(arg);

  if (! expr.exec(value)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(ClErr::INVALID_EXPRESSION, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 2;

    return value;
  }

  if (! value.isValid()) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

std::string
ClLanguageArgs::
getVariableArg(int n, int *error_code)
{
  int error_code1;

  *error_code = 0;

  std::string arg = getArg(n, &error_code1);

  if (error_code1 != 0) {
    *error_code = 1;

    return arg;
  }

  if (! ClParserInst->isValidAssignString(arg)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

std::string
ClLanguageArgs::
getArg(int n, int *error_code)
{
  *error_code = 0;

  std::string arg;

  uint num_args = getNumArgs();

  if (n >= 1 && n <= int(num_args))
    arg = getIArg(n - 1);
  else
    *error_code = 1;

  return arg;
}

const std::string &
ClLanguageArgs::
getIArg(int i) const
{
  return (*arg_list_)[uint(i)];
}

/*------------------------------------------------------------------*
 *
 * setArgList
 *   Set a List of Arguments for the Current Command.
 *
 * CALL:
 *   int num = setArgList(CLArgType type1, <type1_data>,
 *                        CLArgType type2, <type2_data>,
 *                        CLArgType::NONE);
 *
 * INPUT:
 *   type  : Argument Type :-
 *         :   CLArgType::SKIP -
 *         :     Skip this argument, num next argument should be supplied.
 *         :   CLArgType::SKIP_N -
 *         :     Skip a number of arguments, the next argument is the number of arguments to
 *         :     skip (supplied as an integer).
 *         :   CLArgType::REAL -
 *         :     Set the next argument to a real number, double should be the next argument.
 *         :   CLArgType::INTEGER
 *         :     Set the next argument to an integer number, long should be the next argument.
 *         :   CLArgType::WORD
 *         :     Set the next argument to an integer number, int should be the next argument.
 *         :   CLArgType::STRING
 *         :     Set the next argument to a string, char * should be the next argument.
 *         :   CLArgType::CHARS
 *         :     Set the next argument to a character array, char * and int should be the next
 *         :     two arguments.
 *         :   CLArgType::VALUE
 *         :     Set the next argument to a parser value, pointer to a ClParserValuePtr should be
 *         :     the next argument.
 *         :   CLArgType::EXPR
 *         :     Set the next argument to the result of evaluating an expression, char * should
 *         :     be the next argument.
 *         :   CLArgType::REALS
 *         :     Set the next argument to a single dimension real array, pointer to a
 *         :     double * and int, should be the next two arguments.
 *         :   CLArgType::FLOATS
 *         :     Set the next argument to a single dimension real array, pointer to a
 *         :     float * and int, should be the next two arguments.
 *         :   CLArgType::INTEGERS
 *         :     Set the next argument to a single dimension integer array, pointer to an
 *         :     long * and int, should be the next two arguments.
 *         :   CLArgType::WORDS
 *         :     Set the next argument to a single dimension integer array, pointer to an
 *         :     int * and int, should be the next two arguments.
 *         :   CLArgType::STRINGS
 *         :     Set the next argument to a single dimension string array, pointer to a
 *         :     char ** and int, should be the next two arguments.
 *         :   CLArgType::REALARR
 *         :     Set the next argument to a real array, a double *, int * and int should be the
 *         :     next three arguments.
 *         :   CLArgType::FLTARR
 *         :     Set the next argument to a real array, a float *, int * and int should be the
 *         :     next three arguments.
 *         :   CLArgType::INTARR
 *         :     Set the next argument to an integer array, a long *, int * and int should be the
 *         :     next three arguments.
 *         :   CLArgType::WORDARR
 *         :     Set the next argument to an integer array, an int *, int * and int should be the
 *         :     next three arguments.
 *         :   CLArgType::STRARR
 *         :     Set the next argument to a string array, a char **, int * and int should be the
 *         :     next three arguments.
 *         :   CLArgType::STRMAT
 *         :     Set the next argument to a string matrix (an array of null terminated fixed
 *         :     length strings stored in a single char * array), a char * (string matrix),
 *         :     an int (number of strings) and an int (fixed string length),
 *         :     should be the next three arguments.
 *         :   CLArgType::CHRMAT
 *         :     Set the next argument to a character matrix (an array of non-null terminated
 *         :     fixed length strings stored in a single char * array), a char * (character
 *         :     matrix), an int (number of strings) and an int (fixed string length),
 *         :     should be the next three arguments.
 *         :   CLArgType::TYPED
 *         :     Set the next argument to a type which is constructed by an installed converter.
 *         :     The arguments are the type to convert from (specified when the converter was
 *         :     installed), the type to convert to (one of the CLArgType::???? types),
 *         :     the number of input arguments to be supplied to the converter followed
 *         :     by the input arguments.
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
 *   list is terminated by a type of CLArgType::NONE.
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
 *    (CLArgType::SKIP,
 *     CLArgType::SKIP_N , 3,
 *     CLArgType::REAL   , real,
 *     CLArgType::INTEGER, integer,
 *     CLArgType::WORD   , word,
 *     CLArgType::STRING , str,
 *     CLArgType::VALUE  , value,
 *     CLArgType::EXPR   , expr,
 *     CLArgType::REALARR, real_array, real_array_dims, num_real_array_dims,
 *     CLArgType::FLTARR , float_array, float_array_dims, num_float_array_dims,
 *     CLArgType::INTARR , integer_array, integer_array_dims, num_integer_array_dims,
 *     CLArgType::WORDARR, word_array, word_array_dims, num_word_array_dims,
 *     CLArgType::STRARR , string_array, string_array_dims, num_string_array_dims,
 *     CLArgType::NONE);
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

  while (type != CLArgType::NONE) {
    if      (type == CLArgType::SKIP)
      ;
    else if (type == CLArgType::SKIP_N) {
      int integer = va_arg(*vargs, int);

      num += integer - 1;
    }
    else if (type == CLArgType::REAL) {
      double real = va_arg(*vargs, double);

      setRealArg(num + 1, real, &error_code);
    }
    else if (type == CLArgType::INTEGER) {
      long integer = va_arg(*vargs, long);

      setIntegerArg(num + 1, integer, &error_code);
    }
    else if (type == CLArgType::WORD) {
      int integer = va_arg(*vargs, int);

      setIntegerArg(num + 1, integer, &error_code);
    }
    else if (type == CLArgType::STRING) {
      char *str = va_arg(*vargs, char *);

      setStringArg(num + 1, str, &error_code);
    }
    else if (type == CLArgType::CHARS) {
      char *chars     = va_arg(*vargs, char *);
      int   num_chars = va_arg(*vargs, int);

      setCharArrayArg(num + 1, chars, num_chars, &error_code);
    }
    else if (type == CLArgType::VALUE) {
      ClParserValuePtr *value = va_arg(*vargs, ClParserValuePtr *);

      setValueArg(num + 1, *value, &error_code);
    }
    else if (type == CLArgType::EXPR) {
      char *expr = va_arg(*vargs, char *);

      setExpressionArg(num + 1, expr, &error_code);
    }
    else if (type == CLArgType::REALS) {
      uint dims[1];

      double *reals     = va_arg(*vargs, double *);
      int     num_reals = va_arg(*vargs, int);

      dims[0] = uint(num_reals);

      setRealArrayArg(num + 1, reals, dims, 1, &error_code);
    }
    else if (type == CLArgType::FLOATS) {
      uint dims[1];

      float *reals     = va_arg(*vargs, float *);
      int    num_reals = va_arg(*vargs, int);

      dims[0] = uint(num_reals);

      setRealArrayArg(num + 1, reals, dims, 1, &error_code);
    }
    else if (type == CLArgType::INTEGERS) {
      uint dims[1];

      long *integers     = va_arg(*vargs, long *);
      int   num_integers = va_arg(*vargs, int);

      dims[0] = uint(num_integers);

      setIntegerArrayArg(num + 1, integers, dims, 1, &error_code);
    }
    else if (type == CLArgType::WORDS) {
      uint dims[1];

      int *integers     = va_arg(*vargs, int *);
      int  num_integers = va_arg(*vargs, int);

      dims[0] = uint(num_integers);

      setIntegerArrayArg(num + 1, integers, dims, 1, &error_code);
    }
    else if (type == CLArgType::STRINGS) {
      uint dims[1];

      char **strings     = va_arg(*vargs, char **);
      int    num_strings = va_arg(*vargs, int);

      dims[0] = uint(num_strings);

      setStringArrayArg(num + 1, strings, dims, 1, &error_code);
    }
    else if (type == CLArgType::REALARR) {
      double *reals    = va_arg(*vargs, double *);
      uint   *dims     = va_arg(*vargs, uint *);
      uint    num_dims = va_arg(*vargs, uint);

      setRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::FLTARR) {
      float *reals    = va_arg(*vargs, float *);
      uint  *dims     = va_arg(*vargs, uint *);
      uint   num_dims = va_arg(*vargs, uint);

      setRealArrayArg(num + 1, reals, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::INTARR) {
      long *integers = va_arg(*vargs, long *);
      uint *dims     = va_arg(*vargs, uint *);
      uint  num_dims = va_arg(*vargs, uint);

      setIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::WORDARR) {
      int  *integers = va_arg(*vargs, int *);
      uint *dims     = va_arg(*vargs, uint *);
      uint  num_dims = va_arg(*vargs, uint);

      setIntegerArrayArg(num + 1, integers, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::STRARR) {
      char **strings  = va_arg(*vargs, char **);
      uint  *dims     = va_arg(*vargs, uint *);
      uint   num_dims = va_arg(*vargs, uint);

      setStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::STRMAT) {
      uint  *dims;
      char **strings;
      uint   num_dims;

      char *matrix      = va_arg(*vargs, char *);
      int   num_matrix  = va_arg(*vargs, int);
      int   matrix_size = va_arg(*vargs, int);

      int flag = ClLanguageConverterMgrInst->convertStringMatrixToStringArray
                  (&strings, &dims, &num_dims, matrix, num_matrix, matrix_size);

      if (! flag) {
        error_code = -1;
        break;
      }

      setStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::CHRMAT) {
      uint  *dims;
      uint   num_dims;
      char **strings;

      char *matrix      = va_arg(*vargs, char *);
      int   num_matrix  = va_arg(*vargs, int);
      int   matrix_size = va_arg(*vargs, int);

      int flag = ClLanguageConverterMgrInst->convertCharMatrixToStringArray
                  (&strings, &dims, &num_dims, matrix, num_matrix, matrix_size);

      if (! flag) {
        error_code = -1;
        break;
      }

      setStringArrayArg(num + 1, strings, dims, num_dims, &error_code);
    }
    else if (type == CLArgType::TYPED) {
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

      if (to_type == int(CLArgType::STRARR))
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

    type = static_cast<CLArgType>(va_arg(*vargs, int));
  }

  return num;
}

void
ClLanguageArgs::
setArgs(const std::string &str)
{
  stringToArgList(str, arg_list_);
}

void
ClLanguageArgs::
setArgs(ClLanguageCommand *command)
{
  if (command)
    getCommandArgList(command, arg_list_);
}

/*------------------------------------------------------------------*
 *
 * setRealArg
 *   Set the Nth argument supplied by the user to a real number.
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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  ClParserValuePtr value = ClParserValueMgrInst->createValue(real);

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(integer);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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
 *   setStringArg(int n, const std::string &str, int &error_code);
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
setStringArg(int n, const std::string &str, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(str);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(chars, uint(num_chars));

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(dims, num_dims, reals);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(dims, num_dims, reals);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  ClParserValuePtr value = ClParserValueMgrInst->createValue(dims, num_dims, integers);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(dims, num_dims, integers);

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * setStringArrayArg
 *   Set the Nth argument supplied by the user to a string array.
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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  value = ClParserValueMgrInst->createValue(dims, num_dims, const_cast<const char **>(strings));

  if (! value.isValid())
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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
setExpressionArg(int n, const std::string &expression, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  ClParserExpr expr(expression);

  if (! expr.exec(value)) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(ClErr::INVALID_EXPRESSION, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }

  if (! value.isValid()) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->syntaxError("undefined '%s' argument %d - '%s'",
                                   command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  std::string arg = getVariableArg(n, error_code);

  if (*error_code != 0)
    return;

  int error_code1;

  ClParserInst->assignValue(arg, value, &error_code1);

  if (error_code1 != 0) {
    const std::string &command_name = ClLanguageMgrInst->getCommandName();

    ClLanguageMgrInst->expressionError(error_code1, "'%s' argument %d - '%s'",
                                       command_name.c_str(), n, arg.c_str());

    *error_code = 3;

    return;
  }
}

/*------------------------------------------------------------------*
 *
 * getCommandArgValues
 *   Convert all the arguments supplied with a command to values.
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

  *num_values = int(getNumArgs());

  if (*num_values > 0) {
    *values = new ClParserValuePtr [uint(*num_values)];

    for (uint i = 0; i < uint(*num_values); i++)
      (*values)[i] = ClParserValuePtr();
  }
  else
    *values = nullptr;

  /* Evaluate each Argument */

  for (uint i = 1; i <= uint(*num_values); i++) {
    std::string arg = getArg(int(i), &error_code);

    if (error_code != 0)
      goto GetCommandValues_1;

    ClParserExpr expr(arg);

    if (! expr.exec((*values)[i - 1])) {
      const std::string &command_name = ClLanguageMgrInst->getCommandName();

      ClLanguageMgrInst->expressionError(ClErr::INVALID_EXPRESSION, "'%s' argument %d - '%s'",
                                         command_name.c_str(), i, arg.c_str());

      goto GetCommandValues_1;
    }

    if (! (*values)[i - 1].isValid()) {
      const std::string &command_name = ClLanguageMgrInst->getCommandName();

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

  *values    = nullptr;
  *num_values = 0;

 GetCommandValues_2:
  termArgs();

  return flag;
}

/*------------------------------------------------------------------*
 *
 * getCommandArgList
 *   Convert the string supplied after a command to a list of argument strings.
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
  if (command)
    stringToArgList(command->getArgs(), arg_list);
}

/*------------------------------------------------------------------*
 *
 * stringToArgList
 *   Convert a string to a list of argument strings.
 *
 *   The arguments are in the form:
 *     Comma Separated :
 *       <arg1> , <arg2> , ... , <arg3>
 *     Space Separated :
 *       <arg1> <arg2> ... <arg3>
 *
 * CALL:
 *   stringToArgList(const std::string &str, ClLanguageArgList *arg_list);
 *
 * INPUT:
 *   str       : String to convert.
 *
 *   arg_list  : List to add the arguments to (already created)
 *
 *   separator : Separator char (defaults to ','
 *
 * OUTPUT:
 *   arg_list  : List with argument strings added.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageArgs::
stringToArgList(const std::string &str, ClLanguageArgList *arg_list)
{
  arg_list->clear();

  ClLanguageArgParser parser(str);

  parser.skipSpaces();

  while (! parser.eof()) {
    // parse double quoted string
    if      (parser.isChar('\"')) {
      parser.addDoubleQuotedString(isStripQuotes());
    }
    // parse single quoted string
    else if (parser.isChar('\'')) {
      parser.addSingleQuotedString(isStripQuotes());
    }
    // start parse ( ... ) expression
    else if (parser.isChar('(')) {
      parser.startBracket(1);

      parser.addChar();
    }
    // end parse ( .. ) expression
    else if (parser.isChar(')')) {
      if (! parser.endBracket(1))
        return false;

      parser.addChar();
    }
    // start parse [ .. ] expression
    else if (parser.isChar('[')) {
      parser.startBracket(2);

      parser.addChar();
    }
    // end parse [ ... ] expression
    else if (parser.isChar(']')) {
      if (! parser.endBracket(2))
        return false;

      parser.addChar();
    }
    // start parse {{ ... }} expression
    else if (parser.isChars("{{")) {
      parser.startBracket(3);

      parser.addChar();
      parser.addChar();
    }
    // end parse {{ ... }} expression
    else if (parser.isChars("}}")) {
      if (! parser.endBracket(3))
        return false;

      parser.addChar();
      parser.addChar();
    }
    // start parse { ... } expression
    else if (parser.isChar('{')) {
      parser.startBracket(4);

      parser.addChar();
    }
    // end parse { ... } expression
    else if (parser.isChar('}')) {
      if (! parser.endBracket(4))
        return false;

      parser.addChar();
    }
    // handle semi-colon (line separator)
    else if (parser.isChar(';')) {
      // if not in brackets end parse
      if (! parser.inBrackets())
        break;

      parser.addChar();
    }
    // handle comment (terminates)
    else if (parser.isComment()) {
      if (! parser.inBrackets()) // TODO: skip comment
        break;

      parser.addChar();
      parser.addChar();
    }
    else if (! isSpaceSeparated()) {
      // handle comma separator
      if (parser.isChar(',')) {
        // if not in brackets add arg to list
        if (! parser.inBrackets()) {
          parser.skipChar();

          parser.addArg(arg_list);
        }
        else
          parser.addChar();
      }
      else {
        parser.addChar();
      }
    }
    else {
      // handle space separator
      if (parser.isSpace()) {
        if (! parser.inBrackets()) {
          parser.skipSpaces();

          parser.addArg(arg_list);
        }
        else
          parser.addChar();
      }
      else {
        parser.addChar();
      }
    }
  }

  parser.addArg(arg_list);

  return true;
}

bool
ClLanguageArgs::
readArgList(const std::string &str, int *pos, int end_char, std::string &text)
{
  int pos1 = *pos;

  if (! skipArgList(str, pos, end_char))
    return false;

  text = str.substr(uint(pos1), uint(*pos - pos1));

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
 *   bool flag = skipArgList(const std::string &str, int &pos, int end_char);
 *
 * INPUT:
 *   str      : String containing argument list.
 *
 *   pos      : Position in the string where the argument list starts.
 *
 *   end_char : Character which terminates the argument list.
 *
 * OUTPUT:
 *   pos      : Position in the string where the argument list ends.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageArgs::
skipArgList(const std::string &str, int *pos, int end_char, bool stripQuotes)
{
  bool flag = false;

  ClLanguageArgParser parser(str);

  parser.setPos(uint(*pos));

  while (! parser.eof()) {
    if (parser.isChar(char(end_char)) && ! parser.inBrackets()) {
      flag = true;
      break;
    }

    // skip double quoted string
    if      (parser.isChar('\"')) {
      parser.addDoubleQuotedString(stripQuotes);
    }
    // skip single quoted string
    else if (parser.isChar('\'')) {
      parser.addSingleQuotedString(stripQuotes);
    }
    // start parse ( ... ) expression
    else if (parser.isChar('(')) {
      parser.startBracket(1);

      parser.skipChar();
    }
    // end parse ( ... ) expression
    else if (parser.isChar(')')) {
      if (! parser.endBracket(1))
        break;

      parser.skipChar();
    }
    // start parse [ ... ] expression
    else if (parser.isChar('[')) {
      parser.startBracket(2);

      parser.skipChar();
    }
    // end parse [ ... ] expression
    else if (parser.isChar(']')) {
      if (! parser.endBracket(2))
        break;

      parser.skipChar();
    }
    // start parse {{ ... }} expression
    else if (parser.isChars("{{")) {
      parser.startBracket(3);

      parser.skipChar();
      parser.skipChar();
    }
    // end parse {{ ... }} expression
    else if (parser.isChars("}}")) {
      if (! parser.endBracket(3))
        break;

      parser.skipChar();
      parser.skipChar();
    }
    // start parse { ... } expression
    else if (parser.isChar('{')) {
      parser.startBracket(4);

      parser.skipChar();
    }
    // end parse { ... } expression
    else if (parser.isChar('}')) {
      if (! parser.endBracket(4))
        break;

      parser.skipChar();
    }
   // handle semi-colon (line separator)
    else if (parser.isChar(';')) {
      // if not in brackets end parse
      if (! parser.inBrackets())
        break;

      parser.skipChar();
    }
    // handle comment (terminates)
    else if (parser.isComment()) {
      if (! parser.inBrackets()) // TODO: skip comment
        break;

      parser.skipChar();
      parser.skipChar();
    }
    else
      parser.skipChar();
  }

  if (end_char == '\0' && parser.eof())
    flag = true;

  *pos = int(parser.pos());

  return flag;
}

/*------------------------------------------------------------------*
 *
 * replaceCharsInArgList
 *   Replace a character in an argument list.
 *
 * CALL:
 *   replaceCharsInArgList(std::string &str, int c1, int c2);
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
replaceCharsInArgList(std::string &str, int c1, int c2)
{
  uint len = uint(str.size());

  uint i = 0;

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
        str[i] = char(c2);

      i++;
    }
  }
}

void
ClLanguageArgs::
stackChars(char **chars)
{
  if (! chars_stack_)
    chars_stack_ = new CharsStack;

  chars_stack_->push_back(chars);
}

void
ClLanguageArgs::
stackRealArray(double *reals, uint *dims)
{
  void **real_array = new void * [2];

  real_array[0] = reinterpret_cast<void *>(reals);
  real_array[1] = reinterpret_cast<void *>(dims);

  if (! real_array_stack_)
    real_array_stack_ = new RealArrayStack;

  real_array_stack_->push_back(real_array);
}

void
ClLanguageArgs::
stackRealArray(float *reals, uint *dims)
{
  void **real_array = new void * [2];

  real_array[0] = reinterpret_cast<void *>(reals);
  real_array[1] = reinterpret_cast<void *>(dims);

  if (! float_array_stack_)
    float_array_stack_ = new FloatArrayStack;

  float_array_stack_->push_back(real_array);
}

void
ClLanguageArgs::
stackIntegerArray(long *integers, uint *dims)
{
  void **integer_array = new void * [2];

  integer_array[0] = reinterpret_cast<void *>(integers);
  integer_array[1] = reinterpret_cast<void *>(dims);

  if (! integer_array_stack_)
    integer_array_stack_ = new IntegerArrayStack;

  integer_array_stack_->push_back(integer_array);
}

void
ClLanguageArgs::
stackIntegerArray(int *integers, uint *dims)
{
  void **integer_array = new void * [2];

  integer_array[0] = reinterpret_cast<void *>(integers);
  integer_array[1] = reinterpret_cast<void *>(dims);

  if (! word_array_stack_)
    word_array_stack_ = new WordArrayStack;

  word_array_stack_->push_back(integer_array);
}

void
ClLanguageArgs::
stackStringArray(char **strs, uint *dims)
{
  void **string_array = new void * [2];

  string_array[0] = reinterpret_cast<void *>(strs);
  string_array[1] = reinterpret_cast<void *>(dims);

  if (! string_array_stack_)
    string_array_stack_ = new StringArrayStack;

  string_array_stack_->push_back(string_array);
}
