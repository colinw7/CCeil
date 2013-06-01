#ifndef CL_L_PROC_H
#define CL_L_PROC_H

class ClLanguageProcArg {
 private:
  std::string name_;
  bool        returned_;

 public:
  ClLanguageProcArg(const std::string &name, bool returned) :
   name_(name), returned_(returned) {
  }

  const std::string &getName() const { return name_; }

  bool isReturned() const { return returned_; }
};

class ClLanguageProc;

#define ClLanguageProcMgrInst ClLanguageProcMgr::getInstance()

class ClLanguageProcMgr {
 private:
  typedef std::map<std::string, ClLanguageProc *> ProcMap;
  typedef std::vector<ClLanguageProcArg *>        ProcArgList;

  ProcMap proc_map_;

 public:
  static ClLanguageProcMgr *getInstance() {
    static ClLanguageProcMgr *instance;

    if (! instance)
      instance = new ClLanguageProcMgr;

    return instance;
  }

  ClLanguageProc *createProc(const std::string &name, const ProcArgList &args,
                             const ClLanguageCommandList &command_list);

  void deleteProc(const std::string &name);

  ClLanguageProc *lookupProc(const std::string &name) const;

  void printProcs() const;

  void deleteProcs();
};

class ClLanguageProc {
 public:
  typedef std::vector<ClLanguageProcArg *> ArgList;

 private:
  std::string           name_;
  ArgList               args_;
  ClLanguageCommandList command_list_;
  int                   ref_count_;

 private:
  friend class ClLanguageProcMgr;

  ClLanguageProc(const std::string &name, const ArgList &args,
                 const ClLanguageCommandList &command_list);

 ~ClLanguageProc();

 public:
  const std::string &getName() const { return name_; }

  uint getNumArgs() const { return args_.size(); }

  const ClLanguageProcArg &getArg(uint i) const { return *args_[i]; }

  const ClLanguageCommandList &getCommandList() const { return command_list_; }

  void print() const;

  bool canDelete() const { return (ref_count_ == 0); }

  void incRef() { ++ref_count_; }
  void defRef() { --ref_count_; }
};

#endif
