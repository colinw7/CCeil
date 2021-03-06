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

  void setInitProc(ClModuleInitProc proc, void *data=nullptr) {
    init_proc_ = proc;
    init_data_ = data;
  }

  void setSetProc (ClModuleSetProc proc, void *data=nullptr) {
    set_proc_ = proc;
    set_data_ = data;
  }

  void setTermProc(ClModuleTermProc proc, void *data=nullptr) {
    term_proc_ = proc;
    term_data_ = data;
  }

  void setHelpProc(ClModuleHelpProc proc, void *data=nullptr) {
    help_proc_ = proc;
    help_data_ = data;
  }

  bool isLoaded() const { return (state_ == State::LOADED); }

  bool execInitProc();
  bool execReinitProc();
  bool execSetProc (const std::string &option, const std::string &args);
  bool execTermProc();
  bool execHelpProc(const std::string &args);

 private:
  enum class State {
    UNLOADED = 0,
    LOADED   = 1
  };

  std::string       name_;
  ClModuleInitProc  init_proc_ { nullptr };
  void             *init_data_ { nullptr };
  ClModuleSetProc   set_proc_  { nullptr };
  void             *set_data_  { nullptr };
  ClModuleTermProc  term_proc_ { nullptr };
  void             *term_data_ { nullptr };
  ClModuleHelpProc  help_proc_ { nullptr };
  void             *help_data_ { nullptr };
  State             state_     { State::UNLOADED };
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

  void setInitProc(const std::string &name, ClModuleInitProc proc, void *data=nullptr);
  void setSetProc (const std::string &name, ClModuleSetProc  proc, void *data=nullptr);
  void setTermProc(const std::string &name, ClModuleTermProc proc, void *data=nullptr);
  void setHelpProc(const std::string &name, ClModuleHelpProc proc, void *data=nullptr);

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
