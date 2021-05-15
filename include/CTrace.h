#ifndef CTRACE_H
#define CTRACE_H

#include <cstdio>

class CRoutineTraceMgr {
 public:
  CRoutineTraceMgr() :
   active_(true), depth_(0), os_(NULL), fp_(NULL) {
    os_ = &std::cerr;
  }

  void enter(const char *routine) {
    output_depth();

    if (active_) {
      if (fp_ == NULL)
        *os_ << "> " << routine << std::endl;
      else
        fprintf(fp_, "> %s\n", routine);
    }

    ++depth_;
  }

  void leave(const char *routine) {
    --depth_;

    if (active_) {
      output_depth();

      if (fp_ == NULL)
        *os_ << "< " << routine << std::endl;
      else
        fprintf(fp_, "> %s\n", routine);
    }
  }

  bool setActive(bool active) {
    bool old_active = active_;

    active_ = active;

    return old_active;
  }

  bool getActive() const {
    return active_;
  }

  std::ostream &setOutput(std::ostream &os) {
    std::ostream *old_os = os_;

    os_ = &os;

    return *old_os;
  }

  FILE *setOutput(FILE *fp) {
    std::swap(fp_, fp);

    return fp;
  }

  void output(const char *msg) const {
    if (active_) {
      output_depth();

      if (fp_ == NULL)
        *os_ << "  " << msg << std::endl;
      else
        fprintf(fp_, "  %s\n", msg);
    }
  }

  void output(const std::string &msg) const {
    if (active_) {
      output_depth();

      if (fp_ == NULL)
        *os_ << "  " << msg << std::endl;
      else
        fprintf(fp_, "  %s\n", msg.c_str());
    }
  }

 private:
  void output_depth() const {
    if (fp_ == NULL) {
      for (int i = 0; i < depth_; ++i)
        *os_ << " ";
    }
    else {
      for (int i = 0; i < depth_; ++i)
        fputc((int) ' ', fp_);
    }
  }

 private:
  int           active_;
  int           depth_;
  std::ostream *os_;
  FILE         *fp_;
};

class CRoutineTrace {
 public:
  CRoutineTrace(CRoutineTraceMgr &mgr, const char *routine) :
   mgr_(mgr), routine_(routine) {
    mgr_.enter(routine_);
  }

 ~CRoutineTrace() {
    mgr_.leave(routine_);
  }

 private:
  CRoutineTraceMgr &mgr_;
  const char       *routine_;
};

#endif
