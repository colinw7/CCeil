#include <CCeilPI.h>

#include <algorithm>

ClParserFuncMgr::
ClParserFuncMgr()
{
}

ClParserFuncMgr::
~ClParserFuncMgr()
{
}

bool
ClParserFuncMgr::
createFunc(const std::string &name, int *error_code)
{
  *error_code = 0;

  if (! ClParserFunc::isValidName(name) ||
      ! ClParserInst->isValidNewName(ClParserNameType::FUNCTION, name)) {
    *error_code = int(ClErr::INVALID_FUNCTION_NAME);
    return false;
  }

  ClParserFunc *func = new ClParserFunc(name);

  addFunction(func);

  return true;
}

bool
ClParserFuncMgr::
createFunc(const std::string &function_string, const std::string &expression_string,
           int *error_code)
{
  *error_code = 0;

  uint len = function_string.size();

  uint i = 0;

  CStrUtil::skipSpace(function_string, &i);

  if (i >= len ||
      (! isalpha(function_string[i]) && function_string[i] != '_')) {
    *error_code = int(ClErr::MISSING_FUNCTION_NAME);
    return false;
  }

  std::string function_name = readFunctionName(function_string, &i);

  CStrUtil::skipSpace(function_string, &i);

  if (i >= len || function_string[i] != '(') {
    *error_code = int(ClErr::MISSING_OPEN_ROUND_BRACKET);
    return false;
  }

  i++;

  FuncArgList arg_list = readFunctionArgList(function_string, &i, error_code);

  if (*error_code != 0)
    return false;

  CStrUtil::skipSpace(function_string, &i);

  if (i < len) {
    *error_code = int(ClErr::EXTRA_FUNCTION_CHARS);
    return false;
  }

  //------

  if (! ClParserFunc::isValidName(function_name) ||
      ! ClParserInst->isValidNewName(ClParserNameType::FUNCTION, function_name)) {
    *error_code = int(ClErr::INVALID_FUNCTION_NAME);
    return false;
  }

  //------

  ClParserFunc *func = new ClParserFunc(function_name);

  addFunction(func);

  //------

  uint num_args = arg_list.size();

  for (uint j = 0; j < num_args; j++)
    func->addArg(arg_list[j]);

  ClParserStackPtr old_stack = ClParserStackMgrInst->getCurrentStack();

  ClParserStackMgrInst->setCurrentStack(func->getStack());

  ClParserParser parser(expression_string);

  if (! parser.parse()) {
    *error_code = parser.getErrorCode();
    ClParserStackMgrInst->setCurrentStack(old_stack);
    return false;
  }

  ClParserStackPtr stack = func->getStack();

  ClParserStack::StackNodeList::const_iterator p1 = stack->stackNodeListBegin();
  ClParserStack::StackNodeList::const_iterator p2 = stack->stackNodeListEnd();

  for ( ; p1 != p2; ++p1)
    markFunctionArgs(func, *p1);

  ClParserStackMgrInst->setCurrentStack(old_stack);

  if (ClParserInst->isDebug())
    func->debugPrint();

  return true;
}

std::string
ClParserFuncMgr::
readFunctionName(const std::string &str, uint *i)
{
  std::string function_name;

  uint i1 = *i;

  if (! CStrUtil::readIdentifier(str, &i1, function_name))
    function_name = "";

  *i = i1;

  return function_name;
}

ClParserFuncMgr::FuncArgList
ClParserFuncMgr::
readFunctionArgList(const std::string &str, uint *i, int *error_code)
{
  CL_PARSER_TRACE("readFunctionArgList")

  *error_code = 0;

  FuncArgList variable_name_list;

  bool in_brackets = true;

  while (in_brackets) {
    CStrUtil::skipSpace(str, i);

    if (str[*i] != '\0' &&
        (isalpha(str[*i]) || str[*i] == '_')) {
      std::string variable_name = readFunctionVariableName(str, i);

      FuncArgList::iterator p =
        std::find_if(variable_name_list.begin(), variable_name_list.end(),
                     std::bind1st(std::equal_to<std::string>(), variable_name));

      if (p != variable_name_list.end()) {
        *error_code = int(ClErr::MULTIPLE_DEF_FN_VARIABLE);

        goto readFunctionArgList_1;
      }

      variable_name_list.push_back(variable_name);
    }
    else {
      *error_code = int(ClErr::INVALID_CHARACTER);

      goto readFunctionArgList_1;
    }

    if      (str[*i] == ',') {
      (*i)++;
    }
    else if (str[*i] == ')') {
      (*i)++;

      in_brackets = false;
    }
    else {
      *error_code = int(ClErr::INVALID_CHARACTER);

      goto readFunctionArgList_1;
    }
  }

 readFunctionArgList_1:
  if (*error_code != 0)
    variable_name_list.clear();

  return variable_name_list;
}

std::string
ClParserFuncMgr::
readFunctionVariableName(const std::string &str, uint *i)
{
  CL_PARSER_TRACE("readFunctionVariableName")

  std::string variable_name;

  uint i1 = *i;

  if (! CStrUtil::readIdentifier(str, &i1, variable_name))
    variable_name = "";

  *i = i1;

  return variable_name;
}

void
ClParserFuncMgr::
markFunctionArgs(ClParserFunc *func, ClParserStackNode *stack_node)
{
  if (stack_node->isIdentifier()) {
    ClParserIdentifierPtr identifier = stack_node->getIdentifier();

    uint num_args = func->getNumArgs();

    for (uint i = 0; i < num_args; ++i) {
      const std::string &arg = func->getArg(i);

      if (identifier->getName() == arg)
        identifier->setArgument();
    }
  }
}

void
ClParserFuncMgr::
addFunction(ClParserFunc *func)
{
  function_list_[func->getName()] = func;
}

bool
ClParserFuncMgr::
isFunction(const std::string &name) const
{
  return getFunction(name).isValid();
}

ClParserFuncPtr
ClParserFuncMgr::
getFunction(const std::string &name) const
{
  FuncMap::const_iterator p = function_list_.find(name);

  if (p == function_list_.end())
    return ClParserFuncPtr();

  return (*p).second;
}

void
ClParserFuncMgr::
removeFunction(const std::string &name)
{
  FuncMap::const_iterator p = function_list_.find(name);

  if (p == function_list_.end())
    return;

  function_list_.erase(name);
}

void
ClParserFuncMgr::
removeAllFunctions()
{
  function_list_.clear();
}

void
ClParserFuncMgr::
printAllFunctions() const
{
  FuncMap::const_iterator p1 = function_list_.begin();
  FuncMap::const_iterator p2 = function_list_.end  ();

  for ( ; p1 != p2; ++p1)
    (*p1).second->print();
}

//-----------------------

ClParserFunc::
ClParserFunc(const std::string &name) :
 name_(name)
{
}

ClParserFunc::
~ClParserFunc()
{
}

ClParserFunc *
ClParserFunc::
dup() const
{
  return new ClParserFunc(name_);
}

//------

void
ClParserFunc::
print() const
{
  ClParserInst->output("%s(", name_.c_str());

  uint num_args = args_.size();

  if (num_args > 0) {
    ClParserInst->output("%s", args_[0].c_str());

    for (uint i = 1; i < num_args; i++)
      ClParserInst->output(",%s", args_[i].c_str());
  }

  ClParserInst->output(") = ");

  stack_->print();

  ClParserInst->output("\n");
}

void
ClParserFunc::
print(std::ostream &os) const
{
  os << name_ << "(";

  uint num_args = args_.size();

  if (num_args > 0) {
    os << args_[0];

    for (uint i = 1; i < num_args; i++)
      os << "," << args_[i];
  }

  os << ") = ";

  stack_->print();

  os << "\n";
}

void
ClParserFunc::
debugPrint() const
{
  fprintf(stderr, "Function   : ");
  fprintf(stderr, "%s", name_.c_str());

  fprintf(stderr, "(");

  int num_args = args_.size();

  for (int i = 0; i < num_args; i++) {
    if (i > 0) fprintf(stderr, ",");

    fprintf(stderr, "%s", args_[i].c_str());
  }

  fprintf(stderr, ") = ");

  stack_->debugPrint();

  fprintf(stderr, "\n");
}

bool
ClParserFunc::
isValidName(const std::string &name)
{
  return CStrUtil::isIdentifier(name);
}
