#include <CCeil.h>
#include <CCeilP.h>
#include <CEnv.h>
#include <cstdlib>

/*------------------------------------------------------------------*
 *
 * main (CCeil)
 *   Test Harness for CL which provides a basic command
 *   line interface to the CL third generation commands
 *   and expression evaluator.
 *
 * CALL:
 *   CCeil <language_args>
 *
 * INPUT:
 *   <language_args> : See ClLanguageMgr::init() routine
 *
 *------------------------------------------------------------------*/

int
main(int argc, char **argv)
{
  // Initialise Language and Parser
  if (! ClLanguageMgrInst->init(&argc, argv))
    return 0;

  // Turn Real Parsing on if Environment Variable set
  if (CEnvInst.exists("CL_PARSE_REAL"))
    ClParserInst->setParseAsReals(true);

  if (CEnvInst.exists("CL_DOLLAR_PREFIX"))
    ClParserInst->setDollarPrefix(true);

  // Start Command Line Interface (returns when user enters 'exit')
  ClLanguageMgrInst->promptLoop();

  // Terminate Language and Parser
  ClLanguageMgrInst->term();

  exit(ClLanguageMgrInst->getExitCode());
}
