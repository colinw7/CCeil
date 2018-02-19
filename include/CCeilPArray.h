#ifndef CCEILP_ARRAY_H
#define CCEILP_ARRAY_H

#include <CCeilPObj.h>
#include <CMultiArray.h>

class ClParserArray : public ClParserObj {
 public:
  typedef CMultiArrayT<ClParserValuePtr> ValueArray;

  static int getErrorCode() { return error_code_; }

 public:
  static ClParserArrayPtr createArray();
  static ClParserArrayPtr createArray(ClParserValueType type, const uint *dims, uint num_dims);
  static ClParserArrayPtr createArray(ClParserValueType type, const ValueArray &values);

  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const double *reals);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const float *reals);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const long *integers);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const int *integers);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const char **strs);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const StringVectorT &strs);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const LongVectorT &integers);

  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, double real);
  static ClParserArrayPtr createArray(const UIntVectorT &dims, double real);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, long integer);
  static ClParserArrayPtr createArray(const UIntVectorT &dims, long integer);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims, const char *str);
  static ClParserArrayPtr createArray(const UIntVectorT &dims, const char *str);
  static ClParserArrayPtr createArray(const UIntVectorT &dims, const std::string &str);

  static ClParserArrayPtr createArray(ClParserTypePtr type, const uint *dims, uint num_dims);
  static ClParserArrayPtr createArray(ClParserTypePtr type, const UIntVectorT &dims);
  static ClParserArrayPtr createArray(const uint *dims, uint num_dims,
                                      const ClParserValuePtr *values);
  static ClParserArrayPtr createArray(const ClParserValuePtr *values, uint num_values);
  static ClParserArrayPtr createArray(const ClParserValueArray &values);

  static ClParserArrayPtr createArray(const ClParserValuePtr *values,
                                      const uint *dims, uint num_dims);
  static ClParserArrayPtr createArray(const ClParserValueArray &values, const UIntVectorT &dims);

  static ClParserArrayPtr createArray(const ClParserArray &array);

 public:
  static const ClParserArray &castObj(const ClParserObj &obj) {
    assert(obj.getBaseType() == CL_PARSER_VALUE_TYPE_ARRAY);

    return reinterpret_cast<const ClParserArray &>(obj);
  }

 protected:
  friend class CRefPtr<ClParserArray>;
  friend class ClParserValue;

  ClParserArray();
  explicit ClParserArray(ClParserValueType type);
  ClParserArray(ClParserValueType type, const uint *dims, uint num_dims);
  ClParserArray(ClParserValueType type, const UIntVectorT &dims);
  ClParserArray(ClParserValueType type, const ValueArray &values);

  ClParserArray(const uint *dims, uint num_dims, const double *reals);
  ClParserArray(const UIntVectorT &dims, const double *reals);
  ClParserArray(const uint *dims, uint num_dims, const float *reals);

  ClParserArray(const uint *dims, uint num_dims, const long *integers);
  ClParserArray(const uint *dims, uint num_dims, const int *integers);
  ClParserArray(const uint *dims, uint num_dims, const LongVectorT &integers);
  ClParserArray(const UIntVectorT &dims, const long *integers);

  ClParserArray(const uint *dims, uint num_dims, const char **strings);
  ClParserArray(const uint *dims, uint num_dims, const StringVectorT &strs);
  ClParserArray(const UIntVectorT &dims, const char **strings);

  ClParserArray(const uint *dims, uint num_dims, double real);
  ClParserArray(const UIntVectorT &dims, double real);

  ClParserArray(const uint *dims, uint num_dims, long integer);
  ClParserArray(const UIntVectorT &dims, long integer);

  ClParserArray(const uint *dims, uint num_dims, const std::string &str);
  ClParserArray(const UIntVectorT &dims, const std::string &str);

  ClParserArray(ClParserTypePtr type, const uint *dims, uint num_dims);
  ClParserArray(ClParserTypePtr type, const UIntVectorT &dims);

  ClParserArray(const uint *dims, uint num_dims, const ClParserValuePtr *values);

  ClParserArray(const ClParserValuePtr *values, uint num_values);
  explicit ClParserArray(const ClParserValueArray &values);

  // TODO: remove
  ClParserArray(const ClParserValuePtr *values, const uint *dims, uint num_dims);
  ClParserArray(const ClParserValueArray &values, const UIntVectorT &dims);

  ClParserArray(const ClParserArray &array);

 ~ClParserArray();

  ClParserArray &operator=(const ClParserArray &array);

  ClParserArray *dup() const;

  ClParserArrayPtr dupArray() const;

 public:
  void copy(const ClParserObj &obj);

  //--------

  ClParserValueType getType() const { return type_; }

  bool isType(ClParserValueType type) const {
    return (type == type_);
  }

  uint getNumDims() const;
  uint getNumData() const;

  UIntVectorT getDims() const;

  uint getDim(uint i) const;

  bool toBool() const;

  bool toRealArray   ();
  bool toIntegerArray();
  bool toStringArray ();

  bool toReals(double **reals, uint **dims, uint *num_dims) const;
  bool toReals(double **reals, UIntVectorT &dims) const;
  bool toReals( float **reals, uint **dims, uint *num_dims) const;

  bool toReals(double **reals, uint *num_reals) const;
  bool toReals( float **reals, uint *num_reals) const;

  bool toIntegers( int **integers, uint **dims, uint *num_dims) const;
  bool toIntegers(uint **integers, uint **dims, uint *num_dims) const;
  bool toIntegers(long **integers, uint **dims, uint *num_dims) const;
  bool toIntegers(long **integers, UIntVectorT &dims) const;

  bool toIntegers( int **integers, uint *num_integers) const;
  bool toIntegers(uint **integers, uint *num_integers) const;
  bool toIntegers(long **integers, uint *num_integers) const;

  bool toIntegers(IntVectorT  &integers) const;
  bool toIntegers(UIntVectorT &integers) const;
  bool toIntegers(LongVectorT &integers) const;

  bool toStrings(char ***strings, uint **dims, uint *num_dims) const;
  bool toStrings(char ***strings, UIntVectorT &dims) const;

  bool toStrings(char ***strings, uint *num_strings) const;

  bool toValues(ClParserValuePtr **values, uint **dims, uint *num_dims) const;
  bool toValues(ClParserValuePtr **values, UIntVectorT &dims) const;
  bool toValues(ClParserValueArray &values, uint **dims, uint *num_dims) const;
  bool toValues(ClParserValueArray &values, UIntVectorT &dims) const;

  ClParserValuePtr getValue(int i) const;

  ClParserValuePtr getSubscriptValue(const int *subscripts, uint num_subscripts) const;
  ClParserValuePtr getSubscriptValue(const IntVectorT &subscripts) const;

  bool setSubscriptValue(const int *subscripts, uint num_subscripts, ClParserValuePtr value);
  bool setSubscriptValue(const IntVectorT &subscripts, ClParserValuePtr value);

  ClParserArrayPtr getSubArray(int ind) const;
  ClParserArrayPtr getSubArray(int start, int end) const;

  ClParserValuePtr getSubValue(int ind) const;

  bool setValue(int i, ClParserValuePtr value);

  int cmp(const ClParserObj &rhs) const;

  ClParserArrayPtr concat(const ClParserArray &array) const;

  bool sameDimension(const ClParserArray &array) const;

  bool expandTo(const ClParserArray &array);
  bool dimensionTo(const ClParserArray &array);

  bool setIndexArray();

  bool dataCopy(int ind, const ClParserArray &array);

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

  ClParserValuePtr plus       (const ClParserObj &obj) const;
  ClParserValuePtr minus      (const ClParserObj &obj) const;
  ClParserValuePtr times      (const ClParserObj &obj) const;
  ClParserValuePtr divide     (const ClParserObj &obj) const;
  ClParserValuePtr modulus    (const ClParserObj &obj) const;
  ClParserValuePtr power      (const ClParserObj &obj) const;
  ClParserValuePtr approxEqual(const ClParserObj &obj) const;
  ClParserValuePtr bitAnd     (const ClParserObj &obj) const;
  ClParserValuePtr bitOr      (const ClParserObj &obj) const;
  ClParserValuePtr bitXor     (const ClParserObj &obj) const;
  ClParserValuePtr bitLShift  (const ClParserObj &obj) const;
  ClParserValuePtr bitRShift  (const ClParserObj &obj) const;

  //------

  // Internal Fns

  ClParserValuePtr abs     () const;
  ClParserValuePtr ceil    () const;
  ClParserValuePtr floor   () const;
  ClParserValuePtr sign    () const;
  ClParserValuePtr sqr     () const;
  ClParserValuePtr sqrt    () const;
  ClParserValuePtr cos     () const;
  ClParserValuePtr sin     () const;
  ClParserValuePtr tan     () const;
  ClParserValuePtr acos    () const;
  ClParserValuePtr asin    () const;
  ClParserValuePtr atan    () const;
  ClParserValuePtr atan    (double real) const;
  ClParserValuePtr exp     () const;
  ClParserValuePtr log     () const;
  ClParserValuePtr log10   () const;
  ClParserValuePtr cosh    () const;
  ClParserValuePtr sinh    () const;
  ClParserValuePtr tanh    () const;
  ClParserValuePtr toChar  () const;
  ClParserValuePtr toInt   () const;
  ClParserValuePtr toReal  () const;
  ClParserValuePtr toString() const;
  ClParserValuePtr isNan   () const;
  ClParserValuePtr toLower () const;
  ClParserValuePtr toUpper () const;
  ClParserValuePtr min     () const;
  ClParserValuePtr max     () const;
  ClParserValuePtr sum     () const;
  ClParserValuePtr dim     () const;
  ClParserValuePtr ndim    () const;
  ClParserValuePtr len     () const;

  ClParserValuePtr index (const ClParserObj &obj) const;
  ClParserValuePtr rindex(const ClParserObj &obj) const;

  ClParserValuePtr sort(ClParserSortDirection direction) const;

  ClParserValuePtr doAssert() const;

  //------

  ClParserValuePtr binaryOp(ClParserOperatorPtr op, ClParserArray *array);

  static ClParserValuePtr internFn(ClParserInternFnPtr internfn, const ClParserValuePtr *values,
                                   uint num_values);

  ClParserArrayPtr arrayTimes(const ClParserArray &array);

  //------

  std::string asString() const;

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  //------

  bool getSubscriptRange(int *i1, int *i2) const;

 private:
  bool indexToData(int *i) const;
  bool indexToData(int *i, uint dim) const;

 private:
  static int error_code_;

  ClParserValueType type_;
  ValueArray        values_;
};

#endif
