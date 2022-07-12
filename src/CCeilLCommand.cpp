#include <CCeilL.h>
#include <cstdlib>

ClLanguageCommand::
ClLanguageCommand(uint ident, void *data, const std::string &args,
                  const ClLanguageCommandList &command_list) :
 ident_(ident), data_(data), args_(args), command_list_(command_list)
{
  line_num_ = ClLanguageMgrInst->getInputFile()->getLineNum();
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
  extraData_    = command.extraData_;
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
ClLanguageSystemCommand(const std::string &args) :
 ClLanguageCommand(CL_SYSTEM_COMMAND, nullptr, args)
{
  init();
}

ClLanguageSystemCommand::
~ClLanguageSystemCommand()
{
  term();
}

std::string
ClLanguageSystemCommand::
toName() const
{
  return args_;
}

//----------

ClLanguageProcCommand::
ClLanguageProcCommand(ClLanguageProc *proc, const std::string &args) :
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

std::string
ClLanguageProcCommand::
toName() const
{
  return getProcedure()->getName();
}

//----------

ClLanguagePosProcCommand::
ClLanguagePosProcCommand(const std::string &name, const std::string &line) :
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
  assert(command.isPosProcedure());

  free(data_);

  ClLanguageCommand::assign(command);

  data_ = strdup(reinterpret_cast<char *>(command.getData()));
}

void
ClLanguagePosProcCommand::
term()
{
  free(data_);
}

std::string
ClLanguagePosProcCommand::
toName() const
{
  return std::string(reinterpret_cast<char *>(data_));
}

const std::string &
ClLanguagePosProcCommand::
getArgs() const
{
  if (! extraData_)
    return ClLanguageCommand::getArgs();

  return ((ClLanguageCommand *) extraData_)->getArgs();
}

const std::string &
ClLanguagePosProcCommand::
getLine() const
{
  return args_;
}

ClLanguageProc *
ClLanguagePosProcCommand::
getProcedure() const
{
  if (! extraData_)
    return nullptr;

  return ((ClLanguageCommand *) extraData_)->getProcedure();
}

void
ClLanguagePosProcCommand::
setCommand(ClLanguageCommand *cmd)
{
  assert(! extraData_);

  // TODO: ref count ?
  extraData_ = cmd;
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

std::string
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
ClLanguageExpressionCommand(const std::string &name, const std::string &line) :
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
  free(data_);

  ClLanguageCommand::assign(command);

  data_ = strdup(reinterpret_cast<char *>(command.getData()));
}

void
ClLanguageExpressionCommand::
term()
{
  free(data_);
}

std::string
ClLanguageExpressionCommand::
toName() const
{
  return std::string(reinterpret_cast<char *>(data_));
}

//----------

ClLanguageLanguageCommand::
ClLanguageLanguageCommand(const std::string &args) :
 ClLanguageCommand(CL_LANGUAGE_COMMAND, nullptr, args)
{
  init();
}

ClLanguageLanguageCommand::
~ClLanguageLanguageCommand()
{
  term();
}

std::string
ClLanguageLanguageCommand::
toName() const
{
  return args_;
}

//----------

ClLanguageCommandDefCommand::
ClLanguageCommandDefCommand(ClLanguageCommandDef *command_def, const std::string &args,
                            bool is_end_cmd) :
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

std::string
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
