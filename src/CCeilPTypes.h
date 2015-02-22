#ifndef CCEIL_P_TYPES_H
#define CCEIL_P_TYPES_H

#include <vector>
#include <list>
#include <CRefPtr.h>
#include <CAutoPtr.h>

typedef unsigned char uchar;

typedef std::vector<std::string>  StringVectorT;
typedef std::vector<int>          IntVectorT;
typedef std::vector<long>         LongVectorT;
typedef std::vector<unsigned int> UIntVectorT;

enum ClParserSortDirection {
  CL_SORT_ASCENDING  = 0,
  CL_SORT_DESCENDING = 1
};

enum ClParserValueType {
  CL_PARSER_VALUE_TYPE_NONE       =     0,
  CL_PARSER_VALUE_TYPE_INTEGER    = (1<<0),
  CL_PARSER_VALUE_TYPE_REAL       = (1<<1),
  CL_PARSER_VALUE_TYPE_STRING     = (1<<2),
  CL_PARSER_VALUE_TYPE_ARRAY      = (1<<3),
  CL_PARSER_VALUE_TYPE_LIST       = (1<<4),
  CL_PARSER_VALUE_TYPE_DICTIONARY = (1<<5),
  CL_PARSER_VALUE_TYPE_STRUCTURE  = (1<<6),
  CL_PARSER_VALUE_TYPE_VALUE      = (1<<7),
  CL_PARSER_VALUE_TYPE_GENERIC    = (1<<8),

  CL_PARSER_VALUE_TYPE_INPUT      = (1<<14),
  CL_PARSER_VALUE_TYPE_OUTPUT     = (1<<15)
};

#define CL_PARSER_OP_INLINE      (1<<12)
#define CL_PARSER_OP_PUNCTUATION (1<<11)
#define CL_PARSER_OP_UNARY       (1<<10)
#define CL_PARSER_OP_BINARY      (1<< 9)
#define CL_PARSER_OP_ASSIGNMENT  (1<< 8)

enum ClParserOperatorType {
  CL_PARSER_OP_NONE              = 0,

  CL_PARSER_OP_INCREMENT         = CL_PARSER_OP_INLINE + 1,
  CL_PARSER_OP_DECREMENT         = CL_PARSER_OP_INLINE + 2,

  CL_PARSER_OP_OPEN_R_BRACKET    = CL_PARSER_OP_PUNCTUATION + 1,
  CL_PARSER_OP_CLOSE_R_BRACKET   = CL_PARSER_OP_PUNCTUATION + 2,
  CL_PARSER_OP_OPEN_S_BRACKET    = CL_PARSER_OP_PUNCTUATION + 3,
  CL_PARSER_OP_CLOSE_S_BRACKET   = CL_PARSER_OP_PUNCTUATION + 4,
  CL_PARSER_OP_OPEN_BRACE        = CL_PARSER_OP_PUNCTUATION + 5,
  CL_PARSER_OP_CLOSE_BRACE       = CL_PARSER_OP_PUNCTUATION + 6,
  CL_PARSER_OP_OPEN_DICT         = CL_PARSER_OP_PUNCTUATION + 7,
  CL_PARSER_OP_CLOSE_DICT        = CL_PARSER_OP_PUNCTUATION + 8,
  CL_PARSER_OP_COMMA             = CL_PARSER_OP_PUNCTUATION + 9,
  CL_PARSER_OP_QUESTION_MARK     = CL_PARSER_OP_PUNCTUATION + 10,
  CL_PARSER_OP_COLON             = CL_PARSER_OP_PUNCTUATION + 11,
  CL_PARSER_OP_DOT               = CL_PARSER_OP_PUNCTUATION + 12,

  CL_PARSER_OP_UNARY_PLUS        = CL_PARSER_OP_UNARY + 1,
  CL_PARSER_OP_UNARY_MINUS       = CL_PARSER_OP_UNARY + 2,
  CL_PARSER_OP_LOGICAL_NOT       = CL_PARSER_OP_UNARY + 3,
  CL_PARSER_OP_BIT_NOT           = CL_PARSER_OP_UNARY + 4,

/* N.B. PARSER_LESS -> PARSER_NOT_EQUAL must be sequential */

  CL_PARSER_OP_PLUS              = CL_PARSER_OP_BINARY + 1,
  CL_PARSER_OP_MINUS             = CL_PARSER_OP_BINARY + 2,
  CL_PARSER_OP_TIMES             = CL_PARSER_OP_BINARY + 3,
  CL_PARSER_OP_DIVIDE            = CL_PARSER_OP_BINARY + 4,
  CL_PARSER_OP_MODULUS           = CL_PARSER_OP_BINARY + 5,
  CL_PARSER_OP_POWER             = CL_PARSER_OP_BINARY + 6,
  CL_PARSER_OP_LESS              = CL_PARSER_OP_BINARY + 7,
  CL_PARSER_OP_LESS_OR_EQUAL     = CL_PARSER_OP_BINARY + 8,
  CL_PARSER_OP_GREATER           = CL_PARSER_OP_BINARY + 9,
  CL_PARSER_OP_GREATER_OR_EQUAL  = CL_PARSER_OP_BINARY + 10,
  CL_PARSER_OP_EQUAL             = CL_PARSER_OP_BINARY + 11,
  CL_PARSER_OP_NOT_EQUAL         = CL_PARSER_OP_BINARY + 12,
  CL_PARSER_OP_APPROX_EQUAL      = CL_PARSER_OP_BINARY + 13,
  CL_PARSER_OP_LOGICAL_AND       = CL_PARSER_OP_BINARY + 14,
  CL_PARSER_OP_LOGICAL_OR        = CL_PARSER_OP_BINARY + 15,
  CL_PARSER_OP_BIT_AND           = CL_PARSER_OP_BINARY + 16,
  CL_PARSER_OP_BIT_OR            = CL_PARSER_OP_BINARY + 17,
  CL_PARSER_OP_BIT_XOR           = CL_PARSER_OP_BINARY + 18,
  CL_PARSER_OP_BIT_LSHIFT        = CL_PARSER_OP_BINARY + 19,
  CL_PARSER_OP_BIT_RSHIFT        = CL_PARSER_OP_BINARY + 20,
  CL_PARSER_OP_ARRAY_TIMES       = CL_PARSER_OP_BINARY + 21,

  CL_PARSER_OP_EQUALS            = CL_PARSER_OP_ASSIGNMENT + 1,
  CL_PARSER_OP_PLUS_EQUALS       = CL_PARSER_OP_ASSIGNMENT + 2,
  CL_PARSER_OP_MINUS_EQUALS      = CL_PARSER_OP_ASSIGNMENT + 3,
  CL_PARSER_OP_TIMES_EQUALS      = CL_PARSER_OP_ASSIGNMENT + 4,
  CL_PARSER_OP_DIVIDE_EQUALS     = CL_PARSER_OP_ASSIGNMENT + 5,
  CL_PARSER_OP_MODULUS_EQUALS    = CL_PARSER_OP_ASSIGNMENT + 6,
  CL_PARSER_OP_BIT_AND_EQUALS    = CL_PARSER_OP_ASSIGNMENT + 7,
  CL_PARSER_OP_BIT_OR_EQUALS     = CL_PARSER_OP_ASSIGNMENT + 8,
  CL_PARSER_OP_BIT_XOR_EQUALS    = CL_PARSER_OP_ASSIGNMENT + 9,
  CL_PARSER_OP_BIT_LSHIFT_EQUALS = CL_PARSER_OP_ASSIGNMENT + 10,
  CL_PARSER_OP_BIT_RSHIFT_EQUALS = CL_PARSER_OP_ASSIGNMENT + 11
};

enum CLArgType {
  CL_ARG_TYPE_NONE     = -1,

  CL_ARG_TYPE_SKIP     = 0,
  CL_ARG_TYPE_REAL     = 1,
  CL_ARG_TYPE_INTEGER  = 2,
  CL_ARG_TYPE_STRING   = 3,
  CL_ARG_TYPE_TEXT     = 4,
  CL_ARG_TYPE_VALUE    = 5,
  CL_ARG_TYPE_WORD     = 6,

  CL_ARG_TYPE_REALARR  = 11,
  CL_ARG_TYPE_INTARR   = 12,
  CL_ARG_TYPE_STRARR   = 13,
  CL_ARG_TYPE_EXPR     = 14,
  CL_ARG_TYPE_WORDARR  = 15,
  CL_ARG_TYPE_FLTARR   = 16,

  CL_ARG_TYPE_SKIP_N   = 21,
  CL_ARG_TYPE_TYPED    = 22,

  CL_ARG_TYPE_REALS    = 31,
  CL_ARG_TYPE_FLOATS   = 32,
  CL_ARG_TYPE_INTEGERS = 33,
  CL_ARG_TYPE_WORDS    = 34,
  CL_ARG_TYPE_STRINGS  = 35,
  CL_ARG_TYPE_CHARS    = 36,

  CL_ARG_TYPE_STRMAT   = 41,
  CL_ARG_TYPE_CHRMAT   = 42
};

//------

class  ClParserObj;
class  ClParserReal;
class  ClParserInteger;
class  ClParserString;
class  ClParserList;
class  ClParserDict;
class  ClParserStruct;
class  ClParserType;
class  ClParserFunc;
class  ClParserUserFn;
class  ClParserValue;

class  ClParserArray;
class  ClParserIdentifier;
class  ClParserInternFn;
class  ClParserOperator;
class  ClParserStack;
class  ClParserVar;
class  ClParserVarRef;
class  ClParserStructVarRef;

class  ClParserScope;

struct ClParserArg;
class  ClParserArgValue;
class  ClParserKey;
struct ClParserKeyValue;
class  ClParserStackNode;
class  ClParserSubType;

union  ClParserArrayData;
union  ClParserStackNodeData;
union  ClParserValueData;

typedef CRefPtr<ClParserObj>          ClParserObjPtr;
typedef CRefPtr<ClParserReal>         ClParserRealPtr;
typedef CRefPtr<ClParserInteger>      ClParserIntegerPtr;
typedef CRefPtr<ClParserString>       ClParserStringPtr;
typedef CRefPtr<ClParserList>         ClParserListPtr;
typedef CRefPtr<ClParserDict>         ClParserDictPtr;
typedef CRefPtr<ClParserStruct>       ClParserStructPtr;
typedef CRefPtr<ClParserType>         ClParserTypePtr;
typedef CRefPtr<ClParserFunc>         ClParserFuncPtr;
typedef CRefPtr<ClParserUserFn>       ClParserUserFnPtr;
typedef CRefPtr<ClParserValue>        ClParserValuePtr;
typedef CRefPtr<ClParserArray>        ClParserArrayPtr;
typedef CRefPtr<ClParserIdentifier>   ClParserIdentifierPtr;
typedef CRefPtr<ClParserInternFn>     ClParserInternFnPtr;
typedef CRefPtr<ClParserOperator>     ClParserOperatorPtr;
typedef CRefPtr<ClParserStack>        ClParserStackPtr;
typedef CRefPtr<ClParserVar>          ClParserVarPtr;
typedef CRefPtr<ClParserVarRef>       ClParserVarRefPtr;
typedef CRefPtr<ClParserStructVarRef> ClParserStructVarRefPtr;
typedef CRefPtr<ClParserScope>        ClParserScopePtr;

typedef std::vector<ClParserObj *>    ClParserObjArray;
typedef std::vector<ClParserValuePtr> ClParserValueArray;
typedef std::list<ClParserValuePtr>   ClParserValueList;

typedef std::vector<ClParserArgValue>  ClParserArgValueArray;
typedef std::vector<ClParserSubType *> ClParserSubTypeArray;

#endif
