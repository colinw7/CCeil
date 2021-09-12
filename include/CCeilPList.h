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

  ClParserList *dup() const override;

  ClParserListPtr dupList() const;

  ClParserList &operator=(const ClParserList &list);

 public:
  bool toBool() const override { return values_.size() > 0; }

  void copy(const ClParserObj &obj) override;

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

  int cmp(const ClParserObj &rhs) const override;

  //------

  std::string asString() const override;

  void print() const override;
  void print(std::ostream &os) const override;

  void debugPrint() const override;

  //------

  ClParserValuePtr subscriptValue(const uint *subscripts, uint num_subscripts) const;
  ClParserValuePtr subscriptValue(const UIntVectorT &subscripts) const;

  //------

  // Inline Ops

  const ClParserObj &increment() override;
  const ClParserObj &decrement() override;

  //------

  // Unary Ops

  ClParserValuePtr unaryPlus () const override;
  ClParserValuePtr unaryMinus() const override;
  ClParserValuePtr bitNot    () const override;

  //------

  // Binary Ops

  ClParserValuePtr plus(const ClParserObj &obj) const override;
  ClParserValuePtr minus(const ClParserObj &obj) const override;
  ClParserValuePtr times(const ClParserObj &obj) const override;
  ClParserValuePtr divide(const ClParserObj &obj) const override;
  ClParserValuePtr modulus(const ClParserObj &obj) const override;
  ClParserValuePtr power(const ClParserObj &obj) const override;
  ClParserValuePtr approxEqual(const ClParserObj &obj) const override;
  ClParserValuePtr bitAnd(const ClParserObj &obj) const override;
  ClParserValuePtr bitOr(const ClParserObj &obj) const override;
  ClParserValuePtr bitXor(const ClParserObj &obj) const override;
  ClParserValuePtr bitLShift(const ClParserObj &obj) const override;
  ClParserValuePtr bitRShift(const ClParserObj &obj) const override;

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

  ClParserValuePtr index (const ClParserObj &obj) const override;
  ClParserValuePtr rindex(const ClParserObj &obj) const override;

  ClParserValuePtr sort(ClParserSortDirection direction) const override;

  ClParserValuePtr doAssert() const override;

 private:
  void addListValues(const ClParserList &list);
  bool deleteListValues(const ClParserList &list);

 private:
  static int error_code_;

  ClParserValueList values_;
};

#endif
