#include <CCeilL.h>
#include <COSSignal.h>

using std::string;

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
  signal_command_map_[SIGSYNTAX] = NULL;
  signal_command_map_[SIGEXPR  ] = NULL;
  signal_command_map_[SIGINT   ] = NULL;
#ifdef SIGQUIT
  signal_command_map_[SIGQUIT  ] = NULL;
#endif
  signal_command_map_[SIGABRT  ] = NULL;
  signal_command_map_[SIGTERM  ] = NULL;
#ifdef SIGALRM
  signal_command_map_[SIGALRM  ] = NULL;
#endif
#ifdef SIGCHLD
  signal_command_map_[SIGCHLD  ] = NULL;
#endif
  signal_command_map_[SIGSEGV  ] = NULL;
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
  setSignalProc("interrupt", NULL);
  setSignalProc("quit"     , NULL);
  setSignalProc("abort"    , NULL);
  setSignalProc("terminate", NULL);
}

/*------------------------------------------------------------------*
 *
 * setSignalCommand
 *   Set the Command to be Executed when the Named
 *   Signal is raised.
 *
 * CALL:
 *   bool flag =
 *     setSignalCommand(const string &name, const string &command);
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
setSignalCommand(const string &name, const string &command)
{
  int type = signalNameToType(name);

  if (type == -1)
    return false;

  ClSignalCommand *signal_command = getSignalCommand(type);

  if (signal_command == NULL)
    return false;

  if (command != "") {
    if (isUnixSignal(type)) {
      ClSignalHandler proc = (ClSignalHandler) ClLanguageSignalHandler;

      if (signal_command->proc == NULL)
        signal_command->old_proc = COSSignal::getSignalHandler(type);

        COSSignal::addSignalHandler(type, proc);

      signal_command->proc = proc;
    }

    signal_command->command = command;
  }
  else {
    if (isUnixSignal(type)) {
      if (signal_command->old_proc != NULL)
        COSSignal::addSignalHandler(type, signal_command->old_proc);
      else
        COSSignal::defaultSignal(type);

      signal_command->proc     = NULL;
      signal_command->old_proc = NULL;
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
 *     raiseSignal(const string &name, const string &data);
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
raiseSignal(const string &name, const string &data)
{
  int type = signalNameToType(name);

  if (type == -1)
    return false;

  if      (isUnixSignal(type))
    COSSignal::sendSignalToProcessGroup(type);
  else if (type == SIGSYNTAX) {
    char *message;

    ClLanguageArgs *args = new ClLanguageArgs;

    args->startArgs(NULL);

    if (args->getStringArgList(data, CL_ARG_TYPE_STRING, &message,
                               CL_ARG_TYPE_NONE) != 1)
      return false;

    ClLanguageMgrInst->syntaxError(message);

    args->termArgs();

    delete args;
  }
  else if (type == SIGEXPR) {
    char *message;

    ClLanguageArgs *args = new ClLanguageArgs;

    args->startArgs(NULL);

    if (args->getStringArgList(data, CL_ARG_TYPE_STRING, &message,
                               CL_ARG_TYPE_NONE) != 1)
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
 *     setSignalProc(const string &name, ClSignalHandler proc);
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
setSignalProc(const string &name, ClSignalHandler proc)
{
  int type = signalNameToType(name);

  if (type == -1)
    return false;

  ClSignalCommand *signal_command = getSignalCommand(type);

  if (signal_command == NULL)
    return false;

  if (proc != NULL) {
    if (isUnixSignal(type)) {
      signal_command->old_proc = COSSignal::getSignalHandler(type);

      COSSignal::addSignalHandler(type, proc);
    }

    signal_command->proc = proc;
  }
  else {
    if (isUnixSignal(type)) {
      if (signal_command->old_proc != NULL)
        COSSignal::addSignalHandler(type, signal_command->old_proc);
      else
        COSSignal::defaultSignal(type);
    }

    signal_command->proc     = NULL;
    signal_command->old_proc = NULL;

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

  if (signal_command == NULL)
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

  if (signal_command == NULL)
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

  if (signal_command != NULL && signal_command->command != "")
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
 *                  : (NULL if none).
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

  return NULL;
}

/*------------------------------------------------------------------*
 *
 * signalNameToType
 *   Get the Type of the specified Signal Name.
 *
 * CALL:
 *   int type = signalNameToType(comst string &name);
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
signalNameToType(const string &name) const
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
 *   string name =
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

string
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
