#ifndef CCEILP_VALUE_H
#define CCEILP_VALUE_H

#include <string>

#include <CCeilPTypes.h>

#ifdef rindex
# undef rindex
#endif

/**** Private Structures ****/

class ClParserValueCmp {
 public:
  explicit ClParserValueCmp(ClParserSortDirection direction) :
   direction_(direction) {
  }

  int operator()(ClParserValuePtr obj1, ClParserValuePtr obj2);

 private:
  ClParserSortDirection direction_;
};

//---

union ClParserValueData {
  ClParserRealPtr    *real;
  ClParserIntegerPtr *integer;
  ClParserStringPtr  *str;
  ClParserArrayPtr   *array;
  ClParserListPtr    *list;
  ClParserDictPtr    *dict;
  ClParserStructPtr  *structure;
};

//------

#define ClParserValueMgrInst ClParserValueMgr::getInstance()

class ClParserValueMgr {
 public:
  static ClParserValueMgr *getInstance();

  ClParserValuePtr createValue(ClParserValuePtr value);

  ClParserValuePtr createValue(double real);
  ClParserValuePtr createValue(ClParserRealPtr real);
  ClParserValuePtr createValue(long integer);
  ClParserValuePtr createValue(ClParserIntegerPtr integer);
  ClParserValuePtr createValue(const std::string &str);
  ClParserValuePtr createValue(ClParserStringPtr str);
  ClParserValuePtr createValue(const char *str, uint len);
  ClParserValuePtr createValue(ClParserArrayPtr array);
  ClParserValuePtr createValue(ClParserListPtr list);
  ClParserValuePtr createValue(ClParserDictPtr dict);
  ClParserValuePtr createValue(ClParserStructPtr structure);
  ClParserValuePtr createValue(const ClParserObj &obj);

  ClParserValuePtr createValue(ClParserValueType type);

  ClParserValuePtr createValue(ClParserValueType type, ClParserValuePtr *values, int num_values);
  ClParserValuePtr createValue(ClParserValueType type, const ClParserValueArray &values);

  ClParserValuePtr createValue(ClParserValuePtr *values, int num_values);
  ClParserValuePtr createValue(const ClParserValueArray &values);

  ClParserValuePtr createValue(ClParserTypePtr type);
  ClParserValuePtr createValue(ClParserTypePtr type, ClParserValuePtr *values, int num_values);
  ClParserValuePtr createValue(ClParserTypePtr type, const ClParserValueArray &values);

  ClParserValuePtr createValue(uint *dims, uint num_dims, double *reals);
  ClParserValuePtr createValue(uint *dims, uint num_dims, float *reals);
  ClParserValuePtr createValue(uint *dims, uint num_dims, long *integers);
  ClParserValuePtr createValue(uint *dims, uint num_dims, int *integers);
  ClParserValuePtr createValue(uint *dims, uint num_dims, const char **strs);

  ClParserValuePtr createValue(uint *dims, uint num_dims, const StringVectorT &strs);
  ClParserValuePtr createValue(uint *dims, uint num_dims, const LongVectorT &integers);

  ClParserValuePtr createValue(uint *dims, uint num_dims, double real);
  ClParserValuePtr createValue(const UIntVectorT &dims, double real);
  ClParserValuePtr createValue(uint *dims, uint num_dims, long integer);
  ClParserValuePtr createValue(const UIntVectorT &dims, long integer);
  ClParserValuePtr createValue(uint *dims, uint num_dims, const char *str);
  ClParserValuePtr createValue(const UIntVectorT &dims, const char *str);

  ClParserValuePtr createValue(ClParserTypePtr type, uint *dims, uint num_dims);
  ClParserValuePtr createValue(ClParserTypePtr type, const UIntVectorT &dims);
};

//------

class ClParserValue {
 protected:
  friend class ClParserValueMgr;
  friend class CRefPtr<ClParserValue>;
  friend class CRefPtrDbg<ClParserValue>;

  ClParserValue();
  explicit ClParserValue(double real);
  explicit ClParserValue(ClParserRealPtr real);
  explicit ClParserValue(long integer);
  explicit ClParserValue(ClParserIntegerPtr integer);
  explicit ClParserValue(const std::string &str);
  ClParserValue(const char *text, int len);
  explicit ClParserValue(ClParserStringPtr str);
  explicit ClParserValue(ClParserArrayPtr array);
  explicit ClParserValue(ClParserListPtr list);
  explicit ClParserValue(ClParserDictPtr dict);
  explicit ClParserValue(ClParserStructPtr structure);
  explicit ClParserValue(const ClParserObj &obj);
  explicit ClParserValue(ClParserValueType type);

  ClParserValue(const ClParserValue &value);

  ClParserValue(ClParserValueType type, ClParserValuePtr *values, int num_values);
  ClParserValue(ClParserValueType type, const ClParserValueArray &values);

  ClParserValue(ClParserValuePtr *values, int num_values);
  explicit ClParserValue(const ClParserValueArray &values);

  explicit ClParserValue(ClParserTypePtr type);
  ClParserValue(ClParserTypePtr type, ClParserValuePtr *values, int num_values);
  ClParserValue(ClParserTypePtr type, const ClParserValueArray &values);

  ClParserValue(uint *dims, uint num_dims, double *reals);
  ClParserValue(uint *dims, uint num_dims, float *reals);

  ClParserValue(uint *dims, uint num_dims, long *integers);
  ClParserValue(uint *dims, uint num_dims, int *integers);

  ClParserValue(uint *dims, uint num_dims, const char **strings);
  ClParserValue(uint *dims, uint num_dims, const StringVectorT &strs);
  ClParserValue(uint *dims, uint num_dims, const LongVectorT &integers);

  ClParserValue(uint *dims, uint num_dims, double real);
  ClParserValue(const UIntVectorT &dims, double real);
  ClParserValue(uint *dims, uint num_dims, long integer);
  ClParserValue(const UIntVectorT &dims, long integer);
  ClParserValue(uint *dims, uint num_dims, const char *str);
  ClParserValue(const UIntVectorT &dims, const char *str);

  ClParserValue(ClParserTypePtr type, uint *dims, uint num_dims);
  ClParserValue(ClParserTypePtr type, const UIntVectorT &dims);
  ClParserValue(uint *dims, uint num_dims, ClParserValuePtr *values);

 ~ClParserValue();

  ClParserValue *dup() const;

  const ClParserValue &operator=(const ClParserValue &value);

 public:
  bool canConvertToInteger() const;
  bool canConvertToReal() const;

  bool convertToType(ClParserValuePtr type_value);
  bool convertToArrayStructureType(ClParserTypePtr type);
  bool convertToArrayType(ClParserValueType type);
  bool convertToStructureType(ClParserTypePtr type);
  bool convertToType(ClParserValueType type);

  bool convertToReal(ClParserValuePtr type_value);
  bool convertToRealArrayValue();
  bool convertToIntegerArrayValue();
  bool convertToStringArrayValue();
  bool convertToReal();
  bool convertToInteger();
  bool convertToString();

  bool realValue(double *real) const;
  bool integerValue(long *integer) const;
  bool integerValue(int *integer) const;
  bool stringValue(std::string &str) const;
  bool stringValue(char **chars, int *num_chars) const;
  bool realArrayValue(double **reals, uint **dims, uint *num_dims) const;
  bool realArrayValue(float **reals, uint **dims, uint *num_dims) const;
  bool integerArrayValue(long **integers, uint **dims, uint *num_dims) const;
  bool integerArrayValue(int **integers, uint **dims, uint *num_dims) const;
  bool stringArrayValue(char ***strs, uint **dims, uint *num_dims) const;

  void toSubValues(ClParserValuePtr **values, uint *num_values) const;
  void toSubValues(ClParserValueArray &values) const;

  bool addDictionaryValue(const std::string &key, ClParserValuePtr value);

  bool subscriptValue(ClParserValuePtr subscript, ClParserValuePtr &value) const;

  static bool checkBinaryTypes(ClParserValuePtr &value1, ClParserValuePtr &value2);

  //--------

  bool operator==(ClParserValuePtr value) const {
    return (cmp(value) == 0);
  }

  bool operator<(ClParserValuePtr value) const {
    return (cmp(value) < 0);
  }

  ClParserValuePtr operator+=(ClParserValuePtr value);

  //--------

  ClParserValueType getType() const { return type_; }

  bool isType(ClParserValueType type) const { return type_ == type; }

  bool isNumericArray() const;
  bool isRealArray() const;
  bool isIntegerArray() const;
  bool isStringArray() const;
  bool isListArray() const;
  bool isDictionaryArray() const;
  bool isStructureArray() const;
  bool isNumeric() const;
  bool isReal() const;
  bool isInteger() const;
  bool isString() const;
  bool isList() const;
  bool isDictionary() const;
  bool isStructure() const;

  bool toBool() const;

  void resetValue();

  int cmp(ClParserValuePtr value) const;

  //--------

  std::string asString() const;

  void print() const;
  void print(std::ostream &os) const;

  void debugPrint() const;

  friend std::ostream &operator<<(std::ostream &os, const ClParserValue &th) {
    th.print(os);

    return os;
  }

  //--------

  // Inline Ops

  void increment();
  void decrement();

  //------

  // Unary Ops

  ClParserValuePtr unaryPlus () const;
  ClParserValuePtr unaryMinus() const;
  ClParserValuePtr bitNot    () const;

  //------

  // Binary Ops

  ClParserValuePtr plus(ClParserValuePtr value) const;
  ClParserValuePtr minus(ClParserValuePtr value) const;
  ClParserValuePtr times(ClParserValuePtr value) const;
  ClParserValuePtr divide(ClParserValuePtr value) const;
  ClParserValuePtr modulus(ClParserValuePtr value) const;
  ClParserValuePtr power(ClParserValuePtr value) const;
  ClParserValuePtr approxEqual(ClParserValuePtr value) const;
  ClParserValuePtr bitAnd(ClParserValuePtr value) const;
  ClParserValuePtr bitOr(ClParserValuePtr value) const;
  ClParserValuePtr bitXor(ClParserValuePtr value) const;
  ClParserValuePtr bitLShift(ClParserValuePtr value) const;
  ClParserValuePtr bitRShift(ClParserValuePtr value) const;

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
  ClParserValuePtr dim() const;
  ClParserValuePtr ndim() const;
  ClParserValuePtr len() const;
  ClParserValuePtr min() const;
  ClParserValuePtr max() const;
  ClParserValuePtr sum() const;

  ClParserValuePtr index (ClParserValuePtr value) const;
  ClParserValuePtr rindex(ClParserValuePtr value) const;

  ClParserValuePtr sort(ClParserSortDirection direction) const;

  ClParserValuePtr doAssert() const;

  //--------

  ClParserObj        *getObj       () const;
  ClParserRealPtr     getReal      () const;
  ClParserIntegerPtr  getInteger   () const;
  ClParserStringPtr   getString    () const;
  ClParserArrayPtr    getArray     () const;
  ClParserListPtr     getList      () const;
  ClParserDictPtr     getDictionary() const;
  ClParserStructPtr   getStructure () const;

  void setReal      (ClParserRealPtr real);
  void setInteger   (ClParserIntegerPtr integer);
  void setString    (ClParserStringPtr str);
  void setArray     (ClParserArrayPtr array);
  void setList      (ClParserListPtr list);
  void setDictionary(ClParserDictPtr dictionary);
  void setStructure (ClParserStructPtr structure);

 private:
  ClParserValueType type_;
  ClParserValueData data_;
};

#endif
