#include <CCeilLI.h>

#include <CCeilLStdCmdI.h>
#include <COSProcess.h>
#include <CArgs.h>
#include <CFile.h>

using std::string;

struct ClLanguageRunFileData {
  ClLanguageCommandList command_list;
  ClLanguageInputFile   save_input_file;
};

ClLanguageMgr *
ClLanguageMgr::
getInstance()
{
  static ClLanguageMgr *instance;

  if (! instance)
    instance = new ClLanguageMgr;

  return instance;
}

ClLanguageMgr::
ClLanguageMgr()
{
  initVars();
}

ClLanguageMgr::
~ClLanguageMgr()
{
  for (int i = 0; i < argc_; i++)
    free(argv_[i]);

  delete [] argv_;
}

void
ClLanguageMgr::
initVars()
{
  argc_ = 0;
  argv_ = nullptr;

  exit_flag_     = false;
  exit_code_     = 0;
  intr_flag_     = false;
  break_flag_    = false;
  continue_flag_ = false;
  return_flag_   = false;

  help_proc_ = nullptr;

  command_name_     = "";
  command_line_num_ = 0;

  depth_ = 0;

  run_file_        = "";
  exit_after_run_  = true;
  echo_commands_   = false;
  ident_           = 1;
  block_command_   = nullptr;
  first_line_      = true;
  prompt_          = "";
  ignore_pos_proc_ = false;
  line_            = "";
  line_len_        = 0;
  line_buffer_     = "";
  abort_signal_    = false;

  command_def_list1_.clear();
  command_def_list2_.clear();

  block_command_stack_.clear();

  command_term_proc_ = nullptr;
  command_term_data_ = nullptr;

  errorMgr_ = new CCeilLErrorMgr;
}

/*------------------------------------------------------------------*
 *
 * init
 *   Initialise Environment for Language and Parser routines.
 *
 * CALL:
 *   init(int &argc, char **argv);
 *
 * INPUT:
 *   argc : Number of UNIX Command Line arguments supplied by the user.
 *
 *   argv : UNIX Command Line arguments supplied by the user.
 *
 * OUTPUT:
 *   argc : Number of UNIX Command Line arguments minus those processed by CL.
 *
 *   argv : UNIX Command Line arguments supplied by the user minus those processed by CL.
 *
 * NOTES:
 *   The following command line arguments are recognised :-
 *
 *    -run <file>         : When promptLoop() is called CL will run the file
 *                        : <file> and then exit.
 *
 *    -startup <file>     : When promptLoop() is called CL will run the file
 *                        : <file> and start the Command Line Interface.
 *
 *    -echo               : All commands entered via the Command Line Interface or from
 *                        : a file being run are output to screen before being executed.
 *                        : This is useful for debugging.
 *
 *    -ldebug             : Turn on Language Debug Mode which outputs a trace of all
 *                        : the routines being called. This is useful for debugging.
 *
 *    -pdebug             : Turn on Parser Debug Mode which outputs the execution
 *                        : stack after each process which modifies the stack.
 *                        : This is useful for debugging.
 *
 *    -prompt             : Sets the str which appears at the start of each input line
 *                        : in the Command Line Interface.
 *
 *    -exp <expression>   : Run Expression <expression> which can be used to initialise
 *                        : variables required for a particular run of CL.
 *
 *    -var <name> <value> : Set Variable <name> to the str <value>.
 *
 *    -nocase             : Parser is non-case specific
 *
 *   Any remaining arguments are stored in the str array variable '_argv'.
 *
 *   This routine must be called before any of the ClLanguage routines are called.
 *   If only the ClParser routines are called then the ClParserInit routine can be
 *   used instead.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
init(int *argc, char **argv)
{
  const char *opts = "\
 -run:s     (run a file and then exit) \
 -startup:s (run the file and start prompt loop) \
 -echo:f    (echo executed commands) \
 -ldebug:f  (enable language debug) \
 -pdebug:f  (enable parser debug) \
 -prompt:s  (set prompt) \
 -exp:s     (evaluate expression) \
 -var:s     (set variable) \
 -nocase:f  (set case insensitive) \
 -exit:f    (exit after evaluating expression) \
";

  CArgs cargs(opts);

  if (argc && argv)
    cargs.parse(argc, argv);

  if (cargs.getBooleanArg("-nocase"))
    ClParserInst->setCaseSensitive(false);

  /***********/

  /* Initialise Parser */

  ClParserInst->init();

  ClParserInst->setOutputFile(stdout);

  /***********/

  /* Process Command Line Arguments */

  ClLanguageDebugInst->setDebug(false);

  if      (cargs.isStringArgSet("-run")) {
    setRunFile(cargs.getStringArg("-run"));
    setExitAfterRun(true);
  }
  else if (cargs.isStringArgSet("-startup")) {
    setRunFile(cargs.getStringArg("-startup"));
    setExitAfterRun(false);
  }

  setEchoCommands(cargs.getBooleanArg("-echo"));

  if (cargs.getBooleanArg("-ldebug")) {
    ClLanguageDebugInst->setDebug    (true);
    ClLanguageDebugInst->setDebugFile(stdout);
  }

  if (cargs.getBooleanArg("-pdebug"))
    ClParserInst->setDebug(true);

  if (getRunFile() != "")
    setExitAfterRun(true);

  string prompt = ">";

  if (cargs.isStringArgSet("-prompt"))
    prompt = cargs.getStringArg("-prompt");

  bool isExp  = cargs.isStringArgSet("-exp");
  bool isExit = cargs.getBooleanArg("-exit");

  if (isExp) {
    string exp = cargs.getStringArg("-exp");

    ClParserExpr expr(exp);

    ClParserValuePtr value;

    if      (! expr.exec(value))
      ClLanguageMgrInst->expressionError
       (CLERR_INVALID_EXPRESSION, "'-exp' expression '%s'", exp.c_str());
    else if (! value.isValid())
      ClLanguageMgrInst->syntaxError
       ("undefined '-exp' expression '%s'", exp.c_str());
    else if (isExit) {
      value->print();

      std::cout << std::endl;
    }
  }

#if 0
  if (cargs.isStringArgSet("-var")) {
    string name  = cargs.getStringArg("-var");
    string value = "";

    ClParserInst->createVar(name, value);
  }
#endif

  /***********/

  /* Store remaining arguments in external */

  if (argc != nullptr)
    setArgv(*argc, argv);
  else
    setArgv(0, nullptr);

  /***********/

  ClStdCommandsAdd();

  ClModuleMgrInst->importModule("builtin");

  /***********/

  /* Set Prompt */

  setPrompt(prompt);

  /***********/

  /* Initialise Exit and Return Flag */

  setExitFlag(false);

  setReturnFlag(false);

  /***********/

  /* Initialize First Line Flag so any Initial #! can be ignored */

  setFirstLine(true);

  /***********/

#ifdef CEIL_READLINE
  getReadLine().setName("ceil");
#endif

  /***********/

  if (isExp && isExit)
    setExitFlag(true);
}

/*------------------------------------------------------------------*
 *
 * reinit
 *   Re-Initialise Environment for Language and Parser
 *   routines.
 *
 * CALL:
 *   reinit();
 *
 * NOTES:
 *   Deletes all currently defined variables, structures, procedures and
 *   functions. The standard variables, procedures and functions are then
 *   re-added.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
reinit()
{
  string prompt = getPrompt();

  /* Delete all Variables, Functions and Procedures */

  ClParserInst->deleteAllVariables();
  ClParserInst->deleteAllTypes();
  ClParserInst->deleteAllFunctions();

  ClLanguageProcMgrInst->deleteProcs();
  ClLanguageFuncMgrInst->deleteFuncs();

  /* Add Standard Structure Types and Variables */

  ClModuleMgrInst->reinitModules();

  setPrompt(prompt);
}

/*------------------------------------------------------------------*
 *
 * term
 *   Terminate Language and Parser routines freeing any
 *   allocated resources.
 *
 * CALL:
 *   term();
 *
 * NOTES:
 *   This routine should be called when the application
 *   has finished using the CL routines.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
term()
{
  /* Delete Files, Labels, Procedures, Functions and Command Defs */

  ClLanguageProcMgrInst->deleteProcs();
  ClLanguageFuncMgrInst->deleteFuncs();

  ClLanguageLabelDataMgrInst->clearLabelDataStack();
  ClLanguageLabelDataMgrInst->clearCurrentLabelDatas();

  deleteCommandDefs();

  ClModuleMgrInst->termModules();

  ClModuleMgrInst->deleteModules();

  /***********/

  /* Terminate Parser Routines */

  ClParserInst->term();
}

/*------------------------------------------------------------------*
 *
 * promptLoop
 *   Start the Command Line Interface which allows
 *   the user to enter any of the define Language
 *   commands. This routine will return when the
 *   Command Line Interface is terminated either
 *   because the specfied run file has been executed
 *   or the user has entered the 'exit' command.
 *
 * CALL:
 *   promptLoop();
 *
 * NOTES:
 *   When this routine is called the application
 *   effectively reliquishes control to CL. All
 *   required customisation must be done before this
 *   routine is entered.
 *
 *   If the application requires more command by command
 *   control it should use runCommand() and provide its
 *   own user interface.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
promptLoop()
{
  if (getExitFlag())
    return;

  /* Startup Command Line Interface */

  startup();

  /* Process Commands until 'exit' is Entered */

  ClSignalMgrInst->initSignalProcs();

  while (! getExitFlag()) {
    if (readLine()) {
      if (getEchoCommands()) {
        for (int i = 0; i < getDepth(); i++)
          ClLanguageDebugInst->print(" ");

        ClLanguageDebugInst->print("%s\n", getLine().c_str());
      }

      ClLanguageCommand *command = processLine(getLine());

      if (command != nullptr) {
        processCommand(command);

        delete command;
      }
    }

    /* Handle User Interrupt */

    if (checkAbort()) {
      ClLanguageMgrInst->error("** Interrupt ** program terminated");

      setExitFlag(true);
    }
  }

  ClSignalMgrInst->termSignalProcs();
}

/*------------------------------------------------------------------*
 *
 * startup
 *   Perform startup operations before the Command Line
 *   Interface is started.
 *
 * CALL:
 *   startup();
 *
 * NOTES:
 *   Normally called automatically by promptLoop()
 *   but needs to be called manually if an application
 *   supplied prompt loop is used.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
startup()
{
  setExitFlag(false);

  /* Process Command Line Run/Startup File */

  if (getRunFile() != "") {
    if (getRunFile() != "-") {
      if (CFile::exists(getRunFile()) && CFile::isRegular(getRunFile()))
        runFile(getRunFile());
      else {
        if (! CFile::exists(getRunFile()))
          ClLanguageMgrInst->error("failed to find startup/run file '%s'",
                                    getRunFile().c_str());
        else
          ClLanguageMgrInst->error("startup/run file '%s' is not a "
                                    "regular file", getRunFile().c_str());
      }
    }
    else
      runFile("");

    setRunFile("");

    if (getExitAfterRun())
      setExitFlag(true);
  }
}

/*------------------------------------------------------------------*
 *
 * runFile
 *   Run the commands in the specified file.
 *
 * CALL:
 *   runFile(const string &file_name);
 *
 * INPUT:
 *   file_name : The name of the file containing the commands to be run.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
runFile(const string &file_name)
{
  bool input_terminated;

  /*------------*/

  string prompt;

  if (file_name == "")
    prompt = getPrompt();

  /*------------*/

  /* Save Details of any Current File */

  ClLanguageRunFileData *run_file = new ClLanguageRunFileData;

  run_file->save_input_file = getInputFile();

  /*------------*/

  /* Open File for Read and Set the Input File Pointer */

  ClLanguageInputFile &input_file = getInputFile();

  if (file_name != "") {
    if (! input_file.setName(file_name)) {
      goto runFile_1;
    }
  }
  else
    input_file.setFp(stdin);

  /*------------*/

  /* Process Commands until Input File is terminated */

  incDepth();

  input_terminated = false;

  while (! input_terminated) {
    if (readLine()) {
      if (getEchoCommands()) {
        for (int i = 0; i < getDepth(); i++)
          ClLanguageDebugInst->print(" ");

        ClLanguageDebugInst->print("%s\n", getLine().c_str());
      }

      if (getFirstLine()) {
        setFirstLine(false);

        if (getLine().size() > 0 && getLine()[0] == '#')
          setLine("");
      }

      ClLanguageCommand *command = processLine(getLine());

      if (command != nullptr) {
        if (! command->isSpecial()) {
          ClLanguageCommandDefCommand *ccommand =
            (ClLanguageCommandDefCommand *) command;

          ClLanguageCommandDef *command_def = ccommand->getCommandDef();

          /* Ensure Import, Procedure and Function Commands
             are processed as soon as they are encountered
             and not stored in the set of commands to
             be run */

          if (ClIsImportCommand(command) ||
              command_def->getType() == CL_BLOCK_TYPE_PROCEDURE_BLOCK) {
            processCommand(command);

            delete command;
          }
          else
            run_file->command_list.push_back(command);
        }

        /* Add normal command to list for later running, thus allows goto's etc. */

        else
          run_file->command_list.push_back(command);
      }
    }
    else
      input_terminated = true;

    /* Handle User Interrupt */

    if (checkAbort()) {
      ClLanguageMgrInst->error("** Interrupt ** run of file '%s' abandoned", file_name.c_str());

      getInputFile().setFp(stdin);

      input_terminated = true;
    }
  }

  decDepth();

  /* Ensure All Block Commands are Terminated */

  if (getBlockCommand() != nullptr) {
    clearBlockCommandStack(true);

    goto runFile_1;
  }

  /* Run File's Commands */

  if (! getAbortSignal()) {
    setCommandName("run");

    ClLanguageBlockDataMgrInst->startBlock(run_file->command_list, CL_BLOCK_TYPE_FILE_BLOCK);

    ClLanguageBlockDataMgrInst->processBlockCommands();

    ClLanguageBlockDataMgrInst->endBlock();
  }

 runFile_1:
  if (file_name == "")
    setPrompt(prompt);

  /* Restore Details of any Current File */

  setInputFile(run_file->save_input_file);

  /* Clean Up */

  if (run_file) {
    for (auto &command : run_file->command_list)
      delete command;

    delete run_file;
  }
}

/*------------------------------------------------------------------*
 *
 * readLine
 *   Read the next line from the input device. The input
 *   device could be the terminal or a file.
 *
 * CALL:
 *   bool flag = readLine();
 *
 * RETURNS:
 *   flag  : Whether a line has been successfully read or not.
 *
 * NOTES:
 *   Read line is stored in the 'line_' external variable.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageMgr::
readLine()
{
  string line;
  bool   more;
  int    length;

#ifndef CEIL_READLINE
  if (getLineBuffer() == "" && getInputFile().getFp() == stdin)
    outputPrompt();
#endif

  setLine("");

  ClLanguageInputFile &input_file = getInputFile();

  do {
    if (getLineBuffer() != "")
      line = getLineBuffer();
    else {
      if (input_file.getFp() != stdin) {
        if (! input_file.readLine(line)) {
          if (input_file.getFp() != stdin)
            input_file.setFp(stdin);
          else
            setExitFlag(true);

          return false;
        }
      }
      else {
#ifdef CEIL_READLINE
        string temp_prompt = "";

        if (getLine() == "") {
          for (int j = 0; j < getDepth(); j++)
            temp_prompt += '+';

          temp_prompt += getPrompt();
          temp_prompt += " ";
        }

        getReadLine().setPrompt(temp_prompt);

        line = getReadLine().readLine();
#else
        if (! input_file.readLine(line))
          return false;
#endif
      }
    }

    input_file.setLineNum(input_file.getLineNum() + 1);

    length = line.size();

    if (length > 0 && line[length - 1] == '\\') {
      line = line.substr(0, length - 1);

      more = true;
    }
    else
      more = false;

    appendLine(line);

    setLineBuffer("");
  }
  while (more);

  setLine(CStrUtil::stripSpaces(getLine()));

  if (ClLanguageDebugInst->isDebug())
    ClLanguageDebugInst->print("%s\n", getLine().c_str());

#ifdef CEIL_READLINE
  if (getLine() != "")
    getReadLine().addHistory(getLine());
#endif

  return true;
}

/*------------------------------------------------------------------*
 *
 * runCommand
 *   This routine executes the command specified in the
 *   supplied str. This routine performs exactly the
 *   same operation as when each line is entered at the
 *   Command Line Prompt provided by promptLoop().
 *
 * CALL:
 *   bool exit_flag =
 *     runCommand(const string &command_string);
 *
 * INPUT:
 *   command_string : The command to be executed.
 *
 * RETURNS:
 *   exit_flag      : This flag indicates whether exit
 *                  : has been entered indicating that
 *                  : the Command Interface should be
 *                  : terminated.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageMgr::
runCommand(const string &command_string)
{
  setExitFlag(false);

  string command_string1 = CStrUtil::stripSpaces(command_string);

  ClLanguageCommand *command = processLine(command_string1);

  if (command != nullptr) {
    processCommand(command);

    delete command;
  }

  return getExitFlag();
}

/*------------------------------------------------------------------*
 *
 * processLine
 *   Parse line entered by the user and return entered
 *   command (if any).
 *
 * CALL:
 *   ClLanguageCommand *command =
 *     processLine(const string &line);
 *
 * INPUT:
 *   line    : Line entered by the user.
 *
 * RETURNS:
 *   command : Command parsed from the entered line (nullptr if none).
 *
 *------------------------------------------------------------------*/

ClLanguageCommand *
ClLanguageMgr::
processLine(const string &line)
{
  int                   i;
  string                args;
  string                line1;
  ClParserScopePtr      scope;
  ClLanguageProc       *procedure;
  bool                  is_end_name;
  ClLanguageCommandDef *command_def;
  string                command_name;

  ClLanguageCommand *command = nullptr;

  CStrParse parse(line);

  /********/

  /* Check for Blank Line or Comment */

  parse.skipSpace();

  if (parse.isChar(';')) {
    ClLanguageMgrInst->error("Warning: Old Comment Syntax - Change to '#'");
    return nullptr;
  }

  if (parse.isChar('#') || parse.isString("//"))
    return nullptr;

  /********/

  /* Check for System Command ( ';' supported if bracketed) */

  if (parse.isChar('!')) {
    parse.skipChar();

    parse.skipSpace();

    if (parse.isChar('(')) {
      parse.skipChar();

      i = parse.getPos();

      if (! ClLanguageArgs::readArgList(line, &i, ')', args)) {
        ClLanguageMgrInst->syntaxError
         ("missing close brackets for '!(' '%s' argument list", line.c_str());
        return nullptr;
      }

      parse.setPos(i);

      parse.skipSpace();

      if      (parse.isChar(';')) {
        parse.skipChar();

        parse.skipSpace();

        setLineBuffer(parse.getAt());
      }
      else if (parse.isString("//"))
        ;
      else if (! parse.eof()) {
        ClLanguageMgrInst->syntaxError
         ("spurious characters '%s' after '!' argument list", parse.getAt().c_str());
        return nullptr;
      }
    }
    else
      args = parse.getAt();

    CStrUtil::stripSpaces(args);

    command = new ClLanguageSystemCommand(args);

    goto processLine_1;
  }

  /********/

  /* Check for Execute String Command ( ';' supported if bracketed) */

  if (parse.isChar('$') && ! ClParserInst->getDollarPrefix()) {
    parse.skipChar();

    parse.skipSpace();

    if (parse.isChar('(')) {
      parse.skipChar();

      parse.skipSpace();

      i = parse.getPos();

      if (! ClLanguageArgs::readArgList(line, &i, ')', args)) {
        ClLanguageMgrInst->syntaxError
         ("missing close brackets for '!(' '%s' argument list", line.c_str());
        return nullptr;
      }

      parse.setPos(i);

      parse.skipSpace();

      if      (parse.isChar(';')) {
        parse.skipChar();

        parse.skipSpace();

        setLineBuffer(parse.getAt());
      }
      else if (parse.isString("//"))
        ;
      else if (! parse.eof()) {
        ClLanguageMgrInst->syntaxError
         ("spurious characters '%s' after '$' argument list", parse.getAt().c_str());
        return nullptr;
      }
    }
    else
      args = parse.getAt();

    CStrUtil::stripSpaces(args);

    command = new ClLanguageLanguageCommand(args);

    goto processLine_1;
  }

  /********/

  /* Extract possible Command Name */

  if (parse.readIdentifier(command_name)) {
    string        str;
    StringVectorT scopes;

    while (parse.isString("::")) {
      parse.skipChars(2);

      if (! parse.readIdentifier(str))
        break;

      scopes.push_back(command_name);

      command_name = str;
    }

    if (! scopes.empty())
      scope = ClParserInst->getScope(scopes);
  }
  else
    command_name = "";

  /* Skip to next Non-Blank */

  parse.skipSpace();

  /* Skip to end of line if comment */

  if (parse.isString("//"))
    parse.skipToEnd();

  /********/

  /* Check Command Definitions for Match */

  command_def = getCommandDef(scope, command_name, &is_end_name);

  if (command_def != nullptr) {
    i = parse.getPos();

    if (! ClLanguageArgs::readArgList(line, &i, '\0', args))
      args = "";

    parse.setPos(i);

    parse.skipSpace();

    if      (parse.isChar(';')) {
      parse.skipChar();

      parse.skipSpace();

      setLineBuffer(parse.getAt());
    }
    else if (parse.isString("//"))
      ;
    else if (! parse.eof()) {
      ClLanguageMgrInst->syntaxError
       ("spurious characters '%s' after 'command' '%s' argument list",
        parse.getAt().c_str(), command_name.c_str());
      return nullptr;
    }

    CStrUtil::stripSpaces(args);

    /*-------------*/

    command = new ClLanguageCommandDefCommand(command_def, args, is_end_name);

    goto processLine_1;
  }

  /********/

  /* If no Match then Check Procedures for Match */

  procedure = ClLanguageProcMgrInst->lookupProc(command_name);

  if (procedure != nullptr ||
      (! getIgnorePosProc() && parse.isChar('('))) {
    if (! parse.isChar('(')) {
      if (procedure == nullptr)
        goto processLine_2;

      ClLanguageMgrInst->syntaxError
       ("missing open brackets for 'proc' '%s' argument list", procedure->getName().c_str());
      return nullptr;
    }

    parse.skipChar();

    parse.skipSpace();

    i = parse.getPos();

    if (! ClLanguageArgs::readArgList(line, &i, ')', args)) {
      if (procedure == nullptr)
        goto processLine_2;

      ClLanguageMgrInst->syntaxError
       ("missing close brackets for 'proc' '%s' argument list", procedure->getName().c_str());
      return nullptr;
    }

    parse.setPos(i);

    parse.skipSpace();

    if      (parse.isChar(';')) {
      parse.skipChar();

      parse.skipSpace();

      setLineBuffer(parse.getAt());
    }
    else if (parse.isString("//"))
      ;
    else if (! parse.eof()) {
      if (procedure == nullptr)
        goto processLine_2;

      ClLanguageMgrInst->syntaxError
       ("spurious characters '%s' after 'proc' '%s' argument list", parse.getAt().c_str(),
        procedure->getName().c_str());
      return nullptr;
    }

    CStrUtil::stripSpaces(args);

    if (procedure != nullptr)
      command = new ClLanguageProcCommand(procedure, args);
    else
      command = new ClLanguagePosProcCommand(command_name, line);

    goto processLine_1;
  }

  /********/

  /* Check for Label */

  if (parse.isChar(':') && ! parse.isString("::")) {
    parse.skipChar();

    parse.skipSpace();

    if      (parse.isChar(';')) {
      parse.skipChar();

      parse.skipSpace();

      setLineBuffer(parse.getAt());
    }
    else if (parse.isString("//"))
      ;
    else if (! parse.eof()) {
      ClLanguageMgrInst->syntaxError
       ("spurious characters after label '%s'", command_name.c_str());
      return nullptr;
    }

    if (getDepth() == 0) {
      ClLanguageMgrInst->syntaxError
       ("labels only allowed inside a block command or a file");
      return nullptr;
    }

    ClLanguageLabelData *label_data =
      ClLanguageLabelDataMgrInst->createLabelData(command_name);

    command = new ClLanguageLabelCommand(label_data);

    goto processLine_1;
  }

  /********/

 processLine_2:
  /* If no Match then it must be an Expression */

  /********/

  /* Split Line if there is a ';' */

  parse.setPos(0);

  i = parse.getPos();

  ClLanguageArgs::skipArgList(line, &i, '\0');

  parse.setPos(i);

  parse.skipSpace();

  if      (parse.isChar(';')) {
    parse.skipChar();

    parse.skipSpace();

    setLineBuffer(parse.getAt());

    line1 = line.substr(0, i);
  }
  else if (parse.eof() || parse.isString("//"))
    line1 = line.substr(0, i);

  /********/

  /* Add the Expression Command */

  command = new ClLanguageExpressionCommand(command_name, line1);

  goto processLine_1;

  /********/

 processLine_1:
  /* If we get a RetAll Command then dump all Current Block Commands */

  if (ClIsRetAllCommand(command)) {
    clearBlockCommandStack(false);

    delete command;

    command = nullptr;

    return nullptr;
  }

  /* Add Block Commands to Current Block Command List until
     End Block Command is received */

  if (! command->isSpecial()) {
    ClLanguageCommandDefCommand *ccommand =
      (ClLanguageCommandDefCommand *) command;

    ClLanguageCommandDef *command_def = ccommand->getCommandDef();

    if (command_def->getEndName() != "")
      command = startBlockCommand(command);
    else {
      if (getBlockCommand() != nullptr) {
        getBlockCommand()->addCommand(command);

        command = nullptr;
      }
    }
  }
  else {
    if (getBlockCommand() != nullptr) {
      getBlockCommand()->addCommand(command);

      command = nullptr;
    }
  }

  return command;
}

/*------------------------------------------------------------------*
 *
 * processCommand
 *   Execute the specified command.
 *
 * CALL:
 *   processCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : The command structure : containing the
 *           : definition of the command.
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
ClLanguageMgr::
processCommand(ClLanguageCommand *command)
{
  int error_code = 0;

  if (command == nullptr)
    goto processCommand_1;

  /*----------*/

  /* Set Current Command Name and Line Number */

  setCommandName(command->toName());

  setCommandLineNum(command->getLineNum());

  /*----------*/

  /* Process Command */

  if (command->isSpecial()) {
    if      (command->isProcedure()) {
      ClLanguageProcCommand *pcommand = (ClLanguageProcCommand *) command;

      ClLanguageProc *procedure = pcommand->getProcedure();

      resetLastValue();

      ClLanguageBlockDataMgrInst->startBlock(procedure->getCommandList(),
                                             CL_BLOCK_TYPE_PROCEDURE_BLOCK);

      ClLanguageArgs *args = new ClLanguageArgs;

      args->startArgs(command);

      runProcedure(procedure, args);

      args->termArgs();

      delete args;

      ClLanguageBlockDataMgrInst->endBlock();
    }
    else if (command->isPosProcedure()) {
      ClLanguagePosProcCommand *pcommand = (ClLanguagePosProcCommand *) command;

      setIgnorePosProc(true);

      ClLanguageCommand *command1 = processLine(pcommand->getLine());

      setIgnorePosProc(false);

      if (command1 == nullptr)
        goto processCommand_1;

      command->assign(*command1);

      delete command1;

      processCommand(command);
    }
    else if (command->isLabel())
      ;
    else if (command->isExpression()) {
      ClLanguageArgs *args = new ClLanguageArgs;

      args->startArgs(command);

      processExpression(command, args);

      args->termArgs();

      delete args;
    }
    else if (command->isSystem()) {
      ClLanguageSystemCommand *scommand = (ClLanguageSystemCommand *) command;

      string                   ostr;
      COSProcess::CommandState command_state;

      string args = scommand->getArgs();

      int len = args.size();

      if      (len > 0 && args[0] == '$') {
        string exp = args.substr(1);

        ClParserExpr expr(exp);

        ClParserValuePtr value;

        if      (! expr.exec(value)) {
          ClLanguageMgrInst->expressionError
           (CLERR_INVALID_EXPRESSION, "'$' expression '%s'", exp.c_str());
          goto processCommand_1;
        }

        if (! value.isValid()) {
          ClLanguageMgrInst->syntaxError
           ("undefined '$' expression '%s'", exp.c_str());
          goto processCommand_1;
        }

        string command_string;

        if (! value->stringValue(command_string)) {
          ClLanguageMgrInst->expressionError
           (CLERR_INVALID_EXPRESSION, "'$' expression '%s'", exp.c_str());
          goto processCommand_1;
        }

        COSProcess::executeCommand(command_string, ostr, &command_state);
      }
      else if (len > 1 && args[0] == '\\' && args[1] == '$') {
        string expr = args.substr(1);

        COSProcess::executeCommand(expr, ostr, &command_state);
      }
      else
        COSProcess::executeCommand(args, ostr, &command_state);

      output("%s", ostr.c_str());

      if      (command_state.isSignaled())
        output("Killed by signal %d\n" , command_state.getSignum());
      else if (command_state.isStopped())
        output("Stopped by signal %d\n", command_state.getSignum());

      ClParserVarPtr var = ClParserInst->getVariable("_rc", true);

      var->setValue(ClParserValueMgrInst->createValue(
        (long) command_state.getStatus()));

      fflush(stdout);
      fflush(stderr);
    }
    else if (command->isLanguage()) {
      ClLanguageLanguageCommand *lcommand =
        (ClLanguageLanguageCommand *) command;

      string str;

      const string &args = lcommand->getArgs();

      ClParserExpr expr(args);

      ClParserValuePtr value;

      if (! expr.exec(value)) {
        ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION, "'$' expression '%s'",
                                           args.c_str());
        goto processCommand_1;
      }

      if (! value->stringValue(str))
        error_code = CLERR_INVALID_CONVERSION;

      if (error_code != 0) {
        ClLanguageMgrInst->expressionError
         (error_code, "'$' expression '%s'", args.c_str());
        goto processCommand_1;
      }

      command = processLine(str);

      if (command != nullptr) {
        processCommand(command);

        delete command;
      }
    }
    else
      ClLanguageMgrInst->error("Invalid Command Identification");
  }
  else {
    ClLanguageCommandDefCommand *ccommand =
      (ClLanguageCommandDefCommand *) command;

    ClLanguageCommandDef *command_def = ccommand->getCommandDef();

    if (! (command->isEndBlock())) {
      if (command_def != nullptr && command_def->getFunction() != nullptr) {
        if (command_def->getEndName() != "") {
          const ClLanguageCommandList &command_list =
            ccommand->getCommandList();

          ClLanguageBlockDataMgrInst->startBlock(command_list, command_def->getType());

          command_def->runFunction(command);

          ClLanguageBlockDataMgrInst->endBlock();
        }
        else
          command_def->runFunction(command);
      }
    }
    else
      ClLanguageMgrInst->syntaxError("'%s' without corresponding '%s'",
                                      command_def->getName   ().c_str(),
                                      command_def->getEndName().c_str());
  }

 processCommand_1:
  ClParserInst->restartStack();

  if (command_term_proc_ != nullptr)
    (*command_term_proc_)(command_term_data_);
}

/*------------------------------------------------------------------*
 *
 * runProcedure
 *   Run the specified procedure.
 *
 * CALL:
 *   runProcedure(ClLanguageProc *procedure);
 *
 * INPUT:
 *   procedure : The procedure structure.
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
ClLanguageMgr::
runProcedure(ClLanguageProc *procedure, ClLanguageArgs *args)
{
  int error_code;

  ClParserValuePtr *values = nullptr;

  /* Get Command's Arguments */

  uint num_args = args->getNumArgs();

  if (args->checkNumberOfArgs(procedure->getNumArgs(), num_args) != 0)
    goto runProcedure_1;

  /* Create Values Array */

  if (num_args > 0) {
    values = new ClParserValuePtr [num_args];

    for (uint i = 0; i < num_args; i++)
      values[i] = ClParserValuePtr();
  }

  /* Evaluate each Argument */

  for (uint i = 0; i < num_args; i++) {
    string arg = args->getArg(i + 1, &error_code);

    if (error_code != 0)
      goto runProcedure_1;

    const ClLanguageProcArg &parg = procedure->getArg(i);

    if (parg.isReturned()) {
      if (! ClParserInst->isValidAssignString(arg)) {
        ClLanguageMgrInst->syntaxError
         ("invalid procedure '%s' return argument %d - '%s'",
          procedure->getName().c_str(), i + 1, arg.c_str());
        goto runProcedure_1;
      }

      ClParserExpr expr(arg);

      if (! expr.exec(values[i]))
        values[i] = ClParserValuePtr();
    }
    else {
      ClParserExpr expr(arg);

      if (! expr.exec(values[i])) {
        ClLanguageMgrInst->expressionError
         (CLERR_INVALID_EXPRESSION, "procedure '%s' argument %d - '%s'",
          procedure->getName().c_str(), i + 1, arg.c_str());
        goto runProcedure_1;
      }

      if (! values[i].isValid()) {
        ClLanguageMgrInst->syntaxError
         ("procedure '%s' argument %d - '%s' : undefined expression",
          procedure->getName().c_str(), i + 1, arg.c_str());
        goto runProcedure_1;
      }
    }
  }

  /* Create fresh Variable List for Procedure */

  ClParserInst->newVariableList();
  ClParserInst->newTypeList();

  /* Create Variables from Procedure's Arguments using the
     Command Line Values */

  for (uint i = 0; i < num_args; i++) {
    if (values[i].isValid()) {
      const ClLanguageProcArg &parg = procedure->getArg(i);

      ClParserInst->createVar(parg.getName(), values[i]);

      values[i] = ClParserValuePtr();
    }
  }

  /* Run the Procedure's Commands */

  ClLanguageBlockDataMgrInst->processBlockCommands();

  /* Get the Values of the Variables to be Returned */

  for (uint i = 0; i < num_args; i++) {
    const ClLanguageProcArg &parg = procedure->getArg(i);

    if (parg.isReturned()) {
      values[i] = ClParserInst->getVariableValue(parg.getName());

      if (! values[i].isValid()) {
        ClLanguageMgrInst->syntaxError
         ("invalid procedure '%s' return argument %d - '%s'",
          procedure->getName().c_str(), i + 1, parg.getName().c_str());
        goto runProcedure_2;
      }
    }
  }

  /* Return to previous Variable List */

  ClParserInst->oldTypeList();
  ClParserInst->oldVariableList();

  /* Set the Values of the Variables to be Returned */

  for (uint i = 0; i < num_args; i++) {
    const ClLanguageProcArg &parg = procedure->getArg(i);

    if (parg.isReturned()) {
      string arg = args->getArg(i + 1, &error_code);

      if (error_code != 0)
        goto runProcedure_1;

      ClParserInst->assignValue(arg, values[i], &error_code);

      if (error_code != 0) {
        ClLanguageMgrInst->expressionError
         (error_code, "procedure '%s' return argument %d - '%s'",
          procedure->getName().c_str(), i + 1, parg.getName().c_str());
        goto runProcedure_1;
      }

      values[i] = ClParserValuePtr();
    }
  }

  goto runProcedure_1;

 runProcedure_2:
  /* Return to previous Variable List */

  ClParserInst->oldTypeList();
  ClParserInst->oldVariableList();

 runProcedure_1:
  delete [] values;
}

/*------------------------------------------------------------------*
 *
 * runFunction
 *   Perform a function evaluation.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     runFunction(ClParserValuePtr *values, int num_values, void *data, int &error_code);
 *
 * INPUT:
 *   values     : The array of values supplied
 *              : when the function was called.
 *
 *   num_values  : The number of values supplied
 *              : when the function was called.
 *
 *   data       : The optional data specified
 *              : when the function was specified.
 *
 * OUTPUT:
 *   error_code : Error Code to indicate whether
 *              : the function was successful.
 *              :  <0 : Function failed (function
 *              :     : specific error code).
 *              :  =0 : Function successful.
 *              :  >0 : Function failed (CL
 *              :     : specific error code).
 *
 * RETURNS:
 *   value      : The value of the function.
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

ClParserValuePtr
ClLanguageMgr::
runFunction(ClParserValuePtr *values, uint num_values, void *data, int *error_code)
{
  *error_code = 0;

  ClParserValuePtr value;

  /* Dereference Function */

  ClLanguageFunc *function = (ClLanguageFunc *) data;

  /* Set Command Name */

  setCommandName(function->getName());

  /* Start a Procedure Block */

  resetLastValue();

  ClLanguageBlockDataMgrInst->startBlock(function->getCommandList(), CL_BLOCK_TYPE_PROCEDURE_BLOCK);

  /* Create fresh Variable List for Function */

  ClParserInst->newVariableList();
  ClParserInst->newTypeList();

  ClParserVarPtr variable = ClParserInst->getVariable("_return");

  /* Create Variables from Function's Arguments using the
     Command Line Values */

  uint num_args = function->getNumArgs();

  for (uint i = 0; i < std::min(num_args, num_values); i++)
    ClParserInst->createVar(function->getArg(i).getName(), values[i]);

  /* Run the Function's Commands */

  ClLanguageBlockDataMgrInst->processBlockCommands();

  /* Get Return Value */

  if (! ClParserInst->isVariable("_return")) {
    if (getLastValue().isValid())
      ClParserInst->createVar("_return", getLastValue());
    else
      ClParserInst->createVar("_return", ClParserValueMgrInst->createValue(0L));
  }

  value = ClParserInst->getVariableValue("_return");

  if (! value.isValid()) {
    ClLanguageMgrInst->syntaxError
     ("no '_return' value specified for function '%s'", function->getName().c_str());
    *error_code = -2;
    goto runFunction_1;
  }

  /* Get Values for Function's Output Arguments and Copy back
     into the Passed Values */

  for (uint i = 0; i < num_args; i++) {
    if (! function->getArg(i).isOutput())
      continue;

    ClParserValuePtr value1 =
      ClParserInst->getVariableValue(function->getArg(i).getName());

    if (! value1.isValid())
      continue;

    values[i] = value1;
  }

 runFunction_1:
  /* Return to previous Variable List */

  ClParserInst->oldTypeList();
  ClParserInst->oldVariableList();

  ClLanguageBlockDataMgrInst->endBlock();

  return value;
}

/*------------------------------------------------------------------*
 *
 * processExpression
 *   Process an expression command.
 *
 * CALL:
 *   processExpression(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : The command structure for the expression.
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
ClLanguageMgr::
processExpression(ClLanguageCommand *, ClLanguageArgs *args)
{
  int error_code;

  uint num_args = args->getNumArgs();

  for (uint i = 1; i <= num_args; i++) {
    string arg = args->getArg(i, &error_code);

    if (error_code != 0)
      return;

    ClParserExpr expr(arg);

    ClParserValuePtr value;

    if (! expr.exec(value)) {
      if (num_args == 1)
        ClLanguageMgrInst->expressionError
         (CLERR_INVALID_EXPRESSION, "argument '%s'", arg.c_str());
      else
        ClLanguageMgrInst->expressionError
         (CLERR_INVALID_EXPRESSION, "argument %d - '%s'", i, arg.c_str());
      return;
    }

    if (! value.isValid()) {
      if (num_args == 1)
        ClLanguageMgrInst->syntaxError
          ("undefined argument '%s'", arg.c_str());
      else
        ClLanguageMgrInst->syntaxError
          ("undefined argument %d - '%s'", i, arg.c_str());
      return;
    }

    setLastValue(value);
  }
}

/*------------------------------------------------------------------*
 *
 * checkControlFlags
 *   Check whether any of the flow control flags have
 *   been set (exit, goto, interrupt, break, continue
 *   or return) and return a single flag to indicate
 *   the current state.
 *
 * CALL:
 *   flag = checkControlFlags
 *           (commands, num_commands, &ind);
 *
 * INPUT:
 *   commands     : Array of commands being processed by the current block command.
 *
 *   num_commands : Number of commands in the above array.
 *
 * OUTPUT:
 *   ind          : If an interrupt or goto to command inside the current
 *                : block has occured this is the index of the next
 *                : command to be processed.
 *
 * RETURNS:
 *   flag         : The control flow state :-
 *                :  LANGUAGE_EXIT
 *                :   'exit' command has been entered.
 *                :  LANGUAGE_GOTO_INSIDE
 *                :   'goto' command has been entered and points to a command inside
 *                :   the current block. The command number to goto is stored in 'ind'.
 *                :  LANGUAGE_GOTO_OUTSIDE
 *                :   'goto' command has been entered and points to a command outside
 *                :   the current block.
 *                :  LANGUAGE_BREAK
 *                :   'break' command has been entered.
 *                :  LANGUAGE_CONTINUE
 *                :   'continue' command has been entered.
 *                :  LANGUAGE_RETURN
 *                :   'return' command has been entered.
 *                :  LANGUAGE_INTERRUPT
 *                :   Interrupt has been signalled.
 *                :  LANGUAGE_ERROR
 *                :   Error has occured.
 *                :
 *                :  If none of the above are true 0 is returned.
 *
 * NOTES:
 *   The first two arguments should be those returned by commandListToArray().
 *
 *------------------------------------------------------------------*/

int
ClLanguageMgr::
checkControlFlags(ClLanguageCommand **commands, int num_commands, int *ind)
{
  if (getExitFlag())
    return LANGUAGE_EXIT;

  if (getGoto().getFlag()) {
    if (getDepth() == getGoto().getDepth()) {
      getGoto().setFlag(false);

      int ind1 = getLabelIndex(getGoto().getName(), commands, num_commands);

      if (ind1 != -1)
        *ind = ind1;

      return LANGUAGE_GOTO_INSIDE;
    }
    else
      return LANGUAGE_GOTO_OUTSIDE;
  }

  if (getInterruptFlag()) {
    setInterruptFlag(false);

    return LANGUAGE_INTERRUPT;
  }

  if (getBreakFlag()) {
    setBreakFlag(false);

    return LANGUAGE_BREAK;
  }

  if (getContinueFlag()) {
    setContinueFlag(false);

    return LANGUAGE_CONTINUE;
  }

  if (getReturnFlag())
    return LANGUAGE_RETURN;

  if (ClLanguageMgrInst->getErrorFlag())
    return LANGUAGE_ERROR;

  return 0;
}

/*------------------------------------------------------------------*
 *
 * checkInterrupt
 *   Check whether an interrupt has occurred.
 *
 * CALL:
 *   int flag =
 *     checkInterrupt(ClLanguageCommand **commands, int num_commands);
 *
 * INPUT:
 *   commands     : Array of commands being processed by the
 *                : current block command.
 *
 *   num_commands : Number of commands in the above array.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag          : The control flow state :-
 *                 :  LANGUAGE_INTERRUPT
 *                 :   Interrupt has been signalled.
 *
 * NOTES:
 *   Performed separately to checkControlFlags() in iteration
 *   commands as these may just be evaluating expressions and
 *   checkControlFlags() is only called after a command has been
 *   processed.
 *
 *------------------------------------------------------------------*/

int
ClLanguageMgr::
checkInterrupt(ClLanguageCommand **, int)
{
  if (checkAbort())
    return LANGUAGE_INTERRUPT;

  return 0;
}

/*------------------------------------------------------------------*
 *
 * checkAbort
 *   Return whether the user has requested abort.
 *
 *   The flag tested is set by the signal handler
 *   ClLanguageSignalHandler().
 *
 * CALL:
 *   bool flag = checkAbort();
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether abort has been requested.
 *
 * NOTES:
 *   The abort flag is reset after calling this function
 *   as the caller is expected to deal with the abort.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageMgr::
checkAbort()
{
  if (getAbortSignal()) {
    setAbortSignal(false);

    return true;
  }
  else
    return false;
}

/*------------------------------------------------------------------*
 *
 * getLabelIndex
 *   Search the specified array of commands for a label of the
 *   specified name.
 *
 * CALL:
 *   int ind =
 *     getLabelIndex
 *       (const string label, ClLanguageCommand **commands, int num_commands);
 *
 * INPUT:
 *   label         : The label name to search for.
 *
 *   commands      : The array of commands to search.
 *
 *   num_commands  : The number of commands in the above array.
 *
 * OUTPUT:
 *   ind           : The index into the array of commands where
 *                 : the label was found.
 *
 * RETURNS:
 *     None
 *
 * NOTES:
 *   This routine should always find the label. If it does
 *   not we have a fatal error so we exit the application.
 *
 *------------------------------------------------------------------*/

int
ClLanguageMgr::
getLabelIndex(const string &label, ClLanguageCommand **commands, int num_commands)
{
  int ind = -1;

  for (int i = 0; i < num_commands; i++) {
    if (! commands[i]->isLabel()) continue;

    ClLanguageLabelCommand *lcommand =
      (ClLanguageLabelCommand *) commands[i];

    ClLanguageLabelData *label_data = lcommand->getLabelData();

    if (label_data && label_data->isName(label)) {
      ind = i;
      break;
    }
  }

  if (ind == -1)
    ClLanguageMgrInst->error("unknown label '%s'", label.c_str());

  return ind;
}

void
ClLanguageMgr::
setArgv(int argc, char **argv)
{
  argc_ = argc;
  argv_ = new char * [argc_ + 1];

  for (int i = 0; i < argc_; ++i)
    argv_[i] = strdup(argv[i]);

  updateArgvVar();
}

void
ClLanguageMgr::
updateArgvVar()
{
  uint dims[1];

  dims[0] = argc_;

  ClParserInst->createStdVar("_argv",
    ClParserValueMgrInst->createValue(dims, 1, (const char **) argv_));
}

/*------------------------------------------------------------------*
 *
 * setOutputFp
 *   This routine allows the application to redirect the
 *   output to a file which has been opened for read or
 *   append.
 *
 * CALL:
 *   FILE *old_fp = setOutputFp(FILE *fp);
 *
 * INPUT:
 *   fp    : File pointer of the file to which all
 *         : CL output will be sent.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   old_fp : The file pointer previously being used
 *          : for CL output.
 *
 * NOTES:
 *   This routine should be called after init()
 *   but before any other CL routines which may produce
 *   output.
 *
 *------------------------------------------------------------------*/

FILE *
ClLanguageMgr::
setOutputFp(FILE *output_fp)
{
  return ClParserInst->setOutputFile(output_fp);
}

/*------------------------------------------------------------------*
 *
 * setOutputProc
 *   This routine allows the application to redirect the
 *   output to a procedure which can then display the output
 *   in its own application sepcific manner.
 *
 * CALL:
 *   ClParser::OutputProc old_proc =
 *     setOutputProc(ClLanguageOutputProc proc);
 *
 * INPUT:
 *   proc     : The procedure to which all CL output
 *            : will be sent.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   old_proc : The procedure previously
 *            : being used for CL output.
 *
 * NOTES:
 *   This routine should be called after init()
 *   but before any other CL routines which may produce
 *   output.
 *
 *------------------------------------------------------------------*/

ClParser::OutputProc
ClLanguageMgr::
setOutputProc(ClParser::OutputProc proc, void *data)
{
  return ClParserInst->setOutputProc(proc, data);
}

ClParser::OutputProc
ClLanguageMgr::
getOutputProc() const
{
  return ClParserInst->getOutputProc();
}

/*------------------------------------------------------------------*
 *
 * setHelpProc
 *   This routine allows the application to replace the
 *   standard Help Procedure (called when 'help' is entered
 *   at the Command Line) with an application defined one.
 *   this allows the application to display help for its
 *   addition CL defined routines as well as to display
 *   the standard help.
 *
 * CALL:
 *   ClLanguageHelpProc old_proc =
 *     setHelpProc(ClLanguageHelpProc proc);
 *
 * INPUT:
 *   proc     : The help procedure to call
 *            : when help is requested.
 *
 * OUTPUT:
 *     None
 *
 * RETURNS:
 *   old_proc : The help procedure previously being used.
 *
 * NOTES:
 *   This routine should be called after init()
 *   before any Language Commands are processed.
 *
 *   The application help procedure should call the standard
 *   CL help procedure ClLanguagePrintCommands() if
 *   the supplied subject is not recognised.
 *
 *------------------------------------------------------------------*/

/*------------------------------------------------------------------*
 *
 * setPrompt
 *   This routine allows the prompt str which appears
 *   at the start of each input line in the Command Line
 *   Interface to be altered.
 *
 * CALL:
 *   string old_prompt =
 *     setPrompt(const string &prompt);
 *
 * INPUT:
 *   prompt     : The new prompt str to use
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   old_prompt : The previous value of the prompt str.
 *
 * NOTES:
 *   This routine should be called after init()
 *   but before the Command Line Interface is started.
 *
 *------------------------------------------------------------------*/

/*------------------------------------------------------------------*
 *
 * getCommandName
 *   Get the name of the command currently being processed.
 *
 * CALL:
 *   const string &name = getCommandName();
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   name : The name of the command.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

/*------------------------------------------------------------------*
 *
 * getLastValue
 *   Get the last value evaluated by the language
 *   routines.
 *
 * CALL:
 *   ClParserValuePtr value = getLastValue();
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   value : Last value evaluated (nullptr if none).
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

/*------------------------------------------------------------------*
 *
 * resetLastValue
 *   Reset the last value (to nullptr).
 *
 * CALL:
 *   resetLastValue();
 *
 * INPUT:
 *   None
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

/*------------------------------------------------------------------*
 *
 * defineBlockCommand
 *   Define a Block Command which is started when a
 *   line is entered starting with the supplied name
 *   str and is terminated when a line is entered
 *   starting with the supplied end name.
 *
 *   When the Block Command has been terminated the
 *   specified routine is called with details of the
 *   initial line and list of commands between the
 *   initial and terminating line.
 *
 * CALL:
 *   int ident =
 *     defineBlockCommand
 *       (const string &name, const string &end_name, ClBlockType type,
 *        ClLanguageCmdProc function, char *dats);
 *
 * INPUT:
 *     name     : The block command name which when entered
 *              : will start the block.
 *
 *     end_name : The block command's end name which when
 *              : entered will end the block (if nullptr then
 *              : this is assumed to be the block command name
 *              : preceded by 'end' i.e. for a block command
 *              : name of 'block' the end name would default to
 *              : 'endblock').
 *
 *     type     : Type of block command :-
 *              :  CL_BLOCK_TYPE_NORMAL_BLOCK -
 *              :   Command List is executed once when the
 *              :   block command is entered.
 *              :  CL_BLOCK_TYPE_ITERATION_BLOCK -
 *              :   Command List is one or more times when the
 *              :   block command is entered.
 *              :  CL_BLOCK_TYPE_PROCEDURE_BLOCK -
 *              :   Command List is saved and executed later.
 *              :  CL_BLOCK_TYPE_FILE_BLOCK -
 *              :   Command List is executed once as if it was
 *              :   not part of the current execution environment.
 *
 *     function : The routine to be called when
 *              : the block has been terminated.
 *
 *     data     : The data passed to the called function.
 *
 * OUTPUT:
 *     None
 *
 * RETURNS:
 *     ident    : The unique ident of this block command.
 *
 * NOTES:
 *   The syntax of the block command is shown below :-
 *
 *     <name> <str>
 *       <command_list>
 *     <end_name>
 *
 *   The str specified after the name on the initial
 *   line is supplied untranslated and should be used
 *   to provide additional parameters to the block
 *   command.
 *
 *   The end name should be unique and should follow the
 *   existing style of being the block command name
 *   prefixed by 'end'.
 *
 *------------------------------------------------------------------*/

uint
ClLanguageMgr::
defineBlockCommand(const string &name, const string &end_name, ClBlockType type,
                   ClLanguageCmdProc function, void *data)
{
  ClLanguageCommandDef *command_def =
    new ClLanguageCommandDef(name, end_name, type, function, data);

  addCommandDef(command_def);

  return command_def->getIdent();
}

/*------------------------------------------------------------------*
 *
 * defineBlockCommand
 *   Define a new command identified by the supplied name
 *   str. When a command starting with the specified
 *   name is entered the supplied function is called.
 *
 * CALL:
 *   int ident =
 *     defineBlockCommand(const string &name, ClLanguageCmdProc function, void *data);
 *
 * INPUT:
 *   name     : The command name.
 *
 *   function : The function called after the command has been
 *            : entered. This is called with the command structure
 *            : as the first argument and 'data' as the second
 *            : argument.
 *
 *   data     : The data passed to the called function.
 *
 * OUTPUT:
 *   ident    : The unique ident of this command.
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   The syntax of the command is :-
 *
 *     <name> <str>
 *
 *   CL provides a large number of routines to help the
 *   application convert the str into the required
 *   arguments in a consistent manner. The application is
 *   not restricted to these routines and may parse the
 *   str itself if required.
 *
 *------------------------------------------------------------------*/

uint
ClLanguageMgr::
defineCommand(const string &name, ClLanguageCmdProc function, void *data)
{
  ClLanguageCommandDef *command_def =
    new ClLanguageCommandDef(name, function, data);

  addCommandDef(command_def);

  return command_def->getIdent();
}

uint
ClLanguageMgr::
defineCommand(ClParserScopePtr scope, const string &name, ClLanguageCmdProc function, void *data)
{
  ClLanguageCommandDef *command_def =
    new ClLanguageCommandDef(scope, name, function, data);

  addCommandDef(scope, command_def);

  return command_def->getIdent();
}

void
ClLanguageMgr::
addCommandDef(ClLanguageCommandDef *command_def)
{
  addCommandDef(ClParserScopePtr(), command_def);
}

void
ClLanguageMgr::
addCommandDef(ClParserScopePtr scope, ClLanguageCommandDef *command_def)
{
  bool is_end_name;

  ClLanguageCommandDef *old_command_def =
    getCommandDef(scope, command_def->getName(), &is_end_name);

  if (old_command_def != nullptr)
    removeCommandDef(scope, old_command_def);

  string scopeName;

  if (scope.isValid())
    scopeName = scope->getFullName() + "::";

  string key1 = scopeName + command_def->getName();

  command_def_list1_[key1] = command_def;

  if (command_def->getEndName() != "") {
    ClLanguageCommandDef *old_command_def =
      getCommandDef(scope, command_def->getEndName(), &is_end_name);

    if (old_command_def != nullptr)
      removeCommandDef(scope, old_command_def);

    string key2 = scopeName + command_def->getEndName();

    command_def_list2_[key2] = command_def;
  }
}

/*------------------------------------------------------------------*
 *
 * getCommandDef
 *   Get the command definition structure for the command
 *   with the specified name.
 *
 * CALL:
 *   ClLanguageCommandDef *command_def =
 *     getCommandDef(ClParserScopePtr scope, const string &name, bool &is_end_name);
 *
 * INPUT:
 *   name        : Name of the command for which the structure
 *               : is to be returned.
 *
 * OUTPUT:
 *   is_end_name : Flag to indicate whether the name is and
 *               : end name for a block command.
 *
 * RETURNS:
 *   command_def : The command definition structure (nullptr
 *               : if none found).
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

ClLanguageCommandDef *
ClLanguageMgr::
getCommandDef(ClParserScopePtr scope, const string &name, bool *is_end_name)
{
  *is_end_name = false;

  string scopeName;

  if (scope.isValid())
    scopeName = scope->getFullName() + "::";

  string key = scopeName + name;

  CommandDefMap::const_iterator p = command_def_list1_.find(key);

  if (p != command_def_list1_.end())
    return (*p).second;

  p = command_def_list2_.find(key);

  if (p != command_def_list2_.end()) {
    *is_end_name = true;

    return (*p).second;
  }

  return nullptr;
}

void
ClLanguageMgr::
deleteCommandDefs()
{
  CommandDefMap::const_iterator p1 = command_def_list1_.begin();
  CommandDefMap::const_iterator p2 = command_def_list1_.end  ();

  for ( ; p1 != p2; ++p1)
    delete (*p1).second;

  command_def_list1_.clear();
  command_def_list2_.clear();
}

void
ClLanguageMgr::
removeCommandDef(ClParserScopePtr scope, ClLanguageCommandDef *command_def)
{
  assert(command_def != nullptr);

  string scopeName;

  if (scope.isValid())
    scopeName = scope->getFullName() + "::";

  string key1 = scopeName + command_def->getName();

  command_def_list1_.erase(key1);

  if (command_def->getEndName() != "") {
    string key2 = scopeName + command_def->getEndName();

    command_def_list2_.erase(key2);
  }

  delete command_def;
}

ClLanguageCommand *
ClLanguageMgr::
startBlockCommand(ClLanguageCommand *command)
{
  if (! command->isEndBlock()) {
    incDepth();

    if (block_command_ != nullptr)
      block_command_stack_.push_back(block_command_);

    block_command_ = command;

    return nullptr;
  }
  else {
    if      (block_command_ == nullptr) {
      ClLanguageCommandDefCommand *ccommand =
        (ClLanguageCommandDefCommand *) command;

      ClLanguageCommandDef *command_def = ccommand->getCommandDef();

      ClLanguageMgrInst->syntaxError("'%s' without corresponding '%s'",
                                      command_def->getEndName().c_str(),
                                      command_def->getName   ().c_str());

      delete command;

      return nullptr;
    }
    else if (! command->isIdentNoEnd(block_command_)) {
      ClLanguageCommandDefCommand *ccommand =
        (ClLanguageCommandDefCommand *) command;
      ClLanguageCommandDefCommand *ccommand1 =
        (ClLanguageCommandDefCommand *) block_command_;

      ClLanguageCommandDef *command_def  = ccommand ->getCommandDef();
      ClLanguageCommandDef *command_def1 = ccommand1->getCommandDef();

      ClLanguageMgrInst->syntaxError("'%s' when in '%s' .. '%s' block",
                                      command_def ->getEndName().c_str(),
                                      command_def1->getName   ().c_str(),
                                      command_def1->getEndName().c_str());

      delete command;

      return nullptr;
    }
    else {
      delete command;

      return endBlockCommand();
    }
  }
}

ClLanguageCommand *
ClLanguageMgr::
endBlockCommand()
{
  decDepth();

  ClLanguageCommand *command = block_command_;

  if (! block_command_stack_.empty()) {
    block_command_ = block_command_stack_.back();

    block_command_stack_.pop_back();
  }
  else
    block_command_ = nullptr;

  if (block_command_ != nullptr && command != nullptr) {
    block_command_->addCommand(command);

    return nullptr;
  }

  return command;
}

void
ClLanguageMgr::
clearBlockCommandStack(bool report_error)
{
  if (block_command_ != nullptr) {
    if (report_error) {
      string command_name = block_command_->toName();

      ClLanguageMgrInst->syntaxError
        ("unterminated '%s' block", command_name.c_str());
    }

    delete block_command_;

    block_command_ = nullptr;
  }

  int num = block_command_stack_.size();

  while (num--) {
    ClLanguageCommand *command = block_command_stack_.back();

    block_command_stack_.pop_back();

    if (report_error) {
      string command_name = command->toName();

      ClLanguageMgrInst->syntaxError
        ("unterminated '%s' block", command_name.c_str());
    }

    delete command;
  }

  block_command_stack_.clear();

  resetDepth();
}

/*------------------------------------------------------------------*
 *
 * removeCommandLabels
 *   Remove any labels in the command list from the master
 *   label list so they will not be deleted after the
 *   specified command list has been executed.
 *
 * CALL:
 *   removeCommandLabels(ClLanguageCommand **commands, int num_commands);
 *
 * INPUT:
 *   commands     : Array of commands which may contain label commands.
 *
 *   num_commands : The number of commands in the above array.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   This is done for procedures and functions where the
 *   command list needs to be retained so it can be run
 *   again.
 *
 *   The label commands have to be added before each run of
 *   the command list using addCommandLabels.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
removeCommandLabels(ClLanguageCommand **commands, int num_commands)
{
  for (int i = 0; i < num_commands; i++) {
    if (! commands[i]->isLabel()) continue;

    ClLanguageLabelCommand *lcommand = (ClLanguageLabelCommand *) commands[i];

    ClLanguageLabelData *label_data = lcommand->getLabelData();

    if (label_data != nullptr)
      ClLanguageLabelDataMgrInst->removeCurrentLabelData(label_data);
  }
}

/*------------------------------------------------------------------*
 *
 * addCommandLabels
 *   Add any labels in the command list to the master
 *   label list so they will found when the command
 *   list is run.
 *
 * CALL:
 *   addCommandLabels(ClLanguageCommand **commands, int num_commands);
 *
 * INPUT:
 *   commands     : Array of commands which may contain label commands.
 *
 *   num_commands : The number of commands in the above array.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   This is done for procedures and functions where the command list
 *   needs to be retained so it can be run again.
 *
 *   The label commands must have already have been removed
 *   using removeCommandLabels.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
addCommandLabels(ClLanguageCommand **commands, int num_commands)
{
  for (int i = 0; i < num_commands; i++) {
    if (! commands[i]->isLabel()) continue;

    ClLanguageLabelCommand *lcommand =
      (ClLanguageLabelCommand *) commands[i];

    ClLanguageLabelData *label_data = lcommand->getLabelData();

    if (label_data != nullptr)
      ClLanguageLabelDataMgrInst->addCurrentLabelData(label_data);
  }
}

/*------------------------------------------------------------------*
 *
 * commandListToArray
 *   Convert a Command List to an Array of Commands.
 *
 * CALL:
 *   commandListToArray
 *    (const ClLanguageCommandList &command_list,
 *     ClLanguageCommand &(**commands), int &num_commands);
 *
 * INPUT:
 *   command_list : The Command List to be converted.
 *
 * OUTPUT:
 *   commands     : The array of commands to search.
 *
 *   num_commands : The number of commands in the above array.
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
commandListToArray(const ClLanguageCommandList &command_list,
                   ClLanguageCommand ***commands, int *num_commands)
{
  *num_commands = command_list.size();

  if (*num_commands > 0) {
    *commands = new ClLanguageCommand * [*num_commands];

    for (int i = 0; i < *num_commands; i++)
      (*commands)[i] = command_list[i];
  }
  else
    *commands = nullptr;
}

/*------------------------------------------------------------------*
 *
 * printCommands
 *   This routine provides the standard help for the language
 *   commands.
 *
 *   This routine should be called by an application supplied
 *   help routine when an unrecognised subject is entered.
 *
 * CALL:
 *   printCommands(const string &subject);
 *
 * INPUT:
 *   subject : The subject on which the user wishes
 *           : help to be displayed.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   The subject is currently ignored but may be used in
 *   future to give a more comprehensive help.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
printCommands(const string &subject)
{
  string args = "";

  /* Display Module Help (if any) */

  if (ClModuleMgrInst->execModuleHelp(subject, args))
    return;

  /* Print Out Help using current Help Procedure */

  ClLanguageHelpProc help_proc = getHelpProc();

  if (help_proc != nullptr)
    (*help_proc)(subject.c_str(), args.c_str());
  else
    ClLanguageMgrInst->error("No help for '%s'", subject.c_str());
}

/*------------------------------------------------------------------*
 *
 * outputPrompt
 *   Output the current prompt str to the current
 *   output device.
 *
 *   This routine can be used by an application which
 *   provides its own user interface.
 *
 * CALL:
 *   outputPrompt();
 *
 * INPUT:
 *     None
 *
 * OUTPUT:
 *     None
 *
 * RETURNS:
 *     None
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
outputPrompt()
{
  if (prompt_ == "")
    return;

  for (int i = 0; i < getDepth(); i++)
    output("+");

  output("%s ", prompt_.c_str());
}

/*------------------------------------------------------------------*
 *
 * output
 *   Print the specified formatted str to the current
 *   output device.
 *
 * CALL:
 *   output(format, ...);
 *
 * INPUT:
 *   format  : 'printf' style format str which will be
 *    char * : used to build the str.
 *
 *   ...     : Optional arguments required by embedded
 *           : format codes in the format str.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   The str will be sent to the current output procedure
 *   (if defined) or the current output file.
 *
 *------------------------------------------------------------------*/

void
ClLanguageMgr::
output(const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  voutput(format, &vargs);

  va_end(vargs);
}

void
ClLanguageMgr::
voutput(const char *format, va_list *vargs)
{
  ClParser::OutputProc output_proc = ClParserInst->getOutputProc();

  if (output_proc != nullptr) {
    string str;

    CStrUtil::vsprintf(str, format, vargs);

    (*output_proc)(str.c_str(), ClParserInst->getOutputData());
  }
  else {
    FILE *fp = ClParserInst->getOutputFile();

    vfprintf(fp, format, *vargs);

    fflush(fp);
  }
}

void
ClLanguageMgr::
error(const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  errorMgr_->error(format, &vargs);

  va_end(vargs);
}

void
ClLanguageMgr::
expressionError(int error_code, const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  errorMgr_->expressionError(error_code, format, &vargs);

  va_end(vargs);
}

void
ClLanguageMgr::
syntaxError(const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  errorMgr_->syntaxError(format, &vargs);

  va_end(vargs);
}

//-----------------------

ClLanguageCommandDef::
ClLanguageCommandDef(const string &name, const string &end_name, ClBlockType type,
                     ClLanguageCmdProc function, void *data) :
 name_(name), end_name_(end_name), type_(type), function_(function), data_(data)
{
  if (end_name_ == "")
    end_name_ = "end" + name_;

  ident_ = ClLanguageMgrInst->nextIdent();
}

ClLanguageCommandDef::
ClLanguageCommandDef(const string &name, ClLanguageCmdProc function, void *data) :
 name_(name), end_name_(""), type_(CL_BLOCK_TYPE_NORMAL_BLOCK), function_(function), data_(data)
{
  ident_ = ClLanguageMgrInst->nextIdent();
}

ClLanguageCommandDef::
ClLanguageCommandDef(ClParserScopePtr scope, const string &name,
                     ClLanguageCmdProc function, void *data) :
 name_(name), end_name_(""), scope_(scope), type_(CL_BLOCK_TYPE_NORMAL_BLOCK),
 function_(function), data_(data)
{
  ident_ = ClLanguageMgrInst->nextIdent();
}

void
ClLanguageCommandDef::
runFunction(ClLanguageCommand *command)
{
  ClLanguageArgs *args = new ClLanguageArgs;

  if (! (type_ & CL_BLOCK_TYPE_NO_ARGS_BLOCK))
    args->startArgs(command);

  (*function_)(command, args, data_);

  if (! (type_ & CL_BLOCK_TYPE_NO_ARGS_BLOCK))
    args->termArgs();

  delete args;
}

//-----------------------

ClLanguageInputFile::
ClLanguageInputFile()
{
  fp_ = stdin;
}

bool
ClLanguageInputFile::
setName(const string &name)
{
  FILE *fp = fopen(name.c_str(), "r");

  if (fp == nullptr) {
    ClLanguageMgrInst->error("failed to read file '%s'", name.c_str());
    return false;
  }

  setFp(fp);

  name_ = name;

  return true;
}

void
ClLanguageInputFile::
setFp(FILE *fp)
{
  if (fp_ != fp) {
    if (fp_ != stdin)
      fclose(fp_);

    fp_ = fp;
  }

  name_     = "";
  line_num_ = 0;
}

bool
ClLanguageInputFile::
readLine(string &line)
{
  if (! fp_)
    fp_ = stdin;

  CFile file(fp_);

  return file.readLine(line);
}
