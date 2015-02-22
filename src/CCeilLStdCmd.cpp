#include <CCeilLI.h>
#include <COSProcess.h>
#include <COSRand.h>
#include <CFile.h>
#include <CEnv.h>

#include <csignal>
#include <algorithm>

using std::string;
using std::vector;

#define LOUT(a) ClLanguageMgrInst->output(a)

typedef vector<ClLanguageProcArg *> ClLanguageProcArgArray;
typedef vector<ClLanguageFuncArg *> ClLanguageFuncArgArray;

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
static void             ClStdGetEnvCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdSetEnvCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdUnSetEnvCommand
                         (ClLanguageCommand *, ClLanguageArgs *, void *);
static void             ClStdSetCommand
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
  { ""    , 0                               , },
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
 *        :    First Time Initialisation, Commands and
 *        :    Variables are defined.
 *        :   CL_MODULE_INIT_TYPE_REINIT -
 *        :    Reinitialisation, Variables are redefined.
 *
 * OUTPUT:
 *   None
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

  if (flag != CL_MODULE_INIT_TYPE_REINIT) {
    language_import_ident =
      ClLanguageMgrInst->defineCommand("import", ClStdImportCommand);

    /******/

    ClLanguageMgrInst->defineCommand("new", ClStdNewCommand);
    ClLanguageMgrInst->defineCommand("run", ClStdRunCommand);

    /******/

    /* Procedures and Functions */

    ClLanguageMgrInst->defineBlockCommand("proc", "",
      CL_BLOCK_TYPE_PROCEDURE_BLOCK, ClStdProcCommand);

    ClLanguageMgrInst->defineBlockCommand("func", "",
      CL_BLOCK_TYPE_PROCEDURE_BLOCK, ClStdFuncCommand);

    /******/

    /* Program Flow/Iteration Statements */

    ClLanguageMgrInst->defineBlockCommand("for", "",
      CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK, ClStdForCommand);

    ClLanguageMgrInst->defineBlockCommand("foreach", "",
      CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK, ClStdForEachCommand);

    ClLanguageMgrInst->defineBlockCommand("while", "",
      CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK, ClStdWhileCommand);

    ClLanguageMgrInst->defineBlockCommand("until", "",
      CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK, ClStdUntilCommand);

    ClLanguageMgrInst->defineBlockCommand("repeat", "",
      CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK, ClStdRepeatCommand);

    ClLanguageMgrInst->defineBlockCommand("if", "",
      CL_BLOCK_TYPE_NORMAL_NO_ARGS_BLOCK, ClStdIfCommand);

    language_elseif_ident =
      ClLanguageMgrInst->defineCommand("elseif", ClStdElseIfCommand);

    language_else_ident =
      ClLanguageMgrInst->defineCommand("else"  , ClStdElseCommand  );

    /******/

    /* Program Flow Statements */

    ClLanguageMgrInst->defineCommand("goto"    , ClStdGotoCommand    );
    ClLanguageMgrInst->defineCommand("break"   , ClStdBreakCommand   );
    ClLanguageMgrInst->defineCommand("continue", ClStdContinueCommand);
    ClLanguageMgrInst->defineCommand("return"  , ClStdReturnCommand  );

    /******/

    /* Signal Handling Statements */

    ClLanguageMgrInst->defineCommand("on"   , ClStdOnCommand   );
    ClLanguageMgrInst->defineCommand("raise", ClStdRaiseCommand);

    /******/

    /* Definition Statements */

    ClLanguageMgrInst->defineCommand("define", ClStdDefineCommand);
    ClLanguageMgrInst->defineCommand("struct", ClStdStructCommand);

    /******/

    /* Input/Output Statements */

    ClLanguageMgrInst->defineCommand("input", ClStdInputCommand);
    ClLanguageMgrInst->defineCommand("print", ClStdPrintCommand);

    /******/

    /* Format Function and Scan Command */

    num_arg_types = 2;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;
    arg_types[1] = CL_PARSER_VALUE_TYPE_INTEGER |
                   CL_PARSER_VALUE_TYPE_REAL |
                   CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("format", CL_PARSER_USERFN_TYPE_VARARGS,
                               arg_types, num_arg_types, ClStdFormatCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      ClLanguageMgrInst->expressionError
       (ClParserUserFn::getErrorCode(), "creating 'format' user function");
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
      ClLanguageMgrInst->expressionError
       (ClParserUserFn::getErrorCode(), "creating 'scan' user function");
      return false;
    }

    /******/

    /* System Access Command */

    num_arg_types = 1;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("exec", CL_PARSER_USERFN_TYPE_STD,
                               arg_types, num_arg_types, ClStdExecCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      ClLanguageMgrInst->expressionError
       (ClParserUserFn::getErrorCode(), "creating 'exec' user function");
      return false;
    }

    /******/

    /* Execute Expression String */

    num_arg_types = 1;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("parse", CL_PARSER_USERFN_TYPE_STD,
                               arg_types, num_arg_types, ClStdParseCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      ClLanguageMgrInst->expressionError
       (ClParserUserFn::getErrorCode(), "creating 'parse' user function");
      return false;
    }

    /******/

    /* Environment Variables */

    ClLanguageMgrInst->defineCommand("getenv"  , ClStdGetEnvCommand  );
    ClLanguageMgrInst->defineCommand("setenv"  , ClStdSetEnvCommand  );
    ClLanguageMgrInst->defineCommand("unsetenv", ClStdUnSetEnvCommand);

    /******/

    /* Random Numbers  */

    num_arg_types = 0;

    ClParserInst->createUserFn("rand", CL_PARSER_USERFN_TYPE_VARARGS,
                               arg_types, num_arg_types, ClStdRandCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      ClLanguageMgrInst->expressionError
       (ClParserUserFn::getErrorCode(), "creating 'rand' user function");
      return false;
    }

    num_arg_types = 0;

    ClParserInst->createUserFn("rrand", CL_PARSER_USERFN_TYPE_VARARGS,
                               arg_types, num_arg_types, ClStdRRandCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      ClLanguageMgrInst->expressionError
       (ClParserUserFn::getErrorCode(), "creating 'rrand' user function");
      return false;
    }

    /******/

    /* Set */

    ClLanguageMgrInst->defineCommand("set", ClStdSetCommand);

    /******/

    /* Control Commands */

    num_arg_types = 1;
    arg_types[0] = CL_PARSER_VALUE_TYPE_STRING;

    ClParserInst->createUserFn("isvar", CL_PARSER_USERFN_TYPE_STD,
                               arg_types, num_arg_types, ClStdIsVarCommand, 0);

    if (ClParserUserFn::getErrorCode() != 0) {
      ClLanguageMgrInst->expressionError
       (ClParserUserFn::getErrorCode(), "creating 'isvar' user function");
      return false;
    }

    ClLanguageMgrInst->defineCommand("undef" , ClStdUndefCommand );
    ClLanguageMgrInst->defineCommand("whatis", ClStdWhatisCommand);

    /******/

    /* Enquiry Commands */

    ClLanguageMgrInst->defineCommand("help"  , ClStdHelpCommand);
    ClLanguageMgrInst->defineCommand("status", ClStdStatusCommand);

    /******/

    language_retall_ident =
       ClLanguageMgrInst->defineCommand("retall", (ClLanguageCmdProc) 0);

    /******/

    ClLanguageMgrInst->defineCommand("exit", ClStdExitCommand);

    ClLanguageMgrInst->defineCommand("die", ClStdDieCommand);

    /***********/

  #ifdef CEIL_READLINE
    ClLanguageMgrInst->defineCommand("history", ClStdHistoryCommand);
  #endif
  }

  return true;
}

/*------------------------------------------------------------------*
 *
 * ClStdCommandsSet
 *   The 'set' method for the 'builtin' module.
 *
 *   This routine allows the environment to be customised
 *   using the specified option with any arguments this
 *   requries :-
 *
 *   Options
 *   -------
 *
 *   angle_type <radians|degrees> : Set Angle Type to
 *                                : Degrees/Radians
 *   prompt <string>              : Set Prompt String
 *   real_format <string>         : Set Real Format String
 *   integer_format <string>      : Set Integer Format String
 *   string_format <string>       : Set String Format String
 *   math_fail <yes|no>           : Set whether a math failure
 *                                : causes an error or a NaN value.
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
 * OUTPUT:
 *   None
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
  if (option[0] == '\0') {
    CAngleType::Type angle_type = ClParserInst->getAngleType();
    string           prompt     = ClLanguageMgrInst->getPrompt();
    bool             math_fail  = ClParserInst->getMathFail();

    ClLanguageMgrInst->output("angle_type     %s\n"  , CAngleType::toString(angle_type).c_str());
    ClLanguageMgrInst->output("prompt         '%s'\n", prompt.c_str());
    ClLanguageMgrInst->output("real_format    '%s'\n", ClParserInst->getRealFormat().c_str());
    ClLanguageMgrInst->output("integer_format '%s'\n", ClParserInst->getIntegerFormat().c_str());
    ClLanguageMgrInst->output("string_format  '%s'\n", ClParserInst->getStringFormat().c_str());
    ClLanguageMgrInst->output("math_fail      %s\n"  , CStrUtil::toString(math_fail).c_str());

    return true;
  }

  if      (CStrUtil::casecmp(option, "angle_type") == 0) {
    /* Set Angle Type degrees/radians */

    CAngleType::Type angle_type = CAngleType::toAngleType(args);

    if (angle_type != CAngleType::NONE)
      ClParserInst->setAngleType(angle_type);
    else
      ClLanguageMgrInst->error("invalid angle type '%s' for 'set %s'", args, option);
  }
  else if (CStrUtil::casecmp(option, "prompt") == 0) {
    char *prompt;

    ClLanguageArgs *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args,
                                CL_ARG_TYPE_STRING, &prompt,
                                CL_ARG_TYPE_NONE) != 1)
      return false;

    /* Set Prompt */

    ClLanguageMgrInst->setPrompt(prompt);

    args1->termArgs();

    delete args1;
  }
  else if (CStrUtil::casecmp(option, "real_format") == 0) {
    char   *format;
    string  real_format;

    bool rc = false;

    ClLanguageArgs *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args,
                                CL_ARG_TYPE_STRING, &format,
                                CL_ARG_TYPE_NONE) == 1) {
      CStrParse parse(format);

      if (parse.readRealFormat(real_format)) {
        parse.skipSpace();

        rc = parse.eof();
      }
    }

    if (rc)
      ClParserInst->setRealFormat(real_format);
    else
      ClLanguageMgrInst->error("invalid format '%s' for 'set %s'", format, option);

    args1->termArgs();

    delete args1;

    return rc;
  }
  else if (CStrUtil::casecmp(option, "integer_format") == 0) {
    char   *format;
    string  integer_format;

    bool rc = false;

    ClLanguageArgs *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args,
                                CL_ARG_TYPE_STRING, &format,
                                CL_ARG_TYPE_NONE) == 1) {
      CStrParse parse(format);

      if (parse.readIntegerFormat(integer_format)) {
        parse.skipSpace();

        rc = parse.eof();
      }
    }

    if (rc)
      ClParserInst->setIntegerFormat(integer_format);
    else
      ClLanguageMgrInst->error("invalid format '%s' for 'set %s'", format, option);

    args1->termArgs();

    delete args1;

    return rc;
  }
  else if (CStrUtil::casecmp(option, "string_format") == 0) {
    char   *format;
    string  string_format;

    bool rc = false;

    ClLanguageArgs *args1 = new ClLanguageArgs;

    args1->startArgs(0);

    if (args1->getStringArgList(args,
                                CL_ARG_TYPE_STRING, &format,
                                CL_ARG_TYPE_NONE) == 1) {
      CStrParse parse(format);

      if (parse.readStringFormat(string_format)) {
        parse.skipSpace();

        rc = parse.eof();
      }
    }

    if (rc)
      ClParserInst->setStringFormat(string_format);
    else
      ClLanguageMgrInst->error("invalid format '%s' for 'set %s'", format, option);

    args1->termArgs();

    return rc;
  }
  else if (CStrUtil::casecmp(option, "math_fail") == 0) {
    /* Set Math Fail True/False */

    if (CStrUtil::isBool(args))
      ClParserInst->setMathFail(CStrUtil::toBool(args));
    else
      ClLanguageMgrInst->error("invalid args '%s' for 'set %s'", args, option);
  }
  else if (CStrUtil::casecmp(option, "debug") == 0) {
    int level;

    /* Get Debug Level */

    if (! CStrUtil::toInteger(args, &level)) {
      ClLanguageMgrInst->error("invalid debug level '%s' for 'set %s'", args, option);
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
 *   bool flag =
 *     ClStdCommandsHelp(const char *args);
 *
 * INPUT:
 *   args : Additional Arguments passed to the Help Command.
 *
 * OUTPUT:
 *   None
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
 *   The routine imports the commands and variables for
 *   the named module or lists the available modules
 *   and their status if no module is defined.
 *
 * CALL:
 *   ClStdImportCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdImportCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  char *name;

  if (args->checkNumberOfArgs(0, 1) != 0)
    return;

  uint num = args->getNumArgs();

  if (num == 1) {
    bool flag;

    if (args->getArgList(CL_ARG_TYPE_TEXT, &name,
                         CL_ARG_TYPE_NONE) != 1)
      return;

    flag = ClModuleMgrInst->importModule(name);

    if (! flag)
      ClLanguageMgrInst->error("Failed to Import Module '%s'", name);
  }
  else {
    StringVectorT modules;

    ClModuleMgrInst->getModuleList(modules);

    int num_modules = modules.size();

    for (int i = 0; i < num_modules; i++) {
      ClLanguageMgrInst->output("%s", modules[i].c_str());

      if (ClModuleMgrInst->isModuleLoaded(modules[i]))
        ClLanguageMgrInst->output(" (loaded)");

      ClLanguageMgrInst->output("\n");
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdNewCommand
 *   Routine called when the 'new' command is entered.
 *
 *   The routine ensures no arguments are entered and
 *   then deletes all variables, structures, procedures
 *   and functions. The standard variables, structures,
 *   procedures and functions are then re-added.
 *
 * CALL:
 *   ClStdNewCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdNewCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdNewCommand");

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Reinitialise */

  ClLanguageMgrInst->reinit();
}

/*------------------------------------------------------------------*
 *
 * ClStdRunCommand
 *   Routine called when the 'run' command is entered.
 *
 *   The routine ensures gets the comma separated list of
 *   strings after the command and treats each as the
 *   name of a file whose contents are language commands
 *   to be run.
 *
 *   Each file is run in turn. If a specified file does
 *   not exist then it is ignored.
 *
 * CALL:
 *   ClStdRunCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdRunCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  string        file_name;
  int           error_code;
  StringVectorT file_names;

  CL_LANGUAGE_TRACE("ClStdRunCommand");

  /* Get Run's Arguments */

  if (args->checkNumberOfArgs(1, -1) != 0)
    return;

  uint num_args = args->getNumArgs();

  /* Get the list of File Names to Run */

  for (uint i = 1; i <= num_args; i++) {
    file_name = args->getStringArg(i, &error_code);

    if (error_code != 0)
      return;

    file_names.push_back(file_name);
  }

  /* Run the file specified by each argument */

  for (uint i = 0; i < num_args; i++) {
    if (! CFile::isRegular(file_names[i])) {
      ClLanguageMgrInst->error("'%s' is not a regular file", file_names[i].c_str());
      continue;
    }

    ClLanguageMgrInst->runFile(file_names[i]);
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdProcCommand
 *   Routine called when a 'proc ... endproc' block command
 *   is entered.
 *
 *   The name following the 'proc' is used as the name of
 *   the procedure and the subsequent round bracketed comma
 *   separated list of argument names is extracted and those
 *   starting with the '&' character are flagged as return
 *   variables. The list of language commands enclosed by the
 *   'proc ... endproc' pair are stored for later execution
 *   when the procedure name is specified as a command.
 *
 * CALL:
 *   ClStdProcCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command and
 *           : arguments supplied by the user.
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

static void
ClStdProcCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  int                     i;
  int                     num_args;
  ClLanguageArgList       arg_list;
  ClLanguageProcArg      *proc_arg;
  ClLanguageProcArgArray  proc_args;
  string                  arg_string;
  string                  name_string;
  ClParserFuncPtr         old_pfunction;
  ClLanguageFunc         *old_lfunction;
  ClLanguageProc         *old_procedure;

  CL_LANGUAGE_TRACE("ClStdProcCommand");

  const string &cargs = command->getArgs();

  const ClLanguageCommandList &command_list = command->getCommandList();

  /* Extract Procedure Name */

  CStrParse parse(cargs);

  if (! parse.readIdentifier(name_string)) {
    ClLanguageMgrInst->syntaxError
     ("'proc' procedure name must start with alphabetic or '_'");
    goto fail;
  }

  /* Get Existing Procedure and Function */

  old_procedure = ClLanguageProcMgrInst->lookupProc(name_string);
  old_lfunction = ClLanguageFuncMgrInst->lookupFunc(name_string);
  old_pfunction = ClParserInst->getFunction(name_string);

  /* Ensure Procedure Can be Deleted */

  if (old_procedure != 0 && ! old_procedure->canDelete()) {
    ClLanguageMgrInst->syntaxError
     ("'proc' Existing procedure '%s' cannot be deleted", name_string.c_str());
    goto fail;
  }

  /* Extract Argument List */

  parse.skipSpace();

  if (! parse.isChar('(')) {
    ClLanguageMgrInst->syntaxError
     ("missing open brackets for 'proc' '%s' argument list", name_string.c_str());
    goto fail;
  }

  parse.skipChar();

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ')', arg_string)) {
    ClLanguageMgrInst->syntaxError
     ("missing close brackets for 'proc' '%s' argument list", name_string.c_str());
    goto fail;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' after 'proc' '%s' argument list",
      parse.getAt().c_str(), name_string.c_str());
    goto fail;
  }

  /* Get List of Procedure's Arguments */

  args->stringToArgList(arg_string, &arg_list);

  num_args = arg_list.size();

  for (i = 0; i < num_args; i++) {
    if (arg_list[i][0] == '&')
      proc_arg = new ClLanguageProcArg(arg_list[i].substr(1), true);
    else
      proc_arg = new ClLanguageProcArg(arg_list[i], false);

    if (! ClParserVar::isValidName(proc_arg->getName())) {
      ClLanguageMgrInst->syntaxError
       ("invalid 'proc' '%s' argument name %d - '%s'",
        name_string.c_str(), i + 1, proc_arg->getName().c_str());

      delete proc_arg;

      goto fail;
    }

    proc_args.push_back(proc_arg);
  }

  /***********/

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

  /* Add Procedure to List */

  static_cast<void>(ClLanguageProcMgrInst->
    createProc(name_string, proc_args, command_list));

  command->clearCommands();

 fail:
  std::for_each(proc_args.begin(), proc_args.end(), CDeletePointer());
}

/*------------------------------------------------------------------*
 *
 * ClStdFuncCommand
 *   Routine called when a 'func ... endfunc' block command
 *   is entered.
 *
 *   The name following the 'func' is used as the name of
 *   the procedure and the subsequent round bracketed comma
 *   separated list of argument names and types is extracted.
 *   The list of language commands enclosed by the
 *   'func ... endfunc' pair are stored for later execution
 *   when the function name is specified in an expression.
 *
 * CALL:
 *   ClStdFuncCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   Only the int, real and str types are currently supported.
 *
 *------------------------------------------------------------------*/

static void
ClStdFuncCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  int                     i;
  int                     num_args;
  ClLanguageArgList       arg_list;
  ClLanguageFunc         *function;
  ClLanguageFuncArg      *func_arg;
  IntVectorT              arg_types;
  ClLanguageFuncArgArray  func_args;
  string                  arg_string;
  string                  name_string;
  ClParserFuncPtr         old_pfunction;
  ClLanguageFunc         *old_lfunction;
  ClLanguageProc         *old_procedure;

  CL_LANGUAGE_TRACE("ClStdFuncCommand");

  const string &cargs = command->getArgs();

  const ClLanguageCommandList &command_list = command->getCommandList();

  /* Extract Function Name */

  CStrParse parse(cargs);

  if (! parse.readIdentifier(name_string)) {
    ClLanguageMgrInst->syntaxError
     ("'func' function name must start with alphabetic or '_'");
    goto fail;
  }

  /* Get Existing Procedure and Function */

  old_procedure = ClLanguageProcMgrInst->lookupProc(name_string);
  old_lfunction = ClLanguageFuncMgrInst->lookupFunc(name_string);
  old_pfunction = ClParserInst->getFunction(name_string);

  /* Ensure Procedure Can be Deleted */

  if (old_procedure != 0 && ! old_procedure->canDelete()) {
    ClLanguageMgrInst->syntaxError
     ("'func' Existing procedure '%s' cannot be deleted", name_string.c_str());
    goto fail;
  }

  /* Extract Argument List */

  parse.skipSpace();

  if (! parse.isChar('(')) {
    ClLanguageMgrInst->syntaxError
     ("missing open brackets for 'func' '%s' argument list", name_string.c_str());
    goto fail;
  }

  parse.skipChar();

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ')', arg_string)) {
    ClLanguageMgrInst->syntaxError
     ("missing close brackets for 'func' '%s' argument list", name_string.c_str());
    goto fail;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' after 'func' '%s' argument list",
      parse.getAt().c_str(), name_string.c_str());
    goto fail;
  }

  /* Get List of Function's Arguments */

  args->stringToArgList(arg_string, &arg_list);

  num_args = arg_list.size();

  for (i = 0; i < num_args; i++) {
    string name = arg_list[i];

    //uint len = name.size();

    StringVectorT words;

    CStrUtil::addWords(name, words);

    int num_words = words.size();

    if (num_words != 1 && num_words != 2) {
      ClLanguageMgrInst->syntaxError
       ("invalid 'func' '%s' argument name %d", name_string.c_str(), i + 1, name.c_str());
    }

    ClParserValueType type;

    if      (words[0] == "int")
      type = CL_PARSER_VALUE_TYPE_INTEGER;
    else if (words[0] == "real")
      type = CL_PARSER_VALUE_TYPE_REAL;
    else if (words[0] == "string")
      type = CL_PARSER_VALUE_TYPE_STRING;
    else if (words[0] == "array")
      type = CL_PARSER_VALUE_TYPE_ARRAY;
    else {
      ClLanguageMgrInst->syntaxError
       ("invalid 'func' '%s' argument type %d - '%s'", name_string.c_str(), i + 1, name.c_str());

      goto fail;
    }

    if (num_words > 1 && words[1][0] == '&') {
      type = (ClParserValueType) (type | CL_PARSER_VALUE_TYPE_OUTPUT);

      words[1] = words[1].substr(1);
    }
    else
      type = (ClParserValueType) (type | CL_PARSER_VALUE_TYPE_INPUT);

    if (words[1].size() == 0 || ! ClParserVar::isValidName(words[1])) {
      if (words[1].size() == 0)
        ClLanguageMgrInst->syntaxError
         ("missing 'func' '%s' argument name %d", name_string.c_str(), i + 1);
      else
        ClLanguageMgrInst->syntaxError
         ("invalid 'func' '%s' argument name %d - '%s'",
          name_string.c_str(), i + 1, name.c_str());

      goto fail;
    }

    name = words[1];

    func_arg = new ClLanguageFuncArg(name, type);

    func_args.push_back(func_arg);
  }

  /***********/

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

  function = ClLanguageFuncMgrInst->
    createFunc(name_string, func_args, command_list);

  command->clearCommands();

  /* Define Function to Parser */

  for (uint i = 0; i < function->getNumArgs(); i++)
    arg_types.push_back(function->getArg(i).getType());

  ClParserInst->createUserFn(function->getName(),
                             CL_PARSER_USERFN_TYPE_STD, arg_types,
                             ClLanguageRunFunction, (void *) function);

 fail:
  std::for_each(func_args.begin(), func_args.end(), CDeletePointer());
}

/*------------------------------------------------------------------*
 *
 * ClStdForCommand
 *   Routine called when a 'for ... endfor' block command
 *   is entered.
 *
 *   The semi-colon separated expressions contained in
 *   round brackets are used as the initial, while and
 *   iterate expressions.
 *
 *   The initial expression is evaluated and the list of
 *   language commands enclosed by 'for ... endfor' pair are
 *   then continually executed followed by the iterate
 *   expression while the while expression evaluates to a
 *   non-zero integer.
 *
 *   Any break commands exit the loop, any continue commands
 *   skip any remaining loop commands, and any gotos which
 *   jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdForCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdForCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  bool             flag;
  ClParserValuePtr value;
  string           for_expr1;
  string           for_expr2;
  string           for_expr3;

  CL_LANGUAGE_TRACE("ClStdForCommand");

  const string &cargs = command->getArgs();

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
    ClLanguageMgrInst->syntaxError
     ("missing open brackets for 'for' expression list");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int i = parse.getPos();

  if (! args->readArgList(cargs, &i, ';', for_expr1)) {
    ClLanguageMgrInst->syntaxError
     ("missing condition expression in 'for' expression list");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ';', for_expr2)) {
    ClLanguageMgrInst->syntaxError
     ("missing condition expression in 'for' expression list");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ')', for_expr3)) {
    ClLanguageMgrInst->syntaxError
     ("missing iteration expression in 'for' expression list");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' after 'for' expression list", parse.getAt().c_str());
    return;
  }

  /*************/

  /* Process Init Expression */

  ClParserExpr expr1(for_expr1);

  if (! expr1.exec(value)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
      "'for' expression 1 - '%s'", for_expr1.c_str());
    return;
  }
  else if (! value.isValid()) {
    ClLanguageMgrInst->syntaxError
     ("undefined 'for' expression 1 - '%s'", for_expr1.c_str());
    return;
  }

  /*************/

  /* Process While Expression and Check Result */

  ClParserExpr expr2(for_expr2);

  if      (! expr2.exec(value)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
      "'for' expression 2 - '%s'", for_expr2.c_str());
    return;
  }
  else if (! value.isValid()) {
    ClLanguageMgrInst->syntaxError
     ("undefined 'for' expression 2 - '%s'", for_expr2.c_str());
    return;
  }

  flag = value->toBool();

  /*************/

  /*
   * While Expression is True :-
   *  Process Command List and Iterate Expression
   */

  while (flag) {
    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      return;

    /************/

    /* Process Iterate Expression */

    ClParserExpr expr3(for_expr3);

    if      (! expr3.exec(value)) {
      ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
        "'for' expression 3 - '%s'", for_expr3.c_str());
      return;
    }
    else if (! value.isValid()) {
      ClLanguageMgrInst->syntaxError
       ("undefined 'for' expression 3 - '%s'", for_expr3.c_str());
      return;
    }

    /*************/

    /* Process While Expression and Check Result */

    if      (! expr2.exec(value)) {
      ClLanguageMgrInst->expressionError
       (CLERR_INVALID_EXPRESSION, "'for' expression 2 - '%s'", for_expr2.c_str());
      return;
    }
    else if (! value.isValid()) {
      ClLanguageMgrInst->syntaxError
       ("undefined 'for' expression 2 - '%s'", for_expr2.c_str());
      return;
    }

    flag = value->toBool();
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdForEachCommand
 *   Routine called when a 'foreach ... endforeach' block
 *   command is entered.
 *
 *   The 'foreach' str should be followed by the name of
 *   the variable to store the iteration value and a
 *   bracketed expression which will evaluate to the array
 *   to be iterated.
 *
 *   The expression is evaluated and the list of language
 *   commands enclosed by the 'foreach ... endforeach' pair
 *   are executed for each value of the resltant array.
 *
 *   Any break commands exit the loop, any continue commands
 *   skip any remaining loop commands, and any gotos which
 *   jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdForEachCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdForEachCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdForEachCommand");

  const string &cargs = command->getArgs();

  /* Get Variable Name */

  CStrParse parse(cargs);

  parse.skipSpace();

  bool lvalue = false;

  if (parse.isChar('&')) {
    parse.skipChar();

    parse.skipSpace();

    lvalue = true;
  }

  string var_name;

  parse.readNonSpace(var_name);

  if (! ClParserVar::isValidName(var_name)) {
    ClLanguageMgrInst->syntaxError
      ("invalid 'foreach' variable name '%s'", var_name.c_str());
    return;
  }

  /* Get Expression */

  parse.skipSpace();

  if (! parse.isChar('(')) {
    ClLanguageMgrInst->syntaxError
      ("missing open brackets for 'foreach' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  int i = parse.getPos();

  string arg_string;

  if (! args->readArgList(cargs, &i, ')', arg_string)) {
    ClLanguageMgrInst->syntaxError
      ("missing close brackets for 'foreach' expression");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
      ("spurious characters '%s' after 'foreach' expression",
       parse.getAt().c_str());
    return;
  }

  ClLanguageArgList arg_list;

  args->stringToArgList(arg_string, &arg_list);

  uint num_args = arg_list.size();

  if (num_args != 1) {
    ClLanguageMgrInst->syntaxError
      ("'foreach' requires a single expression - %d found", num_args);
    return;
  }

  string expression = arg_list[0];

  /*************/

  /* Process Expression */

  ClParserExpr expr(expression);

  ClParserValuePtr value;

  if      (! expr.exec(value)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
      "'foreach' expression 1 - '%s'", expression.c_str());
    return;
  }
  else if (! value.isValid()) {
    ClLanguageMgrInst->syntaxError
      ("undefined 'foreach' expression 1 - '%s'", expression.c_str());
    return;
  }

  /*************/

  ClParserValueArray values;

  value->toSubValues(values);

  uint num_values = values.size();

  /*************/

  /* Set Variable to each Array Value and Process Commands */

  for (uint i = 0; i < num_values; i++) {
    /* Set Variable Value */

    ClParserVarPtr variable =
      ClParserInst->createVar(var_name, values[i]);

    /************/

    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      return;

    /************/

    if (lvalue) {
      int error_code;

      ClParserInst->assignSubscriptValue(expression, i + 1,
                                         variable->getValue(), &error_code);

      if (error_code != 0) {
        ClLanguageMgrInst->expressionError(error_code,
          "'foreach' failed to update value '%s'", expression.c_str());
        return;
      }
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdWhileCommand
 *   Routine called when a 'while ... endwhile' block command
 *   is entered.
 *
 *   The expression contained in round brackets is extracted
 *   and the list of language commands enclosed by the
 *   'while ... endwhile' pair are continually executed
 *   while the expression evaluates to a non-zero integer.
 *
 *   Any break commands exit the loop, any continue commands
 *   skip any remaining loop commands, and any gotos which
 *   jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdWhileCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdWhileCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  int               i;
  bool              flag;
  string            line;
  ClParserValuePtr  value;
  int               num_args;
  ClLanguageArgList arg_list;
  string            expression;

  CL_LANGUAGE_TRACE("ClStdWhileCommand");

  const string &cargs = command->getArgs();

  /* Extract While Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    ClLanguageMgrInst->syntaxError("missing open brackets for 'while' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ')', line)) {
    ClLanguageMgrInst->syntaxError("missing close brackets for 'while' expression");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError("spurious characters '%s' after 'while' expression",
                                   parse.getAt().c_str());
    return;
  }

  args->stringToArgList(line, &arg_list);

  num_args = arg_list.size();

  if (num_args != 1) {
    ClLanguageMgrInst->syntaxError("'while' requires a single expression - %d found", num_args);
    return;
  }

  expression = arg_list[0];

  /*************/

  do {
    /* Process Expression and Check Result */

    ClParserExpr expr(expression);

    if      (! expr.exec(value)) {
      ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION, "'while' expression - '%s'",
                                         expression.c_str());
      return;
    }
    else if (! value.isValid()) {
      ClLanguageMgrInst->syntaxError("undefined 'while' expression - '%s'", expression.c_str());
      return;
    }

    flag = value->toBool();

    /*************/

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
 *   Routine called when a 'until ... enduntil' block command
 *   is entered.
 *
 *   The expression contained in round brackets is extracted
 *   and the list of language commands enclosed by the
 *   'until ... enduntil' pair are continually executed
 *   until the expression evaluates to a zero integer.
 *
 *   Any break commands exit the loop, any continue commands
 *   skip any remaining loop commands, and any gotos which
 *   jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdUntilCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdUntilCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  int                i;
  bool               flag;
  string             line;
  ClParserValuePtr value;
  int                num_args;
  ClLanguageArgList  arg_list;
  string             expression;

  CL_LANGUAGE_TRACE("ClStdUntilCommand");

  const string &cargs = command->getArgs();

  /* Extract Until Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    ClLanguageMgrInst->syntaxError
     ("missing open brackets for 'until' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ')', line)) {
    ClLanguageMgrInst->syntaxError
     ("missing close brackets for 'until' expression");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' after 'until' expression",
      parse.getAt().c_str());
    return;
  }

  args->stringToArgList(line, &arg_list);

  num_args = arg_list.size();

  if (num_args != 1) {
    ClLanguageMgrInst->syntaxError
     ("'until' requires a single expression - %d found", num_args);
    return;
  }

  expression = arg_list[0];

  /*************/

  do {
    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      return;

    /*************/

    /* Process Expression and Check Result */

    ClParserExpr expr(expression);

    if      (! expr.exec(value)) {
      ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
        "'until' expression - '%s'", expression.c_str());
      return;
    }
    else if (! value.isValid()) {
      ClLanguageMgrInst->syntaxError
       ("undefined 'until' expression - '%s'", expression.c_str());
      return;
    }

    flag = value->toBool();
  }
  while (! flag);
}

/*------------------------------------------------------------------*
 *
 * ClStdRepeatCommand
 *   Routine called when a 'repeat ... endrepeat' block
 *   command is entered.
 *
 *   The expression contained in round brackets is extracted
 *   and executed to provide an integer count of the number
 *   of times the loop is to be executed. The list of language
 *   enclosed by the 'repeat ... endrepeat' pair are executed
 *   the specified number of times.
 *
 *   Any break commands exit the loop, any continue commands
 *   skip any remaining loop commands, and any gotos which
 *   jump outside the loop terminate it.
 *
 * CALL:
 *   ClStdRepeatCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdRepeatCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  int                i;
  string             line;
  long               count;
  ClParserValuePtr value;
  int                num_args;
  ClLanguageArgList  arg_list;
  string             expression;

  CL_LANGUAGE_TRACE("ClStdRepeatCommand");

  const string &cargs = command->getArgs();

  /* Extract Repeat Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    ClLanguageMgrInst->syntaxError
     ("missing open brackets for 'repeat' expression");
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ')', line)) {
    ClLanguageMgrInst->syntaxError
     ("missing close brackets for 'repeat' expression");
    return;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' after 'repeat' expression",
      parse.getAt().c_str());
    return;
  }

  args->stringToArgList(line, &arg_list);

  num_args = arg_list.size();

  if (num_args != 1) {
    ClLanguageMgrInst->syntaxError
     ("'repeat' requires a single expression - %d found", num_args);
    return;
  }

  expression = arg_list[0];

  /*************/

  ClParserExpr expr(expression);

  if      (! expr.exec(value)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
      "'repeat' expression - '%s'", expression.c_str());
    return;
  }
  else if (! value.isValid()) {
    ClLanguageMgrInst->syntaxError
     ("undefined 'repeat' expression - '%s'", expression.c_str());
    return;
  }

  if (! value->integerValue(&count)) {
    ClLanguageMgrInst->expressionError
     (CLERR_INVALID_CONVERSION, "'repeat' expression - '%s'",
      expression.c_str());
    return;
  }

  if (count < 1) {
    ClLanguageMgrInst->syntaxError("invalid count %d for 'repeat'", count);
    return;
  }

  /*************/

  for (i = 0; i < count; i++) {
    /* Process Command List */

    if (ClLanguageBlockDataMgrInst->processBlockCommands())
      break;
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdIfCommand
 *   Routine called when a 'if ... endif' block command
 *   is entered.
 *
 *   The block of commands enclosed by the 'if ... endif'
 *   may contain 'elseif' and 'else' commands.
 *
 *   The expression contained in round brackets is extracted
 *   and evaluated. If the expression results in a non-zero
 *   integer then the enclosed commands up to any embedded
 *   'elseif' or 'else' commands are executed. If the
 *   expression results in a zero integer then the enclosed
 *   commands up to any embedded 'elseif' or 'else' commands
 *   are skipped. If the command is an 'elseif' then the
 *   command is effectively treated as an 'if' and the
 *   execution starts again from the current position in the
 *   enclosed command list. If the command is an 'else' then
 *   the remaining enclosed commands are executed.
 *
 *   Any break commands exit the block but remain in force
 *   for any parent block. Any continue commands exit the
 *   block but remain in force for any parent block. Any
 *   goto commands outside the block exit it.
 *
 * CALL:
 *   ClStdIfCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdIfCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  int               i;
  bool              flag;
  string            line;
  ClParserValuePtr  value;
  int               num_args;
  ClLanguageArgList arg_list;
  string            expression;
  int               num_commands;

  CL_LANGUAGE_TRACE("ClStdIfCommand");

  const string &cargs = command->getArgs();

  const ClLanguageCommandList &command_list = command->getCommandList();

  ClLanguageCommandList command_list1;

  ClLanguageCommand **commands = 0;

  /* Extract If Expression */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.isChar('(')) {
    ClLanguageMgrInst->syntaxError
     ("missing open brackets for 'if' expression");
    goto fail;
  }

  parse.skipChar();

  parse.skipSpace();

  i = parse.getPos();

  if (! args->readArgList(cargs, &i, ')', line)) {
    ClLanguageMgrInst->syntaxError
     ("missing close brackets for 'if' expression");
    goto fail;
  }

  parse.setPos(i);

  parse.skipSpace();

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' after 'if' expression",
      parse.getAt().c_str());
    goto fail;
  }

  args->stringToArgList(line, &arg_list);

  num_args = arg_list.size();

  if (num_args != 1) {
    ClLanguageMgrInst->syntaxError
     ("'if' requires a single expression - %d found", num_args);
    goto fail;
  }

  expression = arg_list[0];

  /*************/

  /* Process Expression and Check Result */

  {
  ClParserExpr expr(expression);

  if      (! expr.exec(value)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
      "'if' expression - '%s'", expression.c_str());
    goto fail;
  }
  else if (! value.isValid()) {
    ClLanguageMgrInst->syntaxError
     ("undefined 'if' expression - '%s'", expression.c_str());
    goto fail;
  }

  flag = value->toBool();
  }

  /*************/

  /* If Expression is True Process Command List */

  ClLanguageMgrInst->commandListToArray(command_list, &commands, &num_commands);

  for (i = 0; i < num_commands; i++) {
    if      (commands[i]->isIdent(language_elseif_ident)) {
      if (flag)
        break;

      const string &args1 = commands[i]->getArgs();

      /* Extract Else If Expression */

      CStrParse parse(args1);

      parse.skipSpace();

      if (! parse.isChar('(')) {
        ClLanguageMgrInst->syntaxError
         ("missing open brackets for 'elseif' expression");
        goto fail;
      }

      parse.skipChar();

      parse.skipSpace();

      i = parse.getPos();

      if (! args->readArgList(cargs, &i, ')', line)) {
        ClLanguageMgrInst->syntaxError
         ("missing close brackets for 'elseif' expression");
        goto fail;
      }

      parse.setPos(i);

      parse.skipSpace();

      if (! parse.eof()) {
        ClLanguageMgrInst->syntaxError
         ("spurious characters '%s' after 'elseif' expression",
          parse.getAt().c_str());
        goto fail;
      }

      args->stringToArgList(line, &arg_list);

      num_args = arg_list.size();

      if (num_args != 1) {
        ClLanguageMgrInst->syntaxError
         ("'elseif' requires a single expression - %d found", num_args);
        goto fail;
      }

      expression = arg_list[0];

      /*************/

      /* Process Expression and Check Result */

      ClParserExpr expr(expression);

      if      (! expr.exec(value)) {
        ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
          "'elseif' expression - '%s'", expression.c_str());
        goto fail;
      }
      else if (! value.isValid()) {
        ClLanguageMgrInst->syntaxError
         ("undefined 'elseif' expression - '%s'", expression.c_str());
        goto fail;
      }

      flag = value->toBool();
    }
    else if (commands[i]->isIdent(language_else_ident)) {
      if (flag)
        break;

      const string &args1 = commands[i]->getArgs();

      CStrParse parse(args1);

      parse.skipSpace();

      if (! parse.eof()) {
        ClLanguageMgrInst->syntaxError
         ("spurious characters '%s' after 'else' expression",
          parse.getAt().c_str());
        goto fail;
      }

      flag = true;
    }
    else {
      if (flag)
        command_list1.push_back(commands[i]);
    }
  }

  ClLanguageBlockDataMgrInst->setBlockCommands(command_list1);

  ClLanguageBlockDataMgrInst->processBlockCommands();

 fail:
  delete [] commands;
}

/*------------------------------------------------------------------*
 *
 * ClStdElseIfCommand
 *   Routine called when the 'elseif' command is entered
 *   outside of an 'if ... endif' block.
 *
 *   If the 'elseif' command is encountered outside
 *   an 'if ... endif' block (which processes it manually)
 *   then it is a syntax error.
 *
 * CALL:
 *   ClStdElseIfCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdElseIfCommand(ClLanguageCommand *, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdElseIfCommand");

  /* Else If can only be processed inside If Command List */

  ClLanguageMgrInst->syntaxError("'elseif' outside 'if' block");
}

/*------------------------------------------------------------------*
 *
 * ClStdElseCommand
 *   Routine called when the 'else' command is entered
 *   outside of an 'if ... endif' block.
 *
 *   If the 'else' command is encountered outside
 *   an 'if ... endif' block (which processes it manually)
 *   then it is a syntax error.
 *
 * CALL:
 *   ClStdElseCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdElseCommand(ClLanguageCommand *,
                 ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdElseCommand");

  /* Else can only be processed inside If Command List */

  ClLanguageMgrInst->syntaxError("'else' outside 'if' block");
}

/*------------------------------------------------------------------*
 *
 * ClStdGotoCommand
 *   Routine called when the 'goto' command is entered.
 *
 *   The name after the goto is extracted and we ensure
 *   that we are in a block command (otherwise there will
 *   be no saved label to goto). The label data for the
 *   corresponding label is found (if defined) and the
 *   goto details are stored in global variables ready
 *   for the parent block command to handle.
 *
 * CALL:
 *   ClStdGotoCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   Sets goto flag  to 'true', goto name to the label name and
 *   goto depth to the depth at which the label appears. The parent
 *   block command should check the level and terminate if it is lower
 *   than the current level or perform the goto if at the same level.
 *
 *------------------------------------------------------------------*/

static void
ClStdGotoCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  string               arg;
  int                  error_code;
  ClLanguageLabelData *label_data;

  CL_LANGUAGE_TRACE("ClStdGotoCommand");

  /* Get Goto's Arguments */

  if (args->checkNumberOfArgs(1, 1) != 0)
    return;

  /* Get Label */

  arg = args->getArg(1, &error_code);

  if (error_code != 0)
    return;

  if (! ClParserVar::isValidName(arg)) {
    ClLanguageMgrInst->syntaxError
      ("invalid 'goto' label name '%s'", arg.c_str());
    return;
  }

  /* Check that we are in a Block Command */

  if (ClLanguageMgrInst->getDepth() == 0) {
    ClLanguageMgrInst->syntaxError
      ("'goto' not allowed outside block command");
    return;
  }

  label_data = ClLanguageLabelDataMgrInst->getCurrentLabelData(arg);

  if (! label_data) {
    ClLanguageMgrInst->syntaxError
     ("'goto' label '%s' not known in current scope", arg.c_str());
    return;
  }

  ClLanguageMgrInst->getGoto().set(label_data->getName(),
                                   label_data->getDepth());
}

/*------------------------------------------------------------------*
 *
 * ClStdBreakCommand
 *   Routine called when the 'break' command is entered.
 *
 *   The routine ensures no arguments are supplied and
 *   sets a global variable ready for the parent block
 *   command to handle.
 *
 * CALL:
 *   ClStdBreakCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
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

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Set Break Flag */

  ClLanguageMgrInst->setBreakFlag(true);
}

/*------------------------------------------------------------------*
 *
 * ClStdContinueCommand
 *   Routine called when the 'continue' command is entered.
 *
 *   The routine ensures no arguments are supplied and
 *   that we are in a block command. It then sets a global
 *   variable ready for the parent block command to handle.
 *
 * CALL:
 *   ClStdContinueCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
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

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Check that we are in a Block Command */

  if (ClLanguageMgrInst->getDepth() == 0) {
    ClLanguageMgrInst->syntaxError("'continue' not allowed outside block command");
    return;
  }

  /* Set Continue Flag */

  ClLanguageMgrInst->setContinueFlag(true);
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
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
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

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  /* Check that we are in a Block Command */

  if (ClLanguageMgrInst->getDepth() == 0) {
    ClLanguageMgrInst->syntaxError("'return' not allowed outside block command");
    return;
  }

  /* Set Return Flag */

  ClLanguageMgrInst->setReturnFlag(true);
}

/*------------------------------------------------------------------*
 *
 * ClStdOnCommand
 *   Routine called when the 'on' command is entered.
 *
 *   When the specified signal type occurs the specified
 *   command is executed. If no command is specified
 *   then signal handling reverts to its default action.
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

static void
ClStdOnCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  int    flag;
  string type;
  string command1;

  CL_LANGUAGE_TRACE("ClStdOnCommand");

  const string &cargs = command->getArgs();

  /*-------------------*/

  /* Get Signal Type */

  CStrParse parse(cargs);

  parse.skipSpace();

  parse.readNonSpace(type);

  /* Get Command */

  parse.skipSpace();

  parse.readNonSpace(command1);

  if (! parse.eof()) {
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' at end of 'on' expression",
      parse.getAt().c_str());
    return;
  }

  /*-------------------*/

  flag = ClSignalMgrInst->setSignalCommand(type, command1);

  if (! flag)
    ClLanguageMgrInst->error("invalid signal type '%s'", type.c_str());
}

/*------------------------------------------------------------------*
 *
 * ClStdRaiseCommand
 *   Routine called when the 'raise' command is entered.
 *
 *   The routine raises the specified signal calling
 *   its associated command or routine.
 *
 * CALL:
 *   ClStdRaiseCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdRaiseCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  int    flag;
  string type;
  string data;

  CL_LANGUAGE_TRACE("ClStdRaiseCommand");

  const string &cargs = command->getArgs();

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
    ClLanguageMgrInst->syntaxError
     ("spurious characters '%s' at end of 'raise' expression",
      parse.getAt().c_str());
    return;
  }

  /*-------------------*/

  flag = ClSignalMgrInst->raiseSignal(type, data);

  if (! flag)
    ClLanguageMgrInst->error("invalid signal type '%s'", type.c_str());
}

/*------------------------------------------------------------------*
 *
 * ClStdDefineCommand
 *   Routine called when the 'define' command is entered.
 *
 *   This routine allows the user to define a simple
 *   function consisting of a single expression whose
 *   input variables are set to the values supplied
 *   when the function is invoked.
 *
 * CALL:
 *   ClStdDefineCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   Allows a function to be defined from the command line interface.
 *
 *------------------------------------------------------------------*/

static void
ClStdDefineCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  string::size_type  p;
  string             lhs, rhs;
  int                error_code;
  string             name_string;
  ClLanguageFunc    *old_lfunction;
  ClLanguageProc    *old_procedure;

  CL_LANGUAGE_TRACE("ClStdDefineCommand");

  const string &cargs = command->getArgs();

  /* Extract Procedure Name */

  CStrParse parse(cargs);

  if (! parse.readIdentifier(name_string)) {
    ClLanguageMgrInst->syntaxError
     ("'define' function name must start with alphabetic or '_'");
    return;
  }

  /* Get Existing Procedure and Function */

  old_procedure = ClLanguageProcMgrInst->lookupProc(name_string);
  old_lfunction = ClLanguageFuncMgrInst->lookupFunc(name_string);

  /* Ensure Procedure Can be Deleted */

  if (old_procedure != 0 && ! old_procedure->canDelete()) {
    ClLanguageMgrInst->syntaxError
     ("'define' Existing procedure '%s' cannot be deleted",
      name_string.c_str());
    return;
  }

  /* Define Procedure */

  p = cargs.find('=');

  if (p == string::npos) {
    ClLanguageMgrInst->syntaxError("missing equals sign for 'define'");
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
    ClLanguageMgrInst->expressionError
     (error_code, "'define' '%s' = '%s'", lhs.c_str(), rhs.c_str());

    /* Restore Language Function's Parser User Function if failed
       to define Parser Function */

    if (old_lfunction != 0) {
      int *arg_types;

      if (old_lfunction->getNumArgs() > 0)
        arg_types = new int [old_lfunction->getNumArgs()];
      else
        arg_types = 0;

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
 *   This routine allows the user to define a structure
 *   consisting of a name and a type name list enclosed
 *   in curly brackets.
 *
 * CALL:
 *   ClStdStructCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   Allows the ClParserDefineStructure routine to be
 *   used from the command line interface.
 *
 *------------------------------------------------------------------*/

static void
ClStdStructCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  string type_arg_list;
  string structure_name;

  CL_LANGUAGE_TRACE("ClStdStructCommand");

  const string &cargs = command->getArgs();

  /* Extract Structure Name */

  CStrParse parse(cargs);

  parse.skipSpace();

  if (! parse.readIdentifier(structure_name)) {
    ClLanguageMgrInst->syntaxError("missing structure name for 'struct'");
    return;
  }

  parse.skipSpace();

  if (! parse.isChar('{')) {
    ClLanguageMgrInst->syntaxError
     ("missing open curly brace for 'struct' '%s'", structure_name.c_str());
    return;
  }

  parse.skipChar();

  parse.skipSpace();

  parse.readToChar('}', type_arg_list);

  CStrUtil::stripSpaces(type_arg_list);

  if (type_arg_list == "") {
    ClLanguageMgrInst->syntaxError
      ("missing open type/argument list for 'struct' '%s'", structure_name.c_str());
    return;
  }

  if (! parse.isChar('}')) {
    ClLanguageMgrInst->syntaxError
     ("missing close curly brace for 'struct' '%s'", structure_name.c_str());
    return;
  }

  /*--------------*/

  try {
    ClParserInst->createType(structure_name, type_arg_list);
  }
  catch (int error_code) {
    ClLanguageMgrInst->expressionError
     (error_code, "'struct' '%s' { '%s' }", structure_name.c_str(), type_arg_list.c_str());
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdInputCommand
 *   Routine called when the 'input' command is entered.
 *
 *   This routine outputs the specified prompt str
 *   and waits for the user to enter some text. If the
 *   variable specified as the second argument starts
 *   with a '$' then the text is assigned directly to
 *   the variable (name minus the dollar) otherwise the
 *   text is treated as an expression and is evaluated
 *   and the resultant value assigned to the variable.
 *
 * CALL:
 *   ClStdInputCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
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
  int             i;
  FILE           *fp;
  long            fp1;
  string          arg;
  string          name;
  string          line;
  string          line1;
  ClParserValuePtr value;
  int             evaluate;
  string          variable;
  string          variable1;
  int             error_code;
  int             arg_offset;
  string          prompt_string;

  CL_LANGUAGE_TRACE("ClStdInputCommand");

  /* Check Number of Arguments */

  if (args->checkNumberOfArgs(2, 3))
    return;

  /* Check for specified Input File */

  arg = args->getArg(1, &error_code);

  if (error_code != 0)
    return;

  if (arg[0] == '@') {
    i = 1;

    name = arg.substr(i);

    CStrUtil::stripSpaces(name);

    if (! ClParserVar::isValidName(name)) {
      ClLanguageMgrInst->syntaxError
       ("invalid 'input' file variable %s", arg.c_str());
      return;
    }

    value = ClParserInst->getVariableValue(name);

    if (! value->integerValue(&fp1)) {
      ClLanguageMgrInst->expressionError
       (CLERR_INVALID_CONVERSION, "'input' file variable '%s'", name.c_str());
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
    ClLanguageMgrInst->syntaxError
     ("'input' argument 2 (invalid variable name '%s')", variable1.c_str());
    return;
  }

  /* Prompt User for Input */

  ClLanguageMgrInst->output("%s", prompt_string.c_str());

#ifdef CEIL_READLINE
  if (fp == stdin) {
    ClLanguageMgrInst->getReadLine().setPrompt("");

    line = ClLanguageMgrInst->getReadLine().readLine();
  }
  else {
    if (! CFile::readLine(fp, line))
      line = "";
  }
#else
  if (! CFile::readLine(fp, line1))
    line = "";
#endif

  /* Handle User Interrupt */

  if (line == "" || ClLanguageMgrInst->checkAbort()) {
    if (ClSignalMgrInst->checkSignalCommand(SIGINT)) {
      if (line == "")
        ClSignalMgrInst->raiseSignal("interrupt", "");
    }
    else
      ClLanguageMgrInst->error("** Interrupt ** input abandoned");

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

    value = ClParserValueMgrInst->createValue(line);

    if (! value.isValid())
      goto set_value;

    ClParserInst->assignValue(variable1, value, &error_code);

    if (error_code != 0) {
      ClLanguageMgrInst->expressionError
       (error_code, "'input' argument 2 - '%s'", line.c_str());
      return;
    }
  }

  return;

 set_value:
  if (evaluate)
    args->setExpressionArg(2, "", &error_code);
  else {
    value = ClParserValueMgrInst->createValue("");

    if (! value.isValid())
      return;

    ClParserInst->assignValue(variable1, value, &error_code);

    if (error_code != 0) {
      ClLanguageMgrInst->expressionError
       (error_code, "'input' argument 2 - '%s'", line.c_str());
      return;
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdPrintCommand
 *   Routine called when the 'print' command is entered.
 *
 *   This routine prints the value of each command
 *   separated expression specified followed by an
 *   optional new line. Each expression is separated from
 *   the next with a space.
 *
 * CALL:
 *   ClStdPrintCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   If an argument starts with '@' then the subsequent
 *   str is taken as the name of a variable containing
 *   the file pointer to output to.
 *   If the command ends with a ',' the no newline is output.
 *
 *------------------------------------------------------------------*/

static void
ClStdPrintCommand(ClLanguageCommand *command, ClLanguageArgs *args, void *)
{
  ClParserValuePtr value;
  int              error_code;

  CL_LANGUAGE_TRACE("ClStdPrintCommand");

  const string &cargs = command->getArgs();

  uint len = cargs.size();

  bool newline = true;

  if (len > 0 && cargs[len - 1] == ',')
    newline = false;

  /*-------------*/

  FILE *fp      = ClLanguageMgrInst->setOutputFp(0);
  FILE *save_fp = fp;

  /* Get Print's Arguments */

  uint num_args = args->getNumArgs();

  /* Print Arguments */

  bool value_output = false;

  for (uint i = 1; i <= num_args; i++) {
    string arg = args->getArg(i, &error_code);

    if (error_code != 0)
      goto done;

    if (arg[0] == '@') {
      fflush(fp);

      int j = 1;

      string name = arg.substr(j);

      CStrUtil::stripSpaces(name);

      if (! ClParserVar::isValidName(name)) {
        ClLanguageMgrInst->syntaxError
         ("invalid 'print' file variable %s", arg.c_str());
        goto done;
      }

      value = ClParserInst->getVariableValue(name);

      long fp1;

      if (! value->integerValue(&fp1)) {
        ClLanguageMgrInst->expressionError
         (CLERR_INVALID_CONVERSION, "'print' file variable '%s'", name.c_str());
        goto done;
      }

      fp = (FILE *) fp1;

      ClLanguageMgrInst->setOutputFp(fp);

      value_output = false;

      continue;
    }

    ClParserExpr expr(arg);

    if      (! expr.exec(value)) {
      if (num_args == 1)
        ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
          "'print' expression '%s'", arg.c_str());
      else
        ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
          "'print' expression %d - '%s'", i, arg.c_str());
      goto done;
    }
    else if (! value.isValid()) {
      if (num_args == 1)
        ClLanguageMgrInst->syntaxError
         ("undefined 'print' expression '%s'", arg.c_str());
      else
        ClLanguageMgrInst->syntaxError
         ("undefined 'print' expression %d - '%s'", i, arg.c_str());
      goto done;
    }

    if (value_output)
      ClLanguageMgrInst->output(" ");

    value->print();

    value_output = true;
  }

 done:
  if (newline)
    ClLanguageMgrInst->output("\n");

  fflush(fp);

  ClLanguageMgrInst->setOutputFp(save_fp);
}

/*------------------------------------------------------------------*
 *
 * ClStdFormatCommand
 *   Routine called when the 'format' function is entered.
 *
 *   This routine takes the first value as the format str
 *   and then scans it for embedded control codes which are
 *   used in conjunction with any extra arguments supplied
 *   after the format str.
 *
 *   The style of the format str is identical to that used
 *   by the C function 'printf' without the 'p' and 'n'
 *   format codes. The number and type of the supplied
 *   arguments are checked and are ignored, and the embedded
 *   format code left as is, if the argument is the wrong
 *   type or is not present.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdFormatCommand
 *      (ClParserValuePtr *, uint, void *, int &error_code);
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
ClStdFormatCommand(ClParserValuePtr *values, uint num_values, void *, int *error_code)
{
  CL_LANGUAGE_TRACE("ClStdFormatCommand");

  *error_code = 0;

  ClParserValuePtr value;

  if (num_values <= 0) {
    ClLanguageMgrInst->syntaxError("'format' requires at least 1 value");
    return ClParserValuePtr();
  }

  string format_string;

  if (! values[0]->stringValue(format_string)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'format' argument 1");
    return ClParserValuePtr();
  }

  uint value_num = 1;

  string temp_string1 = "";

  uint len = format_string.size();

  uint i = 0;

  while (i < len) {
    switch (format_string[i]) {
      case '%': {
        int    flags;
        string temp_string2;
        string value_format_string;
        int    field_width, precision;
        char   length_modifier, format_code;

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
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                field_width, integer);
            else if (flags & CPRINTF_PRECISION_AS_VALUE)
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                precision, integer);
            else
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                integer);

            break;
          }
          case 's': {
            string str;

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
              CStrUtil::sprintf(temp_string2, value_format_string.c_str(),
                                str.c_str());

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
 *   The routine extracts values for the supplied str
 *   using the supplied template. The resultant values
 *   are store in the variables specified by the
 *   remaining arguments.
 *
 *   The style of the template is identical to that used
 *   by the C function 'scanf'.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdFormatCommand
 *       (ClParserValuePtr *, uint, void *, int &error_code);
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
  uint    i, len1, len2;
  char   *addrs[20];
  int     codes[20];
  uint    num_addrs = 0;
  uint    num_scanned;
  string  source_string;
  string  format_string;
  string  format_string1;

  CL_LANGUAGE_TRACE("ClStdScanCommand");

  *error_code = -1;

  ClParserValuePtr value;

  /*-----------------*/

  if (num_values < 2) {
    ClLanguageMgrInst->syntaxError("'scan' requires at least 2 values");
    goto fail;
  }

  /*-----------------*/

  /* Get Source and Format Strings */

  if (! values[0]->stringValue(source_string)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'scan' argument 1");
    goto fail;
  }

  if (! values[1]->stringValue(format_string)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'scan' argument 2");
    goto fail;
  }

  /*-----------------*/

  num_addrs = 0;

  i = 0;

  len1 = source_string.size();
  len2 = format_string.size();

  format_string1 = "";

  while (i < len2) {
    switch (format_string[i]) {
      case '%': {
        int    flags;
        char   format_code;
        string format_string2;
        string format_string3;
        char   length_modifier;
        int    field_width, precision;

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
            ClLanguageMgrInst->syntaxError("Missing ']' in scan");
            goto fail;
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

        if (num_addrs >= 20) {
          ClLanguageMgrInst->syntaxError("Too many values to scan (> 20)");
          goto fail;
        }

        codes[num_addrs] = format_code;

        switch (format_code) {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
            addrs[num_addrs] = (char *) new long;
            break;
          case 'c':
            if (len1 > 255)
              addrs[num_addrs] = new char [len1];
            else
              addrs[num_addrs] = new char [256];

            break;
          case 's':
            if (len1 > 255)
              addrs[num_addrs] = new char [len1];
            else
              addrs[num_addrs] = new char [256];

            break;
          case 'e':
          case 'f':
          case 'g':
            addrs[num_addrs] = (char *) new double;
            break;
          case 'p':
            addrs[num_addrs] = (char *) new char *;
            break;
          case 'n':
            addrs[num_addrs] = (char *) new int;
            break;
        }

        num_addrs++;

        format_string1 += format_string2;

        break;
      }
      default: {
        format_string1 += format_string[i++];

        break;
      }
    }
  }

  if (num_values - 2 != num_addrs) {
    ClLanguageMgrInst->syntaxError(
      "invalid no. of args (%d) to scan into - need %d",
      num_values - 2, num_addrs);
    goto fail;
  }

  for (i = num_addrs; i < 20; i++) {
    codes[i] = '\0';
    addrs[i] = 0;
  }

  num_scanned = sscanf(source_string.c_str(), format_string1.c_str(),
                       addrs[ 0], addrs[ 1], addrs[ 2], addrs[ 3], addrs[ 4],
                       addrs[ 5], addrs[ 6], addrs[ 7], addrs[ 8], addrs[ 9],
                       addrs[10], addrs[11], addrs[12], addrs[13], addrs[14],
                       addrs[15], addrs[16], addrs[17], addrs[18], addrs[19]);

/*
  if (num_scanned != num_addrs) {
    ClLanguageMgrInst->syntaxError("Failed to scan %d values, only %d found",
                                    num_addrs, num_scanned);
    goto fail;
  }
*/

  for (i = 0; i < num_scanned; i++) {
    switch (codes[i]) {
      case 'd': case 'i': case 'o': case 'u': case 'x':
        value = ClParserValueMgrInst->createValue(*((long *) addrs[i]));

        break;
      case 'c': case 's':
        value = ClParserValueMgrInst->createValue((char *) addrs[i]);

        break;
      case 'e': case 'f': case 'g':
        value = ClParserValueMgrInst->createValue(*((double *) addrs[i]));

        break;
      case 'p':
        value = ClParserValueMgrInst->createValue((long) addrs[i]);

        break;
      case 'n':
        value = ClParserValueMgrInst->createValue((long) *((int *) addrs[i]));

        break;
    }

    values[i + 2] = value;
  }

  value = ClParserValueMgrInst->createValue((long) num_scanned);

  *error_code = 0;

 fail:
  for (uint i = 0; i < num_addrs; i++)
    delete addrs[i];

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdExecCommand
 *   Routine called when the 'exec' function is entered.
 *
 *   This routine executes a command and stores the
 *   resultant output in a str array value (one element
 *   for each output line from the command). The command is
 *   specifed as the single argument.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdExecCommand
 *      (ClParserValuePtr *, uint, void *, int &error_code);
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
  string                   line;
  StringVectorT            lines;
  string                   output;
  uint                     dims[1];
  string                   command;
  uint                     num_dims;
  int                      num_lines;
  ClLanguageArgList        line_list;
  COSProcess::CommandState command_state;

  CL_LANGUAGE_TRACE("ClStdExecCommand");

  *error_code = 0;

  ClParserValuePtr value;

  if (! values[0]->stringValue(command)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'exec'");
    goto fail;
  }

  /* Execute Command and Read Output */

  if (! COSProcess::executeCommand(command, output, &command_state)) {
    *error_code = -1;

    ClLanguageMgrInst->syntaxError
      ("'exec' command '%s' failed", command.c_str());

    goto fail;
  }

  CStrUtil::addLines(output, lines);

  num_lines = lines.size();

  for (i = 0; i < num_lines; ++i)
    line_list.push_back(lines[i]);

  ClParserInst->createVar("_rc",
    ClParserValueMgrInst->createValue((long) command_state.getStatus()));

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
 *   This routine executes a str as an expression and
 *   returns the resultant value. The expression str is
 *   specifed as the single argument.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdParseCommand
 *       (ClParserValuePtr *, uint, void *, int &error_code);
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
  string command;

  CL_LANGUAGE_TRACE("ClStdParseCommand");

  *error_code = 0;

  ClParserValuePtr value;

  if (! values[0]->stringValue(command)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'parse'");
    return ClParserValuePtr();
  }

  ClParserExpr expr(command);

  if (! expr.exec(value)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_EXPRESSION,
      "'parse' expression '%s'", command.c_str());
    return ClParserValuePtr();
  }

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdGetEnvCommand
 *   Routine called when the 'getenv' command is entered.
 *
 *   This routine gets the value of the specified
 *   environment variable. The environment variable
 *   name is specified as a str in the second argument
 *   and the variable name to which the environment
 *   variable's value str is assigned is specified
 *   as the first argument.
 *
 *   If the environment variable is not set then the
 *   variable's value is set to the empty str "".
 *
 * CALL:
 *   ClStdGetEnvCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdGetEnvCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  string env_name;
  string env_value;
  int    error_code;

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
 *   This routine sets the value of the specified
 *   environment variable. The environment variable
 *   name is specified as a str in the first argument
 *   and the str to which the environment is set
 *   is specified as the second argument.
 *
 * CALL:
 *   ClStdSetEnvCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdSetEnvCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  string env_name;
  string env_value;
  int    error_code;

  CL_LANGUAGE_TRACE("ClStdSetEnvCommand");

  /* Get SetEnv's Arguments */

  if (args->checkNumberOfArgs(2, 2) != 0)
    return;

  /* Get Environment Variable and Value */

  env_name = args->getStringArg(1, &error_code);

  if (error_code != 0)
    return;

  env_value = args->getStringArg(2, &error_code);

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
 *   This routine removes the specified environment variable.
 *   The environment variable name is specified as a str
 *   in the single argument.
 *
 * CALL:
 *   ClStdUnSetEnvCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdUnSetEnvCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  string env_name;
  int    error_code;

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
 * ClStdSetCommand
 *   Routine called when the 'set' command is entered.
 *   This routine calls the set method for any specified
 *   module.
 *
 * CALL:
 *   ClStdSetCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdSetCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdSetCommand");

  const string &cargs = command->getArgs();

  CStrParse parse(cargs);

  /*-------------------*/

  /* Get Module Name */

  string module;

  if (parse.isChar('@')) {
    parse.skipChar();

    parse.readNonSpace(module);

    parse.skipSpace();
  }
  else
    module = "builtin";

  /*-------------------*/

  /* Get Option Name */

  string option;

  parse.readNonSpace(option);

  parse.skipSpace();

  /*-------------------*/

  /* Get Additional Args */

  string args1 = parse.getAt();

  /*-------------------*/

  /* Call Module Set Method */

  bool flag = ClModuleMgrInst->execModuleSet(module, option, args1);

  if (! flag)
    ClLanguageMgrInst->error("'set' failed for module '%s' "
                              "option '%s' args '%s'", module.c_str(),
                              option.c_str(), args1.c_str());
}

// Routine called when the 'rand' function is entered.
//
// The routine a random number (optionally seeded by any supplied value).

static ClParserValuePtr
ClStdRandCommand(ClParserValuePtr *values, uint num_values, void *, int *error_code)
{
  CL_LANGUAGE_TRACE("ClStdRandCommand");

  *error_code = 0;

  ClParserValuePtr value;

  if (num_values > 1) {
    *error_code = CLERR_TOO_MANY_ARGUMENTS;
    return ClParserValuePtr();
  }

  if (num_values == 1) {
    long seed;

    if (! values[0]->integerValue(&seed)) {
      ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'rand'");
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

  *error_code = 0;

  ClParserValuePtr value;

  if (num_values > 1) {
    *error_code = CLERR_TOO_MANY_ARGUMENTS;
    return ClParserValuePtr();
  }

  if (num_values == 1) {
    long seed;

    if (! values[0]->integerValue(&seed)) {
      ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'rrand'");
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
 *   This routine returns whether a variable of the specified
 *   name has been defined or not.
 *
 * CALL:
 *   ClParserValuePtr value =
 *     ClStdIsVarCommand
 *       (ClParserValuePtr *, uint, void *, int &error_code);
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
  string name;

  CL_LANGUAGE_TRACE("ClStdIsVarCommand");

  *error_code = 0;

  ClParserValuePtr value;

  if (! values[0]->stringValue(name)) {
    ClLanguageMgrInst->expressionError(CLERR_INVALID_CONVERSION, "'is_var'");
    return ClParserValuePtr();
  }

  bool flag = ClParserInst->isVariable(name);

  value = ClParserValueMgrInst->createValue((long) flag);

  return value;
}

/*------------------------------------------------------------------*
 *
 * ClStdUndefCommand
 *   Routine called when the 'undef' command is entered.
 *
 *   Undefines the associate procedure, function, structure
 *   type or variable associated with each of the list of
 *   specified identifiers.
 *
 * CALL:
 *   ClStdUndefCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdUndefCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  string          arg;
  ClLanguageFunc *function;
  ClLanguageProc *procedure;
  int             error_code;

  CL_LANGUAGE_TRACE("ClStdUndefCommand");

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
        ClLanguageMgrInst->syntaxError
         ("variable '%s' cannot be deleted", arg.c_str());
    }
    else if (ClParserInst->isFunction(arg)) {
      ClParserInst->removeFunction(arg);
    }
    else if (ClParserInst->isInternFn(arg)) {
      ClLanguageMgrInst->syntaxError
       ("'undef' cannot undefine internal function '%s'", arg.c_str());
    }
    else if (ClParserInst->isType(arg)) {
      ClParserInst->deleteType(arg);
    }
    else if ((procedure = ClLanguageProcMgrInst->lookupProc(arg)) != 0) {
      if (procedure->canDelete())
        ClLanguageProcMgrInst->deleteProc(arg);
      else
        ClLanguageMgrInst->syntaxError
         ("'undef' procedure '%s' cannot be undefined", arg.c_str());
    }
    else if ((function = ClLanguageFuncMgrInst->lookupFunc(arg)) != 0) {
      ClParserInst->removeUserFn(arg);

      ClLanguageFuncMgrInst->deleteFunc(arg);
    }
    else
      ClLanguageMgrInst->syntaxError
       ("'undef' identifier '%s' is undefined", arg.c_str());
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdWhatisCommand
 *   Routine called when the 'whatis' command is entered.
 *
 *   Prints a description of the objects associated with
 *   each of the specified identifiers.
 *
 * CALL:
 *   ClStdWhatisCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdWhatisCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  string                arg;
  ClParserValuePtr      value;
  bool                  end_name;
  ClLanguageProc       *procedure;
  int                   error_code;
  ClLanguageCommandDef *command_def;

  CL_LANGUAGE_TRACE("ClStdWhatisCommand");

  /* Get Whatis's Arguments */

  if (args->checkNumberOfArgs(1, -1) != 0)
    return;

  uint num_args = args->getNumArgs();

  /* Output type of each Argument */

  for (uint i = 1; i <= num_args; i++) {
    arg = args->getArg(i, &error_code);

    if (error_code != 0)
      return;

    //bool found_proc = false;

    if ((procedure = ClLanguageProcMgrInst->lookupProc(arg)) != 0) {
      ClLanguageMgrInst->output("Procedure ");

      procedure->print();

      //found_proc = true;
    }

    ClParserScopePtr scope;

    command_def = ClLanguageMgrInst->getCommandDef(scope, arg, &end_name);

    if      (command_def != 0) {
      if (command_def->getEndName() != "") {
        if (! end_name)
          ClLanguageMgrInst->output("Start Block Command %s\n", arg.c_str());
        else
          ClLanguageMgrInst->output("End Block Command %s\n", arg.c_str());
      }
      else
        ClLanguageMgrInst->output("Command %s\n", arg.c_str());
    }
    else if (ClParserInst->isVariable(arg)) {
      value = ClParserInst->getVariableValue(arg);

      if      (value->isReal())
        ClLanguageMgrInst->output("real ");
      else if (value->isInteger())
        ClLanguageMgrInst->output("int ");
      else if (value->isString())
        ClLanguageMgrInst->output("str ");
      else if (value->isList())
        ClLanguageMgrInst->output("list ");
      else if (value->isDictionary())
        ClLanguageMgrInst->output("dictionary ");
      else if (value->isStructure())
        ClLanguageMgrInst->output("structure ");
      else if (value->isRealArray())
        ClLanguageMgrInst->output("real array ");
      else if (value->isIntegerArray())
        ClLanguageMgrInst->output("integer array ");
      else if (value->isStringArray())
        ClLanguageMgrInst->output("str array ");
      else if (value->isListArray())
        ClLanguageMgrInst->output("list array ");
      else if (value->isDictionaryArray())
        ClLanguageMgrInst->output("dictionary array ");
      else if (value->isStructureArray())
        ClLanguageMgrInst->output("structure array ");

      ClLanguageMgrInst->output("%s = ", arg.c_str());

      value->print();

      ClLanguageMgrInst->output("\n");
    }
    else if (ClParserInst->isFunction(arg)) {
      ClParserFuncPtr function = ClParserInst->getFunction(arg);

      ClLanguageMgrInst->output("Function ");

      function->print();

      ClLanguageMgrInst->output("\n");
    }
    else if (ClParserInst->isInternFn(arg)) {
      ClLanguageMgrInst->output("Internal Function %s\n", arg.c_str());
    }
    else if (ClParserInst->isUserFn(arg)) {
      ClLanguageMgrInst->output("User Function %s\n", arg.c_str());
    }
    else if (ClParserInst->isType(arg)) {
      ClParserTypePtr type = ClParserInst->getType(arg);

      type->print();
    }
    else {
      ClParserExpr expr(arg);

      if (! expr.exec(value)) {
        ClLanguageMgrInst->output("Undefined %s\n", arg.c_str());
        continue;
      }

      if      (value->isReal())
        ClLanguageMgrInst->output("real");
      else if (value->isInteger())
        ClLanguageMgrInst->output("int");
      else if (value->isString())
        ClLanguageMgrInst->output("str");
      else if (value->isList())
        ClLanguageMgrInst->output("list");
      else if (value->isDictionary())
        ClLanguageMgrInst->output("dictionary");
      else if (value->isStructure())
        ClLanguageMgrInst->output("structure");
      else if (value->isRealArray())
        ClLanguageMgrInst->output("real array");
      else if (value->isIntegerArray())
        ClLanguageMgrInst->output("integer array");
      else if (value->isStringArray())
        ClLanguageMgrInst->output("str array");
      else if (value->isListArray())
        ClLanguageMgrInst->output("list array");
      else if (value->isDictionaryArray())
        ClLanguageMgrInst->output("dictionary array");
      else if (value->isStructureArray())
        ClLanguageMgrInst->output("structure array");

      ClLanguageMgrInst->output("\n");
    }
  }
}

/*------------------------------------------------------------------*
 *
 * ClStdStatusCommand
 *   Routine called when the 'list' command is entered.
 *
 *   Prints a list of all the currently defined objects
 *   of the specified type or all the currently defined
 *   objects of all types if no argument is supplied.
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

static void
ClStdStatusCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  const char *type;

  CL_LANGUAGE_TRACE("ClStdStatusCommand");

  /* Check Args */

  if (args->checkNumberOfArgs(0, 1) != 0)
    return;

  /*--------------*/

  uint num = args->getNumArgs();

  if (num == 1) {
    if (args->getArgList(CL_ARG_TYPE_TEXT, &type, CL_ARG_TYPE_NONE) != 1)
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
    ClLanguageMgrInst->output("Variables :-\n\n");

    ClParserInst->printAllVariables();

    ClLanguageMgrInst->output("\nProcedures :-\n\n");

    ClParserInst->printAllFunctions();

    ClLanguageProcMgrInst->printProcs();
    ClLanguageFuncMgrInst->printFuncs();

    ClLanguageMgrInst->output("\nStructures :-\n\n");

    ClParserInst->printAllTypes();
  }
  else
    ClLanguageMgrInst->syntaxError("Unknown type '%s' for 'list'", type);
}

/*------------------------------------------------------------------*
 *
 * ClStdHelpCommand
 *   Routine called when the 'help' command is entered.
 *
 *   This routine outputs help for the optional subject
 *   str using the current help procedure. By default
 *   this will display a list of language commands.
 *
 * CALL:
 *   ClStdHelpCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
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

static void
ClStdHelpCommand(ClLanguageCommand *command, ClLanguageArgs *, void *)
{
  CL_LANGUAGE_TRACE("ClStdHelpCommand");

  const string &cargs = command->getArgs();

  if (cargs == "") {
    ClLanguageMgrInst->output("Help available for :-\n\n");

    StringVectorT modules;

    ClModuleMgrInst->getModuleList(modules);

    int num_modules = modules.size();

    for (int i = 0; i < num_modules; i++) {
      if (! ClModuleMgrInst->isModuleLoaded(modules[i]))
        continue;

      ClLanguageMgrInst->output("  %s\n", modules[i].c_str());
    }

    ClLanguageMgrInst->output("\n");

    return;
  }

  /*-------*/

  /* Get Subject and Args */

  string file;
  string subject;

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

  string args1 = parse.getAt();

  /*-------*/

  FILE *fp     = 0;
  FILE *old_fp = 0;

  /*-------*/

  if (file != "") {
    fp = fopen(file.c_str(), "w");

    if (fp != 0)
      old_fp = ClLanguageMgrInst->setOutputFp(fp);
    else
      ClLanguageMgrInst->error("invalid file '%s'", file.c_str());
  }

  /*-------*/

  /* Display Module Help (if any) */

  if (ClModuleMgrInst->execModuleHelp(subject, args1))
    goto done;

  /*-------*/

  /* Print Out Help using current Help Procedure */

  {
    ClLanguageHelpProc help_proc = ClLanguageMgrInst->getHelpProc();

    if (help_proc != 0) {
      (*help_proc)(subject.c_str(), args1.c_str());

      goto done;
    }
  }

  /*-------*/

  ClLanguageMgrInst->error("No help for '%s'", subject.c_str());

  /*-------*/

 done:
  if (file != "" && fp != 0)
    ClLanguageMgrInst->setOutputFp(old_fp);
}

/*------------------------------------------------------------------*
 *
 * ClStdExitCommand
 *   Routine called when the 'exit' command is entered.
 *
 *   This routine sets a flag to indicate that the run
 *   of the current file or command line interface should
 *   be terminated.
 *
 * CALL:
 *   ClStdExitCommand(ClLanguageCommand *command);
 *
 * INPUT:
 *   command : Structure containing details of the command
 *           : and arguments supplied by the user.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
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

  /* Check Args */

  if (args->checkNumberOfArgs(0, 1) != 0)
    return;

  uint num = args->getNumArgs();

  long code = 0;

  if (num == 1) {
    if (args->getArgList(CL_ARG_TYPE_INTEGER, &code, CL_ARG_TYPE_NONE) != 1)
      return;
  }

  /* Set Exit Flag and Exit Code */

  ClLanguageMgrInst->setExitFlag(true);

  ClLanguageMgrInst->setExitCode(code);
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
    if (args->getArgList(CL_ARG_TYPE_INTEGER, &code, CL_ARG_TYPE_NONE) != 1)
      return;
  }

  exit(code);
}

#ifdef CEIL_READLINE
static void
ClStdHistoryCommand(ClLanguageCommand *, ClLanguageArgs *args, void *)
{
  CL_LANGUAGE_TRACE("ClStdHistoryCommand");

  /* Check Args */

  if (args->checkNumberOfArgs(0, 0) != 0)
    return;

  vector<CReadLineHistoryEntry> entries;

  ClLanguageMgrInst->getReadLine().getHistoryEntries(entries);

  int num_entries = entries.size();

  for (int i = 0; i < num_entries; ++i)
    ClLanguageMgrInst->output("%d: %s\n", entries[i].line_num, entries[i].line.c_str());
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
