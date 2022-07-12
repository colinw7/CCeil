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
 public:
  ClLanguageCommand(uint ident, void *data=nullptr, const std::string &args="",
                    const ClLanguageCommandList &command_list=ClLanguageCommandList());

  virtual ~ClLanguageCommand();

  virtual void assign(const ClLanguageCommand &command);

  virtual ClLanguageProc *getProcedure() const { return nullptr; }

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
    return ((ident_ & uint(~CL_END_BLOCK_COMMAND)) == ident);
  }

  bool isIdentNoEnd(ClLanguageCommand *command) const {
    uint ident1 = (ident_          & uint(~CL_END_BLOCK_COMMAND));
    uint ident2 = (command->ident_ & uint(~CL_END_BLOCK_COMMAND));

    return (ident1 == ident2);
  }

  virtual void init() = 0;
  virtual void term() = 0;

  virtual std::string toName() const = 0;

  virtual const std::string &getArgs() const { return args_; }

  const ClLanguageCommandList &getCommandList() const { return command_list_; }

  void addCommand(ClLanguageCommand *command);

  void clearCommands();

 private:
  ClLanguageCommand(const ClLanguageCommand &command);

  ClLanguageCommand &operator=(const ClLanguageCommand &command);

 protected:
  uint                   ident_     { 0 };
  void                  *data_      { nullptr };
  void                  *extraData_ { nullptr };
  std::string            args_;
  ClLanguageCommandList  command_list_;
  uint                   line_num_  { 0 };
};

//---

class ClLanguageSystemCommand : public ClLanguageCommand {
 public:
  explicit ClLanguageSystemCommand(const std::string &args);

 ~ClLanguageSystemCommand();

  void init() override { }
  void term() override { }

  std::string toName() const override;
};

//---

class ClLanguageProcCommand : public ClLanguageCommand {
 public:
  ClLanguageProcCommand(ClLanguageProc *proc, const std::string &args);

 ~ClLanguageProcCommand();

  void init() override;
  void term() override;

  ClLanguageProc *getProcedure() const override;

  std::string toName() const override;
};

//---

class ClLanguagePosProcCommand : public ClLanguageCommand {
 public:
  ClLanguagePosProcCommand(const std::string &name, const std::string &line);

 ~ClLanguagePosProcCommand();

  void assign(const ClLanguageCommand &command) override;

  void init() override { }
  void term() override;

  std::string toName() const override;

  const std::string &getArgs() const override;

  const std::string &getLine() const;

  ClLanguageProc *getProcedure() const override;

  void setCommand(ClLanguageCommand *cmd);
};

//---

class ClLanguageLabelCommand : public ClLanguageCommand {
 public:
  explicit ClLanguageLabelCommand(ClLanguageLabelData *label_data);

 ~ClLanguageLabelCommand();

  void init() override { }
  void term() override { }

  std::string toName() const override;

  ClLanguageLabelData *getLabelData() const;
};

//---

class ClLanguageExpressionCommand : public ClLanguageCommand {
 public:
  ClLanguageExpressionCommand(const std::string &name, const std::string &line);

 ~ClLanguageExpressionCommand();

  void assign(const ClLanguageCommand &command) override;

  void init() override { }
  void term() override;

  std::string toName() const override;
};

//---

class ClLanguageLanguageCommand : public ClLanguageCommand {
 public:
  explicit ClLanguageLanguageCommand(const std::string &args);

 ~ClLanguageLanguageCommand();

  void init() override { }
  void term() override { }

  std::string toName() const override;
};

//---

class ClLanguageCommandDefCommand : public ClLanguageCommand {
 public:
  ClLanguageCommandDefCommand(ClLanguageCommandDef *command_def,
                              const std::string &args, bool is_end_cmd);

 ~ClLanguageCommandDefCommand();

  void init() override { }
  void term() override { }

  std::string toName() const override;

  ClLanguageCommandDef *getCommandDef() const;
};

#endif
