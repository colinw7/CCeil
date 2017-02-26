#ifndef CL_L_COMMAND_H
#define CL_L_COMMAND_H

#define CL_SPECIAL_COMMAND   (1<<29)
#define CL_END_BLOCK_COMMAND (1<<30)

#define CL_PROCEDURE_COMMAND  (CL_SPECIAL_COMMAND + 1)
#define CL_LABEL_COMMAND      (CL_SPECIAL_COMMAND + 2)
#define CL_EXPRESSION_COMMAND (CL_SPECIAL_COMMAND + 3)
#define CL_SYSTEM_COMMAND     (CL_SPECIAL_COMMAND + 4)
#define CL_DUMMY_COMMAND      (CL_SPECIAL_COMMAND + 5)
#define CL_POS_PROC_COMMAND   (CL_SPECIAL_COMMAND + 6)
#define CL_LANGUAGE_COMMAND   (CL_SPECIAL_COMMAND + 7)

class ClLanguageProc;
class ClLanguageLabelData;
class ClLanguageCommandDef;

class ClLanguageCommand {
 protected:
  uint                   ident_;
  void                  *data_;
  std::string            args_;
  ClLanguageCommandList  command_list_;
  uint                   line_num_;

 public:
  ClLanguageCommand(uint ident, void *data=NULL, const std::string &args="",
                    const ClLanguageCommandList &command_list=ClLanguageCommandList());

  virtual ~ClLanguageCommand();

  virtual void assign(const ClLanguageCommand &command);

  void *getData() const { return data_; }

  uint getLineNum() const { return line_num_; }

  bool isSystem      () const { return (ident_ == CL_SYSTEM_COMMAND    ); }
  bool isProcedure   () const { return (ident_ == CL_PROCEDURE_COMMAND ); }
  bool isPosProcedure() const { return (ident_ == CL_POS_PROC_COMMAND  ); }
  bool isLabel       () const { return (ident_ == CL_LABEL_COMMAND     ); }
  bool isExpression  () const { return (ident_ == CL_EXPRESSION_COMMAND); }
  bool isLanguage    () const { return (ident_ == CL_LANGUAGE_COMMAND  ); }
  bool isSpecial     () const { return (ident_ &  CL_SPECIAL_COMMAND   ); }
  bool isEndBlock    () const { return (ident_ &  CL_END_BLOCK_COMMAND ); }

  bool isIdent(uint ident) const { return (ident_ == ident); }

  bool isIdentNoEnd(uint ident) const {
    return ((ident_ & (~CL_END_BLOCK_COMMAND)) == ident);
  }

  bool isIdentNoEnd(ClLanguageCommand *command) const {
    int ident1 = (ident_          & (~CL_END_BLOCK_COMMAND));
    int ident2 = (command->ident_ & (~CL_END_BLOCK_COMMAND));

    return (ident1 == ident2);
  }

  virtual void init() = 0;
  virtual void term() = 0;

  virtual std::string toName() const = 0;

  const std::string &getArgs() const { return args_; }

  const ClLanguageCommandList &getCommandList() const { return command_list_; }

  void addCommand(ClLanguageCommand *command);

  void clearCommands();

 private:
  ClLanguageCommand(const ClLanguageCommand &command);

  const ClLanguageCommand &operator=(const ClLanguageCommand &command);
};

class ClLanguageSystemCommand : public ClLanguageCommand {
 public:
  explicit ClLanguageSystemCommand(const std::string &args);

 ~ClLanguageSystemCommand();

  void init() { }
  void term() { }

  std::string toName() const;
};

class ClLanguageProcCommand : public ClLanguageCommand {
 public:
  ClLanguageProcCommand(ClLanguageProc *proc, const std::string &args);

 ~ClLanguageProcCommand();

  void init();
  void term();

  ClLanguageProc *getProcedure() const;

  std::string toName() const;
};

class ClLanguagePosProcCommand : public ClLanguageCommand {
 public:
  ClLanguagePosProcCommand(const std::string &name, const std::string &line);

 ~ClLanguagePosProcCommand();

  void assign(const ClLanguageCommand &command);

  void init() { }
  void term();

  std::string toName() const;

  const std::string &getLine() const;
};

class ClLanguageLabelCommand : public ClLanguageCommand {
 public:
  explicit ClLanguageLabelCommand(ClLanguageLabelData *label_data);

 ~ClLanguageLabelCommand();

  void init() { }
  void term() { }

  std::string toName() const;

  ClLanguageLabelData *getLabelData() const;
};

class ClLanguageExpressionCommand : public ClLanguageCommand {
 public:
  ClLanguageExpressionCommand(const std::string &name, const std::string &line);

 ~ClLanguageExpressionCommand();

  void assign(const ClLanguageCommand &command);

  void init() { }
  void term();

  std::string toName() const;
};

class ClLanguageLanguageCommand : public ClLanguageCommand {
 public:
  explicit ClLanguageLanguageCommand(const std::string &args);

 ~ClLanguageLanguageCommand();

  void init() { }
  void term() { }

  std::string toName() const;
};

class ClLanguageCommandDefCommand : public ClLanguageCommand {
 public:
  ClLanguageCommandDefCommand(ClLanguageCommandDef *command_def,
                              const std::string &args, bool is_end_cmd);

 ~ClLanguageCommandDefCommand();

  void init() { }
  void term() { }

  std::string toName() const;

  ClLanguageCommandDef *getCommandDef() const;
};

#endif
