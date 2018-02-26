#ifndef CCEILP_INTEGER_H
#define CCEILP_INTEGER_H

#include <CCeilPObj.h>

class ClParserInteger : public ClParserObj {
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
  static const ClParserInteger &castObj(const ClParserObj &obj) {
    assert(obj.getBaseType() == CL_PARSER_VALUE_TYPE_INTEGER);

    return reinterpret_cast<const ClParserInteger &>(obj);
  }

  void copy(const ClParserObj &obj);

  //--------

  long getValue() const { return integer_; }

  const long *getValueP() const { return &integer_; }

  void setValue(long integer) { integer_ = integer; }

  bool toBool() const;

  //------

  std::string asString() const;

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
  static bool isInteger(double real, long &integer);

 private:
  long integer_ { 0 };
};

#endif
