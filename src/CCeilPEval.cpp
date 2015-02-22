#include <CCeilPI.h>

using std::string;

ClParserExpr::
ClParserExpr(const string &expression) :
 expression_(expression), compiled_(false)
{
}

ClParserExpr::
~ClParserExpr()
{
}

// Compile an expression onto an evaluation stack ready to execute it.
//
// Note: The returned stack value can be used to get the variables in the
// expression and to execute it.

bool
ClParserExpr::
compile()
{
  int error_code = 0;

  ClParserStackPtr stack = ClParserStackMgrInst->startStack();

  ClParserParser parser(expression_);

  if (! parser.parse()) {
    error_code = parser.getErrorCode();
    return false;
  }

  ClParserStackMgrInst->endStack();

  if (error_code != 0) {
    return false;
  }

  stack_    = stack;
  compiled_ = true;

  return true;
}

// Get the variables in the expression compiled into an expression
//
// Note: The returned array can be freed using the freeExpressionVariables() routine.

void
ClParserExpr::
getExpressionVariables(char ***variables, uint *num_variables)
{
  stack_->getVariables(variables, num_variables);
}

// Free the array of variables returned by the routine getExpressionVariables().

void
ClParserExpr::
freeExpressionVariables(char **variables, uint num_variables)
{
  ClParserStack::freeVariables(variables, num_variables);
}

// Run the evaluation stack returned by compiling an expression

bool
ClParserExpr::
exec(ClParserValuePtr &value)
{
  if (stack_.isValid()) {
    ClParserStackMgrInst->startTempStack();

    ClParserStack::StackNodeList::const_iterator p1 = stack_->stackNodeListBegin();
    ClParserStack::StackNodeList::const_iterator p2 = stack_->stackNodeListEnd();

    for ( ; p1 != p2; ++p1)
      (*p1)->stack();
  }
  else {
    ClParserParser parser(expression_);

    if (! parser.parse())
      return false;
  }

  try {
    value = ClParserStackMgrInst->evaluateStack();
  }
  catch (...) {
    value = ClParserValuePtr();
  }

  if (stack_.isValid())
    ClParserStackMgrInst->endTempStack();

  if (! value.isValid())
    return false;

  return true;
}

//------------------------------

ClParserArgValue::
ClParserArgValue()
{
}

ClParserArgValue::
ClParserArgValue(ClParserVarRefPtr var_ref, ClParserValuePtr value) :
 var_ref_(var_ref), value_(value)
{
}

ClParserArgValue::
ClParserArgValue(const ClParserArgValue &arg_value) :
 var_ref_(arg_value.var_ref_), value_(arg_value.value_)
{
}

ClParserArgValue::
~ClParserArgValue()
{
}

const ClParserArgValue &
ClParserArgValue::
operator=(const ClParserArgValue &arg_value)
{
  var_ref_ = arg_value.var_ref_;
  value_   = arg_value.value_;

  return *this;
}
