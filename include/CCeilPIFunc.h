#ifndef CCEILP_IFUNC_H
#define CCEILP_IFUNC_H

#include <CCeilPTypes.h>

/**** Private Defines ****/

enum CLParserInternFnType {
  CL_PARSER_INTERNFN_ABS         = 1,
  CL_PARSER_INTERNFN_ACOS        = 2,
  CL_PARSER_INTERNFN_ADDR        = 3,
  CL_PARSER_INTERNFN_ARRAY_I     = 4,
  CL_PARSER_INTERNFN_ARRAY_R     = 5,
  CL_PARSER_INTERNFN_ARRAY_S     = 6,
  CL_PARSER_INTERNFN_ARRCAT      = 7,
  CL_PARSER_INTERNFN_ASIN        = 8,
  CL_PARSER_INTERNFN_ATAN        = 9,
  CL_PARSER_INTERNFN_ATAN2       = 10,
  CL_PARSER_INTERNFN_CEIL        = 11,
  CL_PARSER_INTERNFN_CHAR_CONV   = 12,
  CL_PARSER_INTERNFN_COS         = 13,
  CL_PARSER_INTERNFN_COSH        = 14,
  CL_PARSER_INTERNFN_DIM         = 15,
  CL_PARSER_INTERNFN_EXP         = 16,
  CL_PARSER_INTERNFN_FLOOR       = 17,
  CL_PARSER_INTERNFN_INDARR_I    = 18,
  CL_PARSER_INTERNFN_INDARR_R    = 19,
  CL_PARSER_INTERNFN_INDARR_S    = 20,
  CL_PARSER_INTERNFN_INDEX       = 21,
  CL_PARSER_INTERNFN_INT_CONV    = 22,
  CL_PARSER_INTERNFN_IS_NAN      = 23,
  CL_PARSER_INTERNFN_IS_CTYPE    = 24,
  CL_PARSER_INTERNFN_LEN         = 25,
  CL_PARSER_INTERNFN_LOG         = 26,
  CL_PARSER_INTERNFN_LOG10       = 27,
  CL_PARSER_INTERNFN_MAX         = 28,
  CL_PARSER_INTERNFN_MIN         = 29,
  CL_PARSER_INTERNFN_NDIM        = 30,
  CL_PARSER_INTERNFN_REAL_CONV   = 31,
  CL_PARSER_INTERNFN_RINDEX      = 32,
  CL_PARSER_INTERNFN_SIGN        = 33,
  CL_PARSER_INTERNFN_SIN         = 34,
  CL_PARSER_INTERNFN_SINH        = 35,
  CL_PARSER_INTERNFN_SORT        = 36,
  CL_PARSER_INTERNFN_SQR         = 37,
  CL_PARSER_INTERNFN_SQRT        = 38,
  CL_PARSER_INTERNFN_STRING_CONV = 39,
  CL_PARSER_INTERNFN_SUBARR      = 40,
  CL_PARSER_INTERNFN_SUM         = 41,
  CL_PARSER_INTERNFN_TAN         = 42,
  CL_PARSER_INTERNFN_TANH        = 43,
  CL_PARSER_INTERNFN_TOLOWER     = 44,
  CL_PARSER_INTERNFN_TOUPPER     = 45,
  CL_PARSER_INTERNFN_TYPARR      = 46,
  CL_PARSER_INTERNFN_VALTYP      = 47,
  CL_PARSER_INTERNFN_WHERE       = 48
};

/**** Private Structures ****/

#define ClParserInternFnMgrInst ClParserInternFnMgr::getInstance()

class ClParserInternFnMgr {
 private:
  typedef std::map<std::string,ClParserInternFnPtr> InternFnMap;

  InternFnMap internfn_map_;

 public:
  ClParserInternFnMgr();
  ~ClParserInternFnMgr();

  bool isInternFn(const std::string &name) const;

  ClParserInternFnPtr getInternFn(const std::string &name) const;

 private:
  void addInternFns();
};

struct ClParserArg {
  uint types;
  uint variable;
};

struct ClParserInternFnData {
  CLParserInternFnType  type;
  std::string           name;
  uint                  num_args;
  ClParserArg          *args;
  bool                  composite;
};

class ClParserInternFn {
 private:
  CLParserInternFnType  type_;
  std::string           name_;
  uint                  num_args_;
  ClParserArg          *args_;
  bool                  composite_;

 protected:
  friend class ClParserInternFnMgr;
  friend class CRefPtr<ClParserInternFn>;

  ClParserInternFn(CLParserInternFnType type, const std::string &name,
                   uint num_args, ClParserArg *args, bool composite);

  ClParserInternFn(const ClParserInternFnData &data);

  ClParserInternFn *dup() const;

 public:
  CLParserInternFnType getType() const { return type_; }

  const std::string &getName() const { return name_; }

  uint getNumArgs() const { return num_args_; }

  const ClParserArg &getArg(uint i) const { return args_[i]; }

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;
};

#endif
