#ifndef CCEILP_INTEGER_H
#define CCEILP_INTEGER_H

#include <CCeilPObj.h>

class ClParserInteger : public ClParserObj {
 private:
  long integer_;

 public:
  friend class CRefPtr<ClParserInteger>;
  friend class ClParserValue;

  static ClParserIntegerPtr createInteger(long integer=0) {
    ClParserInteger *pinteger = new ClParserInteger(integer);

    return ClParserIntegerPtr(pinteger);
  }

  static ClParserIntegerPtr createInteger(const ClParserInteger &integer) {
    ClParserInteger *pinteger = new ClParserInteger(integer);

    return ClParserIntegerPtr(pinteger);
  }

  static ClParserIntegerPtr createInteger(ClParserIntegerPtr integer) {
    ClParserInteger *pinteger = new ClParserInteger(integer);

    return ClParserIntegerPtr(pinteger);
  }

  explicit ClParserInteger(long integer = 0) :
   ClParserObj(CL_PARSER_VALUE_TYPE_INTEGER), integer_(integer) {
  }

  ClParserInteger(const ClParserInteger &integer) :
   ClParserObj(CL_PARSER_VALUE_TYPE_INTEGER), integer_(integer.integer_) {
  }

  explicit ClParserInteger(ClParserIntegerPtr integer) :
   ClParserObj(CL_PARSER_VALUE_TYPE_INTEGER), integer_(integer->integer_) {
  }

 ~ClParserInteger() { }

  ClParserInteger &operator=(const ClParserInteger &integer) {
    integer_ = integer.integer_;

    return *this;
  }

  ClParserInteger *dup() const;

 public:
  void copy(const ClParserObj &obj);

  //--------

  long getValue() const { return integer_; }

  const long *getValueP() const { return &integer_; }

  void setValue(long integer) { integer_ = integer; }

  bool toBool() const;

  //------

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  int cmp(const ClParserObj &rhs) const;

  int cmp(const ClParserInteger &rhs) const;

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