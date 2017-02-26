#include <CCeilL.h>

using std::string;

/*------------------------------------------------------------------*
 *
 * startBlock
 *   Initialise Environment for Processing the Command List of a Block Command.
 *   How the program flow commands 'break', 'continue', 'goto', 'return' and 'exit'
 *   and interrupts are handled is determined by the type of block being processed.
 *
 * CALL:
 *   startBlock(ClLanguageCommandList *command_list, ClBlockType type);
 *
 * INPUT:
 *   command_list  : List of commands to be executed by
 *                 : the block command.
 *
 *   type          : Type of block command :-
 *                 :  CL_BLOCK_TYPE_NORMAL_BLOCK -
 *                 :   Command List is executed once when the block command is entered.
 *                 :  CL_BLOCK_TYPE_ITERATION_BLOCK -
 *                 :   Command List is one or more times when the block command is entered.
 *                 :  CL_BLOCK_TYPE_PROCEDURE_BLOCK -
 *                 :   Command List is saved and executed later.
 *                 :  CL_BLOCK_TYPE_FILE_BLOCK -
 *                 :   Command List is executed once as if it was not part of the current
 *                 :   execution environment.
 *
 * NOTES:
 *   This routine is called automatically when a block command is started.
 *
 *------------------------------------------------------------------*/

void
ClLanguageBlockDataMgr::
startBlock(const ClLanguageCommandList &command_list, ClBlockType type)
{
  /* Save any active block command's environment */

  if (current_block_data_.getCommands() != NULL) {
    ClLanguageBlockData *block_data = new ClLanguageBlockData(current_block_data_);

    block_data_stack_.push_back(block_data);
  }

  /*----------*/

  /* Set up current block command's environment */

  const string &command_name = ClLanguageMgrInst->getCommandName();

  current_block_data_ =
    ClLanguageBlockData((ClBlockType) (type & CL_BLOCK_TYPE_BLOCK_MASK), command_name);

  current_block_data_.setCommandList(command_list);

  /*----------*/

  /* Increment the Depth */

  ClLanguageMgrInst->incDepth();

  /*----------*/

  /* Add the Block's Labels to the Current Label List */

  current_block_data_.addCommandLabels();

  /*----------*/

  /* Ensure program control flags are reset before running the
     command list */

  if      (current_block_data_.isType(CL_BLOCK_TYPE_ITERATION_BLOCK)) {
    ClLanguageMgrInst->setBreakFlag   (false);
    ClLanguageMgrInst->setContinueFlag(false);

    ClLanguageMgrInst->setErrorFlag(false);
  }
  else if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK)) {
    ClLanguageMgrInst->getGoto().setFlag(false);

    ClLanguageMgrInst->setInterruptFlag(false);
    ClLanguageMgrInst->setBreakFlag    (false);
    ClLanguageMgrInst->setContinueFlag (false);
    ClLanguageMgrInst->setReturnFlag   (false);

    ClLanguageMgrInst->setErrorFlag(false);
  }
  else if (current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK)) {
    ClLanguageMgrInst->getGoto().setFlag(false);

    ClLanguageMgrInst->setInterruptFlag(false);
    ClLanguageMgrInst->setBreakFlag    (false);
    ClLanguageMgrInst->setContinueFlag (false);
    ClLanguageMgrInst->setReturnFlag   (false);
    ClLanguageMgrInst->setExitFlag     (false);
  }
}

/*------------------------------------------------------------------*
 *
 * endBlock
 *   Terminate Environment for Processing the Command List of
 *   a Block Command initialised by startBlock().
 *
 * CALL:
 *   endBlock();
 *
 * NOTES:
 *   This routine is called automatically when a block
 *   command is terminated.
 *
 *------------------------------------------------------------------*/

void
ClLanguageBlockDataMgr::
endBlock()
{
  /* Remove Block's Labels from the Current Label List */

  current_block_data_.removeCommandLabels();

  /*-------------*/

  /* Reset return flag on exit of procedure */

  if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK))
    ClLanguageMgrInst->setReturnFlag(false);

  /*-------------*/

  /* Decrement the Depth */

  ClLanguageMgrInst->decDepth();

  if (ClLanguageMgrInst->getDepth() == 0) {
    if (ClLanguageMgrInst->getBreakFlag())
      ClLanguageMgrInst->syntaxError("Unhandled 'break' command");

    if (ClLanguageMgrInst->getContinueFlag())
      ClLanguageMgrInst->syntaxError("Unhandled 'continue' command");

    if (ClLanguageMgrInst->getReturnFlag())
      ClLanguageMgrInst->syntaxError("Unhandled 'return' command");

    ClLanguageMgrInst->setBreakFlag   (false);
    ClLanguageMgrInst->setContinueFlag(false);
    ClLanguageMgrInst->setReturnFlag  (false);
  }

  /*-------------*/

  /* Ensure program control flags are reset after running the
     command list */

  if      (current_block_data_.isType(CL_BLOCK_TYPE_ITERATION_BLOCK)) {
    ClLanguageMgrInst->setBreakFlag   (false);
    ClLanguageMgrInst->setContinueFlag(false);

    ClLanguageMgrInst->setErrorFlag(false);
  }
  else if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK)) {
    ClLanguageMgrInst->getGoto().setFlag(false);

    ClLanguageMgrInst->setInterruptFlag(false);
    ClLanguageMgrInst->setBreakFlag    (false);
    ClLanguageMgrInst->setContinueFlag (false);

    ClLanguageMgrInst->setErrorFlag(false);
  }
  else if (current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK)) {
    ClLanguageMgrInst->getGoto().setFlag(false);

    ClLanguageMgrInst->setInterruptFlag(false);
    ClLanguageMgrInst->setBreakFlag    (false);
    ClLanguageMgrInst->setContinueFlag (false);
    ClLanguageMgrInst->setReturnFlag   (false);
    ClLanguageMgrInst->setExitFlag     (false);

    ClLanguageMgrInst->setErrorFlag(false);
  }

  /* Free resources allocated by current block command */

  current_block_data_.setCommandList(ClLanguageCommandList());

  /* Restore previous block command (if any)*/

  ClLanguageBlockData *block_data = NULL;

  if (! block_data_stack_.empty()) {
    block_data = block_data_stack_.back();

    block_data_stack_.pop_back();
  }

  if (block_data != NULL) {
    current_block_data_ = *block_data;

    delete block_data;
  }
  else
    current_block_data_.init();
}

/*------------------------------------------------------------------*
 *
 * setBlockCommands
 *   Reset the Current Block Command's Command List.
 *
 * CALL:
 *   setBlockCommands(const ClLanguageCommandList &command_list);
 *
 * INPUT:
 *   command_list : List of commands to be executed by
 *                : the block command.
 *
 *------------------------------------------------------------------*/

void
ClLanguageBlockDataMgr::
setBlockCommands(const ClLanguageCommandList &command_list)
{
  /* Remove Original Command List's Labels from the Current Label List */

  current_block_data_.removeCommandLabels();

  /*----------*/

  /* Set up new block command list */

  current_block_data_.setCommandList(command_list);

  /*----------*/

  /* Add the Block's Labels to the Current Label List */

  current_block_data_.addCommandLabels();
}

/*------------------------------------------------------------------*
 *
 * processBlockCommands
 *   Process the Command List specified by the call to startBlock().
 *   This will execute the commands and handle and program flow
 *   commands and check for interrupts. The routine will return
 *   whether an error occured.
 *
 * CALL:
 *   bool flag = processBlockCommands();
 *
 * RETURNS:
 *   flag : Whether executing of the current command
 *        : should be abandoned as an error has occurred
 *        : or program flow has moved to another command.
 *
 * NOTES:
 *   The return flag is only really of use to iteration
 *   block commands where the command list may be executed
 *   many times.
 *
 *------------------------------------------------------------------*/

int
ClLanguageBlockDataMgr::
processBlockCommands()
{
  int i, type;

  bool flag = false;

  int num_commands = current_block_data_.getNumCommands();

  /* Add Command Labels for Procedure Block */

  if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK))
    current_block_data_.addCommandLabels();

  /* Process Empty Command List */

  if (num_commands == 0) {
    /* Check whether the last executed command was a program flow
       command and update the current command number for a goto
       to a label inside the block */

    int type = current_block_data_.checkControlFlags(&i);

    /* Exit Command stops execution of all commands at all levels
       in the current file or terminates the interactive session */

    if      (type == LANGUAGE_EXIT || type == LANGUAGE_ERROR)
      flag = true;

    /* Return terminates a procedure */

    else if (type == LANGUAGE_RETURN) {
      if (current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK))
        ClLanguageMgrInst->syntaxError
         ("Invalid 'return' command when executing file '%s'",
          current_block_data_.getName().c_str());

      flag = true;
    }

    /* Goto to a label outside the block or an interrupt is not
       valid for a file or a procedure. Other types terminate
       and leave parent block to handle */

    else if (type == LANGUAGE_GOTO_OUTSIDE ||
             type == LANGUAGE_INTERRUPT) {
      if      (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK)) {
        if (type == LANGUAGE_GOTO_OUTSIDE)
          ClLanguageMgrInst->syntaxError
           ("Invalid 'goto' to label '%s' outside procedure '%s'",
            ClLanguageMgrInst->getGoto().getName().c_str(),
            current_block_data_.getName().c_str());
        else
          ClLanguageMgrInst->syntaxError
           ("'interrupt' in procedure '%s' ignored",
            current_block_data_.getName().c_str());
      }
      else if (current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK)) {
        if (type == LANGUAGE_GOTO_OUTSIDE)
          ClLanguageMgrInst->syntaxError
           ("Invalid 'goto' to label '%s' outside file '%s'",
            ClLanguageMgrInst->getGoto().getName().c_str(),
            current_block_data_.getName().c_str());
        else
          ClLanguageMgrInst->syntaxError
           ("'interrupt' in file '%s' ignored",
            current_block_data_.getName().c_str());
      }

      flag = true;
    }

    /* Break terminates an iteration, otherwise invalid */

    else if (type == LANGUAGE_BREAK) {
      if      (current_block_data_.isType(CL_BLOCK_TYPE_NORMAL_BLOCK))
        ClLanguageMgrInst->setBreakFlag(true);
      else if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK) ||
               current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK))
        ClLanguageMgrInst->syntaxError
         ("'break' encountered outside Iteration Block in '%s'",
          current_block_data_.getName().c_str());

      flag = true;
    }

    /* Continue starts next iteration (i.e. abandons execution
       of remaining commands and leaves caller to do the next
       iteration), otherwise invalid */

    else if (type == LANGUAGE_CONTINUE) {
      if      (current_block_data_.isType(CL_BLOCK_TYPE_NORMAL_BLOCK))
        ClLanguageMgrInst->setContinueFlag(true);
      else if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK) ||
               current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK)) {
        ClLanguageMgrInst->syntaxError
         ("'continue' encountered outside Iteration Block in '%s'",
          current_block_data_.getName().c_str());

        flag = true;
      }
    }

    goto processBlockCommands1;
  }

  /* Process Command List */

  i = 0;

  for ( ; i < num_commands; i++) {
 again:
    ClLanguageCommand *command = current_block_data_.getCommand(i);

    ClLanguageMgrInst->processCommand(command);

    /* Check whether the last executed command was a program flow
       command and update the current command number for a goto
       to a label inside the block */

    int type = current_block_data_.checkControlFlags(&i);

    /* Exit Command stops execution of all commands at all levels
       in the current file or terminates the interactive session */

    if      (type == LANGUAGE_EXIT || type == LANGUAGE_ERROR) {
      flag = true;

      goto processBlockCommands1;
    }

    /* Return terminates a procedure */

    else if (type == LANGUAGE_RETURN) {
      if (current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK))
        ClLanguageMgrInst->syntaxError
         ("Invalid 'return' command when executing file '%s'",
          current_block_data_.getName().c_str());

      flag = true;

      goto processBlockCommands1;
    }

    /* Goto to a label inside the block moves to the next command
       after the label and continues execution */

    else if (type == LANGUAGE_GOTO_INSIDE)
      goto again;

    /* Goto to a label outside the block or an interrupt is not
       valid for a file or a procedure. Other types terminate
       and leave parent block to handle */

    else if (type == LANGUAGE_GOTO_OUTSIDE ||
             type == LANGUAGE_INTERRUPT) {
      if      (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK)) {
        if (type == LANGUAGE_GOTO_OUTSIDE)
          ClLanguageMgrInst->syntaxError
           ("Invalid 'goto' to label '%s' outside procedure '%s'",
            ClLanguageMgrInst->getGoto().getName().c_str(),
            current_block_data_.getName().c_str());
        else
          ClLanguageMgrInst->syntaxError
           ("'interrupt' in procedure '%s' ignored",
            current_block_data_.getName().c_str());
      }
      else if (current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK)) {
        if (type == LANGUAGE_GOTO_OUTSIDE)
          ClLanguageMgrInst->syntaxError
           ("Invalid 'goto' to label '%s' outside file '%s'",
            ClLanguageMgrInst->getGoto().getName().c_str(),
            current_block_data_.getName().c_str());
        else
          ClLanguageMgrInst->syntaxError
           ("'interrupt' in file '%s' ignored",
            current_block_data_.getName().c_str());
      }

      flag = true;

      goto processBlockCommands1;
    }

    /* Break terminates an iteration, otherwise invalid */

    else if (type == LANGUAGE_BREAK) {
      if      (current_block_data_.isType(CL_BLOCK_TYPE_NORMAL_BLOCK))
        ClLanguageMgrInst->setBreakFlag(true);
      else if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK) ||
               current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK))
        ClLanguageMgrInst->syntaxError
         ("'break' encountered outside Iteration Block in '%s'",
          current_block_data_.getName().c_str());

      flag = true;

      goto processBlockCommands1;
    }

    /* Continue starts next iteration (i.e. abandons execution
       of remaining commands and leaves caller to do the next
       iteration), otherwise invalid */

    else if (type == LANGUAGE_CONTINUE) {
      if      (current_block_data_.isType(CL_BLOCK_TYPE_NORMAL_BLOCK))
        ClLanguageMgrInst->setContinueFlag(true);
      else if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK) ||
               current_block_data_.isType(CL_BLOCK_TYPE_FILE_BLOCK)) {
        ClLanguageMgrInst->syntaxError
         ("'continue' encountered outside Iteration Block in '%s'",
          current_block_data_.getName().c_str());

        flag = true;

        goto processBlockCommands1;
      }

      break;
    }
  }

  /* Handle User Interrupt */

  type = current_block_data_.checkInterrupt();

  if (type == LANGUAGE_INTERRUPT) {
    ClLanguageMgrInst->error("** Interrupt ** %s loop terminated",
                              current_block_data_.getName().c_str());

    flag = true;

    goto processBlockCommands1;
  }

 processBlockCommands1:
  /* Remove Command Labels for Procedure Block */

  if (current_block_data_.isType(CL_BLOCK_TYPE_PROCEDURE_BLOCK))
    current_block_data_.removeCommandLabels();

  return flag;
}

//-------------------------

ClLanguageBlockData::
ClLanguageBlockData()
{
  init();
}

ClLanguageBlockData::
ClLanguageBlockData(ClBlockType type, const string &name,
                    ClLanguageCommand **commands, int num_commands) :
 type_(type), name_(name), commands_(commands), num_commands_(num_commands)
{
}

ClLanguageBlockData::
ClLanguageBlockData(const ClLanguageBlockData &block_data)
{
  type_         = block_data.type_;
  name_         = block_data.name_;
  commands_     = block_data.commands_;
  num_commands_ = block_data.num_commands_;
}

ClLanguageBlockData &
ClLanguageBlockData::
operator=(const ClLanguageBlockData &block_data)
{
  type_         = block_data.type_;
  name_         = block_data.name_;
  commands_     = block_data.commands_;
  num_commands_ = block_data.num_commands_;

  return *this;
}

void
ClLanguageBlockData::
init()
{
  type_         = CL_BLOCK_TYPE_NORMAL_BLOCK;
  name_         = "";
  commands_     = NULL;
  num_commands_ = 0;
}

void
ClLanguageBlockData::
setCommandList(const ClLanguageCommandList &command_list)
{
  delete [] commands_;

  ClLanguageMgrInst->commandListToArray(command_list, &commands_, &num_commands_);
}

void
ClLanguageBlockData::
addCommandLabels() const
{
  ClLanguageMgrInst->addCommandLabels(commands_, num_commands_);
}

void
ClLanguageBlockData::
removeCommandLabels() const
{
  ClLanguageMgrInst->removeCommandLabels(commands_, num_commands_);
}

int
ClLanguageBlockData::
checkControlFlags(int *pos)
{
  return ClLanguageMgrInst->checkControlFlags(commands_, num_commands_, pos);
}

int
ClLanguageBlockData::
checkInterrupt()
{
  return ClLanguageMgrInst->checkInterrupt(commands_, num_commands_);
}
