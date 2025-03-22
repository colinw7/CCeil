#include <CCeilPI.h>

static ClParserValuePtr ClParserProcessArrayUserFn
                         (ClParserUserFnPtr, const ClParserArgValueArray &, int *);
static ClParserValuePtr ClParserProcessStructureUserFn
                         (ClParserUserFnPtr, const ClParserArgValueArray &, int *);

// Apply inline operator to variable's value and return the result.
//
// Note: For prefix the value returned is the processed value,
// for postfix the value returned is the original value.
// In both case the variable's value is set to the processed value.

ClParserValuePtr
ClParserProcessInlineOperator(ClParserOperatorPtr op, ClParserVarRefPtr var_ref,
                              int postfix, int *error_code)
{
  ClParserStackAutoDebugPrint auto_print;

  CL_PARSER_TRACE("ClParserProcessInlineOperator")

  *error_code = 0;

  if (! op || ! var_ref) {
    ClParserInst->signalError(error_code, ClErr::INVALID_UNARY_EXPRESSION);
    return ClParserValuePtr();
  }

  ClParserValuePtr value;

  if (! var_ref->getValue(value)) {
    ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
    return ClParserValuePtr();
  }

  ClParserValuePtr value2;

  if (postfix)
    value2 = value;

  ClParserValuePtr value1 = ClParserSubProcessInlineOperator(op, value, error_code);

  if (*error_code != 0)
    return ClParserValuePtr();

  if (! var_ref->setValue(value1))
    return ClParserValuePtr();

  if (! postfix)
    value2 = value1;

  return value2;
}

// Apply inline operator to value and return the result.
//
// Note: Called by ClParserProcessInlineOperator to actual
// process the value.

ClParserValuePtr
ClParserSubProcessInlineOperator(ClParserOperatorPtr op, ClParserValuePtr value, int *error_code)
{
  *error_code = 0;

  ClParserValuePtr value1;

  if (! value)
    value1 = ClParserValueMgrInst->createValue(0L);
  else
    value1 = value;

  try {
    switch (op->getType()) {
      case CL_PARSER_OP_INCREMENT:
        value1->increment();
        break;
      case CL_PARSER_OP_DECREMENT:
        value1->decrement();
        break;
      default:
        assert(false);
        break;
    }
  }
  catch (...) {
    ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
    return ClParserValuePtr();
  }

  return value1;
}

// Apply unary operator to value and return the result.

ClParserValuePtr
ClParserProcessUnaryOperator(ClParserOperatorPtr op, ClParserValuePtr value, int *error_code)
{
  ClParserStackAutoDebugPrint auto_print;

  CL_PARSER_TRACE("ClParserProcessUnaryOperator")

  *error_code = 0;

  /*------------------*/

  if (! op || ! value) {
    ClParserInst->signalError(error_code, ClErr::INVALID_UNARY_EXPRESSION);
    return ClParserValuePtr();
  }

  /*------------------*/

  if (op->isType(CL_PARSER_OP_LOGICAL_NOT))
    return ClParserValueMgrInst->createValue(long(! value->toBool()));

  /*------------------*/

  ClParserValuePtr value1;

  try {
    switch (op->getType()) {
      case CL_PARSER_OP_UNARY_PLUS:
        value1 = value->unaryPlus();
        break;
      case CL_PARSER_OP_UNARY_MINUS:
        value1 = value->unaryMinus();
        break;
      case CL_PARSER_OP_BIT_NOT:
        value1 = value->bitNot();
        break;
      default:
        assert(false);
        break;
    }
  }
  catch (...) {
    ClParserInst->signalError(error_code, ClErr::INVALID_UNARY_EXPRESSION);
    return ClParserValuePtr();
  }

  return value1;
}

// Apply binary operator to the two values and return the result.

ClParserValuePtr
ClParserProcessBinaryOperator(ClParserValuePtr rvalue1, ClParserOperatorPtr op,
                              ClParserValuePtr rvalue2, int *error_code)
{
  ClParserValuePtr value1 = rvalue1;
  ClParserValuePtr value2 = rvalue2;

  ClParserStackAutoDebugPrint auto_print;

  CL_PARSER_TRACE("ClParserProcessBinaryOperator")

  *error_code = 0;

  /*-----------------*/

  if (! value1 || ! op || ! value2) {
    ClParserInst->signalError(error_code, ClErr::INVALID_BINARY_EXPRESSION);
    return ClParserValuePtr();
  }

  /*-----------------*/

  if (op->isType(CL_PARSER_OP_LOGICAL_AND) ||
      op->isType(CL_PARSER_OP_LOGICAL_OR)) {
    bool boolean1 = value1->toBool();
    bool boolean2 = value2->toBool();

    if (op->isType(CL_PARSER_OP_LOGICAL_AND))
      return ClParserValueMgrInst->createValue(long(boolean1 && boolean2));
    else
      return ClParserValueMgrInst->createValue(long(boolean1 || boolean2));
  }

  /*-----------------*/

  if (op->getType() >= CL_PARSER_OP_LESS &&
      op->getType() <= CL_PARSER_OP_NOT_EQUAL) {
    if (! ClParserValue::checkBinaryTypes(value1, value2)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_MIX);
      return ClParserValuePtr();
    }

    int cmp = value1->cmp(value2);

    switch (op->getType()) {
      case CL_PARSER_OP_LESS:
        return ClParserValueMgrInst->createValue(long(cmp <  0));
      case CL_PARSER_OP_LESS_OR_EQUAL:
        return ClParserValueMgrInst->createValue(long(cmp <= 0));
      case CL_PARSER_OP_GREATER:
        return ClParserValueMgrInst->createValue(long(cmp >  0));
      case CL_PARSER_OP_GREATER_OR_EQUAL:
        return ClParserValueMgrInst->createValue(long(cmp >= 0));
      case CL_PARSER_OP_EQUAL:
        return ClParserValueMgrInst->createValue(long(cmp == 0));
      case CL_PARSER_OP_NOT_EQUAL:
        return ClParserValueMgrInst->createValue(long(cmp != 0));
      default:
        assert(false);
        return ClParserValuePtr();
    }
  }

  /*-----------------*/

  if (op->isType(CL_PARSER_OP_ARRAY_TIMES)) {
    if (! value1->isType(CL_PARSER_VALUE_TYPE_ARRAY) ||
        ! value2->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_MIX);
      return ClParserValuePtr();
    }

    ClParserArrayPtr array1 = value1->getArray();
    ClParserArrayPtr array2 = value2->getArray();

    ClParserArrayPtr array = array1->arrayTimes(*array2);

    return ClParserValueMgrInst->createValue(array);
  }

  /*-----------------*/

  if (value1->getType() != value2->getType()) {
    if (op->getType() == CL_PARSER_OP_TIMES &&
        value1->isType(CL_PARSER_VALUE_TYPE_INTEGER))
      return ClParserProcessBinaryOperator(value2, op, value1, error_code);
    else {
      bool fix_list = false;

      if (value1->isType(CL_PARSER_VALUE_TYPE_LIST) ||
          value2->isType(CL_PARSER_VALUE_TYPE_LIST))
        fix_list = true;

      if (value1->isType(CL_PARSER_VALUE_TYPE_LIST) &&
          (op->getType() == CL_PARSER_OP_TIMES ||
           op->getType() == CL_PARSER_OP_DIVIDE))
        fix_list = false;

      if (fix_list) {
        ClParserValuePtr value3 = ClParserValueMgrInst->createValue(CL_PARSER_VALUE_TYPE_LIST);

        if (value1->isType(CL_PARSER_VALUE_TYPE_LIST)) {
          value3->getList()->addValue(value2);

          return ClParserProcessBinaryOperator(value1, op, value3, error_code);
        }
        else {
          value3->getList()->addValue(value1);

          return ClParserProcessBinaryOperator(value3, op, value2, error_code);
        }
      }
    }

    /*-----------------*/

    if (value1->isNumeric() && value2->isNumeric()) {
      if (! ClParserValue::checkBinaryTypes(value1, value2)) {
        ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_MIX);
        return ClParserValuePtr();
      }
    }
  }

  /*-----------------*/

  if (value1->isType(CL_PARSER_VALUE_TYPE_ARRAY) &&
      value2->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
    if (! ClParserValue::checkBinaryTypes(value1, value2)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_MIX);
      return ClParserValuePtr();
    }
  }

  /*-----------------*/

  ClParserValuePtr value = ClParserValueMgrInst->createValue(value1);

  try {
    switch (op->getType()) {
      case CL_PARSER_OP_PLUS        : return value1->plus(value2);
      case CL_PARSER_OP_MINUS       : return value1->minus(value2);
      case CL_PARSER_OP_TIMES       : return value1->times(value2);
      case CL_PARSER_OP_DIVIDE      : return value1->divide(value2);
      case CL_PARSER_OP_MODULUS     : return value1->modulus(value2);
      case CL_PARSER_OP_POWER       : return value1->power(value2);
      case CL_PARSER_OP_APPROX_EQUAL: return value1->approxEqual(value2);
      case CL_PARSER_OP_BIT_AND     : return value1->bitAnd(value2);
      case CL_PARSER_OP_BIT_OR      : return value1->bitOr(value2);
      case CL_PARSER_OP_BIT_XOR     : return value1->bitXor(value2);
      case CL_PARSER_OP_BIT_LSHIFT  : return value1->bitLShift(value2);
      case CL_PARSER_OP_BIT_RSHIFT  : return value1->bitRShift(value2);

      default: assert(false); return ClParserValuePtr();
    }
  }
  catch (...) {
    ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
    return ClParserValuePtr();
  }
}

// Apply assignment operator to the two values and return the result.
void
ClParserProcessAssignmentOperator(ClParserVarRefPtr var_ref, ClParserOperatorPtr op,
                                  ClParserValuePtr value, int *error_code)
{
  ClParserStackAutoDebugPrint auto_print;

  CL_PARSER_TRACE("ClParserProcessAssignmentOperator")

  *error_code = 0;

  if (! value) {
    ClParserInst->signalError(error_code, ClErr::INVALID_RHS_FOR_ASSIGNMENT);
    return;
  }

  if (var_ref->isFixedType()) {
    ClParserValuePtr sub_value;

    if (! var_ref->getValue(sub_value)) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
      return;
    }

    ClParserCheckUnaryValue(value, sub_value->getType(), error_code);

    if (*error_code != 0)
      return;
  }

  ClParserOperatorPtr operator1;

  if      (op->isType(CL_PARSER_OP_PLUS_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_PLUS);
  else if (op->isType(CL_PARSER_OP_MINUS_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_MINUS);
  else if (op->isType(CL_PARSER_OP_TIMES_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_TIMES);
  else if (op->isType(CL_PARSER_OP_DIVIDE_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_DIVIDE);
  else if (op->isType(CL_PARSER_OP_MODULUS_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_MODULUS);
  else if (op->isType(CL_PARSER_OP_BIT_AND_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_AND);
  else if (op->isType(CL_PARSER_OP_BIT_OR_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_OR);
  else if (op->isType(CL_PARSER_OP_BIT_XOR_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_XOR);
  else if (op->isType(CL_PARSER_OP_BIT_LSHIFT_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_LSHIFT);
  else if (op->isType(CL_PARSER_OP_BIT_RSHIFT_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_RSHIFT);
  else
    operator1 = ClParserOperatorPtr();

  ClParserValuePtr value1;

  if (operator1) {
    ClParserValuePtr sub_value;

    if (! var_ref->getValue(sub_value)) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
      return;
    }

    value1 = ClParserProcessBinaryOperator
               (sub_value, operator1, value, error_code);

    if (*error_code != 0)
      return;
  }
  else
    value1 = value;

  if (! var_ref->setValue(value1)) {
    ClParserInst->signalError(error_code, ClErr::UNDEFINED_ASSIGN_VALUE);
    return;
  }

  var_ref->callVariableProc();
}

// Apply assignment operator to the two values and return the result.
void
ClParserProcessAssignmentOperator(ClParserStructVarRefPtr svar_ref, ClParserOperatorPtr op,
                                  ClParserValuePtr value, int *error_code)
{
  ClParserStackAutoDebugPrint auto_print;

  CL_PARSER_TRACE("ClParserProcessAssignmentOperator")

  *error_code = 0;

  if (! value) {
    ClParserInst->signalError(error_code, ClErr::INVALID_RHS_FOR_ASSIGNMENT);
    return;
  }

  if (svar_ref->isFixedType()) {
    ClParserValuePtr sub_value;

    if (! svar_ref->getValue(sub_value)) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
      return;
    }

    ClParserCheckUnaryValue(value, sub_value->getType(), error_code);

    if (*error_code != 0)
      return;
  }

  ClParserOperatorPtr operator1;

  if      (op->isType(CL_PARSER_OP_PLUS_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_PLUS);
  else if (op->isType(CL_PARSER_OP_MINUS_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_MINUS);
  else if (op->isType(CL_PARSER_OP_TIMES_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_TIMES);
  else if (op->isType(CL_PARSER_OP_DIVIDE_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_DIVIDE);
  else if (op->isType(CL_PARSER_OP_MODULUS_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_MODULUS);
  else if (op->isType(CL_PARSER_OP_BIT_AND_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_AND);
  else if (op->isType(CL_PARSER_OP_BIT_OR_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_OR);
  else if (op->isType(CL_PARSER_OP_BIT_XOR_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_XOR);
  else if (op->isType(CL_PARSER_OP_BIT_LSHIFT_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_LSHIFT);
  else if (op->isType(CL_PARSER_OP_BIT_RSHIFT_EQUALS))
    operator1 = ClParserOperatorMgrInst->getOperator(CL_PARSER_OP_BIT_RSHIFT);
  else
    operator1 = ClParserOperatorPtr();

  ClParserValuePtr value1;

  if (operator1) {
    ClParserValuePtr sub_value;

    if (! svar_ref->getValue(sub_value)) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
      return;
    }

    value1 = ClParserProcessBinaryOperator
               (sub_value, operator1, value, error_code);

    if (*error_code != 0)
      return;
  }
  else
    value1 = value;

  if (! svar_ref->setValue(value1)) {
    ClParserInst->signalError(error_code, ClErr::UNDEFINED_ASSIGN_VALUE);
    return;
  }

  svar_ref->callVariableProc();
}

// Execute an internal function with the supplied array of values.
ClParserValuePtr
ClParserProcessInternFn(ClParserInternFnPtr internfn, const ClParserValueArray &values,
                        int *error_code)
{
  ClParserStackAutoDebugPrint auto_print;

  CL_PARSER_TRACE("ClParserProcessInternFn")

  *error_code = 0;

  uint num_values = uint(values.size());

  ClParserValuePtr value = ClParserValueMgrInst->createValue(values[0]);

  try {
    switch (internfn->getType()) {
      case CLParserInternFnType::VALTYP:
        if (value->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
          ClParserArrayPtr array = value->getArray();

          value = ClParserValueMgrInst->createValue(
                    long(array->getType() | CL_PARSER_VALUE_TYPE_ARRAY));
        }
        else
          value = ClParserValueMgrInst->createValue(long(value->getType()));

        break;

      case CLParserInternFnType::ADDR: {
        ClParserValuePtr value1 = ClParserProcessAddrCommand(values, error_code);

        if (*error_code == 0)
          value = value1;

        break;
      }

      //------

      case CLParserInternFnType::IS_CTYPE: {
        if (values[1]->getType() != CL_PARSER_VALUE_TYPE_STRING) {
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
          return ClParserValuePtr();
        }

        std::string is_type = values[1]->getString()->getText();

        CStrUtil::IsCType is_func = CStrUtil::getIsCType(is_type);

        if (! is_func) {
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
          return ClParserValuePtr();
        }

        long result;

        if (values[0]->isType(CL_PARSER_VALUE_TYPE_STRING)) {
          std::string text = values[0]->toString()->getString()->getText();

          uint len = uint(text.size());

          uint match = 0;

          for (uint i = 0; i < len; ++i)
            if ((*is_func)(int(text[i])))
              ++match;

          result = (match > 0 && match == len);
        }
        else {
          long integer1 = values[0]->toInt()->getInteger()->getValue();

          result = long((*is_func)(int(integer1)));
        }

        value = ClParserValueMgrInst->createValue(result);

        break;
      }

      //------

      case CLParserInternFnType::WHERE: {
       if (values[0]->getType() != CL_PARSER_VALUE_TYPE_STRING ||
           values[1]->getType() != CL_PARSER_VALUE_TYPE_STRING) {
         ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
          return ClParserValuePtr();
        }

        ClParserStringPtr str1 = values[0]->getString();
        ClParserStringPtr str2 = values[1]->getString();

        value = ClParserProcessWhere(const_cast<char *>(str1->getText().c_str()),
                                     const_cast<char *>(str2->getText().c_str()),
                                     error_code);

        if (*error_code != 0)
          return ClParserValuePtr();

        break;
      }

      //------

      case CLParserInternFnType::ASSERT:
        value->doAssert();
        break;

      //------

      case CLParserInternFnType::CEIL:
        value = value->ceil();
        break;
      case CLParserInternFnType::FLOOR:
        value = value->floor();
        break;
      case CLParserInternFnType::SIGN:
        value = value->sign();
        break;

      case CLParserInternFnType::SQR:
        value = value->sqr();
        break;
      case CLParserInternFnType::SQRT:
        value = value->sqrt();
        break;

      case CLParserInternFnType::COS:
        value = value->cos();
        break;
      case CLParserInternFnType::SIN:
        value = value->sin();
        break;
      case CLParserInternFnType::TAN:
        value = value->tan();
        break;

      case CLParserInternFnType::ACOS:
        value = value->acos();
        break;
      case CLParserInternFnType::ASIN:
        value = value->asin();
        break;
      case CLParserInternFnType::ATAN:
        value = value->atan();
        break;
      case CLParserInternFnType::ATAN2: {
        double y = values[1]->toReal()->getReal()->getValue();

        value = value->atan(y);

        break;
      }
      case CLParserInternFnType::EXP:
        value = value->exp();
        break;
      case CLParserInternFnType::LOG:
        value = value->log();
        break;
      case CLParserInternFnType::LOG10:
        value = value->log10();
        break;

      case CLParserInternFnType::COSH:
        value = value->cosh();
        break;
      case CLParserInternFnType::SINH:
        value = value->sinh();
        break;
      case CLParserInternFnType::TANH:
        value = value->tanh();
        break;

      case CLParserInternFnType::NORM:
        value = value->norm();
        break;
      case CLParserInternFnType::INVNORM:
        value = value->invnorm();
        break;

      case CLParserInternFnType::CHAR_CONV:
        value = value->toChar();
        break;
      case CLParserInternFnType::INT_CONV:
        value = value->toInt();
        break;
      case CLParserInternFnType::REAL_CONV:
        value = value->toReal();
        break;
      case CLParserInternFnType::STRING_CONV:
        value = value->toString();
        break;

      case CLParserInternFnType::IS_NAN:
        value = value->isNan();
        break;

      case CLParserInternFnType::TOLOWER:
        value = value->toLower();
        break;
      case CLParserInternFnType::TOUPPER:
        value = value->toUpper();
        break;

      case CLParserInternFnType::DIM:
        value = value->dim();
        break;
      case CLParserInternFnType::NDIM:
        value = value->ndim();
        break;
      case CLParserInternFnType::LEN:
        value = value->len();
        break;

      case CLParserInternFnType::MIN:
        value = value->min();
        break;
      case CLParserInternFnType::MAX:
        value = value->max();
        break;
      case CLParserInternFnType::SUM:
        value = value->sum();
        break;

      case CLParserInternFnType::ABS:
        value = value->abs();
        break;

      case CLParserInternFnType::INDEX:
        value = value->index(values[1]);
        break;
      case CLParserInternFnType::RINDEX:
        value = value->rindex(values[1]);
        break;

      case CLParserInternFnType::SORT: {
        if (num_values > 2) {
          ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
          return ClParserValuePtr();
        }

        ClParserSortDirection direction = ClParserSortDirection::ASCENDING;

        if (num_values > 1) {
          if (values[1]->getType() != CL_PARSER_VALUE_TYPE_STRING) {
            ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
            return ClParserValuePtr();
          }

          std::string text = values[1]->getString()->getText();

          if      (CStrUtil::casecmp(text, "ascending" ) == 0)
            direction = ClParserSortDirection::ASCENDING;
          else if (CStrUtil::casecmp(text, "descending") == 0)
            direction = ClParserSortDirection::DESCENDING;
        }

        value = value->sort(direction);

        break;
      }

      //------

      case CLParserInternFnType::ARRAY_I: {
        UIntVectorT dims;

        for (uint i = 0; i < num_values; i++) {
          ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

          if (! value1->convertToInteger()) {
            ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
            return ClParserValuePtr();
          }

          int dim = int(value1->getInteger()->getValue());

          if (dim <= 0 && ! (i == 0 && dim == 0 && num_values == 1)) {
            ClParserInst->signalError(error_code, ClErr::INVALID_DIMENSION);
            return ClParserValuePtr();
          }

          dims.push_back(uint(dim));
        }

        ClParserArrayPtr array = ClParserArray::createArray(dims, 0L);

        value = ClParserValueMgrInst->createValue(*array);

        break;
      }
      case CLParserInternFnType::ARRAY_R: {
        UIntVectorT dims;

        for (uint i = 0; i < num_values; i++) {
          ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

          if (! value1->convertToInteger()) {
            ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
            return ClParserValuePtr();
          }

          int dim = int(value1->getInteger()->getValue());

          if (dim <= 0 && ! (i == 0 && dim == 0 && num_values == 1)) {
            ClParserInst->signalError(error_code, ClErr::INVALID_DIMENSION);
            return ClParserValuePtr();
          }

          dims.push_back(uint(dim));
        }

        ClParserArrayPtr array = ClParserArray::createArray(dims, 0.0);

        value = ClParserValueMgrInst->createValue(array);

        break;
      }
      case CLParserInternFnType::ARRAY_S: {
        UIntVectorT dims;

        for (uint i = 0; i < num_values; i++) {
          ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

          if (! value1->convertToInteger()) {
            ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
            return ClParserValuePtr();
          }

          int dim = int(value1->getInteger()->getValue());

          if (dim <= 0 && ! (i == 0 && dim == 0 && num_values == 1)) {
            ClParserInst->signalError(error_code, ClErr::INVALID_DIMENSION);
            return ClParserValuePtr();
          }

          dims.push_back(uint(dim));
        }

        ClParserArrayPtr array = ClParserArray::createArray(dims, "");

        value = ClParserValueMgrInst->createValue(*array);

        break;
      }

      case CLParserInternFnType::ARRCAT: {
        if (values[0]->getType() != CL_PARSER_VALUE_TYPE_ARRAY ||
            values[1]->getType() != CL_PARSER_VALUE_TYPE_ARRAY) {
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
          break;
        }

        if (values[1]->getType() != CL_PARSER_VALUE_TYPE_ARRAY ||
            values[1]->getArray()->getType() != values[0]->getArray()->getType()) {
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_MIX);
          break;
        }

        ClParserArrayPtr array = value->getArray()->concat(*values[1]->getArray());

        if (! array) {
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_MIX);
          break;
        }

        value = ClParserValueMgrInst->createValue(array);

        break;
      }

      case CLParserInternFnType::INDARR_I: {
        UIntVectorT dims;

        for (uint i = 0; i < num_values; i++) {
          ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

          if (! value1->convertToInteger()) {
            ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
            return ClParserValuePtr();
          }

          int dim = int(value1->getInteger()->getValue());

          if (dim <= 0 && ! (i == 0 && dim == 0 && num_values == 1)) {
            ClParserInst->signalError(error_code, ClErr::INVALID_DIMENSION);
            return ClParserValuePtr();
          }

          dims.push_back(uint(dim));
        }

        ClParserArrayPtr array = ClParserArray::createArray(dims, 0L);

        array->setIndexArray();

        value = ClParserValueMgrInst->createValue(*array);

        break;
      }
      case CLParserInternFnType::INDARR_R: {
        UIntVectorT dims;

        for (uint i = 0; i < num_values; i++) {
          ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

          if (! value1->convertToInteger()) {
            ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
            return ClParserValuePtr();
          }

          int dim = int(value1->getInteger()->getValue());

          if (dim <= 0 && ! (i == 0 && dim == 0 && num_values == 1)) {
            ClParserInst->signalError(error_code, ClErr::INVALID_DIMENSION);
            return ClParserValuePtr();
          }

          dims.push_back(uint(dim));
        }

        ClParserArrayPtr array = ClParserArray::createArray(dims, 0.0);

        array->setIndexArray();

        value = ClParserValueMgrInst->createValue(*array);

        break;
      }
      case CLParserInternFnType::INDARR_S: {
        UIntVectorT dims;

        for (uint i = 0; i < num_values; i++) {
          ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

          if (! value1->convertToInteger()) {
            ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
            return ClParserValuePtr();
          }

          int dim = int(value1->getInteger()->getValue());

          if (dim <= 0 && ! (i == 0 && dim == 0 && num_values == 1)) {
            ClParserInst->signalError(error_code, ClErr::INVALID_DIMENSION);
            return ClParserValuePtr();
          }

          dims.push_back(uint(dim));
        }

        ClParserArrayPtr array = ClParserArray::createArray(dims, "");

        array->setIndexArray();

        value = ClParserValueMgrInst->createValue(*array);

        break;
      }

      case CLParserInternFnType::SUBARR:
        ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
        break;

      case CLParserInternFnType::TYPARR: {
        if (values[1]->getType() != CL_PARSER_VALUE_TYPE_STRING) {
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
          break;
        }

        ClParserStringPtr str1 = values[1]->getString();

        auto struct_type = ClParserInst->getType(const_cast<char *>(str1->getText().c_str()));

        if (! struct_type) {
          ClParserInst->signalError(error_code, ClErr::UNDEFINED_STRUCT_TYPE);
          break;
        }

        UIntVectorT dims;

        for (uint i = 1; i < num_values; i++) {
          ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

          if (! value1->convertToInteger()) {
            ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
            return ClParserValuePtr();
          }

          int dim = int(value1->getInteger()->getValue());

          if (dim <= 0 && ! (i == 1 && dim == 0 && num_values == 1)) {
            ClParserInst->signalError(error_code, ClErr::INVALID_DIMENSION);
            return ClParserValuePtr();
          }

          dims.push_back(uint(dim));
        }

        ClParserArrayPtr array = ClParserArray::createArray(struct_type, dims);

        value = ClParserValueMgrInst->createValue(*array);

        break;
      }

      default:
        assert(false);
        break;
    }
  }
  catch (...) {
    ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
    return ClParserValuePtr();
  }

  return value;
}

// Process the addr() internal function.

ClParserValuePtr
ClParserProcessAddrCommand(const ClParserValueArray &values, int *error_code)
{
  ClParserValuePtr value;

  *error_code = 0;

  uint num_values = uint(values.size());

  if      (values[0]->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
    uint offset;

    char *address = reinterpret_cast<char *>(values[0]->getInteger()->getValue());

    if (num_values < 2) {
      ClParserInst->signalError(error_code, ClErr::TOO_FEW_ARGUMENTS);
      return value;
    }

    if (values[1]->getType() != CL_PARSER_VALUE_TYPE_STRING ||
        ! values[1]->getString()) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return value;
    }

    std::string type = values[1]->getString()->getText();

    if (num_values >= 3) {
      if (values[2]->getType() != CL_PARSER_VALUE_TYPE_INTEGER) {
        ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
        return value;
      }

      offset = uint(values[2]->getInteger()->getValue());
    }
    else
      offset = 0;

    if      (type == "short") {
      address += offset*sizeof(short);

      if (num_values > 3)
        ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
      else
        value = ClParserValueMgrInst->createValue(long(*reinterpret_cast<short *>(address)));
    }
    else if (type == "int") {
      address += offset*sizeof(int);

      if (num_values > 3)
        ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
      else
        value = ClParserValueMgrInst->createValue(long(*reinterpret_cast<int *>(address)));
    }
    else if (type == "long") {
      address += offset*sizeof(long);

      if (num_values > 3)
        ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
      else
        value = ClParserValueMgrInst->createValue((*reinterpret_cast<long *>(address)));
    }
    else if (type == "float") {
      address += offset*sizeof(float);

      if (num_values > 3)
        ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
      else
        value = ClParserValueMgrInst->createValue(double((*reinterpret_cast<float *>(address))));
    }
    else if (type == "double") {
      address += offset*sizeof(double);

      if (num_values > 3)
        ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
      else
        value = ClParserValueMgrInst->createValue(*(reinterpret_cast<double *>(address)));
    }
    else if (type == "char *") {
      address += offset*sizeof(char);

      if (num_values > 3)
        ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
      else
        value = ClParserValueMgrInst->createValue(reinterpret_cast<char *>(address));
    }
    else
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
  }
  else if (values[0]->isType(CL_PARSER_VALUE_TYPE_STRING)) {
    char *address;

    if (num_values != 1) {
      ClParserInst->signalError(error_code, ClErr::TOO_MANY_ARGUMENTS);
      return value;
    }

    if (! values[0]->getString()) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return value;
    }

    std::string name = values[0]->getString()->getText();

    if (! ClParserInst->isVariable(name)) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VARIABLE);
      return value;
    }

    ClParserValuePtr value1 = ClParserInst->getVariableValue(name);

    if (! value1) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return value1;
    }

    if      (value1->isType(CL_PARSER_VALUE_TYPE_REAL)) {
      double *real = const_cast<double *>(value1->getReal()->getValueP());

      address = reinterpret_cast<char *>(real);
    }
    else if (value1->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
      long *integer = const_cast<long *>(value1->getInteger()->getValueP());

      address = reinterpret_cast<char *>(integer);
    }
    else if (value1->isType(CL_PARSER_VALUE_TYPE_STRING)) {
      char *str = const_cast<char *>(value1->getString()->getCharP());

      address = str;
    }
    else if (value1->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
      UIntVectorT dims;

      ClParserArrayPtr array1 = value1->getArray();

      if      (array1->isType(CL_PARSER_VALUE_TYPE_REAL)) {
        double *reals;

        if (array1->toReals(&reals, dims))
          address = reinterpret_cast<char *>(reals);
        else
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      }
      else if (array1->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
        long *integers;

        if (array1->toIntegers(&integers, dims))
          address = reinterpret_cast<char *>(integers);
        else
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      }
      else if (array1->isType(CL_PARSER_VALUE_TYPE_STRING)) {
        char **strings;

        if (array1->toStrings(&strings, dims))
          address = reinterpret_cast<char *>(strings);
        else
          ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      }
      else
        ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
    }
    else
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);

    if (*error_code == 0)
      value = ClParserValueMgrInst->createValue(long(address));
  }
  else
    ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);

  return value;
}

// Process the where() internal function.

ClParserValuePtr
ClParserProcessWhere(const std::string &variables_string, const std::string &expression,
                     int *error_code)
{
  ClParserValueArray values;

  /*****************/

  StringVectorT variable_list;

  uint len = uint(variables_string.size());

  for (uint i = 0; i < len; ++i) {
    CStrUtil::skipSpace(variables_string, &i);

    std::string variable;

    if (! CStrUtil::readIdentifier(variables_string, &i, variable)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_CHARACTER);
      return ClParserValuePtr();
    }

    variable_list.push_back(variable);

    if (variables_string[i] == '\0')
      break;

    if (variables_string[i] != ',') {
      ClParserInst->signalError(error_code, ClErr::INVALID_CHARACTER);
      return ClParserValuePtr();
    }
  }

  if (variable_list.empty()) {
    ClParserInst->signalError(error_code, ClErr::INVALID_VARIABLE_NAME);
    return ClParserValuePtr();
  }

  /*****************/

  uint num_variables = uint(variable_list.size());

  for (uint i = 0; i < num_variables; i++) {
    ClParserValuePtr value = ClParserInst->getVariableValue(variable_list[i]);

    if (! value || ! value->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
      ClParserInst->signalError(error_code, ClErr::ARRAY_FN_HAS_NON_ARRAY);
      return ClParserValuePtr();
    }

    values.push_back(value);

    if (i > 1 && ! ClParserValue::checkBinaryTypes(values[i], values[i - 1])) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_MIX);
      return ClParserValuePtr();
    }
  }

  if (values[0]->getArray()->getNumData() <= 0) {
    uint dims = 0;

    auto value = ClParserValueMgrInst->createValue(&dims, 1, static_cast<long *>(nullptr));

    return value;
  }

  /*************/

  ClParserExpr expr(expression);

  if (! expr.compile()) {
    ClParserInst->signalError(error_code, ClErr::INVALID_EXPRESSION);
    return ClParserValuePtr();
  }

  /*************/

  ClParserInst->newVariableList();

  LongVectorT indices;

  for (uint u = 0; u < values[0]->getArray()->getNumData(); u++) {
    for (uint k = 0; k < num_variables; k++) {
      auto value2 = values[k]->getArray()->getValue(int(u + 1));

      ClParserInst->createVar(variable_list[k], value2);
    }

    ClParserValuePtr value1;

    if (! expr.exec(value1)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_EXPRESSION);
      return ClParserValuePtr();
    }

    long flag;

    if (! value1->integerValue(&flag)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
      return ClParserValuePtr();
    }

    if (! flag) continue;

    indices.push_back(u + 1);
  }

  /*************/

  uint dims = uint(indices.size());

  ClParserValuePtr value = ClParserValueMgrInst->createValue(&dims, 1, indices);

  ClParserInst->oldVariableList();

  return value;
}

// Execute a user function with the supplied array of values.

ClParserValuePtr
ClParserProcessUserFn(ClParserUserFnPtr userfn, const ClParserArgValueArray &arg_values,
                      int *error_code)
{
  CL_PARSER_TRACE("ClParserProcessUserFn")

  *error_code = 0;

  uint num_arg_values = uint(arg_values.size());

  if (num_arg_values > 0) {
    ClParserValuePtr value1;

    ClParserVarRefPtr var_ref = arg_values[0].getVarRef();

    if (var_ref) {
      if (! var_ref->getValue(value1)) {
        ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
        return ClParserValuePtr();
      }
    }
    else
      value1 = arg_values[0].getValue();

    if (value1) {
      if      (value1->isType(CL_PARSER_VALUE_TYPE_ARRAY) &&
               userfn->getNumArgTypes() > 0 &&
               ! (userfn->getArgType(0) & CL_PARSER_VALUE_TYPE_ARRAY)) {
        ClParserValuePtr value = ClParserProcessArrayUserFn(userfn, arg_values, error_code);

        return value;
      }
      else if (value1->isType(CL_PARSER_VALUE_TYPE_STRUCTURE) &&
               userfn->getNumArgTypes() > 0 &&
               ! (userfn->getArgType(0) & CL_PARSER_VALUE_TYPE_STRUCTURE)) {
        ClParserValuePtr value = ClParserProcessStructureUserFn(userfn, arg_values, error_code);

        return value;
      }
    }
  }

  ClParserValueArray values;

  for (uint i = 0; i < num_arg_values; i++)
    values.push_back(ClParserValuePtr());

  for (uint i = 0; i < num_arg_values; i++) {
    ClParserVarRefPtr var_ref = arg_values[i].getVarRef();

    if (var_ref) {
      if (! var_ref->getValue(values[i])) {
        ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
        return ClParserValuePtr();
      }
    }
    else
      values[i] = arg_values[i].getValue();

    if (userfn->getNumArgTypes() <= 0)
      continue;

    int types;

    if (i < userfn->getNumArgTypes())
      types = userfn->getArgType(i);
    else
      types = userfn->getArgType(userfn->getNumArgTypes() - 1);

    if (types & CL_PARSER_VALUE_TYPE_OUTPUT) {
      if (! var_ref) {
        ClParserInst->signalError(error_code, ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
        return ClParserValuePtr();
      }

      if (! values[i]) {
        if      (types & CL_PARSER_VALUE_TYPE_REAL)
          values[i] = ClParserValueMgrInst->createValue(0.0);
        else if (types & CL_PARSER_VALUE_TYPE_INTEGER)
          values[i] = ClParserValueMgrInst->createValue(0L);
        else if (types & CL_PARSER_VALUE_TYPE_STRING)
          values[i] = ClParserValueMgrInst->createValue("");
        else
          values[i] = ClParserValueMgrInst->createValue(0L);

        ClParserValuePtr value1 = ClParserValueMgrInst->createValue(values[i]);

        var_ref->setValue(value1);
      }
    }

    if (! values[i]) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);

      return ClParserValuePtr();
    }

    if (types & CL_PARSER_VALUE_TYPE_INPUT) {
      ClParserCheckUnaryValue(values[i], types, error_code);

      if (*error_code != 0)
        return ClParserValuePtr();
    }
  }

  /*-----------------*/

  ClParserValuePtr value = userfn->exec(values, error_code);

  if (! value || *error_code != 0)
    return ClParserValuePtr();

  /*-----------------*/

  for (uint i = 0; i < num_arg_values; i++) {
    if (userfn->getNumArgTypes() <= 0)
      continue;

    int types;

    if (i < userfn->getNumArgTypes())
      types = userfn->getArgType(i);
    else
      types = userfn->getArgType(userfn->getNumArgTypes() - 1);

    if (types & CL_PARSER_VALUE_TYPE_OUTPUT) {
      ClParserVarRefPtr var_ref = arg_values[i].getVarRef();

      if (! var_ref->setValue(values[i])) {
        ClParserInst->signalError(error_code, ClErr::UNDEFINED_ASSIGN_VALUE);
        return ClParserValuePtr();
      }
    }
  }

  /*-----------------*/

  if (ClParserInst->isDebug())
    ClParserStackMgrInst->debugPrintCurrent();

  return value;
}

// Execute a user function with the supplied array of
// values where the first argument is an array and the
// user function takes a single value as the first
// argument.

static ClParserValuePtr
ClParserProcessArrayUserFn(ClParserUserFnPtr userfn, const ClParserArgValueArray &arg_values,
                           int *error_code)
{
  CL_PARSER_TRACE("ClParserProcessArrayUserFn")

  *error_code = 0;

  ClParserValuePtr value1;

  ClParserVarRefPtr var_ref = arg_values[0].getVarRef();

  if (var_ref) {
    if (! var_ref->getValue(value1)) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
      return ClParserValuePtr();
    }
  }
  else
    value1 = arg_values[0].getValue();

  ClParserArrayPtr array = value1->getArray();

  ClParserValuePtr *array_values;
  UIntVectorT       array_dims;

  array->toValues(&array_values, array_dims);

  ClParserValueArray array_values1;

  for (uint i = 0; i < array->getNumData(); i++)
    array_values1.push_back(ClParserValuePtr());

  ClParserArgValueArray arg_values1 = arg_values;

  arg_values1[0].setVarRef(ClParserVarRefPtr());

  for (uint i = 0; i < array->getNumData(); i++) {
    arg_values1[0].setValue(array_values[i]);

    array_values1[i] = ClParserProcessUserFn(userfn, arg_values1, error_code);

    if (*error_code != 0)
      return ClParserValuePtr();
  }

  if (array_values1[0]->getType() == CL_PARSER_VALUE_TYPE_ARRAY) {
    ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
    return ClParserValuePtr();
  }

  ClParserValueType type = array_values1[0]->getType();

  for (uint i = 1; i < array->getNumData(); i++)
    if (array_values1[i]->getType() != CL_PARSER_VALUE_TYPE_ARRAY &&
        array_values1[i]->getType() >  type)
      type = array_values1[i]->getType();

  for (uint i = 0; i < array->getNumData(); i++) {
    ClParserCheckUnaryValue(array_values1[i], type, error_code);

    if (*error_code != 0)
      return ClParserValuePtr();
  }

  ClParserArrayPtr array1 = ClParserArray::createArray(array_values1, array_dims);

  ClParserValuePtr value = ClParserValueMgrInst->createValue(array1);

  return value;
}

// Execute a user function with the supplied array of
// values where the first argument is a structure and
// the user function takes a single value as the first
// argument.

ClParserValuePtr
ClParserProcessStructureUserFn(ClParserUserFnPtr userfn, const ClParserArgValueArray &arg_values,
                               int *error_code)
{
  CL_PARSER_TRACE("ClParserProcessStructureUserFn")

  *error_code = 0;

  ClParserValuePtr value1;

  ClParserVarRefPtr var_ref = arg_values[0].getVarRef();

  if (var_ref) {
    if (! var_ref->getValue(value1)) {
      ClParserInst->signalError(error_code, ClErr::UNDEFINED_VALUE);
      return ClParserValuePtr();
    }
  }
  else
    value1 = arg_values[0].getValue();

  ClParserStructPtr structure = value1->getStructure();

  ClParserTypePtr type       = structure->getType();
  uint            num_values = structure->getNumValues();

  ClParserValueArray struct_values;

  for (uint i = 0; i < num_values; ++i)
    struct_values.push_back(ClParserValuePtr());

  ClParserArgValueArray arg_values1 = arg_values;

  arg_values1[0].setVarRef(ClParserVarRefPtr());

  for (uint i = 0; i < num_values; i++) {
    const std::string &name = type->getSubType(i)->getName();

    ClParserValuePtr svalue;

    if (structure->getValue(name, svalue))
      arg_values1[0].setValue(svalue);

    struct_values[i] = ClParserProcessUserFn(userfn, arg_values1, error_code);

    if (*error_code != 0)
      return ClParserValuePtr();
  }

  for (uint i = 0; i < num_values; i++) {
    const std::string &name = type->getSubType(i)->getName();

    ClParserValuePtr svalue;

    if (! structure->getValue(name, svalue))
      return ClParserValuePtr();

    ClParserCheckUnaryValue(struct_values[i], svalue->getType(), error_code);

    if (*error_code != 0)
      return ClParserValuePtr();
  }

  ClParserValuePtr value = ClParserValueMgrInst->createValue(structure->getType(), struct_values);

  return value;
}

// Check whether the specified value can be used with a
// unary operator which handles the specified list of
// types.

void
ClParserCheckUnaryValue(ClParserValuePtr value, int types, int *error_code)
{
  *error_code = 0;

  if (types & CL_PARSER_VALUE_TYPE_GENERIC)
    return;

  if (! (value->getType() & types)) {
    if      (value->isType(CL_PARSER_VALUE_TYPE_REAL)) {
      if (types & CL_PARSER_VALUE_TYPE_INTEGER) {
        if (! value->canConvertToInteger()) {
          ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
          return;
        }
      }
      else {
        ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
        return;
      }
    }
    else if (value->isType(CL_PARSER_VALUE_TYPE_INTEGER)) {
      if (types & CL_PARSER_VALUE_TYPE_REAL) {
        if (! value->canConvertToReal()) {
          ClParserInst->signalError(error_code, ClErr::INVALID_CONVERSION);
          return;
        }
      }
      else {
        ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
        return;
      }
    }
    else if (value->isType(CL_PARSER_VALUE_TYPE_STRING)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return;
    }
    else if (value->isType(CL_PARSER_VALUE_TYPE_ARRAY)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return;
    }
    else if (value->isType(CL_PARSER_VALUE_TYPE_LIST)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return;
    }
    else if (value->isType(CL_PARSER_VALUE_TYPE_DICTIONARY)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return;
    }
    else if (value->isType(CL_PARSER_VALUE_TYPE_STRUCTURE)) {
      ClParserInst->signalError(error_code, ClErr::INVALID_TYPE_FOR_OPERATOR);
      return;
    }
  }

  return;
}
