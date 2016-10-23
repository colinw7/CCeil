#ifndef CL_L_H
#define CL_L_H

#include <CCeilP.h>

/**** Public Types ****/

class ClLanguageCommand;
class ClLanguageArgs;

typedef std::vector<ClLanguageCommand *> ClLanguageCommandList;

typedef void (*ClLanguageHelpProc)
                (const char *name, const char *args);
typedef void (*ClLanguageCmdProc)
                (ClLanguageCommand *, ClLanguageArgs *, void *);
typedef void (*ClLanguageErrorProc)
                (const char *str, void *);
typedef int  (*ClLanguageConvertProc)
                (...);

#include <CCeilLArgs.h>
#include <CCeilLBlock.h>
#include <CCeilLFunc.h>
#include <CCeilLProc.h>
#include <CCeilLCommand.h>
#include <CCeilLLabel.h>
#include <CCeilLError.h>
#include <CCeilLCntrl.h>
#include <CCeilLConvert.h>
#include <CCeilLDebug.h>
#include <CCeilLGeneral.h>
#include <CCeilLModule.h>
#include <CCeilLSignal.h>

#endif
