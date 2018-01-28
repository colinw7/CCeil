#include <CCeilL.h>
#include <COSSignal.h>

static void
ClLanguageSignalHandler(int sig)
{
  ClSignalMgrInst->handleSignal(sig);
}

//------------------------

void
ClSignalMgr::
init()
{
  signal_command_map_[SIGSYNTAX] = nullptr;
  signal_command_map_[SIGEXPR  ] = nullptr;
  signal_command_map_[SIGINT   ] = nullptr;
#ifdef SIGQUIT
  signal_command_map_[SIGQUIT  ] = nullptr;
#endif
  signal_command_map_[SIGABRT  ] = nullptr;
  signal_command_map_[SIGTERM  ] = nullptr;
#ifdef SIGALRM
  signal_command_map_[SIGALRM  ] = nullptr;
#endif
#ifdef SIGCHLD
  signal_command_map_[SIGCHLD  ] = nullptr;
#endif
  signal_command_map_[SIGSEGV  ] = nullptr;
}

void
ClSignalMgr::
initSignalProcs()
{
  setSignalProc("interrupt", ClLanguageSignalHandler);
  setSignalProc("quit"     , ClLanguageSignalHandler);
  setSignalProc("abort"    , ClLanguageSignalHandler);
  setSignalProc("terminate", ClLanguageSignalHandler);
}

void
ClSignalMgr::
termSignalProcs()
{
  setSignalProc("interrupt", nullptr);
  setSignalProc("quit"     , nullptr);
  setSignalProc("abort"    , nullptr);
  setSignalProc("terminate", nullptr);
}

/*------------------------------------------------------------------*
 *
 * setSignalCommand
 *   Set the Command to be Executed when the Named
 *   Signal is raised.
 *
 * CALL:
 *   bool flag =
 *     setSignalCommand(const std::string &name, const std::string &command);
 *
 * INPUT:
 *   name    : Signal Name
 *
 *   command : Command String
 *
 * RETURNS:
 *   flag    : Whether the routine was successful or not.
 *
 *------------------------------------------------------------------*/

bool
ClSignalMgr::
setSignalCommand(const std::string &name, const std::string &command)
{
  int type = signalNameToType(name);

  if (type == -1)
    return false;

  ClSignalCommand *signal_command = getSignalCommand(type);

  if (! signal_command)
    return false;

  if (command != "") {
    if (isUnixSignal(type)) {
      ClSignalHandler proc = (ClSignalHandler) ClLanguageSignalHandler;

      if (! signal_command->proc)
        signal_command->old_proc = COSSignal::getSignalHandler(type);

        COSSignal::addSignalHandler(type, proc);

      signal_command->proc = proc;
    }

    signal_command->command = command;
  }
  else {
    if (isUnixSignal(type)) {
      if (signal_command->old_proc)
        COSSignal::addSignalHandler(type, signal_command->old_proc);
      else
        COSSignal::defaultSignal(type);

      signal_command->proc     = nullptr;
      signal_command->old_proc = nullptr;
    }

    signal_command->command = "";
  }

  return true;
}

/*------------------------------------------------------------------*
 *
 * raiseSignal
 *   Raise the Named Signal to invoke is associated
 *   Command or Routine.
 *
 * CALL:
 *   bool flag =
 *     raiseSignal(const std::string &name, const std::string &data);
 *
 * INPUT:
 *   name : Signal Name
 *
 *   data : Signal Data (message str for "syntax" and "expression").
 *
 * RETURNS:
 *   flag : Whether the routine was successful or not.
 *
 *------------------------------------------------------------------*/

bool
ClSignalMgr::
raiseSignal(const std::string &name, const std::string &data)
{
  int type = signalNameToType(name);

  if (type == -1)
    return false;

  if      (isUnixSignal(type))
    COSSignal::sendSignalToProcessGroup(type);
  else if (type == SIGSYNTAX) {
    char *message;

    ClLanguageArgs *args = new ClLanguageArgs;

    args->startArgs(nullptr);

    if (args->getStringArgList(data, CLArgType::STRING, &message, CLArgType::NONE) != 1)
      return false;

    ClLanguageMgrInst->syntaxError(message);

    args->termArgs();

    delete args;
  }
  else if (type == SIGEXPR) {
    char *message;

    ClLanguageArgs *args = new ClLanguageArgs;

    args->startArgs(nullptr);

    if (args->getStringArgList(data, CLArgType::STRING, &message, CLArgType::NONE) != 1)
      return false;

    ClLanguageMgrInst->expressionError(-1, message);

    args->termArgs();

    delete args;
  }
  else
    return false;

  return true;
}

/*------------------------------------------------------------------*
 *
 * setSignalProc
 *   Set the Routine to be called when the Named Signal is raised.
 *
 * CALL:
 *   bool flag =
 *     setSignalProc(const std::string &name, ClSignalHandler proc);
 *
 * INPUT:
 *   name : Signal Name
 *
 *   proc : Signal Handling routine to call.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the routine was successful or not.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

bool
ClSignalMgr::
setSignalProc(const std::string &name, ClSignalHandler proc)
{
  int type = signalNameToType(name);

  if (type == -1)
    return false;

  ClSignalCommand *signal_command = getSignalCommand(type);

  if (! signal_command)
    return false;

  if (proc) {
    if (isUnixSignal(type)) {
      signal_command->old_proc = COSSignal::getSignalHandler(type);

      COSSignal::addSignalHandler(type, proc);
    }

    signal_command->proc = proc;
  }
  else {
    if (isUnixSignal(type)) {
      if (signal_command->old_proc)
        COSSignal::addSignalHandler(type, signal_command->old_proc);
      else
        COSSignal::defaultSignal(type);
    }

    signal_command->proc     = nullptr;
    signal_command->old_proc = nullptr;

    signal_command->command = "";
  }

  return true;
}

/*------------------------------------------------------------------*
 *
 * checkSignalCommand
 *   Check whether there is a Command associated with
 *   the Signal Type.
 *
 * CALL:
 *   bool flag = checkSignalCommand(int type);
 *
 * INPUT:
 *   type : Signal Type
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether there is a Command associated with the Signal.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

bool
ClSignalMgr::
checkSignalCommand(int type)
{
  ClSignalCommand *signal_command = getSignalCommand(type);

  if (! signal_command)
    return false;

  return (signal_command->command != "");
}

/*------------------------------------------------------------------*
 *
 * executeSignalCommand
 *   Run any Command associated with the Signal Type.
 *
 * CALL:
 *   bool flag = executeSignalCommand(int type);
 *
 * INPUT:
 *   type : Signal Type
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether there is a Command associated with the Signal.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

bool
ClSignalMgr::
executeSignalCommand(int type)
{
  ClSignalCommand *signal_command = getSignalCommand(type);

  if (! signal_command)
    return false;

  if (signal_command->command != "")
    ClLanguageMgrInst->runCommand(signal_command->command);

  return true;
}

/*------------------------------------------------------------------*
 *
 * handleSignal
 *   Signal Handler for UNIX signals which runs any
 *   associated Commands/Routines.
 *
 * CALL:
 *   handleSignal(int sig);
 *
 * INPUT:
 *   sig : Signal Type
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

void
ClSignalMgr::
handleSignal(int sig)
{
  ClSignalCommand *signal_command = getSignalCommand(sig);

  if (signal_command && signal_command->command != "")
    ClLanguageMgrInst->runCommand(signal_command->command);
  else {
    if (sig == SIGINT  || sig == SIGQUIT ||
        sig == SIGABRT || sig == SIGTERM)
      ClLanguageMgrInst->setAbortSignal(true);

    if (sig == SIGINT)
      ClLanguageMgrInst->setInterruptFlag(true);
  }
}

/*------------------------------------------------------------------*
 *
 * getSignalCommand
 *   Get the Signal Command Structure associated with
 *   the specified Signal Type.
 *
 * CALL:
 *   ClSignalCommand *signal_command =
 *     getSignalCommand(int type);
 *
 * INPUT:
 *   type           : Signal Type
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   signal_command : Signal Command Structure
 *                  : (null if none).
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

ClSignalCommand *
ClSignalMgr::
getSignalCommand(int type) const
{
  SignalCommandMap::const_iterator p = signal_command_map_.find(type);

  if (p != signal_command_map_.end())
    return (*p).second;

  return nullptr;
}

/*------------------------------------------------------------------*
 *
 * signalNameToType
 *   Get the Type of the specified Signal Name.
 *
 * CALL:
 *   int type = signalNameToType(comst std::string &name);
 *
 * INPUT:
 *   name : Signal Name
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   type : Signal Type
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

int
ClSignalMgr::
signalNameToType(const std::string &name) const
{
  if      (CStrUtil::casecmp(name, "syntax") == 0)
    return SIGSYNTAX;
  else if (CStrUtil::casecmp(name, "expression") == 0)
    return SIGEXPR;

  return COSSignal::string_to_signal(name);
}

/*------------------------------------------------------------------*
 *
 * signalTypeToName
 *   Get the Name of the specified Signal Type.
 *
 * CALL:
 *   std::string name =
 *     signalTypeToName(int type);
 *
 * INPUT:
 *   type : Signal Type
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   name : Signal Name
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

std::string
ClSignalMgr::
signalTypeToName(int type) const
{
  switch (type) {
    case SIGSYNTAX:
      return "syntax";
      break;
    case SIGEXPR:
      return "expression";
      break;
  }

  return COSSignal::strsignal(type);
}

/*------------------------------------------------------------------*
 *
 * isUnixSignal
 *   Check if a Signal Type is a UNIX Signal.
 *
 * CALL:
 *   bool flag = isUnixSignal(int type);
 *
 * INPUT:
 *   type : Signal Type
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether the Signal Type is a UNIX Signal.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

bool
ClSignalMgr::
isUnixSignal(int type) const
{
  if (type == SIGSYNTAX || type == SIGEXPR)
    return false;

  return true;
}
