#ifndef CL_L_BLOCK_H
#define CL_L_BLOCK_H

/**** Public Defines ****/

#define CL_BLOCK_TYPE_BLOCK_MASK 0x000F

enum ClBlockType {
  CL_BLOCK_TYPE_NORMAL_BLOCK    = 0,
  CL_BLOCK_TYPE_ITERATION_BLOCK = 1,
  CL_BLOCK_TYPE_PROCEDURE_BLOCK = 2,
  CL_BLOCK_TYPE_FILE_BLOCK      = 3,

  CL_BLOCK_TYPE_NO_ARGS_BLOCK   = 1<<30,

  CL_BLOCK_TYPE_NORMAL_NO_ARGS_BLOCK =
   (CL_BLOCK_TYPE_NORMAL_BLOCK | CL_BLOCK_TYPE_NO_ARGS_BLOCK),
  CL_BLOCK_TYPE_ITERATION_NO_ARGS_BLOCK =
   (CL_BLOCK_TYPE_ITERATION_BLOCK | CL_BLOCK_TYPE_NO_ARGS_BLOCK)
};

#define ClLanguageBlockDataMgrInst ClLanguageBlockDataMgr::getInstance()

class ClLanguageBlockData {
 public:
  ClLanguageBlockData();

  ClLanguageBlockData(ClBlockType type, const std::string &name,
                      ClLanguageCommand **commands=nullptr, int num_commands=0);

  ClLanguageBlockData(const ClLanguageBlockData &block_data);

  ClLanguageBlockData &operator=(const ClLanguageBlockData &block_data);

  ClBlockType         getType       () const { return type_        ; }
  const std::string  &getName       () const { return name_        ; }
  ClLanguageCommand **getCommands   () const { return commands_    ; }
  int                 getNumCommands() const { return num_commands_; }

  bool isType(ClBlockType type) const { return type_ == type; }

  ClLanguageCommand *getCommand(int i) { return commands_[i]; }

  void init();

  void setCommandList(const ClLanguageCommandList &command_list);

  void addCommandLabels() const;
  void removeCommandLabels() const;

  int checkControlFlags(int *pos);

  int checkInterrupt();

 private:
  typedef std::vector<ClLanguageBlockData *> BlockDataStack;

  ClBlockType         type_         { CL_BLOCK_TYPE_NORMAL_BLOCK };
  std::string         name_;
  ClLanguageCommand **commands_     { nullptr };
  int                 num_commands_ { 0 };
};

//---

class ClLanguageBlockDataMgr {
 public:
  static ClLanguageBlockDataMgr *getInstance() {
    static ClLanguageBlockDataMgr *instance;

    if (! instance)
      instance = new ClLanguageBlockDataMgr;

    return instance;
  }

  void startBlock(const ClLanguageCommandList &command_list, ClBlockType type);
  void endBlock();

  void setBlockCommands(const ClLanguageCommandList &command_list);

  int processBlockCommands();

 private:
  ClLanguageBlockDataMgr() { }

 private:
  typedef std::vector<ClLanguageBlockData *> BlockDataStack;

  BlockDataStack      block_data_stack_;
  ClLanguageBlockData current_block_data_;
};

#endif
