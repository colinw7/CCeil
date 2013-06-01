#include <CCeilL.h>

#include <cstdlib>

using std::string;

ClLanguageCommand::
ClLanguageCommand(uint ident, void *data, const string &args,
                  const ClLanguageCommandList &command_list) :
 ident_(ident), data_(data), args_(args), command_list_(command_list)
{
  line_num_ = ClLanguageMgrInst->getInputFile().getLineNum();
}

ClLanguageCommand::
~ClLanguageCommand()
{
  command_list_.clear();
}

void
ClLanguageCommand::
assign(const ClLanguageCommand &command)
{
  ident_        = command.ident_;
  data_         = command.data_;
  args_         = command.args_;
  command_list_ = command.command_list_;
  line_num_     = command.line_num_;
}

void
ClLanguageCommand::
addCommand(ClLanguageCommand *command)
{
  command_list_.push_back(command);
}

void
ClLanguageCommand::
clearCommands()
{
  command_list_.clear();
}

//----------

ClLanguageSystemCommand::
ClLanguageSystemCommand(const string &args) :
 ClLanguageCommand(CL_SYSTEM_COMMAND, NULL, args)
{
  init();
}

ClLanguageSystemCommand::
~ClLanguageSystemCommand()
{
  term();
}

string
ClLanguageSystemCommand::
toName() const
{
  return args_;
}

//----------

ClLanguageProcCommand::
ClLanguageProcCommand(ClLanguageProc *proc, const string &args) :
 ClLanguageCommand(CL_PROCEDURE_COMMAND, proc, args, proc->getCommandList())
{
  init();
}

ClLanguageProcCommand::
~ClLanguageProcCommand()
{
  term();
}

void
ClLanguageProcCommand::
init()
{
  getProcedure()->incRef();
}

void
ClLanguageProcCommand::
term()
{
  getProcedure()->defRef();
}

ClLanguageProc *
ClLanguageProcCommand::
getProcedure() const
{
  return (ClLanguageProc *) data_;
}

string
ClLanguageProcCommand::
toName() const
{
  return getProcedure()->getName();
}

//----------

ClLanguagePosProcCommand::
ClLanguagePosProcCommand(const string &name, const string &line) :
 ClLanguageCommand(CL_POS_PROC_COMMAND, strdup(name.c_str()), line)
{
  init();
}

ClLanguagePosProcCommand::
~ClLanguagePosProcCommand()
{
  term();
}

void
ClLanguagePosProcCommand::
assign(const ClLanguageCommand &command)
{
  free((char *) data_);

  ClLanguageCommand::assign(command);

  data_ = strdup((char *) command.getData());
}

void
ClLanguagePosProcCommand::
term()
{
  free((char *) data_);
}

string
ClLanguagePosProcCommand::
toName() const
{
  return string((char *) data_);
}

const string &
ClLanguagePosProcCommand::
getLine() const
{
  return args_;
}

//----------

ClLanguageLabelCommand::
ClLanguageLabelCommand(ClLanguageLabelData *label_data) :
 ClLanguageCommand(CL_LABEL_COMMAND, label_data)
{
  init();
}

ClLanguageLabelCommand::
~ClLanguageLabelCommand()
{
  term();
}

string
ClLanguageLabelCommand::
toName() const
{
  return getLabelData()->getName();
}

ClLanguageLabelData *
ClLanguageLabelCommand::
getLabelData() const
{
  return ((ClLanguageLabelData *) data_);
}

//----------

ClLanguageExpressionCommand::
ClLanguageExpressionCommand(const string &name, const string &line) :
 ClLanguageCommand(CL_EXPRESSION_COMMAND, strdup(name.c_str()), line)
{
  init();
}

ClLanguageExpressionCommand::
~ClLanguageExpressionCommand()
{
  term();
}

void
ClLanguageExpressionCommand::
assign(const ClLanguageCommand &command)
{
  free((char *) data_);

  ClLanguageCommand::assign(command);

  data_ = strdup((char *) command.getData());
}

void
ClLanguageExpressionCommand::
term()
{
  free((char *) data_);
}

string
ClLanguageExpressionCommand::
toName() const
{
  return string((char *) data_);
}

//----------

ClLanguageLanguageCommand::
ClLanguageLanguageCommand(const string &args) :
 ClLanguageCommand(CL_LANGUAGE_COMMAND, NULL, args)
{
  init();
}

ClLanguageLanguageCommand::
~ClLanguageLanguageCommand()
{
  term();
}

string
ClLanguageLanguageCommand::
toName() const
{
  return args_;
}

//----------

ClLanguageCommandDefCommand::
ClLanguageCommandDefCommand(ClLanguageCommandDef *command_def,
                            const string &args, bool is_end_cmd) :
 ClLanguageCommand(command_def->getIdent(), command_def, args)
{
  if (is_end_cmd)
    ident_ |= CL_END_BLOCK_COMMAND;

  init();
}

ClLanguageCommandDefCommand::
~ClLanguageCommandDefCommand()
{
  term();
}

string
ClLanguageCommandDefCommand::
toName() const
{
  return getCommandDef()->getName();
}

ClLanguageCommandDef *
ClLanguageCommandDefCommand::
getCommandDef() const
{
  return ((ClLanguageCommandDef *) data_);
}
