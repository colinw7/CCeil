#ifndef CCEILP_FUNC_H
#define CCEILP_FUNC_H

#include <CCeilPTypes.h>

/**** Private Structures ****/

class ClParserFuncMgr {
 private:
  typedef std::map<std::string, ClParserFuncPtr> FuncMap;
  typedef StringVectorT                          FuncArgList;

 public:
  ClParserFuncMgr();
 ~ClParserFuncMgr();

  bool createFunc(const std::string &name, int *error_code);
  bool createFunc(const std::string &function_string,
                  const std::string &expression_string, int *error_code);

  bool isFunction(const std::string &name) const;

  ClParserFuncPtr getFunction(const std::string &name) const;

  void removeFunction(const std::string &name);

  void removeAllFunctions();

  void printAllFunctions() const;

 private:
  void addFunction(ClParserFunc *func);

  bool parseFunc(const std::string &function_string, const std::string &expression_string);

  static std::string readFunctionName(const std::string &str, uint *i);

  static FuncArgList readFunctionArgList(const std::string &str, uint *i, int *error_code);

  static std::string readFunctionVariableName(const std::string &str, uint *i);

  void markFunctionArgs(ClParserFunc *func, ClParserStackNode *stack_node);

 private:
  FuncMap function_list_;
};

//---

class ClParserFunc {
 protected:
  friend class ClParserFuncMgr;

  ClParserFunc(const std::string &name);
  ClParserFunc(const std::string &function_string, const std::string &expression_string);

  ClParserFunc *dup() const;

 public:
 ~ClParserFunc();

  const std::string &getName() const { return name_; }

  uint getNumArgs() const { return uint(args_.size()); }

  const std::string &getArg(uint i) const { return args_[i]; }

  void addArg(const std::string &arg) {
    args_.push_back(arg);
  }

  ClParserStackPtr getStack() const { return stack_; }

 public:
  void print() const;
  void print(std::ostream &os) const;

  void debugPrint() const;

  static bool isValidName(const std::string &name);

 private:
  std::string      name_;
  StringVectorT    args_;
  ClParserStackPtr stack_;
};

#endif
