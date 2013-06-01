#include <CCeilL.h>

using std::string;

/*------------------------------------------------------------------*
 *
 * setDebug
 *   Turn on/off debugging for the language commands
 *   which when on will output a message when every
 *   non-trivial routine is entered and exited.
 *
 *   This routine only should be used for debugging.
 *
 * CALL:
 *   setDebug(bool flag);
 *
 * INPUT:
 *   flag  : Flag which specifies the required debugging state.
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
ClLanguageDebug::
setDebug(bool flag)
{
#ifdef CL_LANGUAGE_DEBUG
  trace_mgr_.setActive(flag);
#endif
}

/*------------------------------------------------------------------*
 *
 * setDebugFile
 *   Set the output file to which debug output is sent to
 *   the specified file pointer.
 *
 * CALL:
 *   FILE *old_fp = setDebugFile(FILE *fp);
 *
 * INPUT:
 *   fp     : File pointer to use for debug output.
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   old_fp : The previous file pointer used for debug output.
 *
 * NOTES:
 *   None
 *
 *------------------------------------------------------------------*/

FILE *
ClLanguageDebug::
setDebugFile(FILE *fp)
{
#ifdef CL_LANGUAGE_DEBUG
  return trace_mgr_.setOutput(fp);
#else
  return 0;
#endif
}

/*------------------------------------------------------------------*
 *
 * isDebug
 *   Check whether language command debugging is active.
 *
 * CALL:
 *   bool flag = isDebug();
 *
 * INPUT:
 *   None
 *
 * OUTPUT:
 *   None
 *
 * RETURNS:
 *   flag : Whether language command debugging is active.
 *
 * NOTES:
 *   Can be used by application routines to provide their
 *   own debug output.
 *
 *------------------------------------------------------------------*/

bool
ClLanguageDebug::
isDebug() const
{
#ifdef CL_LANGUAGE_DEBUG
  return trace_mgr_.getActive();
#else
  return false;
#endif
}

/*------------------------------------------------------------------*
 *
 * print
 *   Print the specified formatted str to the current
 *   debug output device.
 *
 * CALL:
 *   print(char *format, ...);
 *
 * INPUT:
 *   format : 'printf' style format str which will be
 *          : used to build the str.
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
 *   The str will be sent to the current debug output file.
 *
 *------------------------------------------------------------------*/

void
ClLanguageDebug::
print(const char *format, ...)
{
  string  msg;
  va_list vargs;

  va_start(vargs, format);

  CStrUtil::vsprintf(msg, format, &vargs);

  va_end(vargs);

#ifdef CL_LANGUAGE_DEBUG
  trace_mgr_.output(msg);
#endif
}
