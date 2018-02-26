#include <CCeilPI.h>

/**** Internal Defines ****/

#define PARSER_I__ CL_PARSER_VALUE_TYPE_INTEGER
#define PARSER__R_ CL_PARSER_VALUE_TYPE_REAL
#define PARSER___S CL_PARSER_VALUE_TYPE_STRING

#define PARSER_IR_ (PARSER_I__ | CL_PARSER_VALUE_TYPE_REAL)
#define PARSER__RS (PARSER__R_ | CL_PARSER_VALUE_TYPE_STRING)
#define PARSER_I_S (PARSER___S | CL_PARSER_VALUE_TYPE_INTEGER)
#define PARSER_IRS (PARSER_IR_ | CL_PARSER_VALUE_TYPE_STRING)

/**** Internal Data ****/

static ClParserArg parser_ir__arg = { PARSER_IR_, false, };
static ClParserArg parser_i_s_arg = { PARSER_I_S, false, };
static ClParserArg parser_irs_arg = { PARSER_IRS, false, };
static ClParserArg parser__r__arg = { PARSER__R_, false, };
static ClParserArg parser__rs_arg = { PARSER__RS, false, };
static ClParserArg parser___s_arg = { PARSER___S, false, };

static ClParserArg parser_arr_arg = { CL_PARSER_VALUE_TYPE_ARRAY, false, };

static ClParserArg
parser_any_arg = {
  CL_PARSER_VALUE_TYPE_REAL       |
  CL_PARSER_VALUE_TYPE_INTEGER    |
  CL_PARSER_VALUE_TYPE_STRING     |
  CL_PARSER_VALUE_TYPE_ARRAY      |
  CL_PARSER_VALUE_TYPE_LIST       |
  CL_PARSER_VALUE_TYPE_DICTIONARY |
  CL_PARSER_VALUE_TYPE_STRUCTURE,
  false,
};

static ClParserArg
parser_addr_args[1] = {
  { PARSER_I_S, true, },
};

static ClParserArg
parser_indarr_args[1] = {
  { PARSER_I__, true, },
};

static ClParserArg
parser_atan2_args[2] = {
  { PARSER__R_, false, },
  { PARSER__R_, false, },
};

static ClParserArg
parser_arrcat_args[2] = {
  {
    CL_PARSER_VALUE_TYPE_ARRAY     |
    CL_PARSER_VALUE_TYPE_LIST      |
    CL_PARSER_VALUE_TYPE_DICTIONARY,
    false,
  },
  {
    CL_PARSER_VALUE_TYPE_ARRAY     |
    CL_PARSER_VALUE_TYPE_LIST      |
    CL_PARSER_VALUE_TYPE_DICTIONARY,
    false,
  },
};

static ClParserArg
parser_index_args[2] = {
  { PARSER___S, false, },
  { PARSER___S, false, },
};

static ClParserArg
parser_isctype_args[2] = {
  { PARSER_I__, false, },
  { PARSER___S, false, },
};

static ClParserArg
parser_len_args[1] = {
  {
    CL_PARSER_VALUE_TYPE_STRING    |
    CL_PARSER_VALUE_TYPE_ARRAY     |
    CL_PARSER_VALUE_TYPE_LIST      |
    CL_PARSER_VALUE_TYPE_DICTIONARY,
    false,
  },
};

static ClParserArg
parser_max_args[1] = {
  {
    CL_PARSER_VALUE_TYPE_STRING |
    CL_PARSER_VALUE_TYPE_ARRAY,
    false,
  },
};

static ClParserArg
parser_min_args[1] = {
  {
    CL_PARSER_VALUE_TYPE_STRING |
    CL_PARSER_VALUE_TYPE_ARRAY,
    false,
  },
};

static ClParserArg
parser_sort_args[1] = {
  { PARSER_IRS, true, },
};

static ClParserArg
parser_subarr_args[3] = {
  { PARSER_IRS, false, },
  { PARSER_I__, false, },
  { PARSER_I__, false, },
};

static ClParserArg
parser_typarr_args[2] = {
  { PARSER___S, false, },
  { PARSER_I__, true , },
};

static ClParserArg
parser_where_args[2] = {
  { PARSER___S, false, },
  { PARSER___S, false, },
};

static ClParserInternFnData
parser_internfn[] = {
 { CLParserInternFnType::ABS        , "abs"     , 1,     &parser_ir__arg, false, },
 { CLParserInternFnType::ACOS       , "acos"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::ADDR       , "addr"    , 1,    parser_addr_args, false, },
 { CLParserInternFnType::ARRAY_I    , "arrayI"  , 1,  parser_indarr_args, false, },
 { CLParserInternFnType::ARRAY_R    , "arrayR"  , 1,  parser_indarr_args, false, },
 { CLParserInternFnType::ARRAY_S    , "arrayS"  , 1,  parser_indarr_args, false, },
 { CLParserInternFnType::ARRCAT     , "arrcat"  , 2,  parser_arrcat_args, true , },
 { CLParserInternFnType::ASIN       , "asin"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::ASSERT     , "assert"  , 1,     &parser_irs_arg, false, },
 { CLParserInternFnType::ATAN       , "atan"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::ATAN2      , "atan2"   , 2,   parser_atan2_args, false, },
 { CLParserInternFnType::CEIL       , "ceil"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::CHAR_CONV  , "char"    , 1,     &parser_i_s_arg, false, },
 { CLParserInternFnType::COS        , "cos"     , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::COSH       , "cosh"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::DIM        , "dim"     , 1,     &parser_arr_arg, true , },
 { CLParserInternFnType::EXP        , "exp"     , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::FLOOR      , "floor"   , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::INDARR_I   , "indarrI" , 1,  parser_indarr_args, false, },
 { CLParserInternFnType::INDARR_R   , "indarrR" , 1,  parser_indarr_args, false, },
 { CLParserInternFnType::INDARR_S   , "indarrS" , 1,  parser_indarr_args, false, },
 { CLParserInternFnType::INDEX      , "index"   , 2,   parser_index_args, false, },
 { CLParserInternFnType::INT_CONV   , "int"     , 1,     &parser_i_s_arg, false, },
 { CLParserInternFnType::INVNORM    , "invnorm" , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::IS_NAN     , "isNaN"   , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::IS_CTYPE   , "isctype" , 2, parser_isctype_args, false, },
 { CLParserInternFnType::LEN        , "len"     , 1,     parser_len_args, true , },
 { CLParserInternFnType::LOG        , "log"     , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::LOG10      , "log10"   , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::MAX        , "max"     , 1,     parser_max_args, true , },
 { CLParserInternFnType::MIN        , "min"     , 1,     parser_min_args, true , },
 { CLParserInternFnType::NDIM       , "ndim"    , 1,     &parser_arr_arg, true , },
 { CLParserInternFnType::NORM       , "norm"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::REAL_CONV  , "real"    , 1,     &parser__rs_arg, false, },
 { CLParserInternFnType::RINDEX     , "rindex"  , 2,   parser_index_args, false, },
 { CLParserInternFnType::SIGN       , "sign"    , 1,     &parser_ir__arg, false, },
 { CLParserInternFnType::SIN        , "sin"     , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::SINH       , "sinh"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::SORT       , "sort"    , 1,    parser_sort_args, false, },
 { CLParserInternFnType::SQR        , "sqr"     , 1,     &parser_ir__arg, false, },
 { CLParserInternFnType::SQRT       , "sqrt"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::STRING_CONV, "string"  , 1,     &parser_irs_arg, false, },
 { CLParserInternFnType::SUBARR     , "subarr"  , 3,  parser_subarr_args, true , },
 { CLParserInternFnType::SUM        , "sum"     , 1,     &parser_arr_arg, true , },
 { CLParserInternFnType::TAN        , "tan"     , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::TANH       , "tanh"    , 1,     &parser__r__arg, false, },
 { CLParserInternFnType::TOLOWER    , "tolower" , 1,     &parser___s_arg, false, },
 { CLParserInternFnType::TOUPPER    , "toupper" , 1,     &parser___s_arg, false, },
 { CLParserInternFnType::TYPARR     , "typarr"  , 2,  parser_typarr_args, false, },
 { CLParserInternFnType::VALTYP     , "valtyp"  , 1,     &parser_any_arg, false, },
 { CLParserInternFnType::WHERE      , "where"   , 2,   parser_where_args, false, },
 { }
};
