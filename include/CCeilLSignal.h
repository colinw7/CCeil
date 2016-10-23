#ifndef CL_SIGNAL_H
#define CL_SIGNAL_H

#define SIGSYNTAX 1000
#define SIGEXPR   1001

typedef void (*ClSignalHandler)(int type);

struct ClSignalCommand {
  int             type;
  std::string     command;
  ClSignalHandler proc;
  ClSignalHandler old_proc;
};

#define ClSignalMgrInst ClSignalMgr::getInstance()

class ClSignalMgr {
 private:
  typedef std::map<int,ClSignalCommand*> SignalCommandMap;

  SignalCommandMap signal_command_map_;

 public:
  static ClSignalMgr *getInstance() {
    static ClSignalMgr *instance;

    if (! instance) {
      instance = new ClSignalMgr;

      instance->init();
    }

    return instance;
  }

  void initSignalProcs();
  void termSignalProcs();

  bool setSignalCommand(const std::string &name, const std::string &command);

  bool raiseSignal(const std::string &name, const std::string &data);

  bool setSignalProc(const std::string &name, ClSignalHandler proc);

  bool checkSignalCommand(int type);

  bool executeSignalCommand(int type);

  void handleSignal(int sig);

  ClSignalCommand *getSignalCommand(int type) const;

  int         signalNameToType(const std::string &name) const;
  std::string signalTypeToName(int type) const;

  bool isUnixSignal(int type) const;

 private:
  void init();
};

#endif
