#ifndef CCEILP_IFUNC_H
#define CCEILP_IFUNC_H

#include <CCeilPTypes.h>

/**** Private Defines ****/

enum class CLParserInternFnType {
  NONE,
  ABS,
  ACOS,
  ADDR,
  ARRAY_I,
  ARRAY_R,
  ARRAY_S,
  ARRCAT,
  ASIN,
  ASSERT,
  ATAN,
  ATAN2,
  CEIL,
  CHAR_CONV,
  COS,
  COSH,
  DIM,
  EXP,
  FLOOR,
  INDARR_I,
  INDARR_R,
  INDARR_S,
  INDEX,
  INT_CONV,
  INVNORM,
  IS_NAN,
  IS_CTYPE,
  LEN,
  LOG,
  LOG10,
  MAX,
  MIN,
  NDIM,
  NORM,
  REAL_CONV,
  RINDEX,
  SIGN,
  SIN,
  SINH,
  SORT,
  SQR,
  SQRT,
  STRING_CONV,
  SUBARR,
  SUM,
  TAN,
  TANH,
  TOLOWER,
  TOUPPER,
  TYPARR,
  VALTYP,
  WHERE
};

/**** Private Structures ****/

#define ClParserInternFnMgrInst ClParserInternFnMgr::getInstance()

class ClParserInternFnMgr {
 private:
  typedef std::map<std::string,ClParserInternFnPtr> InternFnMap;

 public:
  ClParserInternFnMgr();
 ~ClParserInternFnMgr();

  bool isInternFn(const std::string &name) const;

  ClParserInternFnPtr getInternFn(const std::string &name) const;

 private:
  void addInternFns();

 private:
  InternFnMap internfn_map_;
};

//---

struct ClParserArg {
  uint types    { 0 };
  bool variable { false };
};

//---

struct ClParserInternFnData {
  CLParserInternFnType type      { CLParserInternFnType::NONE };
  std::string          name;
  uint                 numArgs   { 0 };
  ClParserArg*         args      { nullptr };
  bool                 composite { false };

  ClParserInternFnData() = default;

  ClParserInternFnData(CLParserInternFnType type1, const std::string &name1,
                       uint numArgs1, ClParserArg *args1, bool composite1=false) :
   type(type1), name(name1), numArgs(numArgs1), args(args1), composite(composite1) {
  }
};

//---

class ClParserInternFn {
 protected:
  friend class ClParserInternFnMgr;

  ClParserInternFn(CLParserInternFnType type, const std::string &name,
                   uint numArgs, ClParserArg *args, bool composite);

  ClParserInternFn(const ClParserInternFnData &data);

  ClParserInternFn *dup() const;

 public:
  CLParserInternFnType getType() const { return data_.type; }

  const std::string &getName() const { return data_.name; }

  uint getNumArgs() const { return data_.numArgs; }

  ClParserArg *getArgs() const { return data_.args; }

  const ClParserArg &getArg(uint i) const {
    assert(i < data_.numArgs);

    return data_.args[i];
  }

  bool isComposite() const { return data_.composite; }

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

 private:
  ClParserInternFnData data_;
};

#endif
