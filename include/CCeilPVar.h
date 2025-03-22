#ifndef CCEIL_P_VAR_H
#define CCEIL_P_VAR_H

#include <CCeilPValue.h>

/**** Private Defines ****/

#define CLP_VAR_FIXED_TYPE  (1<<0)
#define CLP_VAR_FIXED_VALUE (1<<1)
#define CLP_VAR_NO_DELETE   (1<<2)
#define CLP_VAR_GLOBAL      (1<<3)
#define CLP_VAR_HIDDEN      (1<<4)

/**** Private Structures ****/

class ClParser;

class ClParserVarMgr {
 public:
  typedef void (*VarProc)(ClParserVarPtr, void *);

 public:
  ClParserVarMgr(ClParser *parser);
 ~ClParserVarMgr();

  ClParserVarPtr createStdVar(const std::string &name, ClParserValuePtr value);

  ClParserVarPtr createVar(const std::string &name, ClParserValuePtr value);

  ClParserVarRefPtr createVarRef(ClParserVarPtr variable,
                                 const ClParserValueArray &subscripts = ClParserValueArray());

  ClParserStructVarRefPtr createStructVarRef(ClParserVarPtr variable, const std::string &name,
                                             const ClParserValueArray &subscripts =
                                              ClParserValueArray());

  void removeVariable(const std::string &name);

  void newVariableList();
  void oldVariableList();
  void deleteAllVariables();

  bool isVariable(const std::string &name) const;
  bool isLocalVariable(const std::string &name) const;

  ClParserVarPtr getVariable(const std::string &name, bool create=false) const;
  ClParserValuePtr getVariableValue(const std::string &name) const;
  void setVariableValue(const std::string &name, ClParserValuePtr value);

  void printAllVariables() const;

  void addVariableProc(const std::string &name, VarProc proc, void *data);
  void deleteVariableProc(const std::string &name, VarProc proc, void *data);
  void callVariableProc(ClParserVarPtr variable);

 private:
  ClParserVarPtr addVariable(ClParserVar *var);

  bool getVariableI(const std::string &name, ClParserVarPtr &var, bool parentScope=true) const;

  ClParserVarPtr createVarI(const std::string &name, ClParserValuePtr value);

  void addStandardVariables();

 private:
  struct VarProcData {
    std::string  name;
    VarProc      proc { nullptr };
    void        *data { nullptr };

    VarProcData(const std::string &name1, VarProc proc1, void *data1) :
     name(name1), proc(proc1), data(data1) {
    }
  };

  typedef std::map<std::string,ClParserVarPtr> VarMap;
  typedef std::vector<VarMap *>                VarMapStack;
  typedef std::list<VarProcData *>             VarProcDataList;

  ClParser*       parser_         { nullptr };
  VarMap*         varMap_         { nullptr };
  VarMapStack     varMapStack_;
  VarProcDataList varProcDataList_;
};

//------

class ClParserVar {
 public:
  static bool isValidName(const std::string &name);

 protected:
  friend class ClParserVarMgr;

  explicit ClParserVar(const std::string &name);
  ClParserVar(const std::string &name, ClParserValuePtr value);

  ClParserVar(const ClParserVar &var);

  ClParserVar *dup() const;

 public:
 ~ClParserVar();

  const std::string &getName() const {
    return name_;
  }

  ClParserValuePtr getValue() const;

  void setValue(ClParserValuePtr value);

  bool isFixedType() const {
    return (flags_ & CLP_VAR_FIXED_TYPE);
  }

  void setFixedType() {
    flags_ |= CLP_VAR_FIXED_TYPE;
  }

  bool isFixedValue() const {
    return (flags_ & CLP_VAR_FIXED_VALUE);
  }

  void setFixedValue() {
    flags_ |= CLP_VAR_FIXED_TYPE;
    flags_ |= CLP_VAR_FIXED_VALUE;
  }

  bool isNoDelete() const {
    return (flags_ & CLP_VAR_NO_DELETE);
  }

  void setNoDelete() {
    flags_ |= CLP_VAR_NO_DELETE;
  }

  bool isGlobal() const {
    return (flags_ & CLP_VAR_GLOBAL);
  }

  void setGlobal() {
    flags_ |= CLP_VAR_GLOBAL;
  }

  bool isHidden() const {
    return (flags_ & CLP_VAR_HIDDEN);
  }

  void setHidden() {
    flags_ |= CLP_VAR_HIDDEN;
  }

  void print() const;
  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const ClParserVar &th) {
    th.print(os);

    return os;
  }

 private:
  void init();
  void term();

 private:
  std::string      name_;
  ClParserValuePtr value_;
  uint             flags_;
};

//------

// Variable Ref
//   variable_   : associated variable
//   value_      : current value
//   subscripts_ : list of subscript values (each one is an array of
//               : one or two values e.g. a[1,2][3,4])

class ClParserVarRef {
 protected:
  friend class ClParserVarMgr;

  ClParserVarRef(ClParserVarPtr variable,
                 const ClParserValueArray &subscripts = ClParserValueArray());

  ClParserVarRef *dup() const;

 public:
  virtual ~ClParserVarRef() { }

  virtual bool setValue(ClParserValuePtr value);

  virtual bool getValue(ClParserValuePtr &value) const;

  bool isFixedType() const {
    return variable_->isFixedType();
  }

  void callVariableProc();

  void addSubscript(ClParserValuePtr subscript_value);

  virtual void print() const;
  virtual void print(std::ostream &) const;

  virtual void debugPrint() const;

  friend std::ostream &operator<<(std::ostream &os, const ClParserVarRef &th) {
    th.print(os);

    return os;
  }

 protected:
  typedef ClParserValueArray ValueArray;

  ClParserVarPtr variable_;
  ValueArray     subscripts_;
};

//------

// Struct Variable Ref
//   name_ : structure part name

class ClParserStructVarRef : public ClParserVarRef {
 protected:
  friend class ClParserVarMgr;

  ClParserStructVarRef(ClParserVarPtr variable, const std::string &name,
                       const ClParserValueArray &subscripts = ClParserValueArray());

  ClParserStructVarRef *dup() const;

 public:
 ~ClParserStructVarRef() { }

  bool setValue(ClParserValuePtr value) override;

  bool getValue(ClParserValuePtr &value) const override;

  void addName(const std::string &name);

  void print() const override;
  void print(std::ostream &) const override;

  void debugPrint() const override;

  friend std::ostream &operator<<(std::ostream &os, const ClParserStructVarRef &th) {
    th.print(os);

    return os;
  }

 private:
  typedef StringVectorT NameList;

  NameList names_;
};

#endif
