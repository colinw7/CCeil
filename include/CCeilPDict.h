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
 private:
  typedef std::list<ClParserKeyValue> KeyValueList;

  static int error_code_;

  KeyValueList key_values_;

 public:
  static int getErrorCode() { return error_code_; }

  static ClParserDictPtr createDict();
  static ClParserDictPtr createDict(const ClParserValuePtr *values, uint num_values);
  static ClParserDictPtr createDict(const ClParserValueArray &values);
  static ClParserDictPtr createDict(const ClParserDict &dict);

 protected:
  friend class CRefPtr<ClParserDict>;
  friend class ClParserValue;

  ClParserDict();
  ClParserDict(const ClParserValuePtr *values, uint num_values);
  explicit ClParserDict(const ClParserValueArray &values);
  ClParserDict(const ClParserDict &dict);

 ~ClParserDict();

  const ClParserDict &operator=(const ClParserDict &dict);

  ClParserDict *dup() const;

  ClParserDictPtr dupDict() const;

 public:
  void copy(const ClParserObj &obj);

  //--------

  bool toBool() const;

  void clearValues();

  void addValues(ClParserDictPtr dict);

  void addValue(const ClParserKey &key, ClParserValuePtr value);
  void addValue(const ClParserKeyValue &keyval);

  void deleteValues(const ClParserKey &key);

  uint numValues() const;

  ClParserValuePtr getKeyValue(const ClParserKey &key) const;
  ClParserValuePtr getIndValue(uint ind) const;

  bool getIndKeyValue(uint ind, ClParserKey &key, ClParserValuePtr &value) const;

  int cmp(const ClParserObj &rhs) const;

  uint countKeys(const ClParserKey &key) const;

  ClParserValuePtr getMaxValue() const;
  ClParserValuePtr getMinValue() const;

  const ClParserKey &objToKey(const ClParserObj &obj) const;
  const ClParserKey &valueToKey(ClParserValuePtr value) const;
  const ClParserKey &integerToKey(long integer) const;
  const ClParserKey &stringToKey(const std::string &str) const;

  //------

  std::string asString() const;

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

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
  ClParserValuePtr len     () const;

  ClParserValuePtr index (const ClParserObj &obj) const;
  ClParserValuePtr rindex(const ClParserObj &obj) const;

  ClParserValuePtr sort(ClParserSortDirection direction) const;

  ClParserValuePtr doAssert() const;

  //------

  ClParserValuePtr subscriptValue(const ClParserValuePtr *subscripts, uint num_subscripts) const;

  ClParserValuePtr getValue(ClParserValuePtr ind) const;
  ClParserValuePtr getValue(const ClParserKey &key) const;

  ClParserValuePtr getKeyValue(int ind) const;

 private:
  void addValues(const ClParserDict &list);
};

#endif
