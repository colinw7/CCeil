#ifndef CCEILP_REAL_H
#define CCEILP_REAL_H

#include <CCeilPObj.h>

class ClParserReal : public ClParserObj {
 private:
  double real_;

 protected:
  friend class CRefPtr<ClParserReal>;
  friend class ClParserValue;

  static ClParserRealPtr createReal(double real=0.0) {
    ClParserReal *preal = new ClParserReal(real);

    return ClParserRealPtr(preal);
  }

  static ClParserRealPtr createReal(const ClParserReal &real) {
    ClParserReal *preal = new ClParserReal(real);

    return ClParserRealPtr(preal);
  }

  static ClParserRealPtr createReal(ClParserRealPtr real) {
    ClParserReal *preal = new ClParserReal(real);

    return ClParserRealPtr(preal);
  }

  ClParserReal(double real = 0.0) :
   ClParserObj(CL_PARSER_VALUE_TYPE_REAL), real_(real) {
  }

  ClParserReal(const ClParserReal &real) :
   ClParserObj(CL_PARSER_VALUE_TYPE_REAL), real_(real.real_) {
  }

  ClParserReal(ClParserRealPtr real) :
   ClParserObj(CL_PARSER_VALUE_TYPE_REAL), real_(real->real_) {
  }

 ~ClParserReal() { }

  const ClParserReal &operator=(const ClParserReal &real) {
    real_ = real.real_;

    return *this;
  }

  ClParserReal *dup() const;

 public:
  void copy(const ClParserObj &obj);

  //--------

  double getValue() const { return real_; }

  const double *getValueP() const { return &real_; }

  void setValue(double real) { real_ = real; }

  bool isNanValue() const;

  long getInteger() const {
    // TODO: range check

    if (! isNanValue())
      return (long) real_;
    else
      return 0;
  }

  bool toBool() const;

  //------

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  int cmp(const ClParserObj &rhs) const;

  //------

  // Inline Ops

  const ClParserObj &increment();
  const ClParserObj &decrement();

  //------

  // Unary Ops

  ClParserValuePtr unaryPlus () const;
  ClParserValuePtr unaryMinus() const;
  ClParserValuePtr bitNot    () const;

  //------

  // Binary Ops

  ClParserValuePtr plus(const ClParserObj &obj) const;
  ClParserValuePtr minus(const ClParserObj &obj) const;
  ClParserValuePtr times(const ClParserObj &obj) const;
  ClParserValuePtr divide(const ClParserObj &obj) const;
  ClParserValuePtr modulus(const ClParserObj &obj) const;
  ClParserValuePtr power(const ClParserObj &obj) const;
  ClParserValuePtr approxEqual(const ClParserObj &obj) const;
  ClParserValuePtr bitAnd(const ClParserObj &obj) const;
  ClParserValuePtr bitOr(const ClParserObj &obj) const;
  ClParserValuePtr bitXor(const ClParserObj &obj) const;
  ClParserValuePtr bitLShift(const ClParserObj &obj) const;
  ClParserValuePtr bitRShift(const ClParserObj &obj) const;

  //------

  // Internal Fns

  ClParserValuePtr abs() const;
  ClParserValuePtr ceil() const;
  ClParserValuePtr floor() const;
  ClParserValuePtr sign() const;
  ClParserValuePtr sqr() const;
  ClParserValuePtr sqrt() const;
  ClParserValuePtr cos() const;
  ClParserValuePtr sin() const;
  ClParserValuePtr tan() const;
  ClParserValuePtr acos() const;
  ClParserValuePtr asin() const;
  ClParserValuePtr atan() const;
  ClParserValuePtr atan(double real) const;
  ClParserValuePtr exp() const;
  ClParserValuePtr log() const;
  ClParserValuePtr log10() const;
  ClParserValuePtr cosh() const;
  ClParserValuePtr sinh() const;
  ClParserValuePtr tanh() const;
  ClParserValuePtr toChar() const;
  ClParserValuePtr toInt() const;
  ClParserValuePtr toReal() const;
  ClParserValuePtr toString() const;
  ClParserValuePtr isNan() const;
  ClParserValuePtr toLower() const;
  ClParserValuePtr toUpper() const;
  ClParserValuePtr min() const;
  ClParserValuePtr max() const;
  ClParserValuePtr sum() const;
  ClParserValuePtr index(const ClParserObj &obj) const;
  ClParserValuePtr rindex(const ClParserObj &obj) const;
  ClParserValuePtr sort(ClParserSortDirection direction) const;
};

#endif
