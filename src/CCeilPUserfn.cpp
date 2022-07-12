#include <CCeilPI.h>

int             ClParserUserFn::error_code_;
ClParserUserFn *ClParserUserFn::execUserFn_;

ClParserUserFnMgr::
ClParserUserFnMgr() :
 ind_(0)
{
}

ClParserUserFnMgr::
~ClParserUserFnMgr()
{
}

ClParserUserFnPtr
ClParserUserFnMgr::
createUserFn(const ClUserFnData &data)
{
  int        type;
  IntVectorT arg_types;

  if (data.types != 0)
    stringToTypes(data.types, &type, arg_types);
  else
    type = 0;

  auto userfn = createUserFn(data.name, uint(type), arg_types, data.proc, data.data);

  if (data.num != 0)
    *data.num = int(userfn->getInd());

  return userfn;
}

ClParserUserFnPtr
ClParserUserFnMgr::
createUserFn(const std::string &name, uint type, int *arg_types,
             uint num_arg_types, ClParserUserFnProc proc, void *data)
{
  IntVectorT arg_types1;

  for (uint i = 0; i < num_arg_types; ++i)
    arg_types1.push_back(arg_types[i]);

  return createUserFn(name, type, arg_types1, proc, data);
}

ClParserUserFnPtr
ClParserUserFnMgr::
createUserFn(const std::string &name, uint type, const IntVectorT &arg_types,
             ClParserUserFnProc proc, void *data)
{
  ClParserUserFn *userfn = new ClParserUserFn(name, type, arg_types, proc, data);

  user_function_map_[name] = userfn;

  return user_function_map_[name];
}

uint
ClParserUserFnMgr::
getNextInd()
{
  return ind_++;
}

void
ClParserUserFnMgr::
addUserFn(ClParserUserFnPtr userfn)
{
  user_function_map_[userfn->getName()] = userfn;
}

void
ClParserUserFnMgr::
removeUserFn(const std::string &name)
{
  user_function_map_.erase(name);
}

bool
ClParserUserFnMgr::
isUserFn(const std::string &name) const
{
  UserFnMap::const_iterator p = user_function_map_.find(name);

  return (p != user_function_map_.end());
}

ClParserUserFnPtr
ClParserUserFnMgr::
getUserFn(const std::string &name) const
{
  UserFnMap::const_iterator p = user_function_map_.find(name);

  if (p == user_function_map_.end())
    return ClParserUserFnPtr();

  return (*p).second;
}

void
ClParserUserFnMgr::
stringToTypes(const std::string &str, int *type, IntVectorT &arg_types)
{
  uint i   = 0;
  uint len = uint(str.size());

  *type = CL_PARSER_USERFN_TYPE_STD;

  while (i < len) {
    CStrUtil::skipSpace(str, &i);

    if (i >= len)
      break;

    int arg_type = CL_PARSER_VALUE_TYPE_INPUT;

    while (i < len && str[i] != ',' && ! isspace(str[i])) {
      switch (str[i]) {
        case '&': {
          arg_type &= ~CL_PARSER_VALUE_TYPE_INPUT;
          arg_type |=  CL_PARSER_VALUE_TYPE_OUTPUT;

          break;
        }
        case 'r':
          arg_type |= CL_PARSER_VALUE_TYPE_REAL;
          break;
        case 'i':
          arg_type |= CL_PARSER_VALUE_TYPE_INTEGER;
          break;
        case 's':
          arg_type |= CL_PARSER_VALUE_TYPE_STRING;
          break;
        case 'a':
          arg_type |= CL_PARSER_VALUE_TYPE_ARRAY;
          break;
        case 'l':
          arg_type |= CL_PARSER_VALUE_TYPE_LIST;
          break;
        case 'd':
          arg_type |= CL_PARSER_VALUE_TYPE_DICTIONARY;
          break;
        case 't':
          arg_type |= CL_PARSER_VALUE_TYPE_STRUCTURE;
          break;
        case 'g':
          arg_type |= CL_PARSER_VALUE_TYPE_GENERIC;
          break;
        case '.': {
          uint j = i;

          i = len;

          std::string arg = CStrUtil::stripSpaces(str.substr(j));

          if (arg != "...") {
            ClParserInst->error("Illegal Argument Type '%s'\n", arg.c_str());
            break;
          }

          *type = CL_PARSER_USERFN_TYPE_VARARGS;

          break;
        }
        default:
          ClParserInst->error("Illegal Argument Type '%c'\n", arg_type);
          break;
      }

      if (*type == CL_PARSER_USERFN_TYPE_VARARGS)
        break;

      i++;
    }

    if (*type == CL_PARSER_USERFN_TYPE_VARARGS)
      break;

    if (i < len && str[i] == ',')
      i++;

    arg_types.push_back(arg_type);
  }
}

//----------------

bool
ClParserUserFn::
getExecArgList(CLArgType type, va_list *vargs)
{
  assert(execUserFn_);

  return execUserFn_->getArgList(type, vargs);
}

bool
ClParserUserFn::
setExecArgList(CLArgType type, va_list *vargs)
{
  assert(execUserFn_);

  return execUserFn_->setArgList(type, vargs);
}

ClParserUserFn::
ClParserUserFn(const std::string &name, uint type, const IntVectorT &arg_types,
               ClParserUserFnProc proc, void *data) :
 ind_(0), type_(type), name_(name), proc_(proc), data_(data)
{
  error_code_ = 0;

  // Ensure valid name

  if (! isValidName(name_) ||
      ! ClParserInst->isValidNewName(ClParserNameType::USERFN, name_)) {
    error_code_ = int(ClErr::INVALID_FUNCTION_NAME);
    return;
  }

  //-----

  ind_ = ClParserInst->getNextUserFnInd();

  uint num_arg_types = uint(arg_types.size());

  for (uint i = 0; i <  num_arg_types; i++) {
    arg_types_.push_back(arg_types[i]);

    if (! (arg_types[i] & CL_PARSER_VALUE_TYPE_INPUT ) &&
        ! (arg_types[i] & CL_PARSER_VALUE_TYPE_OUTPUT))
      arg_types_[i] |= CL_PARSER_VALUE_TYPE_INPUT;
  }
}

ClParserUserFn::
~ClParserUserFn()
{
}

ClParserUserFn *
ClParserUserFn::
dup() const
{
  return new ClParserUserFn(name_, type_, arg_types_, proc_, data_);
}

ClParserValuePtr
ClParserUserFn::
exec(ClParserValuePtr *values, uint num_values, int *error_code)
{
  initArgs(values, num_values);

  ClParserStackMgrInst->startTempStack();

  ClParserValuePtr value = exec1(values, num_values, error_code);

  ClParserStackMgrInst->endTempStack();

  termArgs();

  if (*error_code < 0) {
    *error_code = int(ClErr::USER_FUNCTION_FAILED);
    return ClParserValuePtr();
  }

  return value;
}

ClParserValuePtr
ClParserUserFn::
exec(ClParserValueArray &values, int *error_code)
{
  uint num_values = uint(values.size());

  ClParserValuePtr *values1 = new ClParserValuePtr [num_values + 1];

  for (uint i = 0; i < num_values; ++i)
    values1[i] = values[i];

  ClParserValuePtr value = exec(values1, num_values, error_code);

  for (uint i = 0; i < num_values; ++i)
    values[i] = values1[i];

  delete [] values1;

  return value;
}

void
ClParserUserFn::
initArgs(ClParserValuePtr *values, uint num_values)
{
  execUserFn_ = this;

  exec_data_.values_     = values;
  exec_data_.num_values_ = num_values;
}

ClParserValuePtr
ClParserUserFn::
exec1(ClParserValuePtr *values, uint num_values, int *error_code)
{
  return (*proc_)(values, num_values, data_, error_code);
}

void
ClParserUserFn::
termArgs()
{
  exec_data_.term();
}

void
ClParserUserFn::
print() const
{
  ClParserInst->output(" %s ", name_.c_str());
}

void
ClParserUserFn::
print(std::ostream &os) const
{
  os << " " << name_ << " ";
}

void
ClParserUserFn::
debugPrint() const
{
  fprintf(stderr, " %s ", name_.c_str());
}

bool
ClParserUserFn::
isValidName(const std::string &name)
{
  return CStrUtil::isIdentifier(name);
}

bool
ClParserUserFn::
getArgList(CLArgType type, va_list *vargs)
{
  uint num = 0;

  while (type != CLArgType::NONE) {
    if (num >= exec_data_.num_values_)
      return getArgsFail(int(ClErr::INVALID_NO_FUNCTION_ARGS));

    if      (type == CLArgType::REAL) {
      double *real = va_arg(*vargs, double *);

      if (! exec_data_.values_[num]->realValue(real))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));
    }
    else if (type == CLArgType::INTEGER) {
      long *integer = va_arg(*vargs, long *);

      if (! exec_data_.values_[num]->integerValue(integer))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));
    }
    else if (type == CLArgType::WORD) {
      int *integer = va_arg(*vargs, int *);

      if (! exec_data_.values_[num]->integerValue(integer))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));
    }
    else if (type == CLArgType::STRING) {
      std::string str1;

      char **str = va_arg(*vargs, char **);

      if (! exec_data_.values_[num]->stringValue(str1))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));

      *str = CStrUtil::strdup(str1.c_str());

      exec_data_.free_strings_.push_back(*str);
    }
    else if (type == CLArgType::CHARS) {
      char **chars = va_arg(*vargs, char **);
      int   *len   = va_arg(*vargs, int *);

      if (! exec_data_.values_[num]->stringValue(chars, len))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));

      exec_data_.free_chars_.push_back(*chars);
    }
    else if (type == CLArgType::REALS) {
      uint *dims;
      uint  num_dims;

      double **reals     = va_arg(*vargs, double **);
      int     *num_reals = va_arg(*vargs, int *);

      if (! exec_data_.values_[num]->realArrayValue(reals, &dims, &num_dims))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));

      if (num_dims != 1) {
        ClParserInst->error("Invalid Real Array Dimension");
        *num_reals = 0;
      }
      else
        *num_reals = int(dims[0]);

      exec_data_.free_array_reals_    .push_back(*reals);
      exec_data_.free_array_real_dims_.push_back(dims);
    }
    else if (type == CLArgType::FLOATS) {
      uint *dims;
      uint  num_dims;

      float **reals     = va_arg(*vargs, float **);
      int    *num_reals = va_arg(*vargs, int *);

      if (! exec_data_.values_[num]->realArrayValue(reals, &dims, &num_dims))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));

      if (num_dims != 1) {
        ClParserInst->error("Invalid Real Array Dimension");
        *num_reals = 0;
      }
      else
        *num_reals = int(dims[0]);

      exec_data_.free_array_floats_    .push_back(*reals);
      exec_data_.free_array_float_dims_.push_back(dims);
    }
    else if (type == CLArgType::INTEGERS) {
      uint *dims;
      uint  num_dims;

      long **integers     = va_arg(*vargs, long **);
      int   *num_integers = va_arg(*vargs, int *);

      if (! exec_data_.values_[num]->integerArrayValue(integers, &dims, &num_dims))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));

      if (num_dims != 1) {
        ClParserInst->error("Invalid Integer Array Dimension");
        *num_integers = 0;
      }
      else
        *num_integers = int(dims[0]);

      exec_data_.free_array_integers_    .push_back(*integers);
      exec_data_.free_array_integer_dims_.push_back(dims);
    }
    else if (type == CLArgType::WORDS) {
      uint *dims;
      uint  num_dims;

      int **integers     = va_arg(*vargs, int **);
      int  *num_integers = va_arg(*vargs, int *);

      if (! exec_data_.values_[num]->integerArrayValue(integers, &dims, &num_dims))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));

      if (num_dims != 1) {
        ClParserInst->error("Invalid Integer Array Dimension");
        *num_integers = 0;
      }
      else
        *num_integers = int(dims[0]);

      exec_data_.free_array_words_    .push_back(*integers);
      exec_data_.free_array_word_dims_.push_back(dims);
    }
    else if (type == CLArgType::STRINGS) {
      uint *dims;
      uint  num_dims;

      char ***strings     = va_arg(*vargs, char ***);
      int    *num_strings = va_arg(*vargs, int *);

      if (! exec_data_.values_[num]->stringArrayValue(strings, &dims, &num_dims))
        return getArgsFail(int(ClErr::INVALID_CONVERSION));

      if (num_dims != 1) {
        ClParserInst->error("Invalid String Array Dimension");
        *num_strings = 0;
      }
      else
        *num_strings = int(dims[0]);

      exec_data_.free_array_strings_    .push_back(*strings);
      exec_data_.free_array_string_dims_.push_back(dims);
    }
    else if (type == CLArgType::SKIP)
      ;
    else if (type == CLArgType::SKIP_N) {
      int n = va_arg(*vargs, int);

      if (n <= 0)
        ClParserInst->error("Invalid Skip Size");
      else
        num += uint(n - 1);
    }
    else
      return getArgsFail(int(ClErr::INVALID_TYPE_FOR_OPERATOR));

    num++;

    type = static_cast<CLArgType>(va_arg(*vargs, int));
  }

  return true;
}

bool
ClParserUserFn::
setArgList(CLArgType type, va_list *vargs)
{
  ClParserValuePtr value;

  uint num = 0;

  while (type != CLArgType::NONE) {
    if (num >= exec_data_.num_values_)
      return setArgsFail(int(ClErr::INVALID_NO_FUNCTION_ARGS));

    if      (type == CLArgType::REAL) {
      double real = va_arg(*vargs, double);

      value = ClParserValueMgrInst->createValue(real);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::INTEGER) {
      long integer = va_arg(*vargs, long);

      value = ClParserValueMgrInst->createValue(integer);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::WORD) {
      int integer = va_arg(*vargs, int);

      value = ClParserValueMgrInst->createValue(long(integer));

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::STRING) {
      char *str = va_arg(*vargs, char *);

      value = ClParserValueMgrInst->createValue(str);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::CHARS) {
      char *chars = va_arg(*vargs, char *);
      int   len   = va_arg(*vargs, int);

      value = ClParserValueMgrInst->createValue(chars, uint(len));

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::REALS) {
      uint dims[1];

      double *reals     = va_arg(*vargs, double *);
      int     num_reals = va_arg(*vargs, int);

      dims[0] = uint(num_reals);

      value = ClParserValueMgrInst->createValue(dims, 1, reals);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::FLOATS) {
      uint dims[1];

      float *reals     = va_arg(*vargs, float *);
      int    num_reals = va_arg(*vargs, int);

      dims[0] = uint(num_reals);

      value = ClParserValueMgrInst->createValue(dims, 1, reals);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::INTEGERS) {
      uint dims[1];

      long *integers     = va_arg(*vargs, long *);
      int   num_integers = va_arg(*vargs, int);

      dims[0] = uint(num_integers);

      value = ClParserValueMgrInst->createValue(dims, 1, integers);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::WORDS) {
      uint dims[1];

      int *integers     = va_arg(*vargs, int *);
      int  num_integers = va_arg(*vargs, int);

      dims[0] = uint(num_integers);

      value = ClParserValueMgrInst->createValue(dims, 1, integers);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::STRINGS) {
      uint dims[1];

      const char **strings     = const_cast<const char **>(va_arg(*vargs, char **));
      int          num_strings = va_arg(*vargs, int);

      dims[0] = uint(num_strings);

      value = ClParserValueMgrInst->createValue(dims, 1, strings);

      exec_data_.values_[num] = value;
    }
    else if (type == CLArgType::SKIP)
      ;
    else if (type == CLArgType::SKIP_N) {
      int n = va_arg(*vargs, int);

      if (n <= 0)
        ClParserInst->error("Invalid Skip Size");
      else
        num += uint(n - 1);
    }
    else
      return setArgsFail(int(ClErr::INVALID_TYPE_FOR_OPERATOR));

    num++;

    type = static_cast<CLArgType>(va_arg(*vargs, int));
  }

  return true;
}

bool
ClParserUserFn::
getArgsFail(int error_code) const
{
  ClParserInst->error("Failed to get values for User Function %s - %s",
                      name_.c_str(), ClParserInst->getErrorMessage(error_code));
  return false;
}

bool
ClParserUserFn::
setArgsFail(int error_code) const
{
  ClParserInst->error("Failed to set values for User Function %s - %s",
                      name_.c_str(), ClParserInst->getErrorMessage(error_code));
  return false;
}

void
ClParserUserFn::ExecData::
term()
{
  uint num = uint(free_strings_.size());

  for (uint i = 0; i < num; i++)
    delete [] free_strings_[i];

  free_strings_.clear();

  num = uint(free_chars_.size());

  for (uint i = 0; i < num; i++)
    delete [] free_chars_[i];

  free_chars_.clear();

  num = uint(free_array_reals_.size());

  for (uint i = 0; i < num; i++) {
    delete [] free_array_reals_[i];
    delete [] free_array_real_dims_[i];
  }

  free_array_reals_.clear();
  free_array_real_dims_.clear();

  num = uint(free_array_floats_.size());

  for (uint i = 0; i < num; i++) {
    delete [] free_array_floats_[i];
    delete [] free_array_float_dims_[i];
  }

  free_array_floats_.clear();
  free_array_float_dims_.clear();

  num = uint(free_array_integers_.size());

  for (uint i = 0; i < num; i++) {
    delete [] free_array_integers_[i];
    delete [] free_array_integer_dims_[i];
  }

  free_array_integers_.clear();
  free_array_integer_dims_.clear();

  num = uint(free_array_words_.size());

  for (uint i = 0; i < num; i++) {
    delete [] free_array_words_[i];
    delete [] free_array_word_dims_[i];
  }

  free_array_words_.clear();
  free_array_word_dims_.clear();

  num = uint(free_array_strings_.size());

  for (uint i = 0; i < num; i++) {
    delete [] free_array_strings_[i];
    delete [] free_array_string_dims_[i];
  }

  free_array_strings_.clear();
  free_array_string_dims_.clear();

  execUserFn_ = 0;

  values_     = 0;
  num_values_ = 0;
}
