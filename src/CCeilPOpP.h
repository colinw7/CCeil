#include <CCeilPI.h>

/**** Internal Defines ****/

#define PARSER_I__ CL_PARSER_VALUE_TYPE_INTEGER
#define PARSER__R_ CL_PARSER_VALUE_TYPE_REAL
#define PARSER___S CL_PARSER_VALUE_TYPE_STRING
#define PARSER_IR_ CL_PARSER_VALUE_TYPE_INTEGER | CL_PARSER_VALUE_TYPE_REAL
#define PARSER_I_S CL_PARSER_VALUE_TYPE_INTEGER | CL_PARSER_VALUE_TYPE_STRING
#define PARSER__RS CL_PARSER_VALUE_TYPE_REAL    | CL_PARSER_VALUE_TYPE_STRING
#define PARSER_IRS CL_PARSER_VALUE_TYPE_INTEGER | CL_PARSER_VALUE_TYPE_REAL | \
                   CL_PARSER_VALUE_TYPE_STRING

/**** External Data ****/

/* Inline Operators */

ClParserOperatorData
cl_increment_op = {
  CL_PARSER_OP_INCREMENT,
  "++",
  "Increment",
  14,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_IR_,
};

ClParserOperatorData
cl_decrement_op = {
  CL_PARSER_OP_DECREMENT,
  "--",
  "Decrement",
  14,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_IR_,
};

/* Punctuation Operators */

ClParserOperatorData
cl_open_rbracket_op = {
  CL_PARSER_OP_OPEN_R_BRACKET,
  "(",
  "Open Round Bracket",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_close_rbracket_op = {
  CL_PARSER_OP_CLOSE_R_BRACKET,
  ")",
  "Close Round Bracket",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_open_sbracket_op = {
  CL_PARSER_OP_OPEN_S_BRACKET,
  "[",
  "Open Square Bracket",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_close_sbracket_op = {
  CL_PARSER_OP_CLOSE_S_BRACKET,
  "]",
  "Close Square Bracket",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_open_brace_op = {
  CL_PARSER_OP_OPEN_BRACE,
  "{",
  "Open Brace",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_close_brace_op = {
  CL_PARSER_OP_CLOSE_BRACE,
  "}",
  "Close Brace",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_open_dict_op = {
  CL_PARSER_OP_OPEN_DICT,
  "{{",
  "Open Dict",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_close_dict_op = {
  CL_PARSER_OP_CLOSE_DICT,
  "}}",
  "Close Dict",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_comma_op = {
  CL_PARSER_OP_COMMA,
  ",",
  "Comma",
  0,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_question_op = {
  CL_PARSER_OP_QUESTION_MARK,
  "?",
  "Question Mark",
  2,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_I__,
};

ClParserOperatorData
cl_colon_op = {
  CL_PARSER_OP_COLON,
  ":",
  "Colon",
  2,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_I__,
};

ClParserOperatorData
cl_dot_op = {
  CL_PARSER_OP_DOT,
  ".",
  "Dot",
  15,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

/* Unary Operators */

ClParserOperatorData
cl_unary_plus_op = {
  CL_PARSER_OP_UNARY_PLUS,
  "+",
  "Unary Plus",
  14,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_IR_,
};

ClParserOperatorData
cl_unary_minus_op = {
  CL_PARSER_OP_UNARY_MINUS,
  "-",
  "Unary Minus",
  14,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_IR_,
};

ClParserOperatorData
cl_logical_not_op = {
  CL_PARSER_OP_LOGICAL_NOT,
  "!",
  "Logical Not",
  14,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_I_S,
};

ClParserOperatorData
cl_bitwise_not_op = {
  CL_PARSER_OP_BIT_NOT,
  "~",
  "Bitwise Not",
  14,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_I__,
};

/* Binary Operators */

ClParserOperatorData
cl_plus_op = {
  CL_PARSER_OP_PLUS,
  "+",
  "Plus",
  11,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_minus_op = {
  CL_PARSER_OP_MINUS,
  "-",
  "Minus",
  11,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_times_op = {
  CL_PARSER_OP_TIMES,
  "*",
  "Times",
  12,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IR_,
};

ClParserOperatorData
cl_divide_op = {
  CL_PARSER_OP_DIVIDE,
  "/",
  "Divide",
  12,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_modulus_op = {
  CL_PARSER_OP_MODULUS,
  "%",
  "Modulus",
  12,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_power_op = {
  CL_PARSER_OP_POWER,
  "**",
  "Power",
  13,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_IR_,
};

ClParserOperatorData
cl_less_op = {
  CL_PARSER_OP_LESS,
  "<",
  "Less Than",
  9,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_less_equal_op = {
  CL_PARSER_OP_LESS_OR_EQUAL,
  "<=",
  "Less Than Or Equal",
  9,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_greater_op = {
  CL_PARSER_OP_GREATER,
  ">",
  "Greater Than",
  9,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_greater_equal_op = {
  CL_PARSER_OP_GREATER_OR_EQUAL,
  ">=",
  "Greater Than Or Equal",
  9,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_equal_op = {
  CL_PARSER_OP_EQUAL,
  "==",
  "Equal",
  8,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_not_equal_op = {
  CL_PARSER_OP_NOT_EQUAL,
  "!=",
  "Not Equal",
  8,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_approx_equal_op = {
  CL_PARSER_OP_APPROX_EQUAL,
  "~=",
  "Approx. Equal",
  8,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_logical_and_op = {
  CL_PARSER_OP_LOGICAL_AND,
  "&&",
  "Logical AND",
  4,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_logical_or_op = {
  CL_PARSER_OP_LOGICAL_OR,
  "||",
  "Logical OR",
  3,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_bitwise_and_op = {
  CL_PARSER_OP_BIT_AND,
  "&",
  "Bitwise AND",
  7,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_bitwise_or_op = {
  CL_PARSER_OP_BIT_OR,
  "|",
  "Bitwise OR",
  5,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_bitwise_xor_op = {
  CL_PARSER_OP_BIT_XOR,
  "^",
  "Bitwise XOR",
  6,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_lshift_op = {
  CL_PARSER_OP_BIT_LSHIFT,
  "<<",
  "Bit Shift Left",
  10,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_rshift_op = {
  CL_PARSER_OP_BIT_RSHIFT,
  ">>",
  "Bit Shift Right",
  10,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_array_times_op = {
  CL_PARSER_OP_ARRAY_TIMES,
  "#",
  "Array Multiplier",
  10,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IR_,
};

/* Assignment Operators */

ClParserOperatorData
cl_equals_op = {
  CL_PARSER_OP_EQUALS,
  "=",
  "Equals",
  1,
  CLParserOperatorAssociate::R_TO_L,
  PARSER_IRS,
};

ClParserOperatorData
cl_plus_equals_op = {
  CL_PARSER_OP_PLUS_EQUALS,
  "+=",
  "Plus Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_minus_equals_op = {
  CL_PARSER_OP_MINUS_EQUALS,
  "-=",
  "Minus Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IRS,
};

ClParserOperatorData
cl_times_equals_op = {
  CL_PARSER_OP_TIMES_EQUALS,
  "*=",
  "Times Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IR_,
};

ClParserOperatorData
cl_divide_equals_op = {
  CL_PARSER_OP_DIVIDE_EQUALS,
  "/=",
  "Divide Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_IR_,
};

ClParserOperatorData
cl_modulus_equals_op = {
  CL_PARSER_OP_MODULUS_EQUALS,
  "%=",
  "Modulus Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_bitwise_and_equals_op = {
  CL_PARSER_OP_BIT_AND_EQUALS,
  "&=",
  "Bitwise AND Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_bitwise_or_equals_op = {
  CL_PARSER_OP_BIT_OR_EQUALS,
  "|=",
  "Bitwise OR Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_bitwise_xor_equals_op = {
  CL_PARSER_OP_BIT_XOR_EQUALS,
  "^=",
  "Bitwise XOR Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_lshift_equals_op = {
  CL_PARSER_OP_BIT_LSHIFT_EQUALS,
  "<<=",
  "Bit Shift Left Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

ClParserOperatorData
cl_rshift_equals_op = {
  CL_PARSER_OP_BIT_RSHIFT_EQUALS,
  ">>=",
  "Bit Shift Right Equals",
  1,
  CLParserOperatorAssociate::L_TO_R,
  PARSER_I__,
};

/**** Internal Data ****/

static ClParserOperatorData *
parser_operators[] = {
  /* Inline Operators */

  &cl_increment_op,
  &cl_decrement_op,

  /* Punctuation Operators */

  &cl_open_rbracket_op,
  &cl_close_rbracket_op,
  &cl_open_sbracket_op,
  &cl_close_sbracket_op,
  &cl_open_brace_op,
  &cl_close_brace_op,
  &cl_open_dict_op,
  &cl_close_dict_op,
  &cl_comma_op,
  &cl_question_op,
  &cl_colon_op,
  &cl_dot_op,

  /* Unary Operators */

  &cl_unary_plus_op,
  &cl_unary_minus_op,
  &cl_logical_not_op,
  &cl_bitwise_not_op,

  /* Binary Operators */

  &cl_plus_op,
  &cl_minus_op,
  &cl_times_op,
  &cl_divide_op,
  &cl_modulus_op,
  &cl_power_op,
  &cl_less_op,
  &cl_less_equal_op,
  &cl_greater_op,
  &cl_greater_equal_op,
  &cl_equal_op,
  &cl_not_equal_op,
  &cl_approx_equal_op,
  &cl_logical_and_op,
  &cl_logical_or_op,
  &cl_bitwise_and_op,
  &cl_bitwise_or_op,
  &cl_bitwise_xor_op,
  &cl_lshift_op,
  &cl_rshift_op,
  &cl_array_times_op,

  /* Assignment Operators */

  &cl_equals_op,
  &cl_plus_equals_op,
  &cl_minus_equals_op,
  &cl_times_equals_op,
  &cl_divide_equals_op,
  &cl_modulus_equals_op,
  &cl_bitwise_and_equals_op,
  &cl_bitwise_or_equals_op,
  &cl_bitwise_xor_equals_op,
  &cl_lshift_equals_op,
  &cl_rshift_equals_op,

  NULL,
};
