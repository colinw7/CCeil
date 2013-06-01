#ifndef CCEILP_USERFN_H
#define CCEILP_USERFN_H

#include <CCeilPValue.h>

enum ClUserFnType {
  CL_PARSER_USERFN_TYPE_STD     = 0,
  CL_PARSER_USERFN_TYPE_VARARGS = (1<<9)
};

typedef ClParserValuePtr (*ClParserUserFnProc)
                          (ClParserValuePtr *, uint, void *, int *);

struct ClUserFnData {
  const char         *name;
  ClParserUserFnProc  proc;
  void               *data;
  const char         *types;
  int                *num;
};

class ClParserUserFnMgr {
 private:
  typedef std::map<std::string,ClParserUserFnPtr> UserFnMap;

  UserFnMap user_function_map_;
  uint      ind_;

 public:
  ClParserUserFnMgr();
 ~ClParserUserFnMgr();

  ClParserUserFnPtr createUserFn(const ClUserFnData &data);

  ClParserUserFnPtr createUserFn(const std::string &name, uint type, int *arg_types,
                                 uint num_arg_types, ClParserUserFnProc proc, void *data);

  ClParserUserFnPtr createUserFn(const std::string &name, uint type, const IntVectorT &arg_types,
                                 ClParserUserFnProc proc, void *data);

  uint getNextInd();

  void addUserFn(ClParserUserFnPtr userfn);
  void removeUserFn(const std::string &name);

  bool isUserFn(const std::string &name) const;

  ClParserUserFnPtr getUserFn(const std::string &name) const;

  void stringToTypes(const std::string &str, int *types, IntVectorT &arg_types);
};

class ClParserUserFn {
 private:
  struct ExecData {
    ClParserValuePtr *values_;
    uint              num_values_;

    std::vector<char *>   free_strings_;
    std::vector<char *>   free_chars_;
    std::vector<double *> free_array_reals_;
    std::vector<uint *>   free_array_real_dims_;
    std::vector<float *>  free_array_floats_;
    std::vector<uint *>   free_array_float_dims_;
    std::vector<long *>   free_array_integers_;
    std::vector<uint *>   free_array_integer_dims_;
    std::vector<int *>    free_array_words_;
    std::vector<uint *>   free_array_word_dims_;
    std::vector<char **>  free_array_strings_;
    std::vector<uint *>   free_array_string_dims_;

    void term();
  };

  static int             error_code_;
  static ClParserUserFn *execUserFn_;

  uint                ind_;
  uint                type_;
  std::string         name_;
  std::vector<int>    arg_types_;
  ClParserUserFnProc  proc_;
  void               *data_;
  ExecData            exec_data_;

 public:
  static int getErrorCode() { return error_code_; }

  static bool getExecArgList(CLArgType type, va_list *vargs);
  static bool setExecArgList(CLArgType type, va_list *vargs);

 protected:
  friend class ClParserUserFnMgr;
  friend class CRefPtr<ClParserUserFn>;

  ClParserUserFn(const std::string &name, uint type, const IntVectorT &arg_types,
                 ClParserUserFnProc proc, void *data);

 ~ClParserUserFn();

  ClParserUserFn *dup() const;

 public:
  uint getInd() const { return ind_; }

  bool isVarArgs() const { return (type_ & CL_PARSER_USERFN_TYPE_VARARGS); }

  const std::string &getName() const { return name_; }

  uint getNumArgTypes() const { return arg_types_.size(); }

  int getArgType(uint i) const { return arg_types_[i]; }

  void initArgs(ClParserValuePtr *values, uint num_values);

  ClParserValuePtr exec(ClParserValuePtr *values, uint num_values, int *error_code);
  ClParserValuePtr exec(ClParserValueArray &values, int *error_code);

  ClParserValuePtr exec1(ClParserValuePtr *values, uint num_values, int *error_code);

  void termArgs();

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  static bool isValidName(const std::string &name);

  bool getArgList(CLArgType type, va_list *vargs);
  bool setArgList(CLArgType type, va_list *vargs);

 private:
  bool getArgsFail(int error_code) const;
  bool setArgsFail(int error_code) const;
};

#endif
