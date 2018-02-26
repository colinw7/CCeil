#ifndef CCEILP_LIST_H
#define CCEILP_LIST_H

#include <CCeilPObj.h>

class ClParserList : public ClParserObj {
 public:
  static int getErrorCode() { return error_code_; }

 public:
  static ClParserListPtr createList();
  static ClParserListPtr createList(const ClParserValuePtr *values, uint num_values);
  static ClParserListPtr createList(const ClParserValueArray &values);
  static ClParserListPtr createList(const ClParserList &list);

 public:
  static const ClParserList &castObj(const ClParserObj &obj) {
    assert(obj.getBaseType() == CL_PARSER_VALUE_TYPE_LIST);

    return reinterpret_cast<const ClParserList &>(obj);
  }

 protected:
  friend class CRefPtr<ClParserList>;
  friend class ClParserValue;

  ClParserList();
  ClParserList(const ClParserValuePtr *values, uint num_values);
  ClParserList(const ClParserValueArray &values);
  ClParserList(const ClParserList &list);

 ~ClParserList();

  ClParserList *dup() const;

  ClParserListPtr dupList() const;

  const ClParserList &operator=(const ClParserList &list);

 public:
  bool toBool() const { return values_.size() > 0; }

  void copy(const ClParserObj &obj);

  //--------

  uint getNumValues() const;

  void addValue(ClParserValuePtr value);
  void addListValues(ClParserListPtr list);

  bool deleteValue(ClParserValuePtr value);
  bool deleteListValues(ClParserListPtr list);

  void deleteAllValues();

  ClParserValuePtr getValue(int ind) const;

  int getValuePos(ClParserValuePtr value) const;
  int getRValuePos(ClParserValuePtr value) const;

  void sortValues(ClParserSortDirection direction);

  uint countValues(ClParserValuePtr value) const;

  void roll(int num);

  ClParserValuePtr getMaxValue() const;
  ClParserValuePtr getMinValue() const;

  //------

  int cmp(const ClParserObj &rhs) const;

  //------

  std::string asString() const;

  void print() const;
  void print(std::ostream &os) const;

  void debugPrint() const;

  //------

  ClParserValuePtr subscriptValue(const uint *subscripts, uint num_subscripts) const;
  ClParserValuePtr subscriptValue(const UIntVectorT &subscripts) const;

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
  ClParserValuePtr dim     () const override;
  ClParserValuePtr len     () const override;

  ClParserValuePtr index (const ClParserObj &obj) const;
  ClParserValuePtr rindex(const ClParserObj &obj) const;

  ClParserValuePtr sort(ClParserSortDirection direction) const;

  ClParserValuePtr doAssert() const;

 private:
  void addListValues(const ClParserList &list);
  bool deleteListValues(const ClParserList &list);

 private:
  static int error_code_;

  ClParserValueList values_;
};

#endif
