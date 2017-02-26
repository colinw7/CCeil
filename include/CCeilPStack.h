#ifndef CCEILP_STACK_H
#define CCEILP_STACK_H

#include <CCeilPTypes.h>
#include <CCeilPValue.h>
#include <CCeilPIdent.h>

/**** Private Defines ****/

enum ClParserStackNodeType {
  CL_PARSER_STACK_NODE_NONE           = 0,
  CL_PARSER_STACK_NODE_OPERATOR       = 1,
  CL_PARSER_STACK_NODE_INTERN_FN      = 2,
  CL_PARSER_STACK_NODE_VALUE          = 3,
  CL_PARSER_STACK_NODE_VAR_REF        = 4,
  CL_PARSER_STACK_NODE_STRUCT_VAR_REF = 5,
  CL_PARSER_STACK_NODE_IDENTIFIER     = 6,
  CL_PARSER_STACK_NODE_USER_FN        = 7,
  CL_PARSER_STACK_NODE_TYPE           = 8
};

/**** Private Structures ****/

struct ClParserFuncValue {
  ClParserFuncPtr    function;
  ClParserValueArray values;
};

#define ClParserStackMgrInst ClParserStackMgr::getInstance()

class ClParserStackMgr {
 private:
  typedef std::vector<ClParserStackPtr> StackStack;

  ClParserStackPtr current_stack_;
  StackStack       stack_stack_;

 public:
  static ClParserStackMgr *getInstance();

 private:
  ClParserStackMgr();

 public:
 ~ClParserStackMgr();

  ClParserStackPtr getCurrentStack() const { return current_stack_; }

  ClParserValuePtr evaluateStack();

  ClParserValuePtr  unstackExpressionValues(int *error_code);
  void              unstackExpression(int *error_code);
  ClParserValuePtr  unstackExpressionValue(int *error_code);
  void              subscriptStackedValue(ClParserValuePtr value, int *error_code);
  void              backUnstackExpression(int *error_code);
  void              unstackInlineOperator(int *error_code);
  void              unstackUnaryOperator(int *error_code);
  void              unstackBinaryOperator(int *error_code);
  void              unstackAssignmentOperator(int *error_code);
  void              unstackInternalFunction(int *error_code);
  void              unstackUserFunction(int *error_code);
  void              unstackTypeFunction(int *error_code);
  void              unstackBracketedExpression(int *error_code);
  void              unstackArrayValue(int *error_code);
  void              unstackListValue(int *error_code);
  void              unstackDictionaryValue(int *error_code);
  void              unstackSelectExpression(int *error_code);
  void              skipExpression(int *error_code);
  void              unstackStructure(int *error_code);
  void              unstackVariable(int *error_code);
  void              unstackFunction(int *error_code);
  ClParserArgValue *unstackArgValue(int *error_code);
  void              stackFunctionStackNode(ClParserStackNode *stack_node,
                                           ClParserFuncValue *func_value);

  bool checkUnstack(ClParserOperatorPtr op);

  void setCurrentStack(ClParserStackPtr stack);

  void push(ClParserIdentifierPtr identifier);
  void push(ClParserInternFnPtr internfn);
  void push(ClParserOperatorType type);
  void push(ClParserOperatorPtr op);
  void push(ClParserTypePtr type);
  void push(ClParserUserFnPtr userfn);
  void push(ClParserValuePtr value);
  void push(ClParserVarRefPtr var_ref);
  void push(ClParserStructVarRefPtr var_ref);

  bool pop(ClParserIdentifierPtr &identifier);
  bool pop(ClParserInternFnPtr &internfn);
  bool pop(ClParserOperatorPtr &op);
  bool pop(ClParserTypePtr &type);
  bool pop(ClParserUserFnPtr &userfn);
  bool pop(ClParserValuePtr &value);
  bool pop(ClParserVarRefPtr &var_ref);
  bool pop(ClParserStructVarRefPtr &var_ref);

  void pushStackNode(ClParserStackNode *stack_node);
  void popStackNode();

  void debugPrintCurrent();

  void restartAll();
  void restart();

  ClParserStackPtr startStack();
  ClParserStackPtr endStack();

  ClParserStackPtr startTempStack();
  ClParserStackPtr endTempStack();

  void toStart();
  void toEnd();

  void setInBracket(bool flag);
  bool getInBracket();

  void setRestart();
  bool getRestart();

  ClParserStackNode *getCurrentStackNode() const;

  void removeCurrentStackNode();

  bool toNext();
  bool toPrev();

  void setLastOperator();

  ClParserOperatorPtr getLastOperator();
};

class ClParserStack {
 public:
  typedef std::list<ClParserStackNode *> StackNodeList;

 private:
  bool                    restart_;
  bool                    in_bracket_;
  ClParserOperatorPtr     last_operator_;
  StackNodeList           stack_node_list_;
  StackNodeList::iterator current_stack_node_;

 protected:
  friend class ClParserStackMgr;
  friend class CRefPtr<ClParserStack>;

  ClParserStack();
  ClParserStack(const ClParserStack &stack);
 ~ClParserStack();

  ClParserStack *dup() const;

 public:
  bool getRestart() const {
    return restart_;
  }

  void setRestart(bool restart = true) {
    restart_ = restart;
  }

  bool getInBracket() const {
    return in_bracket_;
  }

  void setInBracket(bool in_bracket) {
    in_bracket_ = in_bracket;
  }

  ClParserOperatorPtr getLastOperator() const {
    return last_operator_;
  }

  StackNodeList::const_iterator stackNodeListBegin() const {
    return stack_node_list_.begin();
  }

  StackNodeList::const_iterator stackNodeListEnd() const {
    return stack_node_list_.end();
  }

  ClParserStackNode *getCurrentStackNodeP() const;

  StackNodeList::iterator getCurrentStackNode() const {
    return current_stack_node_;
  }

  void setLastOperator();

  void restart(bool restart);

  void pushStackNode(ClParserStackNode *stack_node);
  void popStackNode();

  void toStart();
  void toEnd();

  bool toNext();
  bool toPrev();

  void print() const;
  void print(std::ostream &os) const;
  void debugPrintCurrent() const;
  void debugPrint() const;

  void getVariables(char ***variables, uint *num_variables);

  static void freeVariables(char **variables, uint num_variables);
};

union ClParserStackNodeData {
  ClParserValuePtr        *value;
  ClParserOperatorPtr     *op;
  ClParserInternFnPtr     *internfn;
  ClParserVarRefPtr       *var_ref;
  ClParserStructVarRefPtr *svar_ref;
  ClParserIdentifierPtr   *identifier;
  ClParserUserFnPtr       *userfn;
  ClParserTypePtr         *type;
};

class ClParserStackNode {
 protected:
  ClParserStackNodeType type_;
  ClParserStackNodeData data_;

  friend class ClParserStack;

 public:
  ClParserStackNode();

  explicit ClParserStackNode(ClParserOperatorPtr op);
  explicit ClParserStackNode(ClParserInternFnPtr internfn);
  explicit ClParserStackNode(ClParserValuePtr value);
  explicit ClParserStackNode(ClParserVarRefPtr var_ref);
  explicit ClParserStackNode(ClParserStructVarRefPtr var_ref);
  explicit ClParserStackNode(ClParserIdentifierPtr identifier);
  explicit ClParserStackNode(ClParserUserFnPtr userfn);
  explicit ClParserStackNode(ClParserTypePtr type);

 ~ClParserStackNode();

  bool isOperator() const {
    return (type_ == CL_PARSER_STACK_NODE_OPERATOR);
  }

  bool isOperator(ClParserOperatorType type);

  ClParserOperatorPtr getOperator() const {
    assert(type_ == CL_PARSER_STACK_NODE_OPERATOR);

    return *data_.op;
  }

  bool isInternFn() const {
    return (type_ == CL_PARSER_STACK_NODE_INTERN_FN);
  }

  ClParserInternFnPtr getInternFn() const {
    assert(type_ == CL_PARSER_STACK_NODE_INTERN_FN);

    return *data_.internfn;
  }

  bool isValue() const {
    return (type_ == CL_PARSER_STACK_NODE_VALUE);
  }

  ClParserValuePtr getValue() const {
    assert(type_ == CL_PARSER_STACK_NODE_VALUE);

    return *data_.value;
  }

  bool isVarRef() const {
    return (type_ == CL_PARSER_STACK_NODE_VAR_REF);
  }

  ClParserVarRefPtr getVarRef() const {
    assert(type_ == CL_PARSER_STACK_NODE_VAR_REF);

    return *data_.var_ref;
  }

  bool isStructVarRef() const {
    return (type_ == CL_PARSER_STACK_NODE_STRUCT_VAR_REF);
  }

  ClParserStructVarRefPtr getStructVarRef() const {
    assert(type_ == CL_PARSER_STACK_NODE_STRUCT_VAR_REF);

    return *data_.svar_ref;
  }

  bool isIdentifier() const {
    return (type_ == CL_PARSER_STACK_NODE_IDENTIFIER);
  }

  ClParserIdentifierPtr getIdentifier() const {
    assert(type_ == CL_PARSER_STACK_NODE_IDENTIFIER);

    return *data_.identifier;
  }

  bool isUserFn() const {
    return (type_ == CL_PARSER_STACK_NODE_USER_FN);
  }

  ClParserUserFnPtr getUserFn() {
    assert(type_ == CL_PARSER_STACK_NODE_USER_FN);

    return *data_.userfn;
  }

  bool isType() const {
    return (type_ == CL_PARSER_STACK_NODE_TYPE);
  }

  ClParserTypePtr getType() {
    assert(type_ == CL_PARSER_STACK_NODE_TYPE);

    return *data_.type;
  }

  void stack();

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;
};

class ClParserStackAutoDebugPrint {
 public:
  ClParserStackAutoDebugPrint() { }

 ~ClParserStackAutoDebugPrint();
};

#endif
