#ifndef CL_L_ARGS_H
#define CL_L_ARGS_H

typedef StringVectorT ClLanguageArgList;

class ClLanguageArgs {
 public:
  ClLanguageArgs() :
   arg_list_(NULL), string_stack_(NULL), chars_stack_(NULL), real_array_stack_(NULL),
   float_array_stack_(NULL), integer_array_stack_(NULL), word_array_stack_(NULL),
   string_array_stack_(NULL), space_separated_(false) {
  }

 ~ClLanguageArgs() { }

  void setSpaceSeparated(bool flag) { space_separated_ = flag; }

  void startArgs(ClLanguageCommand *command);
  void startArgs();
  void termArgs();

  int checkNumberOfArgs(int min, int max);

  uint getNumArgs() const;

  int getArgList(CLArgType type, ...);
  int getVArgList(CLArgType type, va_list *vars);

  int getStringArgList(const std::string &str, CLArgType type, ...);

  double      getRealArg(int n, int *error_code);
  long        getIntegerArg(int n, int *error_code);
  std::string getStringArg(int n, int *error_code);

  void getCharArrayArg(int n, char **chars, int *num_chars, int *error_code);

  void getRealArrayArg(int n, double **reals, uint *num_reals, int *error_code);
  void getRealArrayArg(int n, float **reals, uint *num_reals, int *error_code);
  void getRealArrayArg(int n, double **reals, uint **dims, uint *num_dims, int *error_code);
  void getRealArrayArg(int n, float **reals, uint **dims, uint *num_dims, int *error_code);

  void getIntegerArrayArg(int n, long **integers, uint **dims, uint *num_dims, int *error_code);
  void getIntegerArrayArg(int n, int **integers, uint **dims, uint *num_dims, int *error_code);
  void getStringArrayArg(int n, char ***strings, uint **dims, uint *num_dims, int *error_code);

  ClParserValuePtr getValueArg(int n, int *error_code);

  std::string getVariableArg(int n, int *error_code);

  std::string getArg(int i, int *error_code);

  int setArgList(CLArgType type, ...);
  int setVArgList(CLArgType type, va_list *vars);

  void setArgs(const std::string &str);
  void setArgs(ClLanguageCommand *command);

  void setRealArg(int n, double real, int *error_code);
  void setIntegerArg(int n, long integer, int *error_code);
  void setStringArg(int n, const std::string &str, int *error_code);

  void setCharArrayArg(int n, char *chars, int num_chars, int *error_code);

  void setRealArrayArg(int n, double *reals, uint *dims, uint num_dims, int *error_code);
  void setRealArrayArg(int n, float *reals, uint *dims, uint num_dims, int *error_code);
  void setIntegerArrayArg(int n, long *integers, uint *dims, uint num_dims, int *error_code);
  void setIntegerArrayArg(int n, int *integers, uint *dims, uint num_dims, int *error_code);
  void setStringArrayArg(int n, char **strings, uint *dims, uint num_dims, int *error_code);

  void setExpressionArg(int n, const std::string &expression, int *error_code);

  void setValueArg(int n, const ClParserValuePtr &value, int *error_code);

  bool getCommandArgValues(ClLanguageCommand *command, ClParserValuePtr **values, int *num_values);

  void getCommandArgList(ClLanguageCommand *command, ClLanguageArgList *arg_list);

  void stringToArgList(const std::string &str, ClLanguageArgList *arg_list);

  static bool readArgList(const std::string &str, int *pos, int end_char, std::string &text);
  static bool skipArgList(const std::string &str, int *pos, int end_char);

  void replaceCharsInArgList(std::string &str, int c1, int c2);

  void stackChars(char **chars);
  void stackRealArray(double *reals, uint *dims);
  void stackRealArray(float *reals, uint *dims);
  void stackIntegerArray(long *integers, uint *dims);
  void stackIntegerArray(int *integers, uint *dims);
  void stackStringArray(char **strs, uint *dims);

 private:
  const std::string &getIArg(int i) const;

 private:
  typedef std::vector<ClLanguageArgList *> ArgListStack;
  typedef std::vector<char *>              StringStack;
  typedef std::vector<char **>             CharsStack;
  typedef std::vector<void **>             RealArrayStack;
  typedef std::vector<void **>             FloatArrayStack;
  typedef std::vector<void **>             IntegerArrayStack;
  typedef std::vector<void **>             WordArrayStack;
  typedef std::vector<void **>             StringArrayStack;

  ArgListStack       arg_list_stack_;
  ClLanguageArgList *arg_list_;
  StringStack       *string_stack_;
  CharsStack        *chars_stack_;
  RealArrayStack    *real_array_stack_;
  FloatArrayStack   *float_array_stack_;
  IntegerArrayStack *integer_array_stack_;
  WordArrayStack    *word_array_stack_;
  StringArrayStack  *string_array_stack_;
  bool               space_separated_;
};

#endif
