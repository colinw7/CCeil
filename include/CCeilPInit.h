#ifndef CL_P_INIT_H
#define CL_P_INIT_H

#include <accessor.h>
#include <cmath>
#include <CTrace.h>
#include <CAngleType.h>
#include <CCeilPValue.h>

enum class ClParserNameType {
  EXTERNAL = 0,
  FUNCTION = 1,
  STRUCT   = 2,
  VARIABLE = 3,
  USERFN   = 4
};

#define ClParserInst ClParser::getInstance()

#ifdef CL_PARSER_DEBUG
# define CL_PARSER_TRACE(a) \
    CRoutineTrace trace_(ClParserInst->getTraceMgr(), a);
#else
# define CL_PARSER_TRACE(a)
#endif

class ClParser {
 public:
  typedef void (*OutputProc)(const char *str, void *data);

 private:
  typedef int (*StrCmpProc )(const char *, const char *);
  typedef int (*StrNCmpProc)(const char *, const char *, size_t);

 public:
  static ClParser *getInstance();

 private:
  ClParser();

 public:
 ~ClParser();

  void init();
  void term();

  CAngleType setAngleType(CAngleType type) {
    std::swap(angle_type_, type);

    if     (angle_type_ == CANGLE_TYPE_RADIANS) {
      angle_to_radians_ = 1.0;
      angle_to_degrees_ = 180.0/M_PI;
      radians_to_angle_ = 1.0;
      degrees_to_angle_ = M_PI/180.0;
    }
    else if (angle_type_ == CANGLE_TYPE_DEGREES) {
      angle_to_radians_ = M_PI/180.0;
      angle_to_degrees_ = 1.0;
      radians_to_angle_ = 180.0/M_PI;
      degrees_to_angle_ = 1.0;
    }

    return type;
  }

  CAngleType getAngleType() const {
    return angle_type_;
  }

  double toRadians(double angle) const {
    return angle_to_radians_*angle;
  }

  double toDegrees(double angle) const {
    return angle_to_degrees_*angle;
  }

  double fromRadians(double angle) const {
    return radians_to_angle_*angle;
  }

  double fromDegrees(double angle) const {
    return degrees_to_angle_*angle;
  }

  FILE *setOutputFile(FILE *fp) {
    if (fp != NULL)
      std::swap(output_fp_, fp);
    else
      fp = output_fp_;

    return fp;
  }

  FILE *getOutputFile() const {
    return output_fp_;
  }

  OutputProc setOutputProc(OutputProc proc, void *data) {
    if (proc != NULL) {
      std::swap(output_proc_, proc);

      output_data_ = data;
    }
    else
      proc = output_proc_;

    return proc;
  }

  OutputProc getOutputProc() const {
    return output_proc_;
  }

  void *getOutputData() const {
    return output_data_;
  }

  void setRealFormat(int width, int decimal_places) {
    if (width <= decimal_places + 1)
      width = 0;

    char temp_string[64];

    if (width > 0 && decimal_places >= 0)
      sprintf(temp_string, "%%%d.%dlf", width, decimal_places);
    else if (width > 0)
      sprintf(temp_string, "%%%dlf", width);
    else if (decimal_places > 0)
      sprintf(temp_string, "%%.%dlf", decimal_places);
    else
      sprintf(temp_string, "%%lf");

    real_format_ = temp_string;
  }

  void setRealFormat(const std::string &format) {
    real_format_ = format;
  }

  std::string getRealFormat() const {
    return real_format_;
  }

  void setIntegerFormat(int width) {
    char temp_string[64];

    if (width > 0)
      sprintf(temp_string, "%%%dld", width);
    else
      sprintf(temp_string, "%%ld");

    integer_format_ = temp_string;
  }

  void setIntegerFormat(const std::string &format) {
    integer_format_ = format;
  }

  std::string getIntegerFormat() const {
    return integer_format_;
  }

  void setStringFormat(int width) {
    char temp_string[64];

    if (width > 0)
      sprintf(temp_string, "%%%ds", width);
    else
      sprintf(temp_string, "%%s");

    string_format_ = temp_string;
  }

  ACCESSOR(StringFormat, std::string, string_format)

  ACCESSOR(MathFail    , bool  , math_fail)
  ACCESSOR(Tolerance   , double, tolerance)
  ACCESSOR(ParseAsReals, bool  , parse_as_reals)
  ACCESSOR(DollarPrefix, bool  , dollar_prefix )

  void setCaseSensitive(bool flag);

#ifdef CL_PARSER_DEBUG
  const CRoutineTraceMgr &getTraceMgr() const {
    return trace_mgr_;
  }
#endif

#ifdef CL_PARSER_DEBUG
  void setDebug(bool flag) { trace_mgr_.setActive(flag); }
#else
  void setDebug(bool flag) { debug_ = flag; }
#endif

#ifdef CL_PARSER_DEBUG
  FILE *setDebugFile(FILE *fp) { return trace_mgr_.setOutput(fp); }
#else
  FILE *setDebugFile(FILE *) { return nullptr; }
#endif

  bool isDebug() const {
#ifdef CL_PARSER_DEBUG
    return trace_mgr_.getActive();
#else
    return debug_;
#endif
  }

  bool isValidAssignString(const std::string &str);

  void assignValue(const std::string &str, ClParserValuePtr value, int *error_code);

  void assignSubscriptValue(const std::string &str, int i, ClParserValuePtr value, int *error_code);

  bool isValidNewName(ClParserNameType type, const std::string &name) const;

  //-----------

  // Types

  void newTypeList();
  void oldTypeList();

  void printAllTypes() const;

  bool isType(const std::string &name) const;

  ClParserTypePtr createType();
  ClParserTypePtr createType(const std::string &name);
  ClParserTypePtr createType(const std::string &name, const std::string &arg_string);

  ClParserTypePtr getType(const std::string &name) const;

  ClParserTypePtr getIntegerType() const;
  ClParserTypePtr getRealType   () const;
  ClParserTypePtr getStringType () const;

  void deleteType(const std::string &name);
  void deleteAllTypes();

  //-----------

  // variables

  ClParserVarPtr createStdVar(const std::string &name, ClParserValuePtr value);

  ClParserVarPtr createVar(const std::string &name, ClParserValuePtr value);

  ClParserVarRefPtr createVarRef(ClParserVarPtr variable,
                                 const ClParserValueArray &subscripts = ClParserValueArray());

  ClParserStructVarRefPtr
  createStructVarRef(ClParserVarPtr variable, const std::string &name,
                     const ClParserValueArray &subscripts = ClParserValueArray());

  void newVariableList();
  void oldVariableList();

  void deleteAllVariables();

  void printAllVariables() const;

  bool isVariable(const std::string &name) const;

  ClParserVarPtr getVariable(const std::string &name, bool create=false) const;

  ClParserValuePtr getVariableValue(const std::string &name) const;

  void setVariableValue(const std::string &name, ClParserValuePtr value);

  void removeVariable(const std::string &name);

  void callVariableProc(ClParserVarPtr variable);

  //-----------

  // Function

  bool createFunc(const std::string &name, int *error_code);
  bool createFunc(const std::string &function_string, const std::string &expr_string,
                  int *error_code);

  void deleteAllFunctions();

  void printAllFunctions() const;

  bool isFunction(const std::string &name) const;

  ClParserFuncPtr getFunction(const std::string &name) const;

  void removeFunction(const std::string &name);

  //-----------

  // InternFn

  bool isInternFn(const std::string &name) const;

  ClParserInternFnPtr getInternFn(const std::string &name) const;

  //-----------

  // UserFn

  ClParserUserFnPtr createUserFn(const ClUserFnData &data);

  ClParserUserFnPtr createUserFn(const std::string &name, uint type, int *arg_types,
                                 uint num_arg_types, ClParserUserFnProc proc, void *data);

  ClParserUserFnPtr createUserFn(const std::string &name, uint type, const IntVectorT &arg_types,
                                 ClParserUserFnProc proc, void *data);

  void removeUserFn(const std::string &name);

  bool isUserFn(const std::string &name) const;

  ClParserUserFnPtr getExecUserFn() const;

  void setExecUserFn(ClParserUserFnPtr userfn);

  ClParserUserFnPtr getUserFn(const std::string &name) const;

  uint getNextUserFnInd();

  //-----------

  ClParserScopePtr getScope(const std::string &name) {
    return scopeMgr_.getScope(name);
  }

  ClParserScopePtr getScope(const StringVectorT &names) {
    return scopeMgr_.getScope(names);
  }

  //-----------

  bool getUserFnArgList(CLArgType type, ...);
  bool setUserFnArgList(CLArgType type, ...);

  void restartStack();

  void output(const char *format, ...);
  void output(const char *format, va_list *args);

  void signalError(int *error_code, ClErr err) const;

  void error(const char *format, ...);
  void error(const char *format, va_list *args);

  const char *getErrorMessage(int error_code);

  std::string toString(long integer) const {
    char *text = fmtBuffer();

    sprintf(text, integer_format_.c_str(), integer);

    return text;
  }

  std::string toString(double real) const {
    char *text = fmtBuffer();

    sprintf(text, real_format_.c_str(), real);

    return text;
  }

  std::string toString(const char *str) {
    char *text = fmtBuffer();

    sprintf(text, string_format_.c_str(), str);

    return text;
  }

 private:
  static char *fmtBuffer() {
    static char text[512];

    return text;
  }

 private:
  CAngleType  angle_type_       { CANGLE_TYPE_RADIANS };
  double      angle_to_radians_ { 1.0 };
  double      angle_to_degrees_ { 1.0 };
  double      radians_to_angle_ { 1.0 };
  double      degrees_to_angle_ { 1.0 };
  std::string real_format_      { "%lf" };
  std::string integer_format_   { "%ld" };
  std::string string_format_    { "%s" };
  bool        math_fail_        { false };
  double      tolerance_        { 1E-6 };
  bool        parse_as_reals_   { false };
  bool        dollar_prefix_    { false };

#ifdef CL_PARSER_DEBUG
  CRoutineTraceMgr trace_mgr_;
#else
  bool             debug_       { false};
#endif

  int         init_depth_;
  FILE       *output_fp_;
  OutputProc  output_proc_;
  void       *output_data_         { nullptr };

  StrCmpProc  strcmp_;
  StrNCmpProc strncmp_;

  ClParserVarMgr*     varMgr_      { nullptr };
  ClParserFuncMgr     funcMgr_;
  ClParserInternFnMgr internFnMgr_;
  ClParserUserFnMgr   userFnMgr_;
  ClParserScopeMgr    scopeMgr_;
  ClParserTypeMgr     typeMgr_;
};

#endif
