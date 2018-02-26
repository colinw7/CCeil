#ifndef CCEILP_EVAL_H
#define CCEILP_EVAL_H

#include <CCeilPValue.h>

class ClParserExpr {
 public:
  explicit ClParserExpr(const std::string &expression);

 ~ClParserExpr();

  bool compile();

  void getExpressionVariables(char ***variables, uint *num_variables);

  void freeExpressionVariables(char **variables, uint num_variables);

  bool exec(ClParserValuePtr &value);

 private:
  std::string      expression_;
  ClParserStackPtr stack_;
  bool             compiled_ { false };
};

//---

class ClParserArgValue {
 public:
  ClParserArgValue();
  ClParserArgValue(ClParserVarRefPtr var_ref, ClParserValuePtr value);
  ClParserArgValue(const ClParserArgValue &arg_value);
 ~ClParserArgValue();

  ClParserArgValue &operator=(const ClParserArgValue &arg_value);

  ClParserVarRefPtr getVarRef() const { return var_ref_; }
  ClParserValuePtr  getValue () const { return value_; }

  void setVarRef(ClParserVarRefPtr var_ref) {
    var_ref_ = var_ref;
  }

  void setValue(ClParserValuePtr value) {
    value_ = value;
  }

 private:
  ClParserVarRefPtr var_ref_;
  ClParserValuePtr  value_;
};

#endif
