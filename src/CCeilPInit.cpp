#include <CCeilPI.h>
#include <CCeilPErrorP.h>

using std::string;

ClParser *
ClParser::
getInstance()
{
  static ClParser *instance;

  if (! instance)
    instance = new ClParser;

  return instance;
}

ClParser::
ClParser()
{
  angle_type_       = CANGLE_TYPE_RADIANS;
  angle_to_radians_ = 1.0;
  angle_to_degrees_ = 180.0/M_PI;
  radians_to_angle_ = 1.0;
  degrees_to_angle_ = M_PI/180.0;
  real_format_      = "%lf";
  integer_format_   = "%ld";
  string_format_    = "%s";
  math_fail_        = false;
  tolerance_        = 1E-5;

  parse_as_reals_ = false;
  dollar_prefix_  = false;

  init_depth_  = 0;
  output_fp_   = stdout;
  output_proc_ = NULL;
  output_data_ = NULL;

  strcmp_  = (StrCmpProc ) strcmp;
  strncmp_ = (StrNCmpProc) strncmp;
}

void
ClParser::
init()
{
  setDebug(false);

  if (init_depth_ != 0) {
    newTypeList();
    newVariableList();
  }

  ++init_depth_;
}

void
ClParser::
term()
{
  --init_depth_;

  if (init_depth_ < 0)
    init_depth_ = 0;

  if (init_depth_ != 0) {
    oldTypeList();
    oldVariableList();
  }
}

void
ClParser::
setCaseSensitive(bool flag)
{
  if (flag) {
    strcmp_  = (StrCmpProc ) strcmp;
    strncmp_ = (StrNCmpProc) strncmp;
  }
  else {
    strcmp_  = (StrCmpProc ) CStrUtil::casecmp;
    strncmp_ = (StrNCmpProc) CStrUtil::casecmp;
  }
}

bool
ClParser::
isValidAssignString(const string &str)
{
  ClParserParser parser(str);

  return parser.isValidAssign();
}

// Assign the specified value to the value resulting from
// evaluating supplied str.

void
ClParser::
assignValue(const string &str, ClParserValuePtr value, int *error_code)
{
  *error_code = 0;

  if (ClParserVar::isValidName(str))
    createVar(str, value);
  else {
    ClParserParser parser(str);

    if (! parser.parse()) {
      *error_code = parser.getErrorCode();
      return;
    }

    ClParserStackMgrInst->push(CL_PARSER_OP_EQUALS);

    ClParserStackMgrInst->toNext();

    ClParserStackMgrInst->push(value);

    ClParserStackMgrInst->toNext();

    ClParserValuePtr value2 = ClParserStackMgrInst->evaluateStack();

    if (! value2.isValid()) {
      *error_code = CLERR_INVALID_EXPRESSION;
      return;
    }
  }
}

// Assign the specified value to the specified subscript
// of the value resulting from evaluating supplied str.

void
ClParser::
assignSubscriptValue(const string &str, int i, ClParserValuePtr value,
                     int *error_code)
{
  *error_code = 0;

  ClParserParser parser(str);

  if (! parser.parse()) {
    *error_code = parser.getErrorCode();
    return;
  }

  ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_S_BRACKET);

  ClParserStackMgrInst->toNext();

  ClParserValuePtr value1 = ClParserValueMgrInst->createValue(long(i));

  ClParserStackMgrInst->push(value1);

  ClParserStackMgrInst->toNext();

  ClParserStackMgrInst->push(CL_PARSER_OP_CLOSE_S_BRACKET);

  ClParserStackMgrInst->toNext();

  ClParserStackMgrInst->push(CL_PARSER_OP_EQUALS);

  ClParserStackMgrInst->toNext();

  ClParserStackMgrInst->push(value);

  ClParserStackMgrInst->toNext();

  ClParserValuePtr value2 = ClParserStackMgrInst->evaluateStack();

  if (! value2.isValid()) {
    *error_code = CLERR_INVALID_EXPRESSION;
    return;
  }
}

bool
ClParser::
isValidNewName(ClParserNameType type, const string &name) const
{
  if (isInternFn(name) ||
      (type != CL_PARSER_NAME_TYPE_FUNCTION && isFunction(name)) ||
      (type != CL_PARSER_NAME_TYPE_STRUCT   && isType    (name)) ||
      (type != CL_PARSER_NAME_TYPE_VARIABLE && isVariable(name)) ||
      (type != CL_PARSER_NAME_TYPE_USERFN   && isUserFn  (name)))
    return false;
  else
    return true;
}

//------------------

// Type

void
ClParser::
newTypeList()
{
  typeMgr_.newTypeList();
}

void
ClParser::
oldTypeList()
{
  typeMgr_.oldTypeList();
}

void
ClParser::
printAllTypes() const
{
  typeMgr_.printAllTypes();
}

bool
ClParser::
isType(const string &name) const
{
  return typeMgr_.isType(name);
}

ClParserTypePtr
ClParser::
createType()
{
  return typeMgr_.createType();
}

ClParserTypePtr
ClParser::
createType(const string &name)
{
  return typeMgr_.createType(name);
}

ClParserTypePtr
ClParser::
createType(const string &name, const string &arg_string)
{
  return typeMgr_.createType(name, arg_string);
}

ClParserTypePtr
ClParser::
getType(const string &name) const
{
  return typeMgr_.getType(name);
}

ClParserTypePtr
ClParser::
getIntegerType() const
{
  return typeMgr_.getIntegerType();
}

ClParserTypePtr
ClParser::
getRealType() const
{
  return typeMgr_.getRealType();
}

ClParserTypePtr
ClParser::
getStringType() const
{
  return typeMgr_.getStringType();
}

void
ClParser::
deleteType(const string &name)
{
  return typeMgr_.deleteType(name);
}

void
ClParser::
deleteAllTypes()
{
  typeMgr_.deleteAllTypes();
}

//------------------

ClParserVarPtr
ClParser::
createStdVar(const string &name, ClParserValuePtr value)
{
  return varMgr_.createStdVar(name, value);
}

ClParserVarPtr
ClParser::
createVar(const string &name, ClParserValuePtr value)
{
  return varMgr_.createVar(name, value);
}

ClParserVarRefPtr
ClParser::
createVarRef(ClParserVarPtr variable, const ClParserValueArray &subscripts)
{
  return varMgr_.createVarRef(variable, subscripts);
}

ClParserStructVarRefPtr
ClParser::
createStructVarRef(ClParserVarPtr variable, const string &name,
                   const ClParserValueArray &subscripts)
{
  return varMgr_.createStructVarRef(variable, name, subscripts);
}

void
ClParser::
newVariableList()
{
  varMgr_.newVariableList();
}

void
ClParser::
oldVariableList()
{
  varMgr_.oldVariableList();
}

void
ClParser::
deleteAllVariables()
{
  varMgr_.deleteAllVariables();
}

void
ClParser::
printAllVariables() const
{
  varMgr_.printAllVariables();
}

bool
ClParser::
isVariable(const string &name) const
{
  return varMgr_.isVariable(name);
}

ClParserVarPtr
ClParser::
getVariable(const string &name, bool create) const
{
  return varMgr_.getVariable(name, create);
}

ClParserValuePtr
ClParser::
getVariableValue(const string &name) const
{
  return varMgr_.getVariableValue(name);
}

void
ClParser::
setVariableValue(const string &name, ClParserValuePtr value)
{
  varMgr_.setVariableValue(name, value);
}

void
ClParser::
removeVariable(const string &name)
{
  return varMgr_.removeVariable(name);
}

void
ClParser::
callVariableProc(ClParserVarPtr variable)
{
  varMgr_.callVariableProc(variable);
}

//------------------

bool
ClParser::
createFunc(const string &name, int *error_code)
{
  return funcMgr_.createFunc(name, error_code);
}

bool
ClParser::
createFunc(const string &function_string, const string &expression_string, int *error_code)
{
  return funcMgr_.createFunc(function_string, expression_string, error_code);
}

void
ClParser::
deleteAllFunctions()
{
  funcMgr_.removeAllFunctions();
}

void
ClParser::
printAllFunctions() const
{
  funcMgr_.printAllFunctions();
}

bool
ClParser::
isFunction(const string &name) const
{
  return funcMgr_.isFunction(name);
}

ClParserFuncPtr
ClParser::
getFunction(const string &name) const
{
  return funcMgr_.getFunction(name);
}

void
ClParser::
removeFunction(const string &name)
{
  return funcMgr_.removeFunction(name);
}

//------------------

bool
ClParser::
isInternFn(const string &name) const
{
  return internFnMgr_.isInternFn(name);
}

ClParserInternFnPtr
ClParser::
getInternFn(const string &name) const
{
  return internFnMgr_.getInternFn(name);
}

//------------------

ClParserUserFnPtr
ClParser::
createUserFn(const ClUserFnData &data)
{
  return userFnMgr_.createUserFn(data);
}

ClParserUserFnPtr
ClParser::
createUserFn(const string &name, uint type, int *arg_types, uint num_arg_types,
             ClParserUserFnProc proc, void *data)
{
  return userFnMgr_.createUserFn(name, type, arg_types, num_arg_types, proc, data);
}

ClParserUserFnPtr
ClParser::
createUserFn(const string &name, uint type, const IntVectorT &arg_types,
             ClParserUserFnProc proc, void *data)
{
  return userFnMgr_.createUserFn(name, type, arg_types, proc, data);
}

void
ClParser::
removeUserFn(const string &name)
{
  return userFnMgr_.removeUserFn(name);
}

bool
ClParser::
isUserFn(const string &name) const
{
  return userFnMgr_.isUserFn(name);
}

ClParserUserFnPtr
ClParser::
getUserFn(const string &name) const
{
  return userFnMgr_.getUserFn(name);
}

uint
ClParser::
getNextUserFnInd()
{
  return userFnMgr_.getNextInd();
}

//------------------

bool
ClParser::
getUserFnArgList(CLArgType type, ...)
{
  va_list vargs;

  /*CONSTANTCONDITION*/
  va_start(vargs, type);

  bool flag = ClParserUserFn::getExecArgList(type, &vargs);

  va_end(vargs);

  return flag;
}

bool
ClParser::
setUserFnArgList(CLArgType type, ...)
{
  va_list vargs;

  /*CONSTANTCONDITION*/
  va_start(vargs, type);

  bool flag = ClParserUserFn::setExecArgList(type, &vargs);

  va_end(vargs);

  return flag;
}

void
ClParser::
restartStack()
{
  ClParserStackMgrInst->restart();
}

void
ClParser::
output(const char *format, ...)
{
  va_list args;

  va_start(args, format);

  output(format, &args);

  va_end(args);
}

void
ClParser::
output(const char *format, va_list *args)
{
  if (output_proc_ != NULL) {
    string str;

    CStrUtil::vsprintf(str, format, args);

    (*output_proc_)(str.c_str(), output_data_);
  }
  else {
    vfprintf(output_fp_, format, *args);

    fflush(output_fp_);
  }
}

void
ClParser::
error(const char *format, ...)
{
  va_list args;

  va_start(args, format);

  error(format, &args);

  va_end(args);
}

void
ClParser::
error(const char *format, va_list *args)
{
  fprintf (stderr, "CL Parser Error - ");
  vfprintf(stderr, format, *args);
  fprintf (stderr,"\n");

  ClParserStackMgrInst->restartAll();
}

const char *
ClParser::
getErrorMessage(int error_code)
{
  static char error_message[64];

  if (error_code == 0)
    return "";

  if (error_code < 1 || error_code > PARSER_NO_ERRORS) {
    sprintf(error_message, "Invalid Error Code %d", error_code);

    return error_message;
  }

  return parser_error_strings[error_code - 1];
}
