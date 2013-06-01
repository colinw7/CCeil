#include <CCeilL.h>

using std::string;

CCeilLErrorMgr::
CCeilLErrorMgr()
{
  error_fp_   = NULL;
  error_proc_ = NULL;
  error_data_ = NULL;
  error_flag_ = false;
}

/*------------------------------------------------------------------*
 *
 * error
 *   Print the specified formatted str to the current error output device.
 *
 * CALL:
 *   error(char *format, ...);
 *
 * INPUT:
 *   format : 'printf' style format str which will be used to build the str.
 *
 *   ...    : Optional arguments required by embedded
 *          : format codes in the format str.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   The str will be sent to the current error output
 *   procedure (if defined) or the current error output file.
 *
 *------------------------------------------------------------------*/

void
CCeilLErrorMgr::
error(const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  error(format, &vargs);

  va_end(vargs);
}

void
CCeilLErrorMgr::
error(const char *format, va_list *vargs)
{
  CStrUtil::vsprintf(error_string_, format, vargs);

  if (error_proc_ != NULL) {
    string str;

    const string &input_file = ClLanguageMgrInst->getInputFile().getName();

    if (input_file != "")
      CStrUtil::sprintf(str, "Error in %s:%d - %s\n", input_file.c_str(),
                        ClLanguageMgrInst->getCommandLineNum(),
                        error_string_.c_str());
    else
      CStrUtil::sprintf(str, "Error - %s\n", error_string_.c_str());

    (*error_proc_)(str.c_str(), error_data_);
  }
  else {
    if (error_fp_ == NULL)
      error_fp_ = stderr;

    fprintf(error_fp_, "Error - %s\n", error_string_.c_str());
  }

  error_flag_ = true;
}

/*------------------------------------------------------------------*
 *
 * syntaxError
 *   This routine should be called if the application
 *   program encounters a syntax error in its argument
 *   str.
 *
 * CALL:
 *   syntaxError(char *format, ...);
 *
 * INPUT:
 *   format : 'printf' style format str which will be
 *          : used to build the syntax error message.
 *
 *   ...    : Optional arguments required by embedded
 *          : format codes in the format str.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   This routine will output a message :-
 *
 *     Syntax Error - <error_message>
 *
 *   unless the 'onsyntax <command>' command has been
 *   specified in which case the error message str
 *   is stored in the variable '_error_string' and the
 *   'onsyntax' command is run.
 *
 *------------------------------------------------------------------*/

void
CCeilLErrorMgr::
syntaxError(const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  syntaxError(format, &vargs);

  va_end(vargs);
}

void
CCeilLErrorMgr::
syntaxError(const char *format, va_list *vargs)
{
  CStrUtil::vsprintf(error_string_, format, vargs);

  if (! ClSignalMgrInst->checkSignalCommand(SIGSYNTAX)) {
    const string &input_file = ClLanguageMgrInst->getInputFile().getName();

    if (error_proc_ != NULL) {
      string str;

      if (input_file != "")
        CStrUtil::sprintf(str, "Syntax Error in %s:%d - %s\n", input_file.c_str(),
                          ClLanguageMgrInst->getCommandLineNum(), error_string_.c_str());
      else
        CStrUtil::sprintf(str, "Syntax Error - %s\n", error_string_.c_str());

      (*error_proc_)(str.c_str(), error_data_);
    }
    else {
      if (error_fp_ == NULL)
        error_fp_ = stderr;

      if (input_file != "")
        fprintf(error_fp_, "Syntax Error in %s:%d - %s\n", input_file.c_str(),
                ClLanguageMgrInst->getCommandLineNum(), error_string_.c_str());
      else
        fprintf(error_fp_, "Syntax Error - %s\n", error_string_.c_str());
    }
  }
  else {
    ClParserInst->createVar("_error_string",
      ClParserValueMgrInst->createValue(error_string_));

    ClSignalMgrInst->executeSignalCommand(SIGSYNTAX);
  }

  error_flag_ = true;
}

/*------------------------------------------------------------------*
 *
 * expressionError
 *   This routine should be called if the application
 *   program encounters an error when calling one of
 *   the CL parser routines.
 *
 * CALL:
 *   expressionError(int error_code, char *format, ...);
 *
 * INPUT:
 *   error_code : The error code returned from the
 *              : CL parser routine.
 *
 *   format     : 'printf' style format str which will
 *              : be used to build the syntax error message.
 *
 *   ...        : Optional arguments required by embedded
 *              : format codes in the format str.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   This routine will output a message :-
 *
 *     Expression Error - <str>
 *
 *   unless the 'onexpr <command>' command has been
 *   specified in which case the error message str
 *   is stored in the variable '_error_string' and the
 *   'onexpr' command is run.
 *
 *------------------------------------------------------------------*/

void
CCeilLErrorMgr::
expressionError(int error_code, const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  expressionError(error_code, format, &vargs);

  va_end(vargs);
}

void
CCeilLErrorMgr::
expressionError(int error_code, const char *format, va_list *vargs)
{
  CStrUtil::vsprintf(error_string_, format, vargs);

  if (! ClSignalMgrInst->checkSignalCommand(SIGEXPR)) {
    const string &input_file = ClLanguageMgrInst->getInputFile().getName();

    if (error_proc_ != NULL) {
      string str;

      if (error_code != -1) {
        if (input_file != "")
          CStrUtil::sprintf(str, "Expression Error in %s:%d - %s (%s)\n",
                            input_file.c_str(), ClLanguageMgrInst->getCommandLineNum(),
                            error_string_.c_str(), ClParserInst->getErrorMessage(error_code));
        else
          CStrUtil::sprintf(str, "Expression Error - %s (%s)\n",
                            error_string_.c_str(), ClParserInst->getErrorMessage(error_code));
      }
      else {
        if (input_file != "")
          CStrUtil::sprintf(str, "Expression Error in %s:%d - %s\n",
                            input_file.c_str(), ClLanguageMgrInst->getCommandLineNum(),
                            error_string_.c_str());
        else
          CStrUtil::sprintf(str, "Expression Error - %s\n", error_string_.c_str());
      }

      (*error_proc_)(str.c_str(), error_data_);
    }
    else {
      if (error_fp_ == NULL)
        error_fp_ = stderr;

      if (error_code != -1) {
        if (input_file != "")
          fprintf(error_fp_, "Expression Error in %s:%d - %s (%s)\n",
                  input_file.c_str(), ClLanguageMgrInst->getCommandLineNum(),
                  error_string_.c_str(), ClParserInst->getErrorMessage(error_code));
        else
          fprintf(error_fp_, "Expression Error - %s (%s)\n",
                  error_string_.c_str(), ClParserInst->getErrorMessage(error_code));
      }
      else {
        if (input_file != "")
          fprintf(error_fp_, "Expression Error in %s:%d - %s\n",
                  input_file.c_str(), ClLanguageMgrInst->getCommandLineNum(),
                  error_string_.c_str());
        else
          fprintf(error_fp_, "Expression Error - %s\n", error_string_.c_str());
      }
    }
  }
  else {
    ClParserInst->createVar("_error_string", ClParserValueMgrInst->createValue(error_string_));

    ClSignalMgrInst->executeSignalCommand(SIGEXPR);
  }

  error_flag_ = true;
}

/*------------------------------------------------------------------*
 *
 * setErrorFile
 *   Set the output file to which error messages are sent
 *   to the specified file pointer.
 *
 * CALL:
 *   FILE *old_fp = setErrorFile(FILE *fp);
 *
 * INPUT:
 *   fp     : File pointer to use for error output.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   old_fp : The previous file pointer used for error output.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

FILE *
CCeilLErrorMgr::
setErrorFile(FILE *fp)
{
  if (error_fp_ == NULL)
    error_fp_ = stderr;

  FILE *old_fp = error_fp_;

  if (fp != NULL)
    error_fp_ = fp;

  return old_fp;
}


/*------------------------------------------------------------------*
 *
 * setErrorProc
 *   Specify that all error messages will be passed to the
 *   supplied error procedure.
 *
 * CALL:
 *   ClLanguageErrorProc old_proc =
 *     setErrorProc(ClLanguageErrorProc proc, void *data);
 *
 * INPUT:
 *   proc     : Procedure to send error messages to.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   old_proc : Previous procedure to send error messages to.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

ClLanguageErrorProc
CCeilLErrorMgr::
setErrorProc(ClLanguageErrorProc proc, void *data)
{
  ClLanguageErrorProc old_proc = error_proc_;

  if (proc != NULL) {
    error_proc_ = proc;
    error_data_ = data;
  }

  return old_proc;
}
