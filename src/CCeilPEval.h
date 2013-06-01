#ifndef CCEILP_EVAL_H
#define CCEILP_EVAL_H

#include <CCeilPValue.h>

class ClParserExpr {
 private:
  std::string      expression_;
  ClParserStackPtr stack_;
  bool             compiled_;

 public:
  ClParserExpr(const std::string &expression);
 ~ClParserExpr();

  bool compile();

  void getExpressionVariables(char ***variables, uint *num_variables);

  void freeExpressionVariables(char **variables, uint num_variables);

  bool exec(ClParserValuePtr &value);
};

class ClParserArgValue {
 private:
  ClParserVarRefPtr var_ref_;
  ClParserValuePtr  value_;

 public:
  ClParserArgValue();
  ClParserArgValue(ClParserVarRefPtr var_ref, ClParserValuePtr value);
  ClParserArgValue(const ClParserArgValue &arg_value);
 ~ClParserArgValue();

  const ClParserArgValue &operator=(const ClParserArgValue &arg_value);

  ClParserVarRefPtr getVarRef() const { return var_ref_; }
  ClParserValuePtr  getValue () const { return value_; }

  void setVarRef(ClParserVarRefPtr var_ref) {
    var_ref_ = var_ref;
  }

  void setValue(ClParserValuePtr value) {
    value_ = value;
  }
};

#endif
