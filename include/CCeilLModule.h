#ifndef CL_L_MODULE_H
#define CL_L_MODULE_H

/*---- Defines ----*/

enum ClModuleInitType {
  CL_MODULE_INIT_TYPE_INIT   = 0,
  CL_MODULE_INIT_TYPE_REINIT = 1
};

/*---- Typedefs/Structures ----*/

typedef bool (*ClModuleProc)
                (...);
typedef bool (*ClModuleInitProc)
                (ClModuleInitType init_type, void *data);
typedef bool (*ClModuleSetProc)
                (const char *option, const char *args, void *data);
typedef bool (*ClModuleTermProc)
                (void *data);
typedef bool (*ClModuleHelpProc)
                (const char *args, void *data);

struct ClModuleDef {
  std::string  name;
  ClModuleProc proc;
};

/*---- Routines ----*/

class ClModule {
 public:
  explicit ClModule(const std::string &name);

  const std::string &getName() const { return name_; }

  void setInitProc(ClModuleInitProc proc, void *data=NULL) {
    init_proc_ = proc;
    init_data_ = data;
  }

  void setSetProc (ClModuleSetProc proc, void *data=NULL) {
    set_proc_ = proc;
    set_data_ = data;
  }

  void setTermProc(ClModuleTermProc proc, void *data=NULL) {
    term_proc_ = proc;
    term_data_ = data;
  }

  void setHelpProc(ClModuleHelpProc proc, void *data=NULL) {
    help_proc_ = proc;
    help_data_ = data;
  }

  bool isLoaded() const { return (state_ == MODULE_LOADED); }

  bool execInitProc();
  bool execReinitProc();
  bool execSetProc (const std::string &option, const std::string &args);
  bool execTermProc();
  bool execHelpProc(const std::string &args);

 private:
  enum State {
    MODULE_UNLOADED = 0,
    MODULE_LOADED   = 1
  };

  std::string       name_;
  ClModuleInitProc  init_proc_;
  void             *init_data_;
  ClModuleSetProc   set_proc_;
  void             *set_data_;
  ClModuleTermProc  term_proc_;
  void             *term_data_;
  ClModuleHelpProc  help_proc_;
  void             *help_data_;
  State             state_;
};

//------

#define ClModuleMgrInst ClModuleMgr::getInstance()

class ClModuleMgr {
 public:
  static ClModuleMgr *getInstance() {
    static ClModuleMgr *instance;

    if (! instance)
      instance = new ClModuleMgr;

    return instance;
  }

  void defineModule(const std::string &name, ClModuleDef *module_def);
  void defineModule(const std::string &name);

  void setInitProc(const std::string &name, ClModuleInitProc proc, void *data=NULL);
  void setSetProc (const std::string &name, ClModuleSetProc  proc, void *data=NULL);
  void setTermProc(const std::string &name, ClModuleTermProc proc, void *data=NULL);
  void setHelpProc(const std::string &name, ClModuleHelpProc proc, void *data=NULL);

  bool importModule(const std::string &name);

  void getModuleList(StringVectorT &modules) const;

  bool isModuleLoaded(const std::string &name) const;

  bool execModuleSet(const std::string &name, const std::string &option, const std::string &args);

  bool execModuleHelp(const std::string &name, const std::string &args);

  void reinitModules();
  bool reinitModule(const std::string &name);

  void termModules();
  bool termModule(const std::string &name);

  bool importExternalModule(const std::string &name);

  std::string moduleSearchLib(const std::string &envname, const std::string &name);

  ClModule *getModule(const std::string &name) const;

  void deleteModules();

 private:
  ClModuleMgr() { }
 ~ClModuleMgr() { }

  ClModule *createModule(const std::string &name);

 private:
  typedef std::map<std::string,ClModule *> ModuleMap;

  ModuleMap module_list_;
};

#endif
