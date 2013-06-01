#ifndef CL_L_ERROR_H
#define CL_L_ERROR_H

class CCeilLErrorMgr {
 public:
  CCeilLErrorMgr();

  void error(const char *format, ...);
  void error(const char *format, va_list *vargs);
  void syntaxError(const char *format, ...);
  void syntaxError(const char *format, va_list *vargs);
  void expressionError(int error_code, const char *format, ...);
  void expressionError(int error_code, const char *format, va_list *vargs);

  FILE *setErrorFile(FILE *fp);

  ClLanguageErrorProc setErrorProc(ClLanguageErrorProc proc, void *data);

  void setErrorFlag(bool flag) { error_flag_ = flag; }
  bool getErrorFlag() const { return error_flag_; }

 private:
  FILE                *error_fp_;
  ClLanguageErrorProc  error_proc_;
  void                *error_data_;
  std::string          error_string_;
  bool                 error_flag_;
};

#endif
