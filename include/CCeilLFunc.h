#ifndef CL_L_FUNC_H
#define CL_L_FUNC_H

class ClLanguageFuncArg {
 private:
  std::string       name_;
  ClParserValueType type_;

 public:
  ClLanguageFuncArg(const std::string &name, ClParserValueType type) :
   name_(name), type_(type) {
  }

  const std::string &getName() const { return name_; }

  ClParserValueType getType() const { return type_; }

  bool isOutput() const { return (type_ & CL_PARSER_VALUE_TYPE_OUTPUT); }
};

class ClLanguageFunc;

#define ClLanguageFuncMgrInst ClLanguageFuncMgr::getInstance()

class ClLanguageFuncMgr {
 private:
  typedef std::map<std::string, ClLanguageFunc *> FuncMap;
  typedef std::vector<ClLanguageFuncArg *>        FuncArgList;

  FuncMap function_map_;

 public:
  static ClLanguageFuncMgr *getInstance() {
    static ClLanguageFuncMgr *instance;

    if (! instance)
      instance = new ClLanguageFuncMgr;

    return instance;
  }

  ClLanguageFunc *createFunc(const std::string &name, const FuncArgList &args,
                             const ClLanguageCommandList &command_list);

  void deleteFunc(const std::string &name);

  ClLanguageFunc *lookupFunc(const std::string &name) const;

  void printFuncs() const;

  void deleteFuncs();
};

class ClLanguageFunc {
 public:
  typedef std::vector<ClLanguageFuncArg *> ArgList;

 private:
  std::string           name_;
  ArgList               args_;
  ClLanguageCommandList command_list_;

 private:
  friend class ClLanguageFuncMgr;

  ClLanguageFunc(const std::string &name, const ArgList &args,
                 const ClLanguageCommandList &command_list);
 ~ClLanguageFunc();

 public:
  const std::string &getName() const { return name_; }

  uint getNumArgs() const { return uint(args_.size()); }

  const ClLanguageFuncArg &getArg(uint i) const { return *args_[i]; }

  const ClLanguageCommandList &getCommandList() const { return command_list_; }

  void print() const;
};

#endif
