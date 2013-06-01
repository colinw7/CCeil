#include <CCeil.h>
#include <cstdlib>

#include <CCeilPI.h>

/*------------------------------------------------------------------*
 *
 * main (ceil.test)
 *   Test Harness for CL which provides a basic command
 *   line interface to the CL third generation commands
 *   and expression evaluator.
 *
 * CALL:
 *   ceil.test <language_args>
 *
 * INPUT:
 *   <language_args> : See ClLanguageInit() routine
 *
 *------------------------------------------------------------------*/

int
main(int argc, char **argv)
{
  /* Initialise Language and Parser */

  ClLanguageMgrInst->init(&argc, argv);

  /* Turn Real Parsing on if Environment Variable set */

  if (getenv("CL_PARSE_REAL"))
    ClParserInst->setParseAsReals(true);

  if (getenv("CL_DOLLAR_PREFIX"))
    ClParserInst->setDollarPrefix(true);

  /* Start Command Line Interface
     (returns when user enters 'exit') */

  ClLanguageMgrInst->promptLoop();

  /* Terminate Language and Parser */

  ClLanguageMgrInst->term();

  exit(ClLanguageMgrInst->getExitCode());
}
