#include <CCeilPI.h>

static char parser_string_chars         [] = "'\"";
static char parser_unary_operator_chars [] = "+-!~";
static char parser_binary_operator_chars[] = "+-*/%<>=!&|^#~";

// Parse the expression string from the current position
// and add the parsed elements to the execution stack.

bool
ClParserParser::
parse()
{
  ClParserOperatorType op = CL_PARSER_OP_NONE;

  error_code_ = 0;

  bool expression = false;

  parse_.skipSpace();

  while (! parse_.eof()) {
    /* <expression> := <inline_operator> <expression>
       <expression> := <expression> <inline_operator> */

    if      (parse_.isString("++") || parse_.isString("--")) {
      if (parse_.isChar('+'))
        ClParserStackMgrInst->push(CL_PARSER_OP_INCREMENT);
      else
        ClParserStackMgrInst->push(CL_PARSER_OP_DECREMENT);

      ClParserStackMgrInst->toNext();

      parse_.skipChars(2);

      if (! expression) {
        ClParserParser parser(this);

        if (! parser.parse()) {
          error_code_ = parser.getErrorCode();
          goto fail;
        }

        if (parser.isAssign()) {
          error_code_ = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
          goto fail;
        }
      }

      expression = true;
    }

    /* <expression> := <unary_operator> <expression> */

    else if (! expression && parse_.isOneOf(parser_unary_operator_chars)) {
      if (! readUnaryOperator(&op)) {
        error_code_ = int(ClErr::INVALID_CHARACTER);
        goto fail;
      }

      ClParserStackMgrInst->push(op);

      ClParserStackMgrInst->toNext();

      ClParserParser parser(this);

      if (! parser.parse()) {
        error_code_ = parser.getErrorCode();
        goto fail;
      }

      if (parser.isAssign()) {
        error_code_ = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
        goto fail;
      }

      expression = true;
    }

    /* <expression> := '(' <expression> ')' */

    else if (! expression && parse_.isChar('(')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_R_BRACKET);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      ClParserParser parser(this);

      /* TODO: set end char */
      if (! parser.parse() &&
          parser.getErrorCode() != int(ClErr::INVALID_CHARACTER)) {
        error_code_ = parser.getErrorCode();
        goto fail;
      }

      parse_.skipSpace();

      if (! parse_.isChar(')')) {
        error_code_ = int(ClErr::MISSING_CLOSE_ROUND_BRACKET);
        goto fail;
      }

      ClParserStackMgrInst->push(CL_PARSER_OP_CLOSE_R_BRACKET);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      expression = true;
    }

    /* <expression> := <numeric_value> */

    else if (! expression && (parse_.isDigit() || parse_.isChar('.'))) {
      ClParserValuePtr value;

      if (! readNumericValue(value))
        goto fail;

      ClParserStackMgrInst->push(value);

      ClParserStackMgrInst->toNext();

      expression = true;
    }

    /* <expression> := <string_value> */

    else if (! expression && parse_.isOneOf(parser_string_chars)) {
      ClParserValuePtr value;

      if (! readStringValue(value))
        goto fail;

      ClParserStackMgrInst->push(value);

      ClParserStackMgrInst->toNext();

      expression = true;
    }

    /* <expression> := <array_value> */

    else if (! expression && parse_.isChar('[')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_S_BRACKET);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      if (! readArray())
        goto fail;

      expression = true;
    }

    /* <expression> := <dict_value> */

    else if (! expression && parse_.isString("{{")) {
      ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_DICT);

      ClParserStackMgrInst->toNext();

      parse_.skipChars(2);

      if (! readDictionary())
        goto fail;

      expression = true;
    }

    /* <expression> := <list_value> */

    else if (! expression && parse_.isChar('{')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_BRACE);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      if (! readList())
        goto fail;

      expression = true;
    }

    /* <expression> := <expression> <binary_operator> <expression> */

    else if (expression && parse_.isOneOf(parser_binary_operator_chars)) {
      if (! readBinaryOperator(&op))
        goto fail;

      ClParserStackMgrInst->push(op);

      ClParserStackMgrInst->toNext();

      ClParserParser parser(this);

      if (! parser.parse()) {
        error_code_ = parser.getErrorCode();
        goto fail;
      }

      ClParserOperatorPtr op1 = ClParserOperatorMgrInst->getOperator(op);

      if (! op1->isAssignment() && parser.isAssign()) {
        error_code_ = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
        goto fail;
      }

      if (op1->isAssignment())
        assign_ = true;

      expression = true;
    }

    /* <expression> := <expression> '?' <expression> ':' <expression> */

    else if (expression && parse_.isChar('?')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_QUESTION_MARK);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      ClParserParser parser1(this);

      if (! parser1.parse() &&
          parser1.getErrorCode() != int(ClErr::INVALID_CHARACTER)) {
        error_code_ = parser1.getErrorCode();
        goto fail;
      }

      parse_.skipSpace();

      if (! parse_.isChar(':')) {
        error_code_ = int(ClErr::MISSING_COLON_OPERATOR);
        goto fail;
      }

      ClParserStackMgrInst->push(CL_PARSER_OP_COLON);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      ClParserParser parser2(this);

      if (! parser2.parse() &&
          parser2.getErrorCode() != int(ClErr::INVALID_CHARACTER)) {
        error_code_ = parser2.getErrorCode();
        goto fail;
      }

      assign_ = parser1.isAssign() || parser2.isAssign();

      expression = true;
    }

    /* <expression> := <expression> '[' <expression> ',' .... ']' */

    else if (expression && parse_.isChar('[')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_S_BRACKET);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      if (! readArray())
        goto fail;

      expression = true;
    }

    /* <expression> := <expression> ',' <expression> */

    else if (expression && parse_.isChar(',')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_COMMA);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      ClParserParser parser(this);

      if (! parser.parse()) {
        error_code_ = parser.getErrorCode();
        goto fail;
      }

      expression = true;
    }

    /* <expression> := <variable>                               */
    /* <expression> := <variable> '.' <variable> '.' <...>      */
    /* <expression> := <variable> '[' <expression> ',' .... ']' */
    /* <expression> := <function> '(' <expression> ',' .... ')' */
    /* <expression> := <type>     '(' <expression> ',' .... ')' */

    else if (! expression &&
             ((! ClParserInst->getDollarPrefix() && parse_.isIdentifier()) ||
              (  ClParserInst->getDollarPrefix() &&
               parse_.isChar('$') && parse_.isIdentifier(1)))) {
      if (ClParserInst->getDollarPrefix() && parse_.isChar('$'))
        parse_.skipChar();

      ClParserIdentifierPtr identifier;

      if (! readIdentifier(identifier))
        goto fail;

      const std::string &name  = identifier->getName();
      ClParserScopePtr   scope = identifier->getScope();

      parse_.skipSpace();

      if (parse_.isChar('(')) {
        ClParserInternFnPtr internfn;

        if (scope.isValid())
          internfn = scope->getInternFn(name);
        else
          internfn = ClParserInst->getInternFn(name);

        if (internfn.isValid()) {
          ClParserStackMgrInst->push(internfn);

          ClParserStackMgrInst->toNext();
        }
        else {
          ClParserUserFnPtr userfn;

          if (scope.isValid())
            userfn = scope->getUserFn(name);
          else
            userfn = ClParserInst->getUserFn(name);

          if (userfn.isValid()) {
            ClParserStackMgrInst->push(userfn);

            ClParserStackMgrInst->toNext();
          }
          else {
            ClParserTypePtr type = ClParserInst->getType(name);

            if (type.isValid()) {
              ClParserStackMgrInst->push(type);

              ClParserStackMgrInst->toNext();
            }
            else {
              identifier->setFunction();

              ClParserStackMgrInst->push(identifier);

              ClParserStackMgrInst->toNext();
            }
          }
        }

        ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_R_BRACKET);

        ClParserStackMgrInst->toNext();

        parse_.skipChar();

        if (! readArgList())
          goto fail;

        expression = true;
      }
      else {
        identifier->setVariable();

        ClParserStackMgrInst->push(identifier);

        ClParserStackMgrInst->toNext();

        if (parse_.isChar('[')) {
          ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_S_BRACKET);

          ClParserStackMgrInst->toNext();

          parse_.skipChar();

          if (! readArray())
            goto fail;

          parse_.skipSpace();
        }

        while (parse_.isChar('.')) {
          identifier->setStructPart();

          ClParserStackMgrInst->push(CL_PARSER_OP_DOT);

          ClParserStackMgrInst->toNext();

          parse_.skipChar();

          parse_.skipSpace();

          if (! readIdentifier(identifier)) {
            error_code_ = int(ClErr::INVALID_CHARACTER);
            goto fail;
          }

          identifier->setVariable();

          ClParserStackMgrInst->push(identifier);

          ClParserStackMgrInst->toNext();

          if (parse_.isChar('[')) {
            ClParserStackMgrInst->push(CL_PARSER_OP_OPEN_S_BRACKET);

            ClParserStackMgrInst->toNext();

            parse_.skipChar();

            if (! readArray())
              goto fail;

            parse_.skipSpace();
          }
        }

        expression = true;
      }
    }
    /* <expression> := <string_value> */

    else if (! expression && ClParserInst->getDollarPrefix()) {
      std::string str;

      parse_.readNonSpace(str);

      ClParserValuePtr value = ClParserValueMgrInst->createValue(str);

      ClParserStackMgrInst->push(value);

      ClParserStackMgrInst->toNext();

      parse_.skipSpace();

      expression = true;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      goto fail;
    }

    parse_.skipSpace();
  }

  if (error_code_ == 0 && ! expression) {
    error_code_ = int(ClErr::NULL_EXPRESSION);
    goto fail;
  }

  if (parent_)
    parent_->parse_.setPos(int(parent_->parse_.getPos()) + int(parse_.getPos()));

  return true;

 fail:
  if (parent_)
    parent_->parse_.setPos(int(parent_->parse_.getPos()) + int(parse_.getPos()));

  return false;
}

// Skip the expression string from the current position.

bool
ClParserParser::
skipExpression(bool *assign)
{
  bool expression = false;

  error_code_ = 0;

  parse_.skipSpace();

  while (! parse_.eof()) {
    /* <expression> := <unary_operator> <expression> */

    if      (! expression && strchr(parser_unary_operator_chars, parse_.getCharAt()) != nullptr) {
      skipUnaryOperator();

      if (! skipExpression(assign))
        return false;

      if (*assign) {
        error_code_ = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
        return false;
      }

      expression = true;
    }

    /* <expression> := '(' <expression> ')' */

    else if (! expression && parse_.isChar('(')) {
      bool assign1 = false;

      parse_.skipChar();

      if (! skipExpression(&assign1) && error_code_ != int(ClErr::INVALID_CHARACTER))
        return false;

      error_code_ = 0;

      parse_.skipSpace();

      if (! parse_.isChar(')')) {
        error_code_ = int(ClErr::MISSING_CLOSE_ROUND_BRACKET);
        return false;
      }

      parse_.skipChar();

      expression = true;
    }

    /* <expression> := <numeric_value> */

    else if (! expression &&
             (parse_.isDigit() || parse_.isChar('.'))) {
      if (! skipNumericValue())
        return false;

      expression = true;
    }

    /* <expression> := <string_value> */

    else if (! expression && parse_.isOneOf(parser_string_chars)) {
      if (! skipStringValue())
        return false;

      expression = true;
    }

    /* <expression> := <array_value> */

    else if (! expression && parse_.isChar('[')) {
      parse_.skipChar();

      if (! skipArray())
        return false;

      expression = true;
    }

    /* <expression> := <dict_value> */

    else if (! expression && parse_.isString("{{")) {
      parse_.skipChars(2);

      if (! skipDictionary())
        return false;

      expression = true;
    }

    /* <expression> := <list_value> */

    else if (! expression && parse_.isChar('{')) {
      parse_.skipChar();

      if (! skipList())
        return false;

      expression = true;
    }

    /* <expression> := <expression> <binary_operator> <expression> */

    else if (expression && parse_.isOneOf(parser_binary_operator_chars)) {
      bool assign1 = false;

      skipBinaryOperator(&assign1);

      if (error_code_ != 0)
        return false;

      if (! skipExpression(assign))
        return false;

      if (! assign1 && *assign) {
        error_code_ = int(ClErr::INVALID_LVALUE_FOR_ASSIGNMENT);
        return false;
      }

      if (assign1)
        *assign = true;

      expression = true;
    }

    /* <expression> := <expression> '?' <expression> ':' <expression> */

    else if (expression && parse_.isChar('?')) {
      bool assign1 = false;
      bool assign2 = false;

      parse_.skipChar();

      if (! skipExpression(&assign1) && error_code_ != int(ClErr::INVALID_CHARACTER))
        return false;

      error_code_ = 0;

      parse_.skipSpace();

      if (! parse_.isChar(':')) {
        error_code_ = int(ClErr::MISSING_COLON_OPERATOR);
        return false;
      }

      parse_.skipChar();

      if (! skipExpression(&assign2))
        return false;

      expression = true;
      *assign    = assign1 | assign2;
    }

    /* <expression> := <expression> '[' <expression> ',' .... ']' */

    else if (expression && parse_.isChar('[')) {
      parse_.skipChar();

      if (! skipArray())
        return false;

      expression = true;
    }

    /* <expression> := <expression> ',' <expression> */

    else if (expression && parse_.isChar(',')) {
      bool assign1 = false;

      ClParserStackMgrInst->push(CL_PARSER_OP_COMMA);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      if (! skipExpression(&assign1))
        return false;

      expression = true;
    }

    /* <expression> := <variable>                               */
    /* <expression> := <variable> '.' <variable> '.' <...>      */
    /* <expression> := <variable> '[' <expression> ',' .... ']' */
    /* <expression> := <function> '(' <expression> ',' .... ')' */
    /* <expression> := <type>     '(' <expression> ',' .... ')' */

    else if (! expression && (parse_.isAlpha() || parse_.isChar('_'))) {
      skipIdentifier();

      if (error_code_ != 0)
        return false;

      parse_.skipSpace();

      if (parse_.isChar('(')) {
        parse_.skipChar();

        if (! skipArgList())
          return false;

        expression = true;
      }
      else {
        if (parse_.isChar('[')) {
          parse_.skipChar();

          if (! skipArray())
            return false;

          parse_.skipSpace();
        }

        while (parse_.isChar('.')) {
          parse_.skipChar();

          parse_.skipSpace();

          skipIdentifier();

          if (error_code_ != 0)
            return false;

          if (parse_.isChar('[')) {
            parse_.skipChar();

            if (! skipArray())
              return false;

            parse_.skipSpace();
          }
        }

        expression = true;
      }
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }

    parse_.skipSpace();
  }

  if (! expression) {
    error_code_ = int(ClErr::NULL_EXPRESSION);
    return false;
  }

  return true;
}

// Check whether the specified string, when evaluated,
// would be valid for the left hand side of an assignment.

bool
ClParserParser::
isValidAssign()
{
  parse_.skipSpace();

  if (parse_.eof())
    return false;

  if (! parse_.isAlpha() && ! parse_.isChar('_'))
    return false;

  skipIdentifier();

  parse_.skipSpace();

  while (parse_.isChar('[')) {
    parse_.skipChar();

    if (! skipArray())
      return false;

    parse_.skipSpace();
  }

  while (parse_.isChar('.')) {
    parse_.skipChar();

    parse_.skipSpace();

    if (! parse_.isAlpha() && ! parse_.isChar('_'))
      return false;

    skipIdentifier();

    while (parse_.isChar('[')) {
      parse_.skipChar();

      if (! skipArray())
        return false;

      parse_.skipSpace();
    }
  }

  if (! parse_.eof())
    return false;

  return true;
}

// Read a unary operator from the current position in
// the expression string.
//
// Note: The returned operator should not be freed.

bool
ClParserParser::
readUnaryOperator(ClParserOperatorType *op)
{
  *op = CL_PARSER_OP_NONE;

  if (parse_.eof())
    return false;

  switch (parse_.getCharAt()) {
    case '+': *op = CL_PARSER_OP_UNARY_PLUS ; parse_.skipChar(); break;
    case '-': *op = CL_PARSER_OP_UNARY_MINUS; parse_.skipChar(); break;
    case '!': *op = CL_PARSER_OP_LOGICAL_NOT; parse_.skipChar(); break;
    case '~': *op = CL_PARSER_OP_BIT_NOT    ; parse_.skipChar(); break;
    default : return false;
  }

  return true;
}

// Skip a unary operator from the current position in
// the expression string.

bool
ClParserParser::
skipUnaryOperator()
{
  if (parse_.eof())
    return false;

  switch (parse_.getCharAt()) {
    case '+': case '-': case '!': case '~':
      parse_.skipChar();
      break;
    default:
      return false;
  }

  return true;
}

// Read a binary operator from the current position in
// the expression string.

bool
ClParserParser::
readBinaryOperator(ClParserOperatorType *op)
{
  *op = CL_PARSER_OP_NONE;

  error_code_ = 0;

  switch (parse_.getCharAt()) {
    case '+':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_PLUS_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_PLUS;

      parse_.skipChar();

      break;
    case '-':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_MINUS_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_MINUS;

      parse_.skipChar();

      break;
    case '*':
      if      (parse_.isCharAfter('*')) {
        *op = CL_PARSER_OP_POWER;

        parse_.skipChar();
      }
      else if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_TIMES_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_TIMES;

      parse_.skipChar();

      break;
    case '/':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_DIVIDE_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_DIVIDE;

      parse_.skipChar();

      break;
    case '^':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_BIT_XOR_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_BIT_XOR;

      parse_.skipChar();

      break;
    case '<':
      if      (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_LESS_OR_EQUAL;

        parse_.skipChar();
      }
      else if (parse_.isCharAfter('<')) {
        if (parse_.isCharAfter('=', 2)) {
          *op = CL_PARSER_OP_BIT_LSHIFT_EQUALS;

          parse_.skipChar();
        }
        else
          *op = CL_PARSER_OP_BIT_LSHIFT;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_LESS;

      parse_.skipChar();

      break;
    case '>':
      if      (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_GREATER_OR_EQUAL;

        parse_.skipChar();
      }
      else if (parse_.isCharAfter('>')) {
        if (parse_.isCharAfter('=', 2)) {
          *op = CL_PARSER_OP_BIT_RSHIFT_EQUALS;

          parse_.skipChar();
        }
        else
          *op = CL_PARSER_OP_BIT_RSHIFT;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_GREATER;

      parse_.skipChar();

      break;
    case '=':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_EQUAL;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_EQUALS;

      parse_.skipChar();

      break;
    case '~':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_APPROX_EQUAL;

        parse_.skipChars(2);
      }
      else {
        error_code_ = int(ClErr::INVALID_OPERATOR);
        return false;
      }

      break;
    case '!':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_NOT_EQUAL;

        parse_.skipChars(2);
      }
      else {
        error_code_ = int(ClErr::INVALID_OPERATOR);
        return false;
      }

      break;
    case '&':
      if      (parse_.isCharAfter('&')) {
        *op = CL_PARSER_OP_LOGICAL_AND;

        parse_.skipChar();
      }
      else if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_BIT_AND_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_BIT_AND;

      parse_.skipChar();

      break;
    case '|':
      if      (parse_.isCharAfter('|')) {
        *op = CL_PARSER_OP_LOGICAL_OR;

        parse_.skipChar();
      }
      else if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_BIT_OR_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_BIT_OR;

      parse_.skipChar();

      break;
    case '%':
      if (parse_.isCharAfter('=')) {
        *op = CL_PARSER_OP_MODULUS_EQUALS;

        parse_.skipChar();
      }
      else
        *op = CL_PARSER_OP_MODULUS;

      parse_.skipChar();

      break;
    case '#':
      *op = CL_PARSER_OP_ARRAY_TIMES;

      parse_.skipChar();

      break;
    default:
      return false;
  }

  return true;
}

// Skip a binary operator from the current position in
// the expression string.

bool
ClParserParser::
skipBinaryOperator(bool *assign)
{
  *assign = false;

  switch (parse_.getCharAt()) {
    case '+':
      if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '-':
      if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '*':
      if      (parse_.isCharAfter('*'))
        parse_.skipChar();
      else if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '/':
      if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '^':
      if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '<':
      if      (parse_.isCharAfter('='))
        parse_.skipChar();
      else if (parse_.isCharAfter('<')) {
        if (parse_.isCharAfter('=', 2)) {
          parse_.skipChar();

          *assign = true;
        }

        parse_.skipChar();
      }

      parse_.skipChar();

      break;
    case '>':
      if      (parse_.isCharAfter('='))
        parse_.skipChar();
      else if (parse_.isCharAfter('>')) {
        if (parse_.isCharAfter('=', 2)) {
          parse_.skipChar();

          *assign = true;
        }

        parse_.skipChar();
      }

      parse_.skipChar();

      break;
    case '=':
      if (parse_.isCharAfter('='))
        parse_.skipChar();
      else
        *assign = true;

      parse_.skipChar();

      break;
    case '~':
      if (parse_.isCharAfter('='))
        parse_.skipChars(2);

      break;
    case '!':
      if (parse_.isCharAfter('='))
        parse_.skipChars(2);

      break;
    case '&':
      if      (parse_.isCharAfter('&'))
        parse_.skipChar();
      else if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '|':
      if      (parse_.isCharAfter('|'))
        parse_.skipChar();
      else if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '%':
      if (parse_.isCharAfter('=')) {
        parse_.skipChar();

        *assign = true;
      }

      parse_.skipChar();

      break;
    case '#':
      parse_.skipChar();

      break;
    default:
      return false;
  }

  return true;
}

// Read a numeric (integer or real) value from the current
// position in the expression string.
//
// Note: An integer can be in hexadecimal or decimal form.

bool
ClParserParser::
readNumericValue(ClParserValuePtr &value)
{
  if (parse_.isChar('0') && (parse_.isCharAfter('x') || parse_.isCharAfter('X'))) {
    parse_.skipChars(2);

    if (! readNumericHexValue(value))
      return false;
  }
  else if (parse_.isChar('0') && CStrUtil::isodigit(parse_.getCharAfter())) {
    if (! readNumericOctValue(value))
      return false;
  }
  else {
    if (! readNumericDecValue(value))
      return false;
  }

  return true;
}

// Skip a numeric (integer or real) value from the current
// position in the expression string.
//
// Note: An integer can be in hexadecimal or decimal form.

bool
ClParserParser::
skipNumericValue()
{
  if (parse_.isChar('0') &&
      (parse_.isCharAfter('x') || parse_.isCharAfter('X'))) {
    parse_.skipChars(2);

    skipNumericHexValue();
  }
  else {
    if (! skipNumericDecValue())
      return false;
  }

  return true;
}

// Read a hexadecimal integer value from the current
// position in the expression string.
//
// Note: A Hexidecimal Number is an integer preceded by '0x' or '0X'.

bool
ClParserParser::
readNumericHexValue(ClParserValuePtr &value)
{
  std::string parse_string;

  while (! parse_.eof() && parse_.isXDigit()) {
    parse_string += parse_.getCharAt();

    parse_.skipChar();
  }

  uint len = uint(parse_string.size());

  /*----------*/

  long hex_value = 0;

  for (uint i = 0; i < len; i++) {
    hex_value *= 16;

    switch (parse_string[i]) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        hex_value += parse_string[i] - '0';
        break;
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        hex_value += parse_string[i] - 'a' + 10;
        break;
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        hex_value += parse_string[i] - 'A' + 10;
        break;
      default:
        break;
    }
  }

  value = ClParserValueMgrInst->createValue(hex_value);

  return true;
}

// Skip a hexadecimal integer value from the current
// position in the expression string.

bool
ClParserParser::
skipNumericHexValue()
{
  while (! parse_.eof() && parse_.isXDigit())
    parse_.skipChar();

  return true;
}

// Read an octal integer value from the current
// position in the expression string.
//
// Note: An Octal Number is an integer starting with '0' and
// followed by an octal digit ('0' - '7').

bool
ClParserParser::
readNumericOctValue(ClParserValuePtr &value)
{
  std::string parse_string;

  while (! parse_.eof() && parse_.isODigit()) {
    parse_string += parse_.getCharAt();

    parse_.skipChar();
  }

  uint len = uint(parse_string.size());

  /*----------*/

  long oct_value = 0;

  for (uint i = 0; i < len; i++) {
    oct_value *= 8;

    oct_value += parse_string[i] - '0';
  }

  value = ClParserValueMgrInst->createValue(oct_value);

  return true;
}

// Read a numeric (integer or real) value from the current
// position in the expression string.

bool
ClParserParser::
readNumericDecValue(ClParserValuePtr &value)
{
  std::string parse_string;

  while (! parse_.eof() && parse_.isDigit()) {
    parse_string += parse_.getCharAt();

    parse_.skipChar();
  }

  bool point_found = false;

  if (! parse_.eof() && parse_.isChar('.')) {
    point_found = true;

    parse_string += parse_.getCharAt();

    parse_.skipChar();

    while (! parse_.eof() && parse_.isDigit()) {
      parse_string += parse_.getCharAt();

      parse_.skipChar();
    }
  }

  bool exponent_found = false;

  if (! parse_.eof() && (parse_.isChar('e') || parse_.isChar('E'))) {
    exponent_found = true;

    parse_string += parse_.getCharAt();

    parse_.skipChar();

    if (! parse_.eof() && (parse_.isChar('+') || parse_.isChar('-'))) {
      parse_string += parse_.getCharAt();

      parse_.skipChar();
    }

    if (parse_.eof() || ! parse_.isDigit()) {
      error_code_ = int(ClErr::MISSING_EXPONENTIAL_VALUE);
      return false;
    }

    while (! parse_.eof() && parse_.isDigit()) {
      parse_string += parse_.getCharAt();

      parse_.skipChar();
    }
  }

  double real;

  if (ClParserInst->getParseAsReals() || point_found || exponent_found) {
    if (sscanf(parse_string.c_str(), "%lf", &real) != 1) {
      error_code_ = int(ClErr::INVALID_STRING_TO_REAL_CONV);
      return false;
    }

    value = ClParserValueMgrInst->createValue(real);

    return true;
  }

  long integer;

  if (sscanf(parse_string.c_str(), "%ld", &integer) != 1) {
    error_code_ = int(ClErr::INVALID_STRING_TO_INTEGER_CONV);
    return false;
  }

  if (sscanf(parse_string.c_str(), "%lf", &real) != 1) {
    error_code_ = int(ClErr::INVALID_STRING_TO_INTEGER_CONV);
    return false;
  }

  if (double(integer) != real)
    value = ClParserValueMgrInst->createValue(real);
  else
    value = ClParserValueMgrInst->createValue(integer);

  return true;
}

// Skip a numeric (integer or real) value from the current
// position in the expression string.

bool
ClParserParser::
skipNumericDecValue()
{
  //bool point_found    = false;
  //bool exponent_found = false;

  while (! parse_.eof() && parse_.isDigit())
    parse_.skipChar();

  if (parse_.isChar('.')) {
    //point_found = true;

    parse_.skipChar();

    while (! parse_.eof() && parse_.isDigit())
      parse_.skipChar();
  }

  if (parse_.isChar('e') || parse_.isChar('E')) {
    //exponent_found = true;

    parse_.skipChar();

    if (parse_.isChar('+') || parse_.isChar('-'))
      parse_.skipChar();

    if (parse_.eof() || ! parse_.isDigit())
      return false;

    while (! parse_.eof() && parse_.isDigit())
      parse_.skipChar();
  }

  return true;
}

// Read a string value from the current position in the
// expression string.

bool
ClParserParser::
readStringValue(ClParserValuePtr &value)
{
  char end_char  = parse_.getCharAt();
  char last_char = '\0';

  parse_.skipChar();

  std::string parse_string;

  int c;

  while (! parse_.eof()) {
    if (last_char == '\\') {
      if      (parse_.isOneOf("01234567")) {
        c = parse_.getCharAt() - '0';

        parse_.skipChar();

        if (! parse_.eof() && parse_.isODigit()) {
          c = 8*c + (parse_.getCharAt() - '0');

          parse_.skipChar();

          if (! parse_.eof() && parse_.isODigit()) {
            c = 8*c + (parse_.getCharAt() - '0');

            parse_.skipChar();
          }
        }

        parse_string += char(c);
      }
      else if (parse_.isChar('x')) {
        c = 0;

        parse_.skipChar();

        while (! parse_.eof() && parse_.isXDigit()) {
          if      (parse_.getCharAt() >= '0' && parse_.getCharAt() <= '9')
            c = 16*c + (parse_.getCharAt() - '0');
          else if (parse_.getCharAt() >= 'a' && parse_.getCharAt() <= 'f')
            c = 16*c + (parse_.getCharAt() - 'a' + 10);
          else
            c = 16*c + (parse_.getCharAt() - 'A' + 10);

          parse_.skipChar();
        }

        parse_string += char(c);
      }
      else {
        switch (parse_.getCharAt()) {
          case 'n' : c = '\n'; break;
          case 't' : c = '\t'; break;
          case 'v' : c = '\v'; break;
          case 'b' : c = '\b'; break;
          case 'r' : c = '\r'; break;
          case 'f' : c = '\f'; break;
          case 'a' : c = ''; break;
          case '\\': c = '\\'; break;
          case '?' : c = '?'; break;
          case '\'': c = '\''; break;
          case '\"': c = '\"'; break;
          default  : {
            parse_string += '\\';

            c = parse_.getCharAt();

            break;
          }
        }

        parse_string += char(c);

        parse_.skipChar();
      }

      if (! parse_.eof() && ! parse_.isChar('\\'))
        last_char = parse_.getCharAt();
      else
        last_char = '\0';
    }
    else {
      last_char = parse_.getCharAt();

      if (parse_.isChar(end_char))
        break;

      if (! parse_.isChar('\\'))
        parse_string += parse_.getCharAt();

      parse_.skipChar();
    }
  }

  if (parse_.eof()) {
    error_code_ = int(ClErr::UNTERMINATED_STRING);
    return false;
  }

  parse_.skipChar();

  value = ClParserValueMgrInst->createValue(parse_string);

  return true;
}

// Read a string value from the current position in the
// expression string.

bool
ClParserParser::
skipStringValue()
{
  char end_char  = parse_.getCharAt();
  char last_char = '\0';

  parse_.skipChar();

  while (! parse_.eof()) {
    if (last_char == '\\') {
      parse_.skipChar();

      if (! parse_.isChar('\\'))
        last_char = parse_.getCharAt();
      else
        last_char = '\0';
    }
    else {
      last_char = parse_.getCharAt();

      if (parse_.isChar(end_char))
        break;

      parse_.skipChar();
    }
  }

  if (parse_.eof())
    return false;

  parse_.skipChar();

  return true;
}

// Read an identifier from the current position in the
// expression string.

bool
ClParserParser::
readIdentifier(ClParserIdentifierPtr &identifier)
{
  StringVectorT scopes;
  std::string   idstr, str;

  if (! parse_.readIdentifier(idstr))
    return false;

  while (parse_.isString("::")) {
    parse_.skipChars(2);

    if (! parse_.readIdentifier(str))
      break;

    scopes.push_back(idstr);

    idstr = str;
  }

  identifier = ClParserIdentifierMgrInst->createIdentifier(idstr);

  auto p1 = scopes.begin();
  auto p2 = scopes.end  ();

  for ( ; p1 != p2; ++p1)
    identifier->addScope(*p1);

  return true;
}

// Skip an identifier from the current position in the
// expression str.

bool
ClParserParser::
skipIdentifier()
{
  std::string parse_string;

  parse_.readIdentifier(parse_string);

  return true;
}

// Stack a comma separated list of expressions onto the
// evaluation stack. The argument list is terminated
// by a close round bracket.
//
// Note: The terminating close round bracket is added to the
// evaluation stack.

bool
ClParserParser::
readArgList()
{
  bool in_brackets = true;

  while (in_brackets) {
    ClParserParser parser(this);

    if (! parser.parse() && parser.getErrorCode() != int(ClErr::INVALID_CHARACTER)) {
      error_code_ = parser.getErrorCode();
      return false;
    }

    if      (parse_.isChar(',')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_COMMA);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();
    }
    else if (parse_.isChar(')')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_CLOSE_R_BRACKET);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

// Skip a comma separated list of expressions in a string.
// The argument list is terminated by a close round bracket.

bool
ClParserParser::
skipArgList()
{
  bool in_brackets = true;

  while (in_brackets) {
    bool assign;

    if (! skipExpression(&assign) && error_code_ != int(ClErr::INVALID_CHARACTER))
      return false;

    if      (parse_.isChar(','))
      parse_.skipChar();
    else if (parse_.isChar(')')) {
      parse_.skipChar();

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

// Stack a comma separated list of expressions onto the
// evaluation stack for an array. The array is terminated
// by a close square bracket.
//
// Note: The terminating close square bracket is added to the
// evaluation stack.

bool
ClParserParser::
readArray()
{
  bool in_brackets = true;

  while (in_brackets) {
    ClParserParser parser(this);

    if (! parser.parse() && parser.getErrorCode() != int(ClErr::INVALID_CHARACTER)) {
      error_code_ = parser.getErrorCode();
      return false;
    }

    if      (parse_.isChar(',')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_COMMA);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();
    }
    else if (parse_.isChar(']')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_CLOSE_S_BRACKET);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

// Skip a comma separated list of expressions in a string
// for an array. The array is terminated by a close square
// bracket.

bool
ClParserParser::
skipArray()
{
  bool in_brackets = true;

  while (in_brackets) {
    bool assign;

    if (! skipExpression(&assign) && error_code_ != int(ClErr::INVALID_CHARACTER))
      return false;

    if      (parse_.isChar(','))
      parse_.skipChar();
    else if (parse_.isChar(']')) {
      parse_.skipChar();

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

// Stack a comma separated list of expressions onto the
// evaluation stack for a list. The list is terminated
// by a close brace.
//
// Note: The terminating close brace is added to the
// evaluation stack.

bool
ClParserParser::
readList()
{
  bool in_brackets = true;

  while (in_brackets) {
    ClParserParser parser(this);

    if (! parser.parse() && parser.getErrorCode() != int(ClErr::INVALID_CHARACTER)) {
      error_code_ = parser.getErrorCode();
      return false;
    }

    if      (parse_.isChar(',')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_COMMA);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();
    }
    else if (parse_.isChar('}') && ! parse_.isCharAfter('}')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_CLOSE_BRACE);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

// Skip a comma separated list of expressions in a string
// for a list. The list is terminated by a close brace.

bool
ClParserParser::
skipList()
{
  bool in_brackets = true;

  while (in_brackets) {
    bool assign;

    if (! skipExpression(&assign) && error_code_ != int(ClErr::INVALID_CHARACTER))
      return false;

    if      (parse_.isChar(','))
      parse_.skipChar();
    else if (parse_.isChar('}') && ! parse_.isCharAfter('}')) {
      parse_.skipChar();

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

// Stack a comma separated list of expressions onto the
// evaluation stack for a dictionary. The dictionary is
// terminated by a double close brace.
//
// Note: The terminating close double close brace is added to
// the evaluation stack.

bool
ClParserParser::
readDictionary()
{
  bool in_brackets = true;

  while (in_brackets) {
    ClParserParser parser(this);

    if (! parser.parse() && parser.getErrorCode() != int(ClErr::INVALID_CHARACTER)) {
      error_code_ = parser.getErrorCode();
      return false;
    }

    if      (parse_.isChar(',')) {
      ClParserStackMgrInst->push(CL_PARSER_OP_COMMA);

      ClParserStackMgrInst->toNext();

      parse_.skipChar();
    }
    else if (parse_.isString("}}")) {
      ClParserStackMgrInst->push(CL_PARSER_OP_CLOSE_DICT);

      ClParserStackMgrInst->toNext();

      parse_.skipChars(2);

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

// Skip a comma separated list of expressions in a string
// for a dictionary. The list is terminated by a double
// close brace.

bool
ClParserParser::
skipDictionary()
{
  bool in_brackets = true;

  while (in_brackets) {
    bool assign;

    if (! skipExpression(&assign) && error_code_ != int(ClErr::INVALID_CHARACTER))
      return false;

    if      (parse_.isChar(','))
      parse_.skipChar();
    else if (parse_.isString("}}")) {
      parse_.skipChars(2);

      in_brackets = false;
    }
    else {
      error_code_ = int(ClErr::INVALID_CHARACTER);
      return false;
    }
  }

  return true;
}

void
ClParserParser::
skipSpace()
{
  parse_.skipSpace();
}
