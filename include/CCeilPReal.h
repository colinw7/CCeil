#ifndef CCEILP_REAL_H
#define CCEILP_REAL_H

#include <CCeilPObj.h>

class ClParserReal : public ClParserObj {
 protected:
  friend class CRefPtr<ClParserReal>;
  friend class ClParserValue;

 protected:
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

  explicit ClParserReal(double real = 0.0) :
   ClParserObj(CL_PARSER_VALUE_TYPE_REAL), real_(real) {
  }

  ClParserReal(const ClParserReal &real) :
   ClParserObj(CL_PARSER_VALUE_TYPE_REAL), real_(real.real_) {
  }

  explicit ClParserReal(ClParserRealPtr real) :
   ClParserObj(CL_PARSER_VALUE_TYPE_REAL), real_(real->real_) {
  }

 ~ClParserReal() { }

  const ClParserReal &operator=(const ClParserReal &real) {
    real_ = real.real_;

    return *this;
  }

  ClParserReal *dup() const;

 public:
  static const ClParserReal &castObj(const ClParserObj &obj) {
    assert(obj.getBaseType() == CL_PARSER_VALUE_TYPE_REAL);

    return reinterpret_cast<const ClParserReal &>(obj);
  }

  void copy(const ClParserObj &obj);

  //--------

  double getValue() const { return real_; }

  const double *getValueP() const { return &real_; }

  void setValue(double real) { real_ = real; }

  bool isNanValue() const;

  long getInteger() const;

  bool toBool() const;

  //------

  std::string asString() const;

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  //------

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

  ClParserValuePtr abs     () const override;
  ClParserValuePtr ceil    () const override;
  ClParserValuePtr floor   () const override;
  ClParserValuePtr sign    () const override;
  ClParserValuePtr sqr     () const override;
  ClParserValuePtr sqrt    () const override;
  ClParserValuePtr cos     () const override;
  ClParserValuePtr sin     () const override;
  ClParserValuePtr tan     () const override;
  ClParserValuePtr acos    () const override;
  ClParserValuePtr asin    () const override;
  ClParserValuePtr atan    () const override;
  ClParserValuePtr atan    (double real) const override;
  ClParserValuePtr exp     () const override;
  ClParserValuePtr log     () const override;
  ClParserValuePtr log10   () const override;
  ClParserValuePtr cosh    () const override;
  ClParserValuePtr sinh    () const override;
  ClParserValuePtr tanh    () const override;
  ClParserValuePtr norm    () const override;
  ClParserValuePtr invnorm () const override;
  ClParserValuePtr toChar  () const override;
  ClParserValuePtr toInt   () const override;
  ClParserValuePtr toReal  () const override;
  ClParserValuePtr toString() const override;
  ClParserValuePtr isNan   () const override;
  ClParserValuePtr toLower () const override;
  ClParserValuePtr toUpper () const override;
  ClParserValuePtr min     () const override;
  ClParserValuePtr max     () const override;
  ClParserValuePtr sum     () const override;

  ClParserValuePtr index (const ClParserObj &obj) const;
  ClParserValuePtr rindex(const ClParserObj &obj) const;

  ClParserValuePtr sort(ClParserSortDirection direction) const;

  ClParserValuePtr doAssert() const;

 private:
  long ival() const;

 private:
  double real_ { 0.0 };
};

#endif
