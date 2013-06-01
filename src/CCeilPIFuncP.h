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

#define PARSER_NO_INTERN_FNS 48

static ClParserInternFnData
parser_internfn[PARSER_NO_INTERN_FNS] = {
 { CL_PARSER_INTERNFN_ABS        , "abs"     , 1,     &parser_ir__arg, false, },
 { CL_PARSER_INTERNFN_ACOS       , "acos"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_ADDR       , "addr"    , 1,    parser_addr_args, false, },
 { CL_PARSER_INTERNFN_ARRAY_I    , "arrayI"  , 1,  parser_indarr_args, false, },
 { CL_PARSER_INTERNFN_ARRAY_R    , "arrayR"  , 1,  parser_indarr_args, false, },
 { CL_PARSER_INTERNFN_ARRAY_S    , "arrayS"  , 1,  parser_indarr_args, false, },
 { CL_PARSER_INTERNFN_ARRCAT     , "arrcat"  , 2,  parser_arrcat_args, true , },
 { CL_PARSER_INTERNFN_ASIN       , "asin"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_ATAN       , "atan"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_ATAN2      , "atan2"   , 2,   parser_atan2_args, false, },
 { CL_PARSER_INTERNFN_CEIL       , "ceil"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_CHAR_CONV  , "char"    , 1,     &parser_i_s_arg, false, },
 { CL_PARSER_INTERNFN_COS        , "cos"     , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_COSH       , "cosh"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_DIM        , "dim"     , 1,     &parser_arr_arg, true , },
 { CL_PARSER_INTERNFN_EXP        , "exp"     , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_FLOOR      , "floor"   , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_INDARR_I   , "indarrI" , 1,  parser_indarr_args, false, },
 { CL_PARSER_INTERNFN_INDARR_R   , "indarrR" , 1,  parser_indarr_args, false, },
 { CL_PARSER_INTERNFN_INDARR_S   , "indarrS" , 1,  parser_indarr_args, false, },
 { CL_PARSER_INTERNFN_INDEX      , "index"   , 2,   parser_index_args, false, },
 { CL_PARSER_INTERNFN_INT_CONV   , "int"     , 1,     &parser_i_s_arg, false, },
 { CL_PARSER_INTERNFN_IS_NAN     , "isNaN"   , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_IS_CTYPE   , "isctype" , 2, parser_isctype_args, false, },
 { CL_PARSER_INTERNFN_LEN        , "len"     , 1,     parser_len_args, true , },
 { CL_PARSER_INTERNFN_LOG        , "log"     , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_LOG10      , "log10"   , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_MAX        , "max"     , 1,     parser_max_args, true , },
 { CL_PARSER_INTERNFN_MIN        , "min"     , 1,     parser_min_args, true , },
 { CL_PARSER_INTERNFN_NDIM       , "ndim"    , 1,     &parser_arr_arg, true , },
 { CL_PARSER_INTERNFN_REAL_CONV  , "real"    , 1,     &parser__rs_arg, false, },
 { CL_PARSER_INTERNFN_RINDEX     , "rindex"  , 2,   parser_index_args, false, },
 { CL_PARSER_INTERNFN_SIGN       , "sign"    , 1,     &parser_ir__arg, false, },
 { CL_PARSER_INTERNFN_SIN        , "sin"     , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_SINH       , "sinh"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_SORT       , "sort"    , 1,    parser_sort_args, false, },
 { CL_PARSER_INTERNFN_SQR        , "sqr"     , 1,     &parser_ir__arg, false, },
 { CL_PARSER_INTERNFN_SQRT       , "sqrt"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_STRING_CONV, "string"  , 1,     &parser_irs_arg, false, },
 { CL_PARSER_INTERNFN_SUBARR     , "subarr"  , 3,  parser_subarr_args, true , },
 { CL_PARSER_INTERNFN_SUM        , "sum"     , 1,     &parser_arr_arg, true , },
 { CL_PARSER_INTERNFN_TAN        , "tan"     , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_TANH       , "tanh"    , 1,     &parser__r__arg, false, },
 { CL_PARSER_INTERNFN_TOLOWER    , "tolower" , 1,     &parser___s_arg, false, },
 { CL_PARSER_INTERNFN_TOUPPER    , "toupper" , 1,     &parser___s_arg, false, },
 { CL_PARSER_INTERNFN_TYPARR     , "typarr"  , 2,  parser_typarr_args, false, },
 { CL_PARSER_INTERNFN_VALTYP     , "valtyp"  , 1,     &parser_any_arg, false, },
 { CL_PARSER_INTERNFN_WHERE      , "where"   , 2,   parser_where_args, false, },
};
