#ifndef CCEILP_DICT_H
#define CCEILP_DICT_H

#include <CCeilPObj.h>
#include <CCeilPValue.h>

struct ClParserKeyValueBase {
  virtual ~ClParserKeyValueBase() { }

  virtual std::string toString() const = 0;

  virtual ClParserValuePtr toValue() const = 0;
};

struct ClParserIntegerKeyValue : public ClParserKeyValueBase {
  long integer;

  explicit ClParserIntegerKeyValue(long integer1) : integer(integer1) { }

  ClParserIntegerKeyValue(const ClParserIntegerKeyValue &value) :
   ClParserKeyValueBase(value), integer(value.integer) {
  }

 ~ClParserIntegerKeyValue() { }

  ClParserIntegerKeyValue &operator=(const ClParserIntegerKeyValue &value) {
    integer = value.integer;

    return *this;
  }

  bool operator==(const ClParserIntegerKeyValue &value) const {
    return (integer == value.integer);
  }

  std::string toString() const {
    return CStrUtil::toString(integer);
  }

  ClParserValuePtr toValue() const {
    return ClParserValueMgrInst->createValue(integer);
  }
};

struct ClParserStringKeyValue : public ClParserKeyValueBase {
  std::string str;

  explicit ClParserStringKeyValue(const std::string &str1) : str(str1) { }

  ClParserStringKeyValue(const ClParserStringKeyValue &value) :
   ClParserKeyValueBase(value), str(value.str) {
  }

  ClParserStringKeyValue &operator=(const ClParserStringKeyValue &value) {
    str = value.str;

    return *this;
  }

 ~ClParserStringKeyValue() { }

  bool operator==(const ClParserStringKeyValue &value) const {
    return (str == value.str);
  }

  std::string toString() const {
    return str;
  }

  ClParserValuePtr toValue() const {
    return ClParserValueMgrInst->createValue(str);
  }
};

//---

class ClParserKey {
 public:
  explicit ClParserKey(long integer=0);
  explicit ClParserKey(const std::string &str);

  ClParserKey(const ClParserKey &key);
 ~ClParserKey();

  ClParserKey &operator=(const ClParserKey &key);

  bool operator==(const ClParserKey &key) const;

  std::string asString() const;

  void print() const;
  void print(std::ostream &os) const;

  void debugPrint() const;

  ClParserValuePtr getAsValue() const;

 private:
  ClParserValueType     type_;
  ClParserKeyValueBase *value_;
};

//---

struct ClParserKeyValue {
  ClParserKey      key;
  ClParserValuePtr value;

  ClParserKeyValue(const ClParserKey &key1, ClParserValuePtr value1) :
   key(key1), value(value1) {
  }
};

#define ClParserDictMgrInst ClParserDictMgr::getInstance()

class ClParserDictMgr {
  typedef std::map<long,ClParserKey>        IntegerKeyMap;
  typedef std::map<std::string,ClParserKey> StringKeyMap;

 private:
  IntegerKeyMap integer_key_map_;
  StringKeyMap  string_key_map_;

 public:
  static ClParserDictMgr *getInstance();

 private:
  ClParserDictMgr();

 public:
 ~ClParserDictMgr();

  const ClParserKey &objToKey(const ClParserObj &obj);
  const ClParserKey &valueToKey(ClParserValuePtr value);
  const ClParserKey &integerToKey(long integer);
  const ClParserKey &stringToKey(const std::string &str);
};

class ClParserDict : public ClParserObj {
 public:
  static int getErrorCode() { return error_code_; }

  static ClParserDictPtr createDict();
  static ClParserDictPtr createDict(const ClParserValuePtr *values, uint num_values);
  static ClParserDictPtr createDict(const ClParserValueArray &values);
  static ClParserDictPtr createDict(const ClParserDict &dict);

 public:
  static const ClParserDict &castObj(const ClParserObj &obj) {
    assert(obj.getBaseType() == CL_PARSER_VALUE_TYPE_DICTIONARY);

    return reinterpret_cast<const ClParserDict &>(obj);
  }

 protected:
  friend class CRefPtr<ClParserDict>;
  friend class ClParserValue;

  ClParserDict();
  ClParserDict(const ClParserValuePtr *values, uint num_values);
  explicit ClParserDict(const ClParserValueArray &values);
  ClParserDict(const ClParserDict &dict);

 ~ClParserDict();

  const ClParserDict &operator=(const ClParserDict &dict);

  ClParserDict *dup() const override;

  ClParserDictPtr dupDict() const;

 public:
  void copy(const ClParserObj &obj) override;

  //--------

  bool toBool() const override;

  void clearValues();

  void addValues(ClParserDictPtr dict);

  void addValue(const ClParserKey &key, ClParserValuePtr value);
  void addValue(const ClParserKeyValue &keyval);

  void deleteValues(const ClParserKey &key);

  uint numValues() const;

  ClParserValuePtr getKeyValue(const ClParserKey &key) const;
  ClParserValuePtr getIndValue(uint ind) const;

  bool getIndKeyValue(uint ind, ClParserKey &key, ClParserValuePtr &value) const;

  int cmp(const ClParserObj &rhs) const override;

  uint countKeys(const ClParserKey &key) const;

  ClParserValuePtr getMaxValue() const;
  ClParserValuePtr getMinValue() const;

  const ClParserKey &objToKey(const ClParserObj &obj) const;
  const ClParserKey &valueToKey(ClParserValuePtr value) const;
  const ClParserKey &integerToKey(long integer) const;
  const ClParserKey &stringToKey(const std::string &str) const;

  //------

  std::string asString() const override;

  void print() const override;
  void print(std::ostream &os) const override;

  void debugPrint() const override;

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

  //------

  ClParserValuePtr subscriptValue(const ClParserValuePtr *subscripts, uint num_subscripts) const;

  ClParserValuePtr getValue(ClParserValuePtr ind) const;
  ClParserValuePtr getValue(const ClParserKey &key) const;

  ClParserValuePtr getKeyValue(int ind) const;

 private:
  void addValues(const ClParserDict &list);

 private:
  typedef std::list<ClParserKeyValue> KeyValueList;

  static int error_code_;

  KeyValueList key_values_;
};

#endif
