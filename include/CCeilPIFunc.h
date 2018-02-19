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
  IS_NAN,
  IS_CTYPE,
  LEN,
  LOG,
  LOG10,
  MAX,
  MIN,
  NDIM,
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

  InternFnMap internfn_map_;

 public:
  ClParserInternFnMgr();
  ~ClParserInternFnMgr();

  bool isInternFn(const std::string &name) const;

  ClParserInternFnPtr getInternFn(const std::string &name) const;

 private:
  void addInternFns();
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

  ClParserInternFnData(CLParserInternFnType type, const std::string &name,
                       uint numArgs, ClParserArg *args, bool composite=false) :
   type(type), name(name), numArgs(numArgs), args(args), composite(composite) {
  }
};

//---

class ClParserInternFn {
 protected:
  friend class ClParserInternFnMgr;
  friend class CRefPtr<ClParserInternFn>;

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
