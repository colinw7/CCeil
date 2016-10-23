#ifndef CCEILP_STRUCT_H
#define CCEILP_STRUCT_H

#include <CCeilPObj.h>
#include <CCeilPValue.h>

class ClParserStruct : public ClParserObj {
 public:
  typedef std::map<std::string,ClParserValuePtr> ValueMap;

 private:
  ClParserTypePtr type_;
  ValueMap        values_;

 public:
  struct BinaryFunc {
    virtual ~BinaryFunc() { }

    ClParserValuePtr process(const ClParserStruct &lstructure, const ClParserObj &robj) const;

    virtual void exec(ClParserValuePtr lhs, ClParserValuePtr rhs) const = 0;
  };

 public:
  static ClParserStructPtr createStruct();
  static ClParserStructPtr createStruct(ClParserTypePtr type);
  static ClParserStructPtr createStruct(ClParserTypePtr type, const ClParserValuePtr *values,
                                        uint num_values);
  static ClParserStructPtr createStruct(ClParserTypePtr type, const ClParserValueArray &values);
  static ClParserStructPtr createStruct(const ClParserStruct &structure);

 protected:
  friend class CRefPtr<ClParserStruct>;
  friend class ClParserValue;

  ClParserStruct();

  explicit ClParserStruct(ClParserTypePtr type);

  ClParserStruct(ClParserTypePtr type, const ClParserValueArray &values);
  ClParserStruct(ClParserTypePtr type, const ClParserValuePtr *values, uint num_values);

  ClParserStruct(const ClParserStruct &structure);

 ~ClParserStruct();

  const ClParserStruct &operator=(const ClParserStruct &structure);

  ClParserStruct *dup() const;

  ClParserStructPtr dupStruct() const;

 public:
  void copy(const ClParserObj &obj);

  //--------

  ClParserTypePtr getType() const { return type_; }

  bool getIsFixedType() const {
    return (type_.isValid() ? type_->getIsFixed() : false);
  }

  uint getNumValues() const { return values_.size(); }

  ValueMap::const_iterator getValuesBegin() const;
  ValueMap::const_iterator getValuesEnd  () const;

  void addName(const std::string &name);

  bool getValue(const std::string &name, ClParserValuePtr &value) const;
  bool setValue(const std::string &name, ClParserValuePtr value);

  bool toBool() const;

  int cmp(const ClParserObj &obj) const;

  void print() const;
  void print(std::ostream &os) const;
  void debugPrint() const;

  //--------

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

 private:
  template<class T>
  ClParserValuePtr binaryOp(const ClParserObj &obj) const {
    T func;

    return func.process(*this, obj);
  }
};

/**** Private Routines ****/

extern ClParserValuePtr ClParserGetStructureVariableRefValue
                          (ClParserVarRefPtr var_ref, const std::string &name, int *error_code);

#endif
