#ifndef CCEILP_PROCESS_H
#define CCEILP_PROCESS_H

#include <CCeilPValue.h>

/**** Private Routines ****/

extern ClParserValuePtr ClParserProcessInlineOperator
                         (ClParserOperatorPtr, ClParserVarRefPtr, int, int *);
extern ClParserValuePtr ClParserSubProcessInlineOperator
                         (ClParserOperatorPtr, ClParserValuePtr, int *);
extern ClParserValuePtr ClParserProcessUnaryOperator
                         (ClParserOperatorPtr, ClParserValuePtr, int *);
extern ClParserValuePtr ClParserProcessBinaryOperator
                         (ClParserValuePtr, ClParserOperatorPtr, ClParserValuePtr, int *);
extern void             ClParserProcessAssignmentOperator
                         (ClParserVarRefPtr, ClParserOperatorPtr, ClParserValuePtr, int *);
extern void             ClParserProcessAssignmentOperator
                         (ClParserStructVarRefPtr, ClParserOperatorPtr, ClParserValuePtr, int *);
extern ClParserValuePtr ClParserProcessInternFn
                         (ClParserInternFnPtr, const ClParserValueArray &, int *);
extern ClParserValuePtr ClParserProcessAddrCommand
                         (const ClParserValueArray &, int *);
extern ClParserValuePtr ClParserProcessWhere
                         (const std::string &variables_string, const std::string &expression,
                          int *error_code);
extern ClParserValuePtr ClParserProcessUserFn
                         (ClParserUserFnPtr, const ClParserArgValueArray &, int *);
extern void             ClParserCheckUnaryValue
                         (ClParserValuePtr, int, int *);

#endif
