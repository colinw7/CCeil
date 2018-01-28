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

//------

class ClParserScope {
 protected:
  friend class ClParserScopeMgr;
  friend class CRefPtr<ClParserScope>;

  ClParserScope(ClParser *parser, const std::string &name);

  ClParserScope(ClParserScope *parent, const std::string &name);

  ClParserScope(const ClParserScope &scope);

 ~ClParserScope();

  const ClParserScope &operator=(const ClParserScope &scope);

  ClParserScope *dup() const;

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

  ClParserScopePtr getScope(const std::string &name);
  ClParserScopePtr getScope(const StringVectorT &names);

  //-----

  ClParserVarPtr getVariable(const std::string &name, bool create=false);

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

 private:
  ClParser*           parser_      { nullptr };
  ClParserScope*      parent_      { nullptr };
  std::string         name_;
  ClParserVarMgr*     varMgr_      { nullptr };
  ClParserFuncMgr     funcMgr_;
  ClParserInternFnMgr internFnMgr_;
  ClParserUserFnMgr   userFnMgr_;
  ClParserScopeMgr    scopeMgr_;
};

#endif
