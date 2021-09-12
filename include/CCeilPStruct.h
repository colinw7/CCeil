#ifndef CCEILP_STRUCT_H
#define CCEILP_STRUCT_H

#include <CCeilPObj.h>
#include <CCeilPValue.h>

class ClParserStruct : public ClParserObj {
 public:
  typedef std::map<std::string,ClParserValuePtr> ValueMap;

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

 public:
  static const ClParserStruct &castObj(const ClParserObj &obj) {
    assert(obj.getBaseType() == CL_PARSER_VALUE_TYPE_STRUCTURE);

    return reinterpret_cast<const ClParserStruct &>(obj);
  }

 protected:
  friend class CRefPtr<ClParserStruct>;
  friend class ClParserValue;

  ClParserStruct();

  explicit ClParserStruct(ClParserTypePtr type);

  ClParserStruct(ClParserTypePtr type, const ClParserValueArray &values);
  ClParserStruct(ClParserTypePtr type, const ClParserValuePtr *values, uint num_values);

  ClParserStruct(const ClParserStruct &structure);

 ~ClParserStruct();

  ClParserStruct &operator=(const ClParserStruct &structure);

  ClParserStruct *dup() const override;

  ClParserStructPtr dupStruct() const;

 public:
  void copy(const ClParserObj &obj) override;

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

  bool toBool() const override;

  int cmp(const ClParserObj &obj) const override;

  //------

  std::string asString() const override;

  void print() const override;
  void print(std::ostream &os) const override;

  void debugPrint() const override;

  //--------

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

  ClParserValuePtr index (const ClParserObj &obj) const override;
  ClParserValuePtr rindex(const ClParserObj &obj) const override;

  ClParserValuePtr sort(ClParserSortDirection direction) const override;

  ClParserValuePtr doAssert() const override;

 private:
  template<class T>
  ClParserValuePtr binaryOp(const ClParserObj &obj) const {
    T func;

    return func.process(*this, obj);
  }

 private:
  ClParserTypePtr type_;
  ValueMap        values_;
};

/**** Private Routines ****/

extern ClParserValuePtr ClParserGetStructureVariableRefValue
                          (ClParserVarRefPtr var_ref, const std::string &name, int *error_code);

#endif
