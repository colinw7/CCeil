#include <CCeilLStdCmd.h>
#include <CCeilLI.h>
#include <COSProcess.h>
#include <COSRand.h>
#include <CFile.h>
#include <CEnv.h>
#include <CUtil.h>

#include <csignal>
#include <algorithm>

#define LOUT(a) ClLanguageMgrInst->output(a)

typedef std::vector<ClLanguageProcArg *> ClLanguageProcArgArray;
typedef std::vector<ClLanguageFuncArg *> ClLanguageFuncArgArray;

static bool             ClStdCommandsInit
                         (ClModuleInitType init_type, void *);
static bool             ClStdCommandsSet
                         (const char *options, const char *args, void *);
static bool             ClStdCommandsHelp
                         (const char *args, void *);
static void             ClStdNewCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdRunCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdImportCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdProcCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdFuncCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdForCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdForEachCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdWhileCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdUntilCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdRepeatCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdIfCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdElseIfCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdElseCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdGotoCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdBreakCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdContinueCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdReturnCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdOnCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdRaiseCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdDefineCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdStructCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdInputCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdPrintCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static ClParserValuePtr ClStdFormatCommand
                         (ClParserValuePtr *, uint, void *, int *);
static ClParserValuePtr ClStdScanCommand
                         (ClParserValuePtr *, uint, void *, int *);
static ClParserValuePtr ClStdExecCommand
                         (ClParserValuePtr *, uint, void *, int *);
static ClParserValuePtr ClStdParseCommand
                         (ClParserValuePtr *, uint, void *, int *);
static void             ClStdSetCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdGetEnvCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdSetEnvCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdUnSetEnvCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdConfigCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static ClParserValuePtr ClStdRandCommand
                         (ClParserValuePtr *, uint, void *, int *);
static ClParserValuePtr ClStdRRandCommand
                         (ClParserValuePtr *, uint, void *, int *);
static ClParserValuePtr ClStdIsVarCommand
                         (ClParserValuePtr *, uint, void *, int *);
static void             ClStdUndefCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdWhatisCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdStatusCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdHelpCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdExitCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdDieCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
#ifdef CEIL_READLINE
static void            ClStdHistoryCommand
                        (ClLanguageCommand *, ClLanguageArgs *, void *);
#endif

static ClModuleDef
cl_builtin_module_def[] = {
  { "init", (ClModuleProc) ClStdCommandsInit, },
  { "set" , (ClModuleProc) ClStdCommandsSet , },
  { "help", (ClModuleProc) ClStdCommandsHelp, },
  { ""    , nullptr                         , },
};

static int language_import_ident = -1;
static int language_retall_ident = -1;
static int language_elseif_ident = -1;
static int language_else_ident   = -1;

//---------------------------

static ClParserValuePtr
ClLanguageRunFunction(ClParserValuePtr *values, uint num_values, void *data, int *error_code)
{
  return ClLanguageMgrInst->runFunction(values, num_values, data, error_code);
}

/*------------------------------------------------------------------*
 *
 * ClStdCommandsAdd
 *   Add the Builtin Module.
 *
 * CALL:
 *   ClStdCommandsAdd();
 *
 * NOTES:
 *   Commands must be imported before they can be used.
 *
 *------------------------------------------------------------------*/

void
ClStdCommandsAdd()
{
  ClModuleMgrInst->defineModule("builtin", cl_builtin_module_def);
}

/*------------------------------------------------------------------*
 *
 * ClStdCommandsInit
 *   Initialise/Re-Initialise the Builtin Module.
 *
 * CALL:
 *   bool flag1 = ClStdCommandsInit(ClModuleInitType flag);
 *
 * INPUT:
 *   flag : Initialisation Type :-
 *        :   CL_MODULE_INIT_TYPE_INIT -
 *        :    First Time Initialisation, Commands and Variables are defined.
 *        :   CL_MODULE_INIT_TYPE_REINIT -
 *        :    Reinitialisation, Variables are redefined.
 *
 * RETURNS:
 *   flag1 : Whether the Initialisation worked or not.
 *
 * NOTES:
 *   Called as the Builtin Module's 'init' method.
 *
 *------------------------------------------------------------------*/

static bool
ClStdCommandsInit(ClModuleInitType flag, void *)
{
  int arg_types[10];
  int num_arg_types;

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  if (flag != CL_MODULE_INIT_TYPE_REINIT) {
    language_import_ident = lmgr->defineCommand("import", ClStdImportCommand);

    //---

    lmgr->defineCommand("new", ClStdNewCommand);
    lmgr->defineCommand("run", ClStdRunCommand);

    //---

    /* Procedures and Functions */

    lmgr->defineBlockCommand("proc", "", CL_BLOCK_TYPE_PROCEDURE_BLOCK, ClStdProcCommand);
    lmgr->defineBlockCommand("func", "", CL_BLOCK_TYPE_PROCEDURE_BLOCK, ClStdFuncCommand);

    //---

    /* Program Flow/Iteration Statements */

    lmgr->defineBlockCommand("for"    , "", CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK,
                             ClStdForCommand);
    lmgr->defineBlockCommand("foreach", "", CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK,
                             ClStdForEachCommand);

    lmgr->defineBlockCommand("while", "", CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK,
                             ClStdWhileCommand);
    lmgr->defineBlockCommand("until", "", CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK,
                             ClStdUntilCommand);
    lmgr->defineBlockCommand("repeat", "", CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK,
                             ClStdRepeatCommand);

    lmgr->defineBlockCommand("if", "", CL_BLOCK_TYPE_NORMAL_NO_ARGS_BLOCK, ClStdIfCommand);

    language_elseif_ident = lmgr->defineCommand("elseif", ClStdElseIfCommand);
    language_else_ident   = lmgr->defineCommand("else"  , ClStdElseCommand  );

    //---

    /* Program Flow Statements */

    lmgr->defineCommand("goto"    , ClStdGotoCommand    );
    lmgr->defineCommand("break"   , ClStdBreakCommand   );
    lmgr->defineCommand("continue", ClStdContinueCommand);
    lmgr->defineCommand("return"  , ClStdReturnCommand  );

    //---

    /* Signal Handling Statements */

    lmgr->defineCommand("on"   , ClStdOnCommand   );
    lmgr->defineCommand("raise", ClStdRaiseCommand);

    //---

    /* Definition Statements */

    lmgr->defineCommand("define", ClStdDefineCommand);
    lmgr->defineCommand("struct", ClStdStructCommand);

    //---

    /* Input/Output Statements */

    lmgr->defineCommand("input", ClStdInputCommand);
    lmgr->defineCommand("print", ClStdPrintCommand);

    //---

    /* Format Function and Scan Command */

    num_arg_types = 2;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;
    arg_types[1] = CL_PARSER_VALUE_TYPE_INTEGER |
                   CL_PARSER_VALUE_TYPE_REAL |
                   CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("format", CL_PARSER_USERFN_TYPE_VARARGS,
                               arg_types, num_arg_types, ClStdFormatCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      lmgr->expressionError(ClParserUserFn::getErrorCode(), "creating 'format' user function");
      return false;
    }

    num_arg_types = 3;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;
    arg_types[1] = CL_PARSER_VALUE_TYPE_STRING;
    arg_types[2] = CL_PARSER_VALUE_TYPE_INTEGER | CL_PARSER_VALUE_TYPE_REAL |
                   CL_PARSER_VALUE_TYPE_STRING  | CL_PARSER_VALUE_TYPE_OUTPUT;

    ClParserInst->createUserFn("scan", CL_PARSER_USERFN_TYPE_VARARGS,
                               arg_types, num_arg_types, ClStdScanCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      lmgr->expressionError(ClParserUserFn::getErrorCode(), "creating 'scan' user function");
      return false;
    }

    //---

    /* System Access Command */

    num_arg_types = 1;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("exec", CL_PARSER_USERFN_TYPE_STD,
                               arg_types, num_arg_types, ClStdExecCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      lmgr->expressionError(ClParserUserFn::getErrorCode(), "creating 'exec' user function");
      return false;
    }

    //---

    /* Execute Expression String */

    num_arg_types = 1;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("parse", CL_PARSER_USERFN_TYPE_STD,
                               arg_types, num_arg_types, ClStdParseCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      lmgr->expressionError(ClParserUserFn::getErrorCode(), "creating 'parse' user function");
      return false;
    }

    //---

    /* Variables */

    if (ClParserInst->getDollarPrefix())
      lmgr->defineCommand("set", ClStdSetCommand);

    //---

    /* Environment Variables */

    lmgr->defineCommand("getenv"  , ClStdGetEnvCommand  );
    lmgr->defineCommand("setenv"  , ClStdSetEnvCommand  );
    lmgr->defineCommand("unsetenv", ClStdUnSetEnvCommand);

    //---

    /* Random Numbers  */

    num_arg_types = 0;

    ClParserInst->createUserFn("rand", CL_PARSER_USERFN_TYPE_VARARGS,
                               arg_types, num_arg_types, ClStdRandCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      lmgr->expressionError(ClParserUserFn::getErrorCode(), "creating 'rand' user function");
      return false;
    }

    num_arg_types = 0;

    ClParserInst->createUserFn("rrand", CL_PARSER_USERFN_TYPE_VARARGS,
                               arg_types, num_arg_types, ClStdRRandCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      lmgr->expressionError(ClParserUserFn::getErrorCode(), "creating 'rrand' user function");
      return false;
    }

    //---

    /* Set */

    if (ClParserInst->getDollarPrefix())
      lmgr->defineCommand("config", ClStdConfigCommand);
    else
      lmgr->defineCommand("set", ClStdConfigCommand);

    //---

    /* Control Commands */

    num_arg_types = 1;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("isvar", CL_PARSER_USERFN_TYPE_STD,
                               arg_types, num_arg_types, ClStdIsVarCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      lmgr->expressionError(ClParserUserFn::getErrorCode(), "creating 'isvar' user function");
      return false;
    }

    lmgr->defineCommand("undef" , ClStdUndefCommand );
    lmgr->defineCommand("whatis", ClStdWhatisCommand);

    //---

    /* Enquiry Commands */

    lmgr->defineCommand("help"  , ClStdHelpCommand);
    lmgr->defineCommand("status", ClStdStatusCommand);

    //---

    language_retall_ident = lmgr->defineCommand("retall", (ClLanguageCmdProc) 0);

    //---

    lmgr->defineCommand("exit", ClStdExitCommand);

    lmgr->defineCommand("die", ClStdDieCommand);

    //---

  #ifdef CEIL_READLINE
    lmgr->defineCommand("history", ClStdHistoryCommand);
  #endif
  }

  return true;
}

/*------------------------------------------------------------------*
 *
 * ClStdCommandsSet
 *   The 'set' method for the 'builtin' module.
 *
 *   This routine allows the environment to be customised using the specified
 *   option with any arguments this requries :-
 *
 *   Options
 *   -------
 *
 *   angle_type <radians|degrees> : Set Angle Type to Degrees or Radians
 *   prompt <string>              : Set Prompt String
 *   real_format <string>         : Set Real Format String
 *   integer_format <string>      : Set Integer Format String
 *   string_format <string>       : Set String Format String
 *   math_fail <yes|no>           : Set whether a math failure causes an error or a NaN value.
 *   debug <level>                : Set Debug Level
 *
 * CALL:
 *   bool flag = ClStdCommandsSet(const char *option, const char *args);
 *
 * INPUT:
 *   option : Option name
 *
 *   args   : String containing additional args.
 *
 * RETURNS:
 *   flag   : Success flag
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static bool
ClStdCommandsSet(const char *option, const char *args, void *)
{
  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  if (option[0] == '\0') {
    CAngleType  angle_type = ClParserInst->getAngleType();
    std::string prompt     = lmgr->getPrompt();
    bool        math_fail  = ClParserInst->getMathFail();
    double      tolerance  = ClParserInst->getTolerance();

    lmgr->output("angle_type     %s\n"  , CAngleTypeUtil::toString(angle_type).c_str());
    lmgr->output("prompt         '%s'\n", prompt.c_str());
    lmgr->output("real_format    '%s'\n", ClParserInst->getRealFormat().c_str());
    lmgr->output("integer_format '%s'\n", ClParserInst->getIntegerFormat().c_str());
    lmgr->output("string_format  '%s'\n", ClParserInst->getStringFormat().c_str());
    lmgr->output("math_fail      %s\n"  , CStrUtil::toString(math_fail).c_str());
    lmgr->output("tolerance      %s\n"  , CStrUtil::toString(tolerance).c_str());

    return true;
  }

  if      (CStrUtil::casecmp(option, "angle_type") == 0) {
    /* Set Angle Type degrees/radians */

    auto angle_type = CAngleTypeUtil::fromString(args);

    if (angle_type != CANGLE_TYPE_NONE)
      ClParserInst->setAngleType(angle_type);
    else
      lmgr->error("invalid angle type '%s' for 'set %s'", args, option);
  }
  else if (CStrUtil::casecmp(option, "prompt") == 0) {
    char *prompt;

    auto *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args, CLArgType::STRING, &prompt, CLArgType::NONE) != 1)
      return false;

    /* Set Prompt */

    lmgr->setPrompt(prompt);

    args1->termArgs();

    delete args1;
  }
  else if (CStrUtil::casecmp(option, "real_format") == 0) {
    char*       format;
    std::string real_format;

    bool rc = false;

    auto *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args, CLArgType::STRING, &format, CLArgType::NONE) == 1) {
      CStrParse parse(format);

      if (parse.readRealFormat(real_format)) {
        parse.skipSpace();

        rc = parse.eof();
      }
    }

    if (rc)
      ClParserInst->setRealFormat(real_format);
    else
      lmgr->error("invalid format '%s' for 'set %s'", format, option);

    args1->termArgs();

    delete args1;

    return rc;
  }
  else if (CStrUtil::casecmp(option, "integer_format") == 0) {
    char*       format;
    std::string integer_format;

    bool rc = false;

    auto *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args, CLArgType::STRING, &format, CLArgType::NONE) == 1) {
      CStrParse parse(format);

      if (parse.readIntegerFormat(integer_format)) {
        parse.skipSpace();

        rc = parse.eof();
      }
    }

    if (rc)
      ClParserInst->setIntegerFormat(integer_format);
    else
      lmgr->error("invalid format '%s' for 'set %s'", format, option);

    args1->termArgs();

    delete args1;

    return rc;
  }
  else if (CStrUtil::casecmp(option, "string_format") == 0) {
    char*       format;
    std::string string_format;

    bool rc = false;

    auto *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args, CLArgType::STRING, &format, CLArgType::NONE) == 1) {
      CStrParse parse(format);

      if (parse.readStringFormat(string_format)) {
        parse.skipSpace();

        rc = parse.eof();
      }
    }

    if (rc)
      ClParserInst->setStringFormat(string_format);
    else
      lmgr->error("invalid format '%s' for 'set %s'", format, option);

    args1->termArgs();

    return rc;
  }
  else if (CStrUtil::casecmp(option, "math_fail") == 0) {
    /* Set Math Fail True/False */

    if (CStrUtil::isBool(args))
      ClParserInst->setMathFail(CStrUtil::toBool(args));
    else
      lmgr->error("invalid args '%s' for 'set %s'", args, option);
  }
  else if (CStrUtil::casecmp(option, "tolerance") == 0) {
    /* Set tolerance to real value */

    double r = ClParserInst->getTolerance();

    if (! CStrUtil::toReal(args, &r)) {
      lmgr->error("invalid tolerance '%s' for 'set %s'", args, option);
      return false;
    }

    ClParserInst->setTolerance(r);
  }
  else if (CStrUtil::casecmp(option, "debug") == 0) {
    int level;

    /* Get Debug Level */

    if (! CStrUtil::toInteger(args, &level)) {
      lmgr->error("invalid debug level '%s' for 'set %s'", args, option);
      return false;
    }

    /* Set Language Debug (Level >= 1) */

    if (level >= 1)
      ClLanguageDebugInst->setDebug(true);
    else
      ClLanguageDebugInst->setDebug(false);

    /* Set Parser Debug (Level >= 2) */

    if (level >= 2)
      ClParserInst->setDebug(true);
    else
      ClParserInst->setDebug(false);
  }
  else
    return false;

  /*-------------------*/

  return true;
}

/*------------------------------------------------------------------*
 *
 * ClStdCommandsHelp
 *   Provide Help for the Builtin Module.
 *
 * CALL:
 *   bool flag = ClStdCommandsHelp(const char *args);
 *
 * INPUT:
 *   args : Additional Arguments passed to the Help Command.
 *
 * RETURNS:
 *   flag : Whether the Help was successful or not.
 *
 * NOTES:
 *   Called as the Builtin Module's 'help' method.
 *
 *------------------------------------------------------------------*/

static bool
ClStdCommandsHelp(const char *, void *)
{
  LOUT("\n");
  LOUT("Commands\n");
  LOUT("--------\n");
  LOUT("\n");
  LOUT("# <comment>\n");
  LOUT("! <System Command>\n");
  LOUT("<label>:\n");
  LOUT("\n");
  LOUT("import <module>\n");
  LOUT("\n");
  LOUT("new\n");
  LOUT("run <file>,...\n");
  LOUT("\n");
  LOUT("proc <name>(<arg>,...)\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("endproc\n");
  LOUT("\n");
  LOUT("func <name>(<type> <arg>, ...)\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("  _return = <return_value>\n");
  LOUT("endfunc\n");
  LOUT("\n");
  LOUT("for (<expression>,<expression>,<expression>)\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("endfor\n");
  LOUT("\n");
  LOUT("foreach <var_name> (<object>)\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("endforeach\n");
  LOUT("\n");
  LOUT("while (<expression>)\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("endwhile\n");
  LOUT("\n");
  LOUT("until <expression>\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("enduntil\n");
  LOUT("\n");
  LOUT("repeat <count>\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("endrepeat\n");
  LOUT("\n");
  LOUT("if (<expression>)\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("elseif (<expression>)\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("else\n");
  LOUT("  <statement>\n");
  LOUT("  ...\n");
  LOUT("endif\n");
  LOUT("\n");
  LOUT("goto <label>\n");
  LOUT("break\n");
  LOUT("continue\n");
  LOUT("return\n");
  LOUT("\n");
  LOUT("on <event> <command>\n");
  LOUT("raise <event> [<data>]\n");
  LOUT("\n");
  LOUT("define <name>(<arg>,...>) = <expression>\n");
  LOUT("struct <name> { <type> <name>, ... }\n");
  LOUT("\n");
  LOUT("input <str>,<variable>\n");
  LOUT("print <expression>,...[,]\n");
  LOUT("<str> = format(<format>,<arg>,...)\n");
  LOUT("<num> = scan(<str>,<format>,<arg>,...)\n");
  LOUT("\n");
  LOUT("<str> = exec(<command>)\n");
  LOUT("<value> = parse(<str>)\n");
  LOUT("\n");
  LOUT("getenv <value>,<variable>\n");
  LOUT("setenv <variable>,<value>\n");
  LOUT("unsetenv <variable>\n");
  LOUT("\n");
  LOUT("set <option> ...\n");
  LOUT("\n");
  LOUT("<num> = rand([<seed>])\n");
  LOUT("<num> = rrand([<seed>])\n");
  LOUT("\n");
  LOUT("<flag> = isvar(<name>)\n");
  LOUT("undef <identifier>,...\n");
  LOUT("whatis <expression>,...\n");
  LOUT("\n");
  LOUT("status [<object>]\n");
  LOUT("\n");
  LOUT("help [<subject>]\n");
  LOUT("\n");
  LOUT("retall\n");
  LOUT("\n");
  LOUT("exit\n");
  LOUT("die\n");
  LOUT("\n");

  return true;
}

/*------------------------------------------------------------------*
 *
 * ClStdImportCommand
 *   Routine called when the 'import' command is entered.
 *
 *   The routine imports the commands and variables for the named module or
 *   lists the available modules and their status if no module is defined.
 *
 * CALL:
 *   ClStdImportCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments
 *           : supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdImportCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  if (args->checkNumberOfArgs(0, 1) != 0)
    return;

  uint num = args->getNumArgs();

  if (num == 1) {
    char *name;

    if (args->getArgList(CLArgType::TEXT, &name, CLArgType::NONE) != 1)
      return;

    bool flag = ClModuleMgrInst->importModule(name);

    if (! flag)
      lmgr->error("Failed to Import Module '%s'", name);
  }
  else {
    StringVectorT modules;

    ClModuleMgrInst->getModuleList(modules);

    int num_modules = modules.size();

    for (int i = 0; i < num_modules; i++) {
      lmgr->output("%s", modules[i].c_str());

      if (ClModuleMgrInst->isModuleLoaded(modules[i]))
        lmgr->output(" (loaded)");

      lmgr->output("\n");
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdNewCommand
 *   Routine called when the 'new' command is entered.
 *
 *   The routine ensures no arguments are entered and then deletes all
 *   variables, structures, procedures and functions. The standard variables,
 *   structures, procedures and functions are then re-added.
 *
 * CALL:
 *   ClStdNewCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments
 *           : supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdNewCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdNewCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Reinitialise */

  lmgr->reinit();
}

/*------------------------------------------------------------------*
 *
 * ClStdRunCommand
 *   Routine called when the 'run' command is entered.
 *
 *   The routine ensures gets the comma separated list of strings after the
 *   command and treats each as the name of a file whose contents are language
 *   commands to be run.
 *
 *   Each file is run in turn. If a specified file does not exist then it is ignored.
 *
 * CALL:
 *   ClStdRunCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments supplied
 *           : by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdRunCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdRunCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Get Run's Arguments */

  if (args->checkNumberOfArgs(1, -1) != 0)
    return;

  uint num_args = args->getNumArgs();

  /* Get the list of File Names to Run */
  StringVectorT file_names;

  for (uint i = 1; i <= num_args; i++) {
    int error_code;

    std::string file_name = args->getStringArg(i, &error_code);

    if (error_code != 0)
      return;

    file_names.push_back(file_name);
  }

  /* Run the file specified by each argument */

  for (uint i = 0; i < num_args; i++) {
    if (! CFile::isRegular(file_names[i])) {
      lmgr->error("'%s' is not a regular file", file_names[i].c_str());
      continue;
    }

    lmgr->runFile(file_names[i]);
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdProcCommand
 *   Routine called when a 'proc ... endproc' block command
 *   is entered.
 *
 *   The name following the 'proc' is used as the name of the procedure and the
 *   subsequent round bracketed comma separated list of argument names is
 *   extracted and those starting with the '&' character are flagged as return
 *   variables. The list of language commands enclosed by the 'proc ... endproc'
 *   pair are stored for later execution when the procedure name is specified as
 *   a command.
 *
 * CALL:
 *   ClStdProcCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments supplied
 *           : by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdProcCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  class ProcArgArray {
   public:
    ProcArgArray() { }

   ~ProcArgArray() {
     if (isAutoFree()) {
       for (auto &arg : args_)
         delete arg;
     }
    }

    ClLanguageProcArgArray &args() { return args_; }

    void push_back(ClLanguageProcArg *arg) {
      args_.push_back(arg);
    }

    bool isAutoFree() const { return autoFree_; }
    void setAutoFree(bool b) { autoFree_ = b; }

   private:
    ClLanguageProcArgArray args_;
    bool                   autoFree_ { true };
  };

  //---

  CL_LANGUAGE_TRACE("ClStdProcCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  const ClLanguageCommandList &command_list = command->getCommandList();

  /* Extract Procedure Name */

  CStrParse parse(cargs);

  std::string name_string;

  if (! parse.readIdentifier(name_string)) {
    lmgr->syntaxError("'proc' procedure name must start with alphabetic or '_'");
    return;
  }

  /* Get Existing Procedure and Function */

  ClLanguageProc* old_procedure = ClLanguageProcMgrInst->lookupProc(name_string);
  ClLanguageFunc* old_lfunction = ClLanguageFuncMgrInst->lookupFunc(name_string);
  ClParserFuncPtr old_pfunction = ClParserInst->getFunction(name_string);

  /* Ensure Procedure Can be Deleted */

  if (old_procedure  && ! old_procedure->canDelete()) {
    lmgr->syntaxError("'proc' Existing procedure '%s' cannot be deleted", name_string.c_str());
    return;
  }

  /* Extract Argument List */

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'proc' '%s' argument list", name_string.c_str());
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int i = parse.getPos();

  std::string arg_string;

  if (! args->readArgList(cargs, &i, ')', arg_string)) {
    lmgr->syntaxError("missing close brackets for 'proc' '%s' argument list", name_string.c_str());
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'proc' '%s' argument list",
                      parse.getAt().c_str(), name_string.c_str());
    return;
  }

  /* Get List of Procedure's Arguments */
  ProcArgArray proc_args;

  ClLanguageArgList arg_list;

  args->stringToArgList(arg_string, &arg_list);

  int num_args = arg_list.size();

  for (i = 0; i < num_args; i++) {
    ClLanguageProcArg *proc_arg;

    if (arg_list[i][0] == '&')
      proc_arg = new ClLanguageProcArg(arg_list[i].substr(1), true);
    else
      proc_arg = new ClLanguageProcArg(arg_list[i], false);

    if (! ClParserVar::isValidName(proc_arg->getName())) {
      lmgr->syntaxError("invalid 'proc' '%s' argument name %d - '%s'",
                       name_string.c_str(), i + 1, proc_arg->getName().c_str());

      delete proc_arg;

      return;
    }

    proc_args.push_back(proc_arg);
  }

  //---

  /* Remove existing language procedure, language function and
     parser function with the same name */

  if (old_procedure)
    ClLanguageProcMgrInst->deleteProc(old_procedure->getName());

  if (old_lfunction) {
    ClParserInst->removeUserFn(old_lfunction->getName());

    ClLanguageFuncMgrInst->deleteFunc(old_lfunction->getName());
  }

  if (old_pfunction.isValid())
    ClParserInst->removeFunction(old_pfunction->getName());

  /* Add Procedure to List */

  static_cast<void>(ClLanguageProcMgrInst->
    createProc(name_string, proc_args.args(), command_list));

  proc_args.setAutoFree(false);

  command->clearCommands();
}

/*------------------------------------------------------------------*
 *
 * ClStdFuncCommand
 *   Routine called when a 'func ... endfunc' block command is entered.
 *
 *   The name following the 'func' is used as the name of the procedure and
 *   the subsequent round bracketed comma separated list of argument names and
 *   types is extracted. The list of language commands enclosed by the
 *   'func ... endfunc' pair are stored for later execution when the function name
 *   is specified in an expression.
 *
 * CALL:
 *   ClStdFuncCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments
 *           : supplied by the user.
 *
 * NOTES:
 *   Only the int, real and str types are currently supported.
 *
 *------------------------------------------------------------------*/

static void
ClStdFuncCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  struct FuncArgs {
   ~FuncArgs() {
      for (auto &func_arg : args)
        delete func_arg;
    }

    ClLanguageFuncArgArray args;
  };

  //---

  CL_LANGUAGE_TRACE("ClStdFuncCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  const ClLanguageCommandList &command_list = command->getCommandList();

  /* Extract Function Name */

  CStrParse parse(cargs);

  std::string name_string;

  if (! parse.readIdentifier(name_string)) {
    lmgr->syntaxError("'func' function name must start with alphabetic or '_'");
    return;
  }

  /* Get Existing Procedure and Function */

  ClLanguageProc* old_procedure = ClLanguageProcMgrInst->lookupProc(name_string);
  ClLanguageFunc* old_lfunction = ClLanguageFuncMgrInst->lookupFunc(name_string);
  ClParserFuncPtr old_pfunction = ClParserInst->getFunction(name_string);

  /* Ensure Procedure Can be Deleted */

  if (old_procedure != 0 && ! old_procedure->canDelete()) {
    lmgr->syntaxError("'func' Existing procedure '%s' cannot be deleted", name_string.c_str());
    return;
  }

  /* Extract Argument List */

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'func' '%s' argument list", name_string.c_str());
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int pos = parse.getPos();

  std::string arg_string;

  if (! args->readArgList(cargs, &pos, ')', arg_string)) {
    lmgr->syntaxError("missing close brackets for 'func' '%s' argument list", name_string.c_str());
    return;
  }

  parse.setPos(pos);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'func' '%s' argument list",
                      parse.getAt().c_str(), name_string.c_str());
    return;
  }

  /* Get List of Function's Arguments */

  ClLanguageArgList arg_list;

  args->stringToArgList(arg_string, &arg_list);

  int num_args = arg_list.size();

  FuncArgs funcArgs;

  for (int i = 0; i < num_args; i++) {
    std::string name = arg_list[i];

    //uint len = name.size();

    StringVectorT words;

    CStrUtil::addWords(name, words);

    int num_words = words.size();

    if (num_words != 1 && num_words != 2) {
      lmgr->syntaxError("invalid 'func' '%s' argument name %d",
                        name_string.c_str(), i + 1, name.c_str());
    }

    ClParserValueType type;

    if      (words[0] == "int"   ) type = CL_PARSER_VALUE_TYPE_INTEGER;
    else if (words[0] == "real"  ) type = CL_PARSER_VALUE_TYPE_REAL;
    else if (words[0] == "string") type = CL_PARSER_VALUE_TYPE_STRING;
    else if (words[0] == "array" ) type = CL_PARSER_VALUE_TYPE_ARRAY;
    else {
      lmgr->syntaxError("invalid 'func' '%s' argument type %d - '%s'",
                        name_string.c_str(), i + 1, name.c_str());
      return;
    }

    if (num_words > 1 && words[1][0] == '&') {
      type = (ClParserValueType) (type | CL_PARSER_VALUE_TYPE_OUTPUT);

      words[1] = words[1].substr(1);
    }
    else
      type = (ClParserValueType) (type | CL_PARSER_VALUE_TYPE_INPUT);

    if (words[1].size() == 0 || ! ClParserVar::isValidName(words[1])) {
      if (words[1].size() == 0)
        lmgr->syntaxError("missing 'func' '%s' argument name %d", name_string.c_str(), i + 1);
      else
        lmgr->syntaxError("invalid 'func' '%s' argument name %d - '%s'",
                          name_string.c_str(), i + 1, name.c_str());
      return;
    }

    name = words[1];

    auto *func_arg = new ClLanguageFuncArg(name, type);

    funcArgs.args.push_back(func_arg);
  }

  //---

  /* Remove existing language procedure, language function and
     parser function with the same name */

  if (old_procedure != 0)
    ClLanguageProcMgrInst->deleteProc(old_procedure->getName());

  if (old_lfunction != 0) {
    ClParserInst->removeUserFn(old_lfunction->getName());

    ClLanguageFuncMgrInst->deleteFunc(old_lfunction->getName());
  }

  if (old_pfunction.isValid())
    ClParserInst->removeFunction(old_pfunction->getName());

  /* Add Function to List */

  ClLanguageFunc *function =
    ClLanguageFuncMgrInst->createFunc(name_string, funcArgs.args, command_list);

  command->clearCommands();

  /* Define Function to Parser */

  IntVectorT arg_types;

  for (uint i = 0; i < function->getNumArgs(); i++)
    arg_types.push_back(function->getArg(i).getType());

  ClParserInst->createUserFn(function->getName(),
                             CL_PARSER_USERFN_TYPE_STD, arg_types,
                             ClLanguageRunFunction, (void *) function);
}

/*------------------------------------------------------------------*
 *
 * ClStdForCommand
 *   Routine called when a 'for ... endfor' block command is entered.
 *
 *   The semi-colon separated expressions contained in round brackets
 *   are used as the initial, while and iterate expressions.
 *
 *   The initial expression is evaluated and the list of language commands
 *   enclosed by 'for ... endfor' pair are then continually executed followed
 *   by the iterate expression while the while expression evaluates to a
 *   non-zero integer.
 *
 *   Any break commands exit the loop, any continue commands skip any remaining
 *   loop commands, and any gotos which jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdForCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdForCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdForCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /*
   * Extract 3 Expressions from For Arguments :-
   *
   *  1. Init    Expression
   *  2. While   Expression
   *  3. Iterate Expression
   */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'for' expression list");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int i = parse.getPos();

  std::string for_expr1;

  if (! args->readArgList(cargs, &i, ';', for_expr1)) {
    lmgr->syntaxError("missing condition expression in 'for' expression list");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  i = parse.getPos();

  std::string for_expr2;

  if (! args->readArgList(cargs, &i, ';', for_expr2)) {
    lmgr->syntaxError("missing condition expression in 'for' expression list");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  i = parse.getPos();

  std::string for_expr3;

  if (! args->readArgList(cargs, &i, ')', for_expr3)) {
    lmgr->syntaxError("missing iteration expression in 'for' expression list");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'for' expression list",
                      parse.getAt().c_str());
    return;
  }

  //---

  ClParserValuePtr value;

  /* Process Init Expression */

  ClParserExpr expr1(for_expr1);

  if (! expr1.exec(value)) {
    lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'for' expression 1 - '%s'",
                          for_expr1.c_str());
    return;
  }
  else if (! value.isValid()) {
    lmgr->syntaxError("undefined 'for' expression 1 - '%s'", for_expr1.c_str());
    return;
  }

  //---

  /* Process While Expression and Check Result */

  ClParserExpr expr2(for_expr2);

  if      (! expr2.exec(value)) {
    lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'for' expression 2 - '%s'",
                          for_expr2.c_str());
    return;
  }
  else if (! value.isValid()) {
    lmgr->syntaxError("undefined 'for' expression 2 - '%s'", for_expr2.c_str());
    return;
  }

  bool flag = value->toBool();

  //---

  /*
   * While Expression is True :-
   *  Process Command List and Iterate Expression
   */

  while (flag) {
    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      return;

    //---

    /* Process Iterate Expression */

    ClParserExpr expr3(for_expr3);

    if      (! expr3.exec(value)) {
      lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'for' expression 3 - '%s'",
                            for_expr3.c_str());
      return;
    }
    else if (! value.isValid()) {
      lmgr->syntaxError("undefined 'for' expression 3 - '%s'", for_expr3.c_str());
      return;
    }

    //---

    /* Process While Expression and Check Result */

    if      (! expr2.exec(value)) {
      lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'for' expression 2 - '%s'",
                            for_expr2.c_str());
      return;
    }
    else if (! value.isValid()) {
      lmgr->syntaxError("undefined 'for' expression 2 - '%s'", for_expr2.c_str());
      return;
    }

    flag = value->toBool();
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdForEachCommand
 *   Routine called when a 'foreach ... endforeach' block command is entered.
 *
 *   The 'foreach' str should be followed by the name of the variable to store
 *   the iteration value and a bracketed expression which will evaluate to the array
 *   to be iterated.
 *
 *   The expression is evaluated and the list of language commands enclosed by the
 *   'foreach ... endforeach' pair are executed for each value of the resltant array.
 *
 *   Any break commands exit the loop, any continue commands skip any remaining loop
 *   commands, and any gotos which jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdForEachCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdForEachCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdForEachCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /* Get Variable Name */

  CStrParse parse(cargs);

  parse.skipSpace();

  bool lvalue = false;

  if (parse.isChar('&')) {
    parse.skipChar();

    parse.skipSpace();

    lvalue = true;
  }

  std::string var_name;

  parse.readNonSpace(var_name);

  if (! ClParserVar::isValidName(var_name)) {
    lmgr->syntaxError("invalid 'foreach' variable name '%s'", var_name.c_str());
    return;
  }

  /* Get Expression */

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'foreach' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int i = parse.getPos();

  std::string arg_string;

  if (! args->readArgList(cargs, &i, ')', arg_string)) {
    lmgr->syntaxError("missing close brackets for 'foreach' expression");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'foreach' expression",
                      parse.getAt().c_str());
    return;
  }

  ClLanguageArgList arg_list;

  args->stringToArgList(arg_string, &arg_list);

  uint num_args = arg_list.size();

  if (num_args != 1) {
    lmgr->syntaxError("'foreach' requires a single expression - %d found", num_args);
    return;
  }

  std::string expression = arg_list[0];

  //---

  /* Process Expression */

  ClParserExpr expr(expression);

  ClParserValuePtr value;

  if      (! expr.exec(value)) {
    lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'foreach' expression 1 - '%s'",
                          expression.c_str());
    return;
  }
  else if (! value.isValid()) {
    lmgr->syntaxError("undefined 'foreach' expression 1 - '%s'", expression.c_str());
    return;
  }

  //---

  ClParserValueArray values;

  value->toSubValues(values);

  uint num_values = values.size();

  //---

  /* Set Variable to each Array Value and Process Commands */

  for (uint iv = 0; iv < num_values; iv++) {
    /* Set Variable Value */

    ClParserVarPtr variable = ClParserInst->createVar(var_name, values[iv]);

    //---

    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      return;

    //---

    if (lvalue) {
      int error_code;

      ClParserInst->assignSubscriptValue(expression, iv + 1, variable->getValue(), &error_code);

      if (error_code != 0) {
        lmgr->expressionError(error_code, "'foreach' failed to update value '%s'",
                              expression.c_str());
        return;
      }
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdWhileCommand
 *   Routine called when a 'while ... endwhile' block command is entered.
 *
 *   The expression contained in round brackets is extracted and the list of
 *   language commands enclosed by the 'while ... endwhile' pair are continually
 *   executed while the expression evaluates to a non-zero integer.
 *
 *   Any break commands exit the loop, any continue commands skip any remaining
 *   loop commands, and any gotos which jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdWhileCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments supplied
 *           : by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdWhileCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdWhileCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /* Extract While Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'while' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int pos = parse.getPos();

  std::string line;

  if (! args->readArgList(cargs, &pos, ')', line)) {
    lmgr->syntaxError("missing close brackets for 'while' expression");
    return;
  }

  parse.setPos(pos);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'while' expression", parse.getAt().c_str());
    return;
  }

  ClLanguageArgList arg_list;

  args->stringToArgList(line, &arg_list);

  int num_args = arg_list.size();

  if (num_args != 1) {
    lmgr->syntaxError("'while' requires a single expression - %d found", num_args);
    return;
  }

  std::string expression = arg_list[0];

  //---

  bool flag = false;

  do {
    /* Process Expression and Check Result */

    ClParserExpr expr(expression);

    ClParserValuePtr value;

    if      (! expr.exec(value)) {
      lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'while' expression - '%s'",
                            expression.c_str());
      return;
    }
    else if (! value.isValid()) {
      lmgr->syntaxError("undefined 'while' expression - '%s'", expression.c_str());
      return;
    }

    flag = value->toBool();

    //---

    /* If Expression is True Process Command List */

    if (flag) {
      if (ClLanguageBlockDataMgrInst->processBlockCommands())
        return;
    }
  }
  while (flag);
}

/*------------------------------------------------------------------*
 *
 * ClStdUntilCommand
 *   Routine called when a 'until ... enduntil' block command is entered.
 *
 *   The expression contained in round brackets is extracted and the list of
 *   language commands enclosed by the 'until ... enduntil' pair are continually
 *   executed until the expression evaluates to a zero integer.
 *
 *   Any break commands exit the loop, any continue commands skip any remaining
 *   loop commands, and any gotos which jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdUntilCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments supplied
 *           : by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdUntilCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdUntilCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /* Extract Until Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'until' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int pos = parse.getPos();

  std::string line;

  if (! args->readArgList(cargs, &pos, ')', line)) {
    lmgr->syntaxError("missing close brackets for 'until' expression");
    return;
  }

  parse.setPos(pos);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'until' expression", parse.getAt().c_str());
    return;
  }

  ClLanguageArgList arg_list;

  args->stringToArgList(line, &arg_list);

  int num_args = arg_list.size();

  if (num_args != 1) {
    lmgr->syntaxError("'until' requires a single expression - %d found", num_args);
    return;
  }

  std::string expression = arg_list[0];

  //---

  bool flag = false;

  do {
    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      return;

    //---

    /* Process Expression and Check Result */

    ClParserExpr expr(expression);

    ClParserValuePtr value;

    if      (! expr.exec(value)) {
      lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'until' expression - '%s'",
                            expression.c_str());
      return;
    }
    else if (! value.isValid()) {
      lmgr->syntaxError("undefined 'until' expression - '%s'", expression.c_str());
      return;
    }

    flag = value->toBool();
  }
  while (! flag);
}

/*------------------------------------------------------------------*
 *
 * ClStdRepeatCommand
 *   Routine called when a 'repeat ... endrepeat' block command is entered.
 *
 *   The expression contained in round brackets is extracted and executed to
 *   provide an integer count of the number of times the loop is to be executed.
 *   The list of language enclosed by the 'repeat ... endrepeat' pair are executed
 *   the specified number of times.
 *
 *   Any break commands exit the loop, any continue commands skip any remaining
 *   loop commands, and any gotos which jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdRepeatCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdRepeatCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdRepeatCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /* Extract Repeat Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'repeat' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int pos = parse.getPos();

  std::string line;

  if (! args->readArgList(cargs, &pos, ')', line)) {
    lmgr->syntaxError("missing close brackets for 'repeat' expression");
    return;
  }

  parse.setPos(pos);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'repeat' expression", parse.getAt().c_str());
    return;
  }

  ClLanguageArgList arg_list;

  args->stringToArgList(line, &arg_list);

  int num_args = arg_list.size();

  if (num_args != 1) {
    lmgr->syntaxError("'repeat' requires a single expression - %d found", num_args);
    return;
  }

  std::string expression = arg_list[0];

  //---

  ClParserExpr expr(expression);

  ClParserValuePtr value;

  if      (! expr.exec(value)) {
    lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'repeat' expression - '%s'",
                          expression.c_str());
    return;
  }
  else if (! value.isValid()) {
    lmgr->syntaxError("undefined 'repeat' expression - '%s'", expression.c_str());
    return;
  }

  long count;

  if (! value->integerValue(&count)) {
    lmgr->expressionError(ClErr::INVALID_CONVERSION, "'repeat' expression - '%s'",
                          expression.c_str());
    return;
  }

  if (count < 1) {
    lmgr->syntaxError("invalid count %d for 'repeat'", count);
    return;
  }

  //---

  for (int i = 0; i < count; i++) {
    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      break;
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdIfCommand
 *   Routine called when a 'if ... endif' block command is entered.
 *
 *   The block of commands enclosed by the 'if ... endif' may contain
 *   'elseif' and 'else' commands.
 *
 *   The expression contained in round brackets is extracted and evaluated.
 *   If the expression results in a non-zero integer then the enclosed commands
 *   up to any embedded 'elseif' or 'else' commands are executed. If the
 *   expression results in a zero integer then the enclosed commands up to
 *   any embedded 'elseif' or 'else' commands are skipped. If the command is
 *   an 'elseif' then the command is effectively treated as an 'if' and the
 *   execution starts again from the current position in the enclosed command list.
 *   If the command is an 'else' then the remaining enclosed commands are executed.
 *
 *   Any break commands exit the block but remain in force for any parent block.
 *   Any continue commands exit the block but remain in force for any parent block.
 *   Any goto commands outside the block exit it.
 *
 * CALL:
 *   ClStdIfCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command : and arguments
 *           : supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdIfCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  struct Commands {
    ~Commands() {
      delete [] commands;
    }

    ClLanguageCommand **commands = nullptr;
  };

  //---

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  CL_LANGUAGE_TRACE("ClStdIfCommand");

  const std::string &cargs = command->getArgs();

  const ClLanguageCommandList &command_list = command->getCommandList();

  ClLanguageCommandList command_list1;

  /* Extract If Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    lmgr->syntaxError("missing open brackets for 'if' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int pos = parse.getPos();

  std::string line;

  if (! args->readArgList(cargs, &pos, ')', line)) {
    lmgr->syntaxError("missing close brackets for 'if' expression");
    return;
  }

  parse.setPos(pos);

  parse.skipSpace();

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' after 'if' expression", parse.getAt().c_str());
    return;
  }

  ClLanguageArgList arg_list;

  args->stringToArgList(line, &arg_list);

  int num_args = arg_list.size();

  if (num_args != 1) {
    lmgr->syntaxError("'if' requires a single expression - %d found", num_args);
    return;
  }

  std::string expression = arg_list[0];

  //---

  /* Process Expression and Check Result */

  ClParserExpr expr(expression);

  ClParserValuePtr evalue;

  if      (! expr.exec(evalue)) {
    lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'if' expression - '%s'", expression.c_str());
    return;
  }
  else if (! evalue.isValid()) {
    lmgr->syntaxError("undefined 'if' expression - '%s'", expression.c_str());
    return;
  }

  bool flag = evalue->toBool();

  //---

  /* If Expression is True Process Command List */

  Commands commands;
  int      num_commands;

  lmgr->commandListToArray(command_list, &commands.commands, &num_commands);

  for (int i = 0; i < num_commands; i++) {
    if      (commands.commands[i]->isIdent(language_elseif_ident)) {
      if (flag)
        break;

      const std::string &args1 = commands.commands[i]->getArgs();

      /* Extract Else If Expression */

      CStrParse parse1(args1);

      parse1.skipSpace();

      if (! parse1.isChar('(')) {
        lmgr->syntaxError("missing open brackets for 'elseif' expression");
        return;
      }

      parse1.skipChar();

      parse1.skipSpace();

      int pos1 = parse1.getPos();

      std::string line1;

      if (! args->readArgList(args1, &pos1, ')', line1)) {
        lmgr->syntaxError("missing close brackets for 'elseif' expression");
        return;
      }

      parse1.setPos(pos1);

      parse1.skipSpace();

      if (! parse1.eof()) {
        lmgr->syntaxError("spurious characters '%s' after 'elseif' expression",
                          parse1.getAt().c_str());
        return;
      }

      args->stringToArgList(line1, &arg_list);

      num_args = arg_list.size();

      if (num_args != 1) {
        lmgr->syntaxError("'elseif' requires a single expression - %d found", num_args);
        return;
      }

      std::string expression0 = arg_list[0];

      //---

      /* Process Expression and Check Result */

      ClParserExpr expr0(expression0);

      ClParserValuePtr evalue0;

      if      (! expr0.exec(evalue0)) {
        lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'elseif' expression - '%s'",
                              expression0.c_str());
        return;
      }
      else if (! evalue0.isValid()) {
        lmgr->syntaxError("undefined 'elseif' expression - '%s'", expression0.c_str());
        return;
      }

      flag = evalue0->toBool();
    }
    else if (commands.commands[i]->isIdent(language_else_ident)) {
      if (flag)
        break;

      const std::string &args1 = commands.commands[i]->getArgs();

      CStrParse parse1(args1);

      parse1.skipSpace();

      if (! parse1.eof()) {
        lmgr->syntaxError("spurious characters '%s' after 'else' expression",
                          parse1.getAt().c_str());
        return;
      }

      flag = true;
    }
    else {
      if (flag)
        command_list1.push_back(commands.commands[i]);
    }
  }

  ClLanguageBlockDataMgrInst->setBlockCommands(command_list1);

  ClLanguageBlockDataMgrInst->processBlockCommands();
}

/*------------------------------------------------------------------*
 *
 * ClStdElseIfCommand
 *   Routine called when the 'elseif' command is entered outside of an
 *   'if ... endif' block.
 *
 *   If the 'elseif' command is encountered outside an 'if ... endif' block
 *  (which processes it manually) then it is a syntax error.
 *
 * CALL:
 *   ClStdElseIfCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdElseIfCommand(ClLanguageCommand *, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdElseIfCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Else If can only be processed inside If Command List */

  lmgr->syntaxError("'elseif' outside 'if' block");
}

/*------------------------------------------------------------------*
 *
 * ClStdElseCommand
 *   Routine called when the 'else' command is entered outside of an
 *   'if ... endif' block.
 *
 *   If the 'else' command is encountered outside an 'if ... endif' block
 *   (which processes it manually) then it is a syntax error.
 *
 * CALL:
 *   ClStdElseCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments
 *           : supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdElseCommand(ClLanguageCommand *, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdElseCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Else can only be processed inside If Command List */

  lmgr->syntaxError("'else' outside 'if' block");
}

/*------------------------------------------------------------------*
 *
 * ClStdGotoCommand
 *   Routine called when the 'goto' command is entered.
 *
 *   The name after the goto is extracted and we ensure that we are in a block
 *   command (otherwise there will be no saved label to goto). The label data for
 *   the corresponding label is found (if defined) and the goto details are stored
 *   in global variables ready for the parent block command to handle.
 *
 * CALL:
 *   ClStdGotoCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments supplied
 *           : by the user.
 *
 * NOTES:
 *   Sets goto flag  to 'true', goto name to the label name and goto depth to the
 *   depth at which the label appears. The parent block command should check the
 *   level and terminate if it is lower than the current level or perform the goto
 *   if at the same level.
 *
 *------------------------------------------------------------------*/

static void
ClStdGotoCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  std::string          arg;
  int                  error_code;
  ClLanguageLabelData *label_data;

  CL_LANGUAGE_TRACE("ClStdGotoCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Get Goto's Arguments */

  if (args->checkNumberOfArgs(1, 1) != 0)
    return;

  /* Get Label */

  arg = args->getArg(1, &error_code);

  if (error_code != 0)
    return;

  if (! ClParserVar::isValidName(arg)) {
    lmgr->syntaxError("invalid 'goto' label name '%s'", arg.c_str());
    return;
  }

  /* Check that we are in a Block Command */

  if (lmgr->getDepth() == 0) {
    lmgr->syntaxError("'goto' not allowed outside block command");
    return;
  }

  label_data = ClLanguageLabelDataMgrInst->getCurrentLabelData(arg);

  if (! label_data) {
    lmgr->syntaxError("'goto' label '%s' not known in current scope", arg.c_str());
    return;
  }

  lmgr->getGoto().set(label_data->getName(), label_data->getDepth());
}

/*------------------------------------------------------------------*
 *
 * ClStdBreakCommand
 *   Routine called when the 'break' command is entered.
 *
 *   The routine ensures no arguments are supplied and sets a global variable
 *   ready for the parent block command to handle.
 *
 * CALL:
 *   ClStdBreakCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments
 *           : supplied by the user.
 *
 * NOTES:
 *   Sets break flag to 'true'. If the parent iterates it should
 *   clear the flag and terminate the block, if it does not iterate
 *   then it should terminate the block and retain the flag.
 *
 *------------------------------------------------------------------*/

static void
ClStdBreakCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdBreakCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Set Break Flag */

  lmgr->setBreakFlag(true);
}

/*------------------------------------------------------------------*
 *
 * ClStdContinueCommand
 *   Routine called when the 'continue' command is entered.
 *
 *   The routine ensures no arguments are supplied and that we are in a block
 *   command. It then sets a global variable ready for the parent block command
 *   to handle.
 *
 * CALL:
 *   ClStdContinueCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   Sets continue flag to 'true'. If the parent iterates it should
 *   clear the flag and move onto its next iteration, if it does not
 *   iterate then it should terminate the block and retain the flag.
 *
 *------------------------------------------------------------------*/

static void
ClStdContinueCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdContinueCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Check that we are in a Block Command */

  if (lmgr->getDepth() == 0) {
    lmgr->syntaxError("'continue' not allowed outside block command");
    return;
  }

  /* Set Continue Flag */

  lmgr->setContinueFlag(true);
}

/*------------------------------------------------------------------*
 *
 * ClStdReturnCommand
 *   Routine called when the 'return' command is entered.
 *
 *   The routine ensures no arguments are supplied and
 *   that we are in a block command. It then sets a global
 *   variable ready for the parent block command to handle.
 *
 * CALL:
 *   ClStdReturnCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   Sets return flag to 'true'. This is handled when the procedure
 *   or function is run and causes it to terminate execution of its
 *   commands. All Block commands should terminate and retain the flag.
 *
 *------------------------------------------------------------------*/

static void
ClStdReturnCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdReturnCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Check that we are in a Block Command */

  if (lmgr->getDepth() == 0) {
    lmgr->syntaxError("'return' not allowed outside block command");
    return;
  }

  /* Set Return Flag */

  lmgr->setReturnFlag(true);
}

/*------------------------------------------------------------------*
 *
 * ClStdOnCommand
 *   Routine called when the 'on' command is entered.
 *
 *   When the specified signal type occurs the specified command is executed.
 *   If no command is specified then signal handling reverts to its default action.
 *
 *   Signal Types :-
 *     syntax     : Syntax Error (Internal Signal)
 *     expression : Expression Error (Internal Signal)
 *     interrupt  : Interrupt (SIGINT)
 *     quit       : Quit (SIGQUIT)
 *     abort      : Abort (SIGABRT)
 *     terminate  : Terminate (SIGTERM)
 *     alarm      : Alarm (SIGALRM)
 *     child      : Child (SIGCHLD)
 *     segv       : Segmentation Violation (SIGSEGV)
 *
 * CALL:
 *   ClStdOnCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdOnCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  int         flag;
  std::string type;
  std::string command1;

  CL_LANGUAGE_TRACE("ClStdOnCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /*-------------------*/

  /* Get Signal Type */

  CStrParse parse(cargs);

  parse.skipSpace();

  parse.readNonSpace(type);

  /* Get Command */

  parse.skipSpace();

  parse.readNonSpace(command1);

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' at end of 'on' expression",
                      parse.getAt().c_str());
    return;
  }

  /*-------------------*/

  flag = ClSignalMgrInst->setSignalCommand(type, command1);

  if (! flag)
    lmgr->error("invalid signal type '%s'", type.c_str());
}

/*------------------------------------------------------------------*
 *
 * ClStdRaiseCommand
 *   Routine called when the 'raise' command is entered.
 *
 *   The routine raises the specified signal calling its associated command
 *   or routine.
 *
 * CALL:
 *   ClStdRaiseCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdRaiseCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  int         flag;
  std::string type;
  std::string data;

  CL_LANGUAGE_TRACE("ClStdRaiseCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /*-------------------*/

  /* Get Signal Type */

  CStrParse parse(cargs);

  parse.skipSpace();

  parse.readNonSpace(type);

  /*-------------------*/

  /* Get Data */

  parse.skipSpace();

  parse.readNonSpace(data);

  if (! parse.eof()) {
    lmgr->syntaxError("spurious characters '%s' at end of 'raise' expression",
                      parse.getAt().c_str());
    return;
  }

  /*-------------------*/

  flag = ClSignalMgrInst->raiseSignal(type, data);

  if (! flag)
    lmgr->error("invalid signal type '%s'", type.c_str());
}

/*------------------------------------------------------------------*
 *
 * ClStdDefineCommand
 *   Routine called when the 'define' command is entered.
 *
 *   This routine allows the user to define a simple function consisting of
 *   a single expression whose input variables are set to the values supplied
 *   when the function is invoked.
 *
 * CALL:
 *   ClStdDefineCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   Allows a function to be defined from the command line interface.
 *
 *------------------------------------------------------------------*/

static void
ClStdDefineCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  std::string::size_type p;
  std::string            lhs, rhs;
  int                    error_code;
  std::string            name_string;
  ClLanguageFunc*        old_lfunction;
  ClLanguageProc*        old_procedure;

  CL_LANGUAGE_TRACE("ClStdDefineCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /* Extract Procedure Name */

  CStrParse parse(cargs);

  if (! parse.readIdentifier(name_string)) {
    lmgr->syntaxError("'define' function name must start with alphabetic or '_'");
    return;
  }

  /* Get Existing Procedure and Function */

  old_procedure = ClLanguageProcMgrInst->lookupProc(name_string);
  old_lfunction = ClLanguageFuncMgrInst->lookupFunc(name_string);

  /* Ensure Procedure Can be Deleted */

  if (old_procedure != 0 && ! old_procedure->canDelete()) {
    lmgr->syntaxError("'define' Existing procedure '%s' cannot be deleted", name_string.c_str());
    return;
  }

  /* Define Procedure */

  p = cargs.find('=');

  if (p == std::string::npos) {
    lmgr->syntaxError("missing equals sign for 'define'");
    return;
  }

  /* Remove existing Language Function's Parser User Function
     to prevent namespace error when creating Parser Function */

  if (old_lfunction != 0)
    ClParserInst->removeUserFn(old_lfunction->getName());

  lhs = cargs.substr(0, p - 1);
  rhs = cargs.substr(p + 1);

  ClParserInst->createFunc(cargs, rhs, &error_code);

  if (error_code != 0) {
    lmgr->expressionError(error_code, "'define' '%s' = '%s'", lhs.c_str(), rhs.c_str());

    /* Restore Language Function's Parser User Function if failed
       to define Parser Function */

    if (old_lfunction != 0) {
      int *arg_types = nullptr;

      if (old_lfunction->getNumArgs() > 0)
        arg_types = new int [old_lfunction->getNumArgs()];

      for (uint i = 0; i < old_lfunction->getNumArgs(); i++)
        arg_types[i] = old_lfunction->getArg(i).getType();

      ClParserInst->createUserFn(old_lfunction->getName(),
                                 CL_PARSER_USERFN_TYPE_STD, arg_types,
                                 old_lfunction->getNumArgs(),
                                 ClLanguageRunFunction, (void *) old_lfunction);

      if (arg_types != 0)
        delete [] arg_types;
    }

    return;
  }

  /* Remove existing alnguage procedure and/or function
     with the same name */

  ClLanguageProcMgrInst->deleteProc(old_procedure->getName());
  ClLanguageFuncMgrInst->deleteFunc(old_lfunction->getName());
}

/*------------------------------------------------------------------*
 *
 * ClStdStructCommand
 *   Routine called when the 'struct' command is entered.
 *
 *   This routine allows the user to define a structure consisting of a name
 *   and a type name list enclosed in curly brackets.
 *
 * CALL:
 *   ClStdStructCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   Allows the ClParserDefineStructure routine to be
 *   used from the command line interface.
 *
 *------------------------------------------------------------------*/

static void
ClStdStructCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  std::string type_arg_list;
  std::string structure_name;

  CL_LANGUAGE_TRACE("ClStdStructCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  /* Extract Structure Name */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.readIdentifier(structure_name)) {
    lmgr->syntaxError("missing structure name for 'struct'");
    return;
  }

  parse.skipSpace();

  if (! parse.isChar('{')) {
    lmgr->syntaxError("missing open curly brace for 'struct' '%s'", structure_name.c_str());
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  parse.readToChar('}', type_arg_list);

  CStrUtil::stripSpaces(type_arg_list);

  if (type_arg_list == "") {
    lmgr->syntaxError("missing open type/argument list for 'struct' '%s'", structure_name.c_str());
    return;
  }

  if (! parse.isChar('}')) {
    lmgr->syntaxError("missing close curly brace for 'struct' '%s'", structure_name.c_str());
    return;
  }

  /*--------------*/

  try {
    ClParserInst->createType(structure_name, type_arg_list);
  }
  catch (int error_code) {
    lmgr->expressionError(error_code, "'struct' '%s' { '%s' }",
                          structure_name.c_str(), type_arg_list.c_str());
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdInputCommand
 *   Routine called when the 'input' command is entered.
 *
 *   This routine outputs the specified prompt str and waits for the user to
 *   enter some text. If the variable specified as the second argument starts
 *   with a '$' then the text is assigned directly to the variable (name minus
 *   the dollar) otherwise the text is treated as an expression and is evaluated
 *   and the resultant value assigned to the variable.
 *
 * CALL:
 *   ClStdInputCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   If an argument starts with '@' then the subsequent
 *   str is taken as the name of a variable containing
 *   the file pointer to output to.
 *
 *   The input str is restricted to 255 characters.
 *
 *------------------------------------------------------------------*/

static void
ClStdInputCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdInputCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  int         evaluate;
  std::string variable;
  std::string variable1;
  int         arg_offset;
  std::string prompt_string;

  /* Check Number of Arguments */

  if (args->checkNumberOfArgs(2, 3))
    return;

  /* Check for specified Input File */

  int error_code;

  std::string arg = args->getArg(1, &error_code);

  if (error_code != 0)
    return;

  FILE *fp = nullptr;

  if (arg[0] == '@') {
    int i = 1;

    std::string name = arg.substr(i);

    CStrUtil::stripSpaces(name);

    if (! ClParserVar::isValidName(name)) {
      lmgr->syntaxError("invalid 'input' file variable %s", arg.c_str());
      return;
    }

    ClParserValuePtr value = ClParserInst->getVariableValue(name);

    long fp1;

    if (! value->integerValue(&fp1)) {
      lmgr->expressionError(ClErr::INVALID_CONVERSION, "'input' file variable '%s'", name.c_str());
      return;
    }

    fp = (FILE *) fp1;

    arg_offset = 1;
  }
  else {
    fp         = stdin;
    arg_offset = 0;
  }

  /* Check Number of Arguments */

  if (args->checkNumberOfArgs(2 + arg_offset, 2 + arg_offset) != 0)
    return;

  /* Get Input Prompt String */

  prompt_string = args->getStringArg(1 + arg_offset, &error_code);

  if (error_code != 0)
    return;

  /* Get Variable Name */

  variable = args->getArg(2 + arg_offset, &error_code);

  if (error_code != 0)
    return;

  if (variable[0] == '$') {
    evaluate  = false;
    variable1 = variable.substr(1);
  }
  else {
    evaluate  = true;
    variable1 = variable;
  }

  if (! ClParserInst->isValidAssignString(variable1)) {
    lmgr->syntaxError("'input' argument 2 (invalid variable name '%s')", variable1.c_str());
    return;
  }

  /* Prompt User for Input */

  lmgr->output("%s", prompt_string.c_str());

  std::string line;

#ifdef CEIL_READLINE
  if (fp == stdin) {
    lmgr->getReadLine().setPrompt("");

    line = lmgr->getReadLine().readLine();
  }
  else {
    CFile file(fp);

    if (! file.readLine(line))
      line = "";
  }
#else
  CFile file(fp);

  if (! file.readLine(line))
    line = "";
#endif

  /* Handle User Interrupt */

  if (line == "" || lmgr->checkAbort()) {
    if (ClSignalMgrInst->checkSignalCommand(SIGINT)) {
      if (line == "")
        ClSignalMgrInst->raiseSignal("interrupt", "");
    }
    else
      lmgr->error("** Interrupt ** input abandoned");

    goto set_value;
  }

  if (evaluate) {
    /* Evaluate Reply as Expression */

    args->setExpressionArg(2, line, &error_code);

    if (error_code != 0)
      goto set_value;
  }
  else {
    /* Set Variable to Entered String */

    ClParserValuePtr value = ClParserValueMgrInst->createValue(line);

    if (! value.isValid())
      goto set_value;

    ClParserInst->assignValue(variable1, value, &error_code);

    if (error_code != 0) {
      lmgr->expressionError(error_code, "'input' argument 2 - '%s'", line.c_str());
      return;
    }
  }

  return;

 set_value:
  if (evaluate)
    args->setExpressionArg(2, "", &error_code);
  else {
    ClParserValuePtr value = ClParserValueMgrInst->createValue("");

    if (! value.isValid())
      return;

    ClParserInst->assignValue(variable1, value, &error_code);

    if (error_code != 0) {
      lmgr->expressionError(error_code, "'input' argument 2 - '%s'", line.c_str());
      return;
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdPrintCommand
 *   Routine called when the 'print' command is entered.
 *
 *   This routine prints the value of each command *   separated expression
 *   specified followed by an optional new line. Each expression is separated
 *   from the next with a space.
 *
 * CALL:
 *   ClStdPrintCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and arguments
 *           : supplied by the user.
 *
 * NOTES:
 *   If an argument starts with '@' then the subsequent str is taken as the
 *   name of a variable containing the file pointer to output to.
 *   If the command ends with a ',' the no newline is output.
 *
 *------------------------------------------------------------------*/

static void
ClStdPrintCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdPrintCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  uint len = cargs.size();

  bool newline = true;

  if (len > 0 && cargs[len - 1] == ',')
    newline = false;

  /*-------------*/

  FILE *fp      = lmgr->setOutputFp(0);
  FILE *save_fp = fp;

  /* Get Print's Arguments */

  uint num_args = args->getNumArgs();

  /* Print Arguments */

  bool value_output = false;

  for (uint i = 1; i <= num_args; i++) {
    int error_code;

    std::string arg = args->getArg(i, &error_code);

    if (error_code != 0)
      goto done;

    if (arg[0] == '@') {
      fflush(fp);

      int j = 1;

      std::string name = arg.substr(j);

      CStrUtil::stripSpaces(name);

      if (! ClParserVar::isValidName(name)) {
        lmgr->syntaxError("invalid 'print' file variable %s", arg.c_str());
        goto done;
      }

      ClParserValuePtr value = ClParserInst->getVariableValue(name);

      long fp1;

      if (! value->integerValue(&fp1)) {
        lmgr->expressionError(ClErr::INVALID_CONVERSION, "'print' file variable '%s'",
                              name.c_str());
        goto done;
      }

      fp = (FILE *) fp1;

      lmgr->setOutputFp(fp);

      value_output = false;

      continue;
    }

    ClParserExpr expr1(arg);

    ClParserValuePtr value;

    if      (! expr1.exec(value)) {
      if (num_args == 1)
        lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'print' expression '%s'", arg.c_str());
      else
        lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'print' expression %d - '%s'",
                              i, arg.c_str());
      goto done;
    }
    else if (! value.isValid()) {
      if (num_args == 1)
        lmgr->syntaxError("undefined 'print' expression '%s'", arg.c_str());
      else
        lmgr->syntaxError("undefined 'print' expression %d - '%s'", i, arg.c_str());
      goto done;
    }

    if (value_output)
      lmgr->output(" ");

    value->print();

    value_output = true;
  }

 done:
  if (newline)
    lmgr->output("\n");

  fflush(fp);

  lmgr->setOutputFp(save_fp);
}

/*------------------------------------------------------------------*
 *
 * ClStdFormatCommand
 *   Routine called when the 'format' function is entered.
 *
 *   This routine takes the first value as the format str and then scans it
 *   for embedded control codes which are used in conjunction with any extra
 *   arguments supplied after the format str.
 *
 *   The style of the format str is identical to that used by the C function
 *   'printf' without the 'p' and 'n' format codes. The number and type of the
 *   supplied arguments are checked and are ignored, and the embedded format
 *   code left as is, if the argument is the wrong type or is not present.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdFormatCommand(ClParserValuePtr *, uint, void *, int &error_code);
 *
 * INPUT:
 *   values     : The array of values supplied when the function was called.
 *
 *   num_values : The number of values supplied when the function was called.
 *
 *   data       : The optional data specified when the function was specified.
 *
 * OUTPUT:
 *   error_code : Error Code to indicate whether the function was successful.
 *              :  <0 : Function failed (function specific error code).
 *              :  =0 : Function successful.
 *              :  >0 : Function failed (CL specific error code).
 *
 * RETURNS:
 *   value      : The value of the function.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static ClParserValuePtr
ClStdFormatCommand(ClParserValuePtr *values, uint num_values, void *, int *error_code)
{
  CL_LANGUAGE_TRACE("ClStdFormatCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  *error_code = 0;

  ClParserValuePtr value;

  if (num_values <= 0) {
    lmgr->syntaxError("'format' requires at least 1 value");
    return ClParserValuePtr();
  }

  std::string format_string;

  if (! values[0]->stringValue(format_string)) {
    lmgr->expressionError(ClErr::INVALID_CONVERSION, "'format' argument 1");
    return ClParserValuePtr();
  }

  uint value_num = 1;

  std::string temp_string1 = "";

  uint len = format_string.size();

  uint i = 0;

  while (i < len) {
    switch (format_string[i]) {
      case '%': {
        int         flags;
        std::string temp_string2;
        std::string value_format_string;
        int         field_width, precision;
        char        length_modifier, format_code;

        if (! CStrUtil::readFormat(format_string, &i, value_format_string, &field_width,
                                   &precision, &length_modifier, &format_code, &flags)) {
          temp_string1 += format_string[i++];
          break;
        }

        if (strchr("diouxXfeEgGcs%", format_code) == 0) {
          temp_string1 += value_format_string;
          break;
        }

        if (format_code == '%') {
          temp_string1 += format_code;
          break;
        }

        if (flags & CPRINTF_FIELD_WIDTH_AS_VALUE) {
          long lfield_width;

          if (value_num >= num_values) {
            temp_string1 += value_format_string;
            break;
          }

          if (values[value_num]->integerValue(&lfield_width))
            field_width = lfield_width;
          else
            field_width = 0;

          value_num++;
        }

        if (flags & CPRINTF_PRECISION_AS_VALUE) {
          long lprecision;

          if (value_num >= num_values) {
            temp_string1 += value_format_string;
            break;
          }

          if (values[value_num]->integerValue(&lprecision))
            precision = lprecision;
          else
            precision = 0;

          value_num++;
        }

        if (value_num >= num_values) {
          temp_string1 += value_format_string;
          break;
        }

        switch (format_code) {
          case 'd': case 'i': case 'o': case 'u': case 'x': case 'X': case 'c': {
            long integer;

            if (! values[value_num]->integerValue(&integer)) {
              temp_string1 += value_format_string;
              break;
            }

            if (format_code == 'c') {
              if (integer == 0 || ! isprint(integer))
                integer = '.';
            }

            if      ((flags & CPRINTF_FIELD_WIDTH_AS_VALUE) &&
                     (flags & CPRINTF_PRECISION_AS_VALUE  ))
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                field_width, precision, integer);
            else if (flags & CPRINTF_FIELD_WIDTH_AS_VALUE)
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(), field_width, integer);
            else if (flags & CPRINTF_PRECISION_AS_VALUE)
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(), precision, integer);
            else
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(), integer);

            break;
          }
          case 's': {
            std::string str;

            if (! values[value_num]->stringValue(str)) {
              temp_string1 += value_format_string;
              break;
            }

            if      ((flags & CPRINTF_FIELD_WIDTH_AS_VALUE) &&
                     (flags & CPRINTF_PRECISION_AS_VALUE  ))
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                field_width, precision, str.c_str());
            else if (flags & CPRINTF_FIELD_WIDTH_AS_VALUE)
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                field_width, str.c_str());
            else if (flags & CPRINTF_PRECISION_AS_VALUE)
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                precision, str.c_str());
            else
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(), str.c_str());

            break;
          }
          case 'f': case 'e': case 'E': case 'g': case 'G': {
            double real;

            if (! values[value_num]->realValue(&real)) {
              temp_string1 += value_format_string;
              break;
            }

            if      ((flags & CPRINTF_FIELD_WIDTH_AS_VALUE) &&
                     (flags & CPRINTF_PRECISION_AS_VALUE  ))
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                field_width, precision, real);
            else if (flags & CPRINTF_FIELD_WIDTH_AS_VALUE)
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(), field_width, real);
            else if (flags & CPRINTF_PRECISION_AS_VALUE)
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(), precision, real);
            else
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(), real);

            break;
          }
        }

        temp_string1 += temp_string2;

        value_num++;

        break;
      }
      default: {
        temp_string1 += format_string[i++];

        break;
      }
    }
  }

  value = ClParserValueMgrInst->createValue(temp_string1);

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdScanCommand
 *   Routine called when the 'scan' function is entered.
 *
 *   The routine extracts values for the supplied str using the supplied template.
 *   The resultant values are store in the variables specified by the remaining
 *   arguments.
 *
 *   The style of the template is identical to that used by the C function 'scanf'.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdFormatCommand(ClParserValuePtr *, uint, void *, int &error_code);
 *
 * INPUT:
 *   values     : The array of values supplied
 *              : when the function was called.
 *
 *   num_values : The number of values supplied
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
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static ClParserValuePtr
ClStdScanCommand(ClParserValuePtr *values, uint num_values, void *, int *error_code)
{
  struct Addrs {
   ~Addrs() {
      for (uint i = 0; i < num_addrs; i++)
        delete addrs[i];
    }

    char* addrs[20];
    int   codes[20];
    uint  num_addrs = 0;
  };

  CL_LANGUAGE_TRACE("ClStdScanCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  *error_code = -1;

  ClParserValuePtr value;

  //---

  if (num_values < 2) {
    lmgr->syntaxError("'scan' requires at least 2 values");
    return value;
  }

  //---

  /* Get Source and Format Strings */

  std::string source_string;

  if (! values[0]->stringValue(source_string)) {
    lmgr->expressionError(ClErr::INVALID_CONVERSION, "'scan' argument 1");
    return value;
  }

  std::string format_string;

  if (! values[1]->stringValue(format_string)) {
    lmgr->expressionError(ClErr::INVALID_CONVERSION, "'scan' argument 2");
    return value;
  }

  //---

  Addrs addrs;

  addrs.num_addrs = 0;

  uint i = 0;

  uint len1 = source_string.size();
  uint len2 = format_string.size();

  std::string format_string1 = "";

  while (i < len2) {
    switch (format_string[i]) {
      case '%': {
        int         flags;
        char        format_code;
        std::string format_string2;
        std::string format_string3;
        char        length_modifier;
        int         field_width, precision;

        bool ignore = false;

        if (! CStrUtil::readFormat(format_string, &i, format_string2,
                                   &field_width, &precision, &length_modifier,
                                   &format_code, &flags))
          continue;

        if ((flags & CPRINTF_FIELD_WIDTH_AS_VALUE) ||
            (flags & CPRINTF_PRECISION_AS_VALUE  ))
          ignore = true;

        if (ignore || format_code == '%')
          continue;

        format_string3 = "";

        if (format_code == '[') {
          format_string3 += format_code;

          if (i < len2 && format_string[i] == '^')
            format_string3 += format_string[i++];

          while (i < len2 && format_string[i] != ']')
            format_string3 += format_string[i++];

          if (i >= len2) {
            lmgr->syntaxError("Missing ']' in scan");
            return value;
          }

          format_string3 += format_string[i++];

          continue;
        }
        else {
          if (strchr("diouxcsefgpn", format_code) == 0)
            continue;

          if (strchr("diouxefg", format_code) != 0)
            format_string3 += 'l';

          format_string3 += format_code;
        }

        if (addrs.num_addrs >= 20) {
          lmgr->syntaxError("Too many values to scan (> 20)");
          return value;
        }

        addrs.codes[addrs.num_addrs] = format_code;

        switch (format_code) {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
            addrs.addrs[addrs.num_addrs] = reinterpret_cast<char *>(new long);
            break;
          case 'c':
            if (len1 > 255)
              addrs.addrs[addrs.num_addrs] = new char [len1];
            else
              addrs.addrs[addrs.num_addrs] = new char [256];

            break;
          case 's':
            if (len1 > 255)
              addrs.addrs[addrs.num_addrs] = new char [len1];
            else
              addrs.addrs[addrs.num_addrs] = new char [256];

            break;
          case 'e':
          case 'f':
          case 'g':
            addrs.addrs[addrs.num_addrs] = reinterpret_cast<char *>(new double);
            break;
          case 'p':
            addrs.addrs[addrs.num_addrs] = reinterpret_cast<char *>(new char *);
            break;
          case 'n':
            addrs.addrs[addrs.num_addrs] = reinterpret_cast<char *>(new int);
            break;
        }

        addrs.num_addrs++;

        format_string1 += format_string2;

        break;
      }
      default: {
        format_string1 += format_string[i++];

        break;
      }
    }
  }

  if (num_values - 2 != addrs.num_addrs) {
    lmgr->syntaxError("invalid no. of args (%d) to scan into - need %d",
                      num_values - 2, addrs.num_addrs);
    return value;
  }

  for (i = addrs.num_addrs; i < 20; i++) {
    addrs.codes[i] = '\0';
    addrs.addrs[i] = 0;
  }

  uint num_scanned =
    sscanf(source_string.c_str(), format_string1.c_str(),
           addrs.addrs[ 0], addrs.addrs[ 1], addrs.addrs[ 2], addrs.addrs[ 3],
           addrs.addrs[ 4], addrs.addrs[ 5], addrs.addrs[ 6], addrs.addrs[ 7],
           addrs.addrs[ 8], addrs.addrs[ 9], addrs.addrs[10], addrs.addrs[11],
           addrs.addrs[12], addrs.addrs[13], addrs.addrs[14], addrs.addrs[15],
           addrs.addrs[16], addrs.addrs[17], addrs.addrs[18], addrs.addrs[19]);

/*
  if (num_scanned != addrs.num_addrs) {
    lmgr->syntaxError("Failed to scan %d values, only %d found", addrs.num_addrs, num_scanned);
    return value;
  }
*/

  for (i = 0; i < num_scanned; i++) {
    switch (addrs.codes[i]) {
      case 'd': case 'i': case 'o': case 'u': case 'x':
        value = ClParserValueMgrInst->createValue(*((long *) addrs.addrs[i]));

        break;
      case 'c': case 's':
        value = ClParserValueMgrInst->createValue(reinterpret_cast<char *>(addrs.addrs[i]));

        break;
      case 'e': case 'f': case 'g':
        value = ClParserValueMgrInst->createValue(*(reinterpret_cast<double *>(addrs.addrs[i])));

        break;
      case 'p':
        value = ClParserValueMgrInst->createValue(reinterpret_cast<long>(addrs.addrs[i]));

        break;
      case 'n':
        value = ClParserValueMgrInst->createValue(
                  static_cast<long>(*(reinterpret_cast<int *>(addrs.addrs[i]))));

        break;
    }

    values[i + 2] = value;
  }

  value = ClParserValueMgrInst->createValue(static_cast<long>(num_scanned));

  *error_code = 0;

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdExecCommand
 *   Routine called when the 'exec' function is entered.
 *
 *   This routine executes a command and stores the resultant output in a string
 *   array value (one element for each output line from the command). The command
 *   is specifed as the single argument.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdExecCommand(ClParserValuePtr *, uint, void *, int &error_code);
 *
 * INPUT:
 *   values     : The array of values supplied
 *              : when the function was called.
 *
 *   num_values : The number of values supplied
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
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static ClParserValuePtr
ClStdExecCommand(ClParserValuePtr *values, uint, void *, int *error_code)
{
  int                      i;
  std::string              line;
  StringVectorT            lines;
  std::string              output;
  uint                     dims[1];
  std::string              command;
  uint                     num_dims;
  int                      num_lines;
  ClLanguageArgList        line_list;
  COSProcess::CommandState command_state;

  CL_LANGUAGE_TRACE("ClStdExecCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  *error_code = 0;

  ClParserValuePtr value;

  if (! values[0]->stringValue(command)) {
    lmgr->expressionError(ClErr::INVALID_CONVERSION, "'exec'");
    goto fail;
  }

  /* Execute Command and Read Output */

  if (! COSProcess::executeCommand(command, output, &command_state)) {
    *error_code = -1;

    lmgr->syntaxError("'exec' command '%s' failed", command.c_str());

    goto fail;
  }

  CStrUtil::addLines(output, lines);

  num_lines = lines.size();

  for (i = 0; i < num_lines; ++i)
    line_list.push_back(lines[i]);

  ClParserInst->createVar("_rc",
    ClParserValueMgrInst->createValue(static_cast<long>(command_state.getStatus())));

  dims[0]  = num_lines;
  num_dims = 1;

  value = ClParserValueMgrInst->createValue(dims, num_dims, line_list);

 fail:
  if (! value.isValid()) {
    dims[0]  = 1;
    num_dims = 1;

    line_list.push_back("");

    value = ClParserValueMgrInst->createValue(dims, num_dims, line_list);
  }

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdParseCommand
 *   Routine called when the 'parse' function is entered.
 *
 *   This routine executes a str as an expression and returns the resultant value.
 *   The expression str is specifed as the single argument.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdParseCommand(ClParserValuePtr *, uint, void *, int &error_code);
 *
 * INPUT:
 *   values     : The array of values supplied
 *              : when the function was called.
 *
 *   num_values : The number of values supplied
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
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static ClParserValuePtr
ClStdParseCommand(ClParserValuePtr *values, uint, void *, int *error_code)
{
  std::string command;

  CL_LANGUAGE_TRACE("ClStdParseCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  *error_code = 0;

  ClParserValuePtr value;

  if (! values[0]->stringValue(command)) {
    lmgr->expressionError(ClErr::INVALID_CONVERSION, "'parse'");
    return ClParserValuePtr();
  }

  ClParserExpr expr(command);

  if (! expr.exec(value)) {
    lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'parse' expression '%s'", command.c_str());
    return ClParserValuePtr();
  }

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdSetCommand
 *   Routine called when the 'set' command is entered.
 *
 *   This routine sets the value of the specified environment variable. The
 *   environment variable name is specified as a str in the first argument
 *   and the str to which the environment is set is specified as the second argument.
 *
 * CALL:
 *   ClStdSetCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdSetCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdSetCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  // Get set's Arguments
  const std::string &cargs = command->getArgs();

  CStrParse parse(cargs);

  // Get Variable Name
  std::string name;

  if (! parse.readIdentifier(name)) {
    lmgr->syntaxError("'set' invalid variable name");
    return;
  }

  // Skip '=' (TODO: required)
  parse.skipSpace();

  if (parse.isChar('=')) {
    parse.skipChar();

    parse.skipSpace();
  }

  // Get Value String
  std::string valueStr;

  parse.readNonSpace(valueStr);

  // Evaluate Value

  ClParserExpr expr(valueStr);

  ClParserValuePtr value;

  if (! expr.exec(value)) {
    lmgr->expressionError(ClErr::INVALID_EXPRESSION, "'set' expression '%s'", valueStr.c_str());
    return;
  }

  // Set Variable
  ClParserInst->setVariableValue(name, value);
}

/*------------------------------------------------------------------*
 *
 * ClStdGetEnvCommand
 *   Routine called when the 'getenv' command is entered.
 *
 *   This routine gets the value of the specified environment variable. The
 *   environment variable name is specified as a str in the second argument
 *   and the variable name to which the environment variable's value str is
 *   assigned is specified as the first argument.
 *
 *   If the environment variable is not set then the variable's value is set
 *   to the empty str "".
 *
 * CALL:
 *   ClStdGetEnvCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdGetEnvCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  std::string env_name;
  std::string env_value;
  int         error_code;

  CL_LANGUAGE_TRACE("ClStdGetEnvCommand");

  /* Get GetEnv's Arguments */

  if (args->checkNumberOfArgs(2, 2) != 0)
    return;

  /* Get Environment Variable Name */

  env_name = args->getStringArg(2, &error_code);

  if (error_code != 0)
    env_name = "";

  /* Get Environment Variable */

  env_value = "";

  if (env_name != "" && CEnvInst.exists(env_name))
    CEnvInst.get(env_name, env_value);

  args->setStringArg(1, env_value, &error_code);
}

/*------------------------------------------------------------------*
 *
 * ClStdSetEnvCommand
 *   Routine called when the 'setenv' command is entered.
 *
 *   This routine sets the value of the specified environment variable.
 *   The environment variable name is specified as a str in the first argument and
 *   the str to which the environment is set is specified as the second argument.
 *
 * CALL:
 *   ClStdSetEnvCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdSetEnvCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdSetEnvCommand");

  /* Get SetEnv's Arguments */

  if (args->checkNumberOfArgs(2, 2) != 0)
    return;

  int error_code;

  /* Get Environment Variable and Value */

  std::string env_name = args->getStringArg(1, &error_code);

  if (error_code != 0)
    return;

  std::string env_value = args->getStringArg(2, &error_code);

  if (error_code != 0)
    return;

  /* Set Environment Variable */

  CEnvInst.set(env_name, env_value);
}

/*------------------------------------------------------------------*
 *
 * ClStdUnSetEnvCommand
 *   Routine called when the 'unsetenv' command is entered.
 *
 *   This routine removes the specified environment variable. The environment
 *   variable name is specified as a str in the single argument.
 *
 * CALL:
 *   ClStdUnSetEnvCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdUnSetEnvCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  std::string env_name;
  int         error_code;

  CL_LANGUAGE_TRACE("ClStdUnSetEnvCommand");

  /* Get UnSetEnv's Arguments */

  if (args->checkNumberOfArgs(1, 1) != 0)
    return;

  /* Get Environment Variable */

  env_name = args->getStringArg(1, &error_code);

  if (error_code != 0)
    return;

  /* Unset Environment Variable */

  if (CEnvInst.exists(env_name))
    CEnvInst.unset(env_name);
}

/*------------------------------------------------------------------*
 *
 * ClStdConfigCommand
 *   Routine called when the 'config' command is entered. This routine calls the
 *   set method for any specified module.
 *
 * CALL:
 *   ClStdConfigCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdConfigCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdConfigCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  CStrParse parse(cargs);

  /*-------------------*/

  /* Get Module Name */

  std::string module;

  if (parse.isChar('@')) {
    parse.skipChar();

    parse.readNonSpace(module);

    parse.skipSpace();
  }
  else
    module = "builtin";

  /*-------------------*/

  /* Get Option Name */

  std::string option;

  parse.readNonSpace(option);

  parse.skipSpace();

  /*-------------------*/

  /* Get Additional Args */

  std::string args1 = parse.getAt();

  /*-------------------*/

  /* Call Module Set Method */

  bool flag = ClModuleMgrInst->execModuleSet(module, option, args1);

  if (! flag)
    lmgr->error("'config' failed for module '%s' option '%s' args '%s'", module.c_str(),
                option.c_str(), args1.c_str());
}

// Routine called when the 'rand' function is entered.
//
// The routine a random number (optionally seeded by any supplied value).

static ClParserValuePtr
ClStdRandCommand(ClParserValuePtr *values, uint num_values, void *, int *error_code)
{
  CL_LANGUAGE_TRACE("ClStdRandCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  *error_code = 0;

  ClParserValuePtr value;

  if (num_values > 1) {
    *error_code = int(ClErr::TOO_MANY_ARGUMENTS);
    return ClParserValuePtr();
  }

  if (num_values == 1) {
    long seed;

    if (! values[0]->integerValue(&seed)) {
      lmgr->expressionError(ClErr::INVALID_CONVERSION, "'rand'");
      return ClParserValuePtr();
    }

    COSRand::srand(seed);
  }

  long r = COSRand::rand();

  value = ClParserValueMgrInst->createValue(r);

  return value;
}

// Routine called when the 'rrand' function is entered.
//
// The routine a random number (optionally seeded by any supplied value).

static ClParserValuePtr
ClStdRRandCommand(ClParserValuePtr *values, uint num_values, void *, int *error_code)
{
  CL_LANGUAGE_TRACE("ClStdRRandCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  *error_code = 0;

  ClParserValuePtr value;

  if (num_values > 1) {
    *error_code = int(ClErr::TOO_MANY_ARGUMENTS);
    return ClParserValuePtr();
  }

  if (num_values == 1) {
    long seed;

    if (! values[0]->integerValue(&seed)) {
      lmgr->expressionError(ClErr::INVALID_CONVERSION, "'rrand'");
      return ClParserValuePtr();
    }

    COSRand::srand(seed);
  }

  double r = COSRand::randIn(0.0, 1.0);

  value = ClParserValueMgrInst->createValue(r);

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdIsVarCommand
 *   Routine called when the 'isvar' function is entered.
 *
 *   This routine returns whether a variable of the specified name has been
 *   defined or not.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdIsVarCommand(ClParserValuePtr *, uint, void *, int &error_code);
 *
 * INPUT:
 *   values     : The array of values supplied
 *              : when the function was called.
 *
 *   num_values : The number of values supplied
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
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static ClParserValuePtr
ClStdIsVarCommand(ClParserValuePtr *values, uint, void *, int *error_code)
{
  CL_LANGUAGE_TRACE("ClStdIsVarCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  *error_code = 0;

  std::string name;

  if (! values[0]->stringValue(name)) {
    lmgr->expressionError(ClErr::INVALID_CONVERSION, "'is_var'");
    return ClParserValuePtr();
  }

  bool flag = ClParserInst->isVariable(name);

  ClParserValuePtr value = ClParserValueMgrInst->createValue(static_cast<long>(flag));

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdUndefCommand
 *   Routine called when the 'undef' command is entered.
 *
 *   Undefines the associate procedure, function, structure type or variable
 *   associated with each of the list of specified identifiers.
 *
 * CALL:
 *   ClStdUndefCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdUndefCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  std::string     arg;
  ClLanguageFunc *function;
  ClLanguageProc *procedure;
  int             error_code;

  CL_LANGUAGE_TRACE("ClStdUndefCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Get Undef's Arguments */

  if (args->checkNumberOfArgs(1, -1) != 0)
    return;

  uint num_args = args->getNumArgs();

  /* Undef Each Argument */

  for (uint i = 1; i <= num_args; i++) {
    arg = args->getArg(i, &error_code);

    if (error_code != 0)
      return;

    if      (ClParserInst->isVariable(arg)) {
      ClParserVarPtr variable = ClParserInst->getVariable(arg);

      if (! variable->isNoDelete())
        ClParserInst->removeVariable(arg);
      else
        lmgr->syntaxError("variable '%s' cannot be deleted", arg.c_str());
    }
    else if (ClParserInst->isFunction(arg)) {
      ClParserInst->removeFunction(arg);
    }
    else if (ClParserInst->isInternFn(arg)) {
      lmgr->syntaxError("'undef' cannot undefine internal function '%s'", arg.c_str());
    }
    else if (ClParserInst->isType(arg)) {
      ClParserInst->deleteType(arg);
    }
    else if ((procedure = ClLanguageProcMgrInst->lookupProc(arg)) != 0) {
      if (procedure->canDelete())
        ClLanguageProcMgrInst->deleteProc(arg);
      else
        lmgr->syntaxError("'undef' procedure '%s' cannot be undefined", arg.c_str());
    }
    else if ((function = ClLanguageFuncMgrInst->lookupFunc(arg)) != 0) {
      ClParserInst->removeUserFn(arg);

      ClLanguageFuncMgrInst->deleteFunc(arg);
    }
    else
      lmgr->syntaxError("'undef' identifier '%s' is undefined", arg.c_str());
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdWhatisCommand
 *   Routine called when the 'whatis' command is entered.
 *
 *   Prints a description of the objects associated with each of the specified
 *   identifiers.
 *
 * CALL:
 *   ClStdWhatisCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdWhatisCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdWhatisCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Get Whatis's Arguments */

  if (args->checkNumberOfArgs(1, -1) != 0)
    return;

  uint num_args = args->getNumArgs();

  /* Output type of each Argument */

  for (uint i = 1; i <= num_args; i++) {
    int error_code;

    std::string arg = args->getArg(i, &error_code);

    if (error_code != 0)
      return;

    //bool found_proc = false;

    ClLanguageProc *procedure;

    if ((procedure = ClLanguageProcMgrInst->lookupProc(arg)) != 0) {
      lmgr->output("Procedure ");

      procedure->print();

      //found_proc = true;
    }

    ClParserScopePtr scope;

    bool end_name;

    ClLanguageCommandDef *command_def = lmgr->getCommandDef(scope, arg, &end_name);

    if      (command_def != 0) {
      if (command_def->getEndName() != "") {
        if (! end_name)
          lmgr->output("Start Block Command %s\n", arg.c_str());
        else
          lmgr->output("End Block Command %s\n", arg.c_str());
      }
      else
        lmgr->output("Command %s\n", arg.c_str());
    }
    else if (ClParserInst->isVariable(arg)) {
      ClParserValuePtr value = ClParserInst->getVariableValue(arg);

      if      (value->isReal())
        lmgr->output("real: ");
      else if (value->isInteger())
        lmgr->output("int: ");
      else if (value->isString())
        lmgr->output("string: ");
      else if (value->isList())
        lmgr->output("list: ");
      else if (value->isDictionary())
        lmgr->output("dictionary: ");
      else if (value->isStructure())
        lmgr->output("structure: ");
      else if (value->isRealArray())
        lmgr->output("real array: ");
      else if (value->isIntegerArray())
        lmgr->output("integer array: ");
      else if (value->isStringArray())
        lmgr->output("str array: ");
      else if (value->isListArray())
        lmgr->output("list array: ");
      else if (value->isDictionaryArray())
        lmgr->output("dictionary array: ");
      else if (value->isStructureArray())
        lmgr->output("structure array: ");

      lmgr->output("%s=", arg.c_str());

      value->print();

      lmgr->output("\n");
    }
    else if (ClParserInst->isFunction(arg)) {
      ClParserFuncPtr function = ClParserInst->getFunction(arg);

      lmgr->output("Function: ");

      function->print();

      lmgr->output("\n");
    }
    else if (ClParserInst->isInternFn(arg)) {
      lmgr->output("Internal Function: %s\n", arg.c_str());
    }
    else if (ClParserInst->isUserFn(arg)) {
      lmgr->output("User Function: %s\n", arg.c_str());
    }
    else if (ClParserInst->isType(arg)) {
      ClParserTypePtr type = ClParserInst->getType(arg);

      type->print();
    }
    else {
      ClParserExpr expr(arg);

      ClParserValuePtr value;

      if (! expr.exec(value)) {
        lmgr->output("Undefined: %s\n", arg.c_str());
        continue;
      }

      if      (value->isReal())
        lmgr->output("real");
      else if (value->isInteger())
        lmgr->output("int");
      else if (value->isString())
        lmgr->output("string");
      else if (value->isList())
        lmgr->output("list");
      else if (value->isDictionary())
        lmgr->output("dictionary");
      else if (value->isStructure())
        lmgr->output("structure");
      else if (value->isRealArray())
        lmgr->output("real array");
      else if (value->isIntegerArray())
        lmgr->output("integer array");
      else if (value->isStringArray())
        lmgr->output("str array");
      else if (value->isListArray())
        lmgr->output("list array");
      else if (value->isDictionaryArray())
        lmgr->output("dictionary array");
      else if (value->isStructureArray())
        lmgr->output("structure array");

      lmgr->output("\n");
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdStatusCommand
 *   Routine called when the 'list' command is entered.
 *
 *   Prints a list of all the currently defined objects of the specified type
 *   or all the currently defined objects of all types if no argument is supplied.
 *
 *   Types :-
 *     variables  : Variables
 *     procedures : Procedures and Functions
 *     structures : Structures/Types
 *     all        : All the above
 *
 * CALL:
 *   ClStdStatusCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdStatusCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdStatusCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const char *type;

  /* Check Args */

  if (args->checkNumberOfArgs(0, 1) != 0)
    return;

  /*--------------*/

  uint num = args->getNumArgs();

  if (num == 1) {
    if (args->getArgList(CLArgType::TEXT, &type, CLArgType::NONE) != 1)
      return;
  }
  else
    type = "all";

  /*--------------*/

  /* Print Types */

  if      (CStrUtil::casecmp(type, "variables" ) == 0)
    ClParserInst->printAllVariables();
  else if (CStrUtil::casecmp(type, "procedures") == 0) {
    ClParserInst->printAllFunctions();

    ClLanguageProcMgrInst->printProcs();
    ClLanguageFuncMgrInst->printFuncs();
  }
  else if (CStrUtil::casecmp(type, "structures") == 0)
    ClParserInst->printAllTypes();
  else if (CStrUtil::casecmp(type, "all") == 0) {
    lmgr->output("Variables :-\n\n");

    ClParserInst->printAllVariables();

    lmgr->output("\nProcedures :-\n\n");

    ClParserInst->printAllFunctions();

    ClLanguageProcMgrInst->printProcs();
    ClLanguageFuncMgrInst->printFuncs();

    lmgr->output("\nStructures :-\n\n");

    ClParserInst->printAllTypes();
  }
  else
    lmgr->syntaxError("Unknown type '%s' for 'list'", type);
}

/*------------------------------------------------------------------*
 *
 * ClStdHelpCommand
 *   Routine called when the 'help' command is entered.
 *
 *   This routine outputs help for the optional subject str using the current
 *   help procedure. By default this will display a list of language commands.
 *
 * CALL:
 *   ClStdHelpCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

static void
ClStdHelpCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdHelpCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  const std::string &cargs = command->getArgs();

  if (cargs == "") {
    lmgr->output("Help available for :-\n\n");

    StringVectorT modules;

    ClModuleMgrInst->getModuleList(modules);

    int num_modules = modules.size();

    for (int i = 0; i < num_modules; i++) {
      if (! ClModuleMgrInst->isModuleLoaded(modules[i]))
        continue;

      lmgr->output("  %s\n", modules[i].c_str());
    }

    lmgr->output("\n");

    return;
  }

  /*-------*/

  /* Get Subject and Args */

  std::string file;
  std::string subject;

  CStrParse parse(cargs);

  if (parse.isChar('@')) {
    parse.skipChar();

    parse.readNonSpace(file);

    parse.skipSpace();
  }
  else
    file = "";

  parse.readNonSpace(subject);

  parse.skipSpace();

  std::string args1 = parse.getAt();

  /*-------*/

  FILE *fp     = nullptr;
  FILE *old_fp = nullptr;

  /*-------*/

  if (file != "") {
    fp = fopen(file.c_str(), "w");

    if (fp != 0)
      old_fp = lmgr->setOutputFp(fp);
    else
      lmgr->error("invalid file '%s'", file.c_str());
  }

  /*-------*/

  /* Display Module Help (if any) */

  if (ClModuleMgrInst->execModuleHelp(subject, args1))
    goto done;

  /*-------*/

  /* Print Out Help using current Help Procedure */

  {
    ClLanguageHelpProc help_proc = lmgr->getHelpProc();

    if (help_proc != 0) {
      (*help_proc)(subject.c_str(), args1.c_str());
      goto done;
    }
  }

  /*-------*/

  lmgr->error("No help for '%s'", subject.c_str());

  /*-------*/

 done:
  if (file != "" && fp != 0)
    lmgr->setOutputFp(old_fp);
}

/*------------------------------------------------------------------*
 *
 * ClStdExitCommand
 *   Routine called when the 'exit' command is entered.
 *
 *   This routine sets a flag to indicate that the run of the current file or
 *   command line interface should be terminated.
 *
 * CALL:
 *   ClStdExitCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * NOTES:
 *   The actual exit is handled by the parent routine
 *   which is running the command.
 *
 *------------------------------------------------------------------*/

static void
ClStdExitCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdExitCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Check Args */

  if (args->checkNumberOfArgs(0, 1) != 0)
    return;

  uint num = args->getNumArgs();

  long code = 0;

  if (num == 1) {
    if (args->getArgList(CLArgType::INTEGER, &code, CLArgType::NONE) != 1)
      return;
  }

  /* Set Exit Flag and Exit Code */

  lmgr->setExitFlag(true);

  lmgr->setExitCode(code);
}

static void
ClStdDieCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdDieCommand");

  /* Check Args */

  if (args->checkNumberOfArgs(0, 1) != 0)
    return;

  uint num = args->getNumArgs();

  long code = 0;

  if (num == 1) {
    if (args->getArgList(CLArgType::INTEGER, &code, CLArgType::NONE) != 1)
      return;
  }

  exit(code);
}

#ifdef CEIL_READLINE
static void
ClStdHistoryCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdHistoryCommand");

  ClLanguageMgr *lmgr = ClLanguageMgrInst;

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  std::vector<CReadLineHistoryEntry> entries;

  lmgr->getReadLine().getHistoryEntries(entries);

  int num_entries = entries.size();

  for (int i = 0; i < num_entries; ++i)
    lmgr->output("%d: %s\n", entries[i].line_num, entries[i].line.c_str());
}
#endif

extern bool
ClIsImportCommand(ClLanguageCommand *command)
{
  return command->isIdent(language_import_ident);
}

extern bool
ClIsRetAllCommand(ClLanguageCommand *command)
{
  return command->isIdent(language_retall_ident);
}
