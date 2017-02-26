#ifndef CL_L_DEBUG_H
#define CL_L_DEBUG_H

#ifdef CL_LANGUAGE_DEBUG
# define CL_LANGUAGE_TRACE(a) ClLanguageDebug::Trace trace_(a);
#else
# define CL_LANGUAGE_TRACE(a)
#endif

#define ClLanguageDebugInst ClLanguageDebug::getInstance()

class ClLanguageDebug {
 public:
  class Trace {
   public:
#ifdef CL_LANGUAGE_DEBUG
    explicit Trace(const char *id) : trace_(*ClLanguageDebugInst->getTrace(), id) { }
#else
    explicit Trace(const char *) { }
#endif

#ifdef CL_LANGUAGE_DEBUG
   private:
    CRoutineTrace trace_;
#endif
  };

  static ClLanguageDebug *getInstance() {
    static ClLanguageDebug *instance;

    if (! instance)
      instance = new ClLanguageDebug;

    return instance;
  }

  void setDebug(bool flag);

  FILE *setDebugFile(FILE *fp);

  bool isDebug() const;

  void print(const char *fmt, ...);

#ifdef CL_LANGUAGE_DEBUG
  CRoutineTraceMgr *getTrace() { return &trace_mgr_; }
#endif

#ifdef CL_LANGUAGE_DEBUG
 private:
  CRoutineTraceMgr trace_mgr_;
#endif
};

#endif
