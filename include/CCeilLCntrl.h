#ifndef CL_L_CONTROL_H
#define CL_L_CONTROL_H

#ifdef CEIL_READLINE
#include <CReadLine.h>
#endif

#define ClLanguageMgrInst ClLanguageMgr::getInstance()

class ClLanguageInputFile {
 public:
  ClLanguageInputFile();

  ClLanguageInputFile(const ClLanguageInputFile &file) :
   name_(file.name_), fp_(file.fp_), line_num_(file.line_num_) {
  }

  ClLanguageInputFile &operator=(const ClLanguageInputFile &file) {
    name_     = file.name_;
    fp_       = file.fp_;
    line_num_ = file.line_num_;

    return *this;
  }

  FILE              *getFp() const { return fp_; }
  const std::string &getName() const { return name_; }
  int                getLineNum() const { return line_num_; }

  bool setName(const std::string &name);
  void setFp(FILE *fp);
  void setLineNum(int line_num) { line_num_ = line_num; }

  bool readLine(std::string &line);

 private:
  std::string  name_;
  FILE        *fp_ { nullptr };
  int          line_num_ { 0 };
};

//------

class ClLanguageGoto {
 public:
  ClLanguageGoto() :
   flag_(false), depth_(0) {
  }

  bool               getFlag () const { return flag_; }
  const std::string &getName () const { return name_; }
  bool               getDepth() const { return depth_; }

  void set(const std::string &name, int depth) {
    flag_  = true;
    name_  = name;
    depth_ = depth;
  }

  void setFlag (int flag) { flag_ = flag; }
  void setName (const std::string &name) { name_ = name; }
  void setDepth(int depth) { depth_ = depth; }

 private:
  bool        flag_ { false };
  std::string name_;
  int         depth_ { 0 };
};

//------

#define LANGUAGE_EXIT         1
#define LANGUAGE_GOTO_INSIDE  2
#define LANGUAGE_GOTO_OUTSIDE 3
#define LANGUAGE_INTERRUPT    4
#define LANGUAGE_BREAK        5
#define LANGUAGE_CONTINUE     6
#define LANGUAGE_RETURN       7
#define LANGUAGE_ERROR        8

class ClLanguageCommandDef {
 private:
  uint               ident_;
  std::string        name_;
  std::string        end_name_;
  ClParserScopePtr   scope_;
  ClBlockType        type_;
  ClLanguageCmdProc  function_;
  void              *data_;

 public:
  ClLanguageCommandDef(const std::string &name, const std::string &end_name, ClBlockType type,
                       ClLanguageCmdProc function, void *data);
  ClLanguageCommandDef(const std::string &name, ClLanguageCmdProc function, void *data);
  ClLanguageCommandDef(ClParserScopePtr scope, const std::string &name,
                       ClLanguageCmdProc function, void *data);

  uint getIdent() const { return ident_; }

  const std::string &getName() const { return name_; }

  const std::string &getEndName() const { return end_name_; }

  const ClParserScopePtr &getScope() const { return scope_; }

  ClBlockType getType() const { return type_; }

  ClLanguageCmdProc getFunction() const { return function_; }

  void runFunction(ClLanguageCommand *command);
};

//------

class ClLanguageMgr {
 public:
  typedef void (*CommandTermProc)(void *data);

 public:
  static ClLanguageMgr *getInstance();

  void initVars();

  void init(int *argc, char **argv);

  void reinit();

  void term();

  void promptLoop();

  void startup();

  void runFile(const std::string &file_name);

  bool readLine();

  bool runCommand(const std::string &command_string);

  ClLanguageCommand *processLine(const std::string &line);

  void processCommand(ClLanguageCommand *command);

  void runProcedure(ClLanguageProc *procedure, ClLanguageArgs *args);

  ClParserValuePtr runFunction(ClParserValuePtr *values, uint num_values,
                               void *data, int *error_code);

  void processExpression(ClLanguageCommand *command, ClLanguageArgs *args);

  int checkControlFlags(ClLanguageCommand **commands, int num_commands, int *ind);

  int checkInterrupt(ClLanguageCommand **commands, int num_commands);

  bool checkAbort();

  int getLabelIndex(const std::string &label, ClLanguageCommand **commands, int num_commands);

  void setArgv(int argc, char **argv);

  void setExitFlag(bool exit_flag) { exit_flag_ = exit_flag; }
  bool getExitFlag() const { return exit_flag_; }

  void setExitCode(int exit_code) { exit_code_ = exit_code; }
  int  getExitCode() const { return exit_code_; }

  void setInterruptFlag(bool intr_flag) { intr_flag_ = intr_flag; }
  bool getInterruptFlag() const { return intr_flag_; }

  void setBreakFlag(bool break_flag) { break_flag_ = break_flag; }
  bool getBreakFlag() const { return break_flag_; }

  void setContinueFlag(bool continue_flag) { continue_flag_ = continue_flag; }
  bool getContinueFlag() const { return continue_flag_; }

  void setReturnFlag(bool return_flag) { return_flag_ = return_flag; }
  bool getReturnFlag() const { return return_flag_; }

  void setHelpProc(ClLanguageHelpProc help_proc) { help_proc_ = help_proc; }
  ClLanguageHelpProc getHelpProc() const { return help_proc_; }

  void setCommandName(const std::string &name) { command_name_ = name; }
  const std::string &getCommandName() const { return command_name_; }

  void setCommandLineNum(int num) { command_line_num_ = num; }
  int  getCommandLineNum() const { return command_line_num_; }

  void incDepth() { ++depth_; }
  void decDepth() { --depth_; }
  void resetDepth() { depth_ = 0; }
  int  getDepth() const { return depth_; }

  void setInputFile(const ClLanguageInputFile &file) { input_file_ = file; }
  ClLanguageInputFile &getInputFile() { return input_file_; }

#ifdef CEIL_READLINE
  CReadLine &getReadLine() { return readline_; }
#endif

  ClLanguageGoto &getGoto() { return goto_; }

  FILE *setOutputFp(FILE *output_fp);
  FILE *getOutputFp() const { return output_fp_; }

  ClParser::OutputProc setOutputProc(ClParser::OutputProc proc, void *data);
  ClParser::OutputProc getOutputProc() const;

  void setRunFile(const std::string &run_file) { run_file_ = run_file; }
  const std::string &getRunFile() const { return run_file_; }

  void setExitAfterRun(bool flag) { exit_after_run_ = flag; }
  bool getExitAfterRun() const { return exit_after_run_; }

  void setEchoCommands(bool flag) { echo_commands_ = flag; }
  bool getEchoCommands() const { return echo_commands_; }

  uint nextIdent() { return ident_++; }

  uint defineBlockCommand(const std::string &name, const std::string &end_name,
                          ClBlockType type, ClLanguageCmdProc proc, void *data=NULL);

  uint defineCommand(const std::string &name, ClLanguageCmdProc proc, void *data=NULL);

  uint defineCommand(ClParserScopePtr scope, const std::string &name,
                     ClLanguageCmdProc proc, void *data=NULL);

  void addCommandDef(ClLanguageCommandDef *command_def);
  void addCommandDef(ClParserScopePtr scope, ClLanguageCommandDef *command_def);

  ClLanguageCommandDef *getCommandDef(ClParserScopePtr scope, const std::string &name,
                                      bool *is_end_name);

  void deleteCommandDefs();

  void removeCommandDef(ClParserScopePtr scope, ClLanguageCommandDef *command_def);

  void setBlockCommand(ClLanguageCommand *command) { block_command_ = command; }
  ClLanguageCommand *getBlockCommand() const { return block_command_; }

  ClLanguageCommand *startBlockCommand(ClLanguageCommand *command);
  ClLanguageCommand *endBlockCommand();

  void clearBlockCommandStack(bool report_error);

  void setFirstLine(bool flag) { first_line_ = flag; }
  bool getFirstLine() const { return first_line_; }

  void setPrompt(const std::string &prompt) { prompt_ = prompt; }
  const std::string &getPrompt() const { return prompt_; }

  void setIgnorePosProc(bool flag) { ignore_pos_proc_ = flag; }
  bool getIgnorePosProc() const { return ignore_pos_proc_; }

  void setLine(const std::string &line) { line_ = line; }
  const std::string &getLine() const { return line_; }
  void appendLine(const std::string &line) { line_ += line; }

  void setLineBuffer(const std::string &str) { line_buffer_ = str; }
  const std::string &getLineBuffer() const { return line_buffer_; }

  void setLastValue(ClParserValuePtr value) { last_value_ = value; }
  ClParserValuePtr getLastValue() const { return last_value_; }

  void resetLastValue() { setLastValue(ClParserValuePtr()); }

  void setAbortSignal(bool abort_signal) { abort_signal_ = abort_signal; }
  bool getAbortSignal() const { return abort_signal_; }

  void removeCommandLabels(ClLanguageCommand **commands, int num_commands);
  void addCommandLabels(ClLanguageCommand **commands, int num_commands);

  void commandListToArray(const ClLanguageCommandList &command_list,
                          ClLanguageCommand ***commands, int *num_commands);

  void printCommands(const std::string &subject);

  void outputPrompt();

  void output(const char *format, ...);
  void voutput(const char *format, va_list *vargs);

  void error(const char *format, ...);
  void expressionError(int error_code, const char *format, ...);
  void syntaxError(const char *format, ...);

  FILE *setErrorFile(FILE *fp) {
    return errorMgr_->setErrorFile(fp);
  }

  ClLanguageErrorProc setErrorProc(ClLanguageErrorProc proc, void *data) {
    return errorMgr_->setErrorProc(proc, data);
  }

  void setErrorFlag(bool flag) { errorMgr_->setErrorFlag(flag); }
  bool getErrorFlag() const { return errorMgr_->getErrorFlag(); }

  void setCommandTermProc(CommandTermProc term_proc, void *term_data) {
    command_term_proc_ = term_proc;
    command_term_data_ = term_data;
  }

 private:
  ClLanguageMgr();
 ~ClLanguageMgr();

  void updateArgvVar();

 private:
  typedef std::map<std::string, ClLanguageCommandDef *> CommandDefMap;

  int                     argc_ { 0 };
  char                  **argv_ { nullptr };
  bool                    exit_flag_ { false };
  int                     exit_code_ { 0 };
  bool                    intr_flag_ { false };
  bool                    break_flag_ { false };
  bool                    continue_flag_ { false };
  bool                    return_flag_ { false };
  ClLanguageHelpProc      help_proc_ { nullptr };
  std::string             command_name_;
  int                     command_line_num_ { 0 };
  int                     depth_ { 0 };
  ClLanguageInputFile     input_file_;
#ifdef CEIL_READLINE
  CReadLine               readline_;
#endif
  ClLanguageGoto          goto_;
  FILE                   *output_fp_ { nullptr };
  std::string             run_file_;
  bool                    exit_after_run_ { false };
  bool                    echo_commands_ { false };
  uint                    ident_ { 1 };
  CommandDefMap           command_def_list1_;
  CommandDefMap           command_def_list2_;
  ClLanguageCommand      *block_command_ { nullptr };
  ClLanguageCommandList   block_command_stack_;
  bool                    first_line_ { true };
  std::string             prompt_;
  bool                    ignore_pos_proc_ { false };
  std::string             line_;
  int                     line_len_ { 0 };
  std::string             line_buffer_;
  ClParserValuePtr        last_value_;
  bool                    abort_signal_ { false };
  CCeilLErrorMgr*         errorMgr_ { nullptr };
  CommandTermProc         command_term_proc_ { nullptr };
  void                   *command_term_data_ { nullptr };
};

#endif
