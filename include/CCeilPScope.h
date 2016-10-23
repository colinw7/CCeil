#ifndef CCEILP_SCOPE_H
#define CCEILP_SCOPE_H

class ClParserScope;

class ClParserScopeMgr {
 private:
  typedef std::map<std::string,ClParserScopePtr> ScopeMap;

  ClParserScope *parent_;
  ScopeMap       scope_map_;

 public:
  ClParserScopeMgr(ClParserScope *parent=NULL) :
   parent_(parent) {
  }

 ~ClParserScopeMgr() { }

  ClParserScopePtr getScope(const std::string &name);

  ClParserScopePtr getScope(const StringVectorT &names);

 private:
  ClParserScopePtr createScope(ClParserScope *parent, const std::string &name);
};

class ClParserScope {
 private:
  ClParserScope       *parent_;
  std::string          name_;
  ClParserVarMgr       varMgr_;
  ClParserFuncMgr      funcMgr_;
  ClParserInternFnMgr  internFnMgr_;
  ClParserUserFnMgr    userFnMgr_;
  ClParserScopeMgr     scopeMgr_;

 protected:
  friend class ClParserScopeMgr;
  friend class CRefPtr<ClParserScope>;

  ClParserScope(ClParserScope *parent, const std::string &name) :
   parent_(parent), name_(name) {
  }

  ClParserScope(const ClParserScope &scope) :
   parent_(scope.parent_), name_(scope.name_), varMgr_(scope.varMgr_), funcMgr_(scope.funcMgr_),
   internFnMgr_(scope.internFnMgr_), userFnMgr_(scope.userFnMgr_), scopeMgr_(scope.scopeMgr_) {
  }

  const ClParserScope &operator=(const ClParserScope &scope) {
    parent_      = scope.parent_;
    name_        = scope.name_;
    varMgr_      = scope.varMgr_;
    funcMgr_     = scope.funcMgr_;
    internFnMgr_ = scope.internFnMgr_;
    userFnMgr_   = scope.userFnMgr_;
    scopeMgr_    = scope.scopeMgr_;

    return *this;
  }

  ClParserScope *dup() const {
    return new ClParserScope(*this);
  }

 public:
  const ClParserScope *getParent() const { return parent_; }

  const std::string &getName() const { return name_; }

  std::string getFullName() const {
    if (parent_ != NULL)
      return parent_->getFullName() + "::" + getName();
    else
      return getName();
  }

  //-----

  ClParserScopePtr getScope(const std::string &name) {
    return scopeMgr_.getScope(name);
  }

  ClParserScopePtr getScope(const StringVectorT &names) {
    return scopeMgr_.getScope(names);
  }

  //-----

  ClParserVarPtr getVariable(const std::string &name, bool create=false) {
    return varMgr_.getVariable(name, create);
  }

  //-----

  ClParserFuncPtr getFunction(const std::string &name) {
    return funcMgr_.getFunction(name);
  }

  //-----

  ClParserInternFnPtr getInternFn(const std::string &name) {
    return internFnMgr_.getInternFn(name);
  }

  //-----

  ClParserUserFnPtr createUserFn(const ClUserFnData &data) {
    return userFnMgr_.createUserFn(data);
  }

  ClParserUserFnPtr createUserFn(const std::string &name, uint type, int *arg_types,
                                 uint num_arg_types, ClParserUserFnProc proc, void *data) {
    return userFnMgr_.createUserFn(name, type, arg_types, num_arg_types, proc, data);
  }

  ClParserUserFnPtr createUserFn(const std::string &name, uint type, const IntVectorT &arg_types,
                                 ClParserUserFnProc proc, void *data) {
    return userFnMgr_.createUserFn(name, type, arg_types, proc, data);
  }

  ClParserUserFnPtr getUserFn(const std::string &name) {
    return userFnMgr_.getUserFn(name);
  }

  //-----

  void print() const;
  void print(std::ostream &os) const;

  void debugPrint() const;
};

#endif
