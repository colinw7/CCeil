#include <CCeilPI.h>

ClParserStackMgr *
ClParserStackMgr::
getInstance()
{
  static ClParserStackMgr *instance;

  if (! instance)
    instance = new ClParserStackMgr;

  return instance;
}

ClParserStackMgr::
ClParserStackMgr()
{
  startStack();
}

ClParserStackMgr::
~ClParserStackMgr()
{
}

//---------------------------

// TODO: move all stack eval code here

ClParserValuePtr
ClParserStackMgr::
evaluateStack()
{
  int error_code = 0;

  toStart();

  ClParserValuePtr value = unstackExpressionValues(&error_code);

  if (error_code != 0)
    return ClParserValuePtr();

  return value;
}

// Unstack a list of expressions and return the resultant value

ClParserValuePtr
ClParserStackMgr::
unstackExpressionValues(int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  for (;;) {
    value = unstackExpressionValue(error_code);

    if (*error_code != 0)
      return value;

    toNext();

    ClParserStackNode *stack_node = getCurrentStackNode();

    if (! stack_node) {
      toPrev();
      break;
    }

    if (! stack_node->isOperator() || ! stack_node->isOperator(CL_PARSER_OP_COMMA)) {
      toPrev();
      break;
    }

    removeCurrentStackNode();

    value = ClParserValuePtr();

    toNext();
  }

  return value;
}

// Unstack Expression and return the resultant value

ClParserValuePtr
ClParserStackMgr::
unstackExpressionValue(int *error_code)
{
  unstackExpression(error_code);

  if (*error_code != 0)
    return ClParserValuePtr();

  ClParserValuePtr value;

  if (! pop(value)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);
    return ClParserValuePtr();
  }

  return value;
}

//------------------------------

// Unstack and evaluate an expression from the evaluation
// stack and leave the resultant value on the stack.
//
// Note: Stops when the last token is removed from the evaluation
// stack or an unexpected punctuation token '(', ')', '[',
// ']', ',', '?', ':', '.' is encountered.

void
ClParserStackMgr::
unstackExpression(int *error_code)
{
  *error_code = 0;

  bool value_on_stack = false;
  bool at_next        = false;

  ClParserStackNode *initial_stack_node = getCurrentStackNode();

  ClParserStackNode *stack_node = initial_stack_node;

  if (! stack_node) {
    *error_code = int(ClErr::NULL_EXPRESSION);
    return;
  }

  bool end_expression = false;

  while (! end_expression) {
    if      (stack_node->isOperator()) {
      ClParserOperatorPtr op = stack_node->getOperator();

      // ++ --
      if      (op->isInline()) {
        if (value_on_stack) {
          unstackInlineOperator(error_code);

          if (*error_code != 0)
            return;

          value_on_stack = true;
        }
        else {
          at_next = toNext();

          value_on_stack = false;
        }
      }
      // + - ! ~
      else if (op->isUnary()) {
        at_next = toNext();

        value_on_stack = false;
      }
      // + - * / % ** < <= > >= == != ~= & | ^ && || << >> #
      else if (op->isBinary()) {
        if (checkUnstack(op)) {
          toPrev();

          ClParserOperatorPtr last_operator = getLastOperator();

          if      (last_operator->isInline()) {
            unstackInlineOperator(error_code);

            if (*error_code != 0)
              return;

            value_on_stack = true;
          }
          else if (last_operator->isUnary()) {
            unstackUnaryOperator(error_code);

            if (*error_code != 0)
              return;

            value_on_stack = true;
          }
          else if (last_operator->isBinary()) {
            unstackBinaryOperator(error_code);

            if (*error_code != 0)
              return;

            value_on_stack = true;
          }
          else {
            *error_code = int(ClErr::INVALID_OPERATOR);
            return;
          }

          at_next = toNext();

          value_on_stack = false;
        }
        else {
          at_next = toNext();

          value_on_stack = false;
        }
      }
      // = += -= *= /= %= &= |= ^= <<= >>=
      else if (op->isAssignment()) {
        at_next = toNext();

        value_on_stack = false;
      }
      // (
      else if (op->isType(CL_PARSER_OP_OPEN_R_BRACKET)) {
        unstackBracketedExpression(error_code);

        if (*error_code != 0)
          return;

        value_on_stack = true;
      }
      // [
      else if (op->isType(CL_PARSER_OP_OPEN_S_BRACKET)) {
        unstackArrayValue(error_code);

        if (*error_code != 0)
          return;

        if (value_on_stack) {
          ClParserValuePtr subscript_value;

          if (! pop(subscript_value)) {
            *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
            return;
          }

          toPrev();

          subscriptStackedValue(subscript_value, error_code);

          if (*error_code != 0)
            return;
        }

        value_on_stack = true;
      }
      // {
      else if (op->isType(CL_PARSER_OP_OPEN_BRACE)) {
        unstackListValue(error_code);

        if (*error_code != 0)
          return;

        value_on_stack = true;
      }
      // {{
      else if (op->isType(CL_PARSER_OP_OPEN_DICT)) {
        unstackDictionaryValue(error_code);

        if (*error_code != 0)
          return;

        value_on_stack = true;
      }
      // ?
      else if (op->isType(CL_PARSER_OP_QUESTION_MARK)) {
        unstackSelectExpression(error_code);

        if (*error_code != 0)
          return;

        value_on_stack = true;
      }
      // ( ) [ ] { } {{ }} , ? : .
      else if (op->isPunctuation())
        end_expression = true;
      else {
        *error_code = int(ClErr::INVALID_OPERATOR);
        return;
      }
    }
    else if (stack_node->isInternFn()) {
      unstackInternalFunction(error_code);

      if (*error_code != 0)
        return;

      value_on_stack = true;
    }
    else if (stack_node->isUserFn()) {
      unstackUserFunction(error_code);

      if (*error_code != 0)
        return;

      value_on_stack = true;
    }
    else if (stack_node->isType()) {
      unstackTypeFunction(error_code);

      if (*error_code != 0)
        return;

      value_on_stack = true;
    }
    else if (stack_node->isValue() || stack_node->isVarRef() || stack_node->isStructVarRef()) {
      at_next = toNext();

      value_on_stack = true;
    }
    else if (stack_node->isIdentifier()) {
      ClParserIdentifierPtr identifier = stack_node->getIdentifier();

      if      (identifier->isStructPart()) {
        unstackStructure(error_code);

        if (*error_code != 0)
          return;

        value_on_stack = true;
      }
      else if (identifier->isVariable()) {
        unstackVariable(error_code);

        if (*error_code != 0)
          return;

        value_on_stack = true;
      }
      else if (identifier->isFunction()) {
        unstackFunction(error_code);

        if (*error_code != 0)
          return;

        value_on_stack = true;
      }
      else {
        *error_code = int(ClErr::INVALID_CHARACTER);
        return;
      }
    }
    else {
      *error_code = int(ClErr::INVALID_CHARACTER);
      return;
    }

    stack_node = getCurrentStackNode();

    if (! stack_node)
      end_expression = true;

    if (ClParserInst->isDebug())
      debugPrintCurrent();
  }

  if (at_next)
    toPrev();

  if (initial_stack_node == stack_node) {
    *error_code = int(ClErr::NULL_EXPRESSION);
    return;
  }

  backUnstackExpression(error_code);

  if (*error_code != 0)
    return;
}

// Apply Subscript Value to Object on top of the Evaluation Stack.

void
ClParserStackMgr::
subscriptStackedValue(ClParserValuePtr subscript_value, int *error_code)
{
  ClParserVarRefPtr var_ref;

  if (pop(var_ref)) {
    var_ref->addSubscript(subscript_value);

    push(var_ref);
  }
  else {
    ClParserValuePtr value;

    if (! pop(value)) {
      *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
      return;
    }

    ClParserValuePtr sub_value;

    if (! value->subscriptValue(subscript_value, sub_value)) {
      *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
      return;
    }

    push(sub_value);
  }
}

// Unstack a structure type function from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note:
//   Stack Before : ^ <typefn> '(' <expression_list> ')'
//   Stack After  : ^ <value>

void
ClParserStackMgr::
unstackTypeFunction(int *error_code)
{
  *error_code = 0;

  ClParserTypePtr type;

  pop(type);

  ClParserValueArray value_list;

  ClParserOperatorPtr op;

  bool in_brackets = true;

  // skip '('
  toNext();

  while (in_brackets) {
    ClParserValuePtr value = unstackExpressionValue(error_code);

    if (*error_code != 0 && *error_code != int(ClErr::NULL_EXPRESSION))
      return;

    if      (*error_code == 0) {
      value_list.push_back(value);

      toNext();

      pop(op);

      if (op) {
        if      (op->isType(CL_PARSER_OP_CLOSE_R_BRACKET))
          in_brackets = false;
        else if (! op->isType(CL_PARSER_OP_COMMA))
          *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
      }
      else
        *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
    }
    else if (value_list.empty()) {
      toNext();

      pop(op);

      if (op) {
        if (op->isType(CL_PARSER_OP_CLOSE_R_BRACKET)) {
          in_brackets = false;

          *error_code = 0;
        }
        else
          *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
      }
      else
        *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
    }
    else
      *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);

    if (*error_code != 0)
      return;
  }

  pop(op);

  uint num_values = uint(value_list.size());

  if (num_values > 0 && type->getNumSubTypes() != num_values) {
    if (num_values < type->getNumSubTypes())
      *error_code = int(ClErr::TOO_FEW_ARGUMENTS);
    else
      *error_code = int(ClErr::TOO_MANY_ARGUMENTS);

    return;
  }

  ClParserValuePtr value;

  if (num_values > 0) {
    ClParserValueArray values;

    for (uint i = 0; i < num_values; ++i)
      values.push_back(value_list[i]);

    value = ClParserValueMgrInst->createValue(type, values);
  }
  else
    value = ClParserValueMgrInst->createValue(type);

  if (*error_code != 0)
    return;

  push(value);
}

// Unstack a bracketed expression from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note:
//   Stack Before : ^ '(' <expression> ')'
//   Stack After  : ^ <value>

void
ClParserStackMgr::
unstackBracketedExpression(int *error_code)
{
  *error_code = 0;

  toNext();

  ClParserValuePtr value = unstackExpressionValues(error_code);

  if (*error_code != 0)
    return;

  ClParserOperatorPtr op;

  pop(op);

  toPrev();

  pop(op);

  push(value);
}

// Unstack an array expression from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note:
//   Stack Before : ^ '[' <expression_list> ']'
//   Stack After  : <value> ^

void
ClParserStackMgr::
unstackArrayValue(int *error_code)
{
  *error_code = 0;

  /*-----------*/

  // skip '['
  toNext();

  // check for '[]'
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (stack_node->isOperator(CL_PARSER_OP_CLOSE_S_BRACKET)) {
    toPrev();

    ClParserOperatorPtr op;

    pop(op);
    pop(op);

    ClParserValuePtr array_value = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_ARRAY);

    push(array_value);

    return;
  }

  /*-----------*/

  ClParserOperatorPtr op;
  ClParserValuePtr    value;
  ClParserValueArray  values;

  bool in_brackets = true;

  while (in_brackets) {
    ClParserValuePtr value1 = unstackExpressionValue(error_code);

    if (*error_code != 0)
      return;

    /*----------*/

    if (value) {
      if (! ClParserValue::checkBinaryTypes(value, value1)) {
        *error_code = int(ClErr::INVALID_TYPE_MIX);
        return;
      }
    }
    else
      value = value1;

    /*----------*/

    if (value1)
      values.push_back(value1);

    /*----------*/

    pop(op);

    if (op) {
      if      (op->isType(CL_PARSER_OP_CLOSE_S_BRACKET))
        in_brackets = false;
      else if (! op->isType(CL_PARSER_OP_COMMA))
        *error_code = int(ClErr::INVALID_ARRAY_SYNTAX);
    }
    else
      *error_code = int(ClErr::INVALID_ARRAY_SYNTAX);

    if (*error_code != 0)
      return;
  }

  /*-----------*/

  // starting '['
  toPrev();

  pop(op);

  ClParserValuePtr array_value;

  if (! values.empty())
    array_value = ClParserValueMgrInst->createValue(values);
  else
    array_value = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_ARRAY);

  if (*error_code != 0)
    return;

  push(array_value);
}

// Unstack a list expression from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note:
//   Stack Before : ^ '{' <expression_list> '}'
//   Stack After  : <value> ^

void
ClParserStackMgr::
unstackListValue(int *error_code)
{
  *error_code = 0;

  /*-----------*/

  // skip '{'
  toNext();

  // check for '{}'
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (stack_node->isOperator(CL_PARSER_OP_CLOSE_BRACE)) {
    toPrev();

    ClParserOperatorPtr op;

    pop(op);
    pop(op);

    ClParserValuePtr list_value = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_LIST);

    push(list_value);

    return;
  }

  /*-----------*/

  ClParserOperatorPtr op;
  ClParserValueArray  values;

  bool in_brackets = true;

  while (in_brackets) {
    ClParserValuePtr value = unstackExpressionValue(error_code);

    if (*error_code != 0)
      return;

    /*----------*/

    values.push_back(value);

    /*----------*/

    pop(op);

    if (op) {
      if      (op->isType(CL_PARSER_OP_CLOSE_BRACE))
        in_brackets = false;
      else if (! op->isType(CL_PARSER_OP_COMMA))
        *error_code = int(ClErr::INVALID_LIST_SYNTAX);
    }
    else
      *error_code = int(ClErr::INVALID_LIST_SYNTAX);

    if (*error_code != 0)
      return;
  }

  /*-----------*/

  // starting '{'
  toPrev();

  pop(op);

  ClParserValuePtr list_value;

  if (! values.empty())
    list_value = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_LIST, values);
  else
    list_value = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_LIST);

  push(list_value);
}

// Unstack a dictionary expression from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note:
//   Stack Before : ^ '{{' <expression_list> '}}'
//   Stack After  : ^ <value>

void
ClParserStackMgr::
unstackDictionaryValue(int *error_code)
{
  *error_code = 0;

  /*-----------*/

  // skip '{{'
  toNext();

  // check for '{{}}'
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (stack_node->isOperator(CL_PARSER_OP_CLOSE_DICT)) {
    toPrev();

    ClParserOperatorPtr op;

    pop(op);
    pop(op);

    ClParserValuePtr dict_value =
      ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_DICTIONARY);

    push(dict_value);

    return;
  }

  /*-----------*/

  ClParserOperatorPtr op;
  ClParserValueArray  values;

  bool in_brackets = true;

  while (in_brackets) {
    ClParserValuePtr value = unstackExpressionValue(error_code);

    if (*error_code != 0)
      return;

    /*----------*/

    values.push_back(value);

    /*----------*/

    pop(op);

    if (op) {
      if      (op->isType(CL_PARSER_OP_CLOSE_DICT))
        in_brackets = false;
      else if (! op->isType(CL_PARSER_OP_COMMA))
        *error_code = int(ClErr::INVALID_DICT_SYNTAX);
    }
    else
      *error_code = int(ClErr::INVALID_DICT_SYNTAX);

    if (*error_code != 0)
      return;
  }

  /*-----------*/

  // starting '{{'
  toPrev();

  pop(op);

  ClParserValuePtr dict_value;

  if (! values.empty())
    dict_value = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_DICTIONARY, values);
  else
    dict_value = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_DICTIONARY);

  push(dict_value);
}

// Unstack a select expression from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note:
//   Stack Before : <expression> ^ '?' <expression> : <expression>
//   Stack After  : ^ <value>

void
ClParserStackMgr::
unstackSelectExpression(int *error_code)
{
  *error_code = 0;

  toPrev();

  backUnstackExpression(error_code);

  if (*error_code != 0)
    return;

  ClParserValuePtr value;

  if (! pop(value)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);

    return;
  }

  if (! value->convertToInteger()) {
    *error_code = int(ClErr::INVALID_CONVERSION);
    return;
  }

  int flag = int(value->getInteger()->getValue());

  ClParserOperatorPtr op;

  pop(op);

  ClParserValuePtr value1;

  if (flag) {
    value1 = unstackExpressionValue(error_code);

    if (*error_code != 0)
      return;

    pop(op);

    skipExpression(error_code);

    if (*error_code != 0)
      return;

    //toPrev();
  }
  else {
    skipExpression(error_code);

    if (*error_code != 0)
      return;

    pop(op);

    value1 = unstackExpressionValue(error_code);

    if (*error_code != 0)
      return;
  }

  push(value1);
}

// Unstack and evaluate a unary (<unary_op> <value>),
// binary (<value> <binary_op> <value>) or assignment
// (<value> <assignment_op> <value>) expression from the
// evaluation stack which ends at the current position
// and leave the resultant value on the stack.

void
ClParserStackMgr::
backUnstackExpression(int *error_code)
{
  *error_code = 0;

  ClParserOperatorPtr last_operator = getLastOperator();

  while (last_operator) {
    if      (last_operator->isInline()) {
      unstackInlineOperator(error_code);

      if (*error_code != 0)
        return;
    }
    else if (last_operator->isUnary()) {
      unstackUnaryOperator(error_code);

      if (*error_code != 0)
        return;
    }
    else if (last_operator->isBinary()) {
      unstackBinaryOperator(error_code);

      if (*error_code != 0)
        return;
    }
    else if (last_operator->isAssignment()) {
      unstackAssignmentOperator(error_code);

      if (*error_code != 0)
        return;
    }
    else
      break;

    if (ClParserInst->isDebug())
      debugPrintCurrent();

    last_operator = getLastOperator();
  }
}

// Unstack an inline (++|-- <variable>) or <variable> ++|--)
// expression from the evaluation stack which ends at the
// current position and restack the resultant value.
//
// Note: Stack Before : <op> ^ <lvalue> | <lvalue> ^ <op>
//       Stack After  : ^ <value>

void
ClParserStackMgr::
unstackInlineOperator(int *error_code)
{
  *error_code = 0;

  toPrev();

  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node) {
    *error_code = int(ClErr::NULL_EXPRESSION);
    return;
  }

  bool postfix = false;

  ClParserOperatorPtr op;
  ClParserVarRefPtr   var_ref;

  if (stack_node->isOperator()) {
    postfix = true;

    pop(op);

    toPrev();

    if (! pop(var_ref)) {
      *error_code = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
      return;
    }
  }
  else {
    if (! pop(var_ref)) {
      *error_code = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
      return;
    }

    toPrev();

    pop(op);
  }

  toPrev();

  ClParserValuePtr value = ClParserProcessInlineOperator(op, var_ref, postfix, error_code);

  if (*error_code != 0)
    return;

  push(value);
}

// Unstack a unary (<unary_op> <value>) expression
// from the evaluation stack which ends at the current
// position and restack the resultant value.
//
// Note: Stack Before : <op> ^ <value>
//       Stack After  : ^ <value>

void
ClParserStackMgr::
unstackUnaryOperator(int *error_code)
{
  *error_code = 0;

  ClParserValuePtr value1;

  if (! pop(value1)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);
    return;
  }

  toPrev();

  ClParserOperatorPtr op;

  pop(op);

  ClParserValuePtr value = ClParserProcessUnaryOperator(op, value1, error_code);

  if (*error_code != 0)
    return;

  push(value);
}

// Unstack a binary (<value> <binary_op> <value>)
// expression from the evaluation stack which ends at
// the current position and restack the resultant value.
//
// Note: Stack Before : <value> <op> ^ <value>
//       Stack After  : <value> ^

void
ClParserStackMgr::
unstackBinaryOperator(int *error_code)
{
  *error_code = 0;

  ClParserValuePtr value2;

  if (! pop(value2)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);
    return;
  }

  toPrev();

  ClParserOperatorPtr op;

  if (! pop(op)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);
    return;
  }

  toPrev();

  ClParserValuePtr value1;

  if (! pop(value1)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);
    return;
  }

  ClParserValuePtr value = ClParserProcessBinaryOperator(value1, op, value2, error_code);

  if (*error_code != 0)
    return;

  push(value);
}

// Unstack an assignment binary (<value> <assignment_op>
// <value>) expression from the evaluation stack which
// ends at the current position and restack the resultant
// value.
//
// Note:
//   Stack Before : <lvalue> <op> ^ <value>
//   Stack After  : <value> ^

void
ClParserStackMgr::
unstackAssignmentOperator(int *error_code)
{
  *error_code = 0;

  ClParserValuePtr value;

  if (! pop(value)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);
    return;
  }

  toPrev();

  ClParserOperatorPtr op;

  pop(op); // assignment_op

  toPrev();

  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node) {
    *error_code = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
    return;
  }

  if (stack_node->isVarRef()) {
    ClParserVarRefPtr var_ref;

    if (! pop(var_ref)) {
      *error_code = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
      return;
    }

    ClParserProcessAssignmentOperator(var_ref, op, value, error_code);

    if (*error_code != 0)
      return;

    push(var_ref);
  }
  else if (stack_node->isStructVarRef()) {
    ClParserStructVarRefPtr svar_ref;

    if (! pop(svar_ref)) {
      *error_code = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
      return;
    }

    ClParserProcessAssignmentOperator(svar_ref, op, value, error_code);

    if (*error_code != 0)
      return;

    push(svar_ref);
  }
  else {
    *error_code = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
    return;
  }
}

// Unstack an internal function from the evaluation stack
// which starts at the current position and restack the
// resultant value.
//
// Note:
//   Stack Before : ^ <internfn> '(' <expression_list> ')'
//   Stack After  : ^ <value>

void
ClParserStackMgr::
unstackInternalFunction(int *error_code)
{
  *error_code = 0;

  ClParserInternFnPtr internfn;

  pop(internfn);

  ClParserOperatorPtr op;
  ClParserValueArray  value_list;

  bool in_brackets = true;

  toNext();

  while (in_brackets) {
    ClParserValuePtr value = unstackExpressionValue(error_code);

    if (*error_code != 0 && *error_code != int(ClErr::NULL_EXPRESSION))
      return;

    if      (*error_code == 0) {
      value_list.push_back(value);

      pop(op);

      if (op) {
        if      (op->isType(CL_PARSER_OP_CLOSE_R_BRACKET))
          in_brackets = false;
        else if (! op->isType(CL_PARSER_OP_COMMA))
          *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
      }
      else
        *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
    }
    else if (value_list.empty()) {
      pop(op);

      if (op) {
        if (op->isType(CL_PARSER_OP_CLOSE_R_BRACKET)) {
          in_brackets = false;

          *error_code = 0;
        }
        else
          *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
      }
      else
        *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
    }
    else
      *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);

    if (*error_code != 0)
      return;
  }

  toPrev();

  pop(op);

  uint num_values = uint(value_list.size());

  if (num_values == 0) {
    *error_code = int(ClErr::INVALID_NO_FUNCTION_ARGS);
    return;
  }

  uint num_args    = 0;
  bool is_variable = false;

  uint inum_args = internfn->getNumArgs();

  for (uint i = 0; i < inum_args; ++i) {
    if (internfn->getArg(i).variable) {
      is_variable = true;
      break;
    }

    ++num_args;
  }

  if (! is_variable && num_args != num_values) {
    if (num_values < num_args)
      *error_code = int(ClErr::TOO_FEW_ARGUMENTS);
    else
      *error_code = int(ClErr::TOO_MANY_ARGUMENTS);
    return;
  }

  ClParserValuePtr value = ClParserProcessInternFn(internfn, value_list, error_code);

  if (*error_code != 0 || ! value)
    return;

  push(value);
}

// Unstack an application defined function from the
// evaluation stack which starts at the current position
// and restack the resultant value.
//
// Note:
//   Stack Before : ^ <userfn> '(' <expression_list> ')'
//   Stack After  : ^ <value>

void
ClParserStackMgr::
unstackUserFunction(int *error_code)
{
  *error_code = 0;

  ClParserUserFnPtr userfn;

  pop(userfn);

  ClParserOperatorPtr   op;
  ClParserArgValueArray arg_value_list;

  bool in_brackets = true;

  // skip '('
  toNext();

  while (in_brackets) {
    unstackExpression(error_code);

    if (*error_code != 0 && *error_code != int(ClErr::NULL_EXPRESSION))
      return;

    if      (*error_code == 0) {
      ClParserArgValue *arg_value = unstackArgValue(error_code);

      if (*error_code != 0)
        return;

      arg_value_list.push_back(*arg_value);

      pop(op);

      if (op) {
        if      (op->isType(CL_PARSER_OP_CLOSE_R_BRACKET))
          in_brackets = false;
        else if (! op->isType(CL_PARSER_OP_COMMA))
          *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
      }
      else
        *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
    }
    else if (arg_value_list.empty()) {
      pop(op);

      if (op) {
        if (op->isType(CL_PARSER_OP_CLOSE_R_BRACKET)) {
          in_brackets = false;

          *error_code = 0;
        }
        else
          *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
      }
      else
        *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);
    }
    else
      *error_code = int(ClErr::INVALID_FN_ARG_SYNTAX);

    if (*error_code != 0)
      return;
  }

  toPrev();

  pop(op);

  uint num_arg_values = uint(arg_value_list.size());

  if (userfn->isVarArgs()) {
    if (userfn->getNumArgTypes() > 0 && num_arg_values < userfn->getNumArgTypes() - 1) {
      *error_code = int(ClErr::TOO_FEW_ARGUMENTS);
      return;
    }
  }
  else {
    if (userfn->getNumArgTypes() != num_arg_values) {
      if (num_arg_values < userfn->getNumArgTypes())
        *error_code = int(ClErr::TOO_FEW_ARGUMENTS);
      else
        *error_code = int(ClErr::TOO_MANY_ARGUMENTS);
      return;
    }
  }

  ClParserValuePtr value = ClParserProcessUserFn(userfn, arg_value_list, error_code);

  if (*error_code != 0 || ! value)
    return;

  push(value);
}

// Unstack a structure value reference from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note;
//   Stack Before : ^ <identifier> '.' <identifier>
//                : [ '.' <identifier> ]
//                : [ <array_expression> ]
//   Stack After  : ^ <lvalue>

void
ClParserStackMgr::
unstackStructure(int *error_code)
{
  *error_code = 0;

  ClParserIdentifierPtr identifier;

  pop(identifier);

  const std::string &name = identifier->getName();

  ClParserVarPtr variable = ClParserInst->getVariable(name);

  if (! variable) {
    ClParserStructPtr structure = ClParserStruct::createStruct();

    ClParserValuePtr value = ClParserValueMgrInst->createValue(structure);

    variable = ClParserInst->createVar(name, value);
  }

  if (! variable->getValue() ||
      ! (variable->getValue()->isStructure() || variable->getValue()->isStructureArray())) {
    *error_code = int(ClErr::INVALID_STRUCT_REF);
    return;
  }

  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isOperator()) {
    *error_code = int(ClErr::INVALID_STRUCT_REF);
    return;
  }

  ClParserVarRefPtr var_ref = ClParserInst->createVarRef(variable);

  if (stack_node->isOperator(CL_PARSER_OP_OPEN_S_BRACKET)) {
    unstackArrayValue(error_code);

    if (*error_code != 0)
      return;

    ClParserValuePtr subscript_value;

    if (! pop(subscript_value)) {
      *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
      return;
    }

    var_ref->addSubscript(subscript_value);

    toNext();

    stack_node = getCurrentStackNode();

    if (! stack_node || ! stack_node->isOperator()) {
      *error_code = int(ClErr::INVALID_STRUCT_REF);
      return;
    }
  }
  else {
    if (variable->getValue()->getType() != CL_PARSER_VALUE_TYPE_STRUCTURE) {
      *error_code = int(ClErr::INVALID_STRUCT_REF);
      return;
    }
  }

  ClParserOperatorPtr op;

  pop(op);

  ClParserStructVarRefPtr svar_ref;

  stack_node = getCurrentStackNode();

  while (stack_node && stack_node->isIdentifier() && stack_node->getIdentifier()->isStructPart()) {
    pop(identifier);

    const std::string &iname = identifier->getName();

    if (var_ref) {
      ClParserValuePtr svalue;

      if (! var_ref->getValue(svalue)) {
        *error_code = int(ClErr::INVALID_STRUCT_REF);
        return;
      }

      if (! svalue->isStructure()) {
        *error_code = int(ClErr::INVALID_STRUCT_REF);
        return;
      }

      ClParserStructPtr structure = svalue->getStructure();

      ClParserValuePtr structure_value;

      if (! structure->getValue(iname, structure_value)) {
        if (structure->getIsFixedType()) {
          *error_code = int(ClErr::INVALID_STRUCT_REF);
          return;
        }

        structure->addName(iname);
      }

      svar_ref = ClParserInst->createStructVarRef(variable, iname);

      var_ref = ClParserVarRefPtr();
    }
    else
      svar_ref->addName(iname);

    toNext();

    stack_node = getCurrentStackNode();

    if (! stack_node || ! stack_node->isOperator()) {
      *error_code = int(ClErr::INVALID_STRUCT_REF);
      return;
    }

    if (stack_node->isOperator(CL_PARSER_OP_OPEN_S_BRACKET)) {
      unstackArrayValue(error_code);

      if (*error_code != 0)
        return;

      ClParserValuePtr subscript_value;

      if (! pop(subscript_value)) {
        *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
        return;
      }

      svar_ref->addSubscript(subscript_value);

      toNext();

      stack_node = getCurrentStackNode();

      if (! stack_node || ! stack_node->isOperator()) {
        *error_code = int(ClErr::INVALID_STRUCT_REF);
        return;
      }
    }
    else {
      if (variable->getValue()->getType() != CL_PARSER_VALUE_TYPE_STRUCTURE) {
        *error_code = int(ClErr::INVALID_STRUCT_REF);
        return;
      }
    }

    pop(op);

    toNext();

    stack_node = getCurrentStackNode();
  }

  if (! stack_node || ! stack_node->isIdentifier() ||
      ! stack_node->getIdentifier()->isVariable()) {
    *error_code = int(ClErr::INVALID_STRUCT_REF);
    return;
  }

  pop(identifier);

  const std::string &name1 = identifier->getName();

  if (var_ref) {
    ClParserValuePtr svalue;

    if (! var_ref->getValue(svalue)) {
      *error_code = int(ClErr::INVALID_STRUCT_REF);
      return;
    }

    if (! svalue->isStructure()) {
      *error_code = int(ClErr::INVALID_STRUCT_REF);
      return;
    }

    ClParserStructPtr structure = svalue->getStructure();

    ClParserValuePtr structure_value;

    if (! structure->getValue(name1, structure_value)) {
      if (structure->getIsFixedType()) {
        *error_code = int(ClErr::INVALID_STRUCT_REF);
        return;
      }

      structure->addName(name1);
    }

    svar_ref = ClParserInst->createStructVarRef(variable, name1);

    var_ref = ClParserVarRefPtr();
  }
  else
    svar_ref->addName(name1);

  stack_node = getCurrentStackNode();

  if (stack_node && stack_node->isOperator(CL_PARSER_OP_OPEN_S_BRACKET)) {
    unstackArrayValue(error_code);

    if (*error_code != 0)
      return;

    ClParserValuePtr subscript_value;

    if (! pop(subscript_value)) {
      *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
      return;
    }

    svar_ref->addSubscript(subscript_value);
  }

  if (! variable->isFixedValue())
    push(svar_ref);
  else {
    ClParserValuePtr value1;

    if (! svar_ref->getValue(value1)) {
      *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
      return;
    }

    push(value1);
  }
}

// Unstack a variable value reference from the evaluation
// stack which starts at the current position and
// restack the resultant value.
//
// Note:
//   Stack Before : ^ <identifier> [ <array_expression> ]
//   Stack After  : <value> ^

void
ClParserStackMgr::
unstackVariable(int *error_code)
{
  *error_code = 0;

  ClParserIdentifierPtr identifier;

  pop(identifier);

  const std::string &name  = identifier->getName();
  ClParserScopePtr   scope = identifier->getScope();

  ClParserVarPtr variable;

  if (scope)
    variable = scope->getVariable(name);
  else
    variable = ClParserInst->getVariable(name);

  ClParserVarRefPtr var_ref;

  ClParserStackNode *stack_node = getCurrentStackNode();

  if (stack_node && stack_node->isOperator(CL_PARSER_OP_OPEN_S_BRACKET)) {
    ClParserValuePtr subscript_value;

    if (! variable || ! variable->getValue()) {
      *error_code = int(ClErr::UNDEFINED_VARIABLE);
      return;
    }

    unstackArrayValue(error_code);

    if (*error_code != 0)
      return;

    if (! pop(subscript_value)) {
      *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
      return;
    }

    var_ref = ClParserInst->createVarRef(variable);

    var_ref->addSubscript(subscript_value);
  }
  else {
    if (! variable) {
      if (scope)
        variable = scope->getVariable(name, true);
      else
        variable = ClParserInst->getVariable(name, true);

      variable->setValue(ClParserValueMgrInst->createValue(0L));
    }

    var_ref = ClParserInst->createVarRef(variable);
  }

  if (! variable->isFixedValue())
    push(var_ref);
  else {
    ClParserValuePtr value1;

    if (! var_ref->getValue(value1)) {
      *error_code = int(ClErr::UNDEFINED_SUBSCRIPT_VALUE);
      return;
    }

    push(value1);
  }
}

// Unstack a function call from the evaluation stack which
// starts at the current position and restack the resultant
// value.
//
// Note:
//   Stack Before : <identifier> ^ '(' <expression_list> ')'
//   Stack After  : <value> ^

void
ClParserStackMgr::
unstackFunction(int *error_code)
{
  *error_code = 0;

  ClParserIdentifierPtr identifier;

  pop(identifier);

  const std::string &name  = identifier->getName();
  ClParserScopePtr   scope = identifier->getScope();

  ClParserFuncPtr function;

  if (scope)
    function = scope->getFunction(name);
  else
    function = ClParserInst->getFunction(name);

  if (! function) {
    *error_code = int(ClErr::UNDEFINED_FUNCTION);
    return;
  }

  ClParserValueArray values;

  uint num_args = function->getNumArgs();

  toNext();

  ClParserOperatorPtr op;

  for (uint i = 0; i < num_args; ++i) {
    toNext();

    ClParserValuePtr value1 = unstackExpressionValue(error_code);

    if (*error_code != 0)
      return;

    values.push_back(value1);

    ClParserStackNode *stack_node = getCurrentStackNode();

    if (stack_node->isOperator(CL_PARSER_OP_CLOSE_R_BRACKET) && i < num_args - 1) {
      *error_code = int(ClErr::TOO_FEW_ARGUMENTS);
      return;
    }

    if (stack_node->isOperator(CL_PARSER_OP_COMMA) && i == num_args - 1) {
      *error_code = int(ClErr::TOO_MANY_ARGUMENTS);
      return;
    }

    toNext();

    pop(op);
  }

  pop(op);

  startTempStack();

  ClParserFuncValue function_value;

  function_value.function = function;
  function_value.values   = values;

  auto p1 = function->getStack()->stackNodeListBegin();
  auto p2 = function->getStack()->stackNodeListEnd  ();

  for ( ; p1 != p2; ++p1)
    stackFunctionStackNode(*p1, &function_value);

  ClParserValuePtr value = evaluateStack();

  endTempStack();

  if (! value) {
    *error_code = int(ClErr::UNDEFINED_FUNCTION);
    return;
  }

  push(value);
}

// Add the specified stack node to the evaluation stack
// replacing any identifiers which correspond to function
// argument names with the value supplied for that
// argument when the function was called.

void
ClParserStackMgr::
stackFunctionStackNode(ClParserStackNode *stack_node, ClParserFuncValue *function_value)
{
  ClParserValuePtr value;

  if (stack_node->isIdentifier() && stack_node->getIdentifier()->isArgument()) {
    uint num_args = function_value->function->getNumArgs();

    for (uint i = 0; i < num_args; ++i) {
      const std::string &arg = function_value->function->getArg(i);

      if (arg == stack_node->getIdentifier()->getName()) {
        value = ClParserValueMgrInst->createValue(function_value->values[i]);

        push(value);

        break;
      }
    }
  }
  else
    stack_node->stack();
}

// Unstack an Argument Value Structure which contains
// a Variable Reference or a Value.
//
// Note:
//   Stack Before : ^ <value_ref>

ClParserArgValue *
ClParserStackMgr::
unstackArgValue(int *error_code)
{
  ClParserValuePtr  value;
  ClParserVarRefPtr var_ref;

  if (! pop(var_ref) && ! pop(value)) {
    *error_code = int(ClErr::UNDEFINED_VALUE);
    return nullptr;
  }

  return new ClParserArgValue(var_ref, value);
}

// Unstack an expression from the evaluation stack
// which starts at the current position but do not
// evaluate it.
//
// Note:
//   Stack Before : ^ <expression>
//   Stack After  : ^

void
ClParserStackMgr::
skipExpression(int *error_code)
{
  *error_code = 0;

  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node) {
    *error_code = int(ClErr::NULL_EXPRESSION);
    return;
  }

  bool end_expression = false;

  while (! end_expression) {
    if      (stack_node->isOperator()) {
      ClParserOperatorPtr op = stack_node->getOperator();

      if      (op->isInline()) {
        popStackNode();
      }
      else if (op->isUnary()) {
        popStackNode();
      }
      else if (op->isBinary()) {
        popStackNode();
      }
      else if (op->isAssignment()) {
        popStackNode();
      }
      else if (op->isType(CL_PARSER_OP_OPEN_R_BRACKET)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;

        while (! stack_node->isOperator() ||
               ! stack_node->isOperator(CL_PARSER_OP_CLOSE_R_BRACKET)) {
          popStackNode();

          skipExpression(error_code);

          if (*error_code != 0)
            return;

          stack_node = getCurrentStackNode();

          if (! stack_node)
            return;
        }

        popStackNode();
      }
      else if (op->isType(CL_PARSER_OP_OPEN_S_BRACKET)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;

        while (! stack_node->isOperator() ||
               ! stack_node->isOperator(CL_PARSER_OP_CLOSE_S_BRACKET)) {
          popStackNode();

          skipExpression(error_code);

          if (*error_code != 0)
            return;

          stack_node = getCurrentStackNode();

          if (! stack_node)
            return;
        }

        popStackNode();
      }
      else if (op->isType(CL_PARSER_OP_OPEN_BRACE)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;

        while (! stack_node->isOperator() || ! stack_node->isOperator(CL_PARSER_OP_CLOSE_BRACE)) {
          popStackNode();

          skipExpression(error_code);

          if (*error_code != 0)
            return;

          stack_node = getCurrentStackNode();

          if (! stack_node)
            return;
        }

        popStackNode();
      }
      else if (op->isType(CL_PARSER_OP_OPEN_DICT)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;

        while (! stack_node->isOperator() || ! stack_node->isOperator(CL_PARSER_OP_CLOSE_DICT)) {
          popStackNode();

          skipExpression(error_code);

          if (*error_code != 0)
            return;

          stack_node = getCurrentStackNode();

          if (! stack_node)
            return;
        }

        popStackNode();
      }
      else if (op->isType(CL_PARSER_OP_QUESTION_MARK)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;

        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;
      }
      else if (op->isPunctuation())
        end_expression = true;
      else {
        *error_code = int(ClErr::INVALID_OPERATOR);

        return;
      }
    }
    else if (stack_node->isInternFn() || stack_node->isUserFn() || stack_node->isType()) {
      popStackNode();

      stack_node = getCurrentStackNode();

      if (! stack_node)
        return;

      popStackNode();

      skipExpression(error_code);

      if (*error_code != 0)
        return;

      stack_node = getCurrentStackNode();

      if (! stack_node)
        return;

      while (! stack_node->isOperator() || ! stack_node->isOperator(CL_PARSER_OP_CLOSE_R_BRACKET)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;
      }

      popStackNode();
    }
    else if (stack_node->isValue() || stack_node->isVarRef() || stack_node->isStructVarRef()) {
      popStackNode();
    }
    else if (stack_node->isIdentifier() && stack_node->getIdentifier()->isStructPart()) {
      do {
        popStackNode();

        popStackNode();

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;
      }
      while (stack_node->isIdentifier() && stack_node->getIdentifier()->isStructPart());

      popStackNode();

      stack_node = getCurrentStackNode();

      if (! stack_node)
        return;

      if (stack_node->isOperator() && stack_node->isOperator(CL_PARSER_OP_OPEN_S_BRACKET)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;

        while (! stack_node->isOperator() ||
               ! stack_node->isOperator(CL_PARSER_OP_CLOSE_S_BRACKET)) {
          popStackNode();

          skipExpression(error_code);

          if (*error_code != 0)
            return;

          stack_node = getCurrentStackNode();

          if (! stack_node)
            return;
        }

        popStackNode();
      }
    }
    else if (stack_node->isIdentifier() && stack_node->getIdentifier()->isVariable()) {
      popStackNode();

      stack_node = getCurrentStackNode();

      if (! stack_node)
        return;

      if (stack_node->isOperator() && stack_node->isOperator(CL_PARSER_OP_OPEN_S_BRACKET)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;

        while (! stack_node->isOperator() ||
               ! stack_node->isOperator(CL_PARSER_OP_CLOSE_S_BRACKET)) {
          popStackNode();

          skipExpression(error_code);

          if (*error_code != 0)
            return;

          stack_node = getCurrentStackNode();

          if (! stack_node)
            return;
        }

        popStackNode();
      }
    }
    else if (stack_node->isIdentifier() && stack_node->getIdentifier()->isFunction()) {
      popStackNode();

      stack_node = getCurrentStackNode();

      if (! stack_node)
        return;

      popStackNode();

      skipExpression(error_code);

      if (*error_code != 0)
        return;

      stack_node = getCurrentStackNode();

      if (! stack_node)
        return;

      while (! stack_node->isOperator() ||
             ! stack_node->isOperator(CL_PARSER_OP_CLOSE_R_BRACKET)) {
        popStackNode();

        skipExpression(error_code);

        if (*error_code != 0)
          return;

        stack_node = getCurrentStackNode();

        if (! stack_node)
          return;
      }

      popStackNode();
    }
    else {
      *error_code = int(ClErr::INVALID_CHARACTER);

      return;
    }

    stack_node = getCurrentStackNode();

    if (! stack_node)
      end_expression = true;
  }
}
// Check whether the preceding unary, binary or assignment
// operator should be unstacked because the current operator
// has a lower (or equal for left to right associating
// operators) precedence than the last operator.

bool
ClParserStackMgr::
checkUnstack(ClParserOperatorPtr op)
{
  ClParserOperatorPtr last_operator = getLastOperator();

  return op->checkUnstack(last_operator);
}

//---------------------------

void
ClParserStackMgr::
setCurrentStack(ClParserStackPtr stack)
{
  current_stack_ = stack;

  if (ClParserInst->isDebug())
    debugPrintCurrent();
}

void
ClParserStackMgr::
push(ClParserIdentifierPtr identifier)
{
  ClParserStackNode *stack_node = new ClParserStackNode(identifier);

  pushStackNode(stack_node);
}

void
ClParserStackMgr::
push(ClParserInternFnPtr internfn)
{
  ClParserStackNode *stack_node = new ClParserStackNode(internfn);

  pushStackNode(stack_node);
}

void
ClParserStackMgr::
push(ClParserOperatorType type)
{
  ClParserOperatorPtr op = ClParserOperatorMgrInst->getOperator(type);

  push(op);
}

void
ClParserStackMgr::
push(ClParserOperatorPtr op)
{
  ClParserStackNode *stack_node = new ClParserStackNode(op);

  pushStackNode(stack_node);
}

void
ClParserStackMgr::
push(ClParserTypePtr type)
{
  ClParserStackNode *stack_node = new ClParserStackNode(type);

  pushStackNode(stack_node);
}

void
ClParserStackMgr::
push(ClParserUserFnPtr userfn)
{
  ClParserStackNode *stack_node = new ClParserStackNode(userfn);

  pushStackNode(stack_node);
}

void
ClParserStackMgr::
push(ClParserValuePtr value)
{
  ClParserStackNode *stack_node = new ClParserStackNode(value);

  pushStackNode(stack_node);
}

void
ClParserStackMgr::
push(ClParserVarRefPtr var_ref)
{
  ClParserStackNode *stack_node = new ClParserStackNode(var_ref);

  pushStackNode(stack_node);
}

void
ClParserStackMgr::
push(ClParserStructVarRefPtr svar_ref)
{
  ClParserStackNode *stack_node = new ClParserStackNode(svar_ref);

  pushStackNode(stack_node);
}

bool
ClParserStackMgr::
pop(ClParserIdentifierPtr &identifier)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isIdentifier())
    return false;

  identifier = stack_node->getIdentifier();

  popStackNode();

  return true;
}

bool
ClParserStackMgr::
pop(ClParserInternFnPtr &internfn)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isInternFn())
    return false;

  internfn = stack_node->getInternFn();

  popStackNode();

  return true;
}

bool
ClParserStackMgr::
pop(ClParserOperatorPtr &op)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isOperator())
    return false;

  op = stack_node->getOperator();

  popStackNode();

  return true;
}

bool
ClParserStackMgr::
pop(ClParserTypePtr &type)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isType())
    return false;

  type = stack_node->getType();

  popStackNode();

  return true;
}

bool
ClParserStackMgr::
pop(ClParserUserFnPtr &userfn)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isUserFn())
    return false;

  userfn = stack_node->getUserFn();

  popStackNode();

  return true;
}

bool
ClParserStackMgr::
pop(ClParserValuePtr &value)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node)
    return false;

  if      (stack_node->isValue()) {
    value = stack_node->getValue();

    popStackNode();
  }
  else if (stack_node->isVarRef()) {
    ClParserVarRefPtr var_ref = stack_node->getVarRef();

    if (! var_ref->getValue(value))
      return false;

    popStackNode();

    if (! value)
      return false;
  }
  else if (stack_node->isStructVarRef()) {
    ClParserStructVarRefPtr svar_ref = stack_node->getStructVarRef();

    if (! svar_ref->getValue(value))
      return false;

    popStackNode();

    if (! value)
      return false;
  }
  else
    return false;

  return true;
}

bool
ClParserStackMgr::
pop(ClParserVarRefPtr &var_ref)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isVarRef())
    return false;

  var_ref = stack_node->getVarRef();

  popStackNode();

  return true;
}

bool
ClParserStackMgr::
pop(ClParserStructVarRefPtr &svar_ref)
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (! stack_node || ! stack_node->isStructVarRef())
    return false;

  svar_ref = stack_node->getStructVarRef();

  popStackNode();

  return true;
}

void
ClParserStackMgr::
pushStackNode(ClParserStackNode *stack_node)
{
  current_stack_->pushStackNode(stack_node);
}

void
ClParserStackMgr::
popStackNode()
{
  current_stack_->popStackNode();
}

void
ClParserStackMgr::
debugPrintCurrent()
{
  current_stack_->debugPrintCurrent();
}

void
ClParserStackMgr::
restartAll()
{
  stack_stack_.clear();

  current_stack_->restart(true);
}

void
ClParserStackMgr::
restart()
{
  current_stack_->restart(false);
}

ClParserStackPtr
ClParserStackMgr::
startStack()
{
  if (current_stack_)
    stack_stack_.push_back(current_stack_);

  ClParserStack *stack = new ClParserStack;

  current_stack_ = ClParserStackPtr(stack);

  return current_stack_;
}

ClParserStackPtr
ClParserStackMgr::
endStack()
{
  ClParserStackPtr old_stack = current_stack_;

  current_stack_ = stack_stack_.back();

  stack_stack_.pop_back();

  return old_stack;
}

ClParserStackPtr
ClParserStackMgr::
startTempStack()
{
  return startStack();
}

ClParserStackPtr
ClParserStackMgr::
endTempStack()
{
  return endStack();
}

void
ClParserStackMgr::
toStart()
{
  current_stack_->toStart();
}

void
ClParserStackMgr::
toEnd()
{
  current_stack_->toEnd();
}

void
ClParserStackMgr::
setInBracket(bool flag)
{
  current_stack_->setInBracket(flag);
}

bool
ClParserStackMgr::
getInBracket()
{
  return current_stack_->getInBracket();
}

void
ClParserStackMgr::
setRestart()
{
  current_stack_->setRestart();
}

bool
ClParserStackMgr::
getRestart()
{
  return current_stack_->getRestart();
}

ClParserStackNode *
ClParserStackMgr::
getCurrentStackNode() const
{
  return current_stack_->getCurrentStackNodeP();
}

void
ClParserStackMgr::
removeCurrentStackNode()
{
  ClParserStackNode *stack_node = getCurrentStackNode();

  if (stack_node)
    popStackNode();
}

bool
ClParserStackMgr::
toNext()
{
  return current_stack_->toNext();
}

bool
ClParserStackMgr::
toPrev()
{
  return current_stack_->toPrev();
}

void
ClParserStackMgr::
setLastOperator()
{
  current_stack_->setLastOperator();
}

ClParserOperatorPtr
ClParserStackMgr::
getLastOperator()
{
  setLastOperator();

  return current_stack_->getLastOperator();
}

//---------------------------

ClParserStack::
ClParserStack() :
 restart_(false), in_bracket_(false)
{
  current_stack_node_ = stack_node_list_.begin();
}

ClParserStack::
ClParserStack(const ClParserStack &stack) :
 restart_(stack.restart_), in_bracket_(stack.in_bracket_)
{
  current_stack_node_ = stack_node_list_.begin();
}

ClParserStack::
~ClParserStack()
{
  StackNodeList::iterator p1 = stack_node_list_.begin();
  StackNodeList::iterator p2 = stack_node_list_.end  ();

  for ( ; p1 != p2; ++p1)
    delete *p1;
}

ClParserStack *
ClParserStack::
dup() const
{
  return new ClParserStack(*this);
}

void
ClParserStack::
setLastOperator()
{
  last_operator_ = ClParserOperatorPtr();

  auto p1 = stack_node_list_.begin();
  auto p2 = stack_node_list_.end  ();

  for ( ; p1 != p2; ++p1) {
    if (current_stack_node_ == p1)
      break;

    ClParserStackNode *stack_node = *p1;

    if (stack_node->isOperator()) {
      ClParserOperatorPtr op = stack_node->getOperator();

      if (op->isPunctuation())
        last_operator_ = ClParserOperatorPtr();
      else
        last_operator_ = op;
    }
  }
}

void
ClParserStack::
restart(bool restart)
{
  StackNodeList::iterator p1 = stack_node_list_.begin();
  StackNodeList::iterator p2 = stack_node_list_.end  ();

  for ( ; p1 != p2; ++p1)
    delete *p1;

  stack_node_list_.clear();

  current_stack_node_ = stack_node_list_.begin();

  restart_       = restart;
  in_bracket_    = false;
  last_operator_ = ClParserOperatorPtr();
}

void
ClParserStack::
pushStackNode(ClParserStackNode *stack_node)
{
  current_stack_node_ = stack_node_list_.insert(current_stack_node_, stack_node);

  if (ClParserInst->isDebug())
    debugPrintCurrent();
}

void
ClParserStack::
popStackNode()
{
  if (current_stack_node_ != stack_node_list_.end()) {
    ClParserStackNode *stack_node = *current_stack_node_;

    current_stack_node_ = stack_node_list_.erase(current_stack_node_);

    delete stack_node;
  }

  if (ClParserInst->isDebug())
    debugPrintCurrent();
}

void
ClParserStack::
toStart()
{
  in_bracket_    = false;
  restart_       = false;
  last_operator_ = ClParserOperatorPtr();

  current_stack_node_ = stack_node_list_.begin();
}

void
ClParserStack::
toEnd()
{
  in_bracket_ = false;
  restart_    = false;

  current_stack_node_ = stack_node_list_.end();
}

bool
ClParserStack::
toNext()
{
  bool rc = (current_stack_node_ != stack_node_list_.end());

  if (rc)
    ++current_stack_node_;

  return rc;
}

bool
ClParserStack::
toPrev()
{
  bool rc = (current_stack_node_ != stack_node_list_.begin());

  if (rc)
    --current_stack_node_;

  return rc;
}

ClParserStackNode *
ClParserStack::
getCurrentStackNodeP() const
{
  if (current_stack_node_ == stack_node_list_.end())
    return nullptr;

  return *current_stack_node_;
}

void
ClParserStack::
print() const
{
  auto p1 = stack_node_list_.begin();
  auto p2 = stack_node_list_.end  ();

  for ( ; p1 != p2; ++p1)
    (*p1)->print();
}

void
ClParserStack::
print(std::ostream &os) const
{
  auto p1 = stack_node_list_.begin();
  auto p2 = stack_node_list_.end  ();

  for ( ; p1 != p2; ++p1)
    (*p1)->print(os);
}

// Print the contents of the specified Stack Node structure for Debug
// highlighting the current Stack Node

void
ClParserStack::
debugPrintCurrent() const
{
  fprintf(stderr, "Stack      : >");

  auto p1 = stack_node_list_.begin();
  auto p2 = stack_node_list_.end  ();

  std::string marker = "\u00ab";
  //std::string marker = "#";

  for ( ; p1 != p2; ++p1) {
    if (current_stack_node_ == p1)
      fprintf(stderr, "%s", marker.c_str());

    (*p1)->debugPrint();
  }

  if (current_stack_node_ == stack_node_list_.end())
    fprintf(stderr, "%s", marker.c_str());

  fprintf(stderr, "<\n");
}

// Print the contents of the specified Stack Node structure for Debug

void
ClParserStack::
debugPrint() const
{
  auto p1 = stack_node_list_.begin();
  auto p2 = stack_node_list_.end  ();

  for ( ; p1 != p2; ++p1)
    (*p1)->debugPrint();
}

//-----------------

ClParserStackNode::
ClParserStackNode() :
 type_(ClParserStackNodeType::NONE)
{
}

ClParserStackNode::
ClParserStackNode(ClParserOperatorPtr op) :
 type_(ClParserStackNodeType::OPERATOR)
{
  data_.op = new ClParserOperatorPtr;

  *data_.op = op;
}

ClParserStackNode::
ClParserStackNode(ClParserInternFnPtr internfn) :
 type_(ClParserStackNodeType::INTERN_FN)
{
  data_.internfn = new ClParserInternFnPtr;

  *data_.internfn = internfn;
}

ClParserStackNode::
ClParserStackNode(ClParserValuePtr value) :
 type_(ClParserStackNodeType::VALUE)
{
  data_.value = new ClParserValuePtr;

  *data_.value = value;
}

ClParserStackNode::
ClParserStackNode(ClParserVarRefPtr var_ref) :
 type_(ClParserStackNodeType::VAR_REF)
{
  data_.var_ref = new ClParserVarRefPtr;

  *data_.var_ref = var_ref;
}

ClParserStackNode::
ClParserStackNode(ClParserStructVarRefPtr svar_ref) :
 type_(ClParserStackNodeType::STRUCT_VAR_REF)
{
  data_.svar_ref = new ClParserStructVarRefPtr;

  *data_.svar_ref = svar_ref;
}

ClParserStackNode::
ClParserStackNode(ClParserIdentifierPtr identifier) :
 type_(ClParserStackNodeType::IDENTIFIER)
{
  data_.identifier = new ClParserIdentifierPtr;

  *data_.identifier = identifier;
}

ClParserStackNode::
ClParserStackNode(ClParserUserFnPtr userfn) :
 type_(ClParserStackNodeType::USER_FN)
{
  data_.userfn = new ClParserUserFnPtr;

  *data_.userfn = userfn;
}

ClParserStackNode::
ClParserStackNode(ClParserTypePtr type) :
 type_(ClParserStackNodeType::TYPE)
{
  data_.type = new ClParserTypePtr;

  *data_.type = type;
}

ClParserStackNode::
~ClParserStackNode()
{
  switch (type_) {
    case ClParserStackNodeType::OPERATOR      : delete data_.op        ; break;
    case ClParserStackNodeType::INTERN_FN     : delete data_.internfn  ; break;
    case ClParserStackNodeType::VALUE         : delete data_.value     ; break;
    case ClParserStackNodeType::VAR_REF       : delete data_.var_ref   ; break;
    case ClParserStackNodeType::STRUCT_VAR_REF: delete data_.svar_ref  ; break;
    case ClParserStackNodeType::IDENTIFIER    : delete data_.identifier; break;
    case ClParserStackNodeType::USER_FN       : delete data_.userfn    ; break;
    case ClParserStackNodeType::TYPE          : delete data_.type      ; break;
    default: break;
  }
}

bool
ClParserStackNode::
isOperator(ClParserOperatorType type)
{
  return (type_ == ClParserStackNodeType::OPERATOR && (*data_.op)->isType(type));
}

void
ClParserStackNode::
stack()
{
  if      (type_ == ClParserStackNodeType::OPERATOR)
    ClParserStackMgrInst->push(*data_.op);
  else if (type_ == ClParserStackNodeType::INTERN_FN)
    ClParserStackMgrInst->push(*data_.internfn);
  else if (type_ == ClParserStackNodeType::USER_FN)
    ClParserStackMgrInst->push(*data_.userfn);
  else if (type_ == ClParserStackNodeType::VALUE)
    ClParserStackMgrInst->push(*data_.value);
  else if (type_ == ClParserStackNodeType::VAR_REF)
    ClParserStackMgrInst->push(*data_.var_ref);
  else if (type_ == ClParserStackNodeType::STRUCT_VAR_REF)
    ClParserStackMgrInst->push(*data_.svar_ref);
  else if (type_ == ClParserStackNodeType::IDENTIFIER)
    ClParserStackMgrInst->push(*data_.identifier);
  else if (type_ == ClParserStackNodeType::TYPE)
    ClParserStackMgrInst->push(*data_.type);
  else
    assert(false);
}

void
ClParserStackNode::
print() const
{
  switch (type_) {
    case ClParserStackNodeType::OPERATOR      : (*data_.op        )->print(); break;
    case ClParserStackNodeType::INTERN_FN     : (*data_.internfn  )->print(); break;
    case ClParserStackNodeType::USER_FN       : (*data_.userfn    )->print(); break;
    case ClParserStackNodeType::VALUE         : (*data_.value     )->print(); break;
    case ClParserStackNodeType::VAR_REF       : (*data_.var_ref   )->print(); break;
    case ClParserStackNodeType::STRUCT_VAR_REF: (*data_.svar_ref  )->print(); break;
    case ClParserStackNodeType::IDENTIFIER    : (*data_.identifier)->print(); break;
    case ClParserStackNodeType::TYPE          : (*data_.type      )->print(); break;
    default: break;
  }
}

void
ClParserStackNode::
print(std::ostream &os) const
{
  switch (type_) {
    case ClParserStackNodeType::OPERATOR      : (*data_.op        )->print(os); break;
    case ClParserStackNodeType::INTERN_FN     : (*data_.internfn  )->print(os); break;
    case ClParserStackNodeType::USER_FN       : (*data_.userfn    )->print(os); break;
    case ClParserStackNodeType::VALUE         : (*data_.value     )->print(os); break;
    case ClParserStackNodeType::VAR_REF       : (*data_.var_ref   )->print(os); break;
    case ClParserStackNodeType::STRUCT_VAR_REF: (*data_.svar_ref  )->print(os); break;
    case ClParserStackNodeType::IDENTIFIER    : (*data_.identifier)->print(os); break;
    case ClParserStackNodeType::TYPE          : (*data_.type      )->print(os); break;
    default: break;
  }
}

void
ClParserStackNode::
debugPrint() const
{
  switch (type_) {
    case ClParserStackNodeType::OPERATOR      : (*data_.op        )->debugPrint(); break;
    case ClParserStackNodeType::INTERN_FN     : (*data_.internfn  )->debugPrint(); break;
    case ClParserStackNodeType::USER_FN       : (*data_.userfn    )->debugPrint(); break;
    case ClParserStackNodeType::VALUE         : (*data_.value     )->debugPrint(); break;
    case ClParserStackNodeType::VAR_REF       : (*data_.var_ref   )->debugPrint(); break;
    case ClParserStackNodeType::STRUCT_VAR_REF: (*data_.svar_ref  )->debugPrint(); break;
    case ClParserStackNodeType::IDENTIFIER    : (*data_.identifier)->debugPrint(); break;
    case ClParserStackNodeType::TYPE          : (*data_.type      )->debugPrint(); break;
    default: break;
  }
}

void
ClParserStack::
getVariables(char ***variables, uint *num_variables)
{
  *variables     = nullptr;
  *num_variables = 0;

  StringVectorT variable_list;

  auto p1 = stack_node_list_.begin();
  auto p2 = stack_node_list_.end  ();

  for ( ; p1 != p2; ++p1) {
    ClParserStackNode *stack_node = *p1;

    if (stack_node->isIdentifier()) {
      ClParserIdentifierPtr identifier = stack_node->getIdentifier();

      if (identifier->isVariable())
        variable_list.push_back(identifier->getName());
    }
  }

  *num_variables = uint(variable_list.size());

  if (*num_variables > 0) {
    *variables = new char * [*num_variables];

    for (uint i = 0; i < *num_variables; ++i) {
      std::string variable = variable_list[i];

      (*variables)[i] = CStrUtil::strdup(variable.c_str());
    }
  }
}

void
ClParserStack::
freeVariables(char **variables, uint num_variables)
{
  for (uint i = 0; i < num_variables; ++i)
    delete [] variables[i];

  delete [] variables;
}

//--------

ClParserStackAutoDebugPrint::
~ClParserStackAutoDebugPrint()
{
  if (ClParserInst->isDebug())
    ClParserStackMgrInst->debugPrintCurrent();
}
