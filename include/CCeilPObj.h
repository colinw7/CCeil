#ifndef CCEIL_POBJ_H
#define CCEIL_POBJ_H

#ifdef rindex
# undef rindex
#endif

#include <CCeilPTypes.h>

class ClParserObjCmp {
 private:
  ClParserSortDirection direction_;

 public:
  explicit ClParserObjCmp(ClParserSortDirection direction) :
   direction_(direction) {
  }

  int operator()(const ClParserObj &obj1, const ClParserObj &obj2);
  int operator()(const ClParserObj *obj1, const ClParserObj *obj2);
  int operator()(const ClParserIntegerPtr &obj1, const ClParserIntegerPtr &obj2);
};

// TODO: use template to get correct signatures

class ClParserObj {
 protected:
  ClParserValueType base_type_;

 public:
  explicit ClParserObj(ClParserValueType base_type) :
   base_type_(base_type) {
  }

  virtual ~ClParserObj() { }

  virtual ClParserObj *dup() const = 0;

  virtual void copy(const ClParserObj &obj) = 0;

  //------

  ClParserValueType getBaseType() const { return base_type_; }

  virtual bool toBool() const = 0;

  //------

  // Print

  virtual void print() const = 0;
  virtual void print(std::ostream &os) const = 0;

  virtual void debugPrint() const = 0;

  //------

  // Inline Ops

  virtual const ClParserObj &increment() = 0;
  virtual const ClParserObj &decrement() = 0;

  //------

  // Unary Ops

  virtual ClParserValuePtr unaryPlus () const = 0;
  virtual ClParserValuePtr unaryMinus() const = 0;
  virtual ClParserValuePtr bitNot    () const = 0;

  //------

  // Binary Ops

  virtual ClParserValuePtr plus(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr minus(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr times(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr divide(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr modulus(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr power(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr approxEqual(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr bitAnd(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr bitOr(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr bitXor(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr bitLShift(const ClParserObj &obj) const = 0;
  virtual ClParserValuePtr bitRShift(const ClParserObj &obj) const = 0;

  //------

  // Internal Fns

  virtual ClParserValuePtr abs() const = 0;
  virtual ClParserValuePtr ceil() const = 0;
  virtual ClParserValuePtr floor() const = 0;
  virtual ClParserValuePtr sign() const = 0;
  virtual ClParserValuePtr sqr() const = 0;
  virtual ClParserValuePtr sqrt() const = 0;
  virtual ClParserValuePtr cos() const = 0;
  virtual ClParserValuePtr sin() const = 0;
  virtual ClParserValuePtr tan() const = 0;
  virtual ClParserValuePtr acos() const = 0;
  virtual ClParserValuePtr asin() const = 0;
  virtual ClParserValuePtr atan() const = 0;
  virtual ClParserValuePtr atan(double real) const = 0;
  virtual ClParserValuePtr exp() const = 0;
  virtual ClParserValuePtr log() const = 0;
  virtual ClParserValuePtr log10() const = 0;
  virtual ClParserValuePtr cosh() const = 0;
  virtual ClParserValuePtr sinh() const = 0;
  virtual ClParserValuePtr tanh() const = 0;
  virtual ClParserValuePtr toChar() const = 0;
  virtual ClParserValuePtr toInt() const = 0;
  virtual ClParserValuePtr toReal() const = 0;
  virtual ClParserValuePtr toString() const = 0;
  virtual ClParserValuePtr isNan() const = 0;
  virtual ClParserValuePtr toLower() const = 0;
  virtual ClParserValuePtr toUpper() const = 0;
  virtual ClParserValuePtr dim() const;
  virtual ClParserValuePtr ndim() const;
  virtual ClParserValuePtr len() const;
  virtual ClParserValuePtr min() const = 0;
  virtual ClParserValuePtr max() const = 0;
  virtual ClParserValuePtr sum() const = 0;
  virtual ClParserValuePtr index(const ClParserObj &value) const = 0;
  virtual ClParserValuePtr rindex(const ClParserObj &value) const = 0;
  virtual ClParserValuePtr sort(ClParserSortDirection direction) const = 0;

  //------

  // Compare

  virtual int cmp(const ClParserObj &obj) const = 0;

  bool operator<(const ClParserObj &rhs) {
    return cmp(rhs) < 0;
  }

  bool operator<=(const ClParserObj &rhs) {
    return cmp(rhs) <= 0;
  }

  bool operator>(const ClParserObj &rhs) {
    return cmp(rhs) > 0;
  }

  bool operator>=(const ClParserObj &rhs) {
    return cmp(rhs) >= 0;
  }

  bool operator==(const ClParserObj &rhs) {
    return cmp(rhs) == 0;
  }

  bool operator!=(const ClParserObj &rhs) {
    return cmp(rhs) != 0;
  }
};

#endif
